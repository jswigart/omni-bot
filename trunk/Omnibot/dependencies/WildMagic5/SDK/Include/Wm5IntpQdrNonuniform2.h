// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPQUADRATICNONUNIFORM2_H
#define WM5INTPQUADRATICNONUNIFORM2_H

// Quadratic interpolation of a network of triangles whose vertices are of
// the form (x,y,f(x,y)).  This code is an implementation of the algorithm
// found in
//
//   Zoltan J. Cendes and Steven H. Wong,
//   C1 quadratic interpolation over arbitrary point sets,
//   IEEE Computer Graphics & Applications,
//   pp. 8-16, 1987

#include "Wm5MathematicsLIB.h"
#include "Wm5Delaunay2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpQdrNonuniform2
{
public:
    // Construction and destruction.  If you want IntpQdrNonuniform2 to
    // delete the input array during destruction, set owner to 'true'.
    // Otherwise, you own the array and must delete it yourself.
    //
    // The first constructor requires you to specify function values F and
    // first-order partial derivative values Fx and Fy.  The second
    // constructor requires only F, but the Fx and Fy values are estimated
    // at the sample points.

    IntpQdrNonuniform2 (const Delaunay2<Real>& DT, Real* F, Real* FX,
        Real* FY, bool owner);

    IntpQdrNonuniform2 (const Delaunay2<Real>& DT, Real* F, bool owner);

    ~IntpQdrNonuniform2 ();

    // Quadratic interpolation.  The return value is 'true' if and only if the
    // input point is in the convex hull of the input vertices, in which case
    // the interpolation is valid.
    bool Evaluate (const Vector2<Real>& P, Real& F, Real& FX, Real& FY);

private:
    class TriangleData
    {
    public:
        Vector2<Real> Center;
        Vector2<Real> Intersect[3];
        Real Coeff[19];
    };

    class Jet
    {
    public:
        Real F, FX, FY;
    };

    const Delaunay2<Real>* mDT;
    Real* mF;
    Real* mFX;
    Real* mFY;
    TriangleData* mTData;  // triangle data
    bool mFOwner, mFXFYOwner;

    void EstimateDerivatives ();
    void ProcessTriangles ();
    void ComputeCrossEdgeIntersections (int t);
    void ComputeCoefficients (int t);
};

typedef IntpQdrNonuniform2<float> IntpQdrNonuniform2f;
typedef IntpQdrNonuniform2<double> IntpQdrNonuniform2d;

}

#endif
