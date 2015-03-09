// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/30)

#ifndef WM5CONSOLEAPPLICATION_H
#define WM5CONSOLEAPPLICATION_H

#include "Wm5Application.h"

namespace Wm5
{

class ConsoleApplication : public Application
{
public:
    // Construction and destruction.
    ConsoleApplication (const char* consoleTitle);
    virtual ~ConsoleApplication ();

    // Entry point to be implemented by the application.  The return value
    // is an exit code, if desired.
    virtual int Main (int numArguments, char** arguments) = 0;

protected:
    // The hookup to the 'main' entry point into the executable.
    static int Run (int numArguments, char** arguments);

    // Console parameters (from the constructor).
    std::string mConsoleTitle;
};

//----------------------------------------------------------------------------
#define WM5_CONSOLE_APPLICATION(classname) \
WM5_IMPLEMENT_INITIALIZE(classname); \
WM5_IMPLEMENT_TERMINATE(classname); \
\
void classname::Initialize () \
{ \
    Application::Run = &ConsoleApplication::Run; \
    TheApplication = new0 classname(); \
} \
\
void classname::Terminate () \
{ \
    delete0(TheApplication); \
}
//----------------------------------------------------------------------------
}

#endif
