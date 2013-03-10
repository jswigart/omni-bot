// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <class Mesh, class Bound>
BoundTree<Mesh,Bound>::BoundTree (const Mesh* mesh, int maxTrisPerLeaf,
    bool storeInteriorTris)
    :
    mMesh(mesh),
    mLChild(0),
    mRChild(0),
    mNumTriangles(0),
    mTriangles(0)
{
    if (maxTrisPerLeaf == 0)
    {
        // This is a signal to defer construction.  This behavior is needed
        // in the BuildTree function.
        return;
    }

    // Centroids of triangles are used for splitting a mesh.  The centroids
    // are projected onto a splitting axis and sorted.  The split is based
    // on the median of the projections.
    int numTriangles = mMesh->GetNumTriangles();
    APoint* centroids = new1<APoint>(numTriangles);
    const float oneThird = 1.0f/3.0f;
    int t, i;
    for (t = 0, i = 0; t < numTriangles; ++t)
    {
        APoint vertex[3];
        mMesh->GetModelTriangle(t, vertex);
        centroids[t] = oneThird*(vertex[0] + vertex[1] + vertex[2]);
    }

    // Initialize binary-tree arrays for storing triangle indices.  These
    // are used to store the indices when the mesh is split.
    int* inSplit = new1<int>(numTriangles);
    int* outSplit = new1<int>(numTriangles);
    for (t = 0; t < numTriangles; ++t)
    {
        inSplit[t] = t;
    }

    BuildTree(maxTrisPerLeaf, storeInteriorTris, centroids, 0, numTriangles-1,
        inSplit, outSplit);

    delete1(centroids);
    delete1(inSplit);
    delete1(outSplit);
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
BoundTree<Mesh,Bound>::~BoundTree ()
{
    delete0(mLChild);
    delete0(mRChild);
    delete1(mTriangles);
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline BoundTree<Mesh,Bound>* BoundTree<Mesh,Bound>::GetLChild ()
{
    return mLChild;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline BoundTree<Mesh,Bound>* BoundTree<Mesh,Bound>::GetRChild ()
{
    return mRChild;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline bool BoundTree<Mesh,Bound>::IsInteriorNode () const
{
    return mLChild || mRChild;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline bool BoundTree<Mesh,Bound>::IsLeafNode () const
{
    return !mLChild && !mRChild;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline Mesh* BoundTree<Mesh,Bound>::GetMesh () const
{
    // The mesh geometry and topology cannot change, but other members might
    // need to be modified for the collision response.
    return (Mesh*)mMesh;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline const Bound& BoundTree<Mesh,Bound>::GetWorldBound () const
{
    return mWorldBound;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline int BoundTree<Mesh,Bound>::GetNumTriangles () const
{
    return mNumTriangles;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline int BoundTree<Mesh,Bound>::GetTriangle (int i) const
{
    return mTriangles[i];
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline const int* BoundTree<Mesh,Bound>::GetTriangles () const
{
    return mTriangles;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void BoundTree<Mesh,Bound>::UpdateWorldBound ()
{
     mModelBound.TransformBy(mMesh->GetWorldTransform(), mWorldBound);
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void BoundTree<Mesh,Bound>::BuildTree (int maxTrisPerLeaf,
    bool storeInteriorTris, const APoint* centroids, int i0, int i1,
    int* inSplit, int* outSplit)
{
    assertion(i0 <= i1, "Invalid index ordering\n");

    APoint origin;
    AVector direction;
    CreateModelBound(i0, i1, inSplit, origin, direction);

    if (i1 - i0 < maxTrisPerLeaf)
    {
        // At a leaf node.
        mNumTriangles = i1 - i0 + 1;
        mTriangles = new1<int>(mNumTriangles);
        memcpy(mTriangles, &inSplit[i0], mNumTriangles*sizeof(int));

        mLChild = 0;
        mRChild = 0;
    }
    else
    {
        // At an interior node.
        if (storeInteriorTris)
        {
            mNumTriangles = i1 - i0 + 1;
            mTriangles = new1<int>(mNumTriangles);
            memcpy(mTriangles, &inSplit[i0], mNumTriangles*sizeof(int));
        }
        else
        {
            mNumTriangles = 0;
            mTriangles = 0;
        }

        int j0, j1;
        SplitTriangles(centroids, i0, i1, inSplit, j0, j1, outSplit, origin,
            direction);

        mLChild = new0 BoundTree(mMesh, 0, false);
        mLChild->BuildTree(maxTrisPerLeaf, storeInteriorTris, centroids,
            i0, j0, outSplit, inSplit);

        mRChild = new0 BoundTree(mMesh, 0, false);
        mRChild->BuildTree(maxTrisPerLeaf, storeInteriorTris, centroids,
            j1, i1, outSplit, inSplit);
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void BoundTree<Mesh,Bound>::CreateModelBound (int i0, int i1, int* inSplit,
    APoint& origin, AVector& direction)
{
    // Tag vertices that are used in the submesh.
    int numVertices = mMesh->GetNumVertices();
    int* valid = new1<int>(numVertices);
    memset(valid, 0, numVertices*sizeof(int));
    int i;
    for (i = i0; i <= i1; ++i)
    {
        int v0, v1, v2;
        mMesh->GetTriangle(inSplit[i], v0, v1, v2);
        valid[v0] = 1;
        valid[v1] = 1;
        valid[v2] = 1;
    }

    // Create a contiguous set of vertices in the submesh.
    std::vector<Float3> meshVertices;
    for (i = 0; i < numVertices; ++i)
    {
        if (valid[i])
        {
            meshVertices.push_back(mMesh->GetPosition(i));
        }
    }
    delete1(valid);

    // Compute the bound for the submesh.
    int numSubvertices = (int)meshVertices.size();
    mModelBound.ComputeFromData(numSubvertices, 3*sizeof(float),
        (const char*)&meshVertices[0]);

    // Compute a splitting line for the submesh.
    Line3f line = OrthogonalLineFit3<float>(numSubvertices,
        (const Vector3f*)&meshVertices[0]);

    origin = APoint(line.Origin);
    direction = AVector(line.Direction);
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void BoundTree<Mesh,Bound>::SplitTriangles (const APoint* centroids,
    int i0, int i1, int* inSplit, int& j0, int& j1, int* outSplit,
    const APoint& origin, const AVector& direction)
{
    // Project onto specified line.
    int quantity = i1 - i0 + 1;
    std::vector<ProjectionInfo> info(quantity);
    int i, k;
    for (i = i0, k = 0; i <= i1; ++i, ++k)
    {
        int triangle = inSplit[i];
        AVector diff = centroids[triangle] - origin;
        info[k].Triangle = triangle;
        info[k].Projection = direction.Dot(diff);
    }

    // Find median of projections by sorting.
    std::sort(info.begin(), info.end());
    int median = (quantity - 1)/2;

    // Partition the triangles by the median.
    for (k = 0, j0 = i0 - 1; k <= median; ++k)
    {
        outSplit[++j0] = info[k].Triangle;
    }
    for (j1 = i1 + 1; k < quantity; ++k)
    {
        outSplit[--j1] = info[k].Triangle;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BoundTree::ProjectionInfo
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
BoundTree<Mesh,Bound>::ProjectionInfo::ProjectionInfo ()
    :
    Triangle(0),
    Projection(0.0f)
{
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
bool BoundTree<Mesh,Bound>::ProjectionInfo::operator< (
    const ProjectionInfo& info) const
{
     return Projection < info.Projection;
}
//----------------------------------------------------------------------------
