// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5CUBICPOLYNOMIALCURVE3_H
#define WM5CUBICPOLYNOMIALCURVE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5PolynomialCurve3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM CubicPolynomialCurve3
    : public PolynomialCurve3<Real>
{
public:
    // Construction and destruction.  CubicPolynomialCurve3 accepts
    // responsibility for deleting the input polynomials.
    CubicPolynomialCurve3 (Polynomial1<Real>* xPoly,
        Polynomial1<Real>* yPoly, Polynomial1<Real>* zPoly);

    virtual ~CubicPolynomialCurve3 ();

    // Tessellation data.
    int GetNumVertices () const;
    const Vector3<Real>* GetVertices () const;
    Vector3<Real>* GetVertices ();

    // Tessellation by recursive subdivision.
    void Tessellate (int level);

protected:
    using PolynomialCurve3<Real>::mTMin;
    using PolynomialCurve3<Real>::mTMax;
    using PolynomialCurve3<Real>::GetPosition;
    using PolynomialCurve3<Real>::GetSecondDerivative;

    // Support for precomputation.
    class WM5_MATHEMATICS_ITEM IntervalParameters
    {
    public:
        int I0, I1;
        Vector3<Real> Xuu[2];
    };

    // Subdivide curve into two halves.
    void Subdivide (int level, Real dsqr, Vector3<Real>* X,
        IntervalParameters& IP);

    // Tessellation data.
    int mNumVertices;
    Vector3<Real>* mVertices;
};

typedef CubicPolynomialCurve3<float> CubicPolynomialCurve3f;
typedef CubicPolynomialCurve3<double> CubicPolynomialCurve3d;

}

#endif
