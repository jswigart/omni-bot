// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/30)

#include "Wm5ApplicationPCH.h"
#include "Wm5ConsoleApplication.h"
using namespace Wm5;

//----------------------------------------------------------------------------
ConsoleApplication::ConsoleApplication (const char* consoleTitle)
    :
    mConsoleTitle(consoleTitle)
{
    ThePath = WM5Path + mConsoleTitle + "/";
}
//----------------------------------------------------------------------------
ConsoleApplication::~ConsoleApplication ()
{
}
//----------------------------------------------------------------------------
int ConsoleApplication::Run (int numArguments, char** arguments)
{
    ConsoleApplication* theApp = (ConsoleApplication*)TheApplication;
    return theApp->Main(numArguments, arguments);
}
//----------------------------------------------------------------------------
