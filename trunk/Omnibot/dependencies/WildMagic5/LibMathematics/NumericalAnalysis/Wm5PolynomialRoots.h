// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POLYNOMIALROOTS_H
#define WM5POLYNOMIALROOTS_H

#include "Wm5MathematicsLIB.h"
#include "Wm5GMatrix.h"
#include "Wm5Vector3.h"
#include "Wm5Polynomial1.h"

namespace Wm5
{

// Methods are
//
// A: algebraic using closed-form expressions (fast, typically not robust)
// B: bisection (after root bounding, slow and robust)
// N: Newton's/bisection hybrid (after root bounding, medium and robust)
// E: eigenvalues of a companion matrix (fast and robust)

// Root bounds:
//
// For a monic polynomial
//   x^n + a[n-1]*x^{n-1} + ... + a[1]*x + a[0]
// the Cauchy bound is
//   M = 1 + max{|a[0]|,...,|a[n-1]|}.
// All real-value roots must lie in the interval [-M,M].  For a non-monic
// polynomial,
//   b[n]*x^n + b[n-1]*x^{n-1} + ... + b[1]*x + b[0],
// if b[n] is not zero, divide through by it and calculate Cauchy's
// bound:
//   1 + max{|b[0]/b[n]|,...,|b[n-1]/b[n]|}.

template <typename Real>
class WM5_MATHEMATICS_ITEM PolynomialRoots
{
public:
    // Construction and destruction.
    PolynomialRoots (Real epsilon);
    ~PolynomialRoots ();

    // Member access.
    int GetCount () const;
    const Real* GetRoots () const;
    Real GetRoot (int i) const;
    Real Epsilon;

    // For FindE functions, default is 128.
    int MaxIterations;

    // Linear equations:  c1*x+c0 = 0
    bool FindA (Real c0, Real c1);
    Real GetBound (Real c0, Real c1);

    // Quadratic equations: c2*x^2+c1*x+c0 = 0
    bool FindA (Real c0, Real c1, Real c2);
    Real GetBound (Real c0, Real c1, Real c2);

    // Cubic equations: c3*x^3+c2*x^2+c1*x+c0 = 0
    bool FindA (Real c0, Real c1, Real c2, Real c3);
    bool FindE (Real c0, Real c1, Real c2, Real c3, bool doBalancing);
    Real GetBound (Real c0, Real c1, Real c2, Real c3);

    // Solve A*r^3 + B*r = C where A > 0 and B > 0.  This equation always has
    // exactly one real-valued root.
    Real SpecialCubic (Real a, Real b, Real c);

    // Quartic equations: c4*x^4+c3*x^3+c2*x^2+c1*x+c0 = 0
    bool FindA (Real c0, Real c1, Real c2, Real c3, Real c4);
    bool FindE (Real c0, Real c1, Real c2, Real c3, Real c4,
        bool doBalancing);
    Real GetBound (Real c0, Real c1, Real c2, Real c3, Real c4);

    // General equations: sum_{i=0}^{degree} c(i)*x^i = 0
    bool FindB (const Polynomial1<Real>& poly, int digits);
    bool FindN (const Polynomial1<Real>& poly, int digits);
    bool FindE (const Polynomial1<Real>& poly, bool doBalancing);
    Real GetBound (const Polynomial1<Real>& poly);

    // Find roots on specified intervals.
    bool FindB (const Polynomial1<Real>& poly, Real xMin, Real xMax,
        int digits);

    bool FindN (const Polynomial1<Real>& poly, Real xMin, Real xMax,
        int digits);

    bool AllRealPartsNegative (const Polynomial1<Real>& poly);
    bool AllRealPartsPositive (const Polynomial1<Real>& poly);

    // Count the number of roots on [t0,t1].  Uses Sturm sequences to do the
    // counting.  It is allowed to pass in t0 = -Math<Real>::MAX_REAL or
    // t1 = Math<Real>::MAX_REAL.  The value of mEpsilon is used as a
    // threshold on the value of a Sturm polynomial at an end point.  If
    // smaller, that value is assumed to be zero.  The return value is the
    // number of roots.  If there are infinitely many, -1 is returned.
    int GetRootCount (const Polynomial1<Real>& poly, Real t0, Real t1);

private:
    // Support for FindB.
    bool Bisection (const Polynomial1<Real>& poly, Real xMin, Real xMax,
        int digitsAccuracy, Real& root);

    // Support for FindE.
    void GetHouseholderVector (int size, const Vector3<Real>& U,
        Vector3<Real>& V);

    void PremultiplyHouseholder (GMatrix<Real>& mat, GVector<Real>& W,
        int rMin, int rMax, int cMin, int cMax, int vSize,
        const Vector3<Real>& V);

    void PostmultiplyHouseholder (GMatrix<Real>& mat, GVector<Real>& W,
        int rMin, int rMax, int cMin, int cMax, int vSize,
        const Vector3<Real>& V);

    void FrancisQRStep (GMatrix<Real>& H, GVector<Real>& W);

    Real GetRowNorm (int row, GMatrix<Real>& mat);
    Real GetColNorm (int col, GMatrix<Real>& mat);
    void ScaleRow (int row, Real scale, GMatrix<Real>& mat);
    void ScaleCol (int col, Real scale, GMatrix<Real>& mat);
    void Balance3 (GMatrix<Real>& mat);
    bool IsBalanced3 (GMatrix<Real>& mat);
    void BalanceCompanion3 (GMatrix<Real>& mat);
    bool IsBalancedCompanion3 (Real a10, Real a21, Real a02, Real a12,
        Real a22);
    bool QRIteration3 (GMatrix<Real>& mat);

    void BalanceCompanion4 (GMatrix<Real>& mat);
    bool IsBalancedCompanion4 (Real a10, Real a21, Real a32, Real a03,
        Real a13, Real a23, Real a33);
    bool QRIteration4 (GMatrix<Real>& mat);

    // Support for testing if all roots have negative real parts.
    bool AllRealPartsNegative (int degree, Real* coeff);

    int mCount, mMaxRoot;
    Real* mRoot;
};

typedef PolynomialRoots<float> PolynomialRootsf;
typedef PolynomialRoots<double> PolynomialRootsd;

}

#endif
