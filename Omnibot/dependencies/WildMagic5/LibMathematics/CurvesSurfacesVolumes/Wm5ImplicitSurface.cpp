// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ImplicitSurface.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ImplicitSurface<Real>::ImplicitSurface ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
ImplicitSurface<Real>::~ImplicitSurface ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool ImplicitSurface<Real>::IsOnSurface (const Vector3<Real>& pos,
    Real epsilon) const
{
    return Math<Real>::FAbs(F(pos)) <= epsilon;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> ImplicitSurface<Real>::GetGradient (const Vector3<Real>& pos)
    const
{
    Real fx = FX(pos);
    Real fy = FY(pos);
    Real fz = FZ(pos);
    return Vector3<Real>(fx, fy, fz);
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> ImplicitSurface<Real>::GetHessian (const Vector3<Real>& pos)
    const
{
    Real fxx = FXX(pos);
    Real fxy = FXY(pos);
    Real fxz = FXZ(pos);
    Real fyy = FYY(pos);
    Real fyz = FYZ(pos);
    Real fzz = FZZ(pos);
    return Matrix3<Real>(fxx, fxy, fxz, fxy, fyy, fyz, fxz, fyz, fzz);
}
//----------------------------------------------------------------------------
template <typename Real>
void ImplicitSurface<Real>::GetFrame (const Vector3<Real>& pos,
    Vector3<Real>& tangent0, Vector3<Real>& tangent1,
    Vector3<Real>& normal) const
{
    normal = GetGradient(pos);
    Vector3<Real>::GenerateOrthonormalBasis(tangent0, tangent1, normal);
}
//----------------------------------------------------------------------------
template <typename Real>
bool ImplicitSurface<Real>::ComputePrincipalCurvatureInfo (
    const Vector3<Real>& pos, Real& curv0, Real& curv1, Vector3<Real>& dir0,
    Vector3<Real>& dir1)
{
    // Principal curvatures and directions for implicitly defined surfaces
    // F(x,y,z) = 0.
    //
    // DF = (Fx,Fy,Fz), L = Length(DF)
    //
    // D^2 F = +-           -+
    //         | Fxx Fxy Fxz |
    //         | Fxy Fyy Fyz |
    //         | Fxz Fyz Fzz |
    //         +-           -+
    //
    // adj(D^2 F) = +-                                                 -+
    //              | Fyy*Fzz-Fyz*Fyz  Fyz*Fxz-Fxy*Fzz  Fxy*Fyz-Fxz*Fyy |
    //              | Fyz*Fxz-Fxy*Fzz  Fxx*Fzz-Fxz*Fxz  Fxy*Fxz-Fxx*Fyz |
    //              | Fxy*Fyz-Fxz*Fyy  Fxy*Fxz-Fxx*Fyz  Fxx*Fyy-Fxy*Fxy |
    //              +-                                                 -+
    //
    // Gaussian curvature = [DF^t adj(D^2 F) DF]/L^4
    // 
    // Mean curvature = 0.5*[trace(D^2 F)/L - (DF^t D^2 F DF)/L^3]

    // first derivatives
    Real fx = FX(pos);
    Real fy = FY(pos);
    Real fz = FZ(pos);
    Real fLength = Math<Real>::Sqrt(fx*fx + fy*fy + fz*fz);
    if (fLength <= Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    Real fxfx = fx*fx;
    Real fxfy = fx*fy;
    Real fxfz = fx*fz;
    Real fyfy = fy*fy;
    Real fyfz = fy*fz;
    Real fzfz = fz*fz;

    Real invLength = ((Real)1)/fLength;
    Real invLength2 = invLength*invLength;
    Real invLength3 = invLength*invLength2;
    Real invLength4 = invLength2*invLength2;

    // second derivatives
    Real fxx = FXX(pos);
    Real fxy = FXY(pos);
    Real fxz = FXZ(pos);
    Real fyy = FYY(pos);
    Real fyz = FYZ(pos);
    Real fzz = FZZ(pos);

    // mean curvature
    Real meanCurv = ((Real)0.5)*invLength3*(fxx*(fyfy + fzfz) +
        fyy*(fxfx + fzfz) + fzz*(fxfx + fyfy) - ((Real)2)*(fxy*fxfy + fxz*fxfz +
        fyz*fyfz));

    // Gaussian curvature
    Real gaussCurv = invLength4*(fxfx*(fyy*fzz - fyz*fyz)
        + fyfy*(fxx*fzz - fxz*fxz) + fzfz*(fxx*fyy - fxy*fxy)
        + ((Real)2)*(fxfy*(fxz*fyz - fxy*fzz) + fxfz*(fxy*fyz - fxz*fyy)
        + fyfz*(fxy*fxz - fxx*fyz)));

    // solve for principal curvatures
    Real discr = Math<Real>::Sqrt(Math<Real>::FAbs(meanCurv*meanCurv-gaussCurv));
    curv0 = meanCurv - discr;
    curv1 = meanCurv + discr;

    Real m00 = ((-(Real)1 + fxfx*invLength2)*fxx)*invLength +
        (fxfy*fxy)*invLength3 + (fxfz*fxz)*invLength3;
    Real m01 = ((-(Real)1 + fxfx*invLength2)*fxy)*invLength +
        (fxfy*fyy)*invLength3 + (fxfz*fyz)*invLength3;
    Real m02 = ((-(Real)1 + fxfx*invLength2)*fxz)*invLength +
        (fxfy*fyz)*invLength3 + (fxfz*fzz)*invLength3;
    Real m10 = (fxfy*fxx)*invLength3 +
        ((-(Real)1 + fyfy*invLength2)*fxy)*invLength + (fyfz*fxz)*invLength3;
    Real m11 = (fxfy*fxy)*invLength3 +
        ((-(Real)1 + fyfy*invLength2)*fyy)*invLength + (fyfz*fyz)*invLength3;
    Real m12 = (fxfy*fxz)*invLength3 +
        ((-(Real)1 + fyfy*invLength2)*fyz)*invLength + (fyfz*fzz)*invLength3;
    Real m20 = (fxfz*fxx)*invLength3 + (fyfz*fxy)*invLength3 +
        ((-(Real)1 + fzfz*invLength2)*fxz)*invLength;
    Real m21 = (fxfz*fxy)*invLength3 + (fyfz*fyy)*invLength3 +
        ((-(Real)1 + fzfz*invLength2)*fyz)*invLength;
    Real m22 = (fxfz*fxz)*invLength3 + (fyfz*fyz)*invLength3 +
        ((-(Real)1 + fzfz*invLength2)*fzz)*invLength;

    // solve for principal directions
    Real tmp1 = m00 + curv0;
    Real tmp2 = m11 + curv0;
    Real tmp3 = m22 + curv0;

    Vector3<Real> U[3];
    Real lengths[3];

    U[0].X() = m01*m12-m02*tmp2;
    U[0].Y() = m02*m10-m12*tmp1;
    U[0].Z() = tmp1*tmp2-m01*m10;
    lengths[0] = U[0].Length();

    U[1].X() = m01*tmp3-m02*m21;
    U[1].Y() = m02*m20-tmp1*tmp3;
    U[1].Z() = tmp1*m21-m01*m20;
    lengths[1] = U[1].Length();

    U[2].X() = tmp2*tmp3-m12*m21;
    U[2].Y() = m12*m20-m10*tmp3;
    U[2].Z() = m10*m21-m20*tmp2;
    lengths[2] = U[2].Length();

    int maxIndex = 0;
    Real maxValue = lengths[0];
    if (lengths[1] > maxValue)
    {
        maxIndex = 1;
        maxValue = lengths[1];
    }
    if (lengths[2] > maxValue)
    {
        maxIndex = 2;
    }

    invLength = ((Real)1)/lengths[maxIndex];
    U[maxIndex] *= invLength;

    dir1 = U[maxIndex];
    dir0 = dir1.UnitCross(Vector3<Real>(fx, fy, fz));

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ImplicitSurface<float>;

template WM5_MATHEMATICS_ITEM
class ImplicitSurface<double>;
//----------------------------------------------------------------------------
}
