// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTPOINT2BOX2_H
#define WM5DISTPOINT2BOX2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Box2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint2Box2
    : public Distance<Real,Vector2<Real> >
{
public:
    DistPoint2Box2 (const Vector2<Real>& point, const Box2<Real>& box);

    // Object access.
    const Vector2<Real>& GetPoint () const;
    const Box2<Real>& GetBox () const;

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

    const Vector2<Real>* mPoint;
    const Box2<Real>* mBox;
};

typedef DistPoint2Box2<float> DistPoint2Box2f;
typedef DistPoint2Box2<double> DistPoint2Box2d;

}

#endif
