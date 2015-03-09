// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/30)

#include "Wm5ApplicationPCH.h"
#include "Wm5Application.h"
using namespace Wm5;

Application* Application::TheApplication = 0;
Command* Application::TheCommand = 0;
std::string Application::WM5Path;
std::string Application::ThePath;
Application::EntryPoint Application::Run = 0;

//----------------------------------------------------------------------------
Application::Application ()
{
}
//----------------------------------------------------------------------------
Application::~Application ()
{
}
//----------------------------------------------------------------------------
int main (int numArguments, char* arguments[])
{
#ifdef WM5_USE_MEMORY
    // To specify your own allocator and deallocator, change this function
    // call to Memory::Initialize(yourAllocator, yourDeallocator).  See the
    // Memory class in LibCore/Memory/Wm5Memory.h for the signatures of these
    // functions.
    Memory::Initialize();
#endif

    // The Wild Magic 5 application layer depends on the directory structure
    // structure that ships with the libraries.  You need to create the
    // WM5_PATH environment variable in order for the applications to find
    // various data files.
#ifndef __APPLE__
    Application::WM5Path = Environment::GetVariable("WM5_PATH");
#else
    // Mac OS X Lion returns NULL on any getenv call (such as the one in
    // Environment::GetVariable).  This hack works around the problem.
    if (system("cp ~/.MacOSX/wm5path.txt tempwm5path.txt") == 0)
    {
        std::ifstream inFile("tempwm5path.txt");
        if (inFile)
        {
            getline(inFile, Application::WM5Path);
            inFile.close();
            system("rm tempwm5path.txt");
        }
    }
#endif
    if (Application::WM5Path == "")
    {
        assertion(false, "Please set the WM5_PATH environment variable.\n");
        std::ofstream outFile("ApplicationError.txt");
        if (outFile)
        {
            outFile << "Please set the WM5_PATH environment variable.\n";
            outFile.close();
        }
        return INT_MAX;
    }
    Application::WM5Path += "/";

    InitTerm::ExecuteInitializers();

    // TODO:  Set paths involving folders BigEndian/LittleEndian and
    // DirectX/OpenGL.  The folder names for endianness can be set up
    // using WM5_BIG_ENDIAN/WM5_LITTLE_ENDIAN.  The folder names for
    // the renderer type can be made available as static strings in
    // the Renderer class.
    std::string directory;

    // Always check the application directory.
    Environment::InsertDirectory(Application::ThePath);

    // The path to shader FX files.
    directory = Application::WM5Path + std::string("Data/Wmfx/");
    Environment::InsertDirectory(directory);

    // The path to scene object files.
    directory = Application::WM5Path + std::string("Data/Wmof/");
    Environment::InsertDirectory(directory);

    // The path to texture image files.
    directory = Application::WM5Path + std::string("Data/Wmtf/");
    Environment::InsertDirectory(directory);

    // The path to vertex/index buffer files.
    directory = Application::WM5Path + std::string("Data/Wmvf/");
    Environment::InsertDirectory(directory);

    // The path to image files.
    directory = Application::WM5Path + std::string("Data/Im/");
    Environment::InsertDirectory(directory);

    Application::TheCommand = new0 Command(numArguments, arguments);
    int exitCode = Application::Run(numArguments, arguments);
    delete0(Application::TheCommand);

    Environment::RemoveAllDirectories();

    InitTerm::ExecuteTerminators();

#ifdef WM5_USE_MEMORY
    Memory::Terminate(Application::ThePath + "MemoryReport.txt");
#endif

    return exitCode;
}
//----------------------------------------------------------------------------
