// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5RiemannianGeodesic.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
RiemannianGeodesic<Real>::RiemannianGeodesic (int dimension)
    :
    mMetric(dimension, dimension),
    mMetricInverse(dimension, dimension)
{
    assertion(dimension >= 2, "Dimension must be at least 2\n");
    mDimension = 2;

    mChristoffel1 = new1<GMatrix<Real> >(mDimension);
    mChristoffel2 = new1<GMatrix<Real> >(mDimension);
    mMetricDerivative = new1<GMatrix<Real> >(mDimension);
    for (int i = 0; i < mDimension; ++i)
    {
        mChristoffel1[i].SetSize(mDimension, mDimension);
        mChristoffel2[i].SetSize(mDimension, mDimension);
        mMetricDerivative[i].SetSize(mDimension, mDimension);
    }
    mMetricInverseExists = true;

    IntegralSamples = 16;
    SearchSamples = 32;
    DerivativeStep = (Real)1e-04;
    Subdivisions = 7;
    Refinements = 8;
    SearchRadius = (Real)1;

    mIntegralStep = ((Real)1)/(Real)(IntegralSamples - 1);
    mSearchStep = ((Real)1)/(Real)SearchSamples;
    mDerivativeFactor = ((Real)0.5)/DerivativeStep;

    RefineCallback = 0;
    mSubdivide = 0;
    mRefine = 0;
    mCurrentQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
RiemannianGeodesic<Real>::~RiemannianGeodesic ()
{
    delete1(mChristoffel1);
    delete1(mChristoffel2);
    delete1(mMetricDerivative);
}
//----------------------------------------------------------------------------
template <typename Real>
int RiemannianGeodesic<Real>::GetDimension () const
{
    return mDimension;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RiemannianGeodesic<Real>::ComputeSegmentLength (
    const GVector<Real>& point0, const GVector<Real>& point1)
{
    // The Trapezoid Rule is used for integration of the length integral.

    GVector<Real> diff = point1 - point0;
    GVector<Real> temp(mDimension);

    // Evaluate the integrand at point0.
    ComputeMetric(point0);
    Real qForm = mMetric.QForm(diff, diff);
    assertion(qForm > (Real)0, "Unexpected condition\n");
    Real length = Math<Real>::Sqrt(qForm);

    // Evaluate the integrand at point1.
    ComputeMetric(point1);
    qForm = mMetric.QForm(diff, diff);
    assertion(qForm > (Real)0, "Unexpected condition\n");
    length += Math<Real>::Sqrt(qForm);
    length *= (Real)0.5;

    int imax = IntegralSamples - 2;
    for (int i = 1; i <= imax; ++i)
    {
        // Evaluate the integrand at point0+t*(point1-point0).
        Real t = mIntegralStep * (Real)i;
        temp = point0 + t*diff;
        ComputeMetric(temp);
        qForm = mMetric.QForm(diff,diff);
        assertion(qForm > (Real)0, "Unexpected condition\n");
        length += Math<Real>::Sqrt(qForm);
    }
    length *= mIntegralStep;

    return length;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RiemannianGeodesic<Real>::ComputeTotalLength (int quantity,
    const GVector<Real>* path)
{
    assertion(quantity >= 2, "Path must have at least two points\n");

    Real length = ComputeSegmentLength(path[0], path[1]);
    for (int i = 1; i <= quantity - 2; ++i)
    {
        length += ComputeSegmentLength(path[i], path[i + 1]);
    }
    return length;
}
//----------------------------------------------------------------------------
template <typename Real>
void RiemannianGeodesic<Real>::ComputeGeodesic (const GVector<Real>& point0,
    const GVector<Real>& point1, int& quantity, GVector<Real>*& path)
{
    assertion(Subdivisions < 32, "Exceeds maximum iterations\n");
    quantity = (1 << Subdivisions) + 1;

    path = new1<GVector<Real> >(quantity);
    int i;
    for (i = 0; i < quantity; ++i)
    {
        path[i].SetSize(mDimension);
    }

    mCurrentQuantity = 2;
    path[0] = point0;
    path[1] = point1;

    for (mSubdivide = 1; mSubdivide <= Subdivisions; ++mSubdivide)
    {
        // A subdivision essentially doubles the number of points.
        int newQuantity = 2*mCurrentQuantity - 1;
        assertion(newQuantity <= quantity, "Unexpected condition.\n");

        // Copy the old points so that there are slots for the midpoints
        // during the subdivision, the slots interleaved between the old
        // points.
        for (i = mCurrentQuantity-1; i > 0; --i)
        {
            path[2*i] = path[i];
        }

        // Subdivide the polyline.
        for (i = 0; i <= mCurrentQuantity-2; ++i)
        {
            Subdivide(path[2*i], path[2*i+1], path[2*i+2]);
        }

        mCurrentQuantity = newQuantity;

        // Refine the current polyline vertices.
        for (mRefine = 1; mRefine <= Refinements; ++mRefine)
        {
            for (i = 1; i <= mCurrentQuantity-2; ++i)
            {
                Refine(path[i-1], path[i], path[i+1]);
            }
        }
    }

    assertion(mCurrentQuantity == quantity, "Unexpected condition\n");
    mSubdivide = 0;
    mRefine = 0;
    mCurrentQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool RiemannianGeodesic<Real>::Subdivide (const GVector<Real>& end0,
    GVector<Real>& mid, const GVector<Real>& end1)
{
    mid = ((Real)0.5)*(end0 + end1);
    RefineCallbackFunction save = RefineCallback;
    RefineCallback = 0;
    bool changed = Refine(end0, mid, end1);
    RefineCallback = save;
    return changed;
}
//----------------------------------------------------------------------------
template <typename Real>
bool RiemannianGeodesic<Real>::Refine (const GVector<Real>& end0,
     GVector<Real>& mid, const GVector<Real>& end1)
{
    // Estimate the gradient vector for F(m) = Length(e0,m) + Length(m,e1).
    GVector<Real> temp = mid, gradient(mDimension);
    int i;
    for (i = 0; i < mDimension; ++i)
    {
        temp[i] = mid[i] + DerivativeStep;
        gradient[i]  = ComputeSegmentLength(end0, temp);
        gradient[i] += ComputeSegmentLength(temp, end1);

        temp[i] = mid[i] - DerivativeStep;
        gradient[i] -= ComputeSegmentLength(end0, temp);
        gradient[i] -= ComputeSegmentLength(temp, end1);

        temp[i] = mid[i];
        gradient[i] *= mDerivativeFactor;
    }

    // Compute the length sum for the current midpoint.
    Real length0 = ComputeSegmentLength(end0, mid);
    Real length1 = ComputeSegmentLength(mid, end1);
    Real oldLength = length0 + length1;

    Real tRay, newLength;
    GVector<Real> PRay(mDimension);

    Real multiplier = mSearchStep*SearchRadius;
    Real minLength = oldLength;
    GVector<Real> minPoint = mid;
    int minIndex = 0;
    for (i = -SearchSamples; i <= SearchSamples; ++i)
    {
        tRay = multiplier*(Real)i;
        PRay = mid - tRay*gradient;
        length0 = ComputeSegmentLength(end0, PRay);
        length1 = ComputeSegmentLength(end1, PRay);
        newLength = length0 + length1;
        if (newLength < minLength)
        {
            minLength = newLength;
            minPoint = PRay;
            minIndex = i;
        }
    }

    mid = minPoint;

    if (RefineCallback)
    {
        RefineCallback();
    }

    return minLength < oldLength;
}
//----------------------------------------------------------------------------
template <typename Real>
int RiemannianGeodesic<Real>::GetSubdivisionStep () const
{
    return mSubdivide;
}
//----------------------------------------------------------------------------
template <typename Real>
int RiemannianGeodesic<Real>::GetRefinementStep () const
{
    return mRefine;
}
//----------------------------------------------------------------------------
template <typename Real>
int RiemannianGeodesic<Real>::GetCurrentQuantity () const
{
    return mCurrentQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RiemannianGeodesic<Real>::ComputeSegmentCurvature (
    const GVector<Real>& point0, const GVector<Real>& point1)
{
    // The Trapezoid Rule is used for integration of the curvature integral.

    GVector<Real> diff = point1 - point0;
    GVector<Real> temp(mDimension);

    // Evaluate the integrand at point0.
    Real curvature = ComputeIntegrand(point0, diff);

    // Evaluate the integrand at point1.
    curvature += ComputeIntegrand(point1, diff);
    curvature *= (Real)0.5;

    int imax = IntegralSamples - 2;
    for (int i = 1; i <= imax; ++i)
    {
        // Evaluate the integrand at point0+t*(point1-point0).
        Real t = mIntegralStep * (Real)i;
        temp = point0 + t*diff;
        curvature += ComputeIntegrand(temp,diff);
    }
    curvature *= mIntegralStep;

    return curvature;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RiemannianGeodesic<Real>::ComputeTotalCurvature (int quantity,
    const GVector<Real>* path)
{
    assertion(quantity >= 2, "Path must have at least two points\n");

    Real curvature = ComputeSegmentCurvature(path[0], path[1]);
    for (int i = 1; i <= quantity - 2; ++i)
    {
        curvature += ComputeSegmentCurvature(path[i], path[i+1]);
    }
    return curvature;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RiemannianGeodesic<Real>::ComputeIntegrand (const GVector<Real>& pos,
    const GVector<Real>& der)
{
    ComputeMetric(pos);
    ComputeChristoffel1(pos);
    ComputeMetricInverse();
    ComputeChristoffel2();

    // g_{ij}*der_{i}*der_{j}
    Real qForm0 = mMetric.QForm(der, der);
    assertion(qForm0 > (Real)0, "Unexpected condition\n");

    // gamma_{kij}*der_{k}*der_{i}*der_{j}
    GMatrix<Real> mat(mDimension, mDimension);
    int k;
    for (k = 0; k < mDimension; ++k)
    {
        mat += der[k]*mChristoffel1[k];
    }
    Real qForm1 = mat.QForm(der, der);

    Real ratio = -qForm1/qForm0;

    // Compute the acceleration.
    GVector<Real> acc = ratio*der;
    for (k = 0; k < mDimension; ++k)
    {
        acc[k] += mChristoffel2[k].QForm(der, der);
    }

    // Compute the curvature.
    Real curvature = Math<Real>::Sqrt(mMetric.QForm(acc, acc));
    return curvature;
}
//----------------------------------------------------------------------------
template <typename Real>
bool RiemannianGeodesic<Real>::ComputeMetricInverse ()
{
    mMetricInverseExists = LinearSystem<Real>().Inverse(mMetric,
        mMetricInverse);

    return mMetricInverseExists;
}
//----------------------------------------------------------------------------
template <typename Real>
void RiemannianGeodesic<Real>::ComputeMetricDerivative ()
{
    for (int derivative = 0; derivative < mDimension; ++derivative)
    {
        for (int i0 = 0; i0 < mDimension; ++i0)
        {
            for (int i1 = 0; i1 < mDimension; ++i1)
            {
                mMetricDerivative[derivative][i0][i1] =
                    mChristoffel1[derivative][i0][i1] +
                    mChristoffel1[derivative][i1][i0];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool RiemannianGeodesic<Real>::ComputeChristoffel2 ()
{
    for (int i2 = 0; i2 < mDimension; ++i2)
    {
        for (int i0 = 0; i0 < mDimension; ++i0)
        {
            for (int i1 = 0; i1 < mDimension; ++i1)
            {
                Real fValue = (Real)0.0;
                for (int j = 0; j < mDimension; ++j)
                {
                    fValue += mMetricInverse[i2][j] *
                        mChristoffel1[j][i0][i1];
                }
                mChristoffel2[i2][i0][i1] = fValue;
            }
        }
    }

    return mMetricInverseExists;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class RiemannianGeodesic<float>;

template WM5_MATHEMATICS_ITEM
class RiemannianGeodesic<double>;
//----------------------------------------------------------------------------
}
