// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INITTERM_H
#define WM5INITTERM_H

#include "Wm5CoreLIB.h"

namespace Wm5
{

class WM5_CORE_ITEM InitTerm
{
public:
    typedef void (*Initializer)(void);
    static void AddInitializer (Initializer function);
    static void ExecuteInitializers ();

    typedef void (*Terminator)(void);
    static void AddTerminator (Terminator function);
    static void ExecuteTerminators ();

private:
    // This number must be large enough to support your application.  If your
    // application triggers an assert about exceeding this, change this number
    // and recompile.  The goal is to avoid dynamic allocation during premain
    // and postmain execution, thus making it easier to manage and track
    // memory usage.
    enum
    {
        MAX_ELEMENTS = 512
    };

    static int msNumInitializers;
    static Initializer msInitializers[MAX_ELEMENTS];

    static int msNumTerminators;
    static Terminator msTerminators[MAX_ELEMENTS];
};

}

//----------------------------------------------------------------------------
#define WM5_DECLARE_INITIALIZE \
public: \
    static bool RegisterInitialize (); \
    static void Initialize (); \
private: \
    static bool msInitializeRegistered
//----------------------------------------------------------------------------
#define WM5_IMPLEMENT_INITIALIZE(classname) \
bool classname::msInitializeRegistered = false; \
bool classname::RegisterInitialize () \
{ \
    if (!msInitializeRegistered) \
    { \
        InitTerm::AddInitializer(classname::Initialize); \
        msInitializeRegistered = true; \
    } \
    return msInitializeRegistered; \
}
//----------------------------------------------------------------------------
#define WM5_REGISTER_INITIALIZE(classname) \
static bool gsInitializeRegistered_##classname = \
    classname::RegisterInitialize ()
//----------------------------------------------------------------------------
#define WM5_DECLARE_TERMINATE \
public: \
    static bool RegisterTerminate (); \
    static void Terminate (); \
private: \
    static bool msTerminateRegistered
//----------------------------------------------------------------------------
#define WM5_IMPLEMENT_TERMINATE(classname) \
bool classname::msTerminateRegistered = false; \
bool classname::RegisterTerminate () \
{ \
    if (!msTerminateRegistered) \
    { \
        InitTerm::AddTerminator(classname::Terminate); \
        msTerminateRegistered = true; \
    } \
    return msTerminateRegistered; \
}
//----------------------------------------------------------------------------
#define WM5_REGISTER_TERMINATE(classname) \
static bool gsTerminateRegistered_##classname = \
    classname::RegisterTerminate ()
//----------------------------------------------------------------------------

#endif
