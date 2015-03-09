// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRTRIANGLE3BOX3_H
#define WM5INTRTRIANGLE3BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Triangle3.h"
#include "Wm5Box3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrTriangle3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Box3 (const Triangle3<Real>& triangle,
        const Box3<Real>& box);

    // Object access.
    const Triangle3<Real>& GetTriangle () const;
    const Box3<Real>& GetBox () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Static test-intersection query.
    virtual bool Find ();

    // Dynamic test-intersection query.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Dynamic find-intersection query.
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // The objects to intersect.
    const Triangle3<Real>* mTriangle;
    const Box3<Real>* mBox;

    // The intersections for static query.  A triangle can intersect a box
    // in at most 7 vertices.
    int mQuantity;
    Vector3<Real> mPoint[7];
};

typedef IntrTriangle3Box3<float> IntrTriangle3Box3f;
typedef IntrTriangle3Box3<double> IntrTriangle3Box3d;

}

#endif
