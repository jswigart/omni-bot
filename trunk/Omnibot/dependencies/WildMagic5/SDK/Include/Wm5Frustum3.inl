// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Frustum3<Real>::Frustum3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Frustum3<Real>::~Frustum3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Frustum3<Real>::Frustum3 (const Vector3<Real>& origin,
    const Vector3<Real>& dvector, const Vector3<Real>& uvector,
    const Vector3<Real>& rvector, Real dmin, Real dmax, Real ubound,
    Real rbound)
    :
    Origin(origin),
    DVector(dvector),
    UVector(uvector),
    RVector(rvector),
    DMin(dmin),
    DMax(dmax),
    UBound(ubound),
    RBound(rbound)
{
    Update();
}
//----------------------------------------------------------------------------
template <typename Real>
void Frustum3<Real>::Update ()
{
    mDRatio = DMax/DMin;
    mMTwoUF = ((Real)-2)*UBound*DMax;
    mMTwoRF = ((Real)-2)*RBound*DMax;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real Frustum3<Real>::GetDRatio () const
{
    return mDRatio;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real Frustum3<Real>::GetMTwoUF () const
{
    return mMTwoUF;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real Frustum3<Real>::GetMTwoRF () const
{
    return mMTwoRF;
}
//----------------------------------------------------------------------------
template <typename Real>
void Frustum3<Real>::ComputeVertices (Vector3<Real> vertex[8]) const
{
    Vector3<Real> DScaled = DMin*DVector;
    Vector3<Real> UScaled = UBound*UVector;
    Vector3<Real> RScaled = RBound*RVector;

    vertex[0] = DScaled - UScaled - RScaled;
    vertex[1] = DScaled - UScaled + RScaled;
    vertex[2] = DScaled + UScaled + RScaled;
    vertex[3] = DScaled + UScaled - RScaled;

    for (int i = 0, ip = 4; i < 4; ++i, ++ip)
    {
        vertex[ip] = Origin + mDRatio*vertex[i];
        vertex[i] += Origin;
    }
}
//----------------------------------------------------------------------------
