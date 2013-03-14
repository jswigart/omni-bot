// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POLYNOMIALCURVE3_H
#define WM5POLYNOMIALCURVE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Polynomial1.h"
#include "Wm5SingleCurve3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM PolynomialCurve3 : public SingleCurve3<Real>
{
public:
    // Construction and destruction.  PolynomialCurve3 accepts responsibility
    // for deleting the input polynomials.
    PolynomialCurve3 (Polynomial1<Real>* xPoly, Polynomial1<Real>* yPoly,
        Polynomial1<Real>* zPoly);

    virtual ~PolynomialCurve3 ();

    int GetDegree () const;
    const Polynomial1<Real>* GetXPolynomial () const;
    const Polynomial1<Real>* GetYPolynomial () const;
    const Polynomial1<Real>* GetZPolynomial () const;

    virtual Vector3<Real> GetPosition (Real t) const;
    virtual Vector3<Real> GetFirstDerivative (Real t) const;
    virtual Vector3<Real> GetSecondDerivative (Real t) const;
    virtual Vector3<Real> GetThirdDerivative (Real t) const;

protected:
    Polynomial1<Real>* mXPoly;
    Polynomial1<Real>* mYPoly;
    Polynomial1<Real>* mZPoly;
    Polynomial1<Real> mXDer1, mYDer1, mZDer1;
    Polynomial1<Real> mXDer2, mYDer2, mZDer2;
    Polynomial1<Real> mXDer3, mYDer3, mZDer3;
};

typedef PolynomialCurve3<float> PolynomialCurve3f;
typedef PolynomialCurve3<double> PolynomialCurve3d;

}

#endif
