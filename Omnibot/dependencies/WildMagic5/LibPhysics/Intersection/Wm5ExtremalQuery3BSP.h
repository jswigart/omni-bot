// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5EXTREMALQUERY3BSP_H
#define WM5EXTREMALQUERY3BSP_H

#include "Wm5PhysicsLIB.h"
#include "Wm5ExtremalQuery3.h"
#include "Wm5BasicMesh.h"

namespace Wm5
{

template <typename Real>
class WM5_PHYSICS_ITEM ExtremalQuery3BSP : public ExtremalQuery3<Real>
{
public:
    ExtremalQuery3BSP (const ConvexPolyhedron3<Real>* polytope);
    virtual ~ExtremalQuery3BSP ();

    // Compute the extreme vertices in the specified direction and return the
    // indices of the vertices in the polyhedron vertex array.
    virtual void GetExtremeVertices (const Vector3<Real>& direction,
        int& positiveDirection, int& negativeDirection);

    // Tree statistics.
    int GetNumNodes () const;
    int GetTreeDepth () const;

private:
    using ExtremalQuery3<Real>::mPolytope;
    using ExtremalQuery3<Real>::mFaceNormals;

    class SphericalArc
    {
    public:
        // Construction.
        SphericalArc ();

        // The arcs are stored in a multiset ordered by increasing separation.
        // The multiset will be traversed in reverse order.  This heuristic is
        // designed to create BSP trees whose top-most nodes can eliminate as
        // many arcs as possible during an extremal query.
        bool operator< (const SphericalArc& arc) const;

        // Indices N[] into the face normal array for the endpoints of the
        // arc.
        int NIndex[2];

        // The number of arcs in the path from normal N[0] to normal N[1].
        // For spherical polygon edges, the number is 1.  The number is 2 or
        // larger for bisector arcs of the spherical polygon.
        int Separation;

        // The normal is Cross(FaceNormal[N[0]],FaceNormal[N[1]]).
        Vector3<Real> Normal;

        // Indices into the vertex array for the extremal points for the
        // two regions sharing the arc.  As the arc is traversed from normal
        // N[0] to normal N[1], PosVertex is the index for the extreme vertex
        // to the left of the arc and NegVertex is the index for the extreme
        // vertex to the right of the arc.
        int PosVertex, NegVertex;

        // Support for BSP trees stored as contiguous nodes in an array.
        int PosChild, NegChild;
    };

    void SortVertexAdjacents (BasicMesh& mesh);

    void CreateSphericalArcs (BasicMesh& mesh,
        std::multiset<SphericalArc>& arcs);

    void CreateSphericalBisectors (BasicMesh& mesh,
        std::multiset<SphericalArc>& arcs);

    void CreateBSPTree (std::multiset<SphericalArc>& arcs,
        std::vector<SphericalArc>& nodes);

    void InsertArc (const SphericalArc& arcs,
        std::vector<SphericalArc>& nodes);

    // Fixed-size storage for the BSP nodes.
    int mNumNodes;
    SphericalArc* mNodes;
    int mTreeDepth;
};

typedef ExtremalQuery3BSP<float> ExtremalQuery3BSPf;
typedef ExtremalQuery3BSP<double> ExtremalQuery3BSPd;

}

#endif
