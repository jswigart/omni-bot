// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRPLANE3CAPSULE3_H
#define WM5INTRPLANE3CAPSULE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Capsule3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrPlane3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Capsule3 (const Plane3<Real>& plane,
        const Capsule3<Real>& capsule);

    // Object access.
    const Plane3<Real>& GetPlane () const;
    const Capsule3<Real>& GetCapsule () const;

    // Static intersection query.
    virtual bool Test ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The capsule is culled if it is on the negative
    // side of the plane.
    bool CapsuleIsCulled () const;

protected:
    // The objects to intersect.
    const Plane3<Real>* mPlane;
    const Capsule3<Real>* mCapsule;
};

typedef IntrPlane3Capsule3<float> IntrPlane3Capsule3f;
typedef IntrPlane3Capsule3<double> IntrPlane3Capsule3d;

}

#endif
