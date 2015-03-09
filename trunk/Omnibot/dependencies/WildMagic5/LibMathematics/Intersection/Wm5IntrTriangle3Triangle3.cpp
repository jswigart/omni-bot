// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/10/23)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrTriangle3Triangle3.h"
#include "Wm5Intersector1.h"
#include "Wm5IntrSegment2Triangle2.h"
#include "Wm5IntrTriangle2Triangle2.h"
#include "Wm5Query2.h"
#include "Wm5DistSegment3Triangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrTriangle3Triangle3<Real>::IntrTriangle3Triangle3 (
    const Triangle3<Real>& triangle0, const Triangle3<Real>& triangle1)
    :
    mTriangle0(&triangle0),
    mTriangle1(&triangle1)
{
    mReportCoplanarIntersections = true;
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrTriangle3Triangle3<Real>::GetTriangle0 () const
{
    return *mTriangle0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrTriangle3Triangle3<Real>::GetTriangle1 () const
{
    return *mTriangle1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::Test ()
{
    // Get edge vectors for triangle0.
    Vector3<Real> E0[3] =
    {
        mTriangle0->V[1] - mTriangle0->V[0],
        mTriangle0->V[2] - mTriangle0->V[1],
        mTriangle0->V[0] - mTriangle0->V[2]
    };

    // Get normal vector of triangle0.
    Vector3<Real> N0 = E0[0].UnitCross(E0[1]);

    // Project triangle1 onto normal line of triangle0, test for separation.
    Real N0dT0V0 = N0.Dot(mTriangle0->V[0]);
    Real min1, max1;
    ProjectOntoAxis(*mTriangle1, N0, min1, max1);
    if (N0dT0V0 < min1 || N0dT0V0 > max1)
    {
        return false;
    }

    // Get edge vectors for triangle1.
    Vector3<Real> E1[3] =
    {
        mTriangle1->V[1] - mTriangle1->V[0],
        mTriangle1->V[2] - mTriangle1->V[1],
        mTriangle1->V[0] - mTriangle1->V[2]
    };

    // Get normal vector of triangle1.
    Vector3<Real> N1 = E1[0].UnitCross(E1[1]);

    Vector3<Real> dir;
    Real min0, max0;
    int i0, i1;

    Vector3<Real> N0xN1 = N0.UnitCross(N1);
    if (N0xN1.Dot(N0xN1) >= Math<Real>::ZERO_TOLERANCE)
    {
        // Triangles are not parallel.

        // Project triangle0 onto normal line of triangle1, test for
        // separation.
        Real N1dT1V0 = N1.Dot(mTriangle1->V[0]);
        ProjectOntoAxis(*mTriangle0, N1, min0, max0);
        if (N1dT1V0 < min0 || N1dT1V0 > max0)
        {
            return false;
        }

        // Directions E0[i0]xE1[i1].
        for (i1 = 0; i1 < 3; ++i1)
        {
            for (i0 = 0; i0 < 3; ++i0)
            {
                dir = E0[i0].UnitCross(E1[i1]);
                ProjectOntoAxis(*mTriangle0, dir, min0, max0);
                ProjectOntoAxis(*mTriangle1, dir, min1, max1);
                if (max0 < min1 || max1 < min0)
                {
                    return false;
                }
            }
        }

        // The test query does not know the intersection set.
        mIntersectionType = IT_OTHER;
    }
    else  // Triangles are parallel (and, in fact, coplanar).
    {
        // Directions N0xE0[i0].
        for (i0 = 0; i0 < 3; ++i0)
        {
            dir = N0.UnitCross(E0[i0]);
            ProjectOntoAxis(*mTriangle0, dir, min0, max0);
            ProjectOntoAxis(*mTriangle1, dir, min1, max1);
            if (max0 < min1 || max1 < min0)
            {
                return false;
            }
        }

        // Directions N1xE1[i1].
        for (i1 = 0; i1 < 3; ++i1)
        {
            dir = N1.UnitCross(E1[i1]);
            ProjectOntoAxis(*mTriangle0, dir, min0, max0);
            ProjectOntoAxis(*mTriangle1, dir, min1, max1);
            if (max0 < min1 || max1 < min0)
            {
                return false;
            }
        }

        // The test query does not know the intersection set.
        mIntersectionType = IT_PLANE;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::Find ()
{
    int i, iM, iP;

    // Get the plane of triangle0.
    Plane3<Real> plane0(mTriangle0->V[0], mTriangle0->V[1],
        mTriangle0->V[2]);

    // Compute the signed distances of triangle1 vertices to plane0.  Use
    // an epsilon-thick plane test.
    int pos1, neg1, zero1, sign1[3];
    Real dist1[3];
    TrianglePlaneRelations(*mTriangle1, plane0, dist1, sign1, pos1, neg1,
        zero1);

    if (pos1 == 3 || neg1 == 3)
    {
        // Triangle1 is fully on one side of plane0.
        return false;
    }

    if (zero1 == 3)
    {
        // Triangle1 is contained by plane0.
        if (mReportCoplanarIntersections)
        {
            return GetCoplanarIntersection(plane0, *mTriangle0,
                *mTriangle1);
        }
        return false;
    }

    // Check for grazing contact between triangle1 and plane0.
    if (pos1 == 0 || neg1 == 0)
    {
        if (zero1 == 2)
        {
            // An edge of triangle1 is in plane0.
            for (i = 0; i < 3; ++i)
            {
                if (sign1[i] != 0)
                {
                    iM = (i + 2) % 3;
                    iP = (i + 1) % 3;
                    return IntersectsSegment(plane0, *mTriangle0,
                        mTriangle1->V[iM], mTriangle1->V[iP]);
                }
            }
        }
        else // zero1 == 1
        {
            // A vertex of triangle1 is in plane0.
            for (i = 0; i < 3; ++i)
            {
                if (sign1[i] == 0)
                {
                    return ContainsPoint(*mTriangle0, plane0,
                        mTriangle1->V[i]);
                }
            }
        }
    }

    // At this point, triangle1 tranversely intersects plane 0.  Compute the
    // line segment of intersection.  Then test for intersection between this
    // segment and triangle 0.
    Real t;
    Vector3<Real> intr0, intr1;
    if (zero1 == 0)
    {
        int iSign = (pos1 == 1 ? +1 : -1);
        for (i = 0; i < 3; ++i)
        {
            if (sign1[i] == iSign)
            {
                iM = (i + 2) % 3;
                iP = (i + 1) % 3;
                t = dist1[i]/(dist1[i] - dist1[iM]);
                intr0 = mTriangle1->V[i] + t*(mTriangle1->V[iM] -
                    mTriangle1->V[i]);
                t = dist1[i]/(dist1[i] - dist1[iP]);
                intr1 = mTriangle1->V[i] + t*(mTriangle1->V[iP] -
                    mTriangle1->V[i]);
                return IntersectsSegment(plane0, *mTriangle0, intr0, intr1);
            }
        }
    }

    // zero1 == 1
    for (i = 0; i < 3; ++i)
    {
        if (sign1[i] == 0)
        {
            iM = (i + 2) % 3;
            iP = (i + 1) % 3;
            t = dist1[iM]/(dist1[iM] - dist1[iP]);
            intr0 = mTriangle1->V[iM] + t*(mTriangle1->V[iP] -
                mTriangle1->V[iM]);
            return IntersectsSegment(plane0, *mTriangle0,
                mTriangle1->V[i],intr0);
        }
    }

    assertion(false, "Should not get here\n");
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Real tfirst = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    // Velocity relative to triangle0.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    // Compute edge and normal directions for triangle0.
    Vector3<Real> E[3] =
    {
        mTriangle0->V[1] - mTriangle0->V[0],
        mTriangle0->V[2] - mTriangle0->V[1],
        mTriangle0->V[0] - mTriangle0->V[2]
    };
    Vector3<Real> N = E[0].UnitCross(E[1]);
    if (!TestOverlap(N,tmax,relVelocity,tfirst,tlast))
    {
        return false;
    }

    // Compute edge and normal directions for triangle1.
    Vector3<Real> F[3] =
    {
        mTriangle1->V[1] - mTriangle1->V[0],
        mTriangle1->V[2] - mTriangle1->V[1],
        mTriangle1->V[0] - mTriangle1->V[2]
    };
    Vector3<Real> M = F[0].UnitCross(F[1]);

    Vector3<Real> dir;
    int i0, i1;

    if (Math<Real>::FAbs(N.Dot(M)) < (Real)1 - Math<Real>::ZERO_TOLERANCE)
    {
        // Triangles are not parallel.

        // Direction M.
        if (!TestOverlap(M, tmax, relVelocity, tfirst, tlast))
        {
            return false;
        }

        // Directions E[i0]xF[i1].
        for (i1 = 0; i1 < 3; ++i1)
        {
            for (i0 = 0; i0 < 3; ++i0)
            {
                dir = E[i0].UnitCross(F[i1]);
                if (!TestOverlap(dir, tmax, relVelocity, tfirst, tlast))
                {
                    return false;
                }
            }
        }

        mIntersectionType = IT_OTHER;
    }
    else  // Triangles are parallel (and, in fact, coplanar).
    {
        // Directions NxE[i0].
        for (i0 = 0; i0 < 3; ++i0)
        {
            dir = N.UnitCross(E[i0]);
            if (!TestOverlap(dir, tmax, relVelocity, tfirst, tlast))
            {
                return false;
            }
        }

        // directions NxF[i1]
        for (i1 = 0; i1 < 3; ++i1)
        {
            dir = M.UnitCross(F[i1]);
            if (!TestOverlap(dir, tmax, relVelocity, tfirst, tlast))
            {
                return false;
            }
        }

        mIntersectionType = IT_PLANE;
    }

    mContactTime = tfirst;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Real tfirst = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    // Velocity relative to triangle0.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    ContactSide side = CS_NONE;
    Configuration tcfg0, tcfg1;

    // Compute edge and normal directions for triangle0.
    Vector3<Real> E[3] =
    {
        mTriangle0->V[1] - mTriangle0->V[0],
        mTriangle0->V[2] - mTriangle0->V[1],
        mTriangle0->V[0] - mTriangle0->V[2]
    };
    Vector3<Real> N = E[0].UnitCross(E[1]);
    if (!FindOverlap(N, tmax, relVelocity, side, tcfg0, tcfg1, tfirst, tlast))
    {
        return false;
    }

    // Compute edge and normal directions for triangle1.
    Vector3<Real> F[3] =
    {
        mTriangle1->V[1] - mTriangle1->V[0],
        mTriangle1->V[2] - mTriangle1->V[1],
        mTriangle1->V[0] - mTriangle1->V[2]
    };
    Vector3<Real> M = F[0].UnitCross(F[1]);

    Vector3<Real> dir;
    int i0, i1;

    if (Math<Real>::FAbs(N.Dot(M)) < (Real)1 - Math<Real>::ZERO_TOLERANCE)
    {
        // Triangles are not parallel.

        // Direction M.
        if (!FindOverlap(M, tmax, relVelocity, side, tcfg0, tcfg1, tfirst,
            tlast))
        {
            return false;
        }

        // Directions E[i0]xF[i1].
        for (i1 = 0; i1 < 3; ++i1)
        {
            for (i0 = 0; i0 < 3; ++i0)
            {
                dir = E[i0].UnitCross(F[i1]);
                if (!FindOverlap(dir, tmax, relVelocity, side, tcfg0, tcfg1,
                    tfirst, tlast))
                {
                    return false;
                }
            }
        }
    }
    else  // Triangles are parallel (and, in fact, coplanar).
    {
        // Directions NxE[i0].
        for (i0 = 0; i0 < 3; ++i0)
        {
            dir = N.UnitCross(E[i0]);
            if (!FindOverlap(dir, tmax, relVelocity, side, tcfg0, tcfg1,
                tfirst, tlast))
            {
                return false;
            }
        }

        // Directions NxF[i1].
        for (i1 = 0; i1 < 3; ++i1)
        {
            dir = M.UnitCross(F[i1]);
            if (!FindOverlap(dir, tmax, relVelocity, side, tcfg0, tcfg1,
                tfirst, tlast))
            {
                return false;
            }
        }
    }
    
    if (tfirst <= (Real)0)
    {
        return false;
    }

    mContactTime = tfirst;

    // adjust U and V for first time of contact before finding contact set
    Triangle3<Real> MTri0, MTri1;
    for (i0 = 0; i0 < 3; ++i0)
    {
        MTri0.V[i0] = mTriangle0->V[i0] + tfirst*velocity0;
        MTri1.V[i0] = mTriangle1->V[i0] + tfirst*velocity1;
    }

    FindContactSet(MTri0, MTri1, side, tcfg0, tcfg1);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrTriangle3Triangle3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrTriangle3Triangle3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle3Triangle3<Real>::ProjectOntoAxis (
    const Triangle3<Real>& triangle, const Vector3<Real>& axis, Real& fmin,
    Real& fmax)
{
    Real dot0 = axis.Dot(triangle.V[0]);
    Real dot1 = axis.Dot(triangle.V[1]);
    Real dot2 = axis.Dot(triangle.V[2]);

    fmin = dot0;
    fmax = fmin;

    if (dot1 < fmin)
    {
        fmin = dot1;
    }
    else if (dot1 > fmax)
    {
        fmax = dot1;
    }

    if (dot2 < fmin)
    {
        fmin = dot2;
    }
    else if (dot2 > fmax)
    {
        fmax = dot2;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle3Triangle3<Real>::TrianglePlaneRelations (
    const Triangle3<Real>& triangle, const Plane3<Real>& plane,
    Real distance[3], int sign[3], int& positive, int& negative, int& zero)
{
    // Compute the signed distances of triangle vertices to the plane.  Use
    // an epsilon-thick plane test.
    positive = 0;
    negative = 0;
    zero = 0;
    for (int i = 0; i < 3; ++i)
    {
        distance[i] = plane.DistanceTo(triangle.V[i]);
        if (distance[i] > Math<Real>::ZERO_TOLERANCE)
        {
            sign[i] = 1;
            positive++;
        }
        else if (distance[i] < -Math<Real>::ZERO_TOLERANCE)
        {
            sign[i] = -1;
            negative++;
        }
        else
        {
            distance[i] = (Real)0;
            sign[i] = 0;
            zero++;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle3Triangle3<Real>::GetInterval (
    const Triangle3<Real>& triangle, const Line3<Real>& line,
    const Real distance[3], const int sign[3], Real param[2])
{
    // Project triangle onto line.
    Real proj[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        Vector3<Real> diff = triangle.V[i] - line.Origin;
        proj[i] = line.Direction.Dot(diff);
    }

    // Compute transverse intersections of triangle edges with line.
    Real numer, denom;
    int i0, i1, i2;
    int quantity = 0;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        if (sign[i0]*sign[i1] < 0)
        {
            assertion(quantity < 2, "Unexpected condition\n");
            numer = distance[i0]*proj[i1] - distance[i1]*proj[i0];
            denom = distance[i0] - distance[i1];
            param[quantity++] = numer/denom;
        }
    }

    // Check for grazing contact.
    if (quantity < 2)
    {
        for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2++)
        {
            if (sign[i2] == 0)
            {
                assertion(quantity < 2, "Unexpected condition\n");
                param[quantity++] = proj[i2];
            }
        }
    }

    // Sort.
    assertion(quantity == 1 || quantity == 2, "Unexpected condition\n");
    if (quantity == 2)
    {
        if (param[0] > param[1])
        {
            Real save = param[0];
            param[0] = param[1];
            param[1] = save;
        }
    }
    else
    {
        param[1] = param[0];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::ContainsPoint (
    const Triangle3<Real>& triangle, const Plane3<Real>& plane,
    const Vector3<Real>& point)
{
    // Generate a coordinate system for the plane.  The incoming triangle has
    // vertices <V0,V1,V2>.  The incoming plane has unit-length normal N.
    // The incoming point is P.  V0 is chosen as the origin for the plane. The
    // coordinate axis directions are two unit-length vectors, U0 and U1,
    // constructed so that {U0,U1,N} is an orthonormal set.  Any point Q
    // in the plane may be written as Q = V0 + x0*U0 + x1*U1.  The coordinates
    // are computed as x0 = Dot(U0,Q-V0) and x1 = Dot(U1,Q-V0).
    Vector3<Real> U0, U1;
    Vector3<Real>::GenerateComplementBasis(U0, U1, plane.Normal);

    // Compute the planar coordinates for the points P, V1, and V2.  To
    // simplify matters, the origin is subtracted from the points, in which
    // case the planar coordinates are for P-V0, V1-V0, and V2-V0.
    Vector3<Real> PmV0 = point - triangle.V[0];
    Vector3<Real> V1mV0 = triangle.V[1] - triangle.V[0];
    Vector3<Real> V2mV0 = triangle.V[2] - triangle.V[0];

    // The planar representation of P-V0.
    Vector2<Real> ProjP(U0.Dot(PmV0), U1.Dot(PmV0));

    // The planar representation of the triangle <V0-V0,V1-V0,V2-V0>.
    Vector2<Real> ProjV[3] =
    {
        Vector2<Real>::ZERO,
        Vector2<Real>(U0.Dot(V1mV0),U1.Dot(V1mV0)),
        Vector2<Real>(U0.Dot(V2mV0),U1.Dot(V2mV0))
    };

    // Test whether P-V0 is in the triangle <0,V1-V0,V2-V0>.
    if (Query2<Real>(3,ProjV).ToTriangle(ProjP,0,1,2) <= 0)
    {
        // Report the point of intersection to the caller.
        mIntersectionType = IT_POINT;
        mQuantity = 1;
        mPoint[0] = point;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::IntersectsSegment (
    const Plane3<Real>& plane, const Triangle3<Real>& triangle,
    const Vector3<Real>& end0, const Vector3<Real>& end1)
{
    // Compute the 2D representations of the triangle vertices and the
    // segment endpoints relative to the plane of the triangle.  Then
    // compute the intersection in the 2D space.

    // Project the triangle and segment onto the coordinate plane most
    // aligned with the plane normal.
    int maxNormal = 0;
    Real fmax = Math<Real>::FAbs(plane.Normal.X());
    Real absMax = Math<Real>::FAbs(plane.Normal.Y());
    if (absMax > fmax)
    {
        maxNormal = 1;
        fmax = absMax;
    }
    absMax = Math<Real>::FAbs(plane.Normal.Z());
    if (absMax > fmax)
    {
        maxNormal = 2;
    }

    Triangle2<Real> projTri;
    Vector2<Real> projEnd0, projEnd1;
    int i;

    if (maxNormal == 0)
    {
        // Project onto yz-plane.
        for (i = 0; i < 3; ++i)
        {
            projTri.V[i].X() = triangle.V[i].Y();
            projTri.V[i].Y() = triangle.V[i].Z();
            projEnd0.X() = end0.Y();
            projEnd0.Y() = end0.Z();
            projEnd1.X() = end1.Y();
            projEnd1.Y() = end1.Z();
        }
    }
    else if (maxNormal == 1)
    {
        // Project onto xz-plane.
        for (i = 0; i < 3; ++i)
        {
            projTri.V[i].X() = triangle.V[i].X();
            projTri.V[i].Y() = triangle.V[i].Z();
            projEnd0.X() = end0.X();
            projEnd0.Y() = end0.Z();
            projEnd1.X() = end1.X();
            projEnd1.Y() = end1.Z();
        }
    }
    else
    {
        // Project onto xy-plane.
        for (i = 0; i < 3; ++i)
        {
            projTri.V[i].X() = triangle.V[i].X();
            projTri.V[i].Y() = triangle.V[i].Y();
            projEnd0.X() = end0.X();
            projEnd0.Y() = end0.Y();
            projEnd1.X() = end1.X();
            projEnd1.Y() = end1.Y();
        }
    }

    Segment2<Real> projSeg(projEnd0, projEnd1);
    IntrSegment2Triangle2<Real> calc(projSeg, projTri);
    if (!calc.Find())
    {
        return false;
    }

    Vector2<Real> intr[2];
    if (calc.GetIntersectionType() == IT_SEGMENT)
    {
        mIntersectionType = IT_SEGMENT;
        mQuantity = 2;
        intr[0] = calc.GetPoint(0);
        intr[1] = calc.GetPoint(1);
    }
    else
    {
        assertion(calc.GetIntersectionType() == IT_POINT,
            "Intersection must be a point\n");
        mIntersectionType = IT_POINT;
        mQuantity = 1;
        intr[0] = calc.GetPoint(0);
    }

    // Unproject the segment of intersection.
    if (maxNormal == 0)
    {
        Real invNX = ((Real)1)/plane.Normal.X();
        for (i = 0; i < mQuantity; ++i)
        {
            mPoint[i].Y() = intr[i].X();
            mPoint[i].Z() = intr[i].Y();
            mPoint[i].X() = invNX*(plane.Constant -
                plane.Normal.Y()*mPoint[i].Y() -
                plane.Normal.Z()*mPoint[i].Z());
        }
    }
    else if (maxNormal == 1)
    {
        Real invNY = ((Real)1)/plane.Normal.Y();
        for (i = 0; i < mQuantity; ++i)
        {
            mPoint[i].X() = intr[i].X();
            mPoint[i].Z() = intr[i].Y();
            mPoint[i].Y() = invNY*(plane.Constant -
                plane.Normal.X()*mPoint[i].X() -
                plane.Normal.Z()*mPoint[i].Z());
        }
    }
    else
    {
        Real invNZ = ((Real)1)/plane.Normal.Z();
        for (i = 0; i < mQuantity; ++i)
        {
            mPoint[i].X() = intr[i].X();
            mPoint[i].Y() = intr[i].Y();
            mPoint[i].Z() = invNZ*(plane.Constant -
                plane.Normal.X()*mPoint[i].X() -
                plane.Normal.Y()*mPoint[i].Y());
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::GetCoplanarIntersection (
    const Plane3<Real>& plane, const Triangle3<Real>& tri0,
    const Triangle3<Real>& tri1)
{
    // Project triangles onto coordinate plane most aligned with plane
    // normal.
    int maxNormal = 0;
    Real fmax = Math<Real>::FAbs(plane.Normal.X());
    Real absMax = Math<Real>::FAbs(plane.Normal.Y());
    if (absMax > fmax)
    {
        maxNormal = 1;
        fmax = absMax;
    }
    absMax = Math<Real>::FAbs(plane.Normal.Z());
    if (absMax > fmax)
    {
        maxNormal = 2;
    }

    Triangle2<Real> projTri0, projTri1;
    int i;

    if (maxNormal == 0)
    {
        // Project onto yz-plane.
        for (i = 0; i < 3; ++i)
        {
            projTri0.V[i].X() = tri0.V[i].Y();
            projTri0.V[i].Y() = tri0.V[i].Z();
            projTri1.V[i].X() = tri1.V[i].Y();
            projTri1.V[i].Y() = tri1.V[i].Z();
        }
    }
    else if (maxNormal == 1)
    {
        // Project onto xz-plane.
        for (i = 0; i < 3; ++i)
        {
            projTri0.V[i].X() = tri0.V[i].X();
            projTri0.V[i].Y() = tri0.V[i].Z();
            projTri1.V[i].X() = tri1.V[i].X();
            projTri1.V[i].Y() = tri1.V[i].Z();
        }
    }
    else
    {
        // Project onto xy-plane.
        for (i = 0; i < 3; ++i)
        {
            projTri0.V[i].X() = tri0.V[i].X();
            projTri0.V[i].Y() = tri0.V[i].Y();
            projTri1.V[i].X() = tri1.V[i].X();
            projTri1.V[i].Y() = tri1.V[i].Y();
        }
    }

    // 2D triangle intersection routines require counterclockwise ordering.
    Vector2<Real> save;
    Vector2<Real> edge0 = projTri0.V[1] - projTri0.V[0];
    Vector2<Real> edge1 = projTri0.V[2] - projTri0.V[0];
    if (edge0.DotPerp(edge1) < (Real)0)
    {
        // Triangle is clockwise, reorder it.
        save = projTri0.V[1];
        projTri0.V[1] = projTri0.V[2];
        projTri0.V[2] = save;
    }

    edge0 = projTri1.V[1] - projTri1.V[0];
    edge1 = projTri1.V[2] - projTri1.V[0];
    if (edge0.DotPerp(edge1) < (Real)0)
    {
        // Triangle is clockwise, reorder it.
        save = projTri1.V[1];
        projTri1.V[1] = projTri1.V[2];
        projTri1.V[2] = save;
    }

    IntrTriangle2Triangle2<Real> intr(projTri0,projTri1);
    if (!intr.Find())
    {
        return false;
    }

    // Map 2D intersections back to the 3D triangle space.
    mQuantity = intr.GetQuantity();
    if (maxNormal == 0)
    {
        Real invNX = ((Real)1)/plane.Normal.X();
        for (i = 0; i < mQuantity; i++)
        {
            mPoint[i].Y() = intr.GetPoint(i).X();
            mPoint[i].Z() = intr.GetPoint(i).Y();
            mPoint[i].X() = invNX*(plane.Constant -
                plane.Normal.Y()*mPoint[i].Y() -
                plane.Normal.Z()*mPoint[i].Z());
        }
    }
    else if (maxNormal == 1)
    {
        Real invNY = ((Real)1)/plane.Normal.Y();
        for (i = 0; i < mQuantity; i++)
        {
            mPoint[i].X() = intr.GetPoint(i).X();
            mPoint[i].Z() = intr.GetPoint(i).Y();
            mPoint[i].Y() = invNY*(plane.Constant -
                plane.Normal.X()*mPoint[i].X() -
                plane.Normal.Z()*mPoint[i].Z());
        }
    }
    else
    {
        Real invNZ = ((Real)1)/plane.Normal.Z();
        for (i = 0; i < mQuantity; i++)
        {
            mPoint[i].X() = intr.GetPoint(i).X();
            mPoint[i].Y() = intr.GetPoint(i).Y();
            mPoint[i].Z() = invNZ*(plane.Constant -
                plane.Normal.X()*mPoint[i].X() -
                plane.Normal.Y()*mPoint[i].Y());
        }
    }

    mIntersectionType = IT_PLANE;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::TestOverlap (Real tmax, Real speed,
    Real umin, Real umax, Real vmin, Real vmax, Real& tfirst, Real& tlast)
{
    // Constant velocity separating axis test.

    Real t;

    if (vmax < umin) // V on left of U
    {
        if (speed <= (Real)0) // V moving away from U
        {
            return false;
        }

        // Find first time of contact on this axis.
        t = (umin - vmax)/speed;
        if (t > tfirst)
        {
            tfirst = t;
        }

        // Quick out: intersection after desired time interval.
        if (tfirst > tmax)
        {
            return false;   
        }

        // Find last time of contact on this axis.
        t = (umax - vmin)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // Quick out: intersection before desired time interval.
        if (tfirst > tlast)
        {
            return false; 
        }
    }
    else if (umax < vmin)   // V on right of U
    {
        if (speed >= (Real)0) // V moving away from U
        {
            return false;
        }

        // Find first time of contact on this axis.
        t = (umax - vmin)/speed;
        if (t > tfirst)
        {
            tfirst = t;
        }

        // Quick out: intersection after desired time interval.
        if (tfirst > tmax)
        {
            return false;   
        }

        // Find last time of contact on this axis.
        t = (umin - vmax)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // Quick out: intersection before desired time interval.
        if (tfirst > tlast)
        {
            return false; 
        }

    }
    else // V and U on overlapping interval
    {
        if (speed > (Real)0)
        {
            // Find last time of contact on this axis.
            t = (umax - vmin)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // Quick out: intersection before desired interval.
            if (tfirst > tlast)
            {
                return false; 
            }
        }
        else if (speed < (Real)0)
        {
            // Find last time of contact on this axis.
            t = (umin - vmax)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // Quick out: intersection before desired interval.
            if (tfirst > tlast)
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::FindOverlap (Real tmax, Real speed,
    const Configuration& UC, const Configuration& VC, ContactSide& side,
    Configuration& TUC, Configuration& TVC, Real& tfirst, Real& tlast)
{
    // Constant velocity separating axis test.  UC and VC are the new
    // potential configurations, and TUC and TVC are the best known
    // configurations.

    Real t;

    if (VC.mMax < UC.mMin) // V on left of U
    {
        if (speed <= (Real)0) // V moving away from U
        {
            return false;
        }

        // Find first time of contact on this axis.
        t = (UC.mMin - VC.mMax)/speed;

        // If this is the new maximum first time of contact, set side and
        // configuration.
        if (t > tfirst)
        {
            tfirst = t;
            side = CS_LEFT;
            TUC = UC;
            TVC = VC;
        }

        // Quick out: intersection after desired interval.
        if (tfirst > tmax)
        {
            return false;
        }

        // Find last time of contact on this axis.
        t = (UC.mMax - VC.mMin)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // Quick out: intersection before desired interval.
        if (tfirst > tlast)
        {
            return false;
        }
    }
    else if (UC.mMax < VC.mMin)   // V on right of U
    {
        if (speed >= (Real)0) // V moving away from U
        {
            return false;
        }

        // Find first time of contact on this axis.
        t = (UC.mMax - VC.mMin)/speed;

        // If this is the new maximum first time of contact, set side and
        // configuration.
        if (t > tfirst)
        {
            tfirst = t;
            side = CS_RIGHT;
            TUC = UC;
            TVC = VC;
        }

        // Quick out: intersection after desired interval.
        if (tfirst > tmax)
        {
            return false;   
        }

        // Find last time of contact on this axis.
        t = (UC.mMin - VC.mMax)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // Quick out: intersection before desired interval.
        if (tfirst > tlast)
        {
            return false;
        }
    }
    else // V and U on overlapping interval
    {
        if (speed > (Real)0)
        {
            // Find last time of contact on this axis.
            t = (UC.mMax - VC.mMin)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // Quick out: intersection before desired interval.
            if (tfirst > tlast)
            {
                return false;
            }
        }
        else if (speed < (Real)0)
        {
            // Find last time of contact on this axis.
            t = (UC.mMin - VC.mMax)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // Quick out: intersection before desired interval.
            if (tfirst > tlast)
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::TestOverlap (const Vector3<Real>& axis,
    Real tmax, const Vector3<Real>& velocity, Real& tfirst, Real& tlast)
{
    Real min0, max0, min1, max1;
    ProjectOntoAxis(*mTriangle0, axis, min0, max0);
    ProjectOntoAxis(*mTriangle1, axis, min1, max1);
    Real speed = velocity.Dot(axis);
    return TestOverlap(tmax, speed, min0, max0, min1, max1, tfirst, tlast);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Triangle3<Real>::FindOverlap (const Vector3<Real>& axis,
    Real tmax, const Vector3<Real>& velocity, ContactSide& side,
    Configuration& tcfg0, Configuration& tcfg1, Real& tfirst,
    Real& tlast)
{
    Configuration cfg0, cfg1;
    ProjectOntoAxis(*mTriangle0, axis, cfg0);
    ProjectOntoAxis(*mTriangle1, axis, cfg1);
    Real speed = velocity.Dot(axis);
    return FindOverlap(tmax, speed, cfg0, cfg1, side, tcfg0, tcfg1,
        tfirst, tlast);
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle3Triangle3<Real>::ProjectOntoAxis (
    const Triangle3<Real>& triangle, const Vector3<Real>& axis,
    Configuration& cfg)
{
    // Find projections of vertices onto potential separating axis.
    Real d0 = axis.Dot(triangle.V[0]);
    Real d1 = axis.Dot(triangle.V[1]);
    Real d2 = axis.Dot(triangle.V[2]);

    // Explicit sort of vertices to construct a Configuration object.
    if (d0 <= d1)
    {
        if (d1 <= d2) // D0 <= D1 <= D2
        {
            if (d0 != d1)
            {
                if (d1 != d2)
                {
                    cfg.mMap = M111;
                }
                else
                {
                    cfg.mMap = M12;
                }
            }
            else // ( D0 == D1 )
            {
                if (d1 != d2)
                {
                    cfg.mMap = M21;
                }
                else
                {
                    cfg.mMap = M3;
                }
            }
            cfg.mIndex[0] = 0;
            cfg.mIndex[1] = 1;
            cfg.mIndex[2] = 2;
            cfg.mMin = d0;
            cfg.mMax = d2;
        }
        else if (d0 <= d2) // D0 <= D2 < D1
        {
            if (d0 != d2)
            {
                cfg.mMap = M111;
                cfg.mIndex[0] = 0;
                cfg.mIndex[1] = 2;
                cfg.mIndex[2] = 1;
            }
            else
            {
                cfg.mMap = M21;
                cfg.mIndex[0] = 2;
                cfg.mIndex[1] = 0;
                cfg.mIndex[2] = 1;
            }
            cfg.mMin = d0;
            cfg.mMax = d1;
        }
        else // D2 < D0 <= D1
        {
            if (d0 != d1)
            {
                cfg.mMap = M111;
            }
            else
            {
                cfg.mMap = M12;
            }

            cfg.mIndex[0] = 2;
            cfg.mIndex[1] = 0;
            cfg.mIndex[2] = 1;
            cfg.mMin = d2;
            cfg.mMax = d1;
        }
    }
    else if (d2 <= d1) // D2 <= D1 < D0
    {
        if (d2 != d1)
        {
            cfg.mMap = M111;
            cfg.mIndex[0] = 2;
            cfg.mIndex[1] = 1;
            cfg.mIndex[2] = 0;
        }
        else
        {
            cfg.mMap = M21;
            cfg.mIndex[0] = 1;
            cfg.mIndex[1] = 2;
            cfg.mIndex[2] = 0;

        }
        cfg.mMin = d2;
        cfg.mMax = d0;
    }
    else if (d2 <= d0) // D1 < D2 <= D0
    {
        if (d2 != d0) 
        {
            cfg.mMap = M111;
        }
        else
        {
            cfg.mMap = M12;
        }

        cfg.mIndex[0] = 1;
        cfg.mIndex[1] = 2;
        cfg.mIndex[2] = 0;
        cfg.mMin = d1;
        cfg.mMax = d0;
    }
    else // D1 < D0 < D2
    {
        cfg.mMap = M111;
        cfg.mIndex[0] = 1;
        cfg.mIndex[1] = 0;
        cfg.mIndex[2] = 2;
        cfg.mMin = d1;
        cfg.mMax = d2;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle3Triangle3<Real>::FindContactSet (
    const Triangle3<Real>& tri0, const Triangle3<Real>& tri1,
    ContactSide& side, Configuration& cfg0, Configuration& cfg1)
{
    if (side == CS_RIGHT) // tri1 to the right of tri0
    {
        if (cfg0.mMap == M21 || cfg0.mMap == M111)
        {
            // tri0 touching tri1 at a single point
            mIntersectionType = IT_POINT;
            mQuantity = 1;
            mPoint[0] = tri0.V[cfg0.mIndex[2]];
        }
        else if (cfg1.mMap == M12 || cfg1.mMap == M111)
        {
            // tri1 touching tri0 at a single point
            mIntersectionType = IT_POINT;
            mQuantity = 1;
            mPoint[0] = tri1.V[cfg1.mIndex[0]];
        }
        else if (cfg0.mMap == M12)
        {
            if (cfg1.mMap == M21)
            {
                // edge0-edge1 intersection
                GetEdgeEdgeIntersection(
                    tri0.V[cfg0.mIndex[1]], tri0.V[cfg0.mIndex[2]],
                    tri1.V[cfg1.mIndex[0]], tri1.V[cfg1.mIndex[1]]);
            }
            else // cfg1.mMap == m3
            {
                // uedge-vface intersection
                GetEdgeFaceIntersection(
                    tri0.V[cfg0.mIndex[1]], tri0.V[cfg0.mIndex[2]],
                    tri1);
            }
        }
        else // cfg0.mMap == M3
        {
            if (cfg1.mMap == M21)
            {
                // face0-edge1 intersection
                GetEdgeFaceIntersection(
                    tri1.V[cfg1.mIndex[0]], tri1.V[cfg1.mIndex[1]],
                    tri0);
            } 
            else // cfg1.mMap == M3
            {
                // face0-face1 intersection
                Plane3<Real> plane0(tri0.V[0], tri0.V[1], tri0.V[2]);
                GetCoplanarIntersection(plane0, tri0, tri1);
            }
        }
    }
    else if (side == CS_LEFT) // tri1 to the left of tri0
    {
        if (cfg1.mMap == M21 || cfg1.mMap == M111)
        {
            // tri1 touching tri0 at a single point
            mIntersectionType = IT_POINT;
            mQuantity = 1;
            mPoint[0] = tri1.V[cfg1.mIndex[2]];
        }
        else if (cfg0.mMap == M12 || cfg0.mMap == M111)
        {
            // tri0 touching tri1 at a single point
            mIntersectionType = IT_POINT;
            mQuantity = 1;
            mPoint[0] = tri0.V[cfg0.mIndex[0]];
        }
        else if (cfg1.mMap == M12)
        {
            if (cfg0.mMap == M21)
            {
                // edge0-edge1 intersection
                GetEdgeEdgeIntersection(
                    tri0.V[cfg0.mIndex[0]], tri0.V[cfg0.mIndex[1]],
                    tri1.V[cfg1.mIndex[1]], tri1.V[cfg1.mIndex[2]]);
            }
            else // cfg0.mMap == M3
            {
                // edge1-face0 intersection
                GetEdgeFaceIntersection(
                    tri1.V[cfg1.mIndex[1]], tri1.V[cfg1.mIndex[2]],
                    tri0);
            }
        }
        else // cfg1.mMap == M3
        {
            if (cfg0.mMap == M21)
            {
                // edge0-face1 intersection
                GetEdgeFaceIntersection(
                    tri0.V[cfg0.mIndex[0]], tri0.V[cfg0.mIndex[1]],
                    tri1);
            } 
            else // cfg0.mMap == M
            {
                // face0-face1 intersection
                Plane3<Real> plane0(tri0.V[0], tri0.V[1], tri0.V[2]);
                GetCoplanarIntersection(plane0, tri0, tri1);
            }
        }
    }
    else // side == CS_NONE
    {
        // Triangles are already intersecting tranversely.
        IntrTriangle3Triangle3<Real> calc(tri0, tri1);
        bool result = calc.Find();
        assertion(result, "Intersection must exist\n");
        WM5_UNUSED(result);
        mQuantity = calc.GetQuantity();
        mIntersectionType = calc.GetIntersectionType();
        for (int i = 0; i < mQuantity; ++i)
        {
            mPoint[i] = calc.GetPoint(i);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle3Triangle3<Real>::GetEdgeEdgeIntersection (
    const Vector3<Real>& U0, const Vector3<Real>& U1,
    const Vector3<Real>& V0, const Vector3<Real>& V1)
{
    // Compute a normal to the plane of the two edges.
    Vector3<Real> edge0 = U1 - U0;
    Vector3<Real> edge1 = V1 - V0;
    Vector3<Real> normal = edge0.Cross(edge1);

    // Solve U0 + s*(U1 - U0) = V0 + t*(V1 - V0).  We know the edges
    // intersect, so s in [0,1] and t in [0,1].  Thus, just solve for s.
    // Note that s*E0 = D + t*E1, where D = V0 - U0. So s*N = s*E0xE1 = DxE1
    // and s = N*DxE1/N*N.
    Vector3<Real> delta = V0 - U0;
    Real s = normal.Dot(delta.Cross(edge1))/normal.SquaredLength();
    if (s < (Real)0)
    {
        assertion(s >= -Math<Real>::ZERO_TOLERANCE,
            "Unexpected s value.\n");
        s = (Real)0;
    }
    else if (s > (Real)1)
    {
        assertion(s <= (Real)1 + Math<Real>::ZERO_TOLERANCE,
            "Unexpected s value.\n");
        s = (Real)1;
    }

    mIntersectionType = IT_POINT;
    mQuantity = 1;
    mPoint[0] = U0 + s*edge0;

    // TODO:  What if the edges are parallel?
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle3Triangle3<Real>::GetEdgeFaceIntersection (
    const Vector3<Real>& U0, const Vector3<Real>& U1,
    const Triangle3<Real>& tri)
{
    // Compute a plane of the triangle.
    Vector3<Real> point = tri.V[0];
    Vector3<Real> edge0 = tri.V[1] - point;
    Vector3<Real> edge1 = tri.V[2] - point;
    Vector3<Real> normal = edge0.UnitCross(edge1);
    Vector3<Real> dir0, dir1;
    Vector3<Real>::GenerateComplementBasis(dir0, dir1, normal);

    // Project the edge endpoints onto the plane.
    Vector2<Real> projU0, projU1;
    Vector3<Real> diff;
    diff = U0 - point;
    projU0[0] = dir0.Dot(diff);
    projU0[1] = dir1.Dot(diff);
    diff = U1 - point;
    projU1[0] = dir0.Dot(diff);
    projU1[1] = dir1.Dot(diff);
    Segment2<Real> projSeg(projU0, projU1);

    // Compute the plane coordinates of the triangle.
    Triangle2<Real> projTri;
    projTri.V[0] = Vector2<Real>::ZERO;
    projTri.V[1] = Vector2<Real>(dir0.Dot(edge0), dir1.Dot(edge0));
    projTri.V[2] = Vector2<Real>(dir0.Dot(edge1), dir1.Dot(edge1));

    // Compute the intersection.
    IntrSegment2Triangle2<Real> calc(projSeg, projTri);
    if (calc.Find())
    {
        mQuantity = calc.GetQuantity();
        for (int i = 0; i < mQuantity; ++i)
        {
            Vector2<Real> proj = calc.GetPoint(i);
            mPoint[i] = point + proj[0]*dir0 + proj[1]*dir1;
        }
    }
    else
    {
        // There must be an intersection.  Most likely numerical
        // round-off errors have led to a failure to find it.  Use a slower
        // 3D distance calculator for robustness.
        Segment3<Real> seg(U0, U1);
        DistSegment3Triangle3<Real> dcalc(seg, tri);

        // We do not need the distance, but we do need the side effect
        // of locating the closest points.
        Real distance = dcalc.Get();
        WM5_UNUSED(distance);
        Real parameter = dcalc.GetSegmentParameter();
        mQuantity = 1;
        mPoint[0] = seg.Center + parameter*seg.Direction;
    }

    mIntersectionType = (mQuantity == 2 ? IT_SEGMENT : IT_POINT);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrTriangle3Triangle3<float>;

template WM5_MATHEMATICS_ITEM
class IntrTriangle3Triangle3<double>;
//----------------------------------------------------------------------------
}
