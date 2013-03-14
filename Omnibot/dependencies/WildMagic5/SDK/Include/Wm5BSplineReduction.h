// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINEREDUCTION_H
#define WM5BSPLINEREDUCTION_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real, typename TVector>
class WM5_MATHEMATICS_ITEM BSplineReduction
{
public:
    // The input quantity numCtrlPoints must be 2 or larger.  The caller is
    // responsible for deleting the input array ctrlPoints and the output array
    // akCtrlOut.  The degree degree of the input curve must satisfy the
    // condition 1 <= degree <= numCtrlPoints-1.  The degree of the output
    // curve is the same as that of the input curve.  The value fraction
    // must be in [0,1].  If the fraction is 1, the output curve is identical
    // to the input curve.  If the fraction is too small to produce a valid
    // number of control points, the output control quantity is chosen to be
    // outNumCtrlPoints = degree+1.

    BSplineReduction (int numCtrlPoints, const TVector* ctrlPoints,
        int degree, Real fraction, int& outNumCtrlPoints,
        TVector*& outCtrlPoints);

    ~BSplineReduction ();

private:
    Real MinSupport (int basis, int i) const;
    Real MaxSupport (int basis, int i) const;
    Real F (int basis, int i, int j, Real t);
    static Real Integrand (Real t, void* data);

    int mDegree;
    int mQuantity[2];
    int mNumKnots[2];  // N+D+2
    Real* mKnot[2];

    // For the integration-based least-squares fitting.
    int mBasis[2], mIndex[2];
};

typedef BSplineReduction<float,Vector2f> BSplineReduction2f;
typedef BSplineReduction<double,Vector2d> BSplineReduction2d;
typedef BSplineReduction<float,Vector3f> BSplineReduction3f;
typedef BSplineReduction<double,Vector3d> BSplineReduction3d;

}

#endif
