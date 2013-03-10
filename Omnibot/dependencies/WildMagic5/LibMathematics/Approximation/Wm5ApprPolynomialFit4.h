// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2010/10/01)

#ifndef WM5APPRPOLYNOMIALFIT4_H
#define WM5APPRPOLYNOMIALFIT4_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

// Fit the data with a polynomial of the form
//     w = sum_{i=0}^{n-1} c[i]*x^{p[i]}*y^{q[i]}*z^{r[i]}
// where <p[i],q[i],r[i]> are distinct triples of nonnegative powers provided
// by the caller.  A least-squares fitting algorithm is used, but the input
// data is first mapped to (x,y,z,w) in [-1,1]^4 for numerical robustness.

template <typename Real>
class WM5_MATHEMATICS_ITEM PolynomialFit4
{
public:
    // Construction and destruction.  The first constructor is for data of the
    // form (x[i],y[i],z[i],w[i]), where 0 <= i < numSamples.  The second
    // constructor is for gridded data of the form (x[i],y[j],z[k],w[m]),
    // where 0 <= i < numXSamples, 0 <= j < numYSamples, 0 <= k < numZSamples,
    // and m = i + numXSamples*(j + numYSamples*k).
    PolynomialFit4 (int numSamples, const Real* xSamples,
        const Real* ySamples, const Real* zSamples, const Real* wSamples,
        int numPowers, const Tuple<3,int>* powers);

    PolynomialFit4 (int numXSamples, int numYSamples, int numZSamples,
        const Real* xSamples, const Real* ySamples, const Real* zSamples,
        const Real* wSamples, int numPowers, const Tuple<3,int>* powers);

    virtual ~PolynomialFit4 ();

    // This is a function class that returns 'true' iff the linear system
    // solver was successful.  If it is not successful, then the polynomial
    // evaluation is invalid and always returns zero.
    operator bool () const;

    // Member access.
    Real GetXMin () const;
    Real GetXMax () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetZMin () const;
    Real GetZMax () const;
    Real GetWMin () const;
    Real GetWMax () const;

    // Evaluation of the fitted polynomial.  A derived class may override this
    // to implement an efficient method based on knowledge about the specific
    // powers passed to the constructor.
    virtual Real operator() (Real x, Real y, Real z) const;

protected:
    // Support for construction.
    void InitializePowers (int numPowers, const Tuple<3,int>* powers);
    void TransformToUnit (int numSamples, const Real* srcSamples[4],
        Real* trgSamples[4]);
    void DoLeastSquaresFit (int numSamples, Real* trgSamples[4]);

    int mNumPowers;
    Tuple<3,int>* mPowers;
    int mMaxXPower, mMaxYPower, mMaxZPower;
    mutable Real* mXPowers;
    mutable Real* mYPowers;
    mutable Real* mZPowers;

    Real mMin[4], mMax[4], mScale[4], mInvTwoWScale;
    Real* mCoefficients;
    bool mSolved;
};

}

#endif
