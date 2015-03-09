// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ETMANIFOLDMESH_H
#define WM5ETMANIFOLDMESH_H

#include "Wm5MathematicsLIB.h"
#include "Wm5EdgeKey.h"
#include "Wm5TriangleKey.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM ETManifoldMesh
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
        TPtr T[2];
    };

    // Triangle object.
    class WM5_MATHEMATICS_ITEM Triangle
    {
    public:
        Triangle (int v0, int v1, int v2);
        virtual ~Triangle ();

        // Vertices, listed in counterclockwise order (V[0],V[1],V[2]).
        int V[3];

        // Adjacent edges:
        //   E[0] points to edge (V[0],V[1])
        //   E[1] points to edge (V[1],V[2])
        //   E[2] points to edge (V[2],V[0])
        EPtr E[3];

        // Adjacent triangles:
        //   T[0] points to triangle sharing edge E[0]
        //   T[1] points to triangle sharing edge E[1]
        //   T[2] points to triangle sharing edge E[2]
        TPtr T[3];
    };


    // Construction and destruction.
    ETManifoldMesh (ECreator eCreator = 0, TCreator tCreator = 0);
    virtual ~ETManifoldMesh ();

    // Member access.
    inline const EMap& GetEdges () const;
    inline const TMap& GetTriangles () const;

    // Mesh manipulation.
    TPtr InsertTriangle (int v0, int v1, int v2);
    bool RemoveTriangle (int v0, int v1, int v2);

    // Manifold mesh is closed if each edge is shared twice.
    bool IsClosed () const;

    // For debugging.
    void Print (const char* acFilename);

protected:
    // The edge data.
    static EPtr CreateEdge (int v0, int v1);
    ECreator mECreator;
    EMap mEMap;

    // The triangle data.
    static TPtr CreateTriangle (int v0, int v1, int v2);
    TCreator mTCreator;
    TMap mTMap;
};

#include "Wm5ETManifoldMesh.inl"

}

#endif
