// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5CONTROLLER_H
#define WM5CONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"

namespace Wm5
{

class ControlledObject;

class WM5_GRAPHICS_ITEM Controller : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Controller);

protected:
    // Abstract base class.  Construction and destruction.
    Controller ();
public:
    virtual ~Controller ();

    // Member access.
    inline ControlledObject* GetObject () const;
    inline void SetApplicationTime (double applicationTime);
    inline double GetApplicationTime () const;

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

    // Time management.  A controller may use its own time scale, and it
    // specifies how times are to be mapped to application time.
    enum WM5_GRAPHICS_ITEM RepeatType
    {
        RT_CLAMP,
        RT_WRAP,
        RT_CYCLE
    };

    // Member access.
    RepeatType Repeat;  // default = RT_CLAMP
    double MinTime;     // default = 0
    double MaxTime;     // default = 0
    double Phase;       // default = 0
    double Frequency;   // default = 1
    bool Active;        // default = true

public_internal:
    // This function is public because the class ControlledObject needs to set
    // the object during a call to AttachController.  Derived classes that
    // manage a set of controllers also need to set the objects during a call
    // to AttachController.
    virtual void SetObject (ControlledObject* object);

protected:
    // Conversion from application time units to controller time units.
    // Derived classes may use this in their update routines.
    double GetControlTime (double applicationTime);

    // The controlled object.  This is a regular pointer rather than a
    // smart pointer to avoid the reference cycle between mObject and
    // 'this'.
    ControlledObject* mObject;

    // The application time in milliseconds.
    double mApplicationTime;
};

WM5_REGISTER_STREAM(Controller);
typedef Pointer0<Controller> ControllerPtr;
#include "Wm5Controller.inl"

}

#endif
