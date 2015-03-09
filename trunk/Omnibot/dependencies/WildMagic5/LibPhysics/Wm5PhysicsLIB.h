// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PHYSICSLIB_H
#define WM5PHYSICSLIB_H

#include "Wm5MathematicsLIB.h"

// Begin Microsoft Windows DLL support.
#if defined(WM5_PHYSICS_DLL_EXPORT)
    // For the DLL library.
    #define WM5_PHYSICS_ITEM __declspec(dllexport)
#elif defined(WM5_PHYSICS_DLL_IMPORT)
    // For a client of the DLL library.
    #define WM5_PHYSICS_ITEM __declspec(dllimport)
#else
    // For the static library and for Apple/Linux.
    #define WM5_PHYSICS_ITEM
#endif
// End Microsoft Windows DLL support.

// The CollisionGroup, CollisionRecord, and BoundTree classes are templated.
// If changes are made to these classes, enable this define to ensure that the
// code compiles.
//#define WM5_TEST_COLLISION_COMPILE

// For writing messages to a log file during testing and debugging of
// LCPSolver.
//#define WM5_LCPSOLVER_LOG
#ifdef WM5_LCPSOLVER_LOG
    #define WM5_LCPSOLVER_FUNCTION(func) func
#else
    #define WM5_LCPSOLVER_FUNCTION(func)
#endif

// For writing messages to a log file during testing and debugging of
// LCPPolyDist.  If you enable this, the distance calculator significantly
// slows down.
//#define WM5_LCPPOLYDIST_LOG
#ifdef WM5_LCPPOLYDIST_LOG
    #define WM5_LCPPOLYDIST_FUNCTION(func) func
#else
    #define WM5_LCPPOLYDIST_FUNCTION(func)
#endif

#endif
