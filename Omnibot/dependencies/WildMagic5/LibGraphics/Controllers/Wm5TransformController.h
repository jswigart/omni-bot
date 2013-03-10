// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.1 (2010/10/01)

#ifndef WM5TRANSFORMCONTROLLER_H
#define WM5TRANSFORMCONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Controller.h"
#include "Wm5Transform.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM TransformController : public Controller
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(TransformController);

public:
    // Construction and destruction.
    TransformController (const Transform& localTransform);
    virtual ~TransformController ();

    // Member access.
    inline void SetTransform (const Transform& localTransform);
    inline const Transform& GetTransform () const;

    // The animation update.  The application time is in milliseconds.
    // The update simply copies mLocalTransform to the Spatial mObject's
    // LocalTransform.  In this sense, TransformController represents a
    // transform that is constant for all time.
    virtual bool Update (double applicationTime);

protected:
    Transform mLocalTransform;
};

WM5_REGISTER_STREAM(TransformController);
typedef Pointer0<TransformController> TransformControllerPtr;
#include "Wm5TransformController.inl"

}

#endif
