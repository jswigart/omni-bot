// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5NURBSRectangle.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
NURBSRectangle<Real>::NURBSRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, Real** ctrlWeight,
    int uDegree, int vDegree, bool uLoop, bool vLoop, bool uOpen, bool vOpen)
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
    CreateControl(ctrlPoint, ctrlWeight);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uOpen);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vOpen);
}
//----------------------------------------------------------------------------
template <typename Real>
NURBSRectangle<Real>::NURBSRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, Real** ctrlWeight,
    int uDegree, int vDegree, bool uLoop, bool vLoop, bool uOpen, Real* vKnot)
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
    CreateControl(ctrlPoint, ctrlWeight);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uOpen);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vKnot);
}
//----------------------------------------------------------------------------
template <typename Real>
NURBSRectangle<Real>::NURBSRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, Real** ctrlWeight,
    int uDegree, int vDegree, bool uLoop, bool vLoop, Real* uKnot, bool vOpen)
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
    CreateControl(ctrlPoint, ctrlWeight);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uKnot);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vOpen);
}
//----------------------------------------------------------------------------
template <typename Real>
NURBSRectangle<Real>::NURBSRectangle (int numUCtrlPoints,
    int numVCtrlPoints, Vector3<Real>** ctrlPoint, Real** ctrlWeight,
    int uDegree, int vDegree, bool uLoop, bool vLoop, Real* uKnot,
    Real* vKnot)
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
    CreateControl(ctrlPoint,ctrlWeight);

    mBasis[0].Create(mNumUCtrlPoints + mUReplicate, uDegree, uKnot);
    mBasis[1].Create(mNumVCtrlPoints + mVReplicate, vDegree, vKnot);
}
//----------------------------------------------------------------------------
template <typename Real>
NURBSRectangle<Real>::~NURBSRectangle ()
{
    delete2(mCtrlPoint);
    delete2(mCtrlWeight);
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSRectangle<Real>::CreateControl (Vector3<Real>** ctrlPoint,
    Real** ctrlWeight)
{
    int newNumUCtrlPoints = mNumUCtrlPoints + mUReplicate;
    int newNumVCtrlPoints = mNumVCtrlPoints + mVReplicate;

    mCtrlPoint = new2<Vector3<Real> >(newNumUCtrlPoints,
        newNumVCtrlPoints);

    mCtrlWeight = new2<Real>(newNumUCtrlPoints, newNumVCtrlPoints);

    for (int i = 1; i < newNumUCtrlPoints; ++i)
    {
        mCtrlPoint[i] = &mCtrlPoint[0][i*newNumVCtrlPoints];
        mCtrlWeight[i] = &mCtrlWeight[0][i*newNumVCtrlPoints];
    }

    for (int iu = 0; iu < newNumUCtrlPoints; iu++)
    {
        int uOld = iu % mNumUCtrlPoints;
        for (int iv = 0; iv < newNumVCtrlPoints; iv++)
        {
            int vOld = iv % mNumVCtrlPoints;
            mCtrlPoint[iu][iv] = ctrlPoint[uOld][vOld];
            mCtrlWeight[iu][iv] = ctrlWeight[uOld][vOld];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int NURBSRectangle<Real>::GetNumCtrlPoints (int dim) const
{
    return mBasis[dim].GetNumCtrlPoints();
}
//----------------------------------------------------------------------------
template <typename Real>
int NURBSRectangle<Real>::GetDegree (int dim) const
{
    return mBasis[dim].GetDegree();
}
//----------------------------------------------------------------------------
template <typename Real>
bool NURBSRectangle<Real>::IsOpen (int dim) const
{
    return mBasis[dim].IsOpen();
}
//----------------------------------------------------------------------------
template <typename Real>
bool NURBSRectangle<Real>::IsUniform (int dim) const
{
    return mBasis[dim].IsUniform();
}
//----------------------------------------------------------------------------
template <typename Real>
bool NURBSRectangle<Real>::IsLoop (int dim) const
{
    return mLoop[dim];
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSRectangle<Real>::SetControlPoint (int uIndex, int vIndex,
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
        int uExt = 0, vExt = 0;

        if (doUReplicate)
        {
            uExt = mNumUCtrlPoints + uIndex;
            mCtrlPoint[uExt][vIndex] = ctrl;
        }
        if (doVReplicate)
        {
            vExt = mNumVCtrlPoints + vIndex;
            mCtrlPoint[uIndex][vExt] = ctrl;
        }
        if (doUReplicate && doVReplicate)
        {
            mCtrlPoint[uExt][vExt] = ctrl;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> NURBSRectangle<Real>::GetControlPoint (int uIndex,
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
void NURBSRectangle<Real>::SetControlWeight (int uIndex, int vIndex,
    Real weight)
{
    if (0 <= uIndex && uIndex < mNumUCtrlPoints
    &&  0 <= vIndex && vIndex < mNumVCtrlPoints)
    {
        // Set the control weight.
        mCtrlWeight[uIndex][vIndex] = weight;

        // Set the replicated control weight.
        bool doUReplicate = (uIndex < mUReplicate );
        bool doVReplicate = (vIndex < mVReplicate);
        int uExt = 0, vExt = 0;

        if (doUReplicate)
        {
            uExt = mNumUCtrlPoints + uIndex;
            mCtrlWeight[uExt][vIndex] = weight;
        }
        if (doVReplicate)
        {
            vExt = mNumVCtrlPoints + vIndex;
            mCtrlWeight[uIndex][vExt] = weight;
        }
        if (doUReplicate && doVReplicate)
        {
            mCtrlWeight[uExt][vExt] = weight;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real NURBSRectangle<Real>::GetControlWeight (int uIndex, int vIndex) const
{
    if (0 <= uIndex && uIndex < mNumUCtrlPoints
    &&  0 <= vIndex && vIndex < mNumVCtrlPoints)
    {
        return mCtrlWeight[uIndex][vIndex];
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSRectangle<Real>::SetKnot (int dim, int i, Real knot)
{
    if (0 <= dim && dim <= 1)
    {
        mBasis[dim].SetKnot(i,knot);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real NURBSRectangle<Real>::GetKnot (int dim, int i) const
{
    if (0 <= dim && dim <= 1)
    {
        return mBasis[dim].GetKnot(i);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSRectangle<Real>::Get (Real u, Real v, Vector3<Real>* pos,
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

    Vector3<Real> X = Vector3<Real>::ZERO;
    Real w = (Real)0;
    for (iu = iumin; iu <= iumax; ++iu)
    {
        for (iv = ivmin; iv <= ivmax; ++iv)
        {
            tmp = mBasis[0].GetD0(iu)*mBasis[1].GetD0(iv)*mCtrlWeight[iu][iv];
            X += tmp*mCtrlPoint[iu][iv];
            w += tmp;
        }
    }
    Real invW = ((Real)1)/w;
    Vector3<Real> P = invW*X;
    if (pos)
    {
        *pos = P;
    }

    if (!derU && !derV && !derUU && !derUV && !derVV)
    {
        return;
    }

    Real wDerU = (Real)0;
    Real wDerV = (Real)0;
    Vector3<Real> PDerU = Vector3<Real>::ZERO;
    Vector3<Real> PDerV = Vector3<Real>::ZERO;

    if (derU || derUU || derUV)
    {
        Vector3<Real> XDerU = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD1(iu)*mBasis[1].GetD0(iv)*
                    mCtrlWeight[iu][iv];
                XDerU += tmp*mCtrlPoint[iu][iv];
                wDerU += tmp;
            }
        }
        PDerU = invW*(XDerU - wDerU*P);
        if (derU)
        {
            *derU = PDerU;
        }
    }

    if (derV || derVV || derUV)
    {
        Vector3<Real> XDerV = Vector3<Real>::ZERO;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD0(iu)*mBasis[1].GetD1(iv)*
                    mCtrlWeight[iu][iv];
                XDerV += tmp*mCtrlPoint[iu][iv];
                wDerV += tmp;
            }
        }
        PDerV = invW*(XDerV - wDerV*P);
        if (derV)
        {
            *derV = PDerV;
        }
    }

    if (!derUU && !derUV && !derVV)
    {
        return;
    }

    if (derUU)
    {
        Vector3<Real> XDerUU = Vector3<Real>::ZERO;
        Real wDerUU = (Real)0;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD2(iu)*mBasis[1].GetD0(iv)*
                    mCtrlWeight[iu][iv];
                XDerUU += tmp*mCtrlPoint[iu][iv];
                wDerUU += tmp;
            }
        }
        *derUU = invW*(XDerUU - ((Real)2)*wDerU*PDerU - wDerUU*P);
    }

    if (derUV)
    {
        Vector3<Real> XDerUV = Vector3<Real>::ZERO;
        Real wDerUV = (Real)0;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD1(iu)*mBasis[1].GetD1(iv)*
                    mCtrlWeight[iu][iv];
                XDerUV += tmp*mCtrlPoint[iu][iv];
                wDerUV += tmp;
            }
        }
        *derUV = invW*(XDerUV - wDerU*PDerV - wDerV*PDerU - wDerUV*P);
    }

    if (derVV)
    {
        Vector3<Real> XDerVV = Vector3<Real>::ZERO;
        Real wDerVV = (Real)0;
        for (iu = iumin; iu <= iumax; ++iu)
        {
            for (iv = ivmin; iv <= ivmax; ++iv)
            {
                tmp = mBasis[0].GetD0(iu)*mBasis[1].GetD2(iv)*
                    mCtrlWeight[iu][iv];
                XDerVV += tmp*mCtrlPoint[iu][iv];
                wDerVV += tmp;
            }
        }
        *derVV = invW*(XDerVV - ((Real)2)*wDerV*PDerV - wDerVV*P);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> NURBSRectangle<Real>::P (Real u, Real v) const
{
    Vector3<Real> pos;
    Get(u, v, &pos, 0, 0, 0, 0, 0);
    return pos;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> NURBSRectangle<Real>::PU (Real u, Real v) const
{
    Vector3<Real> derU;
    Get(u, v, 0, &derU, 0, 0, 0, 0);
    return derU;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> NURBSRectangle<Real>::PV (Real u, Real v) const
{
    Vector3<Real> derV;
    Get(u, v, 0, 0, &derV, 0, 0, 0);
    return derV;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> NURBSRectangle<Real>::PUU (Real u, Real v) const
{
    Vector3<Real> derUU;
    Get(u, v, 0, 0, 0, &derUU, 0, 0);
    return derUU;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> NURBSRectangle<Real>::PUV (Real u, Real v) const
{
    Vector3<Real> derUV;
    Get(u, v, 0, 0, 0, 0, &derUV, 0);
    return derUV;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> NURBSRectangle<Real>::PVV (Real u, Real v) const
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
class NURBSRectangle<float>;

template WM5_MATHEMATICS_ITEM
class NURBSRectangle<double>;
//----------------------------------------------------------------------------
}
