// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTROLLEDOBJECT_H
#define WM5CONTROLLEDOBJECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Controller.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ControlledObject : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ControlledObject);

protected:
    // Abstract base class.  Construction and destruction.
    ControlledObject ();
public:
    virtual ~ControlledObject ();

    // Access to the controllers that control this object.
    inline int GetNumControllers () const;
    inline Controller* GetController (int i) const;
    void AttachController (Controller* controller);
    void DetachController (Controller* controller);
    void DetachAllControllers ();
    bool UpdateControllers (double applicationTime);

private:
    // The array of controllers that control this object.
    enum { CO_GROW_BY = 4 };
    int mNumControllers, mCapacity;
    ControllerPtr* mControllers;
};

WM5_REGISTER_STREAM(ControlledObject);
typedef Pointer0<ControlledObject> ControlledObjectPtr;
#include "Wm5ControlledObject.inl"

}

#endif
