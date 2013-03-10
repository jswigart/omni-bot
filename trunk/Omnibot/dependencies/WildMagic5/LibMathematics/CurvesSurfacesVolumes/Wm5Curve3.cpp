// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Curve3.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Curve3<Real>::Curve3 (Real tmin, Real tmax)
{
    mTMin = tmin;
    mTMax = tmax;
}
//----------------------------------------------------------------------------
template <typename Real>
Curve3<Real>::~Curve3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve3<Real>::GetMinTime () const
{
    return mTMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve3<Real>::GetMaxTime () const
{
    return mTMax;
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve3<Real>::SetTimeInterval (Real tmin, Real tmax)
{
    assertion(tmin < tmax, "Invalid time interval\n");
    mTMin = tmin;
    mTMax = tmax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve3<Real>::GetSpeed (Real t) const
{
    Vector3<Real> velocity = GetFirstDerivative(t);
    Real speed = velocity.Length();
    return speed;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve3<Real>::GetTotalLength () const
{
    return GetLength(mTMin, mTMax);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Curve3<Real>::GetTangent (Real t) const
{
    Vector3<Real> velocity = GetFirstDerivative(t);
    velocity.Normalize();
    return velocity;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Curve3<Real>::GetNormal (Real t) const
{
    Vector3<Real> velocity = GetFirstDerivative(t);
    Vector3<Real> acceleration = GetSecondDerivative(t);
    Real VDotV = velocity.Dot(velocity);
    Real VDotA = velocity.Dot(acceleration);
    Vector3<Real> normal = VDotV*acceleration - VDotA*velocity;
    normal.Normalize();
    return normal;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Curve3<Real>::GetBinormal (Real t) const
{
    Vector3<Real> velocity = GetFirstDerivative(t);
    Vector3<Real> acceleration = GetSecondDerivative(t);
    Real VDotV = velocity.Dot(velocity);
    Real VDotA = velocity.Dot(acceleration);
    Vector3<Real> normal = VDotV*acceleration - VDotA*velocity;
    normal.Normalize();
    velocity.Normalize();
    Vector3<Real> binormal = velocity.Cross(normal);
    return binormal;
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve3<Real>::GetFrame (Real t, Vector3<Real>& position,
    Vector3<Real>& tangent, Vector3<Real>& normal, Vector3<Real>& binormal)
    const
{
    position = GetPosition(t);
    Vector3<Real> velocity = GetFirstDerivative(t);
    Vector3<Real> acceleration = GetSecondDerivative(t);
    Real VDotV = velocity.Dot(velocity);
    Real VDotA = velocity.Dot(acceleration);
    normal = VDotV*acceleration - VDotA*velocity;
    normal.Normalize();
    tangent = velocity;
    tangent.Normalize();
    binormal = tangent.Cross(normal);
}
//----------------------------------------------------------------------------
template <typename Real>
Real Curve3<Real>::GetCurvature (Real t) const
{
    Vector3<Real> velocity = GetFirstDerivative(t);
    Real speedSqr = velocity.SquaredLength();

    if (speedSqr >= Math<Real>::ZERO_TOLERANCE)
    {
        Vector3<Real> acceleration = GetSecondDerivative(t);
        Vector3<Real> cross = velocity.Cross(acceleration);
        Real numer = cross.Length();
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
Real Curve3<Real>::GetTorsion (Real t) const
{
    Vector3<Real> velocity = GetFirstDerivative(t);
    Vector3<Real> acceleration = GetSecondDerivative(t);
    Vector3<Real> cross = velocity.Cross(acceleration);
    Real denom = cross.SquaredLength();

    if (denom >= Math<Real>::ZERO_TOLERANCE)
    {
        Vector3<Real> jerk = GetThirdDerivative(t);
        Real numer = cross.Dot(jerk);
        return numer/denom;
    }
    else
    {
        // Torsion is indeterminate, just return 0.
        return (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve3<Real>::SubdivideByTime (int numPoints,
    Vector3<Real>*& points) const
{
    assertion(numPoints >= 2, "Subdivision requires at least two points\n");
    points = new1<Vector3<Real> >(numPoints);

    Real delta = (mTMax - mTMin)/(numPoints - 1);

    for (int i = 0; i < numPoints; ++i)
    {
        Real t = mTMin + delta*i;
        points[i] = GetPosition(t);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Curve3<Real>::SubdivideByLength (int numPoints,
    Vector3<Real>*& points) const
{
    assertion(numPoints >= 2, "Subdivision requires at least two points\n");
    points = new1<Vector3<Real> >(numPoints);

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
class Curve3<float>;

template WM5_MATHEMATICS_ITEM
class Curve3<double>;
//----------------------------------------------------------------------------
}
