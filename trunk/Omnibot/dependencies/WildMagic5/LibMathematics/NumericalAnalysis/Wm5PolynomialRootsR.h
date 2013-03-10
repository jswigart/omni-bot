// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POLYNOMIALROOTSR_H
#define WM5POLYNOMIALROOTSR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Rational.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM PolynomialRootsR
{
public:
    // Construction and destruction.
    PolynomialRootsR ();
    ~PolynomialRootsR ();

    // Member access.
    enum { INFINITE_QUANTITY = -1 };
    int GetQuantity () const;
    Real GetRoot (int i) const;
    int GetMultiplicity (int i) const;

    typedef Rational<16*sizeof(Real)> PRational;

    // p(x) = c0 + c1*x
    bool Linear (Real c0, Real c1);
    bool Linear (const PRational& c0, const PRational& c1);

    // p(x) = c0 + c1*x + c2*x^2
    // m(x) = a0 + a1*x + x^2
    bool Quadratic (Real c0, Real c1, Real c2);
    bool Quadratic (const PRational& c0, const PRational& c1,
        const PRational& c2);
    bool Quadratic (const PRational& a0, const PRational& a1);

    // p(x) = c0 + c1*x + c2*x^2 + c3*x^3
    // m(x) = a0 + a1*x + a2*x^2 + x^3
    bool Cubic (Real c0, Real c1, Real c2, Real c3);
    bool Cubic (const PRational& c0, const PRational& c1,
        const PRational& c2, const PRational& c3);
    bool Cubic (const PRational& a0, const PRational& a1,
        const PRational& a2);

    // p(x) = c0 + c1*x + c2*x^2 + c3*x^3 + c4*x^4 (monic when c4 = 1)
    // m(x) = c0 + c1*x + c2*x^2 + c3*x^3 + x^4
    bool Quartic (Real c0, Real c1, Real c2, Real c3, Real c4);
    bool Quartic (const PRational& c0, const PRational& c1,
        const PRational& c2, const PRational& c3, const PRational& c4);
    bool Quartic (const PRational& a0, const PRational& a1,
        const PRational& a2, const PRational& a3);

private:
    void SortRoots ();

    int mQuantity;
    Real mRoot[4];
    int mMultiplicity[4];

    static const PRational msZero;
    static const PRational msOne;
};

typedef PolynomialRootsR<float> PolynomialRootsRf;
typedef PolynomialRootsR<double> PolynomialRootsRd;

}

#endif
