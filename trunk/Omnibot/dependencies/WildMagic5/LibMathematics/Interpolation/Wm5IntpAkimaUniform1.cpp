// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpAkimaUniform1.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpAkimaUniform1<Real>::IntpAkimaUniform1 (int quantity, Real xMin,
    Real xSpacing, Real* F)
    :
    IntpAkima1<Real>(quantity, F)
{
    assertion(xSpacing > (Real)0, "Spacing must be positive\n");

    mXMin = xMin;
    mXMax = xMin + xSpacing*(quantity - 1);
    mXSpacing = xSpacing;

    // Compute slopes.
    Real invDX = ((Real)1)/xSpacing;
    Real* slope = new1<Real>(quantity + 3);
    int i, ip1, ip2;
    for (i = 0, ip1 = 1, ip2 = 2; i < quantity - 1; ++i, ++ip1, ++ip2)
    {
        slope[ip2] = (F[ip1] - F[i])*invDX;
    }

    slope[1] = ((Real)2)*slope[2] - slope[3];
    slope[0] = ((Real)2)*slope[1] - slope[2];
    slope[quantity + 1] = ((Real)2)*slope[quantity] - slope[quantity - 1];
    slope[quantity + 2] = ((Real)2)*slope[quantity + 1] - slope[quantity];

    // Construct derivatives.
    Real* FDer = new1<Real>(quantity);
    for (i = 0; i < quantity; ++i)
    {
        FDer[i] = ComputeDerivative(slope + i);
    }

    // Construct polynomials.
    Real invDX2 = ((Real)1)/(xSpacing*xSpacing);
    Real invDX3 = invDX2/xSpacing;
    for (i = 0, ip1 = 1; i < quantity - 1; ++i, ++ip1)
    {
        typename IntpAkima1<Real>::Polynomial& poly = mPoly[i];

        Real F0 = F[i];
        Real F1 = F[ip1];
        Real df = F1 - F0;
        Real FDer0 = FDer[i];
        Real FDer1 = FDer[ip1];

        poly[0] = F0;
        poly[1] = FDer0;
        poly[2] = (((Real)3)*df - xSpacing*(FDer1 + ((Real)2)*FDer0))*invDX2;
        poly[3] = (xSpacing*(FDer0 + FDer1) - ((Real)2)*df)*invDX3;
    }

    delete1(slope);
    delete1(FDer);
}
//----------------------------------------------------------------------------
template <typename Real>
IntpAkimaUniform1<Real>::~IntpAkimaUniform1 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform1<Real>::GetXMin () const
{
    return mXMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform1<Real>::GetXMax () const
{
    return mXMax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform1<Real>::GetXSpacing () const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpAkimaUniform1<Real>::Lookup (Real x, int& index, Real& dx) const
{
    if (x >= mXMin)
    {
        if (x <= mXMax)
        {
            for (index = 0; index + 1 < mQuantity; ++index)
            {
                if (x < mXMin + mXSpacing*(index + 1))
                {
                    dx = x - (mXMin + mXSpacing*index);
                    return true;
                }
            }

            --index;
            dx = x - (mXMin + mXSpacing*index);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpAkimaUniform1<float>;

template WM5_MATHEMATICS_ITEM
class IntpAkimaUniform1<double>;
//----------------------------------------------------------------------------
}
