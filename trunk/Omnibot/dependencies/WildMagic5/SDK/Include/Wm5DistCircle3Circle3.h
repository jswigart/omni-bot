// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTCIRCLE3CIRCLE3_H
#define WM5DISTCIRCLE3CIRCLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Circle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistCircle3Circle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistCircle3Circle3 (const Circle3<Real>& circle0,
        const Circle3<Real>& circle1);

    // Object access.
    const Circle3<Real>& GetCircle0 () const;
    const Circle3<Real>& GetCircle1 () const;

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

    const Circle3<Real>* mCircle0;
    const Circle3<Real>* mCircle1;
};

typedef DistCircle3Circle3<float> DistCircle3Circle3f;
typedef DistCircle3Circle3<double> DistCircle3Circle3d;

}

#endif
