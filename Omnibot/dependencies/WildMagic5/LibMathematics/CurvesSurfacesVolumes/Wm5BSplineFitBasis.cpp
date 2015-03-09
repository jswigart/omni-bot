// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineFitBasis.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BSplineFitBasis<Real>::BSplineFitBasis (int quantity, int degree)
{
    assertion(1 <= degree && degree < quantity, "Invalid inputs.\n");
    mQuantity = quantity;
    mDegree = degree;

    mValue = new1<Real>(degree + 1);
    mKnot = new1<Real>(2*degree);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineFitBasis<Real>::~BSplineFitBasis ()
{
    delete1(mValue);
    delete1(mKnot);
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineFitBasis<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineFitBasis<Real>::GetDegree () const
{
    return mDegree;
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineFitBasis<Real>::Compute (Real t, int& imin, int& imax) const
{
    assertion((Real)0 <= t && t <= (Real)1, "Invalid input.\n");

    // Use scaled time and scaled knots so that 1/(Q-D) does not need to
    // be explicitly stored by the class object.  Determine the extreme
    // indices affected by local control.
    Real QmD = (Real)(mQuantity - mDegree);
    Real tValue;
    if (t <= (Real)0)
    {
        tValue = (Real)0;
        imin = 0;
        imax = mDegree;
    }
    else if (t >= (Real)1)
    {
        tValue = QmD;
        imax = mQuantity - 1;
        imin = imax - mDegree;
    }
    else
    {
        tValue = QmD*t;
        imin = (int)tValue;
        imax = imin + mDegree;
    }

    // Precompute the knots.
    for (int i0 = 0, i1 = imax+1-mDegree; i0 < 2*mDegree; ++i0, ++i1)
    {
        if (i1 <= mDegree)
        {
            mKnot[i0] = (Real)0;
        }
        else if (i1 >= mQuantity)
        {
            mKnot[i0] = QmD;
        }
        else
        {
            mKnot[i0] = (Real)(i1 - mDegree);
        }
    }

    // Initialize the basis function evaluation table.  The first degree-1
    // entries are zero, but they do not have to be set explicitly.
    mValue[mDegree] = (Real)1;

    // Update the basis function evaluation table, each iteration overwriting
    // the results from the previous iteration.
    for (int row = mDegree-1; row >= 0; --row)
    {
        int k0 = mDegree, k1 = row;
        Real knot0 = mKnot[k0], knot1 = mKnot[k1];
        Real invDenom = ((Real)1)/(knot0 - knot1);
        Real c1 = (knot0 - tValue)*invDenom, c0;
        mValue[row] = c1*mValue[row + 1];

        for (int col = row + 1; col < mDegree; ++col)
        {
            c0 = (tValue - knot1)*invDenom;
            mValue[col] *= c0;

            knot0 = mKnot[++k0];
            knot1 = mKnot[++k1];
            invDenom = ((Real)1)/(knot0 - knot1);
            c1 = (knot0 - tValue)*invDenom;
            mValue[col] += c1*mValue[col + 1];
        }

        c0 = (tValue - knot1)*invDenom;
        mValue[mDegree] *= c0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real BSplineFitBasis<Real>::GetValue (int i) const
{
    assertion(0 <= i && i <= mDegree, "Invalid index\n");
    return mValue[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BSplineFitBasis<float>;

template WM5_MATHEMATICS_ITEM
class BSplineFitBasis<double>;
//----------------------------------------------------------------------------
}
