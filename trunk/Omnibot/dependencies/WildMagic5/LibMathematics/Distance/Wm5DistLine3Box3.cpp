// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine3Box3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine3Box3<Real>::DistLine3Box3 (const Line3<Real>& line,
    const Box3<Real>& box)
    :
    mLine(&line),
    mBox(&box),
    mLineParameter((Real)0)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Box3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& DistLine3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Box3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Box3<Real>::GetSquared ()
{
    // Compute coordinates of line in box coordinate system.
    Vector3<Real> diff = mLine->Origin - mBox->Center;
    Vector3<Real> point(
        diff.Dot(mBox->Axis[0]),
        diff.Dot(mBox->Axis[1]),
        diff.Dot(mBox->Axis[2]));
    Vector3<Real> direction(
        mLine->Direction.Dot(mBox->Axis[0]),
        mLine->Direction.Dot(mBox->Axis[1]),
        mLine->Direction.Dot(mBox->Axis[2]));

    // Apply reflections so that direction vector has nonnegative components.
    bool reflect[3];
    int i;
    for (i = 0; i < 3; ++i)
    {
        if (direction[i] < (Real)0)
        {
            point[i] = -point[i];
            direction[i] = -direction[i];
            reflect[i] = true;
        }
        else
        {
            reflect[i] = false;
        }
    }

    Real sqrDistance = (Real)0;
    mLineParameter = (Real)0;

    if (direction.X() > (Real)0)
    {
        if (direction.Y() > (Real)0)
        {
            if (direction.Z() > (Real)0)  // (+,+,+)
            {
                CaseNoZeros(point, direction, sqrDistance);
            }
            else  // (+,+,0)
            {
                Case0(0, 1, 2, point, direction, sqrDistance);
            }
        }
        else
        {
            if (direction.Z() > (Real)0)  // (+,0,+)
            {
                Case0(0, 2, 1, point, direction, sqrDistance);
            }
            else  // (+,0,0)
            {
                Case00(0, 1, 2, point, direction, sqrDistance);
            }
        }
    }
    else
    {
        if (direction.Y() > (Real)0)
        {
            if (direction.Z() > (Real)0)  // (0,+,+)
            {
                Case0(1, 2, 0, point, direction, sqrDistance);
            }
            else  // (0,+,0)
            {
                Case00(1, 0, 2, point, direction, sqrDistance);
            }
        }
        else
        {
            if (direction.Z() > (Real)0)  // (0,0,+)
            {
                Case00(2, 0, 1, point, direction, sqrDistance);
            }
            else  // (0,0,0)
            {
                Case000(point, sqrDistance);
            }
        }
    }

    // Compute closest point on line.
    mClosestPoint0 = mLine->Origin + mLineParameter*mLine->Direction;

    // Compute closest point on box.
    mClosestPoint1 = mBox->Center;
    for (i = 0; i < 3; ++i)
    {
        // Undo the reflections applied previously.
        if (reflect[i])
        {
            point[i] = -point[i];
        }

        mClosestPoint1 += point[i]*mBox->Axis[i];
    }

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Box3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedCenter = mBox->Center + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Box3<Real> movedBox(movedCenter, mBox->Axis, mBox->Extent);
    return DistLine3Box3<Real>(movedLine, movedBox).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Box3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> kMOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> kMCenter = mBox->Center + t*velocity1;
    Line3<Real> kMLine(kMOrigin,mLine->Direction);
    Box3<Real> kMBox(kMCenter,mBox->Axis,mBox->Extent);
    return DistLine3Box3<Real>(kMLine,kMBox).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Box3<Real>::GetLineParameter () const
{
    return mLineParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
void DistLine3Box3<Real>::Face (int i0, int i1, int i2, Vector3<Real>& pnt,
    const Vector3<Real>& dir, const Vector3<Real>& PmE, Real& sqrDistance)
{
    Vector3<Real> PpE;
    Real lenSqr, inv, tmp, param, t, delta;

    PpE[i1] = pnt[i1] + mBox->Extent[i1];
    PpE[i2] = pnt[i2] + mBox->Extent[i2];
    if (dir[i0]*PpE[i1] >= dir[i1]*PmE[i0])
    {
        if (dir[i0]*PpE[i2] >= dir[i2]*PmE[i0])
        {
            // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
            pnt[i0] = mBox->Extent[i0];
            inv = ((Real)1)/dir[i0];
            pnt[i1] -= dir[i1]*PmE[i0]*inv;
            pnt[i2] -= dir[i2]*PmE[i0]*inv;
            mLineParameter = -PmE[i0]*inv;
        }
        else
        {
            // v[i1] >= -e[i1], v[i2] < -e[i2]
            lenSqr = dir[i0]*dir[i0] + dir[i2]*dir[i2];
            tmp = lenSqr*PpE[i1] - dir[i1]*(dir[i0]*PmE[i0] +
                dir[i2]*PpE[i2]);
            if (tmp <= ((Real)2)*lenSqr*mBox->Extent[i1])
            {
                t = tmp/lenSqr;
                lenSqr += dir[i1]*dir[i1];
                tmp = PpE[i1] - t;
                delta = dir[i0]*PmE[i0] + dir[i1]*tmp + dir[i2]*PpE[i2];
                param = -delta/lenSqr;
                sqrDistance += PmE[i0]*PmE[i0] + tmp*tmp + PpE[i2]*PpE[i2]
                    + delta*param;

                mLineParameter = param;
                pnt[i0] = mBox->Extent[i0];
                pnt[i1] = t - mBox->Extent[i1];
                pnt[i2] = -mBox->Extent[i2];
            }
            else
            {
                lenSqr += dir[i1]*dir[i1];
                delta = dir[i0]*PmE[i0] + dir[i1]*PmE[i1] + dir[i2]*PpE[i2];
                param = -delta/lenSqr;
                sqrDistance += PmE[i0]*PmE[i0] + PmE[i1]*PmE[i1]
                    + PpE[i2]*PpE[i2] + delta*param;

                mLineParameter = param;
                pnt[i0] = mBox->Extent[i0];
                pnt[i1] = mBox->Extent[i1];
                pnt[i2] = -mBox->Extent[i2];
            }
        }
    }
    else
    {
        if (dir[i0]*PpE[i2] >= dir[i2]*PmE[i0])
        {
            // v[i1] < -e[i1], v[i2] >= -e[i2]
            lenSqr = dir[i0]*dir[i0] + dir[i1]*dir[i1];
            tmp = lenSqr*PpE[i2] - dir[i2]*(dir[i0]*PmE[i0] +
                dir[i1]*PpE[i1]);
            if (tmp <= ((Real)2)*lenSqr*mBox->Extent[i2])
            {
                t = tmp/lenSqr;
                lenSqr += dir[i2]*dir[i2];
                tmp = PpE[i2] - t;
                delta = dir[i0]*PmE[i0] + dir[i1]*PpE[i1] + dir[i2]*tmp;
                param = -delta/lenSqr;
                sqrDistance += PmE[i0]*PmE[i0] + PpE[i1]*PpE[i1] + tmp*tmp
                    + delta*param;

                mLineParameter = param;
                pnt[i0] = mBox->Extent[i0];
                pnt[i1] = -mBox->Extent[i1];
                pnt[i2] = t - mBox->Extent[i2];
            }
            else
            {
                lenSqr += dir[i2]*dir[i2];
                delta = dir[i0]*PmE[i0] + dir[i1]*PpE[i1] + dir[i2]*PmE[i2];
                param = -delta/lenSqr;
                sqrDistance += PmE[i0]*PmE[i0] + PpE[i1]*PpE[i1] +
                    PmE[i2]*PmE[i2] + delta*param;

                mLineParameter = param;
                pnt[i0] = mBox->Extent[i0];
                pnt[i1] = -mBox->Extent[i1];
                pnt[i2] = mBox->Extent[i2];
            }
        }
        else
        {
            // v[i1] < -e[i1], v[i2] < -e[i2]
            lenSqr = dir[i0]*dir[i0] + dir[i2]*dir[i2];
            tmp = lenSqr*PpE[i1] - dir[i1]*(dir[i0]*PmE[i0] +
                dir[i2]*PpE[i2]);
            if (tmp >= (Real)0)
            {
                // v[i1]-edge is closest
                if (tmp <= ((Real)2)*lenSqr*mBox->Extent[i1])
                {
                    t = tmp/lenSqr;
                    lenSqr += dir[i1]*dir[i1];
                    tmp = PpE[i1] - t;
                    delta = dir[i0]*PmE[i0] + dir[i1]*tmp + dir[i2]*PpE[i2];
                    param = -delta/lenSqr;
                    sqrDistance += PmE[i0]*PmE[i0] + tmp*tmp + PpE[i2]*PpE[i2]
                        + delta*param;

                    mLineParameter = param;
                    pnt[i0] = mBox->Extent[i0];
                    pnt[i1] = t - mBox->Extent[i1];
                    pnt[i2] = -mBox->Extent[i2];
                }
                else
                {
                    lenSqr += dir[i1]*dir[i1];
                    delta = dir[i0]*PmE[i0] + dir[i1]*PmE[i1]
                        + dir[i2]*PpE[i2];
                    param = -delta/lenSqr;
                    sqrDistance += PmE[i0]*PmE[i0] + PmE[i1]*PmE[i1]
                        + PpE[i2]*PpE[i2] + delta*param;

                    mLineParameter = param;
                    pnt[i0] = mBox->Extent[i0];
                    pnt[i1] = mBox->Extent[i1];
                    pnt[i2] = -mBox->Extent[i2];
                }
                return;
            }

            lenSqr = dir[i0]*dir[i0] + dir[i1]*dir[i1];
            tmp = lenSqr*PpE[i2] - dir[i2]*(dir[i0]*PmE[i0] +
                dir[i1]*PpE[i1]);
            if (tmp >= (Real)0)
            {
                // v[i2]-edge is closest
                if (tmp <= ((Real)2)*lenSqr*mBox->Extent[i2])
                {
                    t = tmp/lenSqr;
                    lenSqr += dir[i2]*dir[i2];
                    tmp = PpE[i2] - t;
                    delta = dir[i0]*PmE[i0] + dir[i1]*PpE[i1] + dir[i2]*tmp;
                    param = -delta/lenSqr;
                    sqrDistance += PmE[i0]*PmE[i0] + PpE[i1]*PpE[i1] +
                        tmp*tmp + delta*param;

                    mLineParameter = param;
                    pnt[i0] = mBox->Extent[i0];
                    pnt[i1] = -mBox->Extent[i1];
                    pnt[i2] = t - mBox->Extent[i2];
                }
                else
                {
                    lenSqr += dir[i2]*dir[i2];
                    delta = dir[i0]*PmE[i0] + dir[i1]*PpE[i1]
                        + dir[i2]*PmE[i2];
                    param = -delta/lenSqr;
                    sqrDistance += PmE[i0]*PmE[i0] + PpE[i1]*PpE[i1]
                        + PmE[i2]*PmE[i2] + delta*param;

                    mLineParameter = param;
                    pnt[i0] = mBox->Extent[i0];
                    pnt[i1] = -mBox->Extent[i1];
                    pnt[i2] = mBox->Extent[i2];
                }
                return;
            }

            // (v[i1],v[i2])-corner is closest
            lenSqr += dir[i2]*dir[i2];
            delta = dir[i0]*PmE[i0] + dir[i1]*PpE[i1] + dir[i2]*PpE[i2];
            param = -delta/lenSqr;
            sqrDistance += PmE[i0]*PmE[i0] + PpE[i1]*PpE[i1]
                + PpE[i2]*PpE[i2] + delta*param;

            mLineParameter = param;
            pnt[i0] = mBox->Extent[i0];
            pnt[i1] = -mBox->Extent[i1];
            pnt[i2] = -mBox->Extent[i2];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void DistLine3Box3<Real>::CaseNoZeros (Vector3<Real>& pnt,
    const Vector3<Real>& dir, Real& sqrDistance)
{
    Vector3<Real> PmE(
        pnt.X() - mBox->Extent[0],
        pnt.Y() - mBox->Extent[1],
        pnt.Z() - mBox->Extent[2]);

    Real prodDxPy = dir.X()*PmE.Y();
    Real prodDyPx = dir.Y()*PmE.X();
    Real prodDzPx, prodDxPz, prodDzPy, prodDyPz;

    if (prodDyPx >= prodDxPy)
    {
        prodDzPx = dir.Z()*PmE.X();
        prodDxPz = dir.X()*PmE.Z();
        if (prodDzPx >= prodDxPz)
        {
            // line intersects x = e0
            Face(0, 1, 2, pnt, dir, PmE, sqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2, 0, 1, pnt, dir, PmE, sqrDistance);
        }
    }
    else
    {
        prodDzPy = dir.Z()*PmE.Y();
        prodDyPz = dir.Y()*PmE.Z();
        if (prodDzPy >= prodDyPz)
        {
            // line intersects y = e1
            Face(1, 2, 0, pnt, dir, PmE, sqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2, 0, 1, pnt, dir, PmE, sqrDistance);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void DistLine3Box3<Real>::Case0 (int i0, int i1, int i2, Vector3<Real>& pnt,
    const Vector3<Real>& dir, Real& sqrDistance)
{
    Real PmE0 = pnt[i0] - mBox->Extent[i0];
    Real PmE1 = pnt[i1] - mBox->Extent[i1];
    Real prod0 = dir[i1]*PmE0;
    Real prod1 = dir[i0]*PmE1;
    Real delta, invLSqr, inv;

    if (prod0 >= prod1)
    {
        // line intersects P[i0] = e[i0]
        pnt[i0] = mBox->Extent[i0];

        Real PpE1 = pnt[i1] + mBox->Extent[i1];
        delta = prod0 - dir[i0]*PpE1;
        if (delta >= (Real)0)
        {
            invLSqr = ((Real)1)/(dir[i0]*dir[i0] + dir[i1]*dir[i1]);
            sqrDistance += delta*delta*invLSqr;
            pnt[i1] = -mBox->Extent[i1];
            mLineParameter = -(dir[i0]*PmE0 + dir[i1]*PpE1)*invLSqr;
        }
        else
        {
            inv = ((Real)1)/dir[i0];
            pnt[i1] -= prod0*inv;
            mLineParameter = -PmE0*inv;
        }
    }
    else
    {
        // line intersects P[i1] = e[i1]
        pnt[i1] = mBox->Extent[i1];

        Real PpE0 = pnt[i0] + mBox->Extent[i0];
        delta = prod1 - dir[i1]*PpE0;
        if (delta >= (Real)0)
        {
            invLSqr = ((Real)1)/(dir[i0]*dir[i0] + dir[i1]*dir[i1]);
            sqrDistance += delta*delta*invLSqr;
            pnt[i0] = -mBox->Extent[i0];
            mLineParameter = -(dir[i0]*PpE0 + dir[i1]*PmE1)*invLSqr;
        }
        else
        {
            inv = ((Real)1)/dir[i1];
            pnt[i0] -= prod1*inv;
            mLineParameter = -PmE1*inv;
        }
    }

    if (pnt[i2] < -mBox->Extent[i2])
    {
        delta = pnt[i2] + mBox->Extent[i2];
        sqrDistance += delta*delta;
        pnt[i2] = -mBox->Extent[i2];
    }
    else if (pnt[i2] > mBox->Extent[i2])
    {
        delta = pnt[i2] - mBox->Extent[i2];
        sqrDistance += delta*delta;
        pnt[i2] = mBox->Extent[i2];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void DistLine3Box3<Real>::Case00 (int i0, int i1, int i2,
    Vector3<Real>& pnt, const Vector3<Real>& dir,  Real& sqrDistance)
{
    Real delta;

    mLineParameter = (mBox->Extent[i0] - pnt[i0])/dir[i0];

    pnt[i0] = mBox->Extent[i0];

    if (pnt[i1] < -mBox->Extent[i1])
    {
        delta = pnt[i1] + mBox->Extent[i1];
        sqrDistance += delta*delta;
        pnt[i1] = -mBox->Extent[i1];
    }
    else if (pnt[i1] > mBox->Extent[i1])
    {
        delta = pnt[i1] - mBox->Extent[i1];
        sqrDistance += delta*delta;
        pnt[i1] = mBox->Extent[i1];
    }

    if (pnt[i2] < -mBox->Extent[i2])
    {
        delta = pnt[i2] + mBox->Extent[i2];
        sqrDistance += delta*delta;
        pnt[i2] = -mBox->Extent[i2];
    }
    else if (pnt[i2] > mBox->Extent[i2])
    {
        delta = pnt[i2] - mBox->Extent[i2];
        sqrDistance += delta*delta;
        pnt[i2] = mBox->Extent[i2];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void DistLine3Box3<Real>::Case000 (Vector3<Real>& pnt, Real& sqrDistance)
{
    Real delta;

    if (pnt.X() < -mBox->Extent[0])
    {
        delta = pnt.X() + mBox->Extent[0];
        sqrDistance += delta*delta;
        pnt.X() = -mBox->Extent[0];
    }
    else if (pnt.X() > mBox->Extent[0])
    {
        delta = pnt.X() - mBox->Extent[0];
        sqrDistance += delta*delta;
        pnt.X() = mBox->Extent[0];
    }

    if (pnt.Y() < -mBox->Extent[1])
    {
        delta = pnt.Y() + mBox->Extent[1];
        sqrDistance += delta*delta;
        pnt.Y() = -mBox->Extent[1];
    }
    else if (pnt.Y() > mBox->Extent[1])
    {
        delta = pnt.Y() - mBox->Extent[1];
        sqrDistance += delta*delta;
        pnt.Y() = mBox->Extent[1];
    }

    if (pnt.Z() < -mBox->Extent[2])
    {
        delta = pnt.Z() + mBox->Extent[2];
        sqrDistance += delta*delta;
        pnt.Z() = -mBox->Extent[2];
    }
    else if (pnt.Z() > mBox->Extent[2])
    {
        delta = pnt.Z() - mBox->Extent[2];
        sqrDistance += delta*delta;
        pnt.Z() = mBox->Extent[2];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine3Box3<float>;

template WM5_MATHEMATICS_ITEM
class DistLine3Box3<double>;
//----------------------------------------------------------------------------
}
