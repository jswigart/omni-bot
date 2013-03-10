// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#include "Wm5MathematicsPCH.h"
#include "Wm5PolynomialRootsR.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PolynomialRootsR<Real>::PolynomialRootsR ()
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialRootsR<Real>::~PolynomialRootsR ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
int PolynomialRootsR<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRootsR<Real>::GetRoot (int i) const
{
    assertion(0 <= i && i < mQuantity, "Invalid index\n");
    return mRoot[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int PolynomialRootsR<Real>::GetMultiplicity (int i) const
{
    assertion(0 <= i && i < mQuantity, "Invalid index\n");
    return mMultiplicity[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Linear (Real c0, Real c1)
{
    if (c1 != (Real)0)
    {
        // The equation is c1*x + c0 = 0, where c1 is not zero.
        PRational ratRoot = PRational(-c0)/PRational(c1);
        mQuantity = 1;
        ratRoot.ConvertTo(mRoot[0]);
        mMultiplicity[0] = 1;
        return true;
    }

    if (c0 != (Real)0)
    {
        // The equation is c0 = 0, where c0 is not zero, so there are no
        // solutions.
        mQuantity = 0;
        return false;
    }

    // The polynomial equation is a tautology, 0 = 0, so there are
    // infinitely many solutions.
    mQuantity = INFINITE_QUANTITY;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Linear (const PRational& c0,
    const PRational& c1)
{
    if (c1 != msZero)
    {
        // The equation is c1*x + c0 = 0, where c1 is not zero.
        PRational ratRoot = c0/c1;
        mQuantity = 1;
        ratRoot.ConvertTo(mRoot[0]);
        mRoot[0] = -mRoot[0];
        mMultiplicity[0] = 1;
        return true;
    }

    if (c0 != msZero)
    {
        // The equation is c0 = 0, where c0 is not zero, so there are no
        // solutions.
        mQuantity = 0;
        return false;
    }

    // The polynomial equation is a tautology, 0 = 0, so there are
    // infinitely many solutions.
    mQuantity = INFINITE_QUANTITY;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Quadratic (Real c0, Real c1, Real c2)
{
    if (c2 == (Real)0)
    {
        return Linear(c0, c1);
    }

    // The equation is c2*x^2 + c1*x + c0 = 0, where c2 is not zero.
    PRational ratC0(c0), ratC1(c1), ratC2(c2);

    // Create a monic polynomial, x^2 + a1*x + a0 = 0.
    PRational ratInvC2 = msOne/ratC2;
    PRational ratA1 = ratC1*ratInvC2;
    PRational ratA0 = ratC0*ratInvC2;

    // Solve the equation.
    return Quadratic(ratA0, ratA1);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Quadratic (const PRational& c0,
    const PRational& c1, const PRational& c2)
{
    if (c2 == msZero)
    {
        return Linear(c0, c1);
    }

    // The equation is c2*x^2 + c1*x + c0 = 0, where c2 is not zero.  Create
    // a monic polynomial, x^2 + a1*x + a0 = 0.
    PRational ratInvC2 = msOne/c2;
    PRational ratA1 = c1*ratInvC2;
    PRational ratA0 = c0*ratInvC2;

    // Solve the equation.
    return Quadratic(ratA0, ratA1);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Quadratic (const PRational& a0,
    const PRational& a1)
{
    PRational ratMHalf(-1,2);
    PRational ratMA1Div2 = ratMHalf*a1;
    PRational ratDiscr = ratMA1Div2*ratMA1Div2 - a0;
    if (ratDiscr > msZero)
    {
        // Two distinct real-valued roots.
        mQuantity = 2;

        // Estimate the discriminant.
        double discr;
        ratDiscr.ConvertTo(discr);
        assertion(discr > 0.0, "Unexpected condition\n");
        discr = Mathd::Sqrt(discr);

        ratDiscr = PRational(discr);
        PRational ratRoot0 = ratMA1Div2 - ratDiscr;
        PRational ratRoot1 = ratMA1Div2 + ratDiscr;
        ratRoot0.ConvertTo(mRoot[0]);
        ratRoot1.ConvertTo(mRoot[1]);
        mMultiplicity[0] = 1;
        mMultiplicity[1] = 1;
    }
    else if (ratDiscr == msZero)
    {
        // One repeated real-valued root.
        mQuantity = 1;
        ratMA1Div2.ConvertTo(mRoot[0]);
        mMultiplicity[0] = 2;
    }
    else
    {
        // No real-valued roots.
        mQuantity = 0;
    }

    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Cubic (Real c0, Real c1, Real c2, Real c3)
{
    if (c3 == (Real)0)
    {
        return Quadratic(c0, c1, c2);
    }

    // The equation is c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is not zero.
    PRational ratC0(c0), ratC1(c1), ratC2(c2), ratC3(c3);

    // Create a monic polynomial, x^3 + a2*x^2 + a1*x + a0 = 0.
    PRational ratInvC3 = msOne/ratC3;
    PRational ratA2 = ratC2*ratInvC3;
    PRational ratA1 = ratC1*ratInvC3;
    PRational ratA0 = ratC0*ratInvC3;

    // Solve the equation.
    return Cubic(ratA0, ratA1, ratA2);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Cubic (const PRational& c0,
    const PRational& c1, const PRational& c2, const PRational& c3)
{
    if (c3 == msZero)
    {
        return Quadratic(c0, c1, c2);
    }

    // The equation is c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is not zero.
    // Create a monic polynomial, x^3 + a2*x^2 + a1*x + a0 = 0.
    PRational ratInvC3 = msOne/c3;
    PRational ratA2 = c2*ratInvC3;
    PRational ratA1 = c1*ratInvC3;
    PRational ratA0 = c0*ratInvC3;

    // Solve the equation.
    return Cubic(ratA0, ratA1, ratA2);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Cubic (const PRational& a0,
    const PRational& a1, const PRational& a2)
{
    // Reduce the equation to y^3 + b1*y + b0 = 0.
    PRational ratHalf(1,2), ratThird(1,3), ratTwo(2);
    PRational ratA2Div3 = ratThird*a2;
    PRational ratA2Div3Sqr = ratA2Div3*ratA2Div3;
    PRational ratA2Div3Cube = ratA2Div3*ratA2Div3Sqr;
    PRational ratB1 = a1 - ratA2Div3*a2;
    PRational ratB0 = a0 - a1*ratA2Div3 + ratTwo*ratA2Div3Cube;

    // Solve the equation.
    PRational ratQ = ratThird*ratB1, ratR = ratHalf*ratB0;
    PRational ratDiscr = ratR*ratR + ratQ*ratQ*ratQ;
    if (ratDiscr > msZero)
    {
        // One real-valued root, two complex-valued conjugate roots.
        mQuantity = 1;

        // Estimate the discriminant.
        double discr;
        ratDiscr.ConvertTo(discr);
        assertion(discr > 0.0, "Unexpected condition\n");
        discr = Mathd::Sqrt(discr);

        const double third = 1.0/3.0;
        ratDiscr = PRational(discr);

        PRational ratSum0 = ratR + ratDiscr;
        double sum0;
        ratSum0.ConvertTo(sum0);
        if (sum0 >= 0.0)
        {
            sum0 = Mathd::Pow(sum0, third);
        }
        else
        {
            sum0 = -Mathd::Pow(-sum0, third);
        }
        ratSum0 = PRational(ratSum0);

        PRational ratSum1 = ratR - ratDiscr;
        double sum1;
        ratSum1.ConvertTo(sum1);
        if (sum1 >= 0.0)
        {
            sum1 = Mathd::Pow(sum1, third);
        }
        else
        {
            sum1 = -Mathd::Pow(-sum1, third);
        }
        ratSum1 = PRational(sum1);

        PRational ratRoot = ratA2Div3 + ratSum0 + ratSum1;
        ratRoot.ConvertTo(mRoot[0]);
        mRoot[0] = -mRoot[0];
        mMultiplicity[0] = 1;
    }
    else if (ratDiscr < msZero)
    {
        // Three distinct real-valued roots.
        mQuantity = 3;
        mMultiplicity[0] = 1;
        mMultiplicity[1] = 1;
        mMultiplicity[2] = 1;

        // Compute the eigenvalues by solving for the roots of the polynomial.
        double negQ;
        ratQ.ConvertTo(negQ);
        negQ = -negQ;
        assertion(negQ > 0.0, "Unexpected condition\n");

        double negR;
        ratR.ConvertTo(negR);
        negR = -negR;

        double negDiscr;
        ratDiscr.ConvertTo(negDiscr);
        negDiscr = -negDiscr;

        double negA2d3;
        ratA2Div3.ConvertTo(negA2d3);
        negA2d3 = -negA2d3;

        double sqrt3 = Mathd::Sqrt(3.0);
        double magnitude = Mathd::Sqrt(negQ);
        double angle = Mathd::ATan2(Mathd::Sqrt(negDiscr), negR)/3.0;
        double cs = Mathd::Cos(angle);
        double sn = Mathd::Sin(angle);
        double root0 = negA2d3 + 2.0*magnitude*cs;
        double root1 = negA2d3 - magnitude*(cs + sqrt3*sn);
        double root2 = negA2d3 - magnitude*(cs - sqrt3*sn);

        // Sort in increasing order.
        if (root1 >= root0)
        {
            mRoot[0] = (Real)root0;
            mRoot[1] = (Real)root1;
        }
        else
        {
            mRoot[0] = (Real)root1;
            mRoot[1] = (Real)root0;
        }

        if (root2 >= (double)mRoot[1])
        {
            mRoot[2] = (Real)root2;
        }
        else
        {
            mRoot[2] = mRoot[1];
            if (root2 >= (double)mRoot[0])
            {
                mRoot[1] = (Real)root2;
            }
            else
            {
                mRoot[1] = mRoot[0];
                mRoot[0] = (Real)root2;
            }
        }
    }
    else
    {
        // Three real-valued roots, at least two of which are equal.
        if (ratQ != msZero)
        {
            // Two real-valued roots, one repeated.
            mQuantity = 2;

            const double third = 1.0/3.0;
            double r;
            ratR.ConvertTo(r);
            if (r >= 0.0)
            {
                r = Mathd::Pow(r, third);
            }
            else
            {
                r = -Mathd::Pow(-r, third);
            }
            ratR = PRational(r);

            PRational ratRoot0 = ratA2Div3 - ratR;
            ratRoot0.ConvertTo(mRoot[0]);
            mRoot[0] = -mRoot[0];
            mMultiplicity[0] = 2;

            PRational ratRoot1 = ratA2Div3 + ratTwo*ratR;
            ratRoot1.ConvertTo(mRoot[1]);
            mRoot[1] = -mRoot[1];
            mMultiplicity[1] = 1;

            if (mRoot[1] < mRoot[0])
            {
                Real save = mRoot[0];
                mRoot[0] = mRoot[1];
                mRoot[1] = save;
                int isave = mMultiplicity[0];
                mMultiplicity[0] = mMultiplicity[1];
                mMultiplicity[1] = isave;
            }
        }
        else
        {
            // One real-valued root, all repeated.
            mQuantity = 1;
            ratA2Div3.ConvertTo(mRoot[0]);
            mRoot[0] = -mRoot[0];
            mMultiplicity[0] = 3;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Quartic (Real c0, Real c1, Real c2, Real c3,
    Real c4)
{
    if (c4 == (Real)0)
    {
        return Cubic(c0, c1, c2, c3);
    }

    // The equation is c4*x^4 + c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is
    // not zero.
    PRational ratC0(c0), ratC1(c1), ratC2(c2), ratC3(c3), ratC4(c4);

    // Create a monic polynomial, x^4 + a3*x^3 + a2*x^2 + a1*x + a0 = 0.
    PRational ratInvC4 = msOne/ratC4;
    PRational ratA3 = ratC3*ratInvC4;
    PRational ratA2 = ratC2*ratInvC4;
    PRational ratA1 = ratC1*ratInvC4;
    PRational ratA0 = ratC0*ratInvC4;

    // Solve the equation.
    return Quartic(ratA0, ratA1, ratA2, ratA3);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Quartic (const PRational& c0,
    const PRational& c1, const PRational& c2, const PRational& c3,
    const PRational& c4)
{
    if (c4 == msZero)
    {
        return Cubic(c0, c1, c2, c3);
    }

    // The equation is c4*x^4 + c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is
    // not zero.  Create a monic polynomial,
    // x^4 + a3*x^3 + a2*x^2 + a1*x + a0 = 0.
    PRational ratInvC4 = msOne/c4;
    PRational ratA3 = c3*ratInvC4;
    PRational ratA2 = c2*ratInvC4;
    PRational ratA1 = c1*ratInvC4;
    PRational ratA0 = c0*ratInvC4;

    // Solve the equation.
    return Quartic(ratA0, ratA1, ratA2, ratA3);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRootsR<Real>::Quartic (const PRational& a0,
    const PRational& a1, const PRational& a2, const PRational& a3)
{
    mQuantity = 0;

    // Reduction to resolvent cubic polynomial y^3 + r2*y^2 + r1*y + r0 = 0.
    PRational ratHalf(1,2), ratFourth(1,4), ratEighth(1,8);
    PRational ratFour(4), ratTwo(2);
    PRational ratR2 = -ratHalf*a2;
    PRational ratR1 = ratFourth*a1*a3 - a0;
    PRational ratR0 = -ratEighth*(a1*a1 + a0*(a3*a3 - ratFour*a2));

    // This always produces at least one root.
    PolynomialRootsR<Real> poly;
    poly.Cubic(ratR0, ratR1, ratR2);
    PRational ratY(poly.GetRoot(0));

    PRational ratAlphaSqr = ratFourth*a3*a3 - a2 + ratTwo*ratY;
    double alphaSqr;
    ratAlphaSqr.ConvertTo(alphaSqr);
    if (alphaSqr < 0.0)
    {
        return false;
    }

    int i;

    if (alphaSqr > 0.0)
    {
        double alpha = Mathd::Sqrt(alphaSqr);
        PRational ratAlpha = PRational(alpha);
        PRational ratBeta = ratHalf*(a3*ratY - a1)/ratAlpha;

        PRational ratB0 = ratY - ratBeta;
        PRational ratB1 = ratHalf*a3 - ratAlpha;
        poly.Quadratic(ratB0, ratB1);
        for (i = 0; i < poly.GetQuantity(); ++i)
        {
            mRoot[mQuantity] = poly.GetRoot(i);
            mMultiplicity[mQuantity] = poly.GetMultiplicity(i);
            ++mQuantity;
        }

        ratB0 = ratY + ratBeta;
        ratB1 = ratHalf*a3 + ratAlpha;
        poly.Quadratic(ratB0, ratB1);
        for (i = 0; i < poly.GetQuantity(); ++i)
        {
            mRoot[mQuantity] = poly.GetRoot(i);
            mMultiplicity[mQuantity] = poly.GetMultiplicity(i);
            ++mQuantity;
        }

        SortRoots();
        return mQuantity > 0;
    }

    PRational ratBetaSqr = ratY*ratY - a0;
    double betaSqr;
    ratBetaSqr.ConvertTo(betaSqr);
    if (betaSqr < 0.0)
    {
        return false;
    }

    if (betaSqr > 0.0)
    {
        double beta = Mathd::Sqrt(betaSqr);
        PRational ratBeta(beta);

        PRational ratB0 = ratY - ratBeta;
        PRational ratB1 = ratHalf*a3;
        poly.Quadratic(ratB0, ratB1);
        for (i = 0; i < poly.GetQuantity(); ++i)
        {
            mRoot[mQuantity] = poly.GetRoot(i);
            mMultiplicity[mQuantity] = poly.GetMultiplicity(i);
            ++mQuantity;
        }

        ratB0 = ratY + ratBeta;
        poly.Quadratic(ratB0, ratB1);
        for (i = 0; i < poly.GetQuantity(); ++i)
        {
            mRoot[mQuantity] = poly.GetRoot(i);
            mMultiplicity[mQuantity] = poly.GetMultiplicity(i);
            ++mQuantity;
        }

        SortRoots();
        return mQuantity > 0;
    }

    poly.Quadratic(ratY, ratHalf*a3);
    for (i = 0; i < poly.GetQuantity(); ++i)
    {
        mRoot[mQuantity] = poly.GetRoot(i);
        mMultiplicity[mQuantity] = 2*poly.GetMultiplicity(i);
        ++mQuantity;
    }

    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRootsR<Real>::SortRoots ()
{
    // Sort the roots as: root[0] <= ... <= root[quantity-1].
    int i0, i1;
    for (i0 = 0; i0 <= mQuantity-2; ++i0)
    {
        // Locate the minimum root.
        i1 = i0;
        Real minRoot = mRoot[i1];
        int minMult = mMultiplicity[i1];
        for (int i2 = i0 + 1; i2 < mQuantity; ++i2)
        {
            if (mRoot[i2] < minRoot)
            {
                i1 = i2;
                minRoot = mRoot[i1];
                minMult = mMultiplicity[i1];
            }
        }

        if (i1 != i0)
        {
            // Swap the roots and multiplicities.
            mRoot[i1] = mRoot[i0];
            mRoot[i0] = minRoot;
            mMultiplicity[i1] = mMultiplicity[i0];
            mMultiplicity[i0] = minMult;
        }
    }

    // Combine the multiplicities, if necessary.
    for (i0 = 0; i0 < mQuantity-1; /**/)
    {
        if (mRoot[i0] == mRoot[i0+1])
        {
            // Combine the multiplicities.
            mMultiplicity[i0] += mMultiplicity[i0+1];

            // Eliminate the redundant root by shifting the array elements.
            mQuantity--;
            for (i1 = i0 + 1; i1 < mQuantity; i1++)
            {
                mRoot[i1] = mRoot[i1+1];
                mMultiplicity[i1] = mMultiplicity[i1+1];
            }
        }
        else
        {
            i0++;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template<> const PolynomialRootsR<float>::PRational
PolynomialRootsR<float>::msZero(0);

template<> const PolynomialRootsR<float>::PRational
PolynomialRootsR<float>::msOne(1);

template WM5_MATHEMATICS_ITEM
class PolynomialRootsR<float>;

template<> const PolynomialRootsR<double>::PRational
PolynomialRootsR<double>::msZero(0);

template<> const PolynomialRootsR<double>::PRational
PolynomialRootsR<double>::msOne(1);

template WM5_MATHEMATICS_ITEM
class PolynomialRootsR<double>;
//----------------------------------------------------------------------------
}
