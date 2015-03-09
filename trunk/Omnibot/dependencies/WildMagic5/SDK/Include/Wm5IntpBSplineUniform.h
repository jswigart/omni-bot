// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPBSPLINEUNIFORM_H
#define WM5INTPBSPLINEUNIFORM_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpBSplineUniform
{
public:
    // Construction and destruction.  IntpBSplineUniform accepts
    // responsibility for deleting the input array data.  The input array
    // dim is copied.
    IntpBSplineUniform (int dims, int degree, const int* dim, Real* data);
    virtual ~IntpBSplineUniform ();

    int GetDimension () const;
    int GetDegree () const;
    Real GetDomainMin (int i) const;
    Real GetDomainMax (int i) const;
    int GetGridMin (int i) const;
    int GetGridMax (int i) const;

    // spline evaluation for function interpolation (no derivatives)
    virtual Real operator() (Real* X) = 0;

    // spline evaluation, derivative counts given in dx[]
    virtual Real operator() (int* dx, Real* X) = 0;

protected:
    int mDims;       // N, number of dimensions
    int mDegree;     // D, degree of polynomial spline
    int mDp1;        // D+1
    int mDp1ToN;     // power(D+1,N)
    int mDp1To2N;    // power(D+1,2N)
    int* mDim;       // dimension sizes dim[0] through dim[N-1]
    Real* mData;     // N-dimensional array of data
    Real* mDomMin;   // minimum allowed value of spline input vector
    Real* mDomMax;   // maximum allowed value of spline input vector
    int* mGridMin;   // minimum allowed value for current local grid
    int* mGridMax;   // maximum allowed value for current local grid
    int* mBase;      // base indices for grid of local control points
    int* mOldBase;   // old base indices for grid of local control points
    Real** mMatrix;  // (D+1)x(D+1) blending matrix
    Real* mCache;    // cache for subblock of data
    Real* mInter;    // intermediate product of data with blending matrix
    Real** mPoly;    // poly[N][D+1], store polynomials and derivatives
    Real** mCoeff;   // coefficients for polynomial construction
    Real* mProduct;  // outer tensor product of m with itself N times
    int* mSkip;      // for skipping zero values of mtensor

    Real (*mEvaluateCallback)(int);

    virtual void EvaluateUnknownData () = 0;
    virtual void ComputeIntermediate () = 0;
    void SetPolynomial (int order, Real diff, Real* poly);

    static int Choose (int n, int k);  // n choose k
    static Real** BlendMatrix (int degree);
};

typedef IntpBSplineUniform<float> IntpBSplineUniformf;
typedef IntpBSplineUniform<double> IntpBSplineUniformd;

}

#endif
