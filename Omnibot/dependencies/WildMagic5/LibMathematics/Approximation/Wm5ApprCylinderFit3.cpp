// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprCylinderFit3.h"
#include "Wm5ApprLineFit3.h"
#include "Wm5PolynomialRoots.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
CylinderFit3<Real>::CylinderFit3 (int numPoints, const Vector3<Real>* points,
    Vector3<Real>& center, Vector3<Real>& axis, Real& radius, Real& height,
    bool inputsAreInitialGuess)
{
    Real invRSqr = (Real)1;
    if (!inputsAreInitialGuess)
    {
        // Find the least-squares line that fits the data and use it as an
        // initial guess for the cylinder axis.
        Line3<Real> line = OrthogonalLineFit3(numPoints, points);
        center = line.Origin;
        axis = line.Direction;
    }

    mError = Math<Real>::MAX_REAL;
    const int iMax = 8;
    int i;
    for (i = 0; i < iMax; ++i)
    {
        mError = UpdateInvRSqr(numPoints, points, center, axis, invRSqr);
        mError = UpdateDirection(numPoints, points, center, axis, invRSqr);
        mError = UpdateCenter(numPoints, points, center, axis, invRSqr);
    }

    // Compute the radius.
    radius = Math<Real>::InvSqrt(invRSqr);

    // Project points onto fitted axis to determine extent of cylinder along
    // the axis.
    Real tMin = axis.Dot(points[0] - center);
    Real tMax = tMin;
    for (i = 1; i < numPoints; ++i)
    {
        Real t = axis.Dot(points[i] - center);
        if (t < tMin)
        {
            tMin = t;
        }
        else if (t > tMax)
        {
            tMax = t;
        }
    }

    // Compute the height.  Adjust the center to point that projects to
    // midpoint of extent.
    height = tMax - tMin;
    center += (((Real)0.5)*(tMin + tMax))*axis;
}
//----------------------------------------------------------------------------
template <typename Real>
CylinderFit3<Real>::operator Real ()
{
    return mError;
}
//----------------------------------------------------------------------------
template <typename Real>
Real CylinderFit3<Real>::UpdateInvRSqr (int numPoints,
    const Vector3<Real>* points, const Vector3<Real>& center,
    const Vector3<Real>& axis, Real& invRSqr)
{
    Real aSum = (Real)0, aaSum = (Real)0;
    for (int i = 0; i < numPoints; ++i)
    {
        Vector3<Real> delta = points[i] - center;
        Vector3<Real> deltaCrossAxis = delta.Cross(axis);
        Real len2 = deltaCrossAxis.SquaredLength();
        aSum += len2;
        aaSum += len2*len2;
    }

    invRSqr = aSum/aaSum;
    Real min = (Real)1 - invRSqr*aSum/(Real)numPoints;
    return min;
}
//----------------------------------------------------------------------------
template <typename Real>
Real CylinderFit3<Real>::UpdateDirection (int numPoints,
    const Vector3<Real>* points, const Vector3<Real>& center,
    Vector3<Real>& axis, Real& invRSqr)
{
    Real invNumPoints = ((Real)1)/(Real)numPoints;
    int i;
    Vector3<Real> delta, deltaCrossAxis, deltaCrossVDir;
    Real a, b, c;

    // Compute the direction of steepest descent.
    Vector3<Real> vDir = Vector3<Real>::ZERO;
    Real aMean = (Real)0, aaMean = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        delta = points[i] - center;
        deltaCrossAxis = delta.Cross(axis);
        a = invRSqr*deltaCrossAxis.SquaredLength() - (Real)1;
        aMean += a;
        aaMean += a*a;
        vDir.X() += a*(axis.X()*(delta.Y()*delta.Y() +
            delta.Z()*delta.Z()) - delta.X()*(axis.Y()*delta.Y() +
            axis.Z()*delta.Z()));
        vDir.Y() += a*(axis.Y()*(delta.X()*delta.X() +
            delta.Z()*delta.Z()) - delta.Y()*(axis.X()*delta.X() +
            axis.Z()*delta.Z()));
        vDir.Z() += a*(axis.Z()*(delta.X()*delta.X() + 
            delta.Y()*delta.Y()) - delta.Z()*(axis.X()*delta.X() +
            axis.Y()*delta.Y()));
    }
    aMean *= invNumPoints;
    aaMean *= invNumPoints;
    if (vDir.Normalize() < Math<Real>::ZERO_TOLERANCE)
    {
        return aaMean;
    }

    // Compute the 4th-degree polynomial for the line of steepest descent.
    Real abMean = (Real)0, acMean = (Real)0;
    Real bbMean = (Real)0, bcMean = (Real)0, ccMean = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        delta = points[i] - center;
        deltaCrossAxis = delta.Cross(axis);
        deltaCrossVDir = delta.Cross(vDir);
        a = invRSqr*deltaCrossAxis.SquaredLength() - (Real)1;
        b = invRSqr*(deltaCrossAxis.Dot(deltaCrossVDir));
        c = invRSqr*deltaCrossVDir.SquaredLength();
        abMean += a*b;
        acMean += a*c;
        bbMean += b*b;
        bcMean += b*c;
        ccMean += c*c;
    }
    abMean *= invNumPoints;
    acMean *= invNumPoints;
    bbMean *= invNumPoints;
    bcMean *= invNumPoints;
    ccMean *= invNumPoints;

    Polynomial1<Real> poly(4);
    poly[0] = aaMean;
    poly[1] = -((Real)4)*abMean;
    poly[2] = ((Real)2)*acMean + ((Real)4)*bbMean;
    poly[3] = -((Real)4)*bcMean;
    poly[4] = ccMean;

    Polynomial1<Real> derPoly = poly.GetDerivative();

    PolynomialRoots<Real> polyRoots(Math<Real>::ZERO_TOLERANCE);
    polyRoots.FindA(derPoly[0], derPoly[1], derPoly[2], derPoly[3]);
    int count = polyRoots.GetCount();
    const Real* roots = polyRoots.GetRoots();

    Real pMin = poly((Real)0);
    int iMin = -1;
    for (i = 0; i < count; ++i)
    {
        Real value = poly(roots[i]);
        if (value < pMin)
        {
            pMin = value;
            iMin = i;
        }
    }

    if (iMin >= 0)
    {
        axis -= roots[iMin]*vDir;
        Real length = axis.Normalize();
        invRSqr *= length*length;
    }

    return pMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real CylinderFit3<Real>::UpdateCenter (int numPoints,
    const Vector3<Real>* points, Vector3<Real>& center,
    const Vector3<Real>& axis, const Real& invRSqr)
{
    Real invNumPoints = ((Real)1)/(Real)numPoints;
    int i;
    Vector3<Real> delta, deltaCrossAxis, cDirCrossAxis;
    Real a, b, c;

    // Compute the direction of steepest descent.
    Vector3<Real> cDir = Vector3<Real>::ZERO;
    Real aMean = (Real)0, aaMean = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        delta = points[i] - center;
        deltaCrossAxis = delta.Cross(axis);
        a = invRSqr*deltaCrossAxis.SquaredLength() - (Real)1;
        aMean += a;
        aaMean += a*a;
        cDir += a*(delta - axis.Dot(delta)*axis); // |axis|=1 assumed
    }
    aMean *= invNumPoints;
    aaMean *= invNumPoints;
    if (cDir.Normalize() < Math<Real>::ZERO_TOLERANCE)
    {
        return aaMean;
    }

    // Compute the 4th-degree polynomial for the line of steepest descent.
    cDirCrossAxis = cDir.Cross(axis);
    c = cDirCrossAxis.SquaredLength()*invNumPoints*invRSqr;
    Real bMean = (Real)0,  abMean = (Real)0, bbMean = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        delta = points[i] - center;
        deltaCrossAxis = delta.Cross(axis);
        a = invRSqr*deltaCrossAxis.SquaredLength() - (Real)1;
        b = invRSqr*(deltaCrossAxis.Dot(cDirCrossAxis));
        bMean += b;
        abMean += a*b;
        bbMean += b*b;
    }
    bMean *= invNumPoints;
    abMean *= invNumPoints;
    bbMean *= invNumPoints;

    Polynomial1<Real> poly(4);
    poly[0] = aaMean;
    poly[1] = ((Real)4)*abMean;
    poly[2] = ((Real)2)*c*aMean + ((Real)4)*bbMean;
    poly[3] = ((Real)4)*c*bMean;
    poly[4] = c*c;

    Polynomial1<Real> derPoly = poly.GetDerivative();

    PolynomialRoots<Real> polyRoots(Math<Real>::ZERO_TOLERANCE);
    polyRoots.FindA(derPoly[0], derPoly[1], derPoly[2], derPoly[3]);
    int count = polyRoots.GetCount();
    const Real* roots = polyRoots.GetRoots();

    Real pMin = poly((Real)0);
    int iMin = -1;
    for (i = 0; i < count; ++i)
    {
        Real value = poly(roots[i]);
        if (value < pMin)
        {
            pMin = value;
            iMin = i;
        }
    }

    if (iMin >= 0)
    {
        center -= roots[iMin]*cDir;
    }

    return pMin;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class CylinderFit3<float>;

template WM5_MATHEMATICS_ITEM
class CylinderFit3<double>;
//----------------------------------------------------------------------------
}
