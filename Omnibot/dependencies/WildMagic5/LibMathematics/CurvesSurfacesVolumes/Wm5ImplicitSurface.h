// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IMPLICITSURFACE_H
#define WM5IMPLICITSURFACE_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix3.h"
#include "Wm5Surface.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM ImplicitSurface : public Surface<Real>
{
public:
    // Surface is defined by F(x,y,z) = 0.  In all member functions it is
    // the application's responsibility to ensure that (x,y,z) is a solution
    // to F = 0.

    // Abstract base class.
    virtual ~ImplicitSurface ();

    // The implicit function.
    virtual Real F (const Vector3<Real>& pos) const = 0;

    // First-order partial derivatives.
    virtual Real FX (const Vector3<Real>& pos) const = 0;
    virtual Real FY (const Vector3<Real>& pos) const = 0;
    virtual Real FZ (const Vector3<Real>& pos) const = 0;

    // Second-order partial derivatives.
    virtual Real FXX (const Vector3<Real>& pos) const = 0;
    virtual Real FXY (const Vector3<Real>& pos) const = 0;
    virtual Real FXZ (const Vector3<Real>& pos) const = 0;
    virtual Real FYY (const Vector3<Real>& pos) const = 0;
    virtual Real FYZ (const Vector3<Real>& pos) const = 0;
    virtual Real FZZ (const Vector3<Real>& pos) const = 0;

    // Verify point is on surface (within the tolerance specified by epsilon).
    bool IsOnSurface (const Vector3<Real>& pos, Real epsilon) const;

    // First-order derivatives.
    Vector3<Real> GetGradient (const Vector3<Real>& pos) const;

    // Second-order derivatives.
    Matrix3<Real> GetHessian (const Vector3<Real>& pos) const;

    // Compute a coordinate frame.  The set {T0,T1,N} is a right-handed
    // orthonormal set.
    void GetFrame (const Vector3<Real>& pos, Vector3<Real>& tangent0,
        Vector3<Real>& tangent1, Vector3<Real>& normal) const;

    // Differential geometric quantities.  The returned scalars are the
    // principal curvatures and the returned vectors are the corresponding
    // principal directions.
    bool ComputePrincipalCurvatureInfo (const Vector3<Real>& pos,
        Real& curv0, Real& curv1, Vector3<Real>& dir0, Vector3<Real>& dir1);

protected:
    ImplicitSurface ();
};

typedef ImplicitSurface<float> ImplicitSurfacef;
typedef ImplicitSurface<double> ImplicitSurfaced;

}

#endif
