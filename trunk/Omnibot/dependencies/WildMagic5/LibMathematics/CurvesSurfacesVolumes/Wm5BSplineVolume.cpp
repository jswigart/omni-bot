// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineVolume.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BSplineVolume<Real>::BSplineVolume (int numUCtrlPoints, int numVCtrlPoints,
    int numWCtrlPoints, int uDegree, int vDegree, int wDegree)
{
    assertion(numUCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= uDegree && uDegree <= numUCtrlPoints - 1,
        "Invalid input\n");
    assertion(numVCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= vDegree && vDegree <= numVCtrlPoints - 1,
        "Invalid input\n");
    assertion(numWCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= wDegree && wDegree <= numWCtrlPoints - 1,
        "Invalid input\n");

    mCtrlPoint = new3<Vector3<Real> >(numUCtrlPoints, numVCtrlPoints,
        numWCtrlPoints);
    memset(mCtrlPoint[0][0], 0, numUCtrlPoints*numVCtrlPoints*numWCtrlPoints*
        sizeof(Vector3<Real>));

    mBasis[0].Create(numUCtrlPoints, uDegree, true);
    mBasis[1].Create(numVCtrlPoints, vDegree, true);
    mBasis[2].Create(numWCtrlPoints, wDegree, true);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineVolume<Real>::~BSplineVolume ()
{
    delete3(mCtrlPoint);
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineVolume<Real>::GetNumCtrlPoints (int dim) const
{
    return mBasis[dim].GetNumCtrlPoints();
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineVolume<Real>::GetDegree (int dim) const
{
    return mBasis[dim].GetDegree();
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineVolume<Real>::SetControlPoint (int uIndex, int vIndex,
    int wIndex, const Vector3<Real>& ctrlPoint)
{
    if (0 <= uIndex && uIndex < mBasis[0].GetNumCtrlPoints()
    &&  0 <= vIndex && vIndex < mBasis[1].GetNumCtrlPoints()
    &&  0 <= wIndex && wIndex < mBasis[2].GetNumCtrlPoints())
    {
        mCtrlPoint[uIndex][vIndex][wIndex] = ctrlPoint;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineVolume<Real>::GetControlPoint (int uIndex, int vIndex,
    int wIndex) const
{
    if (0 <= uIndex && uIndex < mBasis[0].GetNumCtrlPoints()
    &&  0 <= vIndex && vIndex < mBasis[1].GetNumCtrlPoints()
    &&  0 <= wIndex && wIndex < mBasis[2].GetNumCtrlPoints())
    {
        return mCtrlPoint[uIndex][vIndex][wIndex];
    }

    return Vector3<Real>(Math<Real>::MAX_REAL, Math<Real>::MAX_REAL,
        Math<Real>::MAX_REAL);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineVolume<Real>::GetPosition (Real u, Real v, Real w) const
{
    int iumin, iumax, ivmin, ivmax, iwmin, iwmax;
    mBasis[0].Compute(u, 0, iumin, iumax);
    mBasis[1].Compute(v, 0, ivmin, ivmax);
    mBasis[2].Compute(w, 0, iwmin, iwmax);

    Vector3<Real> pos = Vector3<Real>::ZERO;
    for (int iu = iumin; iu <= iumax; ++iu)
    {
        Real tmp0 = mBasis[0].GetD0(iu);
        for (int iv = ivmin; iv <= ivmax; ++iv)
        {
            Real tmp1 = mBasis[1].GetD0(iv);
            for (int iw = iwmin; iw <= iwmax; ++iw)
            {
                Real tmp2 = mBasis[2].GetD0(iw);
                Real prod = tmp0*tmp1*tmp2;
                pos += prod*mCtrlPoint[iu][iv][iw];
            }
        }
    }

    return pos;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineVolume<Real>::GetDerivativeU (Real u, Real v, Real w)
    const
{
    int iumin, iumax, ivmin, ivmax, iwmin, iwmax;
    mBasis[0].Compute(u, 1, iumin, iumax);
    mBasis[1].Compute(v, 0, ivmin, ivmax);
    mBasis[2].Compute(w, 0, iwmin, iwmax);

    Vector3<Real> derU = Vector3<Real>::ZERO;
    for (int iu = iumin; iu <= iumax; ++iu)
    {
        Real tmp0 = mBasis[0].GetD1(iu);
        for (int iv = ivmin; iv <= ivmax; ++iv)
        {
            Real tmp1 = mBasis[1].GetD0(iv);
            for (int iw = iwmin; iw <= iwmax; ++iw)
            {
                Real tmp2 = mBasis[2].GetD0(iw);
                Real prod = tmp0*tmp1*tmp2;
                derU += prod*mCtrlPoint[iu][iv][iw];
            }
        }
    }

    return derU;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineVolume<Real>::GetDerivativeV (Real u, Real v, Real w)
    const
{
    int iumin, iumax, ivmin, ivmax, iwmin, iwmax;
    mBasis[0].Compute(u, 0, iumin, iumax);
    mBasis[1].Compute(v, 1, ivmin, ivmax);
    mBasis[2].Compute(w, 0, iwmin, iwmax);

    Vector3<Real> derV = Vector3<Real>::ZERO;
    for (int iu = iumin; iu <= iumax; ++iu)
    {
        Real tmp0 = mBasis[0].GetD0(iu);
        for (int iv = ivmin; iv <= ivmax; ++iv)
        {
            Real tmp1 = mBasis[1].GetD1(iv);
            for (int iw = iwmin; iw <= iwmax; ++iw)
            {
                Real tmp2 = mBasis[2].GetD0(iw);
                Real prod = tmp0*tmp1*tmp2;
                derV += prod*mCtrlPoint[iu][iv][iw];
            }
        }
    }

    return derV;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineVolume<Real>::GetDerivativeW (Real u, Real v, Real w)
    const
{
    int iumin, iumax, ivmin, ivmax, iwmin, iwmax;
    mBasis[0].Compute(u, 0, iumin, iumax);
    mBasis[1].Compute(v, 0, ivmin, ivmax);
    mBasis[2].Compute(w, 1, iwmin, iwmax);

    Vector3<Real> derW = Vector3<Real>::ZERO;
    for (int iu = iumin; iu <= iumax; ++iu)
    {
        Real tmp0 = mBasis[0].GetD0(iu);
        for (int iv = ivmin; iv <= ivmax; ++iv)
        {
            Real tmp1 = mBasis[1].GetD0(iv);
            for (int iw = iwmin; iw <= iwmax; ++iw)
            {
                Real tmp2 = mBasis[2].GetD1(iw);
                Real prod = tmp0*tmp1*tmp2;
                derW += prod*mCtrlPoint[iu][iv][iw];
            }
        }
    }

    return derW;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineVolume<Real>::GetPosition (Real pos[3]) const
{
    return GetPosition(pos[0], pos[1], pos[2]);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineVolume<Real>::GetDerivative (int i, Real pos[3]) const
{
    switch (i)
    {
    case 0:  return GetDerivativeU(pos[0], pos[1], pos[2]);
    case 1:  return GetDerivativeV(pos[0], pos[1], pos[2]);
    case 2:  return GetDerivativeW(pos[0], pos[1], pos[2]);
    }

    assertion(false, "Derivatives larger than order 3 not supported\n");
    return Vector3<Real>::ZERO;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BSplineVolume<float>;

template WM5_MATHEMATICS_ITEM
class BSplineVolume<double>;
//----------------------------------------------------------------------------
}
