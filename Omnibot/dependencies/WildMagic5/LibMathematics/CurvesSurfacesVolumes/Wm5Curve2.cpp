// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Curve2.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Curve2<Real>::Curve2 (Real tmin, Real tmax)
{
    mTMin = tmin;
    mTMax = tmax;
}
//----------------------------------------------------------------------------
template <typename Real>
Curve2<Real>::~Curve2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve2<Real>::GetMinTime () const
{
    return mTMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve2<Real>::GetMaxTime () const
{
    return mTMax;
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve2<Real>::SetTimeInterval (Real tmin, Real tmax)
{
    assertion(tmin < tmax, "Invalid time interval\n");
    mTMin = tmin;
    mTMax = tmax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve2<Real>::GetSpeed (Real t) const
{
    Vector2<Real> velocity = GetFirstDerivative(t);
    Real speed = velocity.Length();
    return speed;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve2<Real>::GetTotalLength () const
{
    return GetLength(mTMin, mTMax);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> Curve2<Real>::GetTangent (Real t) const
{
    Vector2<Real> velocity = GetFirstDerivative(t);
    velocity.Normalize();
    return velocity;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> Curve2<Real>::GetNormal (Real t) const
{
    Vector2<Real> tangent = GetFirstDerivative(t);
    tangent.Normalize();
    Vector2<Real> normal = tangent.Perp();
    return normal;
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve2<Real>::GetFrame (Real t, Vector2<Real>& position,
    Vector2<Real>& tangent, Vector2<Real>& normal) const
{
    position = GetPosition(t);
    tangent = GetFirstDerivative(t);
    tangent.Normalize();
    normal = tangent.Perp();
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve2<Real>::GetCurvature (Real t) const
{
    Vector2<Real> der1 = GetFirstDerivative(t);
    Vector2<Real> der2 = GetSecondDerivative(t);
    Real speedSqr = der1.SquaredLength();

    if (speedSqr >= Math<Real>::ZERO_TOLERANCE)
    {
        Real numer = der1.DotPerp(der2);
        Real denom = Math<Real>::Pow(speedSqr, (Real)1.5);
        return numer/denom;
    }
    else
    {
        // Curvature is indeterminate, just return 0.
        return (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve2<Real>::SubdivideByTime (int numPoints,
    Vector2<Real>*& points) const
{
    assertion(numPoints >= 2, "Subdivision requires at least two points\n");
    points = new1<Vector2<Real> >(numPoints);

    Real delta = (mTMax - mTMin)/(numPoints - 1);

    for (int i = 0; i < numPoints; ++i)
    {
        Real t = mTMin + delta*i;
        points[i] = GetPosition(t);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve2<Real>::SubdivideByLength (int numPoints,
    Vector2<Real>*& points) const
{
    assertion(numPoints >= 2, "Subdivision requires at least two points\n");
    points = new1<Vector2<Real> >(numPoints);

    Real delta = GetTotalLength()/(numPoints - 1);

    for (int i = 0; i < numPoints; ++i)
    {
        Real length = delta*i;
        Real t = GetTime(length);
        points[i] = GetPosition(t);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Curve2<float>;

template WM5_MATHEMATICS_ITEM
class Curve2<double>;
//----------------------------------------------------------------------------
}
