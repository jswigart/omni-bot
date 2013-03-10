// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpAkima1.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpAkima1<Real>::IntpAkima1 (int quantity, Real* F)
{
    // At least three data points are needed to construct the estimates of
    // the boundary derivatives.
    assertion(quantity >= 3 && F, "Invalid input\n");

    mQuantity = quantity;
    mF = F;
    mPoly = new1<Polynomial>(quantity - 1 );
}
//----------------------------------------------------------------------------
template <typename Real>
IntpAkima1<Real>::~IntpAkima1 ()
{
    delete1(mPoly);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpAkima1<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* IntpAkima1<Real>::GetF () const
{
    return mF;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename IntpAkima1<Real>::Polynomial*
IntpAkima1<Real>::GetPolynomials () const
{
    return mPoly;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename IntpAkima1<Real>::Polynomial&
IntpAkima1<Real>::GetPolynomial (int i) const
{
    return mPoly[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkima1<Real>::ComputeDerivative (Real* slope) const
{
    if (slope[1] != slope[2])
    {
        if (slope[0] != slope[1])
        {
            if (slope[2] != slope[3])
            {
                Real ad0 = Math<Real>::FAbs(slope[3] - slope[2]);
                Real ad1 = Math<Real>::FAbs(slope[0] - slope[1]);
                return (ad0*slope[1] + ad1*slope[2])/(ad0+ad1);
            }
            else
            {
                return slope[2];
            }
        }
        else
        {
            if (slope[2] != slope[3])
            {
                return slope[1];
            }
            else
            {
                return ((Real)0.5)*(slope[1] + slope[2]);
            }
        }
    }
    else
    {
        return slope[1];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkima1<Real>::operator() (Real x) const
{
    int index;
    Real dx;

    if (Lookup(x, index, dx))
    {
        return mPoly[index](dx);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkima1<Real>::operator() (int order, Real x) const
{
    int index;
    Real dx;

    if (Lookup(x, index, dx))
    {
        return mPoly[index](order, dx);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IntpAkima1::Polynomial
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkima1<Real>::Polynomial::operator() (Real x) const
{
    return mCoeff[0] + x*(mCoeff[1] + x*(mCoeff[2] + x*mCoeff[3]));
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkima1<Real>::Polynomial::operator() (int order, Real x) const
{
    switch (order)
    {
    case 0:
        return mCoeff[0] + x*(mCoeff[1] + x*(mCoeff[2] + x*mCoeff[3]));
    case 1:
        return mCoeff[1] + x*(((Real)2)*mCoeff[2] + x*((Real)3)*mCoeff[3]);
    case 2:
        return ((Real)2)*mCoeff[2] + x*((Real)6)*mCoeff[3];
    case 3:
        return ((Real)6)*mCoeff[3];
    }

    return (Real)0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpAkima1<float>;

template WM5_MATHEMATICS_ITEM
class IntpAkima1<double>;
//----------------------------------------------------------------------------
}
