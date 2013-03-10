// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5MeshSmoother.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
MeshSmoother<Real>::MeshSmoother ()
{
    mNumVertices = 0;
    mVertices = 0;
    mNumTriangles = 0;
    mIndices = 0;
    mNormals = 0;
    mMeans = 0;
    mNeighborCounts = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
MeshSmoother<Real>::MeshSmoother (int numVertices, Vector3<Real>* vertices,
    int numTriangles, const int* indices)
{
    mVertices = 0;
    mNormals = 0;
    mIndices = 0;
    mMeans = 0;
    mNeighborCounts = 0;

    Create(numVertices, vertices, numTriangles, indices);
}
//----------------------------------------------------------------------------
template <typename Real>
MeshSmoother<Real>::~MeshSmoother ()
{
    Destroy();
}
//----------------------------------------------------------------------------
template <typename Real>
void MeshSmoother<Real>::Create (int numVertices, Vector3<Real>* vertices,
    int numTriangles, const int* indices)
{
    // Remove previously allocated smoother data.
    Destroy();

    mNumVertices = numVertices;
    mVertices = vertices;
    mNumTriangles = numTriangles;
    mIndices = indices;

    mNormals = new1<Vector3<Real> >(mNumVertices);
    mMeans = new1<Vector3<Real> >(mNumVertices);
    mNeighborCounts = new1<int>(mNumVertices);

    // Count the number of vertex neighbors.
    memset(mNeighborCounts, 0, mNumVertices*sizeof(int));
    const int* current = mIndices;
    for (int i = 0; i < mNumTriangles; ++i)
    {
        mNeighborCounts[*current++] += 2;
        mNeighborCounts[*current++] += 2;
        mNeighborCounts[*current++] += 2;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void MeshSmoother<Real>::Destroy ()
{
    delete1(mNormals);
    delete1(mMeans);
    delete1(mNeighborCounts);
}
//----------------------------------------------------------------------------
template <typename Real>
void MeshSmoother<Real>::Update (Real t)
{
    memset(mNormals, 0, mNumVertices*sizeof(Vector3<Real>));
    memset(mMeans, 0, mNumVertices*sizeof(Vector3<Real>));

    const int* current = mIndices;
    int i;
    for (i = 0; i < mNumTriangles; ++i)
    {
        int v0 = *current++;
        int v1 = *current++;
        int v2 = *current++;

        Vector3<Real>& V0 = mVertices[v0];
        Vector3<Real>& V1 = mVertices[v1];
        Vector3<Real>& V2 = mVertices[v2];

        Vector3<Real> edge1 = V1 - V0;
        Vector3<Real> edge2 = V2 - V0;
        Vector3<Real> normal = edge1.Cross(edge2);

        mNormals[v0] += normal;
        mNormals[v1] += normal;
        mNormals[v2] += normal;

        mMeans[v0] += V1 + V2;
        mMeans[v1] += V2 + V0;
        mMeans[v2] += V0 + V1;
    }

    for (i = 0; i < mNumVertices; ++i)
    {
        mNormals[i].Normalize();
        mMeans[i] /= (Real)mNeighborCounts[i];
    }

    for (i = 0; i < mNumVertices; ++i)
    {
        if (VertexInfluenced(i, t))
        {
            Vector3<Real> localDiff = mMeans[i] - mVertices[i];
            Vector3<Real> surfaceNormal = localDiff.Dot(mNormals[i]) *
                mNormals[i];
            Vector3<Real> tangent = localDiff - surfaceNormal;

            Real tanWeight = GetTangentWeight(i, t);
            Real norWeight = GetNormalWeight(i, t);
            mVertices[i] += tanWeight*tangent + norWeight*mNormals[i];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool MeshSmoother<Real>::VertexInfluenced (int, Real)
{
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
Real MeshSmoother<Real>::GetTangentWeight (int, Real)
{
    return (Real)0.5;
}
//----------------------------------------------------------------------------
template <typename Real>
Real MeshSmoother<Real>::GetNormalWeight (int, Real)
{
    return (Real)0.0;
}
//----------------------------------------------------------------------------
template <typename Real>
int MeshSmoother<Real>::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* MeshSmoother<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
int MeshSmoother<Real>::GetNumTriangles () const
{
    return mNumTriangles;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* MeshSmoother<Real>::GetIndices () const
{
    return mIndices;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* MeshSmoother<Real>::GetNormals () const
{
    return mNormals;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* MeshSmoother<Real>::GetMeans () const
{
    return mMeans;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class MeshSmoother<float>;

template WM5_MATHEMATICS_ITEM
class MeshSmoother<double>;
//----------------------------------------------------------------------------
}
