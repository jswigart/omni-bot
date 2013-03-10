// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay3Triangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay3Triangle3<Real>::IntrRay3Triangle3 (const Ray3<Real>& ray,
    const Triangle3<Real>& triangle)
    :
    mRay(&ray),
    mTriangle(&triangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& IntrRay3Triangle3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrRay3Triangle3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Triangle3<Real>::Test ()
{
    // Compute the offset origin, edges, and normal.
    Vector3<Real> diff = mRay->Origin - mTriangle->V[0];
    Vector3<Real> edge1 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> edge2 = mTriangle->V[2] - mTriangle->V[0];
    Vector3<Real> normal = edge1.Cross(edge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real DdN = mRay->Direction.Dot(normal);
    Real sign;
    if (DdN > Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)1;
    }
    else if (DdN < -Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)-1;
        DdN = -DdN;
    }
    else
    {
        // Ray and triangle are parallel, call it a "no intersection"
        // even if the ray does intersect.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    Real DdQxE2 = sign*mRay->Direction.Dot(diff.Cross(edge2));
    if (DdQxE2 >= (Real)0)
    {
        Real DdE1xQ = sign*mRay->Direction.Dot(edge1.Cross(diff));
        if (DdE1xQ >= (Real)0)
        {
            if (DdQxE2 + DdE1xQ <= DdN)
            {
                // Line intersects triangle, check if ray does.
                Real QdN = -sign*diff.Dot(normal);
                if (QdN >= (Real)0)
                {
                    // Ray intersects triangle.
                    mIntersectionType = IT_POINT;
                    return true;
                }
                // else: t < 0, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Triangle3<Real>::Find ()
{
    // Compute the offset origin, edges, and normal.
    Vector3<Real> diff = mRay->Origin - mTriangle->V[0];
    Vector3<Real> edge1 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> edge2 = mTriangle->V[2] - mTriangle->V[0];
    Vector3<Real> normal = edge1.Cross(edge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real DdN = mRay->Direction.Dot(normal);
    Real sign;
    if (DdN > Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)1;
    }
    else if (DdN < -Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)-1;
        DdN = -DdN;
    }
    else
    {
        // Ray and triangle are parallel, call it a "no intersection"
        // even if the ray does intersect.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    Real DdQxE2 = sign*mRay->Direction.Dot(diff.Cross(edge2));
    if (DdQxE2 >= (Real)0)
    {
        Real DdE1xQ = sign*mRay->Direction.Dot(edge1.Cross(diff));
        if (DdE1xQ >= (Real)0)
        {
            if (DdQxE2 + DdE1xQ <= DdN)
            {
                // Line intersects triangle, check if ray does.
                Real QdN = -sign*diff.Dot(normal);
                if (QdN >= (Real)0)
                {
                    // Ray intersects triangle.
                    Real inv = ((Real)1)/DdN;
                    mRayParameter = QdN*inv;
                    mTriBary1 = DdQxE2*inv;
                    mTriBary2 = DdE1xQ*inv;
                    mTriBary0 = (Real)1 - mTriBary1 - mTriBary2;
                    mIntersectionType = IT_POINT;
                    return true;
                }
                // else: t < 0, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrRay3Triangle3<Real>::GetRayParameter () const
{
    return mRayParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrRay3Triangle3<Real>::GetTriBary0 () const
{
    return mTriBary0;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrRay3Triangle3<Real>::GetTriBary1 () const
{
    return mTriBary1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrRay3Triangle3<Real>::GetTriBary2 () const
{
    return mTriBary2;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay3Triangle3<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay3Triangle3<double>;
//----------------------------------------------------------------------------
}
