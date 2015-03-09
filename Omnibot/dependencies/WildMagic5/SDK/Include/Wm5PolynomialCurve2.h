// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POLYNOMIALCURVE2_H
#define WM5POLYNOMIALCURVE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Polynomial1.h"
#include "Wm5SingleCurve2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM PolynomialCurve2 : public SingleCurve2<Real>
{
public:
    // Construction and destruction.  PolynomialCurve2 accepts responsibility
    // for deleting the input polynomials.
    PolynomialCurve2 (Polynomial1<Real>* xPoly, Polynomial1<Real>* yPoly);
    virtual ~PolynomialCurve2 ();

    int GetDegree () const;
    const Polynomial1<Real>* GetXPolynomial () const;
    const Polynomial1<Real>* GetYPolynomial () const;

    virtual Vector2<Real> GetPosition (Real t) const;
    virtual Vector2<Real> GetFirstDerivative (Real t) const;
    virtual Vector2<Real> GetSecondDerivative (Real t) const;
    virtual Vector2<Real> GetThirdDerivative (Real t) const;

protected:
    Polynomial1<Real>* mXPoly;
    Polynomial1<Real>* mYPoly;
    Polynomial1<Real> mXDer1, mYDer1;
    Polynomial1<Real> mXDer2, mYDer2;
    Polynomial1<Real> mXDer3, mYDer3;
};

typedef PolynomialCurve2<float> PolynomialCurve2f;
typedef PolynomialCurve2<double> PolynomialCurve2d;

}

#endif
