// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/06/18)

#ifndef WM5DISTPOINT3ELLIPSOID3_H
#define WM5DISTPOINT3ELLIPSOID3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Ellipsoid3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint3Ellipsoid3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistPoint3Ellipsoid3 (const Vector3<Real>& point,
        const Ellipsoid3<Real>& ellipsoid);

    // Object access.
    const Vector3<Real>& GetPoint () const;
    const Ellipsoid3<Real>& GetEllipsoid () const;

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

    // The ellipsoid is (x0/e0)^2 + (x1/e1)^2 + (x2/e2)^2 = 1 with
    // e0 >= e1 >= e2.  The query point is (y0,y1,y2) with y0 >= 0, y1 >= 0,
    // and y2 >= 0.  The function returns the squared distance from the query
    // point to the ellipsoid.  It also computes the ellipsoid point
    // (x0,x1,x2) in the first octant that is closest to (y0,y1,y2).
    Real SqrDistanceSpecial (const Real e[3], const Real y[3], Real x[3]);

    // The ellipsoid is (x0/e0)^2 + (x1/e1)^2 + (x2/e2)^2 = 1.  The query
    // point is (y0,y1,y2).  The function returns the squared distance from
    // the query point to the ellipsoid.   It also computes the ellipsoid
    // point (x0,x1,x2) that is closest to (y0,y1,y2).
    Real SqrDistance (const Real e[3], const Real y[3], Real x[3]);

    // The bisection algorithm to find the unique root of F(t).
    Real Bisector (int numComponents, const Real* e, const Real* y,
        Real* x);

    const Vector3<Real>* mPoint;
    const Ellipsoid3<Real>* mEllipsoid;
};

typedef DistPoint3Ellipsoid3<float> DistPoint3Ellipsoid3f;
typedef DistPoint3Ellipsoid3<double> DistPoint3Ellipsoid3d;

}

#endif
