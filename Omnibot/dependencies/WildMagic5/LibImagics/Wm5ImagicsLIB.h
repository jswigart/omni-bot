// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IMAGICSLIB_H
#define WM5IMAGICSLIB_H

#include "Wm5MathematicsLIB.h"

// Begin Microsoft Windows DLL support.
#if defined(WM5_IMAGICS_DLL_EXPORT)
    // For the DLL library.
    #define WM5_IMAGICS_ITEM __declspec(dllexport)
#elif defined(WM5_IMAGICS_DLL_IMPORT)
    // For a client of the DLL library.
    #define WM5_IMAGICS_ITEM __declspec(dllimport)
#else
    // For the static library and for Apple/Linux.
    #define WM5_IMAGICS_ITEM
#endif
// End Microsoft Windows DLL support.

#endif
