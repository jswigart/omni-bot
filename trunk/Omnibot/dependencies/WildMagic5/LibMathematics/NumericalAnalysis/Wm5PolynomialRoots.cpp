// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5PolynomialRoots.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PolynomialRoots<Real>::PolynomialRoots (Real epsilon)
    :
    Epsilon(epsilon),
    MaxIterations(128),
    mCount(0),
    mMaxRoot(4)  // default support for degree <= 4
{
    assertion(Epsilon >= (Real)0, "Epsilon must be nonnegative\n");
    mRoot = new1<Real>(mMaxRoot);
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialRoots<Real>::~PolynomialRoots ()
{
    delete1(mRoot);
}
//----------------------------------------------------------------------------
template <typename Real>
int PolynomialRoots<Real>::GetCount () const
{
    return mCount;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* PolynomialRoots<Real>::GetRoots () const
{
    return mRoot;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetRoot (int i) const
{
    assertion(0 <= i && i < mCount, "Invalid index\n");
    return mRoot[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// degree 1
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindA (Real c0, Real c1)
{
    if (Math<Real>::FAbs(c1) >= Epsilon)
    {
        mRoot[0] = -c0/c1;
        mCount = 1;
        return true;
    }

    mCount = 0;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetBound (Real c0, Real c1)
{
    if (Math<Real>::FAbs(c1) <= Epsilon)
    {
        // Polynomial is constant, return invalid bound.
        return (Real)-1;
    }

    Real invC1 = ((Real)1)/c1;
    Real maxValue = Math<Real>::FAbs(c0)*invC1;
    return (Real)1 + maxValue;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// degree 2
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindA (Real c0, Real c1, Real c2)
{
    if (Math<Real>::FAbs(c2) <= Epsilon)
    {
        // Polynomial is linear.
        return FindA(c0, c1);
    }

    Real discr = c1*c1 - ((Real)4)*c0*c2;
    if (Math<Real>::FAbs(discr) <= Epsilon)
    {
        discr = (Real)0;
    }

    if (discr < (Real)0)
    {
        mCount = 0;
        return false;
    }

    Real tmp = ((Real)0.5)/c2;

    if (discr > (Real)0)
    {
        discr = Math<Real>::Sqrt(discr);
        mRoot[0] = tmp*(-c1 - discr);
        mRoot[1] = tmp*(-c1 + discr);
        mCount = 2;
    }
    else
    {
        mRoot[0] = -tmp*c1;
        mCount = 1;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetBound (Real c0, Real c1, Real c2)
{
    if (Math<Real>::FAbs(c2) <= Epsilon)
    {
        // Polynomial is linear.
        return (FindA(c0, c1) ? mRoot[0] : Math<Real>::MAX_REAL);
    }

    Real invC2 = ((Real)1)/c2;
    Real tmp0 = Math<Real>::FAbs(c0)*invC2;
    Real tmp1 = Math<Real>::FAbs(c1)*invC2;
    Real maxValue = (tmp0 >= tmp1 ? tmp0 : tmp1);
    return (Real)1 + maxValue;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// degree 3
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindA (Real c0, Real c1, Real c2, Real c3)
{
    if (Math<Real>::FAbs(c3) <= Epsilon)
    {
        // Polynomial is quadratic.
        return FindA(c0, c1, c2);
    }

    // Make polynomial monic, x^3+c2*x^2+c1*x+c0.
    Real invC3 = ((Real)1)/c3;
    c0 *= invC3;
    c1 *= invC3;
    c2 *= invC3;

    // Convert to y^3+a*y+b = 0 by x = y-c2/3.
    const Real third = (Real)1/(Real)3;
    const Real twentySeventh = (Real)1/(Real)27;
    Real offset = third*c2;
    Real a = c1 - c2*offset;
    Real b = c0+c2*(((Real)2)*c2*c2-((Real)9)*c1)*twentySeventh;
    Real halfB = ((Real)0.5)*b;

    Real discr = halfB*halfB + a*a*a*twentySeventh;
    if (Math<Real>::FAbs(discr) <= Epsilon)
    {
        discr = (Real)0;
    }

    if (discr > (Real)0)  // 1 real, 2 complex roots
    {
        discr = Math<Real>::Sqrt(discr);
        Real temp = -halfB + discr;
        if (temp >= (Real)0)
        {
            mRoot[0] = Math<Real>::Pow(temp, third);
        }
        else
        {
            mRoot[0] = -Math<Real>::Pow(-temp, third);
        }
        temp = -halfB - discr;
        if (temp >= (Real)0)
        {
            mRoot[0] += Math<Real>::Pow(temp, third);
        }
        else
        {
            mRoot[0] -= Math<Real>::Pow(-temp, third);
        }
        mRoot[0] -= offset;
        mCount = 1;
    }
    else if (discr < (Real)0) 
    {
        const Real sqrt3 = Math<Real>::Sqrt((Real)3);
        Real dist = Math<Real>::Sqrt(-third*a);
        Real angle = third*Math<Real>::ATan2(Math<Real>::Sqrt(-discr),
            -halfB);
        Real cs = Math<Real>::Cos(angle);
        Real sn = Math<Real>::Sin(angle);
        mRoot[0] = ((Real)2)*dist*cs - offset;
        mRoot[1] = -dist*(cs + sqrt3*sn) - offset;
        mRoot[2] = -dist*(cs - sqrt3*sn) - offset;
        mCount = 3;
    }
    else 
    {
        Real temp;
        if (halfB >= (Real)0)
        {
            temp = -Math<Real>::Pow(halfB, third);
        }
        else
        {
            temp = Math<Real>::Pow(-halfB, third);
        }
        mRoot[0] = ((Real)2)*temp - offset;
        mRoot[1] = -temp - offset;
        mRoot[2] = mRoot[1];
        mCount = 3;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindE (Real c0, Real c1, Real c2, Real c3,
    bool doBalancing)
{
    if (Math<Real>::FAbs(c3) <= Epsilon)
    {
        // Polynomial is quadratic.
        return FindA(c0, c1, c2);
    }

    // Make polynomial monic, x^3+c2*x^2+c1*x+c0.
    Real invC3 = ((Real)1)/c3;
    c0 *= invC3;
    c1 *= invC3;
    c2 *= invC3;

    // Construct the 3-by-3 companion matrix.
    GMatrix<Real> mat(3, 3);  // initialized to zero
    mat[1][0] = (Real)1;
    mat[2][1] = (Real)1;
    mat[0][2] = -c0;
    mat[1][2] = -c1;
    mat[2][2] = -c2;

    if (doBalancing)
    {
        BalanceCompanion3(mat);
    }

    return QRIteration3(mat);
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetBound (Real c0, Real c1, Real c2, Real c3)
{
    if (Math<Real>::FAbs(c3) <= Epsilon)
    {
        // Polynomial is quadratic.
        return GetBound(c0, c1, c2);
    }

    Real invC3 = ((Real)1)/c3;
    Real maxValue = Math<Real>::FAbs(c0)*invC3;

    Real tmp = Math<Real>::FAbs(c1)*invC3;
    if (tmp > maxValue)
    {
        maxValue = tmp;
    }

    tmp = Math<Real>::FAbs(c2)*invC3;
    if (tmp > maxValue)
    {
        maxValue = tmp;
    }

    return (Real)1 + maxValue;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::SpecialCubic (Real a, Real b, Real c)
{
    // Solve A*r^3 + B*r = C where A > 0 and B > 0.
    //
    // Let r = D*sinh(u) where D = sqrt(4*B/(3*A)).  Then
    // sinh(3*u) = 4*[sinh(u)]^3+3*sinh(u) = E where E = 4*C/(A*D^3).
    // sinh(3*u) = E has solution u = (1/3)*log(E+sqrt(E^2+1)).  This
    // leads to sinh(u) = ((E+sqrt(E^2+1))^{1/3}-(E+sqrt(E^2+1))^{-1/3})/2.
    // Therefore,  r = D*((E+sqrt(E^2+1))^{1/3}-(E+sqrt(E^2+1))^{-1/3})/2.

    const Real third = (Real)1/(Real)3;
    Real d = Math<Real>::Sqrt(((Real)4)*third*b/a);
    Real e = ((Real)4)*c/(a*d*d*d);
    Real f = Math<Real>::Pow(e + Math<Real>::Sqrt(e*e + (Real)1), third);
    Real root = ((Real)0.5)*d*(f - ((Real)1)/f);
    return root;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetRowNorm (int row, GMatrix<Real>& mat)
{
    Real norm = Math<Real>::FAbs(mat[row][0]);
    for (int col = 1; col < mat.GetNumColumns(); ++col)
    {
        Real absValue = Math<Real>::FAbs(mat[row][col]);
        if (absValue > norm)
        {
            norm = absValue;
        }
    }
    return norm;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetColNorm (int col, GMatrix<Real>& mat)
{
    Real norm = Math<Real>::FAbs(mat[0][col]);
    for (int row = 1; row < mat.GetNumRows(); ++row)
    {
        Real absValue = Math<Real>::FAbs(mat[row][col]);
        if (absValue > norm)
        {
            norm = absValue;
        }
    }
    return norm;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::ScaleRow (int row, Real scale, GMatrix<Real>& mat)
{
    for (int col = 0; col < mat.GetNumColumns(); ++col)
    {
        mat[row][col] *= scale;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::ScaleCol (int col, Real scale, GMatrix<Real>& mat)
{
    for (int row = 0; row < mat.GetNumRows(); ++row)
    {
        mat[row][col] *= scale;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::IsBalanced3 (GMatrix<Real>& mat)
{
    const Real tolerance = (Real)0.001;
    for (int i = 0; i < 3; ++i)
    {
        Real rowNorm = GetRowNorm(i, mat);
        Real colNorm = GetColNorm(i, mat);
        Real test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
        if (test > tolerance)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::Balance3 (GMatrix<Real>& mat)
{
    const int imax = 16;
    int i;
    for (i = 0; i < imax; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            Real rowNorm = GetRowNorm(j, mat);
            Real colNorm = GetColNorm(j, mat);
            Real scale = Math<Real>::Sqrt(colNorm/rowNorm);
            Real invScale = ((Real)1)/scale;
            ScaleRow(j, scale, mat);
            ScaleCol(j, invScale, mat);
        }

        if (IsBalanced3(mat))
        {
            break;
        }
    }
    assertion(i < imax, "Exceeded max iterations\n");
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::IsBalancedCompanion3 (Real a10, Real a21,
    Real a02, Real a12, Real a22)
{
    const Real tolerance = (Real)0.001;

    // row/col 0
    Real rowNorm = a02;
    Real colNorm = a10;
    Real test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
    if (test > tolerance)
    {
        return false;
    }

    // row/col 1
    rowNorm = (a10 >= a12 ? a10 : a12);
    colNorm = a21;
    test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
    if (test > tolerance)
    {
        return false;
    }

    // row/col 2
    rowNorm = (a21 >= a22 ? a21 : a22);
    colNorm = (a02 >= a12 ? a02 : a12);
    if (a22 > colNorm)
    {
        colNorm = a22;
    }
    test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
    return test <= tolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::BalanceCompanion3 (GMatrix<Real>& mat)
{
    Real a10 = Math<Real>::FAbs(mat[1][0]);
    Real a21 = Math<Real>::FAbs(mat[2][1]);
    Real a02 = Math<Real>::FAbs(mat[0][2]);
    Real a12 = Math<Real>::FAbs(mat[1][2]);
    Real a22 = Math<Real>::FAbs(mat[2][2]);
    Real rowNorm, colNorm, scale, invScale;

    const int imax = 16;
    int i;
    for (i = 0; i < imax; ++i)
    {
        // balance row/col 0
        rowNorm = a02;
        colNorm = a10;
        scale = Math<Real>::Sqrt(colNorm/rowNorm);
        a02 *= scale;
        a10 = a02;

        // balance row/col 1
        rowNorm = (a10 >= a12 ? a10 : a12);
        colNorm = a21;
        scale = Math<Real>::Sqrt(colNorm/rowNorm);
        invScale = ((Real)1)/scale;
        a10 *= scale;
        a12 *= scale;
        a21 *= invScale;

        // balance row/col 2
        rowNorm = (a21 >= a22 ? a21 : a22);
        colNorm = (a02 >= a12 ? a02 : a12);
        if (a22 > colNorm)
        {
            colNorm = a22;
        }
        scale = Math<Real>::Sqrt(colNorm/rowNorm);
        invScale = ((Real)1)/scale;
        a21 *= scale;
        a02 *= invScale;
        a12 *= invScale;

        if (IsBalancedCompanion3(a10, a21, a02, a12, a22))
        {
            break;
        }
    }
    assertion(i < imax, "Exceeded max iterations\n");

    mat[1][0] = (mat[1][0] >= (Real)0 ? a10 : -a10);
    mat[2][1] = (mat[2][1] >= (Real)0 ? a21 : -a21);
    mat[0][2] = (mat[0][2] >= (Real)0 ? a02 : -a02);
    mat[1][2] = (mat[1][2] >= (Real)0 ? a12 : -a12);
    mat[2][2] = (mat[2][2] >= (Real)0 ? a22 : -a22);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::QRIteration3 (GMatrix<Real>& mat)
{
    GVector<Real> W(3);
    Real rhs, trace, det;
    for (int i = 0; i < MaxIterations; ++i)
    {
        rhs = Epsilon*(Math<Real>::FAbs(mat[0][0]) +
            Math<Real>::FAbs(mat[1][1]));

        if (Math<Real>::FAbs(mat[1][0]) <= rhs)
        {
            // mat[0][0] is a root, solve the quadratic for the submatrix.
            trace = mat[1][1] + mat[2][2];
            det = mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1];
            FindA(det, -trace, (Real)1);
            mRoot[mCount++] = mat[0][0];
            return true;
        }

        rhs = Epsilon*(Math<Real>::FAbs(mat[1][1]) +
            Math<Real>::FAbs(mat[2][2]));

        if (Math<Real>::FAbs(mat[2][1]) <= rhs)
        {
            // mat[2][2] is a root, solve the quadratic for the submatrix.
            trace = mat[0][0] + mat[1][1];
            det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
            FindA(det, -trace, (Real)1);
            mRoot[mCount++] = mat[2][2];
            return true;
        }

        FrancisQRStep(mat, W);
    }

    // TO DO: In theory, cubic polynomials always have one real-valued root,
    // but if the maximum iterations were exceeded, what to do?  Some
    // experiments show that when the polynomial nearly has a double root,
    // the convergence of the algorithm is slow.  Maybe a random perturbation
    // to "kick" the system a bit might work?
    //
    // If you want to trap exceeding the maximum iterations, uncomment the
    // next 'assertion' line of code.
    //
    // assertion(false, "Exceeded max iterations\n");

    // For now, zero out the smallest subdiagonal entry to decouple the
    // matrix.
    if (Math<Real>::FAbs(mat[1][0]) <= Math<Real>::FAbs(mat[2][1]))
    {
        // mat[0][0] is a root, solve the quadratic for the submatrix
        trace = mat[1][1] + mat[2][2];
        det = mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1];
        FindA(det, -trace, (Real)1);
        mRoot[mCount++] = mat[0][0];
    }
    else
    {
        // mat[2][2] is a root, solve the quadratic for the submatrix
        trace = mat[0][0] + mat[1][1];
        det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
        FindA(det, -trace, (Real)1);
        mRoot[mCount++] = mat[2][2];
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// degree 4
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindA (Real c0, Real c1, Real c2, Real c3,
    Real c4)
{
    if (Math<Real>::FAbs(c4) <= Epsilon)
    {
        // Polynomial is cubic.
        return FindA(c0, c1, c2, c3);
    }

    // Make polynomial monic, x^4+c3*x^3+c2*x^2+c1*x+c0.
    Real invC4 = ((Real)1)/c4;
    c0 *= invC4;
    c1 *= invC4;
    c2 *= invC4;
    c3 *= invC4;

    // Reduction to resolvent cubic polynomial y^3+r2*y^2+r1*y+r0 = 0.
    Real r0 = -c3*c3*c0 + ((Real)4)*c2*c0 - c1*c1;
    Real r1 = c3*c1 - ((Real)4)*c0;
    Real r2 = -c2;
    FindA(r0, r1, r2, (Real)1);  // always produces at least one root
    Real y = mRoot[0];

    mCount = 0;
    Real discr = ((Real)0.25)*c3*c3 - c2 + y;
    if (Math<Real>::FAbs(discr) <= Epsilon)
    {
        discr = (Real)0;
    }

    if (discr > (Real)0) 
    {
        Real r = Math<Real>::Sqrt(discr);
        Real t1 = ((Real)0.75)*c3*c3 - r*r - ((Real)2)*c2;
        Real t2 = (((Real)4)*c3*c2 - ((Real)8)*c1 - c3*c3*c3) /
            (((Real)4.0)*r);

        Real tPlus = t1 + t2;
        Real tMinus = t1 - t2;
        if (Math<Real>::FAbs(tPlus) <= Epsilon) 
        {
            tPlus = (Real)0;
        }
        if (Math<Real>::FAbs(tMinus) <= Epsilon) 
        {
            tMinus = (Real)0;
        }

        if (tPlus >= (Real)0)
        {
            Real d = Math<Real>::Sqrt(tPlus);
            mRoot[0] = -((Real)0.25)*c3 + ((Real)0.5)*(r + d);
            mRoot[1] = -((Real)0.25)*c3 + ((Real)0.5)*(r - d);
            mCount += 2;
        }
        if (tMinus >= (Real)0)
        {
            Real e = Math<Real>::Sqrt(tMinus);
            mRoot[mCount++] = -((Real)0.25)*c3+((Real)0.5)*(e - r);
            mRoot[mCount++] = -((Real)0.25)*c3-((Real)0.5)*(e + r);
        }
    }
    else if (discr < (Real)0)
    {
        mCount = 0;
    }
    else
    {
        Real t2 = y*y - ((Real)4)*c0;
        if (t2 >= -Epsilon) 
        {
            if (t2 < (Real)0) // round to zero
            {
                t2 = (Real)0;
            }
            t2 = ((Real)2)*Math<Real>::Sqrt(t2);
            Real t1 = ((Real)0.75)*c3*c3 - ((Real)2)*c2;
            Real tPlus = t1 + t2;
            if (tPlus >= Epsilon)
            {
                Real d = Math<Real>::Sqrt(tPlus);
                mRoot[0] = -((Real)0.25)*c3 + ((Real)0.5)*d;
                mRoot[1] = -((Real)0.25)*c3 - ((Real)0.5)*d;
                mCount += 2;
            }
            Real tMinus = t1 - t2;
            if (tMinus >= Epsilon) 
            {
                Real e = Math<Real>::Sqrt(tMinus);
                mRoot[mCount++] = -((Real)0.25)*c3+((Real)0.5)*e;
                mRoot[mCount++] = -((Real)0.25)*c3-((Real)0.5)*e;
            }
        }
    }

    return mCount > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindE (Real c0, Real c1, Real c2, Real c3,
    Real c4, bool doBalancing)
{
    if (Math<Real>::FAbs(c4) <= Epsilon)
    {
        // Polynomial is cubic.
        return FindA(c0,c1,c2,c3);
    }

    // Make polynomial monic, x^4+c3*x^3+c2*x^2+c1*x+c0.
    Real invC4 = ((Real)1)/c4;
    c0 *= invC4;
    c1 *= invC4;
    c2 *= invC4;
    c3 *= invC4;

    // Construct the 4-by-4 companion matrix.
    GMatrix<Real> mat(4, 4);  // initialized to zero
    mat[1][0] = (Real)1;
    mat[2][1] = (Real)1;
    mat[3][2] = (Real)1;
    mat[0][3] = -c0;
    mat[1][3] = -c1;
    mat[2][3] = -c2;
    mat[3][3] = -c3;

    if (doBalancing)
    {
        BalanceCompanion4(mat);
    }

    return QRIteration4(mat);
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetBound (Real c0, Real c1, Real c2, Real c3,
    Real c4)
{
    if (Math<Real>::FAbs(c4) <= Epsilon)
    {
        // Polynomial is cubic.
        return GetBound(c0, c1, c2, c3);
    }

    Real invC4 = ((Real)1)/c4;
    Real maxValue = Math<Real>::FAbs(c0)*invC4;

    Real tmp = Math<Real>::FAbs(c1)*invC4;
    if (tmp > maxValue)
    {
        maxValue = tmp;
    }

    tmp = Math<Real>::FAbs(c2)*invC4;
    if (tmp > maxValue)
    {
        maxValue = tmp;
    }

    tmp = Math<Real>::FAbs(c3)*invC4;
    if (tmp > maxValue)
    {
        maxValue = tmp;
    }

    return (Real)1 + maxValue;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::IsBalancedCompanion4 (Real a10, Real a21,
    Real a32, Real a03, Real a13, Real a23, Real a33)
{
    const Real tolerance = (Real)0.001;

    // row/col 0
    Real rowNorm = a03;
    Real colNorm = a10;
    Real test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
    if (test > tolerance)
    {
        return false;
    }

    // row/col 1
    rowNorm = (a10 >= a13 ? a10 : a13);
    colNorm = a21;
    test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
    if (test > tolerance)
    {
        return false;
    }

    // row/col 2
    rowNorm = (a21 >= a23 ? a21 : a23);
    colNorm = a32;
    test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
    if (test > tolerance)
    {
        return false;
    }

    // row/col 3
    rowNorm = (a32 >= a33 ? a32 : a33);
    colNorm = (a03 >= a13 ? a03 : a13);
    if (a23 > colNorm)
    {
        colNorm = a23;
    }
    if (a33 > colNorm)
    {
        colNorm = a33;
    }
    test = Math<Real>::FAbs((Real)1 - colNorm/rowNorm);
    return test <= tolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::BalanceCompanion4 (GMatrix<Real>& mat)
{
    Real a10 = Math<Real>::FAbs(mat[1][0]);
    Real a21 = Math<Real>::FAbs(mat[2][1]);
    Real a32 = Math<Real>::FAbs(mat[3][2]);
    Real a03 = Math<Real>::FAbs(mat[0][3]);
    Real a13 = Math<Real>::FAbs(mat[1][3]);
    Real a23 = Math<Real>::FAbs(mat[2][3]);
    Real a33 = Math<Real>::FAbs(mat[3][3]);
    Real rowNorm, colNorm, scale, invScale;

    const int imax = 16;
    int i;
    for (i = 0; i < imax; ++i)
    {
        // balance row/col 0
        rowNorm = a03;
        colNorm = a10;
        scale = Math<Real>::Sqrt(colNorm/rowNorm);
        a03 *= scale;
        a10 = a03;

        // balance row/col 1
        rowNorm = (a10 >= a13 ? a10 : a13);
        colNorm = a21;
        scale = Math<Real>::Sqrt(colNorm/rowNorm);
        invScale = ((Real)1)/scale;
        a10 *= scale;
        a13 *= scale;
        a21 *= invScale;

        // balance row/col 2
        rowNorm = (a21 >= a23 ? a21 : a23);
        colNorm = a32;
        scale = Math<Real>::Sqrt(colNorm/rowNorm);
        invScale = ((Real)1)/scale;
        a21 *= scale;
        a23 *= scale;
        a32 *= invScale;

        // balance row/col 3
        rowNorm = (a32 >= a33 ? a32 : a33);
        colNorm = (a03 >= a13 ? a03 : a13);
        if (a23 > colNorm)
        {
            colNorm = a23;
        }
        if (a33 > colNorm)
        {
            colNorm = a33;
        }
        scale = Math<Real>::Sqrt(colNorm/rowNorm);
        invScale = ((Real)1)/scale;
        a32 *= scale;
        a03 *= invScale;
        a13 *= invScale;
        a23 *= invScale;

        if (IsBalancedCompanion4(a10, a21, a32, a03, a13, a23, a33))
        {
            break;
        }
    }
    assertion(i < imax, "Exceeded max iterations\n");

    mat[1][0] = (mat[1][0] >= (Real)0 ? a10 : -a10);
    mat[2][1] = (mat[2][1] >= (Real)0 ? a21 : -a21);
    mat[3][2] = (mat[3][2] >= (Real)0 ? a32 : -a32);
    mat[0][3] = (mat[0][3] >= (Real)0 ? a03 : -a03);
    mat[1][3] = (mat[1][3] >= (Real)0 ? a13 : -a13);
    mat[2][3] = (mat[2][3] >= (Real)0 ? a23 : -a23);
    mat[3][3] = (mat[3][3] >= (Real)0 ? a33 : -a33);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::QRIteration4 (GMatrix<Real>& mat)
{
    GVector<Real> W(4);
    GMatrix<Real> matMS(3, 3);
    Real rhs, trace, det, saveRoot[2];
    int i, j, saveCount;
    for (i = 0; i < MaxIterations; ++i)
    {
        rhs = Epsilon*(Math<Real>::FAbs(mat[0][0]) +
            Math<Real>::FAbs(mat[1][1]));

        if (Math<Real>::FAbs(mat[1][0]) <= rhs)
        {
            // mat[0][0] is a root, reduce the 3-by-3 submatrix
            // TO DO:  Avoid the copy and pass row/column offsets to the
            // FrancisQR method.
            matMS[0][0] = mat[1][1];
            matMS[0][1] = mat[1][2];
            matMS[0][2] = mat[1][3];
            matMS[1][0] = mat[2][1];
            matMS[1][1] = mat[2][2];
            matMS[1][2] = mat[2][3];
            matMS[2][0] = mat[3][1];
            matMS[2][1] = mat[3][2];
            matMS[2][2] = mat[3][3];
            QRIteration3(matMS);
            mRoot[mCount++] = mat[0][0];
            return true;
        }

        rhs = Epsilon*(Math<Real>::FAbs(mat[1][1]) +
            Math<Real>::FAbs(mat[2][2]));

        if (Math<Real>::FAbs(mat[2][1]) <= rhs)
        {
            // The matrix is decoupled into two 2-by-2 blocks.  Solve the
            // quadratics for the blocks.
            trace = mat[0][0] + mat[1][1];
            det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
            FindA(det, -trace, (Real)1);
            saveCount = mCount;
            for (j = 0; j < saveCount; ++j)
            {
                saveRoot[j] = mRoot[j];
            }

            trace = mat[2][2] + mat[3][3];
            det = mat[2][2]*mat[3][3] - mat[2][3]*mat[3][2];
            FindA(det, -trace, (Real)1);
            for (j = 0; j < saveCount; ++j)
            {
                mRoot[mCount++] = saveRoot[j];
            }
            return mCount > 0;
        }

        rhs = Epsilon*(Math<Real>::FAbs(mat[2][2]) +
            Math<Real>::FAbs(mat[3][3]));

        if (Math<Real>::FAbs(mat[3][2]) <= rhs)
        {
            // mat[3][3] is a root, reduce the 3-by-3 submatrix
            // TO DO:  Avoid the copy and pass row/column offsets to the
            // FrancisQR method.
            matMS[0][0] = mat[0][0];
            matMS[0][1] = mat[0][1];
            matMS[0][2] = mat[0][2];
            matMS[1][0] = mat[1][0];
            matMS[1][1] = mat[1][1];
            matMS[1][2] = mat[1][2];
            matMS[2][0] = mat[2][0];
            matMS[2][1] = mat[2][1];
            matMS[2][2] = mat[2][2];
            QRIteration3(matMS);
            mRoot[mCount++] = mat[3][3];
            return true;
        }

        FrancisQRStep(mat, W);
    }

    // TO DO:  What to do if the maximum iterations were exceeded?  Maybe a
    // random perturbation to "kick" the system a bit might work?
    //
    // If you want to trap exceeding the maximum iterations, uncomment the
    // next 'assertion' line of code.
    //
    // assertion(false, "Exceeded max iterations\n");

    // For now, decouple the matrix using the smallest subdiagonal entry.
    i = 0;
    Real minValue = Math<Real>::FAbs(mat[1][0]);
    Real absValue = Math<Real>::FAbs(mat[2][1]);
    if (absValue < minValue)
    {
        minValue = absValue;
        i = 1;
    }
    absValue = Math<Real>::FAbs(mat[3][2]);
    if (absValue < minValue)
    {
        minValue = absValue;
        i = 2;
    }

    if (i == 0)
    {
        // mat[0][0] is a root, reduce the 3-by-3 submatrix
        // TO DO:  Avoid the copy and pass row/column offsets to the
        // FrancisQR method.
        matMS[0][0] = mat[1][1];
        matMS[0][1] = mat[1][2];
        matMS[0][2] = mat[1][3];
        matMS[1][0] = mat[2][1];
        matMS[1][1] = mat[2][2];
        matMS[1][2] = mat[2][3];
        matMS[2][0] = mat[3][1];
        matMS[2][1] = mat[3][2];
        matMS[2][2] = mat[3][3];
        QRIteration3(matMS);
        mRoot[mCount++] = mat[0][0];
    }
    else if (i == 1)
    {
        // The matrix is decoupled into two 2-by-2 blocks.  Solve the
        // quadratics for the blocks.
        trace = mat[0][0] + mat[1][1];
        det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
        FindA(det, -trace, (Real)1);
        saveCount = mCount;
        for (j = 0; j < saveCount; ++j)
        {
            saveRoot[j] = mRoot[j];
        }

        trace = mat[2][2] + mat[3][3];
        det = mat[2][2]*mat[3][3] - mat[2][3]*mat[3][2];
        FindA(det, -trace, (Real)1);
        for (j = 0; j < saveCount; ++j)
        {
            mRoot[mCount++] = saveRoot[j];
        }
    }
    else  // i == 2
    {
        // mat[3][3] is a root, reduce the 3-by-3 submatrix
        // TO DO:  Avoid the copy and pass row/column offsets to the
        // FrancisQR method.
        matMS[0][0] = mat[0][0];
        matMS[0][1] = mat[0][1];
        matMS[0][2] = mat[0][2];
        matMS[1][0] = mat[1][0];
        matMS[1][1] = mat[1][1];
        matMS[1][2] = mat[1][2];
        matMS[2][0] = mat[2][0];
        matMS[2][1] = mat[2][1];
        matMS[2][2] = mat[2][2];
        QRIteration3(matMS);
        mRoot[mCount++] = mat[3][3];
    }

    return mCount > 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// degree N
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindB (const Polynomial1<Real>& poly,
    int digits)
{
    Real bound = GetBound(poly);
    return FindB(poly, -bound, bound, digits);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindN (const Polynomial1<Real>&, int)
{
    // TO DO:  Implement this.
    assertion(false, "Not yet implemented\n");
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindE (const Polynomial1<Real>&, bool)
{
    // TO DO:  Implement this.
    assertion(false, "Not yet implemented\n");
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialRoots<Real>::GetBound (const Polynomial1<Real>& poly)
{
    Polynomial1<Real> copyPoly = poly;
    copyPoly.Compress(Epsilon);
    int degree = copyPoly.GetDegree();
    if (degree < 1)
    {
        // Polynomial is constant, return invalid bound.
        return (Real)-1;
    }

    Real invCopyDeg = ((Real)1)/copyPoly[degree];
    Real maxValue = (Real)0;
    for (int i = 0; i < degree; ++i)
    {
        Real tmp = Math<Real>::FAbs(copyPoly[i])*invCopyDeg;
        if (tmp > maxValue)
        {
            maxValue = tmp;
        }
    }

    return (Real)1 + maxValue;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindB (const Polynomial1<Real>& poly,
    Real xMin, Real xMax, int digits)
{
    // Reallocate root array if necessary.
    if (poly.GetDegree() > mMaxRoot)
    {
        mMaxRoot = poly.GetDegree();
        delete1(mRoot);
        mRoot = new1<Real>(mMaxRoot);
    }

    Real root;
    if (poly.GetDegree() == 1)
    {
        if (Bisection(poly, xMin, xMax, digits, root))
        {
            mCount = 1;
            mRoot[0] = root;
            return true;
        }
        mCount = 0;
        return false;
    }

    // Get roots of derivative polynomial.
    Polynomial1<Real> deriv = poly.GetDerivative();
    FindB(deriv, xMin, xMax, digits);

    int i, newCount = 0;
    Real* newRoot = new1<Real>(mCount + 1);

    if (mCount > 0)
    {
        // Find root on [xmin,root[0]].
        if (Bisection(poly, xMin, mRoot[0], digits, root))
        {
            newRoot[newCount++] = root;
        }

        // Find root on [root[i],root[i+1]] for 0 <= i <= count-2.
        for (i = 0; i <= mCount-2; ++i)
        {
            if (Bisection(poly, mRoot[i], mRoot[i+1], digits, root))
            {
                newRoot[newCount++] = root;
            }
        }

        // Find root on [root[count-1],xmax].
        if (Bisection(poly, mRoot[mCount-1], xMax, digits, root))
        {
            newRoot[newCount++] = root;
        }
    }
    else
    {
        // Polynomial is monotone on [xmin,xmax], has at most one root.
        if (Bisection(poly, xMin, xMax, digits, root))
        {
            newRoot[newCount++] = root;
        }
    }

    // Copy to old buffer.
    if (newCount > 0)
    {
        mCount = 1;
        mRoot[0] = newRoot[0];
        for (i = 1; i < newCount; ++i)
        {
            Real rootDiff = newRoot[i] - newRoot[i-1];
            if (Math<Real>::FAbs(rootDiff) > Epsilon)
            {
                mRoot[mCount++] = newRoot[i];
            }
        }
    }
    else
    {
        mCount = 0;
    }

    delete1(newRoot);
    return mCount > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::FindN (const Polynomial1<Real>&, Real, Real,
    int)
{
    // TO DO:  Implement this.
    assertion(false, "Not yet implemented\n");
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::AllRealPartsNegative (
    const Polynomial1<Real>& poly)
{
    // Make a copy of coefficients, later calls will change the copy.
    int degree = poly.GetDegree();
    const Real* polyCoeff = (const Real*)poly;
    Real* coeff = new1<Real>(degree + 1);
    memcpy(coeff, polyCoeff, (degree + 1)*sizeof(Real));

    // Make polynomial monic.
    if (coeff[degree] != (Real)1)
    {
        Real inv = ((Real)1)/coeff[degree];
        for (int i = 0; i < degree; ++i)
        {
            coeff[i] *= inv;
        }
        coeff[degree] = (Real)1;
    }

    bool result = AllRealPartsNegative(degree, coeff);
    delete1(coeff);
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::AllRealPartsPositive (
    const Polynomial1<Real>& poly)
{
    // Make a copy of coefficients, later calls will change the copy.
    int degree = poly.GetDegree();
    const Real* polyCoeff = (const Real*)poly;
    Real* coeff = new1<Real>(degree + 1);
    memcpy(coeff, polyCoeff, (degree + 1)*sizeof(Real));

    // Make polynomial monic.
    int i;
    if (coeff[degree] != (Real)1)
    {
        Real inv = ((Real)1)/coeff[degree];
        for (i = 0; i < degree; ++i)
        {
            coeff[i] *= inv;
        }
        coeff[degree] = (Real)1;
    }

    // Reflect z -> -z.
    int sign = -1;
    for (i = degree - 1; i >= 0; --i, sign = -sign)
    {
        coeff[i] *= sign;
    }

    bool result = AllRealPartsNegative(degree, coeff);
    delete1(coeff);
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::AllRealPartsNegative (int degree, Real* coeff)
{
    // assert:  coeff[degree] = 1

    if (coeff[degree-1] <= (Real)0)
    {
        return false;
    }
    if (degree == 1)
    {
        return true;
    }

    Real* tmpCoeff = new1<Real>(degree);
    tmpCoeff[0] = ((Real)2)*coeff[0]*coeff[degree-1];
    int i;
    for (i = 1; i <= degree-2; ++i) 
    {
        tmpCoeff[i] = coeff[degree-1]*coeff[i];
        if (((degree-i) % 2) == 0)
        {
            tmpCoeff[i] -= coeff[i-1];
        }
        tmpCoeff[i] *= (Real)2;
    }
    tmpCoeff[degree-1] = ((Real)2)*coeff[degree-1]*coeff[degree-1];

    int nextDegree;
    for (nextDegree = degree-1; nextDegree >= 0; --nextDegree)
    {
        if (tmpCoeff[nextDegree] != (Real)0)
        {
            break;
        }
    }
    for (i = 0; i <= nextDegree-1; ++i)
    {
        coeff[i] = tmpCoeff[i]/tmpCoeff[nextDegree];
    }
    delete1(tmpCoeff);

    return AllRealPartsNegative(nextDegree, coeff);
}
//----------------------------------------------------------------------------
template <typename Real>
int PolynomialRoots<Real>::GetRootCount (const Polynomial1<Real>& poly,
    Real t0, Real t1)
{
    int degree = poly.GetDegree();
    const Real* coeff = (const Real*)poly;

    if (degree == 0)
    {
        // Polynomial is constant on the interval.
        if (coeff[0] != (Real)0)
        {
            return 0;
        }
        else
        {
            return -1;  // to indicate "infinitely many"
        }
    }

    // Generate the Sturm sequence.
    std::vector<Polynomial1<Real>*> sturm;
    Polynomial1<Real>* f0 = new0 Polynomial1<Real>(poly);
    Polynomial1<Real>* f1 = new0 Polynomial1<Real>(f0->GetDerivative());
    sturm.push_back(f0);
    sturm.push_back(f1);

    while (f1->GetDegree() > 0)
    {
        Polynomial1<Real>* f2 = new0 Polynomial1<Real>(-1);
        Polynomial1<Real> quot;
        f0->Divide(*f1, quot, *f2, Math<Real>::ZERO_TOLERANCE);
        *f2 = -(*f2);
        sturm.push_back(f2);
        f0 = f1;
        f1 = f2;
    }

    int i;
    Real value0, value1;

    // Count the sign changes at t0.
    int signChanges0 = 0;
    if (t0 == -Math<Real>::MAX_REAL)
    {
        f0 = sturm[0];
        if (f0->GetDegree() & 1)
        {
            value0 = -(*f0)[f0->GetDegree()];
        }
        else
        {
            value0 = (*f0)[f0->GetDegree()];
        }

        if (Math<Real>::FAbs(value0) < Epsilon)
        {
            value0 = (Real)0;
        }

        const int numSturm = (int)sturm.size();
        for (i = 1; i < numSturm; ++i)
        {
            f1 = sturm[i];

            if (f1->GetDegree() & 1)
            {
                value1 = -(*f1)[f1->GetDegree()];
            }
            else
            {
                value1 = (*f1)[f1->GetDegree()];
            }

            if (Math<Real>::FAbs(value1) < Epsilon)
            {
                value1 = (Real)0;
            }

            if (value0*value1 < (Real)0 || value0 == (Real)0)
            {
                signChanges0++;
            }

            value0 = value1;
            f0 = f1;
        }
    }
    else
    {
        f0 = sturm[0];
        value0 = (*f0)(t0);
        if (Math<Real>::FAbs(value0) < Epsilon)
        {
            value0 = (Real)0;
        }

        const int numSturm = (int)sturm.size();
        for (i = 1; i < numSturm; ++i)
        {
            f1 = sturm[i];
            value1 = (*f1)(t0);
            if (Math<Real>::FAbs(value1) < Epsilon)
            {
                value1 = (Real)0;
            }

            if (value0*value1 < (Real)0 || value0 == (Real)0)
            {
                signChanges0++;
            }

            value0 = value1;
            f0 = f1;
        }
    }

    // Count the sign changes at t1.
    int signChanges1 = 0;
    if (t1 == Math<Real>::MAX_REAL)
    {
        f0 = sturm[0];
        value0 = (*f0)[f0->GetDegree()];
        if (Math<Real>::FAbs(value0) < Epsilon)
        {
            value0 = (Real)0;
        }

        const int numSturm = (int)sturm.size();
        for (i = 1; i < numSturm; ++i)
        {
            f1 = sturm[i];
            value1 = (*f1)[f1->GetDegree()];
            if (Math<Real>::FAbs(value1) < Epsilon)
            {
                value1 = (Real)0;
            }

            if (value0*value1 < (Real)0 || value0 == (Real)0)
            {
                signChanges1++;
            }

            value0 = value1;
            f0 = f1;
        }
    }
    else
    {
        f0 = sturm[0];
        value0 = (*f0)(t1);
        if (Math<Real>::FAbs(value0) < Epsilon)
        {
            value0 = (Real)0;
        }

        const int numSturm = (int)sturm.size();
        for (i = 1; i < numSturm; ++i)
        {
            f1 = sturm[i];
            value1 = (*f1)(t1);
            if (Math<Real>::FAbs(value1) < Epsilon)
            {
                value1 = (Real)0;
            }

            if (value0*value1 < (Real)0 || value0 == (Real)0)
            {
                signChanges1++;
            }

            value0 = value1;
            f0 = f1;
        }
    }

    // Clean up.
    const int numSturm = (int)sturm.size();
    for (i = 0; i < numSturm; ++i)
    {
        delete0(sturm[i]);
    }

    if (signChanges0 >= signChanges1)
    {
        return signChanges0 - signChanges1;
    }

    // Theoretically we should not get here.
    assertion(false, "Unexpected condition\n");
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PolynomialRoots<Real>::Bisection (const Polynomial1<Real>& poly,
    Real xMin, Real xMax, int digits, Real& root)
{
    Real p0 = poly(xMin);
    if (Math<Real>::FAbs(p0) <= Math<Real>::ZERO_TOLERANCE)
    {
        root = xMin;
        return true;
    }
    Real p1 = poly(xMax);
    if (Math<Real>::FAbs(p1) <= Math<Real>::ZERO_TOLERANCE)
    {
        root = xMax;
        return true;
    }
    if (p0*p1 > (Real)0)
    {
        return false;
    }

    // Determine number of iterations to get 'digits' accuracy..
    Real tmp0 = Math<Real>::Log(xMax - xMin);
    Real tmp1 = ((Real)digits)*Math<Real>::Log((Real)10);
    Real arg = (tmp0 + tmp1)/Math<Real>::Log((Real)2);
    int maxIter = (int)(arg + (Real)0.5);

    for (int i = 0; i < maxIter; ++i)
    {
        root = ((Real)0.5)*(xMin + xMax);
        Real p = poly(root);
        Real product = p*p0;
        if (product < (Real)0)
        {
            xMax = root;
            p1 = p;
        }
        else if (product > (Real)0)
        {
            xMin = root;
            p0 = p;
        }
        else
        {
            break;
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// FindE support
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::GetHouseholderVector (int size,
    const Vector3<Real>& U, Vector3<Real>& V)
{
    // Householder vector V:
    // Given a vector U, compute a vector V such that V[0] = 1 and
    // (I-2*V*V^T/|V|^2)*U is zero in all but the first component.  The
    // matrix P = I-2*V*V^T/|V|^2 is a Householder transformation, a
    // reflection matrix.

    Real length = U[0]*U[0];
    int i;
    for (i = 1; i < size; ++i)
    {
        length += U[i]*U[i];
    }
    length = Math<Real>::Sqrt(length);

    if (length > Epsilon)
    {
        Real inv = ((Real)1)/(U[0] + Math<Real>::Sign(U[0])*length);
        V[0] = (Real)1;
        for (i = 1; i < size; ++i)
        {
            V[i] = inv*U[i];
        }
    }
    else
    {
        // U is the zero vector, any vector will do.
        V[0] = (Real)1;
        for (i = 1; i < size; ++i)
        {
            V[i] = (Real)0;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::PremultiplyHouseholder (GMatrix<Real>& mat,
    GVector<Real>& W, int rMin, int rMax, int cMin, int cMax,
    int vSize, const Vector3<Real>& V)
{
    // Householder premultiplication:
    // Given a matrix A and an m-by-1 vector V with V[0] = 1, let S be the
    // submatrix of A with m rows rmin <= r <= m+rmin-1 and columns
    // cmin <= c <= cmax.  Overwrite S with P*S where P = I-2*V*V^T/|V|^2.

    int subRows = rMax - rMin + 1;
    int subCols = cMax - cMin + 1;
    int row, col;

    Real sqrLen = V[0]*V[0];
    for (int i = 1; i < vSize; ++i)
    {
        sqrLen += V[i]*V[i];
    }

    Real beta = -((Real)2)/sqrLen;
    for (col = 0; col < subCols; ++col)
    {
        W[col] = (Real)0;
        for (row = 0; row < subRows; ++row)
        {
            W[col] += V[row]*mat[rMin + row][cMin + col];
        }
        W[col] *= beta;
    }

    for (row = 0; row < subRows; ++row)
    {
        for (col = 0; col < subCols; ++col)
        {
            mat[rMin + row][cMin + col] += V[row]*W[col];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::PostmultiplyHouseholder (GMatrix<Real>& mat,
    GVector<Real>& W, int rMin, int rMax, int cMin, int cMax,
    int vSize, const Vector3<Real>& V)
{
    // Householder postmultiplication:
    // Given a matrix A and an n-by-1 vector V with V[0] = 1, let S be the
    // submatrix of A with n columns cmin <= c <= n+cmin-1 and rows
    // rmin <= r <= rmax.  Overwrite S with S*P where P = I-2*V*V^T/|V|^2.

    int subRows = rMax - rMin + 1;
    int subCols = cMax - cMin + 1;
    int row, col;

    Real sqrLen = V[0]*V[0];
    for (int i = 1; i < vSize; ++i)
    {
        sqrLen += V[i]*V[i];
    }

    Real beta = -((Real)2)/sqrLen;
    for (row = 0; row < subRows; ++row)
    {
        W[row] = (Real)0;
        for (col = 0; col < subCols; ++col)
        {
            W[row] += mat[rMin+row][cMin+col]*V[col];
        }
        W[row] *= beta;
    }

    for (row = 0; row < subRows; ++row)
    {
        for (col = 0; col < subCols; ++col)
        {
            mat[rMin + row][cMin + col] += W[row]*V[col];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialRoots<Real>::FrancisQRStep (GMatrix<Real>& H,
    GVector<Real>& W)
{
    // Given an n-by-n unreduced upper Hessenberg matrix H whose trailing
    // 2-by-2 principal submatrix has eigenvalues a1 and a2, overwrite H
    // with Z^T*H*Z where Z = P(1)*...*P(n-2) is a product of Householder
    // matrices and Z^T*(H-a1*I)*(H-a2*I) is upper triangular.

    // assert:  H is unreduced upper Hessenberg and n >= 3

    // Compute first column of (H-a1*I)*(H-a2*I).
    int n = H.GetNumRows();
    Real trace = H[n-2][n-2] + H[n-1][n-1];
    Real det = H[n-2][n-2]*H[n-1][n-1] - H[n-2][n-1]*H[n-1][n-2];
    Vector3<Real> U;
    U[0] = H[0][0]*H[1][1] + H[0][1]*H[1][0] - trace*H[0][0] + det;
    U[1] = H[1][0]*(H[0][0] + H[1][1] - trace);
    U[2] = H[1][0]*H[2][1];

    // Overwrite H with P(0)*H*P(0)^T.
    Vector3<Real> V;
    GetHouseholderVector(3, U, V);
    PremultiplyHouseholder(H, W, 0, 2, 0, n-1, 3, V);
    PostmultiplyHouseholder(H, W, 0, n-1, 0, 2, 3, V);

    for (int i = 1; i <= n-3; ++i)
    {
        // Overwrite H with P(i)*H*P(i)^T.
        U[0] = H[i  ][i-1];
        U[1] = H[i+1][i-1];
        U[2] = H[i+2][i-1];
        GetHouseholderVector(3, U, V);

        // The column range does not need to be 0 to n-1 because of the
        // pattern of zeros that occur in matrix H.
        PremultiplyHouseholder(H, W, i, i+2, i-1, n-1, 3, V);

        // The row range does not need to be 0 to n-1 because of the pattern
        // of zeros that occur in matrix H.
        int rMax = i + 3;
        if (rMax >= n)
        {
            rMax = n-1;
        }
        PostmultiplyHouseholder(H, W, 0, rMax, i, i+2, 3, V);
    }

    // Overwrite H with P(n-2)*H*P(n-2)^T.
    U[0] = H[n-2][n-3];
    U[1] = H[n-1][n-3];
    GetHouseholderVector(2, U, V);

    // The column range does not need to be 0 to n-1 because of the pattern
    // of zeros that occur in matrix H.
    PremultiplyHouseholder(H, W, n-2, n-1, n-3, n-1, 2, V);

    PostmultiplyHouseholder(H, W, 0, n-1, n-2, n-1, 2, V);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class PolynomialRoots<float>;

template WM5_MATHEMATICS_ITEM
class PolynomialRoots<double>;
//----------------------------------------------------------------------------
}
