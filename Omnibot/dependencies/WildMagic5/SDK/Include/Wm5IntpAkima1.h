// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPAKIMA1_H
#define WM5INTPAKIMA1_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpAkima1
{
public:
    // Abstract base class.
    virtual ~IntpAkima1 ();

    class WM5_MATHEMATICS_ITEM Polynomial
    {
    public:
        // P(x) = c[0] + c[1]*x + c[2]*x^2 + c[3]*x^3
        inline Real& operator[] (int i) { return mCoeff[i]; }
        Real operator() (Real x) const;
        Real operator() (int order, Real x) const;

    private:
        Real mCoeff[4];
    };

    int GetQuantity () const;
    const Real* GetF () const;
    const Polynomial* GetPolynomials () const;
    const Polynomial& GetPolynomial (int i) const;

    virtual Real GetXMin () const = 0;
    virtual Real GetXMax () const = 0;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax.  If x is outside the
    // extremes, the function returns MAXREAL.  The first operator is for
    // function evaluation.  The second operator is for function or derivative
    // evaluations.  The uiOrder argument is the order of the derivative, zero
    // for the function itself.
    Real operator() (Real x) const;
    Real operator() (int order, Real x) const;

protected:
    // Construction.  IntpAkima1 does not accept responsibility for
    // deleting the input array.  The application must do so.
    IntpAkima1 (int quantity, Real* F);

    Real ComputeDerivative (Real* slope) const;
    virtual bool Lookup (Real x, int& index, Real& dx) const = 0;

    int mQuantity;
    Real* mF;
    Polynomial* mPoly;
};

typedef IntpAkima1<float> IntpAkima1f;
typedef IntpAkima1<double> IntpAkima1d;

}

#endif
