// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine3Capsule3.h"
#include "Wm5DistLine3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine3Capsule3<Real>::IntrLine3Capsule3 (const Line3<Real>& line,
    const Capsule3<Real>& capsule)
    :
    mLine(&line),
    mCapsule(&capsule)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrLine3Capsule3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Capsule3<Real>& IntrLine3Capsule3<Real>::GetCapsule () const
{
    return *mCapsule;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Capsule3<Real>::Test ()
{
    Real distance = DistLine3Segment3<Real>(*mLine, mCapsule->Segment).Get();
    return distance <= mCapsule->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Capsule3<Real>::Find ()
{
    Real t[2];
    mQuantity = Find(mLine->Origin, mLine->Direction, *mCapsule, t);
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
int IntrLine3Capsule3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrLine3Capsule3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine3Capsule3<Real>::Find (const Vector3<Real>& origin,
    const Vector3<Real>& dir, const Capsule3<Real>& capsule, Real t[2])
{
    // Create a coordinate system for the capsule.  In this system, the
    // capsule segment center C is the origin and the capsule axis direction
    // W is the z-axis.  U and V are the other coordinate axis directions.
    // If P = x*U+y*V+z*W, the cylinder containing the capsule wall is
    // x^2 + y^2 = r^2, where r is the capsule radius.  The finite cylinder
    // that makes up the capsule minus its hemispherical end caps has z-values
    // |z| <= e, where e is the extent of the capsule segment.  The top
    // hemisphere cap is x^2+y^2+(z-e)^2 = r^2 for z >= e, and the bottom
    // hemisphere cap is x^2+y^2+(z+e)^2 = r^2 for z <= -e.
    Vector3<Real> U, V, W = capsule.Segment.Direction;
    Vector3<Real>::GenerateComplementBasis(U, V, W);
    Real rSqr = capsule.Radius*capsule.Radius;
    Real extent = capsule.Segment.Extent;

    // Convert incoming line origin to capsule coordinates.
    Vector3<Real> diff = origin - capsule.Segment.Center;
    Vector3<Real> P(U.Dot(diff), V.Dot(diff), W.Dot(diff));

    // Get the z-value, in capsule coordinates, of the incoming line's
    // unit-length direction.
    Real dz = W.Dot(dir);
    if (Math<Real>::FAbs(dz) >= (Real)1 - Math<Real>::ZERO_TOLERANCE)
    {
        // The line is parallel to the capsule axis.  Determine whether the
        // line intersects the capsule hemispheres.
        Real radialSqrDist = rSqr - P.X()*P.X() - P.Y()*P.Y();
        if (radialSqrDist < (Real)0)
        {
            // Line outside the cylinder of the capsule, no intersection.
            return 0;
        }

        // line intersects the hemispherical caps
        Real zOffset = Math<Real>::Sqrt(radialSqrDist) + extent;
        if (dz > (Real)0)
        {
            t[0] = -P.Z() - zOffset;
            t[1] = -P.Z() + zOffset;
        }
        else
        {
            t[0] = P.Z() - zOffset;
            t[1] = P.Z() + zOffset;
        }
        return 2;
    }

    // Convert incoming line unit-length direction to capsule coordinates.
    Vector3<Real> D(U.Dot(dir), V.Dot(dir), dz);

    // Test intersection of line P+t*D with infinite cylinder x^2+y^2 = r^2.
    // This reduces to computing the roots of a quadratic equation.  If
    // P = (px,py,pz) and D = (dx,dy,dz), then the quadratic equation is
    //   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0
    Real a0 = P.X()*P.X() + P.Y()*P.Y() - rSqr;
    Real a1 = P.X()*D.X() + P.Y()*D.Y();
    Real a2 = D.X()*D.X() + D.Y()*D.Y();
    Real discr = a1*a1 - a0*a2;
    if (discr < (Real)0)
    {
        // Line does not intersect infinite cylinder.
        return 0;
    }

    Real root, inv, tValue, zValue;
    int quantity = 0;
    if (discr > Math<Real>::ZERO_TOLERANCE)
    {
        // Line intersects infinite cylinder in two places.
        root = Math<Real>::Sqrt(discr);
        inv = ((Real)1)/a2;
        tValue = (-a1 - root)*inv;
        zValue = P.Z() + tValue*D.Z();
        if (Math<Real>::FAbs(zValue) <= extent)
        {
            t[quantity++] = tValue;
        }

        tValue = (-a1 + root)*inv;
        zValue = P.Z() + tValue*D.Z();
        if (Math<Real>::FAbs(zValue) <= extent)
        {
            t[quantity++] = tValue;
        }

        if (quantity == 2)
        {
            // Line intersects capsule wall in two places.
            return 2;
        }
    }
    else
    {
        // Line is tangent to infinite cylinder.
        tValue = -a1/a2;
        zValue = P.Z() + tValue*D.Z();
        if (Math<Real>::FAbs(zValue) <= extent)
        {
            t[0] = tValue;
            return 1;
        }
    }

    // Test intersection with bottom hemisphere.  The quadratic equation is
    //   t^2 + 2*(px*dx+py*dy+(pz+e)*dz)*t + (px^2+py^2+(pz+e)^2-r^2) = 0
    // Use the fact that currently a1 = px*dx+py*dy and a0 = px^2+py^2-r^2.
    // The leading coefficient is a2 = 1, so no need to include in the
    // construction.
    Real PZpE = P.Z() + extent;
    a1 += PZpE*D.Z();
    a0 += PZpE*PZpE;
    discr = a1*a1 - a0;
    if (discr > Math<Real>::ZERO_TOLERANCE)
    {
        root = Math<Real>::Sqrt(discr);
        tValue = -a1 - root;
        zValue = P.Z() + tValue*D.Z();
        if (zValue <= -extent)
        {
            t[quantity++] = tValue;
            if (quantity == 2)
            {
                if (t[0] > t[1])
                {
                    Real save = t[0];
                    t[0] = t[1];
                    t[1] = save;
                }
                return 2;
            }
        }

        tValue = -a1 + root;
        zValue = P.Z() + tValue*D.Z();
        if (zValue <= -extent)
        {
            t[quantity++] = tValue;
            if (quantity == 2)
            {
                if (t[0] > t[1])
                {
                    Real save = t[0];
                    t[0] = t[1];
                    t[1] = save;
                }
                return 2;
            }
        }
    }
    else if (Math<Real>::FAbs(discr) <= Math<Real>::ZERO_TOLERANCE)
    {
        tValue = -a1;
        zValue = P.Z() + tValue*D.Z();
        if (zValue <= -extent)
        {
            t[quantity++] = tValue;
            if (quantity == 2)
            {
                if (t[0] > t[1])
                {
                    Real save = t[0];
                    t[0] = t[1];
                    t[1] = save;
                }
                return 2;
            }
        }
    }

    // Test intersection with top hemisphere.  The quadratic equation is
    //   t^2 + 2*(px*dx+py*dy+(pz-e)*dz)*t + (px^2+py^2+(pz-e)^2-r^2) = 0
    // Use the fact that currently a1 = px*dx+py*dy+(pz+e)*dz and
    // a0 = px^2+py^2+(pz+e)^2-r^2.  The leading coefficient is a2 = 1, so
    // no need to include in the construction.
    a1 -= ((Real)2)*extent*D.Z();
    a0 -= ((Real)4)*extent*P.Z();
    discr = a1*a1 - a0;
    if (discr > Math<Real>::ZERO_TOLERANCE)
    {
        root = Math<Real>::Sqrt(discr);
        tValue = -a1 - root;
        zValue = P.Z() + tValue*D.Z();
        if (zValue >= extent)
        {
            t[quantity++] = tValue;
            if (quantity == 2)
            {
                if (t[0] > t[1])
                {
                    Real save = t[0];
                    t[0] = t[1];
                    t[1] = save;
                }
                return 2;
            }
        }

        tValue = -a1 + root;
        zValue = P.Z() + tValue*D.Z();
        if (zValue >= extent)
        {
            t[quantity++] = tValue;
            if (quantity == 2)
            {
                if (t[0] > t[1])
                {
                    Real save = t[0];
                    t[0] = t[1];
                    t[1] = save;
                }
                return 2;
            }
        }
    }
    else if (Math<Real>::FAbs(discr) <= Math<Real>::ZERO_TOLERANCE)
    {
        tValue = -a1;
        zValue = P.Z() + tValue*D.Z();
        if (zValue >= extent)
        {
            t[quantity++] = tValue;
            if (quantity == 2)
            {
                if (t[0] > t[1])
                {
                    Real save = t[0];
                    t[0] = t[1];
                    t[1] = save;
                }
                return 2;
            }
        }
    }

    return quantity;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine3Capsule3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine3Capsule3<double>;
//----------------------------------------------------------------------------
}
