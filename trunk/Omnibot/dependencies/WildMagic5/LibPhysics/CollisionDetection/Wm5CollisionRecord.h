// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5COLLISIONRECORD_H
#define WM5COLLISIONRECORD_H

#include "Wm5PhysicsLIB.h"
#include "Wm5BoundTree.h"

// TODO: Add glue until the Mathematics library uses APoint and AVector.
#include "Wm5IntrTriangle3Triangle3.h"

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
class CollisionRecord
{
public:
    typedef void (*Callback) (
        CollisionRecord& record0, int t0,
        CollisionRecord& record1, int t1,
        Intersector<float,Vector3f>* intersector);

    // Construction and destruction.  The input 'tree' must be dynamically
    // allocated and created using 'storeInteriorTris = true'.
    // CollisionRecord assumes responsibility for deleting 'tree'.
    CollisionRecord (BoundTree<Mesh,Bound>* tree, const AVector* velocity,
        Callback callback, void* callbackData);

    ~CollisionRecord ();

    // Member access.
    inline Mesh* GetMesh () const;
    inline const AVector* GetVelocity () const;
    inline const void* GetCallbackData () const;

    // Intersection queries.  See the comments in class CollisionGroup about
    // what information is available to the application via the callback
    // function.
    void TestIntersection (CollisionRecord& record);
    void FindIntersection (CollisionRecord& record);
    void TestIntersection (float tmax, CollisionRecord& record);
    void FindIntersection (float tmax ,CollisionRecord& record);

protected:
    BoundTree<Mesh,Bound>* mTree;
    const AVector* mVelocity;
    Callback mCallback;
    void* mCallbackData;
};

#include "Wm5CollisionRecord.inl"

}

#endif
