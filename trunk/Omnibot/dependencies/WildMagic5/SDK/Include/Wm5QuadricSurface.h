// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5QUADRICSURFACE_H
#define WM5QUADRICSURFACE_H

#include "Wm5MathematicsLIB.h"
#include "Wm5ImplicitSurface.h"
#include "Wm5RVector3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM QuadricSurface : public ImplicitSurface<Real>
{
public:
    // A quadric surface is defined implicitly by
    //
    //   0 = a0 + a1*x[0] + a2*x[1] + a3*x[2] + a4*x[0]^2 + a5*x[0]*x[1] +
    //       a6*x[0]*x[2] + a7*x[1]^2 + a8*x[1]*x[2] + a9*x[2]^2
    //
    //     = a0 + [a1 a2 a3]*X + X^T*[a4   a5/2 a6/2]*X
    //                               [a5/2 a7   a8/2]
    //                               [a6/2 a8/2 a9  ]
    //     = C + B^T*X + X^T*A*X
    //
    // The matrix A is symmetric.

    QuadricSurface ();  // all coefficients zero
    QuadricSurface (const Real coeff[10]);

    // Member access.
    const Real* GetCoefficients () const;
    const Matrix3<Real>& GetA () const;
    const Vector3<Real>& GetB () const;
    Real GetC () const;

    // The quadric function.
    virtual Real F (const Vector3<Real>& pos) const;

    // First-order partial derivatives.
    virtual Real FX (const Vector3<Real>& pos) const;
    virtual Real FY (const Vector3<Real>& pos) const;
    virtual Real FZ (const Vector3<Real>& pos) const;

    // Second-order partial derivatives.
    virtual Real FXX (const Vector3<Real>& pos) const;
    virtual Real FXY (const Vector3<Real>& pos) const;
    virtual Real FXZ (const Vector3<Real>& pos) const;
    virtual Real FYY (const Vector3<Real>& pos) const;
    virtual Real FYZ (const Vector3<Real>& pos) const;
    virtual Real FZZ (const Vector3<Real>& pos) const;

    enum  // solution type
    {
        QT_NONE,
        QT_POINT,
        QT_LINE,
        QT_PLANE,
        QT_TWO_PLANES,
        QT_PARABOLIC_CYLINDER,
        QT_ELLIPTIC_CYLINDER,
        QT_HYPERBOLIC_CYLINDER,
        QT_ELLIPTIC_PARABOLOID,
        QT_HYPERBOLIC_PARABOLOID,
        QT_ELLIPTIC_CONE,
        QT_HYPERBOLOID_ONE_SHEET,
        QT_HYPERBOLOID_TWO_SHEETS,
        QT_ELLIPSOID
    };

    // Classification of the equation using exact arithmetic.
    int GetType () const;

protected:
    Real mCoeff[10];
    Matrix3<Real> mA;
    Vector3<Real> mB;
    Real mC;

private:
    typedef Rational<4*sizeof(Real)> QRational;
    typedef RVector3<4*sizeof(Real)> QSVector;

    class RReps
    {
    public:
        RReps (const Real coeff[10]);

        // Quadratic coefficients.
        QRational A00, A01, A02, A11, A12, A22, B0, B1, B2, C;

        // 2-by-2 determinants
        QRational Sub00, Sub01, Sub02, Sub11, Sub12, Sub22;

        // Characteristic polynomial L^3 - C2*L^2 + C1*L - C0.
        QRational C0, C1, C2;

        // For Sturm sequences.
        QRational C3, C4, C5;
    };

    static void GetRootSigns (RReps& reps, int& positiveRoots,
        int& negativeRoots, int& zeroRoots);
    static int GetSignChanges (int quantity, const QRational* value);
    static int ClassifyZeroRoots0 (const RReps& reps, int positiveRoots);
    static int ClassifyZeroRoots1 (const RReps& reps, int positiveRoots);
    static int ClassifyZeroRoots1 (const RReps& reps, int positiveRoots,
        const QSVector& P0, const QSVector& P1, const QSVector& P2);
    static int ClassifyZeroRoots2 (const RReps& reps, int positiveRoots);
    static int ClassifyZeroRoots2 (const RReps& reps, int positiveRoots,
        const QSVector& P0, const QSVector& P1, const QSVector& P2);
    static int ClassifyZeroRoots3 (const RReps& reps);
};

typedef QuadricSurface<float> QuadricSurfacef;
typedef QuadricSurface<double> QuadricSurfaced;

}

#endif
