// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.1 (2012/07/08)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrTriangle3Cylinder3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrTriangle3Cylinder3<Real>::IntrTriangle3Cylinder3 (
    const Triangle3<Real>& triangle, const Cylinder3<Real>& cylinder)
    :
    mTriangle(&triangle),
    mCylinder(&cylinder)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrTriangle3Cylinder3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
const Cylinder3<Real>& IntrTriangle3Cylinder3<Real>::GetCylinder () const
{
    return *mCylinder;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Cylinder3<Real>::Test ()
{
    // Get an orthonormal basis {U,V,D}.
    Vector3<Real> U, V;
    Vector3<Real>::GenerateComplementBasis(U, V, mCylinder->Axis.Direction);

    // Compute coordinates of P[i] in system {C;U,V,D}.
    Vector3<Real> temp[3];
    int i;
    for (i = 0; i < 3; ++i)
    {
        Vector3<Real> delta = mTriangle->V[i] - mCylinder->Axis.Origin;
        temp[i][0] = U.Dot(delta);
        temp[i][1] = V.Dot(delta);
        temp[i][2] = mCylinder->Axis.Direction.Dot(delta);
    }

    // Sort by z-value.
    int j0, j1, j2;
    if (temp[0][2] < temp[1][2])
    {
        if (temp[2][2] < temp[0][2])
        {
            j0 = 2; j1 = 0; j2 = 1;
        }
        else if (temp[2][2] < temp[1][2])
        {
            j0 = 0; j1 = 2; j2 = 1;
        }
        else
        {
            j0 = 0; j1 = 1; j2 = 2;
        }
    }
    else
    {
        if (temp[2][2] < temp[1][2])
        {
            j0 = 2; j1 = 1; j2 = 0;
        }
        else if (temp[2][2] < temp[0][2])
        {
            j0 = 1; j1 = 2; j2 = 0;
        }
        else
        {
            j0 = 1; j1 = 0; j2 = 2;
        }
    }

    // Maintain the xy-components and z-components separately.  The
    // z-components are used for clipping against bottom and top of
    // cylinder.  The xy-components are used for containment tests
    // in disk x*x+y*y <= r*r.
    Vector2<Real> Q[3] =
    {
        Vector2<Real>(temp[j0][0], temp[j0][1]),
        Vector2<Real>(temp[j1][0], temp[j1][1]),
        Vector2<Real>(temp[j2][0], temp[j2][1])
    };

    // From sorting we know that z[0] <= z[1] <= z[2].
    Real z[3] =
    {
        temp[j0][2],
        temp[j1][2],
        temp[j2][2]
    };

    // Fast no-intersection.
    const Real hhalf = 0.5f*mCylinder->Height;
    if (z[2] < -hhalf || z[0] > hhalf)
    {
        // Triangle strictly below or above the cylinder.
        return false;
    }

    // Fast no-clipping.
    if (-hhalf <= z[0] && z[2] <= hhalf)
    {
        return DiskOverlapsPolygon(3, Q);
    }

    // Clip against |z| <= h/2.  At this point we know that z2 >= -h/2 and
    // z0 <= h/2 with either z0 < -h/2 or z2 > h/2 (or both).
    Vector2<Real> polygon[5];
    Real t, numer0, numer1, invDenom0, invDenom1;

    if (z[0] < -hhalf)
    {
        if (z[2] > hhalf)
        {
            if (z[1] >= hhalf)
            {
                numer0 = -hhalf - z[0];
                numer1 = +hhalf - z[0];
                invDenom0 = ((Real)1)/(z[1] - z[0]);
                invDenom1 = ((Real)1)/(z[2] - z[0]);
                t = numer0*invDenom1;
                polygon[0] = Q[0] + t*(Q[2] - Q[0]);
                t = numer0*invDenom0;
                polygon[1] = Q[0] + t*(Q[1] - Q[0]);
                t = numer1*invDenom0;
                polygon[2] = Q[0] + t*(Q[1] - Q[0]);
                t = numer1*invDenom1;
                polygon[3] = Q[0] + t*(Q[2] - Q[0]);
                return DiskOverlapsPolygon(4, polygon);
            }
            else if (z[1] <= -hhalf)
            {
                numer0 = -hhalf - z[2];
                numer1 = +hhalf - z[2];
                invDenom0 = ((Real)1)/(z[1] - z[2]);
                invDenom1 = ((Real)1)/(z[0] - z[2]);
                t = numer0*invDenom1;
                polygon[0] = Q[2] + t*(Q[0] - Q[2]);
                t = numer0*invDenom0;
                polygon[1] = Q[2] + t*(Q[1] - Q[2]);
                t = numer1*invDenom0;
                polygon[2] = Q[2] + t*(Q[1] - Q[2]);
                t = numer1*invDenom1;
                polygon[3] = Q[2] + t*(Q[0] - Q[2]);
                return DiskOverlapsPolygon(4, polygon);
            }
            else
            {
                numer0 = -hhalf - z[0];
                numer1 = +hhalf - z[0];
                invDenom0 = ((Real)1)/(z[1] - z[0]);
                invDenom1 = ((Real)1)/(z[2] - z[0]);
                t = numer0*invDenom1;
                polygon[0] = Q[0] + t*(Q[2] - Q[0]);
                t = numer0*invDenom0;
                polygon[1] = Q[0] + t*(Q[1] - Q[0]);
                t = numer1*invDenom0;
                polygon[2] = Q[1];
                polygon[3] = Q[0] + t*(Q[1] - Q[0]);
                t = numer1*invDenom1;
                polygon[4] = Q[0] + t*(Q[2] - Q[0]);
                return DiskOverlapsPolygon(5, polygon);
            }
        }
        else if (z[2] > -hhalf)
        {
            if (z[1] <= -hhalf)
            {
                numer0 = -hhalf - z[2];
                invDenom0 = ((Real)1)/(z[1] - z[2]);
                invDenom1 = ((Real)1)/(z[0] - z[2]);
                t = numer0*invDenom0;
                polygon[0] = Q[2] + t*(Q[1] - Q[2]);
                t = numer0*invDenom1;
                polygon[1] = Q[2] + t*(Q[0] - Q[2]);
                polygon[2] = Q[2];
                return DiskOverlapsPolygon(3, polygon);
            }
            else
            {
                numer0 = -hhalf - z[0];
                invDenom0 = ((Real)1)/(z[2] - z[0]);
                invDenom1 = ((Real)1)/(z[1] - z[0]);
                t = numer0*invDenom0;
                polygon[0] = Q[0] + t*(Q[2] - Q[0]);
                t = numer0*invDenom1;
                polygon[1] = Q[0] + t*(Q[1] - Q[0]);
                polygon[2] = Q[1];
                polygon[3] = Q[2];
                return DiskOverlapsPolygon(4, polygon);
            }
        }
        else
        {
            if (z[1] < -hhalf)
            {
                return DiskOverlapsPoint(Q[2]);
            }
            else
            {
                return DiskOverlapsSegment(Q[2], Q[1]);
            }
        }
    }
    else if (z[0] < hhalf)
    {
        if (z[1] >= hhalf)
        {
            numer0 = -hhalf - z[0];
            invDenom0 = ((Real)1)/(z[2] - z[0]);
            invDenom1 = ((Real)1)/(z[1] - z[0]);
            t = numer0*invDenom0;
            polygon[0] = Q[0] + t*(Q[2] - Q[0]);
            t = numer0*invDenom1;
            polygon[1] = Q[0] + t*(Q[1] - Q[0]);
            polygon[2] = Q[0];
            return DiskOverlapsPolygon(3, polygon);
        }
        else
        {
            numer0 = -hhalf - z[2];
            invDenom0 = ((Real)1)/(z[1] - z[2]);
            invDenom1 = ((Real)1)/(z[0] - z[2]);
            t = numer0*invDenom0;
            polygon[0] = Q[2] + t*(Q[1] - Q[2]);
            t = numer0*invDenom1;
            polygon[1] = Q[2] + t*(Q[0] - Q[2]);
            polygon[2] = Q[0];
            polygon[3] = Q[1];
            return DiskOverlapsPolygon(4, polygon);
        }
    }
    else
    {
        if (z[1] > hhalf)
        {
            return DiskOverlapsPoint(Q[0]);
        }
        else
        {
            return DiskOverlapsSegment(Q[0], Q[1]);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Cylinder3<Real>::DiskOverlapsPoint (const Vector2<Real>& Q)
    const
{
    return Q[0]*Q[0] + Q[1]*Q[1] <= mCylinder->Radius*mCylinder->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Cylinder3<Real>::DiskOverlapsSegment (
    const Vector2<Real>& Q0, const Vector2<Real>& Q1) const
{
    Real rSqr = mCylinder->Radius*mCylinder->Radius;
    Vector2<Real> D = Q0 - Q1;
    Real dot = Q0.Dot(D);
    if (dot <= (Real)0)
    {
        return Q0.Dot(Q0) <= rSqr;
    }

    Real lenSqr = D.Dot(D);
    if (dot >= lenSqr)
    {
        return Q1.Dot(Q1) <= rSqr;
    }

    dot = D.DotPerp(Q0);
    return dot*dot <= lenSqr*rSqr;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Cylinder3<Real>::DiskOverlapsPolygon (int numVertices,
    const Vector2<Real>* Q) const
{
    // Test whether the polygon contains (0,0).
    int positive = 0, negative = 0;
    int i0, i1;
    for (i0 = numVertices-1, i1 = 0; i1 < numVertices; i0 = i1++)
    {
        Real dot = Q[i0].DotPerp(Q[i0] - Q[i1]);
        if (dot > (Real)0)
        {
            ++positive;
        }
        else if (dot < (Real)0)
        {
            ++negative;
        }
    }
    if (positive == 0 || negative == 0)
    {
        // The polygon contains (0,0), so the disk and polygon overlap.
        return true;
    }

    // Test whether any edge is overlapped by the polygon.
    for (i0 = numVertices-1, i1 = 0; i1 < numVertices; i0 = i1++)
    {
        if (DiskOverlapsSegment(Q[i0], Q[i1]))
        {
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrTriangle3Cylinder3<float>;

template WM5_MATHEMATICS_ITEM
class IntrTriangle3Cylinder3<double>;
//----------------------------------------------------------------------------
}
