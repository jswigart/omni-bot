// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSphere3Cone3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSphere3Cone3<Real>::IntrSphere3Cone3 (const Sphere3<Real>& rkSphere,
    const Cone3<Real>& rkCone)
    :
    mSphere(&rkSphere),
    mCone(&rkCone)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrSphere3Cone3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
const Cone3<Real>& IntrSphere3Cone3<Real>::GetCone () const
{
    return *mCone;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSphere3Cone3<Real>::Test ()
{
    Real invSin = ((Real)1)/mCone->SinAngle;
    Real cosSqr = mCone->CosAngle*mCone->CosAngle;

    Vector3<Real> CmV = mSphere->Center - mCone->Vertex;
    Vector3<Real> D = CmV + (mSphere->Radius*invSin)*mCone->Axis;
    Real DSqrLen = D.SquaredLength();
    Real e = D.Dot(mCone->Axis);
    if (e > (Real)0 && e*e >= DSqrLen*cosSqr)
    {
        Real sinSqr = mCone->SinAngle*mCone->SinAngle;
        DSqrLen = CmV.SquaredLength();
        e = -CmV.Dot(mCone->Axis);
        if (e > (Real)0 && e*e >= DSqrLen*sinSqr)
        {
            Real rSqr = mSphere->Radius*mSphere->Radius;
            return DSqrLen <= rSqr;
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSphere3Cone3<Real>::Find ()
{
    // Test whether cone vertex is in sphere.
    Vector3<Real> diff = mSphere->Center - mCone->Vertex;
    Real rSqr = mSphere->Radius*mSphere->Radius;
    Real lenSqr = diff.SquaredLength();
    if (lenSqr <= rSqr)
    {
        return true;
    }

    // Test whether sphere center is in cone.
    Real dot = diff.Dot(mCone->Axis);
    Real dotSqr = dot*dot;
    Real cosSqr = mCone->CosAngle*mCone->CosAngle;
    if (dotSqr >= lenSqr*cosSqr && dot > (Real)0)
    {
        // Sphere center is inside cone, so sphere and cone intersect.
        return true;
    }

    // Sphere center is outside cone.  Problem now reduces to looking for
    // an intersection between circle and ray in the plane containing
    // cone vertex and spanned by cone axis and vector from vertex to
    // sphere center.

    // Ray is t*D+V (t >= 0) where |D| = 1 and dot(A,D) = cos(angle).
    // Also, D = e*A+f*(C-V).  Plugging ray equation into sphere equation
    // yields R^2 = |t*D+V-C|^2, so the quadratic for intersections is
    // t^2 - 2*dot(D,C-V)*t + |C-V|^2 - R^2 = 0.  An intersection occurs
    // if and only if the discriminant is nonnegative.  This test becomes
    //
    //     dot(D,C-V)^2 >= dot(C-V,C-V) - R^2
    //
    // Note that if the right-hand side is nonpositive, then the inequality
    // is true (the sphere contains V).  I have already ruled this out in
    // the first block of code in this function.

    Real uLen = Math<Real>::Sqrt(Math<Real>::FAbs(lenSqr-dotSqr));
    Real test = mCone->CosAngle*dot + mCone->SinAngle*uLen;
    Real discr = test*test - lenSqr + rSqr;

    // compute point of intersection closest to vertex V
    Real t = test - Math<Real>::Sqrt(discr);
    Vector3<Real> B = diff - dot*mCone->Axis;
    Real tmp = mCone->SinAngle/uLen;
    mPoint = t*(mCone->CosAngle*mCone->Axis + tmp*B);

    return discr >= (Real)0 && test >= (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSphere3Cone3<Real>::GetPoint () const
{
    return mPoint;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSphere3Cone3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSphere3Cone3<double>;
//----------------------------------------------------------------------------
}
