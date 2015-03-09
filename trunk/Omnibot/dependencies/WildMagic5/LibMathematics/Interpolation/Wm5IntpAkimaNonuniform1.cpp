// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpAkimaNonuniform1.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpAkimaNonuniform1<Real>::IntpAkimaNonuniform1 (int quantity, Real* X,
    Real* F)
    :
    IntpAkima1<Real>(quantity, F),
    mX(X)
{
    // Compute slopes.
    Real* slope = new1<Real>(quantity + 3);
    int i, ip1, ip2;
    for (i = 0, ip1 = 1, ip2 = 2; i < quantity - 1; ++i, ++ip1, ++ip2)
    {
        Real dx = X[ip1] - X[i];
        Real df = F[ip1] - F[i];
        slope[ip2] = df/dx;
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
    for (i = 0, ip1 = 1; i < quantity - 1; ++i, ++ip1)
    {
        typename IntpAkima1<Real>::Polynomial& poly = mPoly[i];

        Real F0 = F[i];
        Real F1 = F[ip1];
        Real FDer0 = FDer[i];
        Real FDer1 = FDer[ip1];
        Real df = F1 - F0;
        Real dx = X[ip1] - X[i];
        Real dx2 = dx*dx;
        Real dx3 = dx2*dx;

        poly[0] = F0;
        poly[1] = FDer0;
        poly[2] = (((Real)3)*df - dx*(FDer1 + ((Real)2)*FDer0))/dx2;
        poly[3] = (dx*(FDer0 + FDer1) - ((Real)2)*df)/dx3;
    }

    delete1(slope);
    delete1(FDer);
}
//----------------------------------------------------------------------------
template <typename Real>
IntpAkimaNonuniform1<Real>::~IntpAkimaNonuniform1 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* IntpAkimaNonuniform1<Real>::GetX () const
{
    return mX;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaNonuniform1<Real>::GetXMin () const
{
    return mX[0];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaNonuniform1<Real>::GetXMax () const
{
    return mX[mQuantity - 1];
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpAkimaNonuniform1<Real>::Lookup (Real x, int& index, Real& dx) const
{
    if (x >= mX[0])
    {
        if (x <= mX[mQuantity-1])
        {
            for (index = 0; index + 1 < mQuantity; ++index)
            {
                if (x < mX[index + 1])
                {
                    dx = x - mX[index];
                    return true;
                }
            }

            --index;
            dx = x - mX[index];
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
class IntpAkimaNonuniform1<float>;

template WM5_MATHEMATICS_ITEM
class IntpAkimaNonuniform1<double>;
//----------------------------------------------------------------------------
}
