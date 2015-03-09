// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5PerspProjEllipsoid.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
void PerspectiveProjectEllipsoid (const Ellipsoid3<Real>& ellipsoid,
    const Vector3<Real>& eye, const Plane3<Real>& plane,
    const Vector3<Real>& U, const Vector3<Real>& V, Vector3<Real>& P,
    Ellipse2<Real>& ellipse)
{
    // Get coefficients for ellipsoid as X^T*A*X + B^T*X + C = 0.
    Matrix3<Real> A;
    Vector3<Real> B;
    Real C;
    ellipsoid.ToCoefficients(A, B, C);

    // Compute matrix M (see PerspectiveProjectionEllipsoid.pdf).
    Vector3<Real> AE = A*eye;
    Real EAE = eye.Dot(AE);
    Real BE = B.Dot(eye);
    Real QuadE = ((Real)4)*(EAE + BE + C);
    Vector3<Real> Bp2AE = B + ((Real)2)*AE;
    Matrix3<Real> mat = Matrix3<Real>(Bp2AE, Bp2AE) - QuadE*A;

    // Compute coefficients for projected ellipse.
    Vector3<Real> MU = mat*U;
    Vector3<Real> MV = mat*V;
    Vector3<Real> MN = mat*plane.Normal;
    Real DmNdE = -plane.DistanceTo(eye);
    P = eye + DmNdE*plane.Normal;

    Matrix2<Real> AOut;
    Vector2<Real> BOut;
    Real COut;
    AOut[0][0] = U.Dot(MU);
    AOut[0][1] = U.Dot(MV);
    AOut[1][1] = V.Dot(MV);
    AOut[1][0] = AOut[0][1];
    BOut[0] = ((Real)2)*DmNdE*(U.Dot(MN));
    BOut[1] = ((Real)2)*DmNdE*(V.Dot(MN));
    COut = DmNdE*DmNdE*(plane.Normal.Dot(MN));
    ellipse.FromCoefficients(AOut, BOut, COut);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
void PerspectiveProjectEllipsoid<float> (const Ellipsoid3<float>&,
    const Vector3<float>&, const Plane3<float>&, const Vector3<float>&,
    const Vector3<float>&, Vector3<float>&, Ellipse2<float>&);

template WM5_MATHEMATICS_ITEM
void PerspectiveProjectEllipsoid<double> (const Ellipsoid3<double>&,
    const Vector3<double>&, const Plane3<double>&, const Vector3<double>&,
    const Vector3<double>&, Vector3<double>&, Ellipse2<double>&);
//----------------------------------------------------------------------------
}
