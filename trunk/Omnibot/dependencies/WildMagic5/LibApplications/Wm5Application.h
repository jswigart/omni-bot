// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5APPLICATION_H
#define WM5APPLICATION_H

#include "Wm5Core.h"
#include "Wm5Mathematics.h"
#include "Wm5Imagics.h"
#include "Wm5Physics.h"
#include "Wm5Graphics.h"
#include "Wm5Renderers.h"
#include "Wm5Command.h"

namespace Wm5
{

class Application
{
protected:
    // Abstract base class.
    Application ();
public:
    virtual ~Application ();

    // The unique application object.
    static Application* TheApplication;

    // The unique command-line arguments object.
    static Command* TheCommand;

    // The installation directory of Wild Magic 5.
    static std::string WM5Path;

    // The path to the application project directory.
    static std::string ThePath;

    // Derived classes must set this function pointer.  The int parameter is
    // the number of command line arguments.  The char** parameter is the
    // array of argument strings.
    typedef int (*EntryPoint)(int, char**);
    static EntryPoint Run;
};

}

#endif
