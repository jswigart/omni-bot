// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Frustum3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Frustum3<Real>::DistPoint3Frustum3 (const Vector3<Real>& point,
    const Frustum3<Real>& frustum)
    :
    mPoint(&point),
    mFrustum(&frustum)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Frustum3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Frustum3<Real>& DistPoint3Frustum3<Real>::GetFrustum () const
{
    return *mFrustum;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Frustum3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Frustum3<Real>::GetSquared ()
{
    // Compute coordinates of point with respect to frustum coordinate system.
    Vector3<Real> diff = *mPoint - mFrustum->Origin;
    Vector3<Real> test = Vector3<Real>(
        diff.Dot(mFrustum->RVector),
        diff.Dot(mFrustum->UVector),
        diff.Dot(mFrustum->DVector));

    // Perform calculations in octant with nonnegative R and U coordinates.
    bool rSignChange;
    if (test.X() < (Real)0)
    {
        rSignChange = true;
        test.X() = -test.X();
    }
    else
    {
        rSignChange = false;
    }

    bool uSignChange;
    if (test.Y() < (Real)0)
    {
        uSignChange = true;
        test.Y() = -test.Y();
    }
    else
    {
        uSignChange = false;
    }

    // Frustum derived parameters.
    Real rmin = mFrustum->RBound;
    Real rmax = mFrustum->GetDRatio()*rmin;
    Real umin = mFrustum->UBound;
    Real umax = mFrustum->GetDRatio()*umin;
    Real dmin = mFrustum->DMin;
    Real dmax = mFrustum->DMax;
    Real rminSqr = rmin*rmin;
    Real uminSqr = umin*umin;
    Real dminSqr = dmin*dmin;
    Real minRDDot = rminSqr + dminSqr;
    Real minUDDot = uminSqr + dminSqr;
    Real minRUDDot = rminSqr + minUDDot;
    Real maxRDDot = mFrustum->GetDRatio()*minRDDot;
    Real maxUDDot = mFrustum->GetDRatio()*minUDDot;
    Real maxRUDDot = mFrustum->GetDRatio()*minRUDDot;

    // Algorithm computes closest point in all cases by determining in which
    // Voronoi region of the vertices, edges, and faces of the frustum that
    // the test point lives.
    Vector3<Real> closest;
    Real rDot, uDot, rdDot, udDot, rudDot, rEdgeDot, uEdgeDot, t;
    if (test.Z() >= dmax)
    {
        if (test.X() <= rmax)
        {
            if (test.Y() <= umax)
            {
                // F-face
                closest.X() = test.X();
                closest.Y() = test.Y();
                closest.Z() = dmax;
            }
            else
            {
                // UF-edge
                closest.X() = test.X();
                closest.Y() = umax;
                closest.Z() = dmax;
            }
        }
        else
        {
            if (test.Y() <= umax)
            {
                // LF-edge
                closest.X() = rmax;
                closest.Y() = test.Y();
                closest.Z() = dmax;
            }
            else
            {
                // LUF-vertex
                closest.X() = rmax;
                closest.Y() = umax;
                closest.Z() = dmax;
            }
        }
    }
    else if (test.Z() <= dmin)
    {
        if (test.X() <= rmin)
        {
            if (test.Y() <= umin)
            {
                // N-face
                closest.X() = test.X();
                closest.Y() = test.Y();
                closest.Z() = dmin;
            }
            else
            {
                udDot = umin*test.Y() + dmin*test.Z();
                if (udDot >= maxUDDot)
                {
                    // UF-edge
                    closest.X() = test.X();
                    closest.Y() = umax;
                    closest.Z() = dmax;
                }
                else if (udDot >= minUDDot)
                {
                    // U-face
                    uDot = dmin*test.Y() - umin*test.Z();
                    t = uDot/minUDDot;
                    closest.X() = test.X();
                    closest.Y() = test.Y() - t*dmin;
                    closest.Z() = test.Z() + t*umin;
                }
                else
                {
                    // UN-edge
                    closest.X() = test.X();
                    closest.Y() = umin;
                    closest.Z() = dmin;
                }
            }
        }
        else
        {
            if (test.Y() <= umin)
            {
                rdDot = rmin*test.X() + dmin*test.Z();
                if (rdDot >= maxRDDot)
                {
                    // LF-edge
                    closest.X() = rmax;
                    closest.Y() = test.Y();
                    closest.Z() = dmax;
                }
                else if (rdDot >= minRDDot)
                {
                    // L-face
                    rDot = dmin*test.X() - rmin*test.Z();
                    t = rDot/minRDDot;
                    closest.X() = test.X() - t*dmin;
                    closest.Y() = test.Y();
                    closest.Z() = test.Z() + t*rmin;
                }
                else
                {
                    // LN-edge
                    closest.X() = rmin;
                    closest.Y() = test.Y();
                    closest.Z() = dmin;
                }
            }
            else
            {
                rudDot = rmin*test.X() + umin*test.Y() + dmin*test.Z();
                rEdgeDot = umin*rudDot - minRUDDot*test.Y();
                if (rEdgeDot >= (Real)0)
                {
                    rdDot = rmin*test.X() + dmin*test.Z();
                    if (rdDot >= maxRDDot)
                    {
                        // LF-edge
                        closest.X() = rmax;
                        closest.Y() = test.Y();
                        closest.Z() = dmax;
                    }
                    else if (rdDot >= minRDDot)
                    {
                        // L-face
                        rDot = dmin*test.X() - rmin*test.Z();
                        t = rDot/minRDDot;
                        closest.X() = test.X() - t*dmin;
                        closest.Y() = test.Y();
                        closest.Z() = test.Z() + t*rmin;
                    }
                    else
                    {
                        // LN-edge
                        closest.X() = rmin;
                        closest.Y() = test.Y();
                        closest.Z() = dmin;
                    }
                }
                else
                {
                    uEdgeDot = rmin*rudDot - minRUDDot*test.X();
                    if (uEdgeDot >= (Real)0)
                    {
                        udDot = umin*test.Y() + dmin*test.Z();
                        if (udDot >= maxUDDot)
                        {
                            // UF-edge
                            closest.X() = test.X();
                            closest.Y() = umax;
                            closest.Z() = dmax;
                        }
                        else if (udDot >= minUDDot)
                        {
                            // U-face
                            uDot = dmin*test.Y() - umin*test.Z();
                            t = uDot/minUDDot;
                            closest.X() = test.X();
                            closest.Y() = test.Y() - t*dmin;
                            closest.Z() = test.Z() + t*umin;
                        }
                        else
                        {
                            // UN-edge
                            closest.X() = test.X();
                            closest.Y() = umin;
                            closest.Z() = dmin;
                        }
                    }
                    else
                    {
                        if (rudDot >= maxRUDDot)
                        {
                            // LUF-vertex
                            closest.X() = rmax;
                            closest.Y() = umax;
                            closest.Z() = dmax;
                        }
                        else if (rudDot >= minRUDDot)
                        {
                            // LU-edge
                            t = rudDot/minRUDDot;
                            closest.X() = t*rmin;
                            closest.Y() = t*umin;
                            closest.Z() = t*dmin;
                        }
                        else
                        {
                            // LUN-vertex
                            closest.X() = rmin;
                            closest.Y() = umin;
                            closest.Z() = dmin;
                        }
                    }
                }
            }
        }
    }
    else
    {
        rDot = dmin*test.X() - rmin*test.Z();
        uDot = dmin*test.Y() - umin*test.Z();
        if (rDot <= (Real)0.0)
        {
            if (uDot <= (Real)0)
            {
                // point inside frustum
                closest = test;
            }
            else
            {
                udDot = umin*test.Y() + dmin*test.Z();
                if (udDot >= maxUDDot)
                {
                    // UF-edge
                    closest.X() = test.X();
                    closest.Y() = umax;
                    closest.Z() = dmax;
                }
                else
                {
                    // U-face
                    t = uDot/minUDDot;
                    closest.X() = test.X();
                    closest.Y() = test.Y() - t*dmin;
                    closest.Z() = test.Z() + t*umin;
                }
            }
        }
        else
        {
            if (uDot <= (Real)0)
            {
                rdDot = rmin*test.X() + dmin*test.Z();
                if (rdDot >= maxRDDot)
                {
                    // LF-edge
                    closest.X() = rmax;
                    closest.Y() = test.Y();
                    closest.Z() = dmax;
                }
                else
                {
                    // L-face
                    t = rDot/minRDDot;
                    closest.X() = test.X() - t*dmin;
                    closest.Y() = test.Y();
                    closest.Z() = test.Z() + t*rmin;
                }
            }
            else
            {
                rudDot = rmin*test.X() + umin*test.Y() + dmin*test.Z();
                rEdgeDot = umin*rudDot - minRUDDot*test.Y();
                if (rEdgeDot >= (Real)0)
                {
                    rdDot = rmin*test.X() + dmin*test.Z();
                    if (rdDot >= maxRDDot)
                    {
                        // LF-edge
                        closest.X() = rmax;
                        closest.Y() = test.Y();
                        closest.Z() = dmax;
                    }
                    else // assert( rdDot >= minRDDot ) from geometry
                    {
                        // L-face
                        t = rDot/minRDDot;
                        closest.X() = test.X() - t*dmin;
                        closest.Y() = test.Y();
                        closest.Z() = test.Z() + t*rmin;
                    }
                }
                else
                {
                    uEdgeDot = rmin*rudDot - minRUDDot*test.X();
                    if (uEdgeDot >= (Real)0)
                    {
                        udDot = umin*test.Y() + dmin*test.Z();
                        if (udDot >= maxUDDot)
                        {
                            // UF-edge
                            closest.X() = test.X();
                            closest.Y() = umax;
                            closest.Z() = dmax;
                        }
                        else // assert( udDot >= minUDDot ) from geometry
                        {
                            // U-face
                            t = uDot/minUDDot;
                            closest.X() = test.X();
                            closest.Y() = test.Y() - t*dmin;
                            closest.Z() = test.Z() + t*umin;
                        }
                    }
                    else
                    {
                        if (rudDot >= maxRUDDot)
                        {
                            // LUF-vertex
                            closest.X() = rmax;
                            closest.Y() = umax;
                            closest.Z() = dmax;
                        }
                        else // assert( rudDot >= minRUDDot ) from geometry
                        {
                            // LU-edge
                            t = rudDot/minRUDDot;
                            closest.X() = t*rmin;
                            closest.Y() = t*umin;
                            closest.Z() = t*dmin;
                        }
                    }
                }
            }
        }
    }

    diff = test - closest;

    // Convert back to original quadrant.
    if (rSignChange)
    {
        closest.X() = -closest.X();
    }

    if (uSignChange)
    {
        closest.Y() = -closest.Y();
    }

    mClosestPoint0 = *mPoint;

    // Convert back to original coordinates.
    mClosestPoint1 = mFrustum->Origin +
        closest.X()*mFrustum->RVector +
        closest.Y()*mFrustum->UVector +
        closest.Z()*mFrustum->DVector;

    return diff.SquaredLength();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Frustum3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedOrigin = mFrustum->Origin + t*velocity1;
    Frustum3<Real> movedFrustum(movedOrigin, mFrustum->DVector,
        mFrustum->UVector, mFrustum->RVector, mFrustum->DMin,
        mFrustum->DMax, mFrustum->UBound, mFrustum->RBound);
    return DistPoint3Frustum3<Real>(movedPoint, movedFrustum).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Frustum3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedOrigin = mFrustum->Origin + t*velocity1;
    Frustum3<Real> movedFrustum(movedOrigin, mFrustum->DVector,
        mFrustum->UVector, mFrustum->RVector, mFrustum->DMin,
        mFrustum->DMax, mFrustum->UBound, mFrustum->RBound);
    return DistPoint3Frustum3<Real>(movedPoint,movedFrustum).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Frustum3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Frustum3<double>;
//----------------------------------------------------------------------------
}
