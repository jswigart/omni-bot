// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2010/10/01)

#ifndef WM5APPRPOLYNOMIALFIT2_H
#define WM5APPRPOLYNOMIALFIT2_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

// Fit the data with a polynomial of the form
//     w = sum_{i=0}^{n-1} c[i]*x^{p[i]}
// where p[i] are distinct nonnegative powers provided by the caller.  A
// least-squares fitting algorithm is used, but the input data is first
// mapped to (x,w) in [-1,1]^2 for numerical robustness.

template <typename Real>
class WM5_MATHEMATICS_ITEM PolynomialFit2
{
public:
    // Construction and destruction.  The constructor is for data of the form
    // (x[i],w[i]), where 0 <= i < numSamples.
    PolynomialFit2 (int numSamples, const Real* xSamples,
        const Real* wSamples, int numPowers, const int* powers);

    virtual ~PolynomialFit2 ();

    // This is a function class that returns 'true' iff the linear system
    // solver was successful.  If it is not successful, then the polynomial
    // evaluation is invalid and always returns zero.
    operator bool () const;

    // Member access.
    Real GetXMin () const;
    Real GetXMax () const;
    Real GetWMin () const;
    Real GetWMax () const;

    // Evaluation of the fitted polynomial.  A derived class may override this
    // to implement an efficient method based on knowledge about the specific
    // powers passed to the constructor.
    virtual Real operator() (Real x) const;

protected:
    // Support for construction.
    void InitializePowers (int numPowers, const int* powers);
    void TransformToUnit (int numSamples, const Real* srcSamples[2],
        Real* trgSamples[2]);
    void DoLeastSquaresFit (int numSamples, Real* trgSamples[2]);

    int mNumPowers;
    int* mPowers;
    int mMaxXPower;
    mutable Real* mXPowers;

    Real mMin[2], mMax[2], mScale[2], mInvTwoWScale;
    Real* mCoefficients;
    bool mSolved;
};

}

#endif
