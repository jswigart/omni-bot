// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2010/10/01)

#ifndef WM5APPRPOLYNOMIALFIT3_H
#define WM5APPRPOLYNOMIALFIT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

// Fit the data with a polynomial of the form
//     w = sum_{i=0}^{n-1} c[i]*x^{p[i]}*y^{q[i]}
// where <p[i],q[i]> are distinct pairs of nonnegative powers provided by the
// caller.  A least-squares fitting algorithm is used, but the input data is
// first mapped to (x,y,w) in [-1,1]^3 for numerical robustness.

template <typename Real>
class WM5_MATHEMATICS_ITEM PolynomialFit3
{
public:
    // Construction and destruction.  The first constructor is for data of the
    // form (x[i],y[i],w[i]), where 0 <= i < numSamples.  The second
    // constructor is for gridded data of the form (x[i],y[j],w[k]), where
    // 0 <= i < numXSamples, 0 <= j < numYSamples, and k = i + numXSamples*j.
    PolynomialFit3 (int numSamples, const Real* xSamples,
        const Real* ySamples, const Real* wSamples, int numPowers,
        const Tuple<2,int>* powers);

    PolynomialFit3 (int numXSamples, int numYSamples, const Real* xSamples,
        const Real* ySamples, const Real* wSamples, int numPowers,
        const Tuple<2,int>* powers);

    virtual ~PolynomialFit3 ();

    // This is a function class that returns 'true' iff the linear system
    // solver was successful.  If it is not successful, then the polynomial
    // evaluation is invalid and always returns zero.
    operator bool () const;

    // Member access.
    Real GetXMin () const;
    Real GetXMax () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetWMin () const;
    Real GetWMax () const;

    // Evaluation of the fitted polynomial.  A derived class may override this
    // to implement an efficient method based on knowledge about the specific
    // powers passed to the constructor.
    virtual Real operator() (Real x, Real y) const;

protected:
    // Support for construction.
    void InitializePowers (int numPowers, const Tuple<2,int>* powers);
    void TransformToUnit (int numSamples, const Real* srcSamples[3],
        Real* trgSamples[3]);
    void DoLeastSquaresFit (int numSamples, Real* trgSamples[3]);

    int mNumPowers;
    Tuple<2,int>* mPowers;
    int mMaxXPower, mMaxYPower;
    mutable Real* mXPowers;
    mutable Real* mYPowers;

    Real mMin[3], mMax[3], mScale[3], mInvTwoWScale;
    Real* mCoefficients;
    bool mSolved;
};

}

#endif
