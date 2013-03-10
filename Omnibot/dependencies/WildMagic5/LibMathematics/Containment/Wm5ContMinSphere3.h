// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTMINSPHERE3_H
#define WM5CONTMINSPHERE3_H

// Compute the minimum volume sphere containing the input set of points.  The
// algorithm randomly permutes the input points so that the construction
// occurs in 'expected' O(N) time.

#include "Wm5MathematicsLIB.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM MinSphere3
{
public:
    // The epsilon value is a floating-point tolerance used for various
    // computations.
    MinSphere3 (int numPoints, const Vector3<Real>* points,
        Sphere3<Real>& minimal, Real epsilon = (Real)1e-03);

private:
    // Indices of points that support current minimum volume sphere.
    class Support
    {
    public:
        bool Contains (int index, Vector3<Real>** points, Real epsilon);

        int Quantity;
        int Index[4];
    };

    // Test whether point P is inside sphere S.
    bool Contains (const Vector3<Real>& point, const Sphere3<Real>& sphere,
        Real& distDiff);

    Sphere3<Real> ExactSphere1 (const Vector3<Real>& P);
    Sphere3<Real> ExactSphere2 (const Vector3<Real>& P0,
        const Vector3<Real>& P1);
    Sphere3<Real> ExactSphere3 (const Vector3<Real>& P0,
        const Vector3<Real>& P1, const Vector3<Real>& P2);
    Sphere3<Real> ExactSphere4 (const Vector3<Real>& P0,
        const Vector3<Real>& P1, const Vector3<Real>& P2,
        const Vector3<Real>& P3);

    Sphere3<Real> UpdateSupport1 (int i, Vector3<Real>** permuted,
        Support& support);
    Sphere3<Real> UpdateSupport2 (int i, Vector3<Real>** permuted,
        Support& support);
    Sphere3<Real> UpdateSupport3 (int i, Vector3<Real>** permuted,
        Support& support);
    Sphere3<Real> UpdateSupport4 (int i, Vector3<Real>** permuted,
        Support& support);

    typedef Sphere3<Real> (MinSphere3<Real>::*UpdateFunction)(
        int, Vector3<Real>**, Support&);

    Real mEpsilon;
    UpdateFunction mUpdate[5];
};

typedef MinSphere3<float> MinSphere3f;
typedef MinSphere3<double> MinSphere3d;

}

#endif
