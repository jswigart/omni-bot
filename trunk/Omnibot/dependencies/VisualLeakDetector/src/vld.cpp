////////////////////////////////////////////////////////////////////////////////
//
//  Visual Leak Detector - VisualLeakDetector Class Implementation
//  Copyright (c) 2005-2012 VLD Team
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//  See COPYING.txt for the full terms of the GNU Lesser General Public License.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma comment(lib, "dbghelp.lib")

#include <sys/stat.h>

#define VLDBUILD         // Declares that we are building Visual Leak Detector.
#include "callstack.h"   // Provides a class for handling call stacks.
#include "crtmfcpatch.h" // Provides CRT and MFC patch functions.
#include "map.h"         // Provides a lightweight STL-like map template.
#include "ntapi.h"       // Provides access to NT APIs.
#include "set.h"         // Provides a lightweight STL-like set template.
#include "utility.h"     // Provides various utility functions.
#include "vldheap.h"     // Provides internal new and delete operators.
#include "vldint.h"      // Provides access to the Visual Leak Detector internals.

#define BLOCK_MAP_RESERVE   64  // This should strike a balance between memory use and a desire to minimize heap hits.
#define HEAP_MAP_RESERVE    2   // Usually there won't be more than a few heaps in the process, so this should be small.
#define MODULE_SET_RESERVE  16  // There are likely to be several modules loaded in the process.

// Imported global variables.
extern vldblockheader_t *g_vldBlockList;
extern HANDLE            g_vldHeap;
extern CriticalSection   g_vldHeapLock;

// Global variables.
HANDLE           g_currentProcess; // Pseudo-handle for the current process.
HANDLE           g_currentThread;  // Pseudo-handle for the current thread.
CriticalSection  g_imageLock;      // Serializes calls to the Debug Help Library PE image access APIs.
HANDLE           g_processHeap;    // Handle to the process's heap (COM allocations come from here).
CriticalSection  g_stackWalkLock;  // Serializes calls to StackWalk64 from the Debug Help Library.
CriticalSection  g_symbolLock;     // Serializes calls to the Debug Help Library symbols handling APIs.
ReportHookSet*   g_pReportHooks;
volatile long    g_loaderLockCounter = 0; 

// The one and only VisualLeakDetector object instance.
__declspec(dllexport) VisualLeakDetector g_vld;

// Patch only this entries in Kernel32.dll and KernelBase.dll
patchentry_t ldrLoadDllPatch [] = {
    "LdrLoadDll",   NULL,    VisualLeakDetector::_LdrLoadDll,
    NULL,           NULL,    NULL
}; 
moduleentry_t ntdllPatch [] = {
    "ntdll.dll",    NULL,   ldrLoadDllPatch,
};

BOOL IsWin7OrBetter()
{
    OSVERSIONINFOEX info = { sizeof(OSVERSIONINFOEX) };
    GetVersionEx((LPOSVERSIONINFO)&info);
    if (info.dwMajorVersion > 6)
        return TRUE;

    if (info.dwMajorVersion == 6 && info.dwMinorVersion >= 1)
        return TRUE;

    return FALSE;
}

// Constructor - Initializes private data, loads configuration options, and
//   attaches Visual Leak Detector to all other modules loaded into the current
//   process.
//
VisualLeakDetector::VisualLeakDetector ()
{
    _set_error_mode(_OUT_TO_STDERR);

    // Initialize configuration options and related private data.
    _wcsnset_s(m_forcedModuleList, MAXMODULELISTLENGTH, '\0', _TRUNCATE);
    m_maxDataDump    = 0xffffffff;
    m_maxTraceFrames = 0xffffffff;
    m_options        = 0x0;
    m_reportFile     = NULL;
    wcsncpy_s(m_reportFilePath, MAX_PATH, VLD_DEFAULT_REPORT_FILE_NAME, _TRUNCATE);
    m_status         = 0x0;

    // Load configuration options.
    configure();
    if (m_options & VLD_OPT_VLDOFF) {
        Report(L"Visual Leak Detector is turned off.\n");
        return;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE kernelBase = GetModuleHandleW(L"KernelBase.dll");

    if (!IsWin7OrBetter()) // kernel32.dll
    {
        if (kernel32)
            m_original_GetProcAddress = (_GetProcAddressType *) GetProcAddress(kernel32,"GetProcAddress");
    }
    else
    {
        assert(m_patchTable[0].patchTable == m_kernelbasePatch);
        m_patchTable[0].exportModuleName = "kernelbase.dll";
        if (kernelBase)
            m_original_GetProcAddress = (_GetProcAddressType *) GetProcAddress(kernelBase,"GetProcAddress");
    }

    // Initialize global variables.
    g_currentProcess    = GetCurrentProcess();
    g_currentThread     = GetCurrentThread();
    g_imageLock.Initialize();
    g_processHeap       = GetProcessHeap();
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll)
    {
        LdrLoadDll        = (LdrLoadDll_t)GetProcAddress(ntdll, "LdrLoadDll");
        RtlAllocateHeap   = (RtlAllocateHeap_t)GetProcAddress(ntdll, "RtlAllocateHeap");
        RtlFreeHeap       = (RtlFreeHeap_t)GetProcAddress(ntdll, "RtlFreeHeap");
        RtlReAllocateHeap = (RtlReAllocateHeap_t)GetProcAddress(ntdll, "RtlReAllocateHeap");
    }
    g_stackWalkLock.Initialize();
    g_symbolLock.Initialize();
    g_vldHeap           = HeapCreate(0x0, 0, 0);
    g_vldHeapLock.Initialize();
    g_pReportHooks    = new ReportHookSet;

    // Initialize remaining private data.
    m_heapMap         = new HeapMap;
    m_heapMap->reserve(HEAP_MAP_RESERVE);
    m_iMalloc         = NULL;
    m_requestCurr     = 1;
    m_totalAlloc      = 0;
    m_curAlloc        = 0;
    m_maxAlloc        = 0;
    m_loadedModules   = NULL;
    m_optionsLock.Initialize();
    m_loaderLock.Initialize();
    m_heapMapLock.Initialize();
    m_modulesLock.Initialize();
    m_selfTestFile    = __FILE__;
    m_selfTestLine    = 0;
    m_tlsIndex        = TlsAlloc();
    m_tlsLock.Initialize();
    m_tlsMap          = new TlsMap;

    if (m_options & VLD_OPT_SELF_TEST) {
        // Self-test mode has been enabled. Intentionally leak a small amount of
        // memory so that memory leak self-checking can be verified.
        if (m_options & VLD_OPT_UNICODE_REPORT) {
            wcsncpy_s(new WCHAR [wcslen(SELFTESTTEXTW) + 1], wcslen(SELFTESTTEXTW) + 1, SELFTESTTEXTW, _TRUNCATE);
            m_selfTestLine = __LINE__ - 1;
        }
        else {
            strncpy_s(new CHAR [strlen(SELFTESTTEXTA) + 1], strlen(SELFTESTTEXTA) + 1, SELFTESTTEXTA, _TRUNCATE);
            m_selfTestLine = __LINE__ - 1;
        }
    }
    if (m_options & VLD_OPT_START_DISABLED) {
        // Memory leak detection will initially be disabled.
        m_status |= VLD_STATUS_NEVER_ENABLED;
    }
    if (m_options & VLD_OPT_REPORT_TO_FILE) {
        setupReporting();
    }
    if (m_options & VLD_OPT_SLOW_DEBUGGER_DUMP) {
        // Insert a slight delay between messages sent to the debugger for
        // output. (For working around a bug in VC6 where data sent to the
        // debugger gets lost if it's sent too fast).
        InsertReportDelay();
    }

    // This is highly unlikely to happen, but just in case, check to be sure
    // we got a valid TLS index.
    if (m_tlsIndex == TLS_OUT_OF_INDEXES) {
        Report(L"ERROR: Visual Leak Detector could not be installed because thread local"
            L"  storage could not be allocated.");
        return;
    }

    // Initialize the symbol handler. We use it for obtaining source file/line
    // number information and function names for the memory leak report.
    LPWSTR symbolpath = buildSymbolSearchPath();
#ifdef NOISY_DBGHELP_DIAGOSTICS
    // From MSDN docs about SYMOPT_DEBUG:
    /* To view all attempts to load symbols, call SymSetOptions with SYMOPT_DEBUG. 
    This causes DbgHelp to call the OutputDebugString function with detailed 
    information on symbol searches, such as the directories it is searching and and error messages.
    In other words, this will really pollute the debug output window with extra messages.
    To enable this debug output to be displayed to the console without changing your source code, 
    set the DBGHELP_DBGOUT environment variable to a non-NULL value before calling the SymInitialize function. 
    To log the information to a file, set the DBGHELP_LOG environment variable to the name of the log file to be used.
    */
    SymSetOptions(SYMOPT_DEBUG | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
#else
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
#endif
    DbgTrace(L"dbghelp32.dll %i: SymInitializeW\n", GetCurrentThreadId());
    if (!SymInitializeW(g_currentProcess, symbolpath, FALSE)) {
        Report(L"WARNING: Visual Leak Detector: The symbol handler failed to initialize (error=%lu).\n"
            L"    File and function names will probably not be available in call stacks.\n", GetLastError());
    }
    delete [] symbolpath;

    ntdllPatch->moduleBase = (UINT_PTR)ntdll;
    PatchImport(kernel32, ntdllPatch);
    if (kernelBase != NULL)
        PatchImport(kernelBase, ntdllPatch);

    // Attach Visual Leak Detector to every module loaded in the process.
    ModuleSet* newmodules = new ModuleSet();
    newmodules->reserve(MODULE_SET_RESERVE);
    DbgTrace(L"dbghelp32.dll %i: EnumerateLoadedModulesW64\n", GetCurrentThreadId());
    EnumerateLoadedModulesW64(g_currentProcess, addLoadedModule, newmodules);
    attachToLoadedModules(newmodules);
    m_loadedModules = newmodules;
    m_status |= VLD_STATUS_INSTALLED;

    HMODULE dbghelp = GetModuleHandleW(L"dbghelp.dll");
    if (dbghelp)
        ChangeModuleState(dbghelp, false);

    Report(L"Visual Leak Detector Version " VLDVERSION L" installed.\n");
    if (m_status & VLD_STATUS_FORCE_REPORT_TO_FILE) {
        // The report is being forced to a file. Let the human know why.
        Report(L"NOTE: Visual Leak Detector: Unicode-encoded reporting has been enabled, but the\n"
            L"  debugger is the only selected report destination. The debugger cannot display\n"
            L"  Unicode characters, so the report will also be sent to a file. If no file has\n"
            L"  been specified, the default file name is \"" VLD_DEFAULT_REPORT_FILE_NAME L"\".\n");

    }
    reportConfig();
}

bool VisualLeakDetector::waitForAllVLDThreads()
{
    bool threadsactive = false;
    DWORD dwCurProcessID = GetCurrentProcessId();
    int waitcount = 0;

    // See if any threads that have ever entered VLD's code are still active.
    CriticalSectionLocker cs(m_tlsLock);
    for (TlsMap::Iterator tlsit = m_tlsMap->begin(); tlsit != m_tlsMap->end(); ++tlsit) {
        if ((*tlsit).second->threadId == GetCurrentThreadId()) {
            // Don't wait for the current thread to exit.
            continue;
        }

        HANDLE thread = OpenThread(SYNCHRONIZE | THREAD_QUERY_INFORMATION, FALSE, (*tlsit).second->threadId);
        if (thread == NULL) {
            // Couldn't query this thread. We'll assume that it exited.
            continue; // XXX should we check GetLastError()?
        }
        if (GetProcessIdOfThread(thread) != dwCurProcessID) {
            //The thread ID has been recycled.
            CloseHandle(thread);
            continue;
        }
        if (WaitForSingleObject(thread, 10000) == WAIT_TIMEOUT) { // 10 seconds
            // There is still at least one other thread running. The CRT
            // will stomp it dead when it cleans up, which is not a
            // graceful way for a thread to go down. Warn about this,
            // and wait until the thread has exited so that we know it
            // can't still be off running somewhere in VLD's code.
            // 
            // Since we've been waiting a while, let the human know we are
            // still here and alive.
            waitcount++;
            threadsactive = true;
            if (waitcount >= 9) // 90 sec.
            {
                CloseHandle(thread);
                return threadsactive;
            }
            Report(L"Visual Leak Detector: Waiting for threads to terminate...\n");
        }
        CloseHandle(thread);
    }
    return threadsactive;
}

void VisualLeakDetector::checkInternalMemoryLeaks() 
{
    const char* leakfile = NULL;
    size_t count;
    WCHAR leakfilew [MAX_PATH];
    int leakline = 0;

    // Do a memory leak self-check.
    SIZE_T  internalleaks = 0;
    vldblockheader_t *header = g_vldBlockList;
    while (header) {
        // Doh! VLD still has an internally allocated block!
        // This won't ever actually happen, right guys?... guys?
        internalleaks++;
        leakfile = header->file;
        leakline = header->line;
        mbstowcs_s(&count, leakfilew, MAX_PATH, leakfile, _TRUNCATE);
        Report(L"ERROR: Visual Leak Detector: Detected a memory leak internal to Visual Leak Detector!!\n");
        Report(L"---------- Block %Iu at " ADDRESSFORMAT L": %u bytes ----------\n", header->serialNumber, VLDBLOCKDATA(header), header->size);
        Report(L"  Call Stack:\n");
        Report(L"    %s (%d): Full call stack not available.\n", leakfilew, leakline);
        if (m_maxDataDump != 0) {
            Report(L"  Data:\n");
            if (m_options & VLD_OPT_UNICODE_REPORT) {
                DumpMemoryW(VLDBLOCKDATA(header), (m_maxDataDump < header->size) ? m_maxDataDump : header->size);
            }
            else {
                DumpMemoryA(VLDBLOCKDATA(header), (m_maxDataDump < header->size) ? m_maxDataDump : header->size);
            }
        }
        Report(L"\n");
        header = header->next;
    }
    if (m_options & VLD_OPT_SELF_TEST) {
        if ((internalleaks == 1) && (strcmp(leakfile, m_selfTestFile) == 0) && (leakline == m_selfTestLine)) {
            Report(L"Visual Leak Detector passed the memory leak self-test.\n");
        }
        else {
            Report(L"ERROR: Visual Leak Detector: Failed the memory leak self-test.\n");
        }
    }
}

// Destructor - Detaches Visual Leak Detector from all modules loaded in the
//   process, frees internally allocated resources, and generates the memory
//   leak report.
//
VisualLeakDetector::~VisualLeakDetector ()
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    if (m_status & VLD_STATUS_INSTALLED) {
        // Detach Visual Leak Detector from all previously attached modules.
        DbgTrace(L"dbghelp32.dll %i: EnumerateLoadedModulesW64\n", GetCurrentThreadId());
        EnumerateLoadedModulesW64(g_currentProcess, detachFromModule, NULL);

        HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
        HMODULE kernelBase = GetModuleHandleW(L"KernelBase.dll");
        RestoreImport(kernel32, ntdllPatch);
        if (kernelBase != NULL)
            RestoreImport(kernelBase, ntdllPatch);

        BOOL threadsactive = waitForAllVLDThreads();

        if (m_status & VLD_STATUS_NEVER_ENABLED) {
            // Visual Leak Detector started with leak detection disabled and
            // it was never enabled at runtime. A lot of good that does.
            Report(L"WARNING: Visual Leak Detector: Memory leak detection was never enabled.\n");
        }
        else {
            // Generate a memory leak report for each heap in the process.
            SIZE_T leaks_count = ReportLeaks();

            // Show a summary.
            if (leaks_count == 0) {
                Report(L"No memory leaks detected.\n");
            }
            else {
                Report(L"Visual Leak Detector detected %Iu memory leak", leaks_count);
                Report((leaks_count > 1) ? L"s (%Iu bytes).\n" : L" (%Iu bytes).\n", m_curAlloc);
                Report(L"Largest number used: %Iu bytes.\n", m_maxAlloc);
                Report(L"Total allocations: %Iu bytes.\n", m_totalAlloc);
            }
        }

        // Free resources used by the symbol handler.
        DbgTrace(L"dbghelp32.dll %i: SymCleanup\n", GetCurrentThreadId());
        if (!SymCleanup(g_currentProcess)) {
            Report(L"WARNING: Visual Leak Detector: The symbol handler failed to deallocate resources (error=%lu).\n",
                GetLastError());
        }

        // Free internally allocated resources used by the heapmap and blockmap.
        for (HeapMap::Iterator heapit = m_heapMap->begin(); heapit != m_heapMap->end(); ++heapit) {
            BlockMap *blockmap = &(*heapit).second->blockMap;
            for (BlockMap::Iterator blockit = blockmap->begin(); blockit != blockmap->end(); ++blockit) {
                delete (*blockit).second->callStack;
                delete (*blockit).second;
            }
            delete blockmap;
        }
        delete m_heapMap;

        // Free internally allocated resources used by the loaded module set.
        for (ModuleSet::Iterator moduleit = m_loadedModules->begin(); moduleit != m_loadedModules->end(); ++moduleit) {
            delete [] (*moduleit).name;
            delete [] (*moduleit).path;
        }
        delete m_loadedModules;

        // Free internally allocated resources used for thread local storage.
        for (TlsMap::Iterator tlsit = m_tlsMap->begin(); tlsit != m_tlsMap->end(); ++tlsit) {
            delete (*tlsit).second;
        }
        delete m_tlsMap;

        if (threadsactive == TRUE) {
            Report(L"WARNING: Visual Leak Detector: Some threads appear to have not terminated normally.\n"
                L"  This could cause inaccurate leak detection results, including false positives.\n");
        }
        Report(L"Visual Leak Detector is now exiting.\n");

        delete g_pReportHooks;
        g_pReportHooks = NULL;

        checkInternalMemoryLeaks();
    }
    else {
        // VLD failed to load properly.
        delete m_heapMap;
        delete m_tlsMap;
        delete g_pReportHooks;
        g_pReportHooks = NULL;
    }
    HeapDestroy(g_vldHeap);

    m_optionsLock.Delete();
    m_loaderLock.Delete();
    m_heapMapLock.Delete();
    m_modulesLock.Delete();
    m_tlsLock.Delete();
    g_imageLock.Delete();
    g_stackWalkLock.Delete();
    g_symbolLock.Delete();
    g_vldHeapLock.Delete();

    if (m_tlsIndex != TLS_OUT_OF_INDEXES) {
        TlsFree(m_tlsIndex);
    }

    if (m_reportFile != NULL) {
        fclose(m_reportFile);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
// Private Leak Detection Functions
//
////////////////////////////////////////////////////////////////////////////////

// attachtoloadedmodules - Attaches VLD to all modules contained in the provided
//   ModuleSet. Not all modules are in the ModuleSet will actually be included
//   in leak detection. Only modules that import the global VisualLeakDetector
//   class object, or those that are otherwise explicitly included in leak
//   detection, will be checked for memory leaks.
//
//   When VLD attaches to a module, it means that any of the imports listed in
//   the import patch table which are imported by the module, will be redirected
//   to VLD's designated replacements.
//
//  - newmodules (IN): Pointer to a ModuleSet containing information about any
//      loaded modules that need to be attached.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::attachToLoadedModules (ModuleSet *newmodules)
{
    ModuleSet::Muterator  updateit;

    // Iterate through the supplied set, until all modules have been attached.
    for (ModuleSet::Iterator newit = newmodules->begin(); newit != newmodules->end(); ++newit) {
        DWORD64 modulebase  = (DWORD64)(*newit).addrLow;

        bool moduleLoaded = false;
        UINT32 moduleflags = 0x0;
        m_modulesLock.Enter();
        ModuleSet* oldmodules = m_loadedModules;
        if (oldmodules != NULL) {
            // This is not the first time we have been called to attach to the
            // currently loaded modules.
            ModuleSet::Iterator oldit = oldmodules->find(*newit);
            if (oldit != oldmodules->end()) {
                moduleLoaded = true;
                moduleflags = (*oldit).flags;
            }
        }
        m_modulesLock.Leave();

        bool refresh = false;
        if (moduleLoaded) // We've seen this "new" module loaded in the process before.
        {
            if (IsModulePatched((HMODULE)modulebase, m_patchTable, _countof(m_patchTable))) 
            {
                // This module is already attached. Just update the module's
                // flags, nothing more.
                updateit = newit;
                (*updateit).flags = moduleflags;
                continue;
            }
            else {
                // This module may have been attached before and has been
                // detached. We'll need to try reattaching to it in case it
                // was unloaded and then subsequently reloaded.
                refresh = true;
            }
        }

        LPCWSTR modulename  = (*newit).name;
        LPCWSTR modulepath  = (*newit).path;
        DWORD modulesize  = (DWORD)((*newit).addrHigh - (*newit).addrLow) + 1;

        g_symbolLock.Enter();
        if ((refresh == true) && (moduleflags & VLD_MODULE_SYMBOLSLOADED)) {
            // Discard the previously loaded symbols, so we can refresh them.
            DbgTrace(L"dbghelp32.dll %i: SymUnloadModule64\n", GetCurrentThreadId());
            if (SymUnloadModule64(g_currentProcess, modulebase) == false) {
                Report(L"WARNING: Visual Leak Detector: Failed to unload the symbols for %s. Function names and line"
                    L" numbers shown in the memory leak report for %s may be inaccurate.\n", modulename, modulename);
            }
        }

        // Try to load the module's symbols. This ensures that we have loaded
        // the symbols for every module that has ever been loaded into the
        // process, guaranteeing the symbols' availability when generating the
        // leak report.
        IMAGEHLP_MODULE64     moduleimageinfo;
        moduleimageinfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
        BOOL SymbolsLoaded = SymGetModuleInfoW64(g_currentProcess, modulebase, &moduleimageinfo);
        g_symbolLock.Leave();

        if (!SymbolsLoaded)
        {
            CriticalSectionLocker cs(g_vld.m_heapMapLock); // fix GetModuleFileName thread lock
            g_symbolLock.Enter();
            DbgTrace(L"dbghelp32.dll %i: SymLoadModuleEx\n", GetCurrentThreadId());
            DWORD64 module = SymLoadModuleEx(g_currentProcess, NULL, modulepath, NULL, modulebase, modulesize, NULL, 0);
            if (module == modulebase)
            {
                DbgTrace(L"dbghelp32.dll %i: SymGetModuleInfoW64\n", GetCurrentThreadId());
                SymbolsLoaded = SymGetModuleInfoW64(g_currentProcess, modulebase, &moduleimageinfo);
            }
            g_symbolLock.Leave();
        }
        if (SymbolsLoaded)
            moduleflags |= VLD_MODULE_SYMBOLSLOADED;

        if (_wcsicmp(TEXT(VLDDLL), modulename) == 0) {
            // What happens when a module goes through it's own portal? Bad things.
            // Like infinite recursion. And ugly bald men wearing dresses. VLD
            // should not, therefore, attach to itself.
            continue;
        }

        if (!FindImport((HMODULE)modulebase, m_vldBase, VLDDLL, "?g_vld@@3VVisualLeakDetector@@A"))
        {
            // This module does not import VLD. This means that none of the module's
            // sources #included vld.h.
            if ((m_options & VLD_OPT_MODULE_LIST_INCLUDE) != 0)
            {
                if (wcsstr(m_forcedModuleList, modulename) == NULL) {
                    // Exclude this module from leak detection.
                    moduleflags |= VLD_MODULE_EXCLUDED;
                }
            }
            else
            {
                if (wcsstr(m_forcedModuleList, modulename) != NULL) {
                    // Exclude this module from leak detection.
                    moduleflags |= VLD_MODULE_EXCLUDED;
                }
            }
        }
        if ((moduleflags & VLD_MODULE_EXCLUDED) == 0 && 
            !(moduleflags & VLD_MODULE_SYMBOLSLOADED) || (moduleimageinfo.SymType == SymExport)) {
            // This module is going to be included in leak detection, but complete
            // symbols for this module couldn't be loaded. This means that any stack
            // traces through this module may lack information, like line numbers
            // and function names.
            Report(L"WARNING: Visual Leak Detector: A module, %s, included in memory leak detection\n"
                L"  does not have any debugging symbols available, or they could not be located.\n"
                L"  Function names and/or line numbers for this module may not be available.\n", modulename);
        }

        // Update the module's flags in the "new modules" set.
        updateit = newit;
        (*updateit).flags = moduleflags;

        // Attach to the module.
        PatchModule((HMODULE)modulebase, m_patchTable, _countof(m_patchTable));
    }
}

// buildsymbolsearchpath - Builds the symbol search path for the symbol handler.
//   This helps the symbol handler find the symbols for the application being
//   debugged.
//
//  Return Value:
//
//    Returns a string containing the search path. The caller is responsible for
//    freeing the string.
//
LPWSTR VisualLeakDetector::buildSymbolSearchPath ()
{
    // Oddly, the symbol handler ignores the link to the PDB embedded in the
    // executable image. So, we'll manually add the location of the executable
    // to the search path since that is often where the PDB will be located.
    WCHAR   directory [_MAX_DIR] = {0};
    WCHAR   drive [_MAX_DRIVE] = {0};
    LPWSTR  path = new WCHAR [MAX_PATH];
    path[0] = L'\0';

    HMODULE module = GetModuleHandleW(NULL);
    GetModuleFileName(module, path, MAX_PATH);
    _wsplitpath_s(path, drive, _MAX_DRIVE, directory, _MAX_DIR, NULL, 0, NULL, 0);
    wcsncpy_s(path, MAX_PATH, drive, _TRUNCATE);
    path = AppendString(path, directory);

    // When the symbol handler is given a custom symbol search path, it will no
    // longer search the default directories (working directory, system root,
    // etc). But we'd like it to still search those directories, so we'll add
    // them to our custom search path.
    //
    // Append the working directory.
    path = AppendString(path, L";.\\");

    // Append the Windows directory.
    WCHAR   windows [MAX_PATH] = {0};
    if (GetWindowsDirectory(windows, MAX_PATH) != 0) {
        path = AppendString(path, L";");
        path = AppendString(path, windows);
    }

    // Append the system directory.
    WCHAR   system [MAX_PATH] = {0};
    if (GetSystemDirectory(system, MAX_PATH) != 0) {
        path = AppendString(path, L";");
        path = AppendString(path, system);
    }

    // Append %_NT_SYMBOL_PATH%.
    DWORD   envlen = GetEnvironmentVariable(L"_NT_SYMBOL_PATH", NULL, 0);
    if (envlen != 0) {
        LPWSTR env = new WCHAR [envlen];
        if (GetEnvironmentVariable(L"_NT_SYMBOL_PATH", env, envlen) != 0) {
            path = AppendString(path, L";");
            path = AppendString(path, env);
        }
        delete [] env;
    }

    // Append %_NT_ALT_SYMBOL_PATH%.
    envlen = GetEnvironmentVariable(L"_NT_ALT_SYMBOL_PATH", NULL, 0);
    if (envlen != 0) {
        LPWSTR env = new WCHAR [envlen];
        if (GetEnvironmentVariable(L"_NT_ALT_SYMBOL_PATH", env, envlen) != 0) {
            path = AppendString(path, L";");
            path = AppendString(path, env);
        }
        delete [] env;
    }

    // Append Visual Studio 2010/2008 symbols cache directory.
    HKEY debuggerkey;
    WCHAR symbolCacheDir [MAX_PATH] = {0};
    LSTATUS regstatus = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\VisualStudio\\10.0\\Debugger", 0, KEY_QUERY_VALUE, &debuggerkey);
    if (regstatus != ERROR_SUCCESS) 
        regstatus = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\VisualStudio\\9.0\\Debugger", 0, KEY_QUERY_VALUE, &debuggerkey);

    if (regstatus == ERROR_SUCCESS)
    {
        DWORD valuetype;
        DWORD dirLength = MAX_PATH * sizeof(WCHAR);
        regstatus = RegQueryValueEx(debuggerkey, L"SymbolCacheDir", NULL, &valuetype, (LPBYTE)&symbolCacheDir, &dirLength);
        if (regstatus == ERROR_SUCCESS && valuetype == REG_SZ)
        {
            path = AppendString(path, L";srv*");
            path = AppendString(path, symbolCacheDir);
            path = AppendString(path, L"\\MicrosoftPublicSymbols;srv*");
            path = AppendString(path, symbolCacheDir);
        }
        RegCloseKey(debuggerkey);
    }

    // Remove any quotes from the path. The symbol handler doesn't like them.
    SIZE_T  pos = 0;
    SIZE_T  length = wcslen(path);
    while (pos < length) {
        if (path[pos] == L'\"') {
            for (SIZE_T  index = pos; index < length; index++) {
                path[index] = path[index + 1];
            }
        }
        pos++;
    }

    return path;
}

// configure - Configures VLD using values read from the vld.ini file.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::configure ()
{
    WCHAR  inipath [MAX_PATH] = {0};
    struct _stat s;
    if (_wstat(L".\\vld.ini", &s) == 0) {
        // Found a copy of vld.ini in the working directory. Use it.
        wcsncpy_s(inipath, MAX_PATH, L".\\vld.ini", _TRUNCATE);
    }
    else {
        BOOL         keyopen = FALSE;
        HKEY         productkey = 0;
        DWORD        length = 0;
        DWORD        valuetype = 0;

        // Get the location of the vld.ini file from the registry.
        LONG regstatus = RegOpenKeyEx(HKEY_CURRENT_USER, VLDREGKEYPRODUCT, 0, KEY_QUERY_VALUE, &productkey);
        if (regstatus == ERROR_SUCCESS) {
            keyopen = TRUE;
            length = MAX_PATH * sizeof(WCHAR);
            regstatus = RegQueryValueExW(productkey, L"IniFile", NULL, &valuetype, (LPBYTE)&inipath, &length);
        }
        if (keyopen) {
            RegCloseKey(productkey);
        }

        if (!keyopen)
        {
            // Get the location of the vld.ini file from the registry.
            regstatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, VLDREGKEYPRODUCT, 0, KEY_QUERY_VALUE, &productkey);
            if (regstatus == ERROR_SUCCESS) {
                keyopen = TRUE;
                length = MAX_PATH * sizeof(WCHAR);
                regstatus = RegQueryValueEx(productkey, L"IniFile", NULL, &valuetype, (LPBYTE)&inipath, &length);
            }
            if (keyopen) {
                RegCloseKey(productkey);
            }
        }

        if ((regstatus != ERROR_SUCCESS) || (_wstat(inipath, &s) != 0)) {
            // The location of vld.ini could not be read from the registry. As a
            // last resort, look in the Windows directory.
            wcsncpy_s(inipath, MAX_PATH, L"vld.ini", _TRUNCATE);
        }
    }
    DbgReport(L"Visual Leak Detector read settings from file: %s\n", inipath);

#define BSIZE 64
    WCHAR        buffer [BSIZE] = {0};
    // Read the boolean options.
    GetPrivateProfileString(L"Options", L"VLD", L"on", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == FALSE) {
        m_options |= VLD_OPT_VLDOFF;
        return;
    }

    GetPrivateProfileString(L"Options", L"AggregateDuplicates", L"", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == TRUE) {
        m_options |= VLD_OPT_AGGREGATE_DUPLICATES;
    }

    GetPrivateProfileString(L"Options", L"SelfTest", L"", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == TRUE) {
        m_options |= VLD_OPT_SELF_TEST;
    }

    GetPrivateProfileString(L"Options", L"SlowDebuggerDump", L"", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == TRUE) {
        m_options |= VLD_OPT_SLOW_DEBUGGER_DUMP;
    }

    GetPrivateProfileString(L"Options", L"StartDisabled", L"", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == TRUE) {
        m_options |= VLD_OPT_START_DISABLED;
    }

    GetPrivateProfileString(L"Options", L"TraceInternalFrames", L"", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == TRUE) {
        m_options |= VLD_OPT_TRACE_INTERNAL_FRAMES;
    }

    GetPrivateProfileString(L"Options", L"SkipHeapFreeLeaks", L"", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == TRUE) {
        m_options |= VLD_OPT_SKIP_HEAPFREE_LEAKS;
    }

    // Read the integer configuration options.
    m_maxDataDump = GetPrivateProfileInt(L"Options", L"MaxDataDump", VLD_DEFAULT_MAX_DATA_DUMP, inipath);
    m_maxTraceFrames = GetPrivateProfileInt(L"Options", L"MaxTraceFrames", VLD_DEFAULT_MAX_TRACE_FRAMES, inipath);
    if (m_maxTraceFrames < 1) {
        m_maxTraceFrames = VLD_DEFAULT_MAX_TRACE_FRAMES;
    }

    // Read the force-include module list.
    GetPrivateProfileString(L"Options", L"ForceIncludeModules", L"", m_forcedModuleList, MAXMODULELISTLENGTH, inipath);
    _wcslwr_s(m_forcedModuleList, MAXMODULELISTLENGTH);
    if (wcscmp(m_forcedModuleList, L"*") == 0)
        m_forcedModuleList[0] = '\0';
    else
        m_options |= VLD_OPT_MODULE_LIST_INCLUDE;
    
    // Read the report destination (debugger, file, or both).
    WCHAR filename [MAX_PATH] = {0};
    GetPrivateProfileString(L"Options", L"ReportFile", L"", filename, MAX_PATH, inipath);
    if (filename[0] == '\0') {
        wcsncpy_s(filename, MAX_PATH, VLD_DEFAULT_REPORT_FILE_NAME, _TRUNCATE);
    }
    WCHAR* path = _wfullpath(m_reportFilePath, filename, MAX_PATH);
    assert(path);

    GetPrivateProfileString(L"Options", L"ReportTo", L"", buffer, BSIZE, inipath);
    if (_wcsicmp(buffer, L"both") == 0) {
        m_options |= (VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE);
    }
    else if (_wcsicmp(buffer, L"file") == 0) {
        m_options |= VLD_OPT_REPORT_TO_FILE;
    }
    else if (_wcsicmp(buffer, L"stdout") == 0) {
        m_options |= VLD_OPT_REPORT_TO_STDOUT;
    }
    else {
        m_options |= VLD_OPT_REPORT_TO_DEBUGGER;
    }

    // Read the report file encoding (ascii or unicode).
    GetPrivateProfileString(L"Options", L"ReportEncoding", L"", buffer, BSIZE, inipath);
    if (_wcsicmp(buffer, L"unicode") == 0) {
        m_options |= VLD_OPT_UNICODE_REPORT;
    }
    if ((m_options & VLD_OPT_UNICODE_REPORT) && !(m_options & VLD_OPT_REPORT_TO_FILE)) {
        // If Unicode report encoding is enabled, then the report needs to be
        // sent to a file because the debugger will not display Unicode
        // characters, it will display question marks in their place instead.
        m_options |= VLD_OPT_REPORT_TO_FILE;
        m_status |= VLD_STATUS_FORCE_REPORT_TO_FILE;
    }

    // Read the stack walking method.
    GetPrivateProfileString(L"Options", L"StackWalkMethod", L"", buffer, BSIZE, inipath);
    if (_wcsicmp(buffer, L"safe") == 0) {
        m_options |= VLD_OPT_SAFE_STACK_WALK;
    }

    GetPrivateProfileString(L"Options", L"ValidateHeapAllocs", L"", buffer, BSIZE, inipath);
    if (StrToBool(buffer) == TRUE) {
        m_options |= VLD_OPT_VALIDATE_HEAPFREE;
    }
}

// enabled - Determines if memory leak detection is enabled for the current
//   thread.
//
//  Return Value:
//
//    Returns true if Visual Leak Detector is enabled for the current thread.
//    Otherwise, returns false.
//
BOOL VisualLeakDetector::enabled ()
{
    if (!(m_status & VLD_STATUS_INSTALLED)) {
        // Memory leak detection is not yet enabled because VLD is still
        // initializing.
        return FALSE;
    }

    tls_t* tls = getTls();
    if (!(tls->flags & VLD_TLS_DISABLED) && !(tls->flags & VLD_TLS_ENABLED)) {
        // The enabled/disabled state for the current thread has not been 
        // initialized yet. Use the default state.
        if (m_options & VLD_OPT_START_DISABLED) {
            tls->flags |= VLD_TLS_DISABLED;
        }
        else {
            tls->flags |= VLD_TLS_ENABLED;
        }
    }

    return ((tls->flags & VLD_TLS_ENABLED) != 0);
}

// eraseduplicates - Erases, from the block maps, blocks that appear to be
//   duplicate leaks of an already identified leak.
//
//  - element (IN): BlockMap Iterator referencing the block of which to search
//      for duplicates.
//
//  Return Value:
//
//    Returns the number of duplicate blocks erased from the block map.
//
SIZE_T VisualLeakDetector::eraseDuplicates (const BlockMap::Iterator &element, Set<blockinfo_t*> &aggregatedLeaks)
{
    blockinfo_t *elementinfo = (*element).second;

    if (elementinfo->callStack == NULL)
        return 0;

    SIZE_T       erased = 0;
    // Iterate through all block maps, looking for blocks with the same size
    // and callstack as the specified element.
    for (HeapMap::Iterator heapit = m_heapMap->begin(); heapit != m_heapMap->end(); ++heapit) {
        BlockMap *blockmap = &(*heapit).second->blockMap;
        for (BlockMap::Iterator blockit = blockmap->begin(); blockit != blockmap->end(); ++blockit) {
            if (blockit == element) {
                // Don't delete the element of which we are searching for
                // duplicates.
                continue;
            }
            blockinfo_t *info = (*blockit).second;
            if (info->callStack == NULL)
                continue;
            Set<blockinfo_t*>::Iterator it = aggregatedLeaks.find(info);
            if (it != aggregatedLeaks.end())
                continue;
            if ((info->size == elementinfo->size) && (*(info->callStack) == *(elementinfo->callStack))) {
                // Found a duplicate. Mark it.
                aggregatedLeaks.insert(info);
                erased++;
            }
        }
    }

    return erased;
}

// gettls - Obtains the thread local storage structure for the calling thread.
//
//  Return Value:
//
//    Returns a pointer to the thread local storage structure. (This function
//    always succeeds).
//
tls_t* VisualLeakDetector::getTls ()
{
    // Get the pointer to this thread's thread local storage structure.
    tls_t* tls = (tls_t*)TlsGetValue(m_tlsIndex);
    assert(GetLastError() == ERROR_SUCCESS);

    if (tls == NULL) {
        // This thread's thread local storage structure has not been allocated.
        tls = new tls_t;
        TlsSetValue(m_tlsIndex, tls);
        ZeroMemory(&tls->context, sizeof(tls->context));
        tls->flags = 0x0;
        tls->oldFlags = 0x0;
        tls->threadId = GetCurrentThreadId();
        tls->ppCallStack = NULL;

        // Add this thread's TLS to the TlsSet.
        CriticalSectionLocker cs(m_tlsLock);
        m_tlsMap->insert(tls->threadId,tls);
    }

    return tls;
}

// mapblock - Tracks memory allocations. Information about allocated blocks is
//   collected and then the block is mapped to this information.
//
//  - heap (IN): Handle to the heap from which the block has been allocated.
//
//  - mem (IN): Pointer to the memory block being allocated.
//
//  - size (IN): Size, in bytes, of the memory block being allocated.
//
//  - addrOfRetAddr (IN): Address of return address at the time this allocation
//      first entered VLD's code. This is used from determining the starting
//      point for the stack trace.
//
//  - crtalloc (IN): Should be set to TRUE if this allocation is a CRT memory
//      block. Otherwise should be FALSE.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::mapBlock (HANDLE heap, LPCVOID mem, SIZE_T size, bool crtalloc, CallStack **&ppcallstack)
{
    // Record the block's information.
    blockinfo_t* blockinfo = new blockinfo_t;
    blockinfo->callStack = NULL;
    ppcallstack = &blockinfo->callStack;
    blockinfo->serialNumber = m_requestCurr++;
    blockinfo->size = size;
    blockinfo->reported = false;

    if (SIZE_MAX - m_totalAlloc > size)
        m_totalAlloc += size;
    else
        m_totalAlloc = SIZE_MAX;
    m_curAlloc += size;

    if (m_curAlloc > m_maxAlloc)
        m_maxAlloc = m_curAlloc;

    // Insert the block's information into the block map.
    CriticalSectionLocker cs(m_heapMapLock);
    HeapMap::Iterator heapit = m_heapMap->find(heap);
    if (heapit == m_heapMap->end()) {
        // We haven't mapped this heap to a block map yet. Do it now.
        mapHeap(heap);
        heapit = m_heapMap->find(heap);
        assert(heapit != m_heapMap->end());
    }
    if (crtalloc) {
        // The heap that this block was allocated from is a CRT heap.
        (*heapit).second->flags |= VLD_HEAP_CRT_DBG;
    }
    BlockMap* blockmap = &(*heapit).second->blockMap;
    BlockMap::Iterator blockit = blockmap->insert(mem, blockinfo);
    if (blockit == blockmap->end()) {
        // A block with this address has already been allocated. The
        // previously allocated block must have been freed (probably by some
        // mechanism unknown to VLD), or the heap wouldn't have allocated it
        // again. Replace the previously allocated info with the new info.
        blockit = blockmap->find(mem);
        delete (*blockit).second->callStack;
        delete (*blockit).second;
        blockmap->erase(blockit);
        blockmap->insert(mem, blockinfo);
        ppcallstack = NULL;
    }
}

// mapheap - Tracks heap creation. Creates a block map for tracking individual
//   allocations from the newly created heap and then maps the heap to this
//   block map.
//
//  - heap (IN): Handle to the newly created heap.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::mapHeap (HANDLE heap)
{
    // Create a new block map for this heap and insert it into the heap map.
    heapinfo_t* heapinfo = new heapinfo_t;
    heapinfo->blockMap.reserve(BLOCK_MAP_RESERVE);
    heapinfo->flags = 0x0;
    CriticalSectionLocker cs(m_heapMapLock);
    HeapMap::Iterator heapit = m_heapMap->insert(heap, heapinfo);
    if (heapit == m_heapMap->end()) {
        // Somehow this heap has been created twice without being destroyed,
        // or at least it was destroyed without VLD's knowledge. Unmap the heap
        // from the existing heapinfo, and remap it to the new one.
        Report(L"WARNING: Visual Leak Detector detected a duplicate heap (" ADDRESSFORMAT L").\n", heap);
        heapit = m_heapMap->find(heap);
        unmapHeap((*heapit).first);
        m_heapMap->insert(heap, heapinfo);
    }
}

// unmapblock - Tracks memory blocks that are freed. Unmaps the specified block
//   from the block's information, relinquishing internally allocated resources.
//
//  - heap (IN): Handle to the heap to which this block is being freed.
//
//  - mem (IN): Pointer to the memory block being freed.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::unmapBlock (HANDLE heap, LPCVOID mem, const context_t &context)
{
    if (NULL == mem)
        return;

    // Find this heap's block map.
    CriticalSectionLocker cs(m_heapMapLock);
    HeapMap::Iterator heapit = m_heapMap->find(heap);
    if (heapit == m_heapMap->end()) {
        // We don't have a block map for this heap. We must not have monitored
        // this allocation (probably happened before VLD was initialized).
        return;
    }

    // Find this block in the block map.
    BlockMap           *blockmap = &(*heapit).second->blockMap;
    BlockMap::Iterator  blockit = blockmap->find(mem);
    if (blockit == blockmap->end()) 
    {
        // This memory block is not in the block map. We must not have monitored this
        // allocation (probably happened before VLD was initialized).

        // This can also result from allocating on one heap, and freeing on another heap.
        // This is an especially bad way to corrupt the application.
        // Now we have to search through every heap and every single block in each to make 
        // sure that this is indeed the case.
        if (m_options & VLD_OPT_VALIDATE_HEAPFREE)
        {
            HANDLE other_heap = NULL;
            blockinfo_t* alloc_block = findAllocedBlock(mem, other_heap); // other_heap is an out parameter
            bool diff = other_heap != heap; // Check indeed if the other heap is different
            if (alloc_block && alloc_block->callStack && diff)
            {
                Report(L"CRITICAL ERROR!: VLD reports that memory was allocated in one heap and freed in another.\nThis will result in a corrupted heap.\nAllocation Call stack.\n");
                Report(L"---------- Block %ld at " ADDRESSFORMAT L": %u bytes ----------\n", alloc_block->serialNumber, mem, alloc_block->size);
                Report(L"  Call Stack:\n");
                alloc_block->callStack->dump(m_options & VLD_OPT_TRACE_INTERNAL_FRAMES);

                // Now we need a way to print the current callstack at this point:
                CallStack* stack_here = CallStack::Create();
                stack_here->getStackTrace(m_maxTraceFrames, context);
                Report(L"Deallocation Call stack.\n");
                Report(L"---------- Block %ld at " ADDRESSFORMAT L": %u bytes ----------\n", alloc_block->serialNumber, mem, alloc_block->size);
                Report(L"  Call Stack:\n");
                stack_here->dump(FALSE);
                // Now it should be safe to delete our temporary callstack
                delete stack_here;
                stack_here = NULL;
                if (IsDebuggerPresent())
                    DebugBreak();
            }
        }
        return;
    }

    // Free the blockinfo_t structure and erase it from the block map.
    blockinfo_t *info = (*blockit).second;
    m_curAlloc -= info->size;
    delete info->callStack;
    delete info;
    blockmap->erase(blockit);
}

// unmapheap - Tracks heap destruction. Unmaps the specified heap from its block
//   map. The block map is cleared and deleted, relinquishing internally
//   allocated resources.
//
//  - heap (IN): Handle to the heap which is being destroyed.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::unmapHeap (HANDLE heap)
{
    // Find this heap's block map.
    CriticalSectionLocker cs(m_heapMapLock);
    HeapMap::Iterator heapit = m_heapMap->find(heap);
    if (heapit == m_heapMap->end()) {
        // This heap hasn't been mapped. We must not have monitored this heap's
        // creation (probably happened before VLD was initialized).
        return;
    }

    // Free all of the blockinfo_t structures stored in the block map.
    heapinfo_t *heapinfo = (*heapit).second;
    BlockMap   *blockmap = &heapinfo->blockMap;
    for (BlockMap::Iterator blockit = blockmap->begin(); blockit != blockmap->end(); ++blockit) {
        delete (*blockit).second->callStack;
        delete (*blockit).second;
    }
    delete heapinfo;

    // Remove this heap's block map from the heap map.
    m_heapMap->erase(heapit);
}

// remapblock - Tracks reallocations. Unmaps a block from its previously
//   collected information and remaps it to updated information.
//
//  Note: If the block itself remains at the same address, then the block's
//   information can simply be updated rather than having to actually erase and
//   reinsert the block.
//
//  - heap (IN): Handle to the heap from which the memory is being reallocated.
//
//  - mem (IN): Pointer to the memory block being reallocated.
//
//  - newmem (IN): Pointer to the memory block being returned to the caller
//      that requested the reallocation. This pointer may or may not be the same
//      as the original memory block (as pointed to by "mem").
//
//  - size (IN): Size, in bytes, of the new memory block.
//
//  - framepointer (IN): The frame pointer at which this reallocation entered
//      VLD's code. Used for determining the starting point of the stack trace.
//
//  - crtalloc (IN): Should be set to TRUE if this reallocation is for a CRT
//      memory block. Otherwise should be set to FALSE.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::remapBlock (HANDLE heap, LPCVOID mem, LPCVOID newmem, SIZE_T size,
    bool crtalloc, CallStack **&ppcallstack, const context_t &context)
{
    if (newmem != mem) {
        // The block was not reallocated in-place. Instead the old block was
        // freed and a new block allocated to satisfy the new size.
        unmapBlock(heap, mem, context);
        mapBlock(heap, newmem, size, crtalloc, ppcallstack);
        return;
    }

    // The block was reallocated in-place. Find the existing blockinfo_t
    // entry in the block map and update it with the new callstack and size.
    CriticalSectionLocker cs(m_heapMapLock);
    HeapMap::Iterator heapit = m_heapMap->find(heap);
    if (heapit == m_heapMap->end()) {
        // We haven't mapped this heap to a block map yet. Obviously the
        // block has also not been mapped to a blockinfo_t entry yet either,
        // so treat this reallocation as a brand-new allocation (this will
        // also map the heap to a new block map).
        mapBlock(heap, newmem, size, crtalloc, ppcallstack);
        return;
    }

    // Find the block's blockinfo_t structure so that we can update it.
    BlockMap           *blockmap = &(*heapit).second->blockMap;
    BlockMap::Iterator  blockit = blockmap->find(mem);
    if (blockit == blockmap->end()) {
        // The block hasn't been mapped to a blockinfo_t entry yet.
        // Treat this reallocation as a new allocation.
        mapBlock(heap, newmem, size, crtalloc, ppcallstack);
        return;
    }

    // Found the blockinfo_t entry for this block. Update it with
    // a new callstack and new size.
    blockinfo_t* info = (*blockit).second;
    if (info->callStack)
    {
        delete info->callStack;
        info->callStack = NULL;
    }

    if (crtalloc) {
        // The heap that this block was allocated from is a CRT heap.
        (*heapit).second->flags |= VLD_HEAP_CRT_DBG;
    }

    if (m_totalAlloc < SIZE_MAX)
    {
        m_totalAlloc -= info->size;
        if (SIZE_MAX - m_totalAlloc > size)
            m_totalAlloc += size;
        else
            m_totalAlloc = SIZE_MAX;
    }

    m_curAlloc -= info->size;
    m_curAlloc += size;

    if (m_curAlloc > m_maxAlloc)
        m_maxAlloc = m_curAlloc;

    // Update the block's size.
    info->size = size;
    ppcallstack = &info->callStack;
}

// reportconfig - Generates a brief report summarizing Visual Leak Detector's
//   configuration, as loaded from the vld.ini file.
//
//  Return Value:
//
//    None.
//
VOID VisualLeakDetector::reportConfig ()
{
    if (m_options & VLD_OPT_AGGREGATE_DUPLICATES) {
        Report(L"    Aggregating duplicate leaks.\n");
    }
    if (m_forcedModuleList[0] != '\0') {
        Report(L"    Forcing %s of these modules in leak detection: %s\n", 
            (m_options & VLD_OPT_MODULE_LIST_INCLUDE) ? L"inclusion" : L"exclusion", m_forcedModuleList);
    }
    if (m_maxDataDump != VLD_DEFAULT_MAX_DATA_DUMP) {
        if (m_maxDataDump == 0) {
            Report(L"    Suppressing data dumps.\n");
        }
        else {
            Report(L"    Limiting data dumps to %Iu bytes.\n", m_maxDataDump);
        }
    }
    if (m_maxTraceFrames != VLD_DEFAULT_MAX_TRACE_FRAMES) {
        Report(L"    Limiting stack traces to %u frames.\n", m_maxTraceFrames);
    }
    if (m_options & VLD_OPT_UNICODE_REPORT) {
        Report(L"    Generating a Unicode (UTF-16) encoded report.\n");
    }
    if (m_options & VLD_OPT_REPORT_TO_FILE) {
        if (m_options & VLD_OPT_REPORT_TO_DEBUGGER) {
            Report(L"    Outputting the report to the debugger and to %s\n", m_reportFilePath);
        }
        else {
            Report(L"    Outputting the report to %s\n", m_reportFilePath);
        }
    }
    if (m_options & VLD_OPT_SLOW_DEBUGGER_DUMP) {
        Report(L"    Outputting the report to the debugger at a slower rate.\n");
    }
    if (m_options & VLD_OPT_SAFE_STACK_WALK) {
        Report(L"    Using the \"safe\" (but slow) stack walking method.\n");
    }
    if (m_options & VLD_OPT_SELF_TEST) {
        Report(L"    Performing a memory leak self-test.\n");
    }
    if (m_options & VLD_OPT_START_DISABLED) {
        Report(L"    Starting with memory leak detection disabled.\n");
    }
    if (m_options & VLD_OPT_TRACE_INTERNAL_FRAMES) {
        Report(L"    Including heap and VLD internal frames in stack traces.\n");
    }
}

// getleakscount - Calculate number of memory leaks.
//
//  - heap (IN): Handle to the heap for which to generate a memory leak
//      report.
//
//  Return Value:
//
//    None.
//
SIZE_T VisualLeakDetector::getLeaksCount (heapinfo_t* heapinfo)
{
    BlockMap* blockmap   = &heapinfo->blockMap;
    SIZE_T memoryleaks = 0;

    for (BlockMap::Iterator blockit = blockmap->begin(); blockit != blockmap->end(); ++blockit)
    {
        // Found a block which is still in the BlockMap. We've identified a
        // potential memory leak.
        LPCVOID block = (*blockit).first;
        blockinfo_t* info = (*blockit).second;
        if (info->reported)
            continue;

        if (heapinfo->flags & VLD_HEAP_CRT_DBG) {
            // This block is allocated to a CRT heap, so the block has a CRT
            // memory block header pretended to it.
            crtdbgblockheader_t* crtheader = (crtdbgblockheader_t*)block;
            if (CRT_USE_TYPE(crtheader->use) == CRT_USE_IGNORE ||
                CRT_USE_TYPE(crtheader->use) == CRT_USE_FREE ||
                CRT_USE_TYPE(crtheader->use) == CRT_USE_INTERNAL) {
                // This block is marked as being used internally by the CRT.
                // The CRT will free the block after VLD is destroyed.
                continue;
            }
        }

        memoryleaks ++;
    }

    return memoryleaks;
}

// reportleaks - Generates a memory leak report for the specified heap.
//
//  - heap (IN): Handle to the heap for which to generate a memory leak
//      report.
//
//  Return Value:
//
//    None.
//
SIZE_T VisualLeakDetector::reportHeapLeaks (HANDLE heap)
{
    assert(heap != NULL);

    // Find the heap's information (blockmap, etc).
    CriticalSectionLocker cs(m_heapMapLock);
    HeapMap::Iterator heapit = m_heapMap->find(heap);
    if (heapit == m_heapMap->end()) {
        // Nothing is allocated from this heap. No leaks.
        return 0;
    }

    Set<blockinfo_t*> aggregatedLeaks;
    heapinfo_t* heapinfo = (*heapit).second;
    // Generate a memory leak report for heap.
    SIZE_T leaks_count = reportLeaks(heapinfo, aggregatedLeaks);

    // Show a summary.
    if (leaks_count != 0) {
        Report(L"Visual Leak Detector detected %Iu memory leak%s in heap " ADDRESSFORMAT L"\n", 
            leaks_count, (leaks_count > 1) ? L"s" : L"", heap);
     }
    return leaks_count;
}

SIZE_T VisualLeakDetector::reportLeaks (heapinfo_t* heapinfo, Set<blockinfo_t*> &aggregatedLeaks)
{
    BlockMap* blockmap   = &heapinfo->blockMap;
    SIZE_T leaksFound = 0;
    bool firstLeak = true;

    for (BlockMap::Iterator blockit = blockmap->begin(); blockit != blockmap->end(); ++blockit)
    {
        // Found a block which is still in the BlockMap. We've identified a
        // potential memory leak.
        LPCVOID block = (*blockit).first;
        blockinfo_t* info = (*blockit).second;
        if (info->reported)
            continue;

        Set<blockinfo_t*>::Iterator it = aggregatedLeaks.find(info);
        if (it != aggregatedLeaks.end())
            continue;

        LPCVOID address = block;
        SIZE_T size = info->size;

        if (heapinfo->flags & VLD_HEAP_CRT_DBG) {
            // This block is allocated to a CRT heap, so the block has a CRT
            // memory block header pretended to it.
            crtdbgblockheader_t* crtheader = (crtdbgblockheader_t*)block;
            if (CRT_USE_TYPE(crtheader->use) == CRT_USE_IGNORE ||
                CRT_USE_TYPE(crtheader->use) == CRT_USE_FREE ||
                CRT_USE_TYPE(crtheader->use) == CRT_USE_INTERNAL)
            {
                // This block is marked as being used internally by the CRT.
                // The CRT will free the block after VLD is destroyed.
                continue;
            }
            // The CRT header is more or less transparent to the user, so
            // the information about the contained block will probably be
            // more useful to the user. Accordingly, that's the information
            // we'll include in the report.
            address = CRTDBGBLOCKDATA(block);
            size = crtheader->size;
        }

        // It looks like a real memory leak.
        if (firstLeak) { // A confusing way to only display this message once
            Report(L"WARNING: Visual Leak Detector detected memory leaks!\n");
            firstLeak = false;
        }
        leaksFound++;
        Report(L"---------- Block %ld at " ADDRESSFORMAT L": %u bytes ----------\n", info->serialNumber, address, size);
        assert(info->callStack);
        if (m_options & VLD_OPT_AGGREGATE_DUPLICATES) {
            // Aggregate all other leaks which are duplicates of this one
            // under this same heading, to cut down on clutter.
            SIZE_T erased = eraseDuplicates(blockit, aggregatedLeaks);

            // add only the number that were erased, since the 'one left over'
            // is already recorded as a leak
            leaksFound += erased;

            DWORD callstackCRC = 0;
            if (info->callStack)
                callstackCRC = CalculateCRC32(info->size, info->callStack->getHashValue());
            Report(L"Leak Hash: 0x%08X Count: %Iu\n", callstackCRC, erased + 1);
        }
        // Dump the call stack.
        Report(L"  Call Stack:\n");
        if (info->callStack)
            info->callStack->dump(m_options & VLD_OPT_TRACE_INTERNAL_FRAMES);

        // Dump the data in the user data section of the memory block.
        if (m_maxDataDump != 0) {
            Report(L"  Data:\n");
            if (m_options & VLD_OPT_UNICODE_REPORT) {
                DumpMemoryW(address, (m_maxDataDump < size) ? m_maxDataDump : size);
            }
            else {
                DumpMemoryA(address, (m_maxDataDump < size) ? m_maxDataDump : size);
            }
        }
        Report(L"\n\n");
    }

    return leaksFound;
}

VOID VisualLeakDetector::markAllLeaksAsReported (heapinfo_t* heapinfo)
{
    BlockMap* blockmap   = &heapinfo->blockMap;

    for (BlockMap::Iterator blockit = blockmap->begin(); blockit != blockmap->end(); ++blockit)
    {
        blockinfo_t* info = (*blockit).second;
        info->reported = true;
    }
}

// FindAllocedBlock - Find if a particular memory allocation is tracked inside of VLD.
//     This is a really good example of how to iterate through the data structures
//     that represent heaps and their associated memory blocks.
// Pre Condition: Be VERY sure that this is only called within a block that already has
// acquired a critical section for m_maplock. 
//
// mem - The particular memory address to search for.
// 
//  Return Value:
//   If mem is found, it will return the blockinfo_t pointer, otherwise NULL
// 
blockinfo_t* VisualLeakDetector::findAllocedBlock(LPCVOID mem, __out HANDLE& heap)
{
    blockinfo_t* result = NULL;
    // Iterate through all heaps
    for (HeapMap::Iterator it = m_heapMap->begin();
        it != m_heapMap->end();
        ++it)
    {
        HANDLE heap_handle  = (*it).first;
        (heap_handle); // unused
        heapinfo_t* heapPtr = (*it).second;

        // Iterate through all memory blocks in each heap
        BlockMap& p_block_map = heapPtr->blockMap;
        for (BlockMap::Iterator iter = p_block_map.begin();
            iter != p_block_map.end();
            ++iter)
        {
            if ((*iter).first == mem)
            {
                // Found the block.
                blockinfo_t* alloc_block = (*iter).second;
                heap = heap_handle;
                result = alloc_block;
                break;
            }
        }

        if (result)
        {
            break;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
//
// Static Leak Detection Functions (Callbacks)
//
////////////////////////////////////////////////////////////////////////////////

// addloadedmodule - Callback function for EnumerateLoadedModules64. This
//   function records information about every module loaded in the process,
//   each time adding the module's information to the provided ModuleSet (the
//   "context" parameter).
//
//   When EnumerateLoadedModules64 has finished calling this function for each
//   loaded module, then the resulting ModuleSet can be used at any time to get
//   information about any modules loaded into the process.
//
//   - modulepath (IN): The fully qualified path from where the module was
//       loaded.
//
//   - modulebase (IN): The base address at which the module has been loaded.
//
//   - modulesize (IN): The size, in bytes, of the loaded module.
//
//   - context (IN): Pointer to the ModuleSet to which information about each
//       module is to be added.
//
//  Return Value:
//
//    Always returns TRUE, which tells EnumerateLoadedModules64 to continue
//    enumerating.
//
BOOL VisualLeakDetector::addLoadedModule (PCWSTR modulepath, DWORD64 modulebase, ULONG modulesize, PVOID context)
{
    size_t length = wcslen(modulepath) + 1;
    LPWSTR modulepathw = new WCHAR [length];
    wcsncpy_s(modulepathw, length, modulepath, _TRUNCATE);

    // Extract just the filename and extension from the module path.
    WCHAR filename [_MAX_FNAME];
    WCHAR extension [_MAX_EXT];
    _wsplitpath_s(modulepathw, NULL, 0, NULL, 0, filename, _MAX_FNAME, extension, _MAX_EXT);

    length = wcslen(filename) + wcslen(extension) + 1;
    LPWSTR modulename = new WCHAR [length];
    wcsncpy_s(modulename, length, filename, _TRUNCATE);
    wcsncat_s(modulename, length, extension, _TRUNCATE);
    _wcslwr_s(modulename, length);

    if (_wcsicmp(modulename, TEXT(VLDDLL)) == 0) {
        // Record Visual Leak Detector's own base address.
        g_vld.m_vldBase = (HMODULE)modulebase;
    }
    else {
        // Convert the module path to ASCII.
        length = ::WideCharToMultiByte(CP_ACP, 0, modulename, -1, 0, 0, 0, 0);
        LPSTR modulenamea = new CHAR [length];

        // wcstombs_s requires locale to be already set up correctly, but it might not be correct on vld init step. So use WideCharToMultiByte instead
        CHAR defaultChar     = '?';
        BOOL defaultCharUsed = FALSE;

        int count = ::WideCharToMultiByte(CP_ACP, 0/*flags*/, modulename, (int)-1, modulenamea, (int)length, &defaultChar, &defaultCharUsed);
        assert(count != 0);
        if ( defaultCharUsed )
        {
            ::OutputDebugStringW(__FILEW__ L": " __FUNCTIONW__ L" - defaultChar was used while conversion from \"");
            ::OutputDebugStringW(modulename);
            ::OutputDebugStringW(L"\" to ANSI \"");
            ::OutputDebugStringA(modulenamea);
            ::OutputDebugStringW(L"\". Result can be wrong.\n");
        }

        // See if this is a module listed in the patch table. If it is, update
        // the corresponding patch table entries' module base address.
        UINT          tablesize = _countof(m_patchTable);
        for (UINT index = 0; index < tablesize; index++) {
            moduleentry_t *entry = &m_patchTable[index];
            if (_stricmp(entry->exportModuleName, modulenamea) == 0) {
                entry->moduleBase = (UINT_PTR)modulebase;
            }
        }

        delete [] modulenamea;
    }

    // Record the module's information and store it in the set.
    moduleinfo_t  moduleinfo;
    moduleinfo.addrLow  = (UINT_PTR)modulebase;
    moduleinfo.addrHigh = (UINT_PTR)(modulebase + modulesize) - 1;
    moduleinfo.flags    = 0x0;
    moduleinfo.name     = modulename;
    moduleinfo.path     = modulepathw;

    ModuleSet*    newmodules = (ModuleSet*)context;
    newmodules->insert(moduleinfo);

    return TRUE;
}

// detachfrommodule - Callback function for EnumerateLoadedModules64 that
//   detaches Visual Leak Detector from the specified module. If the specified
//   module has not previously been attached to, then calling this function will
//   not actually result in any changes.
//
//  - modulepath (IN): String containing the name, which may include a path, of
//      the module to detach from (ignored).
//
//  - modulebase (IN): Base address of the module.
//
//  - modulesize (IN): Total size of the module (ignored).
//
//  - context (IN): User-supplied context (ignored).
//
//  Return Value:
//
//    Always returns TRUE.
//
BOOL VisualLeakDetector::detachFromModule (PCWSTR /*modulepath*/, DWORD64 modulebase, ULONG /*modulesize*/,
    PVOID /*context*/)
{
    UINT tablesize = _countof(m_patchTable);

    RestoreModule((HMODULE)modulebase, m_patchTable, tablesize);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
// Win32 IAT Replacement Functions
//
////////////////////////////////////////////////////////////////////////////////

// _GetProcAddress - Calls to GetProcAddress are patched through to this
//   function. If the requested function is a function that has been patched
//   through to one of VLD's handlers, then the address of VLD's handler
//   function is returned instead of the real address. Otherwise, this 
//   function is just a wrapper around the real GetProcAddress.
//
//  - module (IN): Handle (base address) of the module from which to retrieve
//      the address of an exported function.
//
//  - procname (IN): ANSI string containing the name of the exported function
//      whose address is to be retrieved.
//
//  Return Value:
//
//    Returns a pointer to the requested function, or VLD's replacement for
//    the function, if there is a replacement function.
//
FARPROC VisualLeakDetector::_GetProcAddress (HMODULE module, LPCSTR procname)
{
    // See if there is an entry in the patch table that matches the requested
    // function.
    UINT tablesize = _countof(g_vld.m_patchTable);
    for (UINT index = 0; index < tablesize; index++) {
        moduleentry_t *entry = &g_vld.m_patchTable[index];
        if ((entry->moduleBase == 0x0) || ((HMODULE)entry->moduleBase != module)) {
            // This patch table entry is for a different module.
            continue;
        }

        patchentry_t *patchentry = entry->patchTable;
        while(patchentry->importName)
        {
            // This patch table entry is for the specified module. If the requested
            // imports name matches the entry's import name (or ordinal), then
            // return the address of the replacement instead of the address of the
            // actual import.
            if ((SIZE_T)patchentry->importName < (SIZE_T)g_vld.m_vldBase) {
                // This entry's import name is not a valid pointer to data in
                // vld.dll. It must be an ordinal value.
                if ((UINT_PTR)patchentry->importName == (UINT_PTR)procname) {
                    if (patchentry->original != NULL)
                        *patchentry->original = g_vld._RGetProcAddress(module, procname);
                    return (FARPROC)patchentry->replacement;
                }
            }
            else {
                __try
                {
                    if (strcmp(patchentry->importName, procname) == 0) {
                        if (patchentry->original != NULL)
                            *patchentry->original = g_vld._RGetProcAddress(module, procname);
                        return (FARPROC)patchentry->replacement;
                    }
                }
                __except(FilterFunction(GetExceptionCode()))
                {
                    if ((UINT_PTR)patchentry->importName == (UINT_PTR)procname) {
                        if (patchentry->original != NULL)
                            *patchentry->original = g_vld._RGetProcAddress(module, procname);
                        return (FARPROC)patchentry->replacement;
                    }                	
                }
            }
            patchentry++;
        }
    }

    // The requested function is not a patched function. Just return the real
    // address of the requested function.
    return g_vld._RGetProcAddress(module, procname);
}

FARPROC VisualLeakDetector::_RGetProcAddress (HMODULE module, LPCSTR procname)
{
    return m_original_GetProcAddress(module, procname);
}

// _LdrLoadDll - Calls to LdrLoadDll are patched through to this function. This
//   function invokes the real LdrLoadDll and then re-attaches VLD to all
//   modules loaded in the process after loading of the new DLL is complete.
//   All modules must be re-enumerated because the explicit load of the
//   specified module may result in the implicit load of one or more additional
//   modules which are dependencies of the specified module.
//
//  - searchpath (IN): The path to use for searching for the specified module to
//      be loaded.
//
//  - flags (IN): Pointer to action flags.
//
//  - modulename (IN): Pointer to a unicodestring_t structure specifying the
//      name of the module to be loaded.
//
//  - modulehandle (OUT): Address of a HANDLE to receive the newly loaded
//      module's handle.
//
//  Return Value:
//
//    Returns the value returned by LdrLoadDll.
//
NTSTATUS VisualLeakDetector::_LdrLoadDll (LPWSTR searchpath, ULONG flags, unicodestring_t *modulename,
    PHANDLE modulehandle)
{
    // Load the DLL.
    _InterlockedIncrement(&g_loaderLockCounter);
    NTSTATUS status = LdrLoadDll(searchpath, flags, modulename, modulehandle);

    if (STATUS_SUCCESS == status && g_loaderLockCounter == 1)
        g_vld.RefreshModules();
    _InterlockedDecrement(&g_loaderLockCounter);

    return status;
}

VOID VisualLeakDetector::RefreshModules()
{
    if (m_options & VLD_OPT_VLDOFF)
        return;

    CriticalSectionLocker cs(m_loaderLock);
    // Duplicate code here in this method. Consider refactoring out to another method.
    // Create a new set of all loaded modules, including any newly loaded
    // modules.
    ModuleSet* newmodules = new ModuleSet();
    newmodules->reserve(MODULE_SET_RESERVE);
    DbgTrace(L"dbghelp32.dll %i: EnumerateLoadedModulesW64\n", GetCurrentThreadId());
    EnumerateLoadedModulesW64(g_currentProcess, addLoadedModule, newmodules);

    // Attach to all modules included in the set.
    attachToLoadedModules(newmodules);

    // Start using the new set of loaded modules.
    m_modulesLock.Enter();
    ModuleSet* oldmodules = m_loadedModules;
    m_loadedModules = newmodules;
    m_modulesLock.Leave();

    // Free resources used by the old module list.
    for (ModuleSet::Iterator moduleit = oldmodules->begin(); moduleit != oldmodules->end(); ++moduleit) {
        delete [] (*moduleit).name;
        delete [] (*moduleit).path;
    }
    delete oldmodules;
}

void VisualLeakDetector::getCallStack( CallStack **&ppcallstack, context_t &context_ )
{
    CallStack* callstack = CallStack::Create();

    // Reset thread local flags and variables, in case any libraries called
    // into while mapping the block allocate some memory.
    context_t context = context_;
    *ppcallstack = callstack;
    context_.fp = 0x0;
    ppcallstack = NULL;

    callstack->getStackTrace(g_vld.m_maxTraceFrames, context);
}

// Find the information for the module that initiated this reallocation.
bool VisualLeakDetector::isModuleExcluded(UINT_PTR address)
{
    moduleinfo_t         moduleinfo;
    ModuleSet::Iterator  moduleit;
    moduleinfo.addrLow  = address;
    moduleinfo.addrHigh = address + 1024;
    moduleinfo.flags = 0;

    CriticalSectionLocker cs(g_vld.m_modulesLock);
    moduleit = g_vld.m_loadedModules->find(moduleinfo);
    if (moduleit != g_vld.m_loadedModules->end())
        return (*moduleit).flags & VLD_MODULE_EXCLUDED ? true : false;
    else if (g_loaderLockCounter != 0) // new module inside LdrLoadDll
        return true;
    return false;
}

SIZE_T VisualLeakDetector::GetLeaksCount()
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return 0;
    }

    SIZE_T leaksCount = 0;
    // Generate a memory leak report for each heap in the process.
    CriticalSectionLocker cs(m_heapMapLock);
    for (HeapMap::Iterator heapit = m_heapMap->begin(); heapit != m_heapMap->end(); ++heapit) {
        HANDLE heap = (*heapit).first;
        UNREFERENCED_PARAMETER(heap);
        heapinfo_t* heapinfo = (*heapit).second;
        leaksCount += getLeaksCount(heapinfo);
    }
    return leaksCount;
}

SIZE_T VisualLeakDetector::ReportLeaks( ) 
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return 0;
    }

    // Generate a memory leak report for each heap in the process.
    SIZE_T leaksCount = 0;
    CriticalSectionLocker cs(m_heapMapLock);
    Set<blockinfo_t*> aggregatedLeaks;
    for (HeapMap::Iterator heapit = m_heapMap->begin(); heapit != m_heapMap->end(); ++heapit) {
        HANDLE heap = (*heapit).first;
        UNREFERENCED_PARAMETER(heap);
        heapinfo_t* heapinfo = (*heapit).second;
        leaksCount += reportLeaks(heapinfo, aggregatedLeaks);
    }
    return leaksCount;
}

VOID VisualLeakDetector::MarkAllLeaksAsReported( ) 
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    // Generate a memory leak report for each heap in the process.
    CriticalSectionLocker cs(m_heapMapLock);
    for (HeapMap::Iterator heapit = m_heapMap->begin(); heapit != m_heapMap->end(); ++heapit) {
        HANDLE heap = (*heapit).first;
        UNREFERENCED_PARAMETER(heap);
        heapinfo_t* heapinfo = (*heapit).second;
        markAllLeaksAsReported(heapinfo);
    }
}

void VisualLeakDetector::ChangeModuleState(HMODULE module, bool on)
{
    ModuleSet::Iterator  moduleit;

    CriticalSectionLocker cs(m_modulesLock);
    moduleit = m_loadedModules->begin();
    while( moduleit != m_loadedModules->end() )
    {			
        if ( (*moduleit).addrLow == (UINT_PTR)module) 
        {
            moduleinfo_t *mod = (moduleinfo_t *)&(*moduleit);
            if ( on )
                mod->flags &= ~VLD_MODULE_EXCLUDED;
            else
                mod->flags |= VLD_MODULE_EXCLUDED;

            break;
        }
        ++moduleit;
    }
}

void VisualLeakDetector::EnableModule(HMODULE module)
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    ChangeModuleState(module,true);
}

void VisualLeakDetector::DisableModule(HMODULE module)
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    ChangeModuleState(module,false);
}

void VisualLeakDetector::DisableLeakDetection ()
{
    tls_t *tls;

    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    // Disable memory leak detection for the current thread. There are two flags
    // because if neither flag is set, it means that we are in the default or
    // "starting" state, which could be either enabled or disabled depending on
    // the configuration.
    tls = getTls();
    tls->oldFlags = tls->flags;
    tls->flags &= ~VLD_TLS_ENABLED;
    tls->flags |= VLD_TLS_DISABLED;
}

void VisualLeakDetector::EnableLeakDetection ()
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    tls_t *tls;

    // Enable memory leak detection for the current thread.
    tls = getTls();
    tls->oldFlags = tls->flags;
    tls->flags &= ~VLD_TLS_DISABLED;
    tls->flags |= VLD_TLS_ENABLED;
    m_status &= ~VLD_STATUS_NEVER_ENABLED;
}

void VisualLeakDetector::RestoreLeakDetectionState ()
{
    tls_t *tls;

    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    // Restore state memory leak detection for the current thread.
    tls = getTls();
    tls->flags &= ~(VLD_TLS_DISABLED | VLD_TLS_ENABLED);
    tls->flags |= tls->oldFlags & (VLD_TLS_DISABLED | VLD_TLS_ENABLED);
}

void VisualLeakDetector::GlobalDisableLeakDetection ()
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    m_optionsLock.Enter();
    m_options |= VLD_OPT_START_DISABLED;
    m_optionsLock.Leave();

    // Disable memory leak detection for all threads.
    CriticalSectionLocker cstls(m_tlsLock);
    TlsMap::Iterator     tlsit;
    for (tlsit = m_tlsMap->begin(); tlsit != m_tlsMap->end(); ++tlsit) {
        (*tlsit).second->oldFlags = (*tlsit).second->flags;
        (*tlsit).second->flags &= ~VLD_TLS_ENABLED;
        (*tlsit).second->flags |= VLD_TLS_DISABLED;
    }
}

void VisualLeakDetector::GlobalEnableLeakDetection ()
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    m_optionsLock.Enter();
    m_options &= ~VLD_OPT_START_DISABLED;
    m_status &= ~VLD_STATUS_NEVER_ENABLED;
    m_optionsLock.Leave();

    // Enable memory leak detection for all threads.
    CriticalSectionLocker cstls(m_tlsLock);
    TlsMap::Iterator     tlsit;
    for (tlsit = m_tlsMap->begin(); tlsit != m_tlsMap->end(); ++tlsit) {
        (*tlsit).second->oldFlags = (*tlsit).second->flags;
        (*tlsit).second->flags &= ~VLD_TLS_DISABLED;
        (*tlsit).second->flags |= VLD_TLS_ENABLED;
    }
}

CONST UINT32 OptionsMask = VLD_OPT_AGGREGATE_DUPLICATES | VLD_OPT_MODULE_LIST_INCLUDE | 
    VLD_OPT_SAFE_STACK_WALK | VLD_OPT_SLOW_DEBUGGER_DUMP | VLD_OPT_START_DISABLED | 
    VLD_OPT_TRACE_INTERNAL_FRAMES | VLD_OPT_SKIP_HEAPFREE_LEAKS | VLD_OPT_VALIDATE_HEAPFREE | 
    VLD_OPT_RELEASE_CRT_RUNTIME;

UINT32 VisualLeakDetector::GetOptions()
{
    CriticalSectionLocker cs(m_optionsLock);
    return m_options & OptionsMask;
}

void VisualLeakDetector::SetOptions(UINT32 option_mask, SIZE_T maxDataDump, UINT32 maxTraceFrames)
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    CriticalSectionLocker cs(m_optionsLock);
    m_options &= ~OptionsMask; // clear used bits
    m_options |= option_mask & OptionsMask;

    m_maxDataDump = maxDataDump;
    m_maxTraceFrames = maxTraceFrames;
    if (m_maxTraceFrames < 1) {
        m_maxTraceFrames = VLD_DEFAULT_MAX_TRACE_FRAMES;
    }

    m_options |= option_mask & VLD_OPT_START_DISABLED;
    if (m_options & VLD_OPT_START_DISABLED)
        GlobalDisableLeakDetection();
}

void VisualLeakDetector::SetModulesList(CONST WCHAR *modules, BOOL includeModules)
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    CriticalSectionLocker cs(m_optionsLock);
    wcsncpy_s(m_forcedModuleList, MAXMODULELISTLENGTH, modules, _TRUNCATE);
    _wcslwr_s(m_forcedModuleList, MAXMODULELISTLENGTH);
    if (includeModules)
        m_options |= VLD_OPT_MODULE_LIST_INCLUDE;
    else
        m_options &= ~VLD_OPT_MODULE_LIST_INCLUDE;
}

bool VisualLeakDetector::GetModulesList(WCHAR *modules, UINT size)
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        modules[0] = '\0';
        return true;
    }

    CriticalSectionLocker cs(m_optionsLock);
    wcsncpy_s(modules, size, m_forcedModuleList, _TRUNCATE);
    return (m_options & VLD_OPT_MODULE_LIST_INCLUDE) > 0;
}

void VisualLeakDetector::GetReportFilename(WCHAR *filename)
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        m_reportFilePath[0] = '\0';
        return;
    }

    CriticalSectionLocker cs(m_optionsLock);
    wcsncpy_s(filename, MAX_PATH, m_reportFilePath, _TRUNCATE);
}

void VisualLeakDetector::SetReportOptions(UINT32 option_mask, CONST WCHAR *filename)
{
    if (m_options & VLD_OPT_VLDOFF) {
        // VLD has been turned off.
        return;
    }

    CriticalSectionLocker cs(m_optionsLock);
    m_options &= ~(VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE | 
        VLD_OPT_REPORT_TO_STDOUT | VLD_OPT_UNICODE_REPORT); // clear used bits

    m_options |= option_mask & VLD_OPT_REPORT_TO_DEBUGGER;
    if ( (option_mask & VLD_OPT_REPORT_TO_FILE) && ( filename != NULL ))
    {
        wcsncpy_s(m_reportFilePath, MAX_PATH, filename, _TRUNCATE);
        m_options |= option_mask & VLD_OPT_REPORT_TO_FILE;
    }
    m_options |= option_mask & VLD_OPT_REPORT_TO_STDOUT;
    m_options |= option_mask & VLD_OPT_UNICODE_REPORT;

    if ((m_options & VLD_OPT_UNICODE_REPORT) && !(m_options & VLD_OPT_REPORT_TO_FILE)) {
        // If Unicode report encoding is enabled, then the report needs to be
        // sent to a file because the debugger will not display Unicode
        // characters, it will display question marks in their place instead.
        m_options |= VLD_OPT_REPORT_TO_FILE;
        m_status |= VLD_STATUS_FORCE_REPORT_TO_FILE;
    }

    if (m_options & VLD_OPT_REPORT_TO_FILE) {
        setupReporting();
    }
    else if ( m_reportFile ) { //Close the previous report file if needed.
        fclose(m_reportFile);
        m_reportFile = NULL;
    }
}

int VisualLeakDetector::SetReportHook(int mode, VLD_REPORT_HOOK pfnNewHook)
{
    if (m_options & VLD_OPT_VLDOFF || pfnNewHook == NULL) {
        // VLD has been turned off.
        return -1;
    }
    CriticalSectionLocker cs(m_optionsLock);
    if (mode == VLD_RPTHOOK_INSTALL)
    {
        ReportHookSet::Iterator it = g_pReportHooks->insert(pfnNewHook);
        return (it != g_pReportHooks->end()) ? 0 : -1;
    } 
    else if (mode == VLD_RPTHOOK_REMOVE)
    {
        g_pReportHooks->erase(pfnNewHook);
        return 0;
    }
    return -1;
}

void VisualLeakDetector::setupReporting()
{
    WCHAR      bom = BOM; // Unicode byte-order mark.

    //Close the previous report file if needed.
    if ( m_reportFile )
        fclose(m_reportFile);

    // Reporting to file enabled.
    if (m_options & VLD_OPT_UNICODE_REPORT) {
        // Unicode data encoding has been enabled. Write the byte-order
        // mark before anything else gets written to the file. Open the
        // file for binary writing.
        if (_wfopen_s(&m_reportFile, m_reportFilePath, L"wb") == EINVAL) {
            // Couldn't open the file.
            m_reportFile = NULL;
        }
        else {
            fwrite(&bom, sizeof(WCHAR), 1, m_reportFile);
            SetReportEncoding(unicode);
        }
    }
    else {
        // Open the file in text mode for ASCII output.
        if (_wfopen_s(&m_reportFile, m_reportFilePath, L"w") == EINVAL) {
            // Couldn't open the file.
            m_reportFile = NULL;
        }
        else {
            SetReportEncoding(ascii);
        }
    }
    if (m_reportFile == NULL) {
        Report(L"WARNING: Visual Leak Detector: Couldn't open report file for writing: %s\n"
            L"  The report will be sent to the debugger instead.\n", m_reportFilePath);
    }
    else {
        // Set the "report" function to write to the file.
        SetReportFile(m_reportFile, m_options & VLD_OPT_REPORT_TO_DEBUGGER, m_options & VLD_OPT_REPORT_TO_STDOUT);
    }
}

void VisualLeakDetector::resolveStacks(heapinfo_t* heapinfo)
{
    BlockMap& blockmap = heapinfo->blockMap;

    for (BlockMap::Iterator blockit = blockmap.begin(); blockit != blockmap.end(); ++blockit) {
        // Found a block which is still in the BlockMap. We've identified a
        // potential memory leak.
        const void* block   = (*blockit).first;
        blockinfo_t* info   = (*blockit).second;
        assert(info);
        if (info == NULL)
        {
            continue;
        }
        // The actual memory address
        const void* address = block;
        assert(address != NULL);

        if (heapinfo->flags & VLD_HEAP_CRT_DBG) {
            // This block is allocated to a CRT heap, so the block has a CRT
            // memory block header prepended to it.
            crtdbgblockheader_t* crtheader = (crtdbgblockheader_t*)block;
            if (!crtheader)
                continue;

            if (CRT_USE_TYPE(crtheader->use) == CRT_USE_IGNORE ||
                CRT_USE_TYPE(crtheader->use) == CRT_USE_FREE ||
                CRT_USE_TYPE(crtheader->use) == CRT_USE_INTERNAL)
            {
                // This block is marked as being used internally by the CRT.
                // The CRT will free the block after VLD is destroyed.
                continue;
            }
        }

        // Dump the call stack.
        if (info->callStack)
        {
            info->callStack->resolve(m_options & VLD_OPT_TRACE_INTERNAL_FRAMES);
        }
    }
}

void VisualLeakDetector::ResolveCallstacks()
{
    if (m_options & VLD_OPT_VLDOFF)
        return;

    // Generate the Callstacks early
    CriticalSectionLocker cs(m_heapMapLock);
    for (HeapMap::Iterator heapiter = m_heapMap->begin(); heapiter != m_heapMap->end(); ++heapiter)
    {
        HANDLE heap = (*heapiter).first;
        UNREFERENCED_PARAMETER(heap);
        heapinfo_t* heapinfo = (*heapiter).second;
        resolveStacks(heapinfo);
    }
}
