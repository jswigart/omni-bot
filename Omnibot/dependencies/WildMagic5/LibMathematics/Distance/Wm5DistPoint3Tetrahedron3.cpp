// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Tetrahedron3.h"
#include "Wm5DistPoint3Triangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Tetrahedron3<Real>::DistPoint3Tetrahedron3 (
    const Vector3<Real>& point, const Tetrahedron3<Real>& tetrahedron)
    :
    mPoint(&point),
    mTetrahedron(&tetrahedron)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Tetrahedron3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Tetrahedron3<Real>& DistPoint3Tetrahedron3<Real>::GetTetrahedron ()
    const
{
    return *mTetrahedron;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Tetrahedron3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Tetrahedron3<Real>::GetSquared ()
{
    // Construct the planes for the faces of the tetrahedron.  The normals
    // are outer pointing, but specified not to be unit length.  We only need
    // to know sidedness of the query point, so we will save cycles by not
    // computing unit-length normals.
    Plane3<Real> planes[4];
    mTetrahedron->GetPlanes(planes);

    // Determine which faces are visible to the query point.  Only these
    // need to be processed by point-to-triangle distance queries.
    Real minSqrDistance = Math<Real>::MAX_REAL;
    Vector3<Real> minTetraClosest = Vector3<Real>::ZERO;
    for (int i = 0; i < 4; ++i)
    {
        if (planes[i].WhichSide(*mPoint) >= 0)
        {
            int indices[3] = { 0, 0, 0 };
            mTetrahedron->GetFaceIndices(i, indices);
            Triangle3<Real> triangle(
                mTetrahedron->V[indices[0]],
                mTetrahedron->V[indices[1]],
                mTetrahedron->V[indices[2]]);

            DistPoint3Triangle3<Real> query(*mPoint, triangle);
            Real sqrDistance = query.GetSquared();
            if (sqrDistance < minSqrDistance)
            {
                minSqrDistance = sqrDistance;
                minTetraClosest = query.GetClosestPoint1();
            }
        }
    }

    mClosestPoint0 = *mPoint;
    if (minSqrDistance != Math<Real>::MAX_REAL)
    {
        // The query point is outside the "solid" tetrahedron.
        mClosestPoint1 = minTetraClosest;
    }
    else
    {
        // The query point is inside the "solid" tetrahedron.  Report a zero
        // distance.  The closest points are identical.
        minSqrDistance = (Real)0;
        mClosestPoint1 = *mPoint;
    }

    return minSqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Tetrahedron3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedV0 = mTetrahedron->V[0] + t*velocity1;
    Vector3<Real> movedV1 = mTetrahedron->V[1] + t*velocity1;
    Vector3<Real> movedV2 = mTetrahedron->V[2] + t*velocity1;
    Vector3<Real> movedV3 = mTetrahedron->V[3] + t*velocity1;
    Tetrahedron3<Real> movedTetra(movedV0, movedV1, movedV2, movedV3);
    return DistPoint3Tetrahedron3<Real>(movedPoint, movedTetra).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Tetrahedron3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedV0 = mTetrahedron->V[0] + t*velocity1;
    Vector3<Real> movedV1 = mTetrahedron->V[1] + t*velocity1;
    Vector3<Real> movedV2 = mTetrahedron->V[2] + t*velocity1;
    Vector3<Real> movedV3 = mTetrahedron->V[3] + t*velocity1;
    Tetrahedron3<Real> movedTetra(movedV0, movedV1, movedV2, movedV3);
    return DistPoint3Tetrahedron3<Real>(movedPoint, movedTetra).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Tetrahedron3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Tetrahedron3<double>;
//----------------------------------------------------------------------------
}
