// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineReduction.h"
#include "Wm5Integrate1.h"
#include "Wm5Intersector1.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
BSplineReduction<Real,TVector>::BSplineReduction (int numCtrlPoints,
    const TVector* ctrlPoints, int degree, Real fraction, int& outNumCtrlPoints,
    TVector*& outCtrlPoints)
{
    // Check for valid input.  If invalid, return a "null" curve.
    assertion(numCtrlPoints >= 2, "Invalid input\n");
    assertion(ctrlPoints != 0, "Invalid input\n");
    assertion(1 <= degree && degree < numCtrlPoints, "Invalid input\n");
    if (numCtrlPoints < 2 || !ctrlPoints || degree < 1
    ||  degree >= numCtrlPoints)
    {
        outNumCtrlPoints = 0;
        outCtrlPoints = 0;
        return;
    }

    // Clamp the number of control points to [degree+1,quantity-1].
    outNumCtrlPoints = (int)(fraction*numCtrlPoints);
    if (outNumCtrlPoints > numCtrlPoints)
    {
        outNumCtrlPoints = numCtrlPoints;
        outCtrlPoints = new1<TVector>(outNumCtrlPoints);
        memcpy(outCtrlPoints, ctrlPoints, numCtrlPoints*sizeof(TVector));
        return;
    }
    if (outNumCtrlPoints < degree + 1)
    {
        outNumCtrlPoints = degree + 1;
    }

    // Allocate output control points and set all to the zero vector.
    outCtrlPoints = new1<TVector>(outNumCtrlPoints);

    // Set up basis function parameters.  Function 0 corresponds to the
    // output curve.  Function 1 corresponds to the input curve.
    mDegree = degree;
    mQuantity[0] = outNumCtrlPoints;
    mQuantity[1] = numCtrlPoints;

    for (int j = 0; j <= 1; ++j)
    {
        mNumKnots[j] = mQuantity[j] + mDegree + 1;
        mKnot[j] = new1<Real>(mNumKnots[j]);

        int i;
        for (i = 0; i <= mDegree; ++i)
        {
            mKnot[j][i] = (Real)0;
        }

        Real factor = ((Real)1)/(Real)(mQuantity[j] - mDegree);
        for (/**/; i < mQuantity[j]; ++i)
        {
            mKnot[j][i] = (i-mDegree)*factor;
        }

        for (/**/; i < mNumKnots[j]; i++)
        {
            mKnot[j][i] = (Real)1;
        }
    }

    // Construct matrix A (depends only on the output basis function).
    Real value, tmin, tmax;
    int i0, i1;

    mBasis[0] = 0;
    mBasis[1] = 0;

    BandedMatrix<Real> A(mQuantity[0], mDegree, mDegree);
    for (i0 = 0; i0 < mQuantity[0]; ++i0)
    {
        mIndex[0] = i0;
        tmax = MaxSupport(0, i0);

        for (i1 = i0; i1 <= i0 + mDegree && i1 < mQuantity[0]; ++i1)
        {
            mIndex[1] = i1;
            tmin = MinSupport(0, i1);

            value = Integrate1<Real>::RombergIntegral(8, tmin, tmax,
                Integrand, this);
            A(i0, i1) = value;
            A(i1, i0) = value;
        }
    }

    // Construct A^{-1}.
    GMatrix<Real> invA(mQuantity[0], mQuantity[0]);
    bool solved = LinearSystem<Real>().Invert(A, invA);
    assertion(solved, "Failed to solve linear system\n");
    WM5_UNUSED(solved);

    // Construct B (depends on both input and output basis functions).
    mBasis[1] = 1;
    GMatrix<Real> B(mQuantity[0], mQuantity[1]);
    for (i0 = 0; i0 < mQuantity[0]; ++i0)
    {
        mIndex[0] = i0;
        Real tmin0 = MinSupport(0, i0);
        Real tmax0 = MaxSupport(0, i0);

        for (i1 = 0; i1 < mQuantity[1]; ++i1)
        {
            mIndex[1] = i1;
            Real tmin1 = MinSupport(1, i1);
            Real tmax1 = MaxSupport(1, i1);

            Intersector1<Real> intr(tmin0, tmax0, tmin1, tmax1);
            intr.Find();
            int quantity = intr.GetNumIntersections();

            if (quantity == 2)
            {
                value = Integrate1<Real>::RombergIntegral(8,
                    intr.GetIntersection(0), intr.GetIntersection(1),
                    Integrand, this);
                B[i0][i1] = value;
            }
            else
            {
                B[i0][i1] = (Real)0;
            }
        }
    }

    // Construct A^{-1}*B.
    GMatrix<Real> prod = invA*B;

    // Construct the control points for the least-squares curve.
    memset(outCtrlPoints,0,outNumCtrlPoints*sizeof(TVector));
    for (i0 = 0; i0 < mQuantity[0]; ++i0)
    {
        for (i1 = 0; i1 < mQuantity[1]; ++i1)
        {
            outCtrlPoints[i0] += ctrlPoints[i1]*prod[i0][i1];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
BSplineReduction<Real,TVector>::~BSplineReduction ()
{
    delete1(mKnot[0]);
    delete1(mKnot[1]);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real BSplineReduction<Real,TVector>::MinSupport (int basis, int i) const
{
    return mKnot[basis][i];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real BSplineReduction<Real,TVector>::MaxSupport (int basis, int i) const
{
    return mKnot[basis][i+1+mDegree];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real BSplineReduction<Real,TVector>::F (int basis, int i, int j, Real t)
{
    if (j > 0)
    {
        Real result = (Real)0;

        Real denom = mKnot[basis][i + j] - mKnot[basis][i];
        if (denom > (Real)0)
        {
            result += (t - mKnot[basis][i])*F(basis, i, j - 1, t)/denom;
        }

        denom = mKnot[basis][i + j + 1] - mKnot[basis][i + 1];
        if (denom > (Real)0)
        {
            result += (mKnot[basis][i + j + 1] - t) *
                F(basis, i + 1, j - 1, t)/denom;
        }

        return result;
    }

    if (mKnot[basis][i] <= t && t < mKnot[basis][i + 1])
    {
        return (Real)1;
    }
    else
    {
        return (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real BSplineReduction<Real,TVector>::Integrand (Real t, void* data)
{
    BSplineReduction<Real,TVector>& self =
        *(BSplineReduction<Real,TVector>*)data;

    Real value0 = self.F(self.mBasis[0], self.mIndex[0], self.mDegree, t);
    Real value1 = self.F(self.mBasis[1], self.mIndex[1], self.mDegree, t);
    return value0*value1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BSplineReduction<float,Vector2f>;

template WM5_MATHEMATICS_ITEM
class BSplineReduction<float,Vector3f>;

template WM5_MATHEMATICS_ITEM
class BSplineReduction<double,Vector2d>;

template WM5_MATHEMATICS_ITEM
class BSplineReduction<double,Vector3d>;
//----------------------------------------------------------------------------
}
