// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine3Cylinder3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine3Cylinder3<Real>::IntrLine3Cylinder3 (const Line3<Real>& line,
    const Cylinder3<Real>& cylinder)
    :
    mLine(&line),
    mCylinder(&cylinder)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrLine3Cylinder3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Cylinder3<Real>& IntrLine3Cylinder3<Real>::GetCylinder () const
{
    return *mCylinder;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Cylinder3<Real>::Find ()
{
    Real t[2];
    mQuantity = Find(mLine->Origin, mLine->Direction, *mCylinder, t);

    for (int i = 0; i < mQuantity; ++i)
    {
        mPoint[i] = mLine->Origin + t[i]*mLine->Direction;
    }

    if (mQuantity == 2)
    {
        mIntersectionType = IT_SEGMENT;
    }
    else if (mQuantity == 1)
    {
        mIntersectionType = IT_POINT;
    }
    else
    {
        mIntersectionType = IT_EMPTY;
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine3Cylinder3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrLine3Cylinder3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine3Cylinder3<Real>::Find (const Vector3<Real>& origin,
    const Vector3<Real>& dir, const Cylinder3<Real>& cylinder, Real t[2])
{
    // Create a coordinate system for the cylinder.  In this system, the
    // cylinder segment center C is the origin and the cylinder axis direction
    // W is the z-axis.  U and V are the other coordinate axis directions.
    // If P = x*U+y*V+z*W, the cylinder is x^2 + y^2 = r^2, where r is the
    // cylinder radius.  The end caps are |z| = h/2, where h is the cylinder
    // height.
    Vector3<Real> U, V, W = cylinder.Axis.Direction;
    Vector3<Real>::GenerateComplementBasis(U, V, W);
    Real halfHeight = ((Real)0.5)*cylinder.Height;
    Real rSqr = cylinder.Radius*cylinder.Radius;

    // convert incoming line origin to cylinder coordinates
    Vector3<Real> diff = origin - cylinder.Axis.Origin;
    Vector3<Real> P(U.Dot(diff), V.Dot(diff), W.Dot(diff));

    // Get the z-value, in cylinder coordinates, of the incoming line's
    // unit-length direction.
    Real dz = W.Dot(dir);

    if (Math<Real>::FAbs(dz) >= (Real)1 - Math<Real>::ZERO_TOLERANCE)
    {
        // The line is parallel to the cylinder axis.  Determine if the line
        // intersects the cylinder end disks.
        Real radialSqrDist = rSqr - P.X()*P.X() - P.Y()*P.Y();
        if (radialSqrDist < (Real)0)
        {
            // Line outside the cylinder, no intersection.
            return 0;
        }

        // Line intersects the cylinder end disks.
        if (dz > (Real)0)
        {
            t[0] = -P.Z() - halfHeight;
            t[1] = -P.Z() + halfHeight;
        }
        else
        {
            t[0] = P.Z() - halfHeight;
            t[1] = P.Z() + halfHeight;
        }
        return 2;
    }

    // convert incoming line unit-length direction to cylinder coordinates
    Vector3<Real> D(U.Dot(dir),V.Dot(dir),dz);

    Real a0, a1, a2, discr, root, inv, tValue;

    if (Math<Real>::FAbs(D.Z()) <= Math<Real>::ZERO_TOLERANCE)
    {
        // The line is perpendicular to the cylinder axis.
        if (Math<Real>::FAbs(P.Z()) > halfHeight)
        {
            // Line is outside the planes of the cylinder end disks.
            return 0;
        }

        // Test intersection of line P+t*D with infinite cylinder
        // x^2+y^2 = r^2.  This reduces to computing the roots of a
        // quadratic equation.  If P = (px,py,pz) and D = (dx,dy,dz),
        // then the quadratic equation is
        //   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0
        a0 = P.X()*P.X() + P.Y()*P.Y() - rSqr;
        a1 = P.X()*D.X() + P.Y()*D.Y();
        a2 = D.X()*D.X() + D.Y()*D.Y();
        discr = a1*a1 - a0*a2;
        if (discr < (Real)0)
        {
            // Line does not intersect cylinder.
            return 0;
        }
        else if (discr > Math<Real>::ZERO_TOLERANCE)
        {
            // Line intersects cylinder in two places.
            root = Math<Real>::Sqrt(discr);
            inv = ((Real)1)/a2;
            t[0] = (-a1 - root)*inv;
            t[1] = (-a1 + root)*inv;
            return 2;
        }
        else
        {
            // Line is tangent to the cylinder.
            t[0] = -a1/a2;
            return 1;
        }
    }

    // Test plane intersections first.
    int quantity = 0;
    inv = ((Real)1.0)/D.Z();

    Real t0 = (-halfHeight - P.Z())*inv;
    Real xTmp = P.X() + t0*D.X();
    Real yTmp = P.Y() + t0*D.Y();
    if (xTmp*xTmp + yTmp*yTmp <= rSqr)
    {
        // Planar intersection inside the top cylinder end disk.
        t[quantity++] = t0;
    }

    Real t1 = (+halfHeight - P.Z())*inv;
    xTmp = P.X() + t1*D.X();
    yTmp = P.Y() + t1*D.Y();
    if (xTmp*xTmp + yTmp*yTmp <= rSqr)
    {
        // Planar intersection inside the bottom cylinder end disk.
        t[quantity++] = t1;
    }

    if (quantity == 2)
    {
        // Line intersects both top and bottom cylinder end disks.
        if (t[0] > t[1])
        {
            Real save = t[0];
            t[0] = t[1];
            t[1] = save;
        }
        return 2;
    }

    // If quantity == 1, then the line must intersect cylinder wall in a
    // single point somewhere between the end disks.  This case is detected
    // in the following code that tests for intersection between line and
    // cylinder wall.
    a0 = P.X()*P.X() + P.Y()*P.Y() - rSqr;
    a1 = P.X()*D.X() + P.Y()*D.Y();
    a2 = D.X()*D.X() + D.Y()*D.Y();
    discr = a1*a1 - a0*a2;
    if (discr < (Real)0)
    {
        // Line does not intersect cylinder wall.
        assertion(quantity == 0, "Unexpected condition\n");
        return 0;
    }
    else if (discr > Math<Real>::ZERO_TOLERANCE)
    {
        root = Math<Real>::Sqrt(discr);
        inv = ((Real)1)/a2;
        tValue = (-a1 - root)*inv;
        if (t0 <= t1)
        {
            if (t0 <= tValue && tValue <= t1)
            {
                t[quantity++] = tValue;
            }
        }
        else
        {
            if (t1 <= tValue && tValue <= t0)
            {
                t[quantity++] = tValue;
            }
        }

        if (quantity == 2)
        {
            // Line intersects one of the cylinder end disks and once on the
            // cylinder wall.
            if (t[0] > t[1])
            {
                Real save = t[0];
                t[0] = t[1];
                t[1] = save;
            }
            return 2;
        }

        tValue = (-a1 + root)*inv;
        if (t0 <= t1)
        {
            if (t0 <= tValue && tValue <= t1)
            {
                t[quantity++] = tValue;
            }
        }
        else
        {
            if (t1 <= tValue && tValue <= t0)
            {
                t[quantity++] = tValue;
            }
        }
    }
    else
    {
        tValue = -a1/a2;
        if (t0 <= t1)
        {
            if (t0 <= tValue && tValue <= t1)
            {
                t[quantity++] = tValue;
            }
        }
        else
        {
            if (t1 <= tValue && tValue <= t0)
            {
                t[quantity++] = tValue;
            }
        }
    }

    if (quantity == 2)
    {
        if (t[0] > t[1])
        {
            Real save = t[0];
            t[0] = t[1];
            t[1] = save;
        }
    }

    return quantity;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine3Cylinder3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine3Cylinder3<double>;
//----------------------------------------------------------------------------
}
