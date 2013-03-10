// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTMINCIRCLE2_H
#define WM5CONTMINCIRCLE2_H

// Compute the minimum area circle containing the input set of points.  The
// algorithm randomly permutes the input points so that the construction
// occurs in 'expected' O(N) time.

#include "Wm5MathematicsLIB.h"
#include "Wm5Circle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM MinCircle2
{
public:
    // The epsilon value is a floating-point tolerance used for various
    // computations.
    MinCircle2 (int numPoints, const Vector2<Real>* points,
        Circle2<Real>& minimal, Real epsilon = (Real)1e-05);

private:
    // Indices of points that support current minimum area circle.
    class Support
    {
    public:
        bool Contains (int index, Vector2<Real>** points, Real epsilon);

        int Quantity;
        int Index[3];
    };

    // Test whether point P is inside circle C.
    bool Contains (const Vector2<Real>& point, const Circle2<Real>& circle,
        Real& distDiff);

    Circle2<Real> ExactCircle1 (const Vector2<Real>& P);
    Circle2<Real> ExactCircle2 (const Vector2<Real>& P0,
        const Vector2<Real>& P1);
    Circle2<Real> ExactCircle3 (const Vector2<Real>& P0,
        const Vector2<Real>& P1, const Vector2<Real>& P2);

    Circle2<Real> UpdateSupport1 (int i, Vector2<Real>** permuted,
        Support& support);
    Circle2<Real> UpdateSupport2 (int i, Vector2<Real>** permuted,
        Support& support);
    Circle2<Real> UpdateSupport3 (int i, Vector2<Real>** permuted,
        Support& support);

    typedef Circle2<Real> (MinCircle2<Real>::*UpdateFunction)(
        int, Vector2<Real>**, Support&);

    Real mEpsilon;
    UpdateFunction mUpdate[4];
};

typedef MinCircle2<float> MinCircle2f;
typedef MinCircle2<double> MinCircle2d;

}

#endif
