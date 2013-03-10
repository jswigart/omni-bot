// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/02)

#ifndef WM5DELAUNAY_H
#define WM5DELAUNAY_H

// The base class for Delaunay algorithms stores the number of mesh components
// and the connectivity information for the mesh.

#include "Wm5MathematicsLIB.h"
#include "Wm5FileIO.h"
#include "Wm5Query.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Delaunay
{
public:
    // Abstract base class.
    virtual ~Delaunay ();

    // Member accessors.  For notational purposes in this class documentation,
    // The number of vertices is VQ and the vertex array is V.
    Query::Type GetQueryType () const;
    int GetNumVertices () const;
    Real GetEpsilon () const;
    bool GetOwner () const;

    // The dimension of the result, call it d.  If n is the dimension of the
    // space of the input points, then 0 <= d <= n.
    int GetDimension () const;

    // The interpretations of the return values of these functions depends on
    // the dimension.  Generally, SQ = GetSimplexQuantity() is the number of
    // simplices in the mesh.  The array returned by I = GetIndices() contains
    // SQ tuples, each tuple having d+1 elements and representing a simplex.
    // An index I[*] is relative to the vertex array V.  The array returned by
    // A = GetAdjacencies() contains SQ tuples, each tuple having d+1 elements
    // and representing those simplices adjacent to the d+1 faces of a
    // simplex.  An index A[*] is relative to the index array I.
    int GetNumSimplices () const;
    const int* GetIndices () const;
    const int* GetAdjacencies () const;

    // Dimension d = 0.
    //   SQ = 1
    //   I  = null (use index zero for vertices)
    //   A  = null (use index zero for vertices)

    // Dimension d = 1.
    //   SQ = VQ-1
    //   I  = Array of 2-tuples of indices into V that represent the
    //        segments (2*SQ total elements).
    //   A  = Array of 2-tuples of indices into I that represent the
    //        adjacent segments (2*SQ total elements).
    // The i-th segment has vertices
    //   vertex[0] = V[I[2*i+0]]
    //   vertex[1] = V[I[2*i+1]].
    // The segments adjacent to these vertices have indices
    //   adjacent[0] = A[2*i+0] is the segment sharing vertex[0]
    //   adjacent[1] = A[2*i+1] is the segment sharing vertex[1]
    // If there is no adjacent segment, the A[*] value is set to -1.  The
    // segment adjacent to vertex[j] has vertices
    //   adjvertex[0] = V[I[2*adjacent[j]+0]]
    //   adjvertex[1] = V[I[2*adjacent[j]+1]]

    // Dimension d = 2.
    //   SQ = number of triangles
    //   I  = Array of 3-tuples of indices into V that represent the
    //        triangles (3*SQ total elements).
    //   A  = Array of 3-tuples of indices into I that represent the
    //        adjacent triangles (3*SQ total elements).
    // The i-th triangle has vertices
    //   vertex[0] = V[I[3*i+0]]
    //   vertex[1] = V[I[3*i+1]]
    //   vertex[2] = V[I[3*i+2]]
    // and edge index pairs
    //   edge[0] = <I[3*i+0],I[3*i+1]>
    //   edge[1] = <I[3*i+1],I[3*i+2]>
    //   edge[2] = <I[3*i+2],I[3*i+0]>
    // The triangles adjacent to these edges have indices
    //   adjacent[0] = A[3*i+0] is the triangle sharing edge[0]
    //   adjacent[1] = A[3*i+1] is the triangle sharing edge[1]
    //   adjacent[2] = A[3*i+2] is the triangle sharing edge[2]
    // If there is no adjacent triangle, the A[*] value is set to -1.  The
    // triangle adjacent to edge[j] has vertices
    //   adjvertex[0] = V[I[3*adjacent[j]+0]]
    //   adjvertex[1] = V[I[3*adjacent[j]+1]]
    //   adjvertex[2] = V[I[3*adjacent[j]+2]]

    // Dimension d = 3.
    //   SQ = number of tetrahedra
    //   I  = Array of 4-tuples of indices into V that represent the
    //        tetrahedra (4*SQ total elements).
    //   A  = Array of 4-tuples of indices into I that represent the
    //        adjacent tetrahedra (4*SQ total elements).
    // The i-th tetrahedron has vertices
    //   vertex[0] = V[I[4*i+0]]
    //   vertex[1] = V[I[4*i+1]]
    //   vertex[2] = V[I[4*i+2]]
    //   vertex[3] = V[I[4*i+3]]
    // and face index triples listed below.  The face vertex ordering when
    // viewed from outside the tetrahedron is counterclockwise.
    //   face[0] = <I[4*i+1],I[4*i+2],I[4*i+3]>
    //   face[1] = <I[4*i+0],I[4*i+3],I[4*i+2]>
    //   face[2] = <I[4*i+0],I[4*i+1],I[4*i+3]>
    //   face[3] = <I[4*i+0],I[4*i+2],I[4*i+1]>
    // The tetrahedra adjacent to these faces have indices
    //   adjacent[0] = A[4*i+0] is the tetrahedron opposite vertex[0], so it
    //                 is the tetrahedron sharing face[0].
    //   adjacent[1] = A[4*i+1] is the tetrahedron opposite vertex[1], so it
    //                 is the tetrahedron sharing face[1].
    //   adjacent[2] = A[4*i+2] is the tetrahedron opposite vertex[2], so it
    //                 is the tetrahedron sharing face[2].
    //   adjacent[3] = A[4*i+3] is the tetrahedron opposite vertex[3], so it
    //                 is the tetrahedron sharing face[3].
    // If there is no adjacent tetrahedron, the A[*] value is set to -1.  The
    // tetrahedron adjacent to face[j] has vertices
    //   adjvertex[0] = V[I[4*adjacent[j]+0]]
    //   adjvertex[1] = V[I[4*adjacent[j]+1]]
    //   adjvertex[2] = V[I[4*adjacent[j]+2]]
    //   adjvertex[3] = V[I[4*adjacent[j]+3]]

protected:
    // Abstract base class.  The number of vertices to be processed is
    // iVQuantity.  The value of fEpsilon is a tolerance used for determining
    // the intrinsic dimension of the input set of vertices.  Ownership of the
    // input points to the constructors for the derived classes may be
    // transferred to this class.  If you want the input vertices to be
    // deleted by this class, set bOwner to 'true'; otherwise, you own the
    // array and must delete it yourself.
    Delaunay (int numVertices, Real epsilon, bool owner,
        Query::Type queryType);

    // Support for streaming to/from disk.
    bool Load (FileIO& inFile);
    bool Save (FileIO& outFile) const;

    Query::Type mQueryType;
    int mNumVertices;
    int mDimension;
    int mNumSimplices;
    int* mIndices;
    int* mAdjacencies;
    Real mEpsilon;
    bool mOwner;
};

typedef Delaunay<float> Delaunayf;
typedef Delaunay<double> Delaunayd;

}

#endif
