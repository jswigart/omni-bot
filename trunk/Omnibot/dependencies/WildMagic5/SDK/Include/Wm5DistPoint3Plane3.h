// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTPOINT3PLANE3_H
#define WM5DISTPOINT3PLANE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Plane3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint3Plane3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistPoint3Plane3 (const Vector3<Real>& point,
        const Plane3<Real>& plane);

    // Object access.
    const Vector3<Real>& GetPoint () const;
    const Plane3<Real>& GetPlane () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Vector3<Real>* mPoint;
    const Plane3<Real>* mPlane;
};

typedef DistPoint3Plane3<float> DistPoint3Plane3f;
typedef DistPoint3Plane3<double> DistPoint3Plane3d;

}

#endif
