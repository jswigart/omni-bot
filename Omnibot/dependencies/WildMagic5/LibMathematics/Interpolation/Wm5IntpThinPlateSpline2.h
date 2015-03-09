// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/05/22)

#ifndef WM5INTPTHINPLATESPLINE2_H
#define WM5INTPTHINPLATESPLINE2_H

// WARNING.  The implementation allows you to transform the inputs (x,y) to
// the unit square and perform the interpolation in that space.  The idea is
// to keep the floating-point numbers to order 1 for numerical stability of
// the algorithm.  The classical thin-plate spline algorithm does not include
// this transformation.  The interpolation is invariant to translations and
// rotations of (x,y) but not to scaling.

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpThinPlateSpline2
{
public:
    // Construction and destruction.  Data points are (x,y,f(x,y)).  The
    // smoothing parameter must be nonnegative.  If you want the class to
    // delete the input arrays during destruction, set owner to 'true';
    // otherwise, you own the arrays and must delete them yourself.
    IntpThinPlateSpline2 (int quantity, Real* X, Real* Y,
        Real* F, Real smooth, bool owner, bool transformToUnitSquare);

    ~IntpThinPlateSpline2 ();

    // Check this after the constructor call to see if the thin plate spline
    // coefficients were successfully computed.  If so, then calls to
    // operator()(Real,Real) will work properly.
    bool IsInitialized () const;

    // Member access.  There are 'quantity' A[] coefficients and 3 B[]
    // coefficients.  The A[] coefficients are associated with the Green's
    // functions G(x,y,*) and the B[] coefficients are associated with the
    // affine term (linear polynomial B[0]+B[1]*x+B[2]*y).
    const Real* GetACoefficients () const;
    const Real* GetBCoefficients () const;
    Real GetSmooth () const;  // The smoothing parameter from the constructor.

    // Compute the functional value a^T*M*a when lambda is zero or
    // lambda*w^T*(M+lambda*I)*w when lambda is positive.  See the thin-plate
    // splines PDF for a description of these quantities.
    Real ComputeFunctional () const;

    // Evaluate the thin plate spline interpolator.  If IsInitialized()
    // returns 'false', this operator will always return MAX_REAL.
    Real operator() (Real x, Real y);

    // Kernel(t) = t^2*log(t^2)
    static Real Kernel (Real t);

private:
    bool mInitialized;
    int mQuantity;

    // Input data.
    Real* mX;
    Real* mY;
    Real mSmooth;

    // Thin plate spline coefficients.
    Real* mA;
    Real mB[3];

    // Extent of input data.
    Real mXMin, mXMax, mXInvRange;
    Real mYMin, mYMax, mYInvRange;
};

typedef IntpThinPlateSpline2<float> IntpThinPlateSpline2f;
typedef IntpThinPlateSpline2<double> IntpThinPlateSpline2d;

}

#endif
