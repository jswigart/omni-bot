// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5INTPBSPLINEUNIFORM1_H
#define WM5INTPBSPLINEUNIFORM1_H

#include "Wm5MathematicsLIB.h"
#include "Wm5IntpBSplineUniform.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpBSplineUniform1
    : public IntpBSplineUniform<Real>
{
public:
    // Construction.  IntpBSplineUniform1 accepts responsibility for
    // deleting the input array data.
    IntpBSplineUniform1 (int degree, int dim, Real* data);

    // Spline evaluation for function interpolation (no derivatives).
    Real operator() (Real x);
    virtual Real operator() (Real* X);

    // Spline evaluation, derivative count given in dx and dx[].
    Real operator() (int dx, Real x);
    virtual Real operator() (int* dx, Real* X);

private:
    using IntpBSplineUniform<Real>::mDegree;
    using IntpBSplineUniform<Real>::mData;
    using IntpBSplineUniform<Real>::mGridMin;
    using IntpBSplineUniform<Real>::mGridMax;
    using IntpBSplineUniform<Real>::mBase;
    using IntpBSplineUniform<Real>::mOldBase;
    using IntpBSplineUniform<Real>::mMatrix;
    using IntpBSplineUniform<Real>::mInter;
    using IntpBSplineUniform<Real>::mPoly;
    using IntpBSplineUniform<Real>::mEvaluateCallback;
    using IntpBSplineUniform<Real>::SetPolynomial;

    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

typedef IntpBSplineUniform1<float> IntpBSplineUniform1f;
typedef IntpBSplineUniform1<double> IntpBSplineUniform1d;

}

#endif
