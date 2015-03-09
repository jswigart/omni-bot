// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINEFITBASIS_H
#define WM5BSPLINEFITBASIS_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BSplineFitBasis
{
public:
    // Construction and destruction.  This class is only for open uniform
    // B-spline basis functions.  The input is the number of control points
    // for a B-spline curve using this basis and the degree of that curve.
    BSplineFitBasis (int quantity, int degree);
    ~BSplineFitBasis ();

    // Data member access.
    int GetQuantity () const;
    int GetDegree () const;

    // Evaluate the basis functions.  This function fills in the values
    // returned by GetValue(i) for 0 <= i <= degree.  The return indices iMin
    // and iMax are relative to the array of control points.  The GetValue(i)
    // are the coefficients for the control points ctrl[iMin] throught
    // ctrl[iMax] in the curve evaluation (i.e. the curve has local control).
    void Compute (Real t, int& imin, int& imax) const;
    Real GetValue (int i) const;

private:
    // The number of control points and degree for the curve.
    int mQuantity, mDegree;

    // The storage for knots and basis evaluation.
    mutable Real* mValue;  // mValue[0..degree]
    mutable Real* mKnot;   // mKnot[2*degree]
};

typedef BSplineFitBasis<float> BSplineFitBasisf;
typedef BSplineFitBasis<double> BSplineFitBasisd;

}

#endif
