// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/06/19)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Ellipsoid3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Ellipsoid3<Real>::DistPoint3Ellipsoid3 (const Vector3<Real>& point,
    const Ellipsoid3<Real>& ellipsoid)
    :
    mPoint(&point),
    mEllipsoid(&ellipsoid)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Ellipsoid3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipsoid3<Real>& DistPoint3Ellipsoid3<Real>::GetEllipsoid ()
    const
{
    return *mEllipsoid;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ellipsoid3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ellipsoid3<Real>::GetSquared ()
{
    // Compute coordinates of point in ellipsoid coordinate system.
    Vector3<Real> diff = *mPoint - mEllipsoid->Center;
    Vector3<Real> y(diff.Dot(mEllipsoid->Axis[0]),
        diff.Dot(mEllipsoid->Axis[1]), diff.Dot(mEllipsoid->Axis[2]));
    Vector3<Real> x;

    Real sqrDistance = SqrDistance(mEllipsoid->Extent, y, x);

    mClosestPoint0 = *mPoint;
    mClosestPoint1 = mEllipsoid->Center +
        x[0]*mEllipsoid->Axis[0] +
        x[1]*mEllipsoid->Axis[1] +
        x[2]*mEllipsoid->Axis[2];

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ellipsoid3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mEllipsoid->Center + t*velocity1;
    Ellipsoid3<Real> movedEllipsoid(movedCenter, mEllipsoid->Axis,
        mEllipsoid->Extent);
    return DistPoint3Ellipsoid3<Real>(movedPoint,movedEllipsoid).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ellipsoid3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mEllipsoid->Center + t*velocity1;
    Ellipsoid3<Real> movedEllipsoid(movedCenter, mEllipsoid->Axis,
        mEllipsoid->Extent);
    return DistPoint3Ellipsoid3<Real>(movedPoint,movedEllipsoid).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ellipsoid3<Real>::SqrDistanceSpecial (const Real e[3],
    const Real y[3], Real x[3])
{
    Real sqrDistance;

    Real ePos[3], yPos[3], xPos[3];
    int numPos = 0;
    int i;
    for (i = 0; i < 3; ++i)
    {
        if (y[i] > (Real)0)
        {
            ePos[numPos] = e[i];
            yPos[numPos] = y[i];
            ++numPos;
        }
        else
        {
            x[i] = (Real)0;
        }
    }

    if (y[2] > (Real)0)
    {
        sqrDistance = Bisector(numPos, ePos, yPos, xPos);
    }
    else  // y[2] = 0
    {
        Real e2Sqr = e[2]*e[2];
        Real denom[2], ey[2];
        for (i = 0; i < numPos; ++i)
        {
            denom[i] = ePos[i]*ePos[i] - e2Sqr;
            ey[i] = ePos[i]*yPos[i];
        }

        bool inAABBSubEllipse = true;
        for (i = 0; i < numPos; ++i)
        {
            if (ey[i] >= denom[i])
            {
                inAABBSubEllipse = false;
                break;
            }
        }

        // Initialize to avoid C4701 in MSVS 2008.  The compiler cannot
        // determine that sqrDistance is assigned a value in all cases.
        sqrDistance = (Real)0;

        bool inSubEllipse = false;
        if (inAABBSubEllipse)
        {
            // yPos[] is inside the axis-aligned bounding box of the
            // subellipse.  This intermediate test is designed to guard
            // against the division by zero when ePos[i] == e[N-1] for some i.
            Real xde[2], discr = (Real)1;
            for (i = 0; i < numPos; ++i)
            {
                xde[i] = ey[i]/denom[i];
                discr -= xde[i]*xde[i];
            }
            if (discr > (Real)0)
            {
                // yPos[] is inside the subellipse.  The closest ellipsoid
                // point has x[2] > 0.
                sqrDistance = (Real)0;
                for (i = 0; i < numPos; ++i)
                {
                    xPos[i] = ePos[i]*xde[i];
                    Real diff = xPos[i] - yPos[i];
                    sqrDistance += diff*diff;
                }
                x[2] = e[2]*sqrt(discr);
                sqrDistance += x[2]*x[2];
                inSubEllipse = true;
            }
        }

        if (!inSubEllipse)
        {
            // yPos[] is outside the subellipse.  The closest ellipsoid
            // point has x[2] == 0 and is on the domain-boundary ellipse.
            x[2] = (Real)0;
            sqrDistance = Bisector(numPos, ePos, yPos, xPos);
        }
    }

    // Fill in those x[] values that were not zeroed out initially.
    for (i = 0, numPos = 0; i < 3; ++i)
    {
        if (y[i] > (Real)0)
        {
            x[i] = xPos[numPos];
            ++numPos;
        }
    }

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ellipsoid3<Real>::SqrDistance (const Real e[3],
    const Real y[3], Real x[3])
{
    // Determine reflections for y to the first octant.
    bool reflect[3];
    int i, j;
    for (i = 0; i < 3; ++i)
    {
        reflect[i] = (y[i] < (Real)0);
    }

    // Determine the axis order for decreasing extents.
    int permute[3];
    if (e[0] < e[1])
    {
        if (e[2] < e[0])
        {
            permute[0] = 1;  permute[1] = 0;  permute[2] = 2;
        }
        else if (e[2] < e[1])
        {
            permute[0] = 1;  permute[1] = 2;  permute[2] = 0;
        }
        else
        {
            permute[0] = 2;  permute[1] = 1;  permute[2] = 0;
        }
    }
    else
    {
        if (e[2] < e[1])
        {
            permute[0] = 0;  permute[1] = 1;  permute[2] = 2;
        }
        else if (e[2] < e[0])
        {
            permute[0] = 0;  permute[1] = 2;  permute[2] = 1;
        }
        else
        {
            permute[0] = 2;  permute[1] = 0;  permute[2] = 1;
        }
    }

    int invpermute[3];
    for (i = 0; i < 3; ++i)
    {
        invpermute[permute[i]] = i;
    }

    Real locE[3], locY[3];
    for (i = 0; i < 3; ++i)
    {
        j = permute[i];
        locE[i] = e[j];
        locY[i] = y[j];
        if (reflect[j])
        {
            locY[i] = -locY[i];
        }
    }

    Real locX[3];
    Real sqrDistance = SqrDistanceSpecial(locE, locY, locX);

    // Restore the axis order and reflections.
    for (i = 0; i < 3; ++i)
    {
        j = invpermute[i];
        if (reflect[j])
        {
            locX[j] = -locX[j];
        }
        x[i] = locX[j];
    }

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ellipsoid3<Real>::Bisector (int numComponents, const Real* e,
    const Real* y, Real* x)
{
    Real esqr[3], ey[3], argument = (Real)0;
    int i;
    for (i = 0; i < numComponents; ++i)
    {
        esqr[i] = e[i]*e[i];
        ey[i] = e[i]*y[i];
        argument += ey[i]*ey[i];
    }

    Real t0 = -esqr[numComponents-1] + ey[numComponents-1];
    Real t1 = -esqr[numComponents-1] + sqrt(argument);
    Real t = t0;
    const int jmax = 2*std::numeric_limits<Real>::max_exponent;
    for (int j = 0; j < jmax; ++j)
    {
        t = ((Real)0.5)*(t0 + t1);
        if (t == t0 || t == t1)
        {
            break;
        }

        Real f = (Real)-1;
        for (i = 0; i < numComponents; ++i)
        {
            Real r = ey[i]/(t + esqr[i]);
            f += r*r;
        }
        if (f > (Real)0)
        {
            t0 = t;
        }
        else if (f < (Real)0)
        {
            t1 = t;
        }
        else
        {
            break;
        }
    }

    Real sqrDistance = (Real)0;
    for (i = 0; i < numComponents; ++i)
    {
        x[i] = esqr[i]*y[i]/(t + esqr[i]);
        Real diff = x[i] - y[i];
        sqrDistance += diff*diff;
    }
    return sqrDistance;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Ellipsoid3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
