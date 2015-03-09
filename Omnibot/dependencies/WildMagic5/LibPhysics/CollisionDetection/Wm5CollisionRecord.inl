// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <class Mesh, class Bound>
CollisionRecord<Mesh,Bound>::CollisionRecord (BoundTree<Mesh,Bound>* tree,
    const AVector* velocity, Callback callback, void* callbackData)
    :
    mTree(tree),
    mVelocity(velocity),
    mCallback(callback),
    mCallbackData(callbackData)
{
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
CollisionRecord<Mesh,Bound>::~CollisionRecord ()
{
    delete0(mTree);
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline Mesh* CollisionRecord<Mesh,Bound>::GetMesh () const
{
    // The mesh geometry and topology cannot change, but other members might
    // need to be modified for the collision response.
    return mTree->GetMesh();
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline const AVector* CollisionRecord<Mesh,Bound>::GetVelocity () const
{
    return mVelocity;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
inline const void* CollisionRecord<Mesh,Bound>::GetCallbackData () const
{
    return mCallbackData;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionRecord<Mesh,Bound>::TestIntersection (CollisionRecord& record)
{
    // Convenience variables.
    BoundTree<Mesh,Bound>* tree0 = mTree;
    BoundTree<Mesh,Bound>* tree1 = record.mTree;
    const Mesh* mesh0 = mTree->GetMesh();
    const Mesh* mesh1 = record.mTree->GetMesh();

    tree0->UpdateWorldBound();
    tree1->UpdateWorldBound();

    const Bound& worldBound0 = tree0->GetWorldBound();
    const Bound& worldBound1 = tree1->GetWorldBound();
    if (worldBound0.TestIntersection(worldBound1))
    {
        BoundTree<Mesh,Bound>* root;

        if (tree0->IsInteriorNode())
        {
            root = mTree;

            // Compare Tree0.L to Tree1.
            mTree = root->GetLChild();
            TestIntersection(record);

            // Compare Tree0.R to Tree1.
            mTree = root->GetRChild();
            TestIntersection(record);

            mTree = root;
        }
        else if (tree1->IsInteriorNode())
        {
            root = record.mTree;

            // Compare Tree0 to Tree1.L.
            record.mTree = root->GetLChild();
            TestIntersection(record);

            // Compare Tree0 to Tree1.R.
            record.mTree = root->GetRChild();
            TestIntersection(record);

            record.mTree = root;
        }
        else
        {
            // At a leaf in each tree.
            int numTriangles0 = tree0->GetNumTriangles();
            for (int i0 = 0; i0 < numTriangles0; ++i0)
            {
                int t0 = tree0->GetTriangle(i0);

                // Get world space triangle.
                APoint wvert0[3];
                mesh0->GetWorldTriangle(t0, wvert0);

                // TODO: Add glue until the Mathematics library uses
                // APoint and AVector.
                Vector3f tri0[3] =
                {
                    Vector3f(wvert0[0][0], wvert0[0][1], wvert0[0][2]),
                    Vector3f(wvert0[1][0], wvert0[1][1], wvert0[1][2]),
                    Vector3f(wvert0[2][0], wvert0[2][1], wvert0[2][2])
                };

                int numTriangles1 = tree1->GetNumTriangles();
                for (int i1 = 0; i1 < numTriangles1; ++i1)
                {
                    int t1 = tree1->GetTriangle(i1);

                    // Get world space triangle.
                    APoint wvert1[3];
                    mesh1->GetWorldTriangle(t1, wvert1);

                    // TODO: Add glue until the Mathematics library uses
                    // APoint and AVector.
                    Vector3f tri1[3] =
                    {
                        Vector3f(wvert1[0][0], wvert1[0][1], wvert1[0][2]),
                        Vector3f(wvert1[1][0], wvert1[1][1], wvert1[1][2]),
                        Vector3f(wvert1[2][0], wvert1[2][1], wvert1[2][2])
                    };

                    IntrTriangle3Triangle3<float> calc(tri0, tri1);
                    if (calc.Test())
                    {
                        if (mCallback)
                        {
                            mCallback(*this, t0, record, t1, &calc);
                        }

                        if (record.mCallback)
                        {
                            record.mCallback(record, t1, *this, t0, &calc);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionRecord<Mesh,Bound>::FindIntersection (CollisionRecord& record)
{
    // Convenience variables.
    BoundTree<Mesh,Bound>* tree0 = mTree;
    BoundTree<Mesh,Bound>* tree1 = record.mTree;
    const Mesh* mesh0 = mTree->GetMesh();
    const Mesh* mesh1 = record.mTree->GetMesh();

    tree0->UpdateWorldBound();
    tree1->UpdateWorldBound();

    const Bound& worldBound0 = tree0->GetWorldBound();
    const Bound& worldBound1 = tree1->GetWorldBound();
    if (worldBound0.TestIntersection(worldBound1))
    {
        BoundTree<Mesh,Bound>* root;

        if (tree0->IsInteriorNode())
        {
            root = mTree;

            // Compare Tree0.L to Tree1.
            mTree = root->GetLChild();
            FindIntersection(record);

            // Compare Tree0.R to Tree1.
            mTree = root->GetRChild();
            FindIntersection(record);

            mTree = root;
        }
        else if (tree1->IsInteriorNode())
        {
            root = record.mTree;

            // Compare Tree0 to Tree1.L.
            record.mTree = root->GetLChild();
            FindIntersection(record);

            // Compare Tree0 to Tree1.R.
            record.mTree = root->GetRChild();
            FindIntersection(record);

            record.mTree = root;
        }
        else
        {
            // At a leaf in each tree.
            int numTriangles0 = tree0->GetNumTriangles();
            for (int i0 = 0; i0 < numTriangles0; ++i0)
            {
                int t0 = tree0->GetTriangle(i0);

                // Get world space triangle.
                APoint wvert0[3];
                mesh0->GetWorldTriangle(t0, wvert0);

                // TODO: Add glue until the Mathematics library uses
                // APoint and AVector.
                Vector3f tri0[3] =
                {
                    Vector3f(wvert0[0][0], wvert0[0][1], wvert0[0][2]),
                    Vector3f(wvert0[1][0], wvert0[1][1], wvert0[1][2]),
                    Vector3f(wvert0[2][0], wvert0[2][1], wvert0[2][2])
                };

                int numTriangles1 = tree1->GetNumTriangles();
                for (int i1 = 0; i1 < numTriangles1; ++i1)
                {
                    int t1 = tree1->GetTriangle(i1);

                    // Get world space triangle.
                    APoint wvert1[3];
                    mesh1->GetWorldTriangle(t1, wvert1);

                    // TODO: Add glue until the Mathematics library uses
                    // APoint and AVector.
                    Vector3f tri1[3] =
                    {
                        Vector3f(wvert1[0][0], wvert1[0][1], wvert1[0][2]),
                        Vector3f(wvert1[1][0], wvert1[1][1], wvert1[1][2]),
                        Vector3f(wvert1[2][0], wvert1[2][1], wvert1[2][2])
                    };

                    IntrTriangle3Triangle3<float> calc(tri0, tri1);
                    if (calc.Find())
                    {
                        if (mCallback)
                        {
                            mCallback(*this, t0, record, t1, &calc);
                        }

                        if (record.mCallback)
                        {
                            record.mCallback(record, t1, *this, t0, &calc);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionRecord<Mesh,Bound>::TestIntersection (float tmax,
    CollisionRecord& record)
{
    // Convenience variables.
    BoundTree<Mesh,Bound>* tree0 = mTree;
    BoundTree<Mesh,Bound>* tree1 = record.mTree;
    const Mesh* mesh0 = mTree->GetMesh();
    const Mesh* mesh1 = record.mTree->GetMesh();
    const AVector& wvel0 =
        (mVelocity ? *mVelocity : AVector::ZERO);
    const AVector& wvel1 =
        (record.mVelocity ? *record.mVelocity : AVector::ZERO);

    // TODO: Add glue until the Mathematics library uses APoint and AVector.
    Vector3f velocity0(wvel0[0], wvel0[1], wvel0[2]);
    Vector3f velocity1(wvel1[0], wvel1[1], wvel1[2]);

    tree0->UpdateWorldBound();
    tree1->UpdateWorldBound();

    const Bound& worldBound0 = tree0->GetWorldBound();
    const Bound& worldBound1 = tree1->GetWorldBound();

    if (worldBound0.TestIntersection(worldBound1, tmax, velocity0, velocity1))
    {
        BoundTree<Mesh,Bound>* root;

        if (tree0->IsInteriorNode())
        {
            root = mTree;

            // Compare Tree0.L to Tree1.
            mTree = root->GetLChild();
            TestIntersection(tmax, record);

            // Compare Tree0.R to Tree1.
            mTree = root->GetRChild();
            TestIntersection(tmax, record);

            mTree = root;
        }
        else if (tree1->IsInteriorNode())
        {
            root = record.mTree;

            // Compare Tree0 to Tree1.L.
            record.mTree = root->GetLChild();
            TestIntersection(tmax, record);

            // Compare Tree0 to Tree1.R.
            record.mTree = root->GetRChild();
            TestIntersection(tmax, record);

            record.mTree = root;
        }
        else
        {
            // At a leaf in each tree.
            int numTriangles0 = tree0->GetNumTriangles();
            for (int i0 = 0; i0 < numTriangles0; ++i0)
            {
                int t0 = tree0->GetTriangle(i0);

                // Get world space triangle.
                APoint wvert0[3];
                mesh0->GetWorldTriangle(t0, wvert0);

                // TODO: Add glue until the Mathematics library uses
                // APoint and AVector.
                Vector3f tri0[3] =
                {
                    Vector3f(wvert0[0][0], wvert0[0][1], wvert0[0][2]),
                    Vector3f(wvert0[1][0], wvert0[1][1], wvert0[1][2]),
                    Vector3f(wvert0[2][0], wvert0[2][1], wvert0[2][2])
                };

                int numTriangles1 = tree1->GetNumTriangles();
                for (int i1 = 0; i1 < numTriangles1; ++i1)
                {
                    int t1 = tree1->GetTriangle(i1);

                    // Get world space triangle.
                    APoint wvert1[3];
                    mesh1->GetWorldTriangle(t1, wvert1);

                    // TODO: Add glue until the Mathematics library uses
                    // APoint and AVector.
                    Vector3f tri1[3] =
                    {
                        Vector3f(wvert1[0][0], wvert1[0][1], wvert1[0][2]),
                        Vector3f(wvert1[1][0], wvert1[1][1], wvert1[1][2]),
                        Vector3f(wvert1[2][0], wvert1[2][1], wvert1[2][2])
                    };

                    IntrTriangle3Triangle3<float> calc(tri0, tri1);
                    if (calc.Test(tmax, velocity0, velocity1))
                    {
                        if (mCallback)
                        {
                            mCallback(*this, t0, record, t1, &calc);
                        }

                        if (record.mCallback)
                        {
                            record.mCallback(record, t1, *this, t0, &calc);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionRecord<Mesh,Bound>::FindIntersection (float tmax,
    CollisionRecord& record)
{
    // Convenience variables.
    BoundTree<Mesh,Bound>* tree0 = mTree;
    BoundTree<Mesh,Bound>* tree1 = record.mTree;
    const Mesh* mesh0 = mTree->GetMesh();
    const Mesh* mesh1 = record.mTree->GetMesh();
    const AVector& wvel0 =
        (mVelocity ? *mVelocity : AVector::ZERO);
    const AVector& wvel1 =
        (record.mVelocity ? *record.mVelocity : AVector::ZERO);

    // TODO: Add glue until the Mathematics library uses APoint and AVector.
    Vector3f velocity0(wvel0[0], wvel0[1], wvel0[2]);
    Vector3f velocity1(wvel1[0], wvel1[1], wvel1[2]);

    tree0->UpdateWorldBound();
    tree1->UpdateWorldBound();

    const Bound& worldBound0 = tree0->GetWorldBound();
    const Bound& worldBound1 = tree1->GetWorldBound();

    if (worldBound0.TestIntersection(worldBound1, tmax, velocity0, velocity1))
    {
        BoundTree<Mesh,Bound>* root;

        if (tree0->IsInteriorNode())
        {
            root = mTree;

            // Compare Tree0.L to Tree1.
            mTree = root->GetLChild();
            FindIntersection(tmax, record);

            // Compare Tree0.R to Tree1.
            mTree = root->GetRChild();
            FindIntersection(tmax, record);

            mTree = root;
        }
        else if ( tree1->IsInteriorNode() )
        {
            root = record.mTree;

            // Compare Tree0 to Tree1.L.
            record.mTree = root->GetLChild();
            FindIntersection(tmax, record);

            // compare Tree0 to Tree1.R
            record.mTree = root->GetRChild();
            FindIntersection(tmax, record);

            record.mTree = root;
        }
        else
        {
            // At a leaf in each tree.
            int numTriangles0 = tree0->GetNumTriangles();
            for (int i0 = 0; i0 < numTriangles0; ++i0)
            {
                int t0 = tree0->GetTriangle(i0);

                // Get world space triangle.
                APoint wvert0[3];
                mesh0->GetWorldTriangle(t0, wvert0);

                // TODO: Add glue until the Mathematics library uses
                // APoint and AVector.
                Vector3f tri0[3] =
                {
                    Vector3f(wvert0[0][0], wvert0[0][1], wvert0[0][2]),
                    Vector3f(wvert0[1][0], wvert0[1][1], wvert0[1][2]),
                    Vector3f(wvert0[2][0], wvert0[2][1], wvert0[2][2])
                };

                int numTriangles1 = tree1->GetNumTriangles();
                for (int i1 = 0; i1 < numTriangles1; ++i1)
                {
                    int t1 = tree1->GetTriangle(i1);

                    // Get world space triangle.
                    APoint wvert1[3];
                    mesh1->GetWorldTriangle(t1, wvert1);

                    // TODO: Add glue until the Mathematics library uses
                    // APoint and AVector.
                    Vector3f tri1[3] =
                    {
                        Vector3f(wvert1[0][0], wvert1[0][1], wvert1[0][2]),
                        Vector3f(wvert1[1][0], wvert1[1][1], wvert1[1][2]),
                        Vector3f(wvert1[2][0], wvert1[2][1], wvert1[2][2])
                    };

                    IntrTriangle3Triangle3<float> calc(tri0, tri1);
                    if (calc.Find(tmax, velocity0, velocity1))
                    {
                        if (mCallback)
                        {
                            mCallback(*this, t0, record, t1, &calc);
                        }

                        if (record.mCallback)
                        {
                            record.mCallback(record, t1, *this, t0, &calc);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
