// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5INTPBSPLINEUNIFORMN_H
#define WM5INTPBSPLINEUNIFORMN_H

#include "Wm5MathematicsLIB.h"
#include "Wm5IntpBSplineUniform.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpBSplineUniformN
    : public IntpBSplineUniform<Real>
{
public:
    // Construction and destruction.  IntpBSplineUniformN accepts
    // responsibility for deleting the input array data.  The input array
    // dim is copied.
    IntpBSplineUniformN (int dims, int degree, const int* dim, Real* data);
    virtual ~IntpBSplineUniformN ();

    int Index (int* I) const;

    // Spline evaluation for function interpolation (no derivatives).
    virtual Real operator() (Real* X);

    // Spline evaluation, derivative counts given in dx[].
    virtual Real operator() (int* dx, Real* X);

private:
    using IntpBSplineUniform<Real>::mDims;
    using IntpBSplineUniform<Real>::mDegree;
    using IntpBSplineUniform<Real>::mDp1;
    using IntpBSplineUniform<Real>::mDp1ToN;
    using IntpBSplineUniform<Real>::mDim;
    using IntpBSplineUniform<Real>::mData;
    using IntpBSplineUniform<Real>::mGridMin;
    using IntpBSplineUniform<Real>::mGridMax;
    using IntpBSplineUniform<Real>::mBase;
    using IntpBSplineUniform<Real>::mOldBase;
    using IntpBSplineUniform<Real>::mMatrix;
    using IntpBSplineUniform<Real>::mCache;
    using IntpBSplineUniform<Real>::mInter;
    using IntpBSplineUniform<Real>::mPoly;
    using IntpBSplineUniform<Real>::mProduct;
    using IntpBSplineUniform<Real>::mSkip;
    using IntpBSplineUniform<Real>::mEvaluateCallback;
    using IntpBSplineUniform<Real>::SetPolynomial;

    int* mEvI;
    int* mCiLoop;
    int* mCiDelta;
    int* mOpI;
    int* mOpJ;
    int* mOpDelta;

    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

typedef IntpBSplineUniformN<float> IntpBSplineUniformNf;
typedef IntpBSplineUniformN<double> IntpBSplineUniformNd;

}

#endif
