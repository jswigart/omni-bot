// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTPOINT3FRUSTUM3_H
#define WM5DISTPOINT3FRUSTUM3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Frustum3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint3Frustum3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistPoint3Frustum3 (const Vector3<Real>& point,
        const Frustum3<Real>& frustum);

    // Object access.
    const Vector3<Real>& GetPoint () const;
    const Frustum3<Real>& GetFrustum () const;

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
    const Frustum3<Real>* mFrustum;
};

typedef DistPoint3Frustum3<float> DistPoint3Frustum3f;
typedef DistPoint3Frustum3<double> DistPoint3Frustum3d;

}

#endif
