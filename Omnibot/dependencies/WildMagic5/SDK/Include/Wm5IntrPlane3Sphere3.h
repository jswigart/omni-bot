// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRPLANE3SPHERE3_H
#define WM5INTRPLANE3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Sphere3.h"
#include "Wm5Circle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrPlane3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Sphere3 (const Plane3<Real>& plane,
        const Sphere3<Real>& sphere);

    // Object access.
    const Plane3<Real>& GetPlane () const;
    const Sphere3<Real>& GetSphere () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The sphere is culled if it is on the negative
    // side of the plane.
    bool SphereIsCulled () const;

    // The intersection set.  The circle is valid only when Find() returns
    // 'true'.  If the intersection does not exist, the circle radius is
    // set to -1 as an additional indication that the circle is not valid.
    const Circle3<Real>& GetCircle () const;

protected:
    // The objects to intersect.
    const Plane3<Real>* mPlane;
    const Sphere3<Real>* mSphere;

    // The intersection set.
    Circle3<Real> mCircle;
};

typedef IntrPlane3Sphere3<float> IntrPlane3Sphere3f;
typedef IntrPlane3Sphere3<double> IntrPlane3Sphere3d;

}

#endif
