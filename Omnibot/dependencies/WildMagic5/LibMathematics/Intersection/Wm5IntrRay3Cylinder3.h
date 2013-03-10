// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRRAY3CYLINDER3_H
#define WM5INTRRAY3CYLINDER3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ray3.h"
#include "Wm5Cylinder3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrRay3Cylinder3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Cylinder3 (const Ray3<Real>& ray,
        const Cylinder3<Real>& cylinder);

    // Object access.
    const Ray3<Real>& GetRay () const;
    const Cylinder3<Real>& GetCylinder () const;

    // Static intersection query.
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Ray3<Real>* mRay;
    const Cylinder3<Real>* mCylinder;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
};

typedef IntrRay3Cylinder3<float> IntrRay3Cylinder3f;
typedef IntrRay3Cylinder3<double> IntrRay3Cylinder3d;

}

#endif
