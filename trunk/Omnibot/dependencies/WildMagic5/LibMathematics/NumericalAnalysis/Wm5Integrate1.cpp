// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Integrate1.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Real Integrate1<Real>::RombergIntegral (int order, Real a, Real b,
    Function function, void* userData)
{
    assertion(order > 0, "Integration order must be positive\n");
    Real** rom = new2<Real>(order, 2);

    Real h = b - a;

    rom[0][0] = ((Real)0.5)*h*(function(a, userData) + function(b, userData));
    for (int i0 = 2, p0 = 1; i0 <= order; ++i0, p0 *= 2, h *= (Real)0.5)
    {
        // Approximations via the trapezoid rule.
        Real sum = (Real)0;
        int i1;
        for (i1 = 1; i1 <= p0; ++i1)
        {
            sum += function(a + h*(i1-((Real)0.5)), userData);
        }

        // Richardson extrapolation.
        rom[1][0] = ((Real)0.5)*(rom[0][0] + h*sum);
        for (int i2 = 1, p2 = 4; i2 < i0; ++i2, p2 *= 4)
        {
            rom[1][i2] = (p2*rom[1][i2-1] - rom[0][i2-1])/(p2-1);
        }

        for (i1 = 0; i1 < i0; ++i1)
        {
            rom[0][i1] = rom[1][i1];
        }
    }

    Real result = rom[0][order-1];
    delete2(rom);
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Integrate1<Real>::GaussianQuadrature (Real a, Real b, Function function,
    void* userData)
{
    // Legendre polynomials:
    // P_0(x) = 1
    // P_1(x) = x
    // P_2(x) = (3x^2-1)/2
    // P_3(x) = x(5x^2-3)/2
    // P_4(x) = (35x^4-30x^2+3)/8
    // P_5(x) = x(63x^4-70x^2+15)/8

    // Generation of polynomials:
    //   d/dx[ (1-x^2) dP_n(x)/dx ] + n(n+1) P_n(x) = 0
    //   P_n(x) = sum_{k=0}^{floor(n/2)} c_k x^{n-2k}
    //     c_k = (-1)^k (2n-2k)! / [ 2^n k! (n-k)! (n-2k)! ]
    //   P_n(x) = ((-1)^n/(2^n n!)) d^n/dx^n[ (1-x^2)^n ]
    //   (n+1)P_{n+1}(x) = (2n+1) x P_n(x) - n P_{n-1}(x)
    //   (1-x^2) dP_n(x)/dx = -n x P_n(x) + n P_{n-1}(x)

    // Roots of the Legendre polynomial of specified degree.
    const int degree = 5;
    const Real root[degree] =
    {
        (Real)-0.9061798459,
        (Real)-0.5384693101,
        (Real) 0.0,
        (Real)+0.5384693101,
        (Real)+0.9061798459
    };
    const Real coeff[degree] =
    {
        (Real)0.2369268850,
        (Real)0.4786286705,
        (Real)0.5688888889,
        (Real)0.4786286705,
        (Real)0.2369268850
    };

    // Need to transform domain [a,b] to [-1,1].  If a <= x <= b and
    // -1 <= t <= 1, then x = ((b-a)*t+(b+a))/2.
    Real radius = ((Real)0.5)*(b - a);
    Real center = ((Real)0.5)*(b + a);

    Real result = (Real)0;
    for (int i = 0; i < degree; ++i)
    {
        result += coeff[i]*function(radius*root[i]+center, userData);
    }
    result *= radius;

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Integrate1<Real>::TrapezoidRule (int numSamples, Real a, Real b,
    Function function, void* userData)
{
    assertion(numSamples >= 2, "Must have more than two samples\n");

    Real h = (b - a)/(Real)(numSamples - 1);
    Real result = ((Real)0.5)*(function(a, userData) +
        function(b, userData));

    for (int i = 1; i <= numSamples - 2; ++i)
    {
        result += function(a+i*h, userData);
    }
    result *= h;
    return result;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Integrate1<float>;

template WM5_MATHEMATICS_ITEM
class Integrate1<double>;
//----------------------------------------------------------------------------
}
