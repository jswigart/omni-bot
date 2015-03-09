// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5BOUNDTREE_H
#define WM5BOUNDTREE_H

#include "Wm5PhysicsLIB.h"
#include "Wm5APoint.h"
#include "Wm5Memory.h"

// TODO: Add glue until the Mathematics library uses APoint and AVector.
#include "Wm5ApprLineFit3.h"

// Class Mesh must have the following functions in its interface.
//    int GetNumVertices () const;
//    Float3 GetPosition (int i) const;
//    int GetNumTriangles () const;
//    bool GetTriangle (int triangle, int& i0, int& i1, int& i2) const;
//    bool GetModelTriangle (int triangle, APoint* modelTriangle) const;
//    bool GetWorldTriangle (int triangle, APoint* worldTriangle) const;
//    const Transform& GetWorldTransform () const;
//
// Class Bound must have the following functions in its interface.
//    Bound ();  // default constructor
//    void ComputeFromData (int numElements, int stride, const char* data);
//    void TransformBy (const Transform& transform, Bound& bound) const;
//    bool TestIntersection (const Bound& bound) const;
//    bool TestIntersection (const Bound& bound, float tmax,
//        const AVector& velocity0, const AVector& velocity1) const;

namespace Wm5
{

template <class Mesh, class Bound>
class BoundTree
{
public:
    // Construction and destruction.
    BoundTree (const Mesh* mesh, int maxTrisPerLeaf = 1,
        bool storeInteriorTris = false);

    ~BoundTree ();

    // Tree topology.
    inline BoundTree* GetLChild ();
    inline BoundTree* GetRChild ();
    inline bool IsInteriorNode () const;
    inline bool IsLeafNode () const;

    // Member access.
    inline Mesh* GetMesh () const;
    inline const Bound& GetWorldBound () const;
    inline int GetNumTriangles () const;
    inline int GetTriangle (int i) const;
    inline const int* GetTriangles () const;

    // The Mesh world transform is assumed to change dynamically.
    void UpdateWorldBound ();

private:
    void BuildTree (int maxTrisPerLeaf, bool storeInteriorTris,
        const APoint* centroids, int i0, int i1, int* inSplit,
        int* outSplit);

    // Compute the model bound for the subset of triangles.  Return a
    // line used for splitting the projections of the triangle centroids.
    void CreateModelBound (int i0, int i1, int* inSplit, APoint& origin,
        AVector& direction);

    static void SplitTriangles (const APoint* centroids, int i0, int i1,
        int* inSplit, int& j0, int& j1, int* outSplit, const APoint& origin,
        const AVector& direction);

    // For sorting centroid projections on axes.
    class ProjectionInfo
    {
    public:
        ProjectionInfo ();
        bool operator< (const ProjectionInfo& info) const;
        int Triangle;
        float Projection;
    };

    // Mesh and bounds.
    const Mesh* mMesh;
    Bound mModelBound;
    Bound mWorldBound;

    // Binary tree representation.
    BoundTree* mLChild;
    BoundTree* mRChild;

    // If storeInteriorTris is set to 'false' in the constructor, the interior
    // nodes set the triangle quantity to zero and the array to null.  Leaf
    // nodes set the quantity to the number of triangles at that node (1, if
    // maxTrianglesPerLeaf was set to 1) and allocate an array of triangle
    // indices that are relative to the input mesh of the top level
    // constructor.
    //
    // If storeInteriorTris is set to 'true', the interior nodes also save
    // the triangle quantity and array of triangle indices for the mesh that
    // the node represents.
    int mNumTriangles;
    int* mTriangles;
};

#include "Wm5BoundTree.inl"

}

#endif
