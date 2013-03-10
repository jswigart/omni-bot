// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/11/02)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrTriangle3Cone3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrTriangle3Cone3<Real>::IntrTriangle3Cone3 (
    const Triangle3<Real>& triangle, const Cone3<Real>& cone)
    :
    mTriangle(&triangle),
    mCone(&cone)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrTriangle3Cone3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
const Cone3<Real>& IntrTriangle3Cone3<Real>::GetCone () const
{
    return *mCone;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Cone3<Real>::Test ()
{
    // Triangle is <P0,P1,P2>, edges are E0 = P1-P0, E1=P2-P0.
    int onConeSide = 0;
    Real p0Test = (Real)0, p1Test = (Real)0, p2Test = (Real)0;
    Real AdE, EdE, EdD, c1, c2;

    Real cosSqr = mCone->CosAngle*mCone->CosAngle;

    // Test vertex P0.
    Vector3<Real> diff0 = mTriangle->V[0] - mCone->Vertex;
    Real AdD0 = mCone->Axis.Dot(diff0);
    if (AdD0 >= (Real)0)
    {
        // P0 is on cone side of plane.
        p0Test = AdD0*AdD0 - cosSqr*(diff0.Dot(diff0));
        if (p0Test >= (Real)0)
        {
            // P0 is inside the cone.
            return true;
        }
        else
        {
            // P0 is outside the cone, but on cone side of plane.
            onConeSide |= 1;
        }
    }
    // else P0 is not on cone side of plane.

    // Test vertex P1.
    Vector3<Real> edge0 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> diff1 = diff0 + edge0;
    Real AdD1 = mCone->Axis.Dot(diff1);
    if (AdD1 >= (Real)0)
    {
        // P1 is on cone side of plane.
        p1Test = AdD1*AdD1 - cosSqr*(diff1.Dot(diff1));
        if (p1Test >= (Real)0)
        {
            // P1 is inside the cone.
            return true;
        }
        else
        {
            // P1 is outside the cone, but on cone side of plane.
            onConeSide |= 2;
        }
    }
    // else P1 is not on cone side of plane.

    // Test vertex P2.
    Vector3<Real> edge1 = mTriangle->V[2] - mTriangle->V[0];
    Vector3<Real> diff2 = diff0 + edge1;
    Real AdD2 = mCone->Axis.Dot(diff2);
    if (AdD2 >= (Real)0)
    {
        // P2 is on cone side of plane.
        p2Test = AdD2*AdD2 - cosSqr*(diff2.Dot(diff2));
        if (p2Test >= (Real)0)
        {
            // P2 is inside the cone.
            return true;
        }
        else
        {
            // P2 is outside the cone, but on cone side of plane.
            onConeSide |= 4;
        }
    }
    // else P2 is not on cone side of plane.

    // Test edge <P0,P1> = E0.
    if (onConeSide & 3)
    {
        AdE = AdD1 - AdD0;
        EdE = edge0.Dot(edge0);
        c2 = AdE*AdE - cosSqr*EdE;
        if (c2 < (Real)0)
        {
            EdD = edge0.Dot(diff0);
            c1 = AdE*AdD0 - cosSqr*EdD;
            if (onConeSide & 1)
            {
                if (onConeSide & 2)
                {
                    // <P0,P1> fully on cone side of plane, fC0 = p0Test.
                    if ((Real)0 <= c1 && c1 <= -c2 && c1*c1 >= p0Test*c2)
                    {
                        return true;
                    }
                }
                else
                {
                    // P0 on cone side (Dot(A,P0-V) >= 0),
                    // P1 on opposite side (Dot(A,P1-V) <= 0)
                    // (Dot(A,E0) <= 0), fC0 = p0Test.
                    if ((Real)0 <= c1 && c2*AdD0 <= c1*AdE
                    &&  c1*c1 >= p0Test*c2)
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P1 on cone side (Dot(A,P1-V) >= 0),
                // P0 on opposite side (Dot(A,P0-V) <= 0)
                // (Dot(A,E0) >= 0), fC0 = p0Test (needs calculating).
                if (c1 <= -c2 && c2*AdD0 <= c1*AdE)
                {
                    p0Test = AdD0*AdD0 - cosSqr*(diff0.Dot(diff0));
                    if (c1*c1 >= p0Test*c2)
                    {
                        return true;
                    }
                }
            }
        }
    }
    // else <P0,P1> does not intersect cone half space.

    // Test edge <P0,P2> = E1.
    if (onConeSide & 5)
    {
        AdE = AdD2 - AdD0;
        EdE = edge1.Dot(edge1);
        c2 = AdE*AdE - cosSqr*EdE;
        if (c2 < (Real)0)
        {
            EdD = edge1.Dot(diff0);
            c1 = AdE*AdD0 - cosSqr*EdD;
            if (onConeSide & 1)
            {
                if (onConeSide & 4)
                {
                    // <P0,P2> fully on cone side of plane, fC0 = p0Test.
                    if ((Real)0.0 <= c1 && c1 <= -c2 && c1*c1 >= p0Test*c2)
                    {
                        return true;
                    }
                }
                else
                {
                    // P0 on cone side (Dot(A,P0-V) >= 0),
                    // P2 on opposite side (Dot(A,P2-V) <= 0)
                    // (Dot(A,E1) <= 0), fC0 = p0Test.
                    if ((Real)0.0 <= c1 && c2*AdD0 <= c1*AdE
                    &&  c1*c1 >= p0Test*c2)
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P2 on cone side (Dot(A,P2-V) >= 0),
                // P0 on opposite side (Dot(A,P0-V) <= 0)
                // (Dot(A,E1) >= 0), fC0 = p0Test (needs calculating).
                if (c1 <= -c2 && c2*AdD0 <= c1*AdE)
                {
                    p0Test = AdD0*AdD0 - cosSqr*(diff0.Dot(diff0));
                    if (c1*c1 >= p0Test*c2)
                    {
                        return true;
                    }
                }
            }
        }
    }
    // else <P0,P2> does not intersect cone half space.

    // Test edge <P1,P2> = E1-E0 = E2.
    if (onConeSide & 6)
    {
        Vector3<Real> kE2 = edge1 - edge0;
        AdE = AdD2 - AdD1;
        EdE = kE2.Dot(kE2);
        c2 = AdE*AdE - cosSqr*EdE;
        if (c2 < (Real)0.0)
        {
            EdD = kE2.Dot(diff1);
            c1 = AdE*AdD1 - cosSqr*EdD;
            if (onConeSide & 2)
            {
                if (onConeSide & 4)
                {
                    // <P1,P2> fully on cone side of plane, fC0 = p1Test.
                    if ((Real)0.0 <= c1 && c1 <= -c2 && c1*c1 >= p1Test*c2)
                    {
                        return true;
                    }
                }
                else
                {
                    // P1 on cone side (Dot(A,P1-V) >= 0),
                    // P2 on opposite side (Dot(A,P2-V) <= 0)
                    // (Dot(A,E2) <= 0), fC0 = p1Test.
                    if ((Real)0.0 <= c1 && c2*AdD1 <= c1*AdE
                    &&  c1*c1 >= p1Test*c2)
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P2 on cone side (Dot(A,P2-V) >= 0),
                // P1 on opposite side (Dot(A,P1-V) <= 0)
                // (Dot(A,E2) >= 0), fC0 = p1Test (needs calculating).
                if (c1 <= -c2 && c2*AdD1 <= c1*AdE)
                {
                    p1Test = AdD1*AdD1 - cosSqr*(diff1.Dot(diff1));
                    if (c1*c1 >= p1Test*c2)
                    {
                        return true;
                    }
                }
            }
        }
    }
    // else <P1,P2> does not intersect cone half space.

    // Test triangle <P0,P1,P2>.  It is enough to handle only the case when
    // at least one Pi is on the cone side of the plane.  In this case and
    // after the previous testing, if the triangle intersects the cone, the
    // set of intersection must contain the point of intersection between
    // the cone axis and the triangle.
    if (onConeSide > 0)
    {
        Vector3<Real> N = edge0.Cross(edge1);
        Real NdA = N.Dot(mCone->Axis);
        Real NdD = N.Dot(diff0);
        Vector3<Real> U = NdD*mCone->Axis - NdA*diff0;
        Vector3<Real> NcU = N.Cross(U);

        Real NcUdE0 = NcU.Dot(edge0), NcUdE1, NcUdE2, NdN;
        if (NdA >= (Real)0)
        {
            if (NcUdE0 <= (Real)0)
            {
                NcUdE1 = NcU.Dot(edge1);
                if (NcUdE1 >= (Real)0)
                {
                    NcUdE2 = NcUdE1 - NcUdE0;
                    NdN = N.SquaredLength();
                    if (NcUdE2 <= NdA*NdN)
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            if (NcUdE0 >= (Real)0)
            {
                NcUdE1 = NcU.Dot(edge1);
                if (NcUdE1 <= (Real)0)
                {
                    NcUdE2 = NcUdE1 - NcUdE0;
                    NdN = N.SquaredLength();
                    if (NcUdE2 >= NdA*NdN)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrTriangle3Cone3<float>;

template WM5_MATHEMATICS_ITEM
class IntrTriangle3Cone3<double>;
//----------------------------------------------------------------------------
}
