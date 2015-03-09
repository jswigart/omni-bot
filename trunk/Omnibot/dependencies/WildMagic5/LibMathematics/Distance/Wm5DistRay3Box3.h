// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTRAY3BOX3_H
#define WM5DISTRAY3BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Ray3.h"
#include "Wm5Box3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistRay3Box3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistRay3Box3 (const Ray3<Real>& ray, const Box3<Real>& box);

    // Object access.
    const Ray3<Real>& GetRay () const;
    const Box3<Real>& GetBox () const;

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

    const Ray3<Real>* mRay;
    const Box3<Real>* mBox;
};

typedef DistRay3Box3<float> DistRay3Box3f;
typedef DistRay3Box3<double> DistRay3Box3d;

}

#endif
