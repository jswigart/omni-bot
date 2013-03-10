// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5PolynomialCurve2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PolynomialCurve2<Real>::PolynomialCurve2 (Polynomial1<Real>* xPoly,
    Polynomial1<Real>* yPoly)
    :
    SingleCurve2<Real>((Real)0, (Real)1)
{
    assertion(xPoly && yPoly, "Invalid input\n");
    assertion(xPoly->GetDegree() == yPoly->GetDegree(), "Invalid input\n");

    mXPoly = xPoly;
    mYPoly = yPoly;
    mXDer1 = mXPoly->GetDerivative();
    mYDer1 = mYPoly->GetDerivative();
    mXDer2 = mXDer1.GetDerivative();
    mYDer2 = mYDer1.GetDerivative();
    mXDer3 = mXDer2.GetDerivative();
    mYDer3 = mYDer2.GetDerivative();
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialCurve2<Real>::~PolynomialCurve2 ()
{
    delete0(mXPoly);
    delete0(mYPoly);
}
//----------------------------------------------------------------------------
template <typename Real>
int PolynomialCurve2<Real>::GetDegree () const
{
    return mXPoly->GetDegree();
}
//----------------------------------------------------------------------------
template <typename Real>
const Polynomial1<Real>* PolynomialCurve2<Real>::GetXPolynomial () const
{
    return mXPoly;
}
//----------------------------------------------------------------------------
template <typename Real>
const Polynomial1<Real>* PolynomialCurve2<Real>::GetYPolynomial () const
{
    return mYPoly;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> PolynomialCurve2<Real>::GetPosition (Real t) const
{
    return Vector2<Real>((*mXPoly)(t), (*mYPoly)(t));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> PolynomialCurve2<Real>::GetFirstDerivative (Real t) const
{
    return Vector2<Real>(mXDer1(t), mYDer1(t));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> PolynomialCurve2<Real>::GetSecondDerivative (Real t) const
{
    return Vector2<Real>(mXDer2(t) ,mYDer2(t));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> PolynomialCurve2<Real>::GetThirdDerivative (Real t) const
{
    return Vector2<Real>(mXDer3(t), mYDer3(t));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class PolynomialCurve2<float>;

template WM5_MATHEMATICS_ITEM
class PolynomialCurve2<double>;
//----------------------------------------------------------------------------
}
