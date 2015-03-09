// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5NaturalSpline1.h"
#include "Wm5LinearSystem.h"
#include "Wm5Matrix4.h"
#include "Wm5Polynomial1.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
NaturalSpline1<Real>::NaturalSpline1 (bool free, int numSamples,
    Real* times, Real* values)
    :
    mNumSamples(numSamples),
    mNumSegments(numSamples - 1),
    mTimes(times),
    mA(values)
{
    assertion(numSamples >= 2 && times && values, "Invalid input\n");
    if (free)
    {
        CreateFreeSpline();
    }
    else
    {
        CreatePeriodicSpline();
    }
}
//----------------------------------------------------------------------------
template <typename Real>
NaturalSpline1<Real>::NaturalSpline1 (int numSamples, Real* times,
    Real* values, Real slopeFirst, Real slopeLast)
    :
    mNumSamples(numSamples),
    mNumSegments(numSamples - 1),
    mTimes(times),
    mA(values)
{
    assertion(numSamples >= 2 && times && values, "Invalid input\n");
    CreateClampedSpline(slopeFirst, slopeLast);
}
//----------------------------------------------------------------------------
template <typename Real>
NaturalSpline1<Real>::~NaturalSpline1 ()
{
    delete1(mTimes);
    delete1(mA);
    delete1(mB);
    delete1(mC);
    delete1(mD);
}
//----------------------------------------------------------------------------
template <typename Real>
Real NaturalSpline1<Real>::GetFunction (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    return mA[key] + dt*(mB[key] + dt*(mC[key] + dt*mD[key]));
}
//----------------------------------------------------------------------------
template <typename Real>
Real NaturalSpline1<Real>::GetFirstDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t,key,dt);
    return mB[key] + dt*(mC[key]*((Real)2) + mD[key]*(((Real)3)*dt));
}
//----------------------------------------------------------------------------
template <typename Real>
Real NaturalSpline1<Real>::GetSecondDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t,key,dt);
    return mC[key]*((Real)2) + mD[key]*(((Real)6)*dt);
}
//----------------------------------------------------------------------------
template <typename Real>
Real NaturalSpline1<Real>::GetThirdDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t,key,dt);
    return ((Real)6)*mD[key];
}
//----------------------------------------------------------------------------
template <typename Real>
int NaturalSpline1<Real>::GetNumSegments () const
{
    return mNumSegments;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* NaturalSpline1<Real>::GetA () const
{
    return mA;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* NaturalSpline1<Real>::GetB () const
{
    return mB;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* NaturalSpline1<Real>::GetC () const
{
    return mC;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* NaturalSpline1<Real>::GetD () const
{
    return mD;
}
//----------------------------------------------------------------------------
template <typename Real>
void NaturalSpline1<Real>::CreateFreeSpline ()
{
    mB = new1<Real>(mNumSegments);
    mC = new1<Real>(mNumSegments + 1);
    mD = new1<Real>(mNumSegments);

    Real* delta = new1<Real>(mNumSamples);
    Real* invDelta = new1<Real>(mNumSamples);
    Real* fDeriv = new1<Real>(mNumSamples);
    int i;
    for (i = 0; i < mNumSamples; ++i)
    {
        delta[i] = mTimes[i+1] - mTimes[i];
        invDelta[i] = ((Real)1)/delta[i];
        fDeriv[i] = (mA[i+1] - mA[i])*invDelta[i];
    }

    const int numSegmentsM1 = mNumSegments - 1;
    Real* diagonal = new1<Real>(numSegmentsM1);
    Real* rhs = new1<Real>(numSegmentsM1);
    for (i = 0; i < numSegmentsM1; ++i)
    {
        diagonal[i] = ((Real)2)*(delta[i+1] + delta[i]);
        rhs[i] = ((Real)3)*(fDeriv[i+1] - fDeriv[i]);
    }

    // The boundary conditions.
    mC[0] = (Real)0;
    mC[mNumSegments] = (Real)0;

    // The linear system that determines C[1] through C[numSegs-1].
    bool solved = LinearSystem<Real>().SolveTri(numSegmentsM1, &delta[1],
        diagonal, &delta[1], rhs, &mC[1]);
    assertion(solved, "Failed to solve linear system\n");
    WM5_UNUSED(solved);

    const Real oneThird = ((Real)1)/(Real)3;
    for (i = 0; i < mNumSegments; ++i)
    {
        mB[i] = fDeriv[i] - delta[i]*oneThird*(mC[i+1] + ((Real)2)*mC[i]);
        mD[i] = oneThird*(mC[i+1] - mC[i])*invDelta[i];
    }

    delete1(delta);
    delete1(invDelta);
    delete1(fDeriv);
    delete1(diagonal);
    delete1(rhs);
}
//----------------------------------------------------------------------------
template <typename Real>
void NaturalSpline1<Real>::CreateClampedSpline (Real slopeFirst,
    Real slopeLast)
{
    mB = new1<Real>(mNumSegments + 1);
    mC = new1<Real>(mNumSegments);
    mD = new1<Real>(mNumSegments);

    Real* delta = new1<Real>(mNumSamples);
    Real* invDelta = new1<Real>(mNumSamples);
    Real* fDeriv = new1<Real>(mNumSamples);
    int i;
    for (i = 0; i < mNumSamples; ++i)
    {
        delta[i] = mTimes[i+1] - mTimes[i];
        invDelta[i] = ((Real)1)/delta[i];
        fDeriv[i] = (mA[i+1] - mA[i])*invDelta[i];
    }

    const int numSegmentsM1 = mNumSegments - 1;
    Real* diagonal = new1<Real>(numSegmentsM1);
    Real* rhs = new1<Real>(numSegmentsM1);
    for (i = 0; i < numSegmentsM1; ++i)
    {
        diagonal[i] = ((Real)2)*(delta[i+1] + delta[i]);
        rhs[i] = ((Real)3)*(delta[i]*fDeriv[i+1] + delta[i+1]*fDeriv[i]);
    }
    rhs[0] -= slopeFirst*delta[1];
    rhs[mNumSegments-2] -= slopeLast*delta[mNumSegments-2];

    // The boundary conditions.
    mB[0] = slopeFirst;
    mB[mNumSegments] = slopeLast;

    // The linear system that determines B[1] through B[numSegs-1].
    bool solved = LinearSystem<Real>().SolveTri(numSegmentsM1,&delta[2],
        diagonal,delta,rhs,&mB[1]);
    assertion(solved, "Failed to solve linear system\n");
    WM5_UNUSED(solved);

    const Real oneThird = ((Real)1)/(Real)3;
    for (i = 0; i < mNumSegments; ++i)
    {
        mC[i] = (((Real)3)*fDeriv[i] - mB[i+1] - ((Real)2)*mB[i])*invDelta[i];
        mD[i] = oneThird*(mB[i+1] - mB[i] -
            ((Real)2)*delta[i]*mC[i])*invDelta[i]*invDelta[i];
    }

    delete1(delta);
    delete1(invDelta);
    delete1(fDeriv);
    delete1(diagonal);
    delete1(rhs);
}
//----------------------------------------------------------------------------
template <typename Real>
void NaturalSpline1<Real>::CreatePeriodicSpline ()
{
    mB = new1<Real>(mNumSegments);
    mC = new1<Real>(mNumSegments);
    mD = new1<Real>(mNumSegments);

#if 1
    // Solving the system using a standard linear solver appears to be
    // numerically stable.
    const int size = 4*mNumSegments;
    GMatrix<Real> mat(size,size);
    GVector<Real> rhs(size);
    int i, j, k;
    Real delta, delta2, delta3;
    for (i = 0, j = 0; i < mNumSegments-1; ++i, j += 4)
    {
        delta = mTimes[i+1] - mTimes[i];
        delta2 = delta*delta;
        delta3 = delta*delta2;

        mat[j+0][j+0] = (Real)1;
        mat[j+0][j+1] = (Real)0;
        mat[j+0][j+2] = (Real)0;
        mat[j+0][j+3] = (Real)0;
        mat[j+1][j+0] = (Real)1;
        mat[j+1][j+1] = delta;
        mat[j+1][j+2] = delta2;
        mat[j+1][j+3] = delta3;
        mat[j+2][j+0] = (Real)0;
        mat[j+2][j+1] = (Real)1;
        mat[j+2][j+2] = ((Real)2)*delta;
        mat[j+2][j+3] = ((Real)3)*delta2;
        mat[j+3][j+0] = (Real)0;
        mat[j+3][j+1] = (Real)0;
        mat[j+3][j+2] = (Real)1;
        mat[j+3][j+3] = ((Real)3)*delta;

        k = j + 4;
        mat[j+0][k+0] = (Real)0;
        mat[j+0][k+1] = (Real)0;
        mat[j+0][k+2] = (Real)0;
        mat[j+0][k+3] = (Real)0;
        mat[j+1][k+0] = (Real)-1;
        mat[j+1][k+1] = (Real)0;
        mat[j+1][k+2] = (Real)0;
        mat[j+1][k+3] = (Real)0;
        mat[j+2][k+0] = (Real)0;
        mat[j+2][k+1] = (Real)-1;
        mat[j+2][k+2] = (Real)0;
        mat[j+2][k+3] = (Real)0;
        mat[j+3][k+0] = (Real)0;
        mat[j+3][k+1] = (Real)0;
        mat[j+3][k+2] = (Real)-1;
        mat[j+3][k+3] = (Real)0;
    }

    delta = mTimes[i+1] - mTimes[i];
    delta2 = delta*delta;
    delta3 = delta*delta2;

    mat[j+0][j+0] = (Real)1;
    mat[j+0][j+1] = (Real)0;
    mat[j+0][j+2] = (Real)0;
    mat[j+0][j+3] = (Real)0;
    mat[j+1][j+0] = (Real)1;
    mat[j+1][j+1] = delta;
    mat[j+1][j+2] = delta2;
    mat[j+1][j+3] = delta3;
    mat[j+2][j+0] = (Real)0;
    mat[j+2][j+1] = (Real)1;
    mat[j+2][j+2] = ((Real)2)*delta;
    mat[j+2][j+3] = ((Real)3)*delta2;
    mat[j+3][j+0] = (Real)0;
    mat[j+3][j+1] = (Real)0;
    mat[j+3][j+2] = (Real)1;
    mat[j+3][j+3] = ((Real)3)*delta;

    k = 0;
    mat[j+0][k+0] = (Real)0;
    mat[j+0][k+1] = (Real)0;
    mat[j+0][k+2] = (Real)0;
    mat[j+0][k+3] = (Real)0;
    mat[j+1][k+0] = (Real)-1;
    mat[j+1][k+1] = (Real)0;
    mat[j+1][k+2] = (Real)0;
    mat[j+1][k+3] = (Real)0;
    mat[j+2][k+0] = (Real)0;
    mat[j+2][k+1] = (Real)-1;
    mat[j+2][k+2] = (Real)0;
    mat[j+2][k+3] = (Real)0;
    mat[j+3][k+0] = (Real)0;
    mat[j+3][k+1] = (Real)0;
    mat[j+3][k+2] = (Real)-1;
    mat[j+3][k+3] = (Real)0;

    for (i = 0, j = 0; i < mNumSegments; ++i, j += 4)
    {
        rhs[j+0] = mA[i];
        rhs[j+1] = (Real)0;
        rhs[j+2] = (Real)0;
        rhs[j+3] = (Real)0;
    }

    GVector<Real> coeff(size);
    bool solved = LinearSystem<Real>().Solve(mat, rhs, coeff);
    assertion(solved, "Failed to solve linear system\n");
    WM5_UNUSED(solved);

    for (i = 0, j = 0; i < mNumSegments; ++i)
    {
        j++;
        mB[i] = coeff[j++];
        mC[i] = coeff[j++];
        mD[i] = coeff[j++];
    }
#endif

#if 0
    // Solving the system using the equations derived in the PDF
    // "Fitting a Natural Spline to Samples of the Form (t,f(t))"
    // is ill-conditioned.  TODO: Find a way to row-reduce the matrix of the
    // PDF in a numerically stable manner yet retaining the O(n) asymptotic
    // behavior.

    // Compute the inverses M[i]^{-1}.
    const int numSegmentsM1 = mNumSegments - 1;
    Matrix4<Real>* invM = new1<Matrix4<Real> >(numSegmentsM1);

    Real delta;
    int i;
    for (i = 0; i < numSegmentsM1; i++)
    {
        delta = mTimes[i+1] - mTimes[i];
        Real invDelta1 = ((Real)1)/delta;
        Real invDelta2 = invDelta1/delta;
        Real invDelta3 = invDelta2/delta;

        Matrix4<Real>& invMi = invM[i];
        invMi[0][0] = (Real)1;
        invMi[0][1] = (Real)0;
        invMi[0][2] = (Real)0;
        invMi[0][3] = (Real)0;
        invMi[1][0] = ((Real)(-3))*invDelta1;
        invMi[1][1] = ((Real)3)*invDelta1;
        invMi[1][2] = (Real)(-2);
        invMi[1][3] = delta;
        invMi[2][0] = ((Real)3)*invDelta2;
        invMi[2][1] = ((Real)(-3))*invDelta2;
        invMi[2][2] = ((Real)3)*invDelta1;
        invMi[2][3] = (Real)(-2);
        invMi[3][0] = -invDelta3;
        invMi[3][1] = invDelta3;
        invMi[3][2] = -invDelta2;
        invMi[3][3] = invDelta1;
    }

    // Matrix M[n-1].
    delta = mTimes[i+1] - mTimes[i];
    Real delta2 = delta*delta;
    Real delta3 = delta2*delta;
    Matrix4<Real> lastM
    (
        (Real)1, (Real)0, (Real)0, (Real)0,
        (Real)1, delta, delta2, delta3,
        (Real)0, (Real)1, ((Real)2)*delta, ((Real)3)*delta2,
        (Real)0, (Real)0, (Real)1, ((Real)3)*delta
    );

    // Matrix L.
    Matrix4<Real> LMat
    (
        (Real)0, (Real)0, (Real)0, (Real)0,
        (Real)1, (Real)0, (Real)0, (Real)0,
        (Real)0, (Real)1, (Real)0, (Real)0,
        (Real)0, (Real)0, (Real)1, (Real)0
    );

    // Vector U.
    Vector<4,Real> U((Real)1,(Real)0,(Real)0,(Real)0);

    // Initialize P = L and Q = f[n-2]*U.
    Matrix4<Real> P = LMat;

    const int numSegmentsM2 = mNumSegments - 2;
    Vector<4,Real> Q = mA[numSegmentsM2]*U;

    // Compute P and Q.
    for (i = numSegmentsM2; i >= 0; --i)
    {
        // Matrix L*M[i]^{-1}.
        Matrix4<Real> LMInv = LMat*invM[i];

        // Update P.
        P = LMInv*P;

        // Update Q.
        if (i > 0)
        {
            Q = mA[i-1]*U + LMInv*Q;
        }
        else
        {
            Q = mA[numSegmentsM1]*U + LMInv*Q;
        }
    }

    // Final update of P.
    P = lastM - P;

    // Compute P^{-1}.
    Matrix4<Real> invP = P.Inverse();

    // Compute K[n-1].
    Vector<4,Real> coeff = invP*Q;
    mB[numSegmentsM1] = coeff[1];
    mC[numSegmentsM1] = coeff[2];
    mD[numSegmentsM1] = coeff[3];

    // Back substitution for the other K[i].
    for (i = numSegmentsM2; i >= 0; i--)
    {
        coeff = invM[i]*(mA[i]*U + LMat*coeff);
        mB[i] = coeff[1];
        mC[i] = coeff[2];
        mD[i] = coeff[3];
    }

    delete1(invM);
#endif
}
//----------------------------------------------------------------------------
template <typename Real>
void NaturalSpline1<Real>::GetKeyInfo (Real t, int& key, Real& dt) const
{
    if (t <= mTimes[0])
    {
        key = 0;
        dt = (Real)0;
    }
    else if (t >= mTimes[mNumSegments])
    {
        key = mNumSegments - 1;
        dt = mTimes[mNumSegments] - mTimes[key];
    }
    else
    {
        for (int i = 0; i < mNumSegments; ++i)
        {
            if (t < mTimes[i+1])
            {
                key = i;
                dt = t - mTimes[i];
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class NaturalSpline1<float>;

template WM5_MATHEMATICS_ITEM
class NaturalSpline1<double>;
//----------------------------------------------------------------------------
}
