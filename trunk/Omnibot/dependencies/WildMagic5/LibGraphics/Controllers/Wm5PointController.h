// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POINTCONTROLLER_H
#define WM5POINTCONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Controller.h"
#include "Wm5AVector.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM PointController : public Controller
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(PointController);

protected:
    // Abstract base class.  Construction and destruction.  The object to
    // which this is attached must be Polypoint or a class derived from
    // Polypoint.
    PointController ();
public:
    // Abstract base class.
    virtual ~PointController ();

    // The system motion, in local coordinates.  The velocity vectors should
    // be unit length.
    float SystemLinearSpeed;
    float SystemAngularSpeed;
    AVector SystemLinearAxis;
    AVector SystemAngularAxis;

    // Point motion, in the model space of the system.  The velocity vectors
    // should be unit length.  In applications where the points represent a
    // rigid body, you might choose the origin of the system to be the center
    // of mass of the points and the coordinate axes to correspond to the
    // principal directions of the inertia tensor.
    inline int GetNumPoints () const;
    inline float* GetPointLinearSpeed () const;
    inline float* GetPointAngularSpeed () const;
    inline AVector* GetPointLinearAxis () const;
    inline AVector* GetPointAngularAxis () const;

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

protected:
    // For deferred allocation of the point motion arrays.
    void Reallocate (int numPoints);
    virtual void SetObject (ControlledObject* object);

    // This class computes the new positions and orientations from the motion
    // parameters.  Derived classes should update the motion parameters and
    // then either call the base class update methods or provide its own
    // update methods for position and orientation.
    virtual void UpdateSystemMotion (float ctrlTime);
    virtual void UpdatePointMotion (float ctrlTime);

    int mNumPoints;
    float* mPointLinearSpeed;
    float* mPointAngularSpeed;
    AVector* mPointLinearAxis;
    AVector* mPointAngularAxis;
};

WM5_REGISTER_STREAM(PointController);
typedef Pointer0<PointController> PointControllerPtr;
#include "Wm5PointController.inl"

}

#endif
