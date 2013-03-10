// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5EllipsoidGeodesic.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
EllipsoidGeodesic<Real>::EllipsoidGeodesic (Real xExtent, Real yExtent,
    Real zExtent)
    :
    RiemannianGeodesic<Real>(2)
{
    mXExtent = xExtent;
    mYExtent = yExtent;
    mZExtent = zExtent;
}
//----------------------------------------------------------------------------
template <typename Real>
EllipsoidGeodesic<Real>::~EllipsoidGeodesic ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> EllipsoidGeodesic<Real>::ComputePosition (
    const GVector<Real>& point)
{
    Real cos0 = Math<Real>::Cos(point[0]);
    Real sin0 = Math<Real>::Sin(point[0]);
    Real cos1 = Math<Real>::Cos(point[1]);
    Real sin1 = Math<Real>::Sin(point[1]);

    return Vector3<Real>(mXExtent*cos0*sin1, mYExtent*sin0*sin1,
        mZExtent*cos1);
}
//----------------------------------------------------------------------------
template <typename Real>
void EllipsoidGeodesic<Real>::ComputeMetric (const GVector<Real>& point)
{
    Real cos0 = Math<Real>::Cos(point[0]);
    Real sin0 = Math<Real>::Sin(point[0]);
    Real cos1 = Math<Real>::Cos(point[1]);
    Real sin1 = Math<Real>::Sin(point[1]);

    Vector3<Real> der0(-mXExtent*sin0*sin1, mYExtent*cos0*sin1, (Real)0);
    Vector3<Real> der1(mXExtent*cos0*cos1,  mYExtent*sin0*cos1,
        -mZExtent*sin1);

    mMetric[0][0] = der0.Dot(der0);
    mMetric[0][1] = der0.Dot(der1);
    mMetric[1][0] = mMetric[0][1];
    mMetric[1][1] = der1.Dot(der1);
}
//----------------------------------------------------------------------------
template <typename Real>
void EllipsoidGeodesic<Real>::ComputeChristoffel1 (
    const GVector<Real>& point)
{
    Real cos0 = Math<Real>::Cos(point[0]);
    Real sin0 = Math<Real>::Sin(point[0]);
    Real cos1 = Math<Real>::Cos(point[1]);
    Real sin1 = Math<Real>::Sin(point[1]);

    Vector3<Real> der0(-mXExtent*sin0*sin1, mYExtent*cos0*sin1, (Real)0);

    Vector3<Real> der1(mXExtent*cos0*cos1,  mYExtent*sin0*cos1,
        -mZExtent*sin1);

    Vector3<Real> der00(-mXExtent*cos0*sin1, -mYExtent*sin0*sin1, (Real)0);

    Vector3<Real> der01(-mXExtent*sin0*cos1, mYExtent*cos0*cos1, (Real)0);

    Vector3<Real> der11(-mXExtent*cos0*sin1, -mYExtent*sin0*sin1,
        -mZExtent*cos1);

    mChristoffel1[0][0][0] = der00.Dot(der0);
    mChristoffel1[0][0][1] = der01.Dot(der0);
    mChristoffel1[0][1][0] = mChristoffel1[0][0][1];
    mChristoffel1[0][1][1] = der11.Dot(der0);

    mChristoffel1[1][0][0] = der00.Dot(der1);
    mChristoffel1[1][0][1] = der01.Dot(der1);
    mChristoffel1[1][1][0] = mChristoffel1[1][0][1];
    mChristoffel1[1][1][1] = der11.Dot(der1);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class EllipsoidGeodesic<float>;

template WM5_MATHEMATICS_ITEM
class EllipsoidGeodesic<double>;
}
//----------------------------------------------------------------------------
