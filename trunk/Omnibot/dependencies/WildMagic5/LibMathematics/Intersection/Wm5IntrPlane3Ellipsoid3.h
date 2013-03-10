// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRPLANE3ELLIPSOID3_H
#define WM5INTRPLANE3ELLIPSOID3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Ellipsoid3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrPlane3Ellipsoid3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Ellipsoid3 (const Plane3<Real>& plane,
        const Ellipsoid3<Real>& ellipsoid);

    // Object access.
    const Plane3<Real>& GetPlane () const;
    const Ellipsoid3<Real>& GetEllipsoid () const;

    // Static intersection query.
    virtual bool Test ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The ellipsoid is culled if it is on the negative
    // side of the plane.
    bool EllipsoidIsCulled () const;

protected:
    // The objects to intersect.
    const Plane3<Real>* mPlane;
    const Ellipsoid3<Real>* mEllipsoid;
};

typedef IntrPlane3Ellipsoid3<float> IntrPlane3Ellipsoid3f;
typedef IntrPlane3Ellipsoid3<double> IntrPlane3Ellipsoid3d;

}

#endif
