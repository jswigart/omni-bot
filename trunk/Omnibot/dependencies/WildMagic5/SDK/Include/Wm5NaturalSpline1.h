// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5NATURALSPLINE1_H
#define WM5NATURALSPLINE1_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM NaturalSpline1
{
public:
    // Construction and destruction.  NaturalSpline1 accepts responsibility
    // for deleting the input arrays.  Each input array must have numSamples
    // elements, where numSamples >= 2 is required.  If f(t) is the function
    // to be fit, then values[i] = f(times[i]) is the sample.  The time
    // samples must be increasing:  times[i+1] > times[i] for all i.  The
    // spline curve is referred to as S(t).  In the comments, tmin refers to
    // times[0] and tmax refers to times[numSamples-1].

    // Set free to 'true' for free splines or to 'false' for periodic
    // splines.  In the periodic case, the implementation uses f(tmin) for
    // computing coefficients.  It does not use f(tmax), which is assumed to
    // be equal to f(tmin).
    //
    // Free:  S"(tmin) = 0,  S"(tmax) = 0
    // Periodic:  S(tmin) = S(tmax), S'(tmin) = S'(tmax), S"(tmin) = S"(tmax)
    NaturalSpline1 (bool free, int numSamples, Real* times, Real* values);

    // Clamped:  S'(tmin) = slopeFirst, S'(tmax) = slopeLast
    NaturalSpline1 (int numSamples, Real* times, Real* values,
        Real slopeFirst, Real slopeLast);

    ~NaturalSpline1 ();

    // Evaluators for S(t), S'(t), S''(t), and S'''(t).
    Real GetFunction (Real t) const;
    Real GetFirstDerivative (Real t) const;
    Real GetSecondDerivative (Real t) const;
    Real GetThirdDerivative (Real t) const;

    // Access the coefficients of the polynomials.
    int GetNumSegments () const;
    const Real* GetA () const;
    const Real* GetB () const;
    const Real* GetC () const;
    const Real* GetD () const;

private:
    void CreateFreeSpline ();
    void CreateClampedSpline (Real slopeFirst, Real slopeLast);
    void CreatePeriodicSpline ();

    void GetKeyInfo (Real t, int& key, Real& dt) const;

    int mNumSamples, mNumSegments;
    Real* mTimes;

    // The cubic polynomial coefficients.  All arrays have
    // mNumSegments elements.  The i-th polynomial is
    // S_i(t) =
    //   mA[i] +
    //   mB[i]*(t - mTimes[i]) +
    //   mC[i]*(t - mTimes[i])^2 +
    //   mD[i]*(t - mTimes[i])^3
    Real* mA;
    Real* mB;
    Real* mC;
    Real* mD;
};

typedef NaturalSpline1<float> NaturalSpline1f;
typedef NaturalSpline1<double> NaturalSpline1d;

}

#endif
