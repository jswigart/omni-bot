// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/01/23)

#include <Carbon/Carbon.h>
#include <unistd.h>
#include "Wm5WindowApplication.h"
#include "Wm5AglRendererData.h"
#include "Wm5AglRendererInput.h"
using namespace Wm5;

const int WindowApplication::KEY_ESCAPE = kEscapeCharCode;
const int WindowApplication::KEY_LEFT_ARROW = kLeftArrowCharCode;
const int WindowApplication::KEY_RIGHT_ARROW = kRightArrowCharCode;
const int WindowApplication::KEY_UP_ARROW = kUpArrowCharCode;
const int WindowApplication::KEY_DOWN_ARROW = kDownArrowCharCode;
const int WindowApplication::KEY_HOME = kHomeCharCode;
const int WindowApplication::KEY_END = kEndCharCode;
const int WindowApplication::KEY_PAGE_UP = kPageUpCharCode;
const int WindowApplication::KEY_PAGE_DOWN = kPageDownCharCode;
const int WindowApplication::KEY_INSERT = kHelpCharCode;
const int WindowApplication::KEY_DELETE = kDeleteCharCode;
const int WindowApplication::KEY_BACKSPACE = kBackspaceCharCode;
const int WindowApplication::KEY_TAB = kTabCharCode;
const int WindowApplication::KEY_ENTER = kEnterCharCode;
const int WindowApplication::KEY_RETURN = kReturnCharCode;

// Function key values are actually key codes rather than char codes because
// MacOS passes all function key values as kFunctionKeyCharCode.
const int WindowApplication::KEY_F1 = 122;
const int WindowApplication::KEY_F2 = 120;
const int WindowApplication::KEY_F3 = 99;
const int WindowApplication::KEY_F4 = 118;
const int WindowApplication::KEY_F5 = 96;
const int WindowApplication::KEY_F6 = 97;
const int WindowApplication::KEY_F7 = 98;
const int WindowApplication::KEY_F8 = 100;
const int WindowApplication::KEY_F9 = 101;
const int WindowApplication::KEY_F10 = 109;
const int WindowApplication::KEY_F11 = 103;
const int WindowApplication::KEY_F12 = 111;

const int WindowApplication::KEY_SHIFT = shiftKey;
const int WindowApplication::KEY_CONTROL = controlKey;
const int WindowApplication::KEY_ALT = optionKey;
const int WindowApplication::KEY_COMMAND = cmdKey;

const int WindowApplication::MOUSE_LEFT_BUTTON = kEventMouseButtonPrimary;
const int WindowApplication::MOUSE_MIDDLE_BUTTON = kEventMouseButtonTertiary;
const int WindowApplication::MOUSE_RIGHT_BUTTON = kEventMouseButtonSecondary;
const int WindowApplication::MOUSE_UP = 0;
const int WindowApplication::MOUSE_DOWN = 1;

const int WindowApplication::MODIFIER_CONTROL = controlKey;
const int WindowApplication::MODIFIER_LBUTTON = kEventMouseButtonPrimary;
const int WindowApplication::MODIFIER_MBUTTON = kEventMouseButtonTertiary;
const int WindowApplication::MODIFIER_RBUTTON = kEventMouseButtonSecondary;
const int WindowApplication::MODIFIER_SHIFT   = shiftKey;

//----------------------------------------------------------------------------
// Extra-data system.
//----------------------------------------------------------------------------
// Indices into the extra data storage for some system data.
#define AGLAPP_BACK   0
#define AGLAPP_FONT   AGLAPP_BACK + sizeof(GWorldPtr)
#define AGLAPP_SYSTEM AGLAPP_FONT + sizeof(SInt32)

const int APP_EXTRA_DATA_QUANTITY = 128;
static char gsExtraData[APP_EXTRA_DATA_QUANTITY];
//----------------------------------------------------------------------------
static void SetExtraData (int index, int size, const void* data)
{
    if (0 <= index && index < APP_EXTRA_DATA_QUANTITY - size)
    {
        memcpy(gsExtraData, data, size);
    }
}
//----------------------------------------------------------------------------
static void GetExtraData (int index, int size, void* data)
{
    if (0 <= index && index < APP_EXTRA_DATA_QUANTITY - size)
    {
        memcpy(data, gsExtraData, size);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void WindowApplication::SetMousePosition (int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    Point mouseLoc;
    mouseLoc.h = (short)x;
    mouseLoc.v = (short)y;
    LocalToGlobal(&mouseLoc);
    SetPort(currPort);

    CGPoint point = CGPointMake((float)mouseLoc.h, (float)mouseLoc.v);
    CGPostMouseEvent(point, true, 1, false, 0);
}
//----------------------------------------------------------------------------
void WindowApplication::GetMousePosition (int& x, int& y) const
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    Point mouseLoc;
    GetMouse(&mouseLoc);
    SetPort(currPort);
    
    x = (int)mouseLoc.h;
    y = (int)mouseLoc.v;
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* text) const
{
    GWorldPtr currentWorld;
    GDHandle currentDevice;
    GetGWorld(&currentWorld, &currentDevice);

    GWorldPtr back;
    GetExtraData(AGLAPP_BACK, sizeof(GWorldPtr), &back);
    SetGWorld(back, 0);

    int width = TextWidth(text, 0, strlen(text));

    SetGWorld(currentWorld, currentDevice);
    return width;
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharacterWidth (const char character) const
{
    GWorldPtr currentWorld;
    GDHandle currentDevice;
    GetGWorld(&currentWorld, &currentDevice);

    GWorldPtr back;
    GetExtraData(AGLAPP_BACK, sizeof(GWorldPtr), &back);
    SetGWorld(back, 0);

    int width = CharWidth(character);

    SetGWorld(currentWorld, currentDevice);
    return width;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    GWorldPtr currentWorld;
    GDHandle currentDevice;
    GetGWorld(&currentWorld, &currentDevice);

    GWorldPtr back;
    GetExtraData(AGLAPP_BACK, sizeof(GWorldPtr), &back);
    SetGWorld(back, 0);
    
    FontInfo info;
    GetFontInfo(&info);
    
    SetGWorld(currentWorld, currentDevice);
    return info.ascent + info.descent + info.leading;
}
//----------------------------------------------------------------------------
static OSErr MacApplicationAEGetDescData (const AEDesc* desc,
    DescType* typeCode, void* dataBuffer, ByteCount maximumSize,
    ByteCount* actualSize)
{
    if (typeCode && desc)
    {
        *typeCode = desc->descriptorType;
    }

    ByteCount dataSize = AEGetDescDataSize(desc);
    if (actualSize)
    {
        *actualSize = dataSize;
    }

    OSErr error = noErr;
    if (dataSize > 0 && maximumSize > 0)
    {
        error = AEGetDescData(desc, dataBuffer, maximumSize);
    }

    return error;
}
//----------------------------------------------------------------------------
static OSErr MacApplicationNavGetFile (FSSpecPtr theFileP, Boolean* gotFile)
{   
    NavDialogOptions dialogOptions; 
    OSErr error = NavGetDefaultDialogOptions(&dialogOptions);
    dialogOptions.dialogOptionFlags =
        (kNavDefaultNavDlogOptions | kNavNoTypePopup) &
        ~kNavAllowPreviews & ~kNavAllowMultipleFiles;

    NavReplyRecord theReply;
    NavEventUPP eventProc = 0;
    error = NavGetFile(0, &theReply, &dialogOptions, eventProc, 0, 0, 0,
        (NavCallBackUserData)0);

    if (theReply.validRecord && error == noErr)
    {
        long count;
        error = AECountItems(&theReply.selection ,&count);
        if (error == noErr)
        {
            long index = 1;
            AEDesc resultDesc;
            AEKeyword keyWord;
            error = AEGetNthDesc(&theReply.selection, index, typeFSS,
                &keyWord, &resultDesc);

            if (error == noErr)
            {
                DescType typeCode = typeFSS;
                ByteCount bCount;

                error = MacApplicationAEGetDescData(&resultDesc, &typeCode,
                    theFileP, sizeof(FSSpec), &bCount);
                *gotFile = true;
                error = AEDisposeDesc(&resultDesc);
            }
        }

        error = NavDisposeReply(&theReply);
    }
    else
    {
        *gotFile = false;
    }

    return error;
}
//----------------------------------------------------------------------------
static Boolean MacApplicationOpenFile (FSSpecPtr theFileP)
{
    FSSpec theFile;
    Boolean gotFile = false;
    OSErr error = MacApplicationNavGetFile(&theFile, &gotFile);
    if (error == noErr && gotFile)
    {
        *theFileP = theFile;
    }

    return gotFile;
}
//----------------------------------------------------------------------------
static std::string GetStringPathname (FSSpec& theFile)
{
    FSRef fileRef;
    FSpMakeFSRef(&theFile, &fileRef);

    CFURLRef url = CFURLCreateFromFSRef(0, &fileRef);
    CFStringRef strRef = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    char path[256];
    CFStringGetCString(strRef, path, 256, CFStringGetSystemEncoding());
    CFRelease(strRef);
    CFRelease(url);
    std::string fileName(path);
    return fileName;
}
//----------------------------------------------------------------------------
static char* GetCommandLine ()
{
    OSErr error = noErr;
    SInt32 systemVersion;
    error = Gestalt(gestaltSystemVersion, &systemVersion);
    SetExtraData(AGLAPP_SYSTEM, sizeof(SInt32), &systemVersion);
    if (error != noErr)
    {
        return 0;
    }

    FILE* inFile = fopen("cmdline.txt", "rb");
    if (inFile == 0)
    {
        FSSpec theFile;
        if (MacApplicationOpenFile(&theFile))
        {
            std::string name = GetStringPathname(theFile);
            char* fileName = new1<char>(name.length() + 1);
            strcpy(fileName, name.c_str());
            return fileName;
        }
        return 0;
    }

    // Get size of file.
    unsigned int fileSize = 0;
    while (fgetc(inFile) != EOF)
    {
        ++fileSize;
    }
    
    if (fileSize == 0)
    {
        fclose(inFile);
        return 0;
    }
    
    // Read file.
    fseek(inFile, 0, SEEK_SET);
    char* commandLine = new1<char>(fileSize + 1);
    char* current = commandLine;
    char c;
    while ((c = (char)fgetc(inFile)) != EOF)
    {
        // Cull out non-printable characters.
       *current++ = (isprint(c) ? c : ' ');
            
    }
    *current = 0;
    
    fclose(inFile);
    return commandLine;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowClose (EventHandlerCallRef nextHandler, 
    EventRef evt, void*)
{
    // Allow standard handler to run.
    OSStatus result = CallNextEventHandler(nextHandler, evt);

    // Quit the application.
    QuitApplicationEventLoop();
    
    return result;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowBoundsChange (EventHandlerCallRef, 
    EventRef evt, void*)
{
    UInt32 attributes;
    GetEventParameter(evt, kEventParamAttributes, typeUInt32, 0,
        sizeof(attributes), 0, &attributes);

    Rect rect;
    GetEventParameter(evt, kEventParamCurrentBounds, typeQDRectangle, 0,
        sizeof(rect), 0, &rect);

    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (attributes & kWindowBoundsChangeUserDrag
    ||  attributes & kWindowBoundsChangeOriginChanged)
    {
        // Bounds are changing due to window moving.
        theApp->OnMove(rect.top, rect.left);
    }
    else if (attributes & kWindowBoundsChangeUserResize
    ||  attributes & kWindowBoundsChangeSizeChanged)
    {
        // Bounds are changing due to window resizing.
        theApp->OnResize(rect.right - rect.left, rect.bottom - rect.top);
    }

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowZoomed (EventHandlerCallRef, 
    EventRef evt, void*)
{
    WindowRef window;
    GetEventParameter(evt, kEventParamDirectObject, typeWindowRef, 0,
        sizeof(window), 0, &window);

    Rect rect;
    GetWindowBounds(window, kWindowContentRgn, &rect);

    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    theApp->OnResize(rect.right - rect.left, rect.bottom - rect.top);

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowRedraw (EventHandlerCallRef, EventRef,
    void*)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    theApp->OnDisplay();

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessKeyDown (EventHandlerCallRef, EventRef evt,
    void*)
{
    char charCode;
    GetEventParameter(evt, kEventParamKeyMacCharCodes, typeChar, 0, 
        sizeof(charCode), 0, &charCode);
        
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;
    
    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    Point mouseLoc;
    GetMouse(&mouseLoc);
    SetPort(currPort);
    
    if (charCode == kEscapeCharCode)
    {
        // Quit the application when 'esc' is pressed.
        QuitApplicationEventLoop();
        return eventNotHandledErr;
    }

    if (isalnum(charCode) || isprint(charCode))
    {
        theApp->OnKeyDown(charCode, mouseLoc.h, mouseLoc.v);
    }
    else
    {
       if (charCode == kFunctionKeyCharCode)
       {
           // Function key - get key identity.
           UInt32 keyCode;
           GetEventParameter(evt, kEventParamKeyCode, typeUInt32, 0,
               sizeof(keyCode), 0, &keyCode);
           
           charCode = keyCode & 0x000000FF; 
       }

       // Do not filter for specific keys.  This allows for keys such as tab,
       // delete, enter.
       theApp->OnSpecialKeyDown(charCode, mouseLoc.h, mouseLoc.v);
    }

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessKeyUp (EventHandlerCallRef, EventRef evt, void*)
{
    char charCode;
    GetEventParameter(evt, kEventParamKeyMacCharCodes, typeChar, 0,
        sizeof(charCode), 0, &charCode);

    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;
    
    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    Point mouseLoc;
    GetMouse(&mouseLoc);
    SetPort(currPort);
    
    if (isalnum(charCode) || isprint(charCode))
    {
        theApp->OnKeyUp(charCode, mouseLoc.h, mouseLoc.v);
    }
    else
    {
        if (charCode == kFunctionKeyCharCode)
        {
            // Function key - get key identity.
            UInt32 keyCode;
            GetEventParameter(evt, kEventParamKeyCode, typeUInt32, 0,
                sizeof(keyCode), 0, &keyCode);
            
            charCode = keyCode & 0x000000FF; 
        }
        
        // Do not filter for specific keys.  This allows for keys such as tab,
        // delete, enter.
        theApp->OnSpecialKeyUp(charCode, mouseLoc.h, mouseLoc.v);
    }

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static void ProcessModifier (WindowApplication* theApp, UInt32 key,
    Point mouseLoc, UInt32 currState, UInt32 lastState)
{
    if ((key & currState) && !(key & lastState))
    {
        // Modifier key is pressed.
        theApp->OnSpecialKeyDown((int)key, mouseLoc.h, mouseLoc.v);
    }
    if (!(key & currState) && (key & lastState))
    {
        // Modifier key is released.
        theApp->OnSpecialKeyUp((int)key, mouseLoc.h, mouseLoc.v);
    }
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessKeyModifierChanged (EventHandlerCallRef,
    EventRef evt, void*)
{
    static UInt32 sLastState = 0;

    UInt32 currState;
    GetEventParameter(evt, kEventParamKeyModifiers, typeUInt32, 0,
        sizeof(currState), 0, &currState);
    currState &= 0x0000FFFF;

    if (currState != sLastState)
    {
        WindowApplication* theApp =
            (WindowApplication*)Application::TheApplication;

        CGrafPtr currPort;
        GetPort(&currPort);
        SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
        Point mouseLoc;
        GetMouse(&mouseLoc);
        SetPort(currPort);

        ProcessModifier(theApp, shiftKey, mouseLoc, currState, sLastState);
        ProcessModifier(theApp, controlKey, mouseLoc, currState, sLastState);
        ProcessModifier(theApp, optionKey, mouseLoc, currState, sLastState);
        ProcessModifier(theApp, cmdKey, mouseLoc, currState, sLastState);

        sLastState = currState;
    }

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseDown (EventHandlerCallRef, EventRef evt,
    void*)
{
    Point mouseLoc;    
    GetEventParameter(evt, kEventParamMouseLocation, typeQDPoint, 0,
        sizeof(mouseLoc), 0, &mouseLoc);

    EventMouseButton mouseButton;
    GetEventParameter(evt, kEventParamMouseButton, typeMouseButton, 0,
        sizeof(mouseButton), 0, &mouseButton);

    UInt32 modifiers;
    GetEventParameter(evt, kEventParamKeyModifiers, typeUInt32, 0,
        sizeof(modifiers), 0, &modifiers);

    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    GlobalToLocal(&mouseLoc);
    SetPort(currPort);

    theApp->OnMouseClick(mouseButton, WindowApplication::MOUSE_DOWN,
        mouseLoc.h, mouseLoc.v, modifiers);

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseUp (EventHandlerCallRef, EventRef evt,
    void*)
{
    Point mouseLoc;    
    GetEventParameter(evt, kEventParamMouseLocation, typeQDPoint, 0,
        sizeof(mouseLoc), 0, &mouseLoc);

    EventMouseButton mouseButton;
    GetEventParameter(evt, kEventParamMouseButton, typeMouseButton, 0,
        sizeof(mouseButton), 0, &mouseButton);

    UInt32 modifiers;
    GetEventParameter(evt, kEventParamKeyModifiers, typeUInt32, 0,
        sizeof(modifiers), 0, &modifiers);

    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    GlobalToLocal(&mouseLoc);
    SetPort(currPort);

    theApp->OnMouseClick(mouseButton, WindowApplication::MOUSE_UP,
        mouseLoc.h, mouseLoc.v, modifiers);

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseDragged (EventHandlerCallRef, EventRef evt,
    void*)
{
    Point mouseLoc;    
    GetEventParameter(evt, kEventParamMouseLocation, typeQDPoint, 0,
        sizeof(mouseLoc), 0, &mouseLoc);

    EventMouseButton mouseButton;
    GetEventParameter(evt, kEventParamMouseButton, typeMouseButton, 0,
        sizeof(mouseButton), 0, &mouseButton);

    UInt32 modifiers;
    GetEventParameter(evt, kEventParamKeyModifiers, typeUInt32, 0,
        sizeof(modifiers), 0, &modifiers);

    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    GlobalToLocal(&mouseLoc);
    SetPort(currPort);

    theApp->OnMotion(mouseButton, mouseLoc.h, mouseLoc.v, modifiers);

    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseMoved (EventHandlerCallRef, EventRef evt,
    void*)
{
    Point mouseLoc;    
    GetEventParameter(evt,kEventParamMouseLocation, typeQDPoint, 0,
        sizeof(mouseLoc), 0, &mouseLoc);

    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr currPort;
    GetPort(&currPort);
    SetPortWindowPort((WindowRef)(theApp->GetWindowID()));
    GlobalToLocal(&mouseLoc);
    SetPort(currPort);

    theApp->OnPassiveMotion(mouseLoc.h, mouseLoc.v);
    
    // Allow standard handler to run.
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal void ProcessTimer (EventLoopTimerRef, void*)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    theApp->OnIdle();
}
//----------------------------------------------------------------------------
static OSErr QuitAppleEventHandler(const AppleEvent*, AppleEvent*, UInt32)
{
    QuitApplicationEventLoop();

    GWorldPtr back;
    GetExtraData(AGLAPP_BACK, sizeof(GWorldPtr), &back);
    DisposeGWorld(back);
    return noErr;
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int, char**)
{
    // Initialize the extra data.  TODO:  Port the extra-data system of WM4
    // to WM5.
    memset(gsExtraData, 0, APP_EXTRA_DATA_QUANTITY*sizeof(char));

    InitCursor();
    
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;
    theApp->KEY_TERMINATE = WindowApplication::KEY_ESCAPE;

    // OpenGL uses a projection matrix for depth in [-1,1].
    Camera::SetDefaultDepthType(Camera::PM_DEPTH_MINUS_ONE_TO_ONE);

    // Allocate temporary back buffer to be used for font management.
    GDHandle device = GetGDevice();
    PixMapHandle pixmap = (**device).gdPMap;
    Rect area;
    GetPixBounds(pixmap, &area);
    int depth = GetPixDepth(pixmap);
    GWorldPtr back;
    OSErr error = NewGWorld(&back, depth, &area, 0, 0, useTempMem | pixPurge);
    if (error != noErr || !back)
    {
        return -1;
    }

    SetExtraData(AGLAPP_BACK, sizeof(GWorldPtr), &back);
    
    // Assign desired font settings to back buffer.
    const int fontSize = 9;
    unsigned char fontName[256];
    fontName[0] = 6;
    strcpy((char*)&fontName[1], "Monaco");
    short fontNum;
    GetFNum(fontName, &fontNum);
    SetExtraData(AGLAPP_FONT, sizeof(short), &fontNum);

    GWorldPtr currentWorld;
    GDHandle currentDevice;
    GetGWorld(&currentWorld, &currentDevice);
    SetGWorld(back, 0);

    TextFont(fontNum);
    TextSize(fontSize);
    TextFace(normal);
    SetGWorld(currentWorld, currentDevice);
    
    // Add standard window menu.
    MenuRef menu = 0;
    CreateStandardWindowMenu(0, &menu);
    InsertMenu(menu, 0);
    
    // Change current directory into application bundle.
    FSRef processRef;
    FSCatalogInfo processInfo;
    ProcessSerialNumber serial = { 0, kCurrentProcess };
    GetProcessBundleLocation(&serial, &processRef);
    FSSpec fileSpec;
    FSGetCatalogInfo(&processRef, kFSCatInfoNodeFlags, &processInfo, 0,
        &fileSpec, 0);
    std::string appFile = GetStringPathname(fileSpec);
    const int maxPathLen = 1024;
    char path[maxPathLen];
    strcpy(path, appFile.c_str());
    char* last = strrchr(path, '/');
    *last = 0;
    int result = chdir(path);
    assertion(result == 0, "Cannot change directory.\n");
    
    // If the application is packaged, we have to get back up a couple of
    // levels such that the current working directory is the same as the
    // application's directory.  
    char buffer[maxPathLen];
    char* currentDirectory = getcwd(buffer, maxPathLen);
    if (strstr(currentDirectory, "/Contents/MacOS"))
    {
        result = chdir("../../..");
        assertion(result == 0, "Cannot change directory.\n");
        currentDirectory = getcwd(buffer, maxPathLen);
    }

#if 0
    // TODO.  We had this in Wild Magic 4.  Add it to Wild Magic 5.
    //
    // Launch a file dialog, if requested, when the application needs to
    // select an input file.  The derived-class application should set
    // mLaunchFileDialog to 'true' in its constructor when the dialog is
    // needed.
    if (theApp->LaunchFileDialog())
    {
        char* arguments = GetCommandLine();
        if (arguments)
        {
            delete0(theApp->TheCommand);
            theApp->TheCommand = new0 Command(arguments);
            delete1(arguments);
        }
    }
#endif

    if (!theApp->OnPrecreate())
    {
        return -2;
    }
    
    // Require the window to have the specified client area.
    short windowTop = 60, windowLeft = 40;
    Rect rect = { windowTop, windowLeft, theApp->GetHeight() + windowTop - 1,
        theApp->GetWidth() + windowLeft - 1 };

    // Create the application window.
    WindowRef window = 0;
    OSStatus status = CreateNewWindow(kDocumentWindowClass, 
        kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute,
        &rect, &window);
    if (status != noErr)
    {
        assertion(false, "Error in creating window.\n");
        return -3;
    }

    // Grab GD from kRect - based on FindGDHandleFromRect in Carbon SetupGL.
    GDHandle hGD = 0;
    GDHandle hgdNthDevice = GetDeviceList();
    unsigned int greatestArea = 0;

    // Check window against all gdRects in gDevice list and remember which
    // gdRect contains largest area of window.
    while (hgdNthDevice)
    {
        if (TestDeviceAttribute(hgdNthDevice, screenDevice)
        &&  TestDeviceAttribute(hgdNthDevice, screenActive))
        {
            // The SectRect routine calculates the intersection of the window
            // rectangle and this gDevice rectangle and returns TRUE if the
            // rectangles intersect, FALSE if they don't.
            Rect rectSect;
            SectRect(&rect, &(**hgdNthDevice).gdRect, &rectSect);

            // Determine which screen holds greatest window area first.
            // Calculate area of rectangle on current device.
            unsigned int sectArea = 
                (unsigned int)(rectSect.right - rectSect.left) * 
                (rectSect.bottom - rectSect.top);
            if (sectArea > greatestArea)
            {
                greatestArea = sectArea; // set greatest area so far
                hGD = hgdNthDevice; // set zoom device
            }
            hgdNthDevice = GetNextDevice(hgdNthDevice);
        }
    }

    // Set window title.
    CFStringRef windowTitle = CFStringCreateWithCString(0, 
        theApp->GetWindowTitle(), kCFStringEncodingASCII);
    SetWindowTitleWithCFString(window, windowTitle);
    CFRelease(windowTitle);

    theApp->SetWindowID((int)window);
    SetPortWindowPort(window);

    // Create the renderer.
    RendererInput input;
    input.mDevice = (AGLDevice)hGD;
    input.mWindow = window;
    mRenderer = new0 Renderer(input, theApp->GetWidth(), theApp->GetHeight(),
        mColorFormat, mDepthStencilFormat, mNumMultisamples);

    // Install quit apple event handler.
    error = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
        NewAEEventHandlerUPP((AEEventHandlerProcPtr)QuitAppleEventHandler), 0,
        false);
    if (error != noErr)
    {
        ExitToShell();
    }

    // Install window close handler.
    EventTypeSpec eventType;
    eventType.eventClass = kEventClassWindow;
    eventType.eventKind = kEventWindowClose;
    EventHandlerUPP handlerUPP = NewEventHandlerUPP(ProcessWindowClose); 
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Install window bounds change handler.
    eventType.eventKind = kEventWindowBoundsChanged;
    handlerUPP = NewEventHandlerUPP(ProcessWindowBoundsChange);
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Install window zoomed handler.
    eventType.eventKind = kEventWindowZoomed;
    handlerUPP = NewEventHandlerUPP(ProcessWindowZoomed);
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Install window redraw handler.
    eventType.eventKind = kEventWindowDrawContent;
    handlerUPP = NewEventHandlerUPP(ProcessWindowRedraw);
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Install key down handler.
    EventTypeSpec eventTypes[2];
    eventTypes[0].eventClass = kEventClassKeyboard;
    eventTypes[0].eventKind = kEventRawKeyDown;
    eventTypes[1].eventClass = kEventClassKeyboard;
    eventTypes[1].eventKind = kEventRawKeyRepeat;
    handlerUPP = NewEventHandlerUPP(ProcessKeyDown);
    InstallWindowEventHandler(window, handlerUPP, 2, eventTypes, 0, 0);

    // Install key up handler.
    eventTypes[0].eventClass = kEventClassKeyboard;
    eventTypes[0].eventKind = kEventRawKeyUp;
    handlerUPP = NewEventHandlerUPP(ProcessKeyUp);
    InstallWindowEventHandler(window, handlerUPP, 1, eventTypes, 0, 0);

    // Install key-modifier-changed handler.
    eventTypes[0].eventClass = kEventClassKeyboard;
    eventTypes[0].eventKind = kEventRawKeyModifiersChanged;
    handlerUPP = NewEventHandlerUPP(ProcessKeyModifierChanged);
    InstallWindowEventHandler(window, handlerUPP, 1, eventTypes, 0, 0);

    // Install mouse down handler.
    eventType.eventClass = kEventClassMouse;
    eventType.eventKind = kEventMouseDown;
    handlerUPP = NewEventHandlerUPP(ProcessMouseDown);
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Install mouse up handler.
    eventType.eventKind = kEventMouseUp;
    handlerUPP = NewEventHandlerUPP(ProcessMouseUp);
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Install mouse drag handler.
    eventType.eventKind = kEventMouseDragged;
    handlerUPP = NewEventHandlerUPP(ProcessMouseDragged);
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Install mouse move handler.
    eventType.eventKind = kEventMouseMoved;
    handlerUPP = NewEventHandlerUPP(ProcessMouseMoved);
    InstallWindowEventHandler(window, handlerUPP, 1, &eventType, 0, 0);

    // Create timer.
    EventLoopTimerRef timer;
    InstallEventLoopTimer(GetMainEventLoop(), 0, kEventDurationMillisecond, 
        NewEventLoopTimerUPP(ProcessTimer), 0, &timer);

    if (!theApp->OnInitialize())
    {
        return -4;
    }

    // Set auto-repeat key timing.
    short keyRepeatThresh = LMGetKeyRepThresh();
    short keyThresh = LMGetKeyThresh();
    LMSetKeyRepThresh(1);
    LMSetKeyThresh(30);

    // The default OnPreidle() clears the buffers.  Allow the application to
    // fill them before the window is shown and before the event loop starts.
    theApp->OnPreidle();

    // Display the window.
    ShowWindow(window);

    // Run event loop.
    RunApplicationEventLoop();

    // Reset auto-repeat key timing to initial value.
    LMSetKeyRepThresh(keyRepeatThresh);
    LMSetKeyThresh(keyThresh);

    theApp->OnTerminate();
    delete0(mRenderer);

    RemoveEventLoopTimer(timer);
    DisposeMenu(menu);
    return 0;
}
//----------------------------------------------------------------------------

