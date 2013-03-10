// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineGeodesic.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BSplineGeodesic<Real>::BSplineGeodesic (const BSplineRectangle<Real>& spline)
    :
    RiemannianGeodesic<Real>(2),
    mSpline(&spline)
{
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineGeodesic<Real>::~BSplineGeodesic ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineGeodesic<Real>::ComputeMetric (const GVector<Real>& point)
{
    Vector3<Real> der0 = mSpline->PU(point[0], point[1]);
    Vector3<Real> der1 = mSpline->PV(point[0], point[1]);

    mMetric[0][0] = der0.Dot(der0);
    mMetric[0][1] = der0.Dot(der1);
    mMetric[1][0] = mMetric[0][1];
    mMetric[1][1] = der1.Dot(der1);
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineGeodesic<Real>::ComputeChristoffel1 (const GVector<Real>& point)
{
    Vector3<Real> der0 = mSpline->PU(point[0], point[1]);
    Vector3<Real> der1 = mSpline->PV(point[0], point[1]);
    Vector3<Real> der00 = mSpline->PUU(point[0], point[1]);
    Vector3<Real> der01 = mSpline->PUV(point[0], point[1]);
    Vector3<Real> der11 = mSpline->PVV(point[0], point[1]);

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
class BSplineGeodesic<float>;

template WM5_MATHEMATICS_ITEM
class BSplineGeodesic<double>;
}
//----------------------------------------------------------------------------
