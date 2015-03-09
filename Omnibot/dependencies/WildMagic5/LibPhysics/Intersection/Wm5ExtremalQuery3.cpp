// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5ExtremalQuery3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ExtremalQuery3<Real>::ExtremalQuery3 (const ConvexPolyhedron3<Real>* polytope)
    :
    mPolytope(polytope)
{
    // Create the triangle normals.
    const Vector3<Real>* vertices = mPolytope->GetVertices();
    int numTriangles = mPolytope->GetNumTriangles();
    const int* indices = mPolytope->GetIndices();
    mFaceNormals = new1<Vector3<Real> >(numTriangles);
    for (int i = 0; i < numTriangles; ++i)
    {
        const Vector3<Real>& v0 = vertices[*indices++];
        const Vector3<Real>& v1 = vertices[*indices++];
        const Vector3<Real>& v2 = vertices[*indices++];
        Vector3<Real> edge1 = v1 - v0;
        Vector3<Real> edge2 = v2 - v0;
        mFaceNormals[i] = edge1.UnitCross(edge2);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
ExtremalQuery3<Real>::~ExtremalQuery3 ()
{
    delete1(mFaceNormals);
}
//----------------------------------------------------------------------------
template <typename Real>
const ConvexPolyhedron3<Real>* ExtremalQuery3<Real>::GetPolytope () const
{
    return mPolytope;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* ExtremalQuery3<Real>::GetFaceNormals () const
{
    return mFaceNormals;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class ExtremalQuery3<float>;

template WM5_PHYSICS_ITEM
class ExtremalQuery3<double>;
//----------------------------------------------------------------------------
}
