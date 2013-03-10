// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrBox3Box3.h"
#include "Wm5IntrUtility3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrBox3Box3<Real>::IntrBox3Box3 (const Box3<Real>& box0,
    const Box3<Real>& box1)
    :
    mBox0(&box0),
    mBox1(&box1)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrBox3Box3<Real>::GetBox0 () const
{
    return *mBox0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrBox3Box3<Real>::GetBox1 () const
{
    return *mBox1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox3Box3<Real>::Test ()
{
    // Cutoff for cosine of angles between box axes.  This is used to catch
    // the cases when at least one pair of axes are parallel.  If this
    // happens, there is no need to test for separation along the
    // Cross(A[i],B[j]) directions.
    const Real cutoff = (Real)1 - Math<Real>::ZERO_TOLERANCE;
    bool existsParallelPair = false;
    int i;

    // Convenience variables.
    const Vector3<Real>* A = mBox0->Axis;
    const Vector3<Real>* B = mBox1->Axis;
    const Real* EA = mBox0->Extent;
    const Real* EB = mBox1->Extent;

    // Compute difference of box centers, D = C1-C0.
    Vector3<Real> D = mBox1->Center - mBox0->Center;

    Real C[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    Real AbsC[3][3];  // |c_{ij}|
    Real AD[3];       // Dot(A_i,D)
    Real r0, r1, r;   // interval radii and distance between centers
    Real r01;         // = R0 + R1

    // axis C0+t*A0
    for (i = 0; i < 3; ++i)
    {
        C[0][i] = A[0].Dot(B[i]);
        AbsC[0][i] = Math<Real>::FAbs(C[0][i]);
        if (AbsC[0][i] > cutoff)
        {
            existsParallelPair = true;
        }
    }
    AD[0] = A[0].Dot(D);
    r = Math<Real>::FAbs(AD[0]);
    r1 = EB[0]*AbsC[0][0] + EB[1]*AbsC[0][1] + EB[2]*AbsC[0][2];
    r01 = EA[0] + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1
    for (i = 0; i < 3; ++i)
    {
        C[1][i] = A[1].Dot(B[i]);
        AbsC[1][i] = Math<Real>::FAbs(C[1][i]);
        if (AbsC[1][i] > cutoff)
        {
            existsParallelPair = true;
        }
    }
    AD[1] = A[1].Dot(D);
    r = Math<Real>::FAbs(AD[1]);
    r1 = EB[0]*AbsC[1][0] + EB[1]*AbsC[1][1] + EB[2]*AbsC[1][2];
    r01 = EA[1] + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2
    for (i = 0; i < 3; ++i)
    {
        C[2][i] = A[2].Dot(B[i]);
        AbsC[2][i] = Math<Real>::FAbs(C[2][i]);
        if (AbsC[2][i] > cutoff)
        {
            existsParallelPair = true;
        }
    }
    AD[2] = A[2].Dot(D);
    r = Math<Real>::FAbs(AD[2]);
    r1 = EB[0]*AbsC[2][0] + EB[1]*AbsC[2][1] + EB[2]*AbsC[2][2];
    r01 = EA[2] + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*B0
    r = Math<Real>::FAbs(B[0].Dot(D));
    r0 = EA[0]*AbsC[0][0] + EA[1]*AbsC[1][0] + EA[2]*AbsC[2][0];
    r01 = r0 + EB[0];
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*B1
    r = Math<Real>::FAbs(B[1].Dot(D));
    r0 = EA[0]*AbsC[0][1] + EA[1]*AbsC[1][1] + EA[2]*AbsC[2][1];
    r01 = r0 + EB[1];
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*B2
    r = Math<Real>::FAbs(B[2].Dot(D));
    r0 = EA[0]*AbsC[0][2] + EA[1]*AbsC[1][2] + EA[2]*AbsC[2][2];
    r01 = r0 + EB[2];
    if (r > r01)
    {
        return false;
    }

    // At least one pair of box axes was parallel, so the separation is
    // effectively in 2D where checking the "edge" normals is sufficient for
    // the separation of the boxes.
    if (existsParallelPair)
    {
        return true;
    }

    // axis C0+t*A0xB0
    r = Math<Real>::FAbs(AD[2]*C[1][0] - AD[1]*C[2][0]);
    r0 = EA[1]*AbsC[2][0] + EA[2]*AbsC[1][0];
    r1 = EB[1]*AbsC[0][2] + EB[2]*AbsC[0][1];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A0xB1
    r = Math<Real>::FAbs(AD[2]*C[1][1] - AD[1]*C[2][1]);
    r0 = EA[1]*AbsC[2][1] + EA[2]*AbsC[1][1];
    r1 = EB[0]*AbsC[0][2] + EB[2]*AbsC[0][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A0xB2
    r = Math<Real>::FAbs(AD[2]*C[1][2] - AD[1]*C[2][2]);
    r0 = EA[1]*AbsC[2][2] + EA[2]*AbsC[1][2];
    r1 = EB[0]*AbsC[0][1] + EB[1]*AbsC[0][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1xB0
    r = Math<Real>::FAbs(AD[0]*C[2][0] - AD[2]*C[0][0]);
    r0 = EA[0]*AbsC[2][0] + EA[2]*AbsC[0][0];
    r1 = EB[1]*AbsC[1][2] + EB[2]*AbsC[1][1];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1xB1
    r = Math<Real>::FAbs(AD[0]*C[2][1] - AD[2]*C[0][1]);
    r0 = EA[0]*AbsC[2][1] + EA[2]*AbsC[0][1];
    r1 = EB[0]*AbsC[1][2] + EB[2]*AbsC[1][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1xB2
    r = Math<Real>::FAbs(AD[0]*C[2][2] - AD[2]*C[0][2]);
    r0 = EA[0]*AbsC[2][2] + EA[2]*AbsC[0][2];
    r1 = EB[0]*AbsC[1][1] + EB[1]*AbsC[1][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2xB0
    r = Math<Real>::FAbs(AD[1]*C[0][0] - AD[0]*C[1][0]);
    r0 = EA[0]*AbsC[1][0] + EA[1]*AbsC[0][0];
    r1 = EB[1]*AbsC[2][2] + EB[2]*AbsC[2][1];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2xB1
    r = Math<Real>::FAbs(AD[1]*C[0][1] - AD[0]*C[1][1]);
    r0 = EA[0]*AbsC[1][1] + EA[1]*AbsC[0][1];
    r1 = EB[0]*AbsC[2][2] + EB[2]*AbsC[2][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2xB2
    r = Math<Real>::FAbs(AD[1]*C[0][2] - AD[0]*C[1][2]);
    r0 = EA[0]*AbsC[1][2] + EA[1]*AbsC[0][2];
    r1 = EB[0]*AbsC[2][1] + EB[1]*AbsC[2][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox3Box3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    if (velocity0 == velocity1)
    {
        if (Test())
        {
            mContactTime = (Real)0;
            return true;
        }
        return false;
    }

    // Cutoff for cosine of angles between box axes.  This is used to catch
    // the cases when at least one pair of axes are parallel.  If this
    // happens, there is no need to include the cross-product axes for
    // separation.
    const Real cutoff = (Real)1 - Math<Real>::ZERO_TOLERANCE;
    bool existsParallelPair = false;

    // convenience variables
    const Vector3<Real>* A = mBox0->Axis;
    const Vector3<Real>* B = mBox1->Axis;
    const Real* EA = mBox0->Extent;
    const Real* EB = mBox1->Extent;
    Vector3<Real> D = mBox1->Center - mBox0->Center;
    Vector3<Real> W = velocity1 - velocity0;
    Real C[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    Real AbsC[3][3];  // |c_{ij}|
    Real AD[3];       // Dot(A_i,D)
    Real AW[3];       // Dot(A_i,W)
    Real min0, max0, min1, max1, center, radius, speed;
    int i, j;

    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    // axes C0+t*A[i]
    for (i = 0; i < 3; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            C[i][j] = A[i].Dot(B[j]);
            AbsC[i][j] = Math<Real>::FAbs(C[i][j]);
            if (AbsC[i][j] > cutoff)
            {
                existsParallelPair = true;
            }
        }
        AD[i] = A[i].Dot(D);
        AW[i] = A[i].Dot(W);
        min0 = -EA[i];
        max0 = +EA[i];
        radius = EB[0]*AbsC[i][0] + EB[1]*AbsC[i][1] + EB[2]*AbsC[i][2];
        min1 = AD[i] - radius;
        max1 = AD[i] + radius;
        speed = AW[i];
        if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
        {
            return false;
        }
    }

    // axes C0+t*B[i]
    for (i = 0; i < 3; ++i)
    {
        radius = EA[0]*AbsC[0][i] + EA[1]*AbsC[1][i] + EA[2]*AbsC[2][i];
        min0 = -radius;
        max0 = +radius;
        center = B[i].Dot(D);
        min1 = center - EB[i];
        max1 = center + EB[i];
        speed = W.Dot(B[i]);
        if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
        {
            return false;
        }
    }

    // At least one pair of box axes was parallel, so the separation is
    // effectively in 2D where checking the "edge" normals is sufficient for
    // the separation of the boxes.
    if (existsParallelPair)
    {
        return true;
    }

    // axis C0+t*A0xB0
    radius = EA[1]*AbsC[2][0] + EA[2]*AbsC[1][0];
    min0 = -radius;
    max0 = +radius;
    center = AD[2]*C[1][0] - AD[1]*C[2][0];
    radius = EB[1]*AbsC[0][2] + EB[2]*AbsC[0][1];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[2]*C[1][0] - AW[1]*C[2][0];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A0xB1
    radius = EA[1]*AbsC[2][1] + EA[2]*AbsC[1][1];
    min0 = -radius;
    max0 = +radius;
    center = AD[2]*C[1][1] - AD[1]*C[2][1];
    radius = EB[0]*AbsC[0][2] + EB[2]*AbsC[0][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[2]*C[1][1] - AW[1]*C[2][1];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A0xB2
    radius = EA[1]*AbsC[2][2] + EA[2]*AbsC[1][2];
    min0 = -radius;
    max0 = +radius;
    center = AD[2]*C[1][2] - AD[1]*C[2][2];
    radius = EB[0]*AbsC[0][1] + EB[1]*AbsC[0][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[2]*C[1][2] - AW[1]*C[2][2];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A1xB0
    radius = EA[0]*AbsC[2][0] + EA[2]*AbsC[0][0];
    min0 = -radius;
    max0 = +radius;
    center = AD[0]*C[2][0] - AD[2]*C[0][0];
    radius = EB[1]*AbsC[1][2] + EB[2]*AbsC[1][1];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[0]*C[2][0] - AW[2]*C[0][0];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A1xB1
    radius = EA[0]*AbsC[2][1] + EA[2]*AbsC[0][1];
    min0 = -radius;
    max0 = +radius;
    center = AD[0]*C[2][1] - AD[2]*C[0][1];
    radius = EB[0]*AbsC[1][2] + EB[2]*AbsC[1][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[0]*C[2][1] - AW[2]*C[0][1];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A1xB2
    radius = EA[0]*AbsC[2][2] + EA[2]*AbsC[0][2];
    min0 = -radius;
    max0 = +radius;
    center = AD[0]*C[2][2] - AD[2]*C[0][2];
    radius = EB[0]*AbsC[1][1] + EB[1]*AbsC[1][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[0]*C[2][2] - AW[2]*C[0][2];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A2xB0
    radius = EA[0]*AbsC[1][0] + EA[1]*AbsC[0][0];
    min0 = -radius;
    max0 = +radius;
    center = AD[1]*C[0][0] - AD[0]*C[1][0];
    radius = EB[1]*AbsC[2][2] + EB[2]*AbsC[2][1];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[1]*C[0][0] - AW[0]*C[1][0];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A2xB1
    radius = EA[0]*AbsC[1][1] + EA[1]*AbsC[0][1];
    min0 = -radius;
    max0 = +radius;
    center = AD[1]*C[0][1] - AD[0]*C[1][1];
    radius = EB[0]*AbsC[2][2] + EB[2]*AbsC[2][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[1]*C[0][1] - AW[0]*C[1][1];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    // axis C0+t*A2xB2
    radius = EA[0]*AbsC[1][2] + EA[1]*AbsC[0][2];
    min0 = -radius;
    max0 = +radius;
    center = AD[1]*C[0][2] - AD[0]*C[1][2];
    radius = EB[0]*AbsC[2][1] + EB[1]*AbsC[2][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[1]*C[0][2] - AW[0]*C[1][2];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast))
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox3Box3<Real>::Find (Real tmax, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    mQuantity = 0;

    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    // Relative velocity of box1 relative to box0.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    int i0, i1;
    int side = IntrConfiguration<Real>::NONE;
    IntrConfiguration<Real> box0Cfg, box1Cfg;
    Vector3<Real> axis;

    // box 0 normals
    for (i0 = 0; i0 < 3; ++i0)
    {
        axis = mBox0->Axis[i0];
        if (!IntrAxis<Real>::Find(axis, *mBox0, *mBox1, relVelocity, tmax,
            mContactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    // box 1 normals
    for (i1 = 0; i1 < 3; ++i1)
    {
        axis = mBox1->Axis[i1];
        if (!IntrAxis<Real>::Find(axis, *mBox0, *mBox1, relVelocity, tmax,
            mContactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    // box 0 edges cross box 1 edges
    for (i0 = 0; i0 < 3; ++i0)
    {
        for (i1 = 0; i1 < 3; ++i1)
        {
            axis = mBox0->Axis[i0].Cross(mBox1->Axis[i1]);

            // Since all axes are unit length (assumed), then can just compare
            // against a constant (not relative) epsilon.
            if (axis.SquaredLength() <= Math<Real>::ZERO_TOLERANCE)
            {
                // Axis i0 and i1 are parallel.  If any two axes are parallel,
                // then the only comparisons that needed are between the faces
                // themselves.  At this time the faces have already been
                // tested, and without separation, so all further separation
                // tests will show only overlaps.
                FindContactSet<Real>(*mBox0, *mBox1, side, box0Cfg, box1Cfg,
                    velocity0, velocity1, mContactTime, mQuantity, mPoint);
                return true;
            }

            if (!IntrAxis<Real>::Find(axis, *mBox0, *mBox1, relVelocity,
                tmax, mContactTime, tlast, side, box0Cfg, box1Cfg))
            {
                return false;
            }
        }
    }

    // velocity cross box 0 edges
    for (i0 = 0; i0 < 3; ++i0)
    {
        axis = relVelocity.Cross(mBox0->Axis[i0]);
        if (!IntrAxis<Real>::Find(axis, *mBox0, *mBox1, relVelocity, tmax,
            mContactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    // velocity cross box 1 edges
    for (i1 = 0; i1 < 3; ++i1)
    {
        axis = relVelocity.Cross(mBox1->Axis[i1]);
        if (!IntrAxis<Real>::Find(axis, *mBox0, *mBox1, relVelocity, tmax,
            mContactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    if (mContactTime <= (Real)0 || side == IntrConfiguration<Real>::NONE)
    {
        return false;
    }

    FindContactSet<Real>(*mBox0, *mBox1, side, box0Cfg, box1Cfg,
        velocity0, velocity1, mContactTime, mQuantity, mPoint);

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrBox3Box3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrBox3Box3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox3Box3<Real>::Test (Real tmax, int numSteps,
    const Vector3<Real>& velocity0, const Vector3<Real>& rotCenter0,
    const Vector3<Real>& rotAxis0, const Vector3<Real>& velocity1,
    const Vector3<Real>& rotCenter1, const Vector3<Real>& rotAxis1)
{
    // The time step for the integration.
    Real stepsize = tmax/(Real)numSteps;

    // Initialize subinterval boxes.
    Box3<Real> subBox0, subBox1;
    subBox0.Center = mBox0->Center;
    subBox1.Center = mBox1->Center;
    int i;
    for (i = 0; i < 3; ++i)
    {
        subBox0.Axis[i] = mBox0->Axis[i];
        subBox0.Extent[i] = mBox0->Extent[i];
        subBox1.Axis[i] = mBox1->Axis[i];
        subBox1.Extent[i] = mBox1->Extent[i];
    }

    // Integrate the differential equations using Euler's method.
    for (int istep = 1; istep <= numSteps; ++istep)
    {
        // Compute box velocities and test boxes for intersection.
        Real subTime = stepsize*(Real)istep;
        Vector3<Real> newRotCenter0 = rotCenter0 + subTime*velocity0;
        Vector3<Real> newRotCenter1 = rotCenter1 + subTime*velocity1;
        Vector3<Real> diff0 = subBox0.Center - newRotCenter0;
        Vector3<Real> diff1 = subBox1.Center - newRotCenter1;
        Vector3<Real> subVelocity0 =
            stepsize*(velocity0 + rotAxis0.Cross(diff0));
        Vector3<Real> subVelocity1 =
            stepsize*(velocity1 + rotAxis1.Cross(diff1));

        IntrBox3Box3 calc(subBox0, subBox1);
        if (calc.Test(stepsize, subVelocity0, subVelocity1))
        {
            return true;
        }

        // Update the box centers.
        subBox0.Center = subBox0.Center + subVelocity0;
        subBox1.Center = subBox1.Center + subVelocity1;

        // Update the box axes.
        for (i = 0; i < 3; ++i)
        {
            subBox0.Axis[i] = subBox0.Axis[i] +
                stepsize*rotAxis0.Cross(subBox0.Axis[i]);

            subBox1.Axis[i] = subBox1.Axis[i] +
                stepsize*rotAxis1.Cross(subBox1.Axis[i]);
        }

        // Use Gram-Schmidt to orthonormalize the updated axes.  NOTE:  If
        // T/N is small and N is small, you can remove this expensive stepsize
        // with the assumption that the updated axes are nearly orthonormal.
        Vector3<Real>::Orthonormalize(subBox0.Axis);
        Vector3<Real>::Orthonormalize(subBox1.Axis);
    }

    // NOTE:  If the boxes do not intersect, then the application might want
    // to move/rotate the boxes to their new locations.  In this case you
    // want to return the final values of subBox0 and subBox1 so that the
    // application can set rkBox0 <- subBox0 and rkBox1 <- subBox1.
    // Otherwise, the application would have to solve the differential
    // equation again or compute the new box locations using the closed form
    // solution for the rigid motion.

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox3Box3<Real>::IsSeparated (Real min0, Real max0, Real min1,
    Real max1, Real speed, Real tmax, Real& tlast)
{
    Real invSpeed, t;

    if (max1 < min0) // box1 initially on left of box0
    {
        if (speed <= (Real)0)
        {
            // The projection intervals are moving apart.
            return true;
        }
        invSpeed = ((Real)1)/speed;

        t = (min0 - max1)*invSpeed;
        if (t > mContactTime)
        {
            mContactTime = t;
        }

        if (mContactTime > tmax)
        {
            // Intervals do not intersect during the specified time.
            return true;
        }

        t = (max0 - min1)*invSpeed;
        if (t < tlast)
        {
            tlast = t;
        }

        if (mContactTime > tlast)
        {
            // Physically inconsistent times--the objects cannot intersect.
            return true;
        }
    }
    else if (max0 < min1) // box1 initially on right of box0
    {
        if (speed >= (Real)0)
        {
            // The projection intervals are moving apart.
            return true;
        }
        invSpeed = ((Real)1)/speed;

        t = (max0 - min1)*invSpeed;
        if (t > mContactTime)
        {
            mContactTime = t;
        }

        if (mContactTime > tmax)
        {
            // Intervals do not intersect during the specified time.
            return true;
        }

        t = (min0 - max1)*invSpeed;
        if (t < tlast)
        {
            tlast = t;
        }

        if (mContactTime > tlast)
        {
            // Physically inconsistent times--the objects cannot intersect.
            return true;
        }
    }
    else // box0 and box1 initially overlap
    {
        if (speed > (Real)0)
        {
            t = (max0 - min1)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            if (mContactTime > tlast)
            {
                // Physically inconsistent times--the objects cannot
                // intersect.
                return true;
            }
        }
        else if (speed < (Real)0)
        {
            t = (min0 - max1)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            if (mContactTime > tlast)
            {
                // Physically inconsistent times--the objects cannot
                // intersect.
                return true;
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
class IntrBox3Box3<float>;

template WM5_MATHEMATICS_ITEM
class IntrBox3Box3<double>;
//----------------------------------------------------------------------------
}
