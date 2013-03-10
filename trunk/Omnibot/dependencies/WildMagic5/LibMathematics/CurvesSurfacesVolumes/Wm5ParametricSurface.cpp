// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ParametricSurface.h"
#include "Wm5Matrix2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ParametricSurface<Real>::ParametricSurface (Real umin, Real umax,
   Real vmin, Real vmax, bool rectangular)
{
    assertion(umin < umax && vmin < vmax, "Invalid domain\n");

    mUMin = umin;
    mUMax = umax;
    mVMin = vmin;
    mVMax = vmax;
    mRectangular = rectangular;
}
//----------------------------------------------------------------------------
template <typename Real>
ParametricSurface<Real>::~ParametricSurface ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Real ParametricSurface<Real>::GetUMin () const
{
    return mUMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ParametricSurface<Real>::GetUMax () const
{
    return mUMax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ParametricSurface<Real>::GetVMin () const
{
    return mVMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ParametricSurface<Real>::GetVMax () const
{
    return mVMax;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ParametricSurface<Real>::IsRectangular () const
{
    return mRectangular;
}
//----------------------------------------------------------------------------
template <typename Real>
void ParametricSurface<Real>::GetFrame (Real u, Real v,
    Vector3<Real>& position, Vector3<Real>& tangent0,
    Vector3<Real>& tangent1, Vector3<Real>& normal) const
{
    position = P(u, v);

    tangent0 = PU(u, v);
    tangent1 = PV(u, v);
    tangent0.Normalize();  // T0
    tangent1.Normalize();  // temporary T1 just to compute N
    normal = tangent0.UnitCross(tangent1);  // N

    // The normalized first derivatives are not necessarily orthogonal.
    // Recompute T1 so that {T0,T1,N} is an orthonormal set.
    tangent1 = normal.Cross(tangent0);
}
//----------------------------------------------------------------------------
template <typename Real>
void ParametricSurface<Real>::ComputePrincipalCurvatureInfo (Real u, Real v,
    Real& curv0, Real& curv1, Vector3<Real>& dir0,
    Vector3<Real>& dir1)
{
    // Tangents:  T0 = (x_u,y_u,z_u), T1 = (x_v,y_v,z_v)
    // Normal:    N = Cross(T0,T1)/Length(Cross(T0,T1))
    // Metric Tensor:    G = +-                      -+
    //                       | Dot(T0,T0)  Dot(T0,T1) |
    //                       | Dot(T1,T0)  Dot(T1,T1) |
    //                       +-                      -+
    //
    // Curvature Tensor:  B = +-                          -+
    //                        | -Dot(N,T0_u)  -Dot(N,T0_v) |
    //                        | -Dot(N,T1_u)  -Dot(N,T1_v) |
    //                        +-                          -+
    //
    // Principal curvatures k are the generalized eigenvalues of
    //
    //     Bw = kGw
    //
    // If k is a curvature and w=(a,b) is the corresponding solution to
    // Bw = kGw, then the principal direction as a 3D vector is d = a*U+b*V.
    //
    // Let k1 and k2 be the principal curvatures.  The mean curvature
    // is (k1+k2)/2 and the Gaussian curvature is k1*k2.

    // Compute derivatives.
    Vector3<Real> derU = PU(u,v);
    Vector3<Real> derV = PV(u,v);
    Vector3<Real> derUU = PUU(u,v);
    Vector3<Real> derUV = PUV(u,v);
    Vector3<Real> derVV = PVV(u,v);

    // Compute the metric tensor.
    Matrix2<Real> metricTensor;
    metricTensor[0][0] = derU.Dot(derU);
    metricTensor[0][1] = derU.Dot(derV);
    metricTensor[1][0] = metricTensor[0][1];
    metricTensor[1][1] = derV.Dot(derV);

    // Compute the curvature tensor.
    Vector3<Real> normal = derU.UnitCross(derV);
    Matrix2<Real> curvatureTensor;
    curvatureTensor[0][0] = -normal.Dot(derUU);
    curvatureTensor[0][1] = -normal.Dot(derUV);
    curvatureTensor[1][0] = curvatureTensor[0][1];
    curvatureTensor[1][1] = -normal.Dot(derVV);

    // Characteristic polynomial is 0 = det(B-kG) = c2*k^2+c1*k+c0.
    Real c0 = curvatureTensor.Determinant();
    Real c1 = ((Real)2)*curvatureTensor[0][1]* metricTensor[0][1] -
        curvatureTensor[0][0]*metricTensor[1][1] -
        curvatureTensor[1][1]*metricTensor[0][0];
    Real c2 = metricTensor.Determinant();

    // Principal curvatures are roots of characteristic polynomial.
    Real temp = Math<Real>::Sqrt(Math<Real>::FAbs(c1*c1 - ((Real)4)*c0*c2));
    Real mult = ((Real)0.5)/c2;
    curv0 = -mult*(c1+temp);
    curv1 = mult*(-c1+temp);

    // Principal directions are solutions to (B-kG)w = 0,
    // w1 = (b12-k1*g12,-(b11-k1*g11)) OR (b22-k1*g22,-(b12-k1*g12)).
    Real a0 = curvatureTensor[0][1] - curv0*metricTensor[0][1];
    Real a1 = curv0*metricTensor[0][0] - curvatureTensor[0][0];
    Real length = Math<Real>::Sqrt(a0*a0 + a1*a1);
    if (length >= Math<Real>::ZERO_TOLERANCE)
    {
        dir0 = a0*derU + a1*derV;
    }
    else
    {
        a0 = curvatureTensor[1][1] - curv0*metricTensor[1][1];
        a1 = curv0*metricTensor[0][1] - curvatureTensor[0][1];
        length = Math<Real>::Sqrt(a0*a0 + a1*a1);
        if (length >= Math<Real>::ZERO_TOLERANCE)
        {
            dir0 = a0*derU + a1*derV;
        }
        else
        {
            // Umbilic (surface is locally sphere, any direction principal).
            dir0 = derU;
        }
    }
    dir0.Normalize();

    // Second tangent is cross product of first tangent and normal.
    dir1 = dir0.Cross(normal);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ParametricSurface<float>;

template WM5_MATHEMATICS_ITEM
class ParametricSurface<double>;
//----------------------------------------------------------------------------
}
