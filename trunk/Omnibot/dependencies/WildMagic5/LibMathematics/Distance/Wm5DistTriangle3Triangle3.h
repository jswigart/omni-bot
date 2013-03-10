// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTTRIANGLE3TRIANGLE3_H
#define WM5DISTTRIANGLE3TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Triangle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistTriangle3Triangle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistTriangle3Triangle3 (const Triangle3<Real>& triangle0,
        const Triangle3<Real>& triangle1);

    // Object access.
    const Triangle3<Real>& GetTriangle0 () const;
    const Triangle3<Real>& GetTriangle1 () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest points.
    Real GetTriangleBary0 (int i) const;
    Real GetTriangleBary1 (int i) const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Triangle3<Real>* mTriangle0;
    const Triangle3<Real>* mTriangle1;

    // Information about the closest points.
    Real mTriangleBary0[3];  // closest0 = sum_{i=0}^2 bary0[i]*vertex0[i]
    Real mTriangleBary1[3];  // closest1 = sum_{i=0}^2 bary1[i]*vertex1[i]
};

typedef DistTriangle3Triangle3<float> DistTriangle3Triangle3f;
typedef DistTriangle3Triangle3<double> DistTriangle3Triangle3d;

}

#endif
