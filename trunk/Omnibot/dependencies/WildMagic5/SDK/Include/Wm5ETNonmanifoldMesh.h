// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ETNONMANIFOLDMESH_H
#define WM5ETNONMANIFOLDMESH_H

#include "Wm5MathematicsLIB.h"
#include "Wm5EdgeKey.h"
#include "Wm5TriangleKey.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM ETNonmanifoldMesh
{
public:
    // Edge data types.
    class Edge;
    typedef Edge* EPtr;
    typedef const Edge* ECPtr;
    typedef EPtr (*ECreator)(int,int);
    typedef std::map<EdgeKey,Edge*> EMap;
    typedef EMap::iterator EMapIterator;
    typedef EMap::const_iterator EMapCIterator;

    // Triangle data types.
    class Triangle;
    typedef Triangle* TPtr;
    typedef const Triangle* TCPtr;
    typedef TPtr (*TCreator)(int,int,int);
    typedef std::map<TriangleKey,Triangle*> TMap;
    typedef TMap::iterator TMapIterator;
    typedef TMap::const_iterator TMapCIterator;

    // Edge object.
    class WM5_MATHEMATICS_ITEM Edge
    {
    public:
        Edge (int v0, int v1);
        virtual ~Edge ();

        int V[2];
        std::set<Triangle*> T;
    };

    class WM5_MATHEMATICS_ITEM Triangle
    {
    public:
        Triangle (int v0, int v1, int v2);
        virtual ~Triangle ();

        // vertices (V[0],V[1],V[2])
        int V[3];

        // adjacent edges
        // E[0] represents edge (V[0],V[1])
        // E[1] represents edge (V[1],V[2])
        // E[2] represents edge (V[2],V[0])
        EPtr E[3];
    };


    ETNonmanifoldMesh (ECreator eCreator = 0, TCreator tCreator = 0);
    virtual ~ETNonmanifoldMesh ();

    // Member access.
    inline int GetNumEdges () const;
    inline const EMap& GetEdges () const;
    inline int GetNumTriangles () const;
    inline const TMap& GetTriangles () const;

    // Mesh manipulation.
    TPtr InsertTriangle (int v0, int v1, int v2);
    bool RemoveTriangle (int v0, int v1, int v2);

    // Topological operations.
    bool IsManifold () const;
    bool IsClosed () const;
    bool IsConnected () const;

    // Extract a connected component from the mesh and remove all the
    // triangles of the component from the mesh.  This is useful for computing
    // the components in a very large mesh that uses a lot of memory.  The
    // intention is that the function is called until all components are
    // found.  The typical code is
    //
    //     ETNonmanifoldMesh mesh = <some mesh>;
    //     int numTotalIndices = 3*mesh.GetNumTriangles();
    //     int* indices = new1<int>(numTotalIndices);
    //     for (int numIndices = 0; numIndices < numTotalIndices; /**/ )
    //     {
    //         int currNumIndices;
    //         int* currIndices = indices + numIndices;
    //         mesh.RemoveComponent(currNumIndices, currIndices);
    //         numIndices += currNumIndices;
    //     }
    void RemoveComponent (int& numIndices, int* indices);

    void Print (const char* filename);

protected:
    static EPtr CreateEdge (int v0, int v1);
    ECreator mECreator;
    EMap mEMap;

    static TPtr CreateTriangle (int v0, int v1, int v2);
    TCreator mTCreator;
    TMap mTMap;
};

#include "Wm5ETNonmanifoldMesh.inl"

}

#endif
