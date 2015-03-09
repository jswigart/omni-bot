// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSPHERE3CONE3_H
#define WM5INTRSPHERE3CONE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Sphere3.h"
#include "Wm5Cone3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSphere3Cone3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSphere3Cone3 (const Sphere3<Real>& sphere, const Cone3<Real>& cone);

    // Object access.
    const Sphere3<Real>& GetSphere () const;
    const Cone3<Real>& GetCone () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // In the static find-intersection query, if an intersection occurs
    // between the sphere and cone, it is potentially an infinite set.  The
    // intersection point closest to the cone vertex is returned by this
    // function.
    const Vector3<Real>& GetPoint () const;

private:
    // The objects to intersect.
    const Sphere3<Real>* mSphere;
    const Cone3<Real>* mCone;

    // Closest intersection point to cone vertex.
    Vector3<Real> mPoint;
};

typedef IntrSphere3Cone3<float> IntrSphere3Cone3f;
typedef IntrSphere3Cone3<double> IntrSphere3Cone3d;

}

#endif
