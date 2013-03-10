// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineRectangle.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BSplineRectangle<Real>::BSplineRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, int uDegree,
    int vDegree, bool uLoop, bool vLoop, bool uOpen, bool vOpen)
    :
    ParametricSurface<Real>((Real)0, (Real)1, (Real)0, (Real)1, true)
{
    assertion(numUCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= uDegree && uDegree <= numUCtrlPoints - 1,
        "Invalid input\n");
    assertion(numVCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= vDegree && vDegree <= numVCtrlPoints - 1,
        "Invalid input\n");

    mLoop[0] = uLoop;
    mLoop[1] = vLoop;

    mNumUCtrlPoints = numUCtrlPoints;
    mNumVCtrlPoints = numVCtrlPoints;
    mUReplicate = (uLoop ? (uOpen ? 1 : uDegree) : 0);
    mVReplicate = (vLoop ? (vOpen ? 1 : vDegree) : 0);
    CreateControl(ctrlPoint);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uOpen);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vOpen);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineRectangle<Real>::BSplineRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, int uDegree,
    int vDegree, bool uLoop, bool vLoop, bool uOpen, Real* vKnot)
    :
    ParametricSurface<Real>((Real)0, (Real)1, (Real)0, (Real)1, true)
{
    assertion(numUCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= uDegree && uDegree <= numUCtrlPoints - 1,
        "Invalid input\n");
    assertion(numVCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= vDegree && vDegree <= numVCtrlPoints - 1,
        "Invalid input\n");

    mLoop[0] = uLoop;
    mLoop[1] = vLoop;

    mNumUCtrlPoints = numUCtrlPoints;
    mNumVCtrlPoints = numVCtrlPoints;
    mUReplicate = (uLoop ? (uOpen ? 1 : uDegree) : 0);
    mVReplicate = (vLoop ? 1 : 0);
    CreateControl(ctrlPoint);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uOpen);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vKnot);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineRectangle<Real>::BSplineRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, int uDegree,
    int vDegree, bool uLoop, bool vLoop, Real* uKnot, bool vOpen)
    :
    ParametricSurface<Real>((Real)0, (Real)1, (Real)0, (Real)1, true)
{
    assertion(numUCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= uDegree && uDegree <= numUCtrlPoints - 1,
        "Invalid input\n");
    assertion(numVCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= vDegree && vDegree <= numVCtrlPoints - 1,
        "Invalid input\n");

    mLoop[0] = uLoop;
    mLoop[1] = vLoop;

    mNumUCtrlPoints = numUCtrlPoints;
    mNumVCtrlPoints = numVCtrlPoints;
    mUReplicate = (uLoop ? 1 : 0);
    mVReplicate = (vLoop ? (vOpen ? 1 : vDegree) : 0);
    CreateControl(ctrlPoint);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uKnot);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vOpen);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineRectangle<Real>::BSplineRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, int uDegree,
    int vDegree, bool uLoop, bool vLoop, Real* uKnot, Real* vKnot)
    :
    ParametricSurface<Real>((Real)0, (Real)1, (Real)0, (Real)1, true)
{
    assertion(numUCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= uDegree && uDegree <= numUCtrlPoints - 1,
        "Invalid input\n");
    assertion(numVCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= vDegree && vDegree <= numVCtrlPoints - 1,
        "Invalid input\n");

    mLoop[0] = uLoop;
    mLoop[1] = vLoop;

    mNumUCtrlPoints = numUCtrlPoints;
    mNumVCtrlPoints = numVCtrlPoints;
    mUReplicate = (uLoop ? 1 : 0);
    mVReplicate = (vLoop ? 1 : 0);
    CreateControl(ctrlPoint);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uKnot);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vKnot);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineRectangle<Real>::~BSplineRectangle ()
{
    delete2(mCtrlPoint);
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineRectangle<Real>::CreateControl (Vector3<Real>** ctrlPoint)
{
    int newNumUCtrlPoints = mNumUCtrlPoints + mUReplicate;
    int newNumVCtrlPoints = mNumVCtrlPoints + mVReplicate;
    mCtrlPoint = new2<Vector3<Real> >(newNumVCtrlPoints,
        newNumUCtrlPoints);

    for (int u = 0; u < newNumUCtrlPoints; ++u)
    {
        int uOld = u % mNumUCtrlPoints;
        for (int v = 0; v < newNumVCtrlPoints; ++v)
        {
            int vOld = v % mNumVCtrlPoints;
            mCtrlPoint[u][v] = ctrlPoint[uOld][vOld];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineRectangle<Real>::GetNumCtrlPoints (int dim) const
{
    return mBasis[dim].GetNumCtrlPoints();
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineRectangle<Real>::GetDegree (int dim) const
{
    return mBasis[dim].GetDegree();
}
//----------------------------------------------------------------------------
template <typename Real>
bool BSplineRectangle<Real>::IsOpen (int dim) const
{
    return mBasis[dim].IsOpen();
}
//----------------------------------------------------------------------------
template <typename Real>
bool BSplineRectangle<Real>::IsUniform (int dim) const
{
    return mBasis[dim].IsUniform();
}
//----------------------------------------------------------------------------
template <typename Real>
bool BSplineRectangle<Real>::IsLoop (int dim) const
{
    return mLoop[dim];
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineRectangle<Real>::SetControlPoint (int uIndex, int vIndex,
    const Vector3<Real>& ctrl)
{
    if (0 <= uIndex && uIndex < mNumUCtrlPoints
    &&  0 <= vIndex && vIndex < mNumVCtrlPoints)
    {
        // Set the control point.
        mCtrlPoint[uIndex][vIndex] = ctrl;

        // Set the replicated control point.
        bool doUReplicate = (uIndex < mUReplicate);
        bool doVReplicate = (vIndex < mVReplicate);
        int iUExt = 0, iVExt = 0;

        if (doUReplicate)
        {
            iUExt = mNumUCtrlPoints + uIndex;
            mCtrlPoint[iUExt][vIndex] = ctrl;
        }
        if (doVReplicate)
        {
            iVExt = mNumVCtrlPoints + vIndex;
            mCtrlPoint[uIndex][iVExt] = ctrl;
        }
        if (doUReplicate && doVReplicate)
        {
            mCtrlPoint[iUExt][iVExt] = ctrl;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineRectangle<Real>::GetControlPoint (int uIndex,
    int vIndex) const
{
    if (0 <= uIndex && uIndex < mNumUCtrlPoints
    &&  0 <= vIndex && vIndex < mNumVCtrlPoints)
    {
        return mCtrlPoint[uIndex][vIndex];
    }

    return Vector3<Real>(Math<Real>::MAX_REAL, Math<Real>::MAX_REAL,
        Math<Real>::MAX_REAL);
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineRectangle<Real>::SetKnot (int dim, int i, Real knot)
{
    if (0 <= dim && dim <= 1)
    {
        mBasis[dim].SetKnot(i,knot);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real BSplineRectangle<Real>::GetKnot (int dim, int i) const
{
    if (0 <= dim && dim <= 1)
    {
        return mBasis[dim].GetKnot(i);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineRectangle<Real>::Get (Real u, Real v, Vector3<Real>* pos,
    Vector3<Real>* derU, Vector3<Real>* derV, Vector3<Real>* derUU,
    Vector3<Real>* derUV, Vector3<Real>* derVV) const
{
    int iu, iumin, iumax;
    if (derUU)
    {
        mBasis[0].Compute(u, 0, iumin, iumax);
        mBasis[0].Compute(u, 1, iumin, iumax);
        mBasis[0].Compute(u, 2, iumin, iumax);
    }
    else if (derUV || derU)
    {
        mBasis[0].Compute(u, 0, iumin, iumax);
        mBasis[0].Compute(u, 1, iumin, iumax);
    }
    else
    {
        mBasis[0].Compute(u, 0, iumin, iumax);
    }

    int iv, ivmin, ivmax;
    if (derVV)
    {
        mBasis[1].Compute(v, 0, ivmin, ivmax);
        mBasis[1].Compute(v, 1, ivmin, ivmax);
        mBasis[1].Compute(v, 2, ivmin, ivmax);
    }
    else if (derUV || derV)
    {
        mBasis[1].Compute(v, 0, ivmin, ivmax);
        mBasis[1].Compute(v, 1, ivmin, ivmax);
    }
    else
    {
        mBasis[1].Compute(v, 0, ivmin, ivmax);
    }

    Real tmp;

    if (pos)
    {
        *pos = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD0(iu)*mBasis[1].GetD0(iv);
                *pos += tmp*mCtrlPoint[iu][iv];
            }
        }
    }

    if (derU)
    {
        *derU = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD1(iu)*mBasis[1].GetD0(iv);
                *derU += tmp*mCtrlPoint[iu][iv];
            }
        }
    }

    if (derV)
    {
        *derV = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD0(iu)*mBasis[1].GetD1(iv);
                *derV += tmp*mCtrlPoint[iu][iv];
            }
        }
    }

    if (derUU)
    {
        *derUU = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD2(iu)*mBasis[1].GetD0(iv);
                *derUU += tmp*mCtrlPoint[iu][iv];
            }
        }
    }

    if (derUV)
    {
        *derUV = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD1(iu)*mBasis[1].GetD1(iv);
                *derUV += tmp*mCtrlPoint[iu][iv];
            }
        }
    }

    if (derVV)
    {
        *derVV = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD0(iu)*mBasis[1].GetD2(iv);
                *derVV += tmp*mCtrlPoint[iu][iv];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineRectangle<Real>::P (Real u, Real v) const
{
    Vector3<Real> pos;
    Get(u, v, &pos, 0, 0, 0, 0, 0);
    return pos;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineRectangle<Real>::PU (Real u, Real v) const
{
    Vector3<Real> derU;
    Get(u, v, 0, &derU, 0, 0, 0, 0);
    return derU;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineRectangle<Real>::PV (Real u, Real v) const
{
    Vector3<Real> derV;
    Get(u, v, 0, 0, &derV, 0, 0, 0);
    return derV;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineRectangle<Real>::PUU (Real u, Real v) const
{
    Vector3<Real> derUU;
    Get(u, v, 0, 0, 0, &derUU, 0, 0);
    return derUU;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineRectangle<Real>::PUV (Real u, Real v) const
{
    Vector3<Real> derUV;
    Get(u, v, 0, 0, 0, 0, &derUV, 0);
    return derUV;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineRectangle<Real>::PVV (Real u, Real v) const
{
    Vector3<Real> derVV;
    Get(u, v, 0, 0, 0, 0, 0, &derVV);
    return derVV;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BSplineRectangle<float>;

template WM5_MATHEMATICS_ITEM
class BSplineRectangle<double>;
//----------------------------------------------------------------------------
}
