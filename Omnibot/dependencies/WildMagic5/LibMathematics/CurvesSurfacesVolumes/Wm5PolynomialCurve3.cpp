// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5PolynomialCurve3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PolynomialCurve3<Real>::PolynomialCurve3 (Polynomial1<Real>* xPoly,
    Polynomial1<Real>* yPoly, Polynomial1<Real>* zPoly)
    :
    SingleCurve3<Real>((Real)0, (Real)1)
{
    assertion(xPoly && yPoly && zPoly, "Invalid input\n");
    assertion(xPoly->GetDegree() == yPoly->GetDegree(), "Invalid input\n");
    assertion(xPoly->GetDegree() == zPoly->GetDegree(), "Invalid input\n");

    mXPoly = xPoly;
    mYPoly = yPoly;
    mZPoly = zPoly;
    mXDer1 = mXPoly->GetDerivative();
    mYDer1 = mYPoly->GetDerivative();
    mZDer1 = mZPoly->GetDerivative();
    mXDer2 = mXDer1.GetDerivative();
    mYDer2 = mYDer1.GetDerivative();
    mZDer2 = mZDer1.GetDerivative();
    mXDer3 = mXDer2.GetDerivative();
    mYDer3 = mYDer2.GetDerivative();
    mZDer3 = mZDer2.GetDerivative();
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialCurve3<Real>::~PolynomialCurve3 ()
{
    delete0(mXPoly);
    delete0(mYPoly);
    delete0(mZPoly);
}
//----------------------------------------------------------------------------
template <typename Real>
int PolynomialCurve3<Real>::GetDegree () const
{
    return mXPoly->GetDegree();
}
//----------------------------------------------------------------------------
template <typename Real>
const Polynomial1<Real>* PolynomialCurve3<Real>::GetXPolynomial () const
{
    return mXPoly;
}
//----------------------------------------------------------------------------
template <typename Real>
const Polynomial1<Real>* PolynomialCurve3<Real>::GetYPolynomial () const
{
    return mYPoly;
}
//----------------------------------------------------------------------------
template <typename Real>
const Polynomial1<Real>* PolynomialCurve3<Real>::GetZPolynomial () const
{
    return mZPoly;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> PolynomialCurve3<Real>::GetPosition (Real t) const
{
    return Vector3<Real>((*mXPoly)(t), (*mYPoly)(t), (*mZPoly)(t));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> PolynomialCurve3<Real>::GetFirstDerivative (Real t) const
{
    return Vector3<Real>(mXDer1(t), mYDer1(t), mZDer1(t));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> PolynomialCurve3<Real>::GetSecondDerivative (Real t) const
{
    return Vector3<Real>(mXDer2(t), mYDer2(t), mZDer2(t));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> PolynomialCurve3<Real>::GetThirdDerivative (Real t) const
{
    return Vector3<Real>(mXDer3(t), mYDer3(t), mZDer3(t));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class PolynomialCurve3<float>;

template WM5_MATHEMATICS_ITEM
class PolynomialCurve3<double>;
//----------------------------------------------------------------------------
}
