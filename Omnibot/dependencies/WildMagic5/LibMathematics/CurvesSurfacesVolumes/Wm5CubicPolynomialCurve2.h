// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5CUBICPOLYNOMIALCURVE2_H
#define WM5CUBICPOLYNOMIALCURVE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5PolynomialCurve2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM CubicPolynomialCurve2
    : public PolynomialCurve2<Real>
{
public:
    // Construction and destruction.  CubicPolynomialCurve2 accepts
    // responsibility for deleting the input polynomials.
    CubicPolynomialCurve2 (Polynomial1<Real>* xPoly,
        Polynomial1<Real>* yPoly);
    virtual ~CubicPolynomialCurve2 ();

    // Tessellation data.
    int GetNumVertices () const;
    const Vector2<Real>* GetVertices () const;
    Vector2<Real>* GetVertices ();

    // Tessellation by recursive subdivision.
    void Tessellate (int level);

protected:
    using PolynomialCurve2<Real>::mTMin;
    using PolynomialCurve2<Real>::mTMax;
    using PolynomialCurve2<Real>::GetPosition;
    using PolynomialCurve2<Real>::GetSecondDerivative;

    // Support for precomputation.
    class WM5_MATHEMATICS_ITEM IntervalParameters
    {
    public:
        int I0, I1;
        Vector2<Real> Xuu[2];
    };

    // Subdivide curve into two halves.
    void Subdivide (int level, Real dsqr, Vector2<Real>* X,
        IntervalParameters& IP);

    // Tessellation data.
    int mNumVertices;
    Vector2<Real>* mVertices;
};

typedef CubicPolynomialCurve2<float> CubicPolynomialCurve2f;
typedef CubicPolynomialCurve2<double> CubicPolynomialCurve2d;

}

#endif
