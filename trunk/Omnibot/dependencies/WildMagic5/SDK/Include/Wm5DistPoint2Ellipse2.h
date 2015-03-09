// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/06/18)

#ifndef WM5DISTPOINT2ELLIPSE2_H
#define WM5DISTPOINT2ELLIPSE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Ellipse2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint2Ellipse2
    : public Distance<Real,Vector2<Real> >
{
public:
    DistPoint2Ellipse2 (const Vector2<Real>& point,
        const Ellipse2<Real>& rkEllipse);

    // Object access.
    const Vector2<Real>& GetPoint () const;
    const Ellipse2<Real>& GetEllipse () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);

private:
    using Distance<Real,Vector2<Real> >::mClosestPoint0;
    using Distance<Real,Vector2<Real> >::mClosestPoint1;

    // The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1 with e0 >= e1.  The query
    // point is (y0,y1) with y0 >= 0 and y1 >= 0.  The function returns the
    // squared distance from the query point to the ellipse.  It also computes
    // the ellipse point (x0,x1) in the first quadrant that is closest to
    // (y0,y1).
    Real SqrDistanceSpecial (const Real e[2], const Real y[2], Real x[2]);

    // The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1.  The query point is (y0,y1).
    // The function returns the squared distance from the query point to the
    // ellipse.  It also computes the ellipse point (x0,x1) that is closest to
    // (y0,y1).
    Real SqrDistance (const Real e[2], const Real y[2], Real x[2]);

    const Vector2<Real>* mPoint;
    const Ellipse2<Real>* mEllipse;
};

typedef DistPoint2Ellipse2<float> DistPoint2Ellipse2f;
typedef DistPoint2Ellipse2<double> DistPoint2Ellipse2d;

}

#endif
