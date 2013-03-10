// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5INTPBSPLINEUNIFORM3_H
#define WM5INTPBSPLINEUNIFORM3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5IntpBSplineUniform.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpBSplineUniform3
    : public IntpBSplineUniform<Real>
{
public:
    // Construction.  IntpBSplineUniform3 accepts responsibility for
    // deleting the input array data.
    IntpBSplineUniform3 (int degree, const int* dim, Real* data);

    int Index (int ix, int iy, int iz) const;

    // Spline evaluation for function interpolation (no derivatives).
    Real operator() (Real x, Real y, Real z);
    virtual Real operator() (Real* X);

    // Spline evaluation, derivative counts given in dx, dy, dz, dx[].
    Real operator() (int dx, int dy, int dz, Real x, Real y, Real z);
    virtual Real operator() (int* dx, Real* X);

private:
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

    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

typedef IntpBSplineUniform3<float> IntpBSplineUniform3f;
typedef IntpBSplineUniform3<double> IntpBSplineUniform3d;

}

#endif
