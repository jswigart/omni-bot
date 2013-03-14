// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/29)

#ifndef WM5UNIQUEVERTICESTRIANGLES_H
#define WM5UNIQUEVERTICESTRIANGLES_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Memory.h"
#include "Wm5Tuple.h"

namespace Wm5
{

template <int N, typename Real>
class UniqueVerticesTriangles
{
public:
    // Construction and destruction.  A vertex is an N-tuple of Real values,
    // usually starting with position and optionally followed by attributes
    // such as normal vector, colors, and texture coordinates.
    //
    // TO DO:  Allow the user to specify an epsilon e > 0 so that vertices V0
    // and V1 are considered to be the same when |V1-V0| <= e.  The current
    // code uses e = 0.
    
    // Triangle soup.  The input vertex array consists of triples of vertices,
    // each triple representing a triangle.  The array 'inVertices' must have
    // 3*numTriangles tuples.  The caller is responsible for deleting the
    // input vertex array if it was dynamically allocated.  An array
    // 'outVertices' of 'numOutVertices' unique vertices and an array
    // 'outIndices' of 'numTriangles' unique index triples are computed.  The
    // indices are relative to the array of unique vertices and each index
    // triple represents a triangle.  The output arrays are dynamically
    // allocated.  The caller is responsible for deleting them.
    UniqueVerticesTriangles (int numTriangles,
        const Tuple<N,Real>* inVertices, int& numOutVertices,
        Tuple<N,Real>*& outVertices, int*& outIndices);

    // Indexed triangles.  The input vertex array consists of all vertices
    // referenced by the input index array.  The array 'inVertices' must have
    // 'numInVertices' tuples.  The array 'inIndices' must have 3*numTriangles
    // elements.  The caller is responsible for deleting the input arrays if
    // they were dynamically allocated.  An array 'outVertices' of
    // 'numOutVertices' unique vertices and an array 'outIndices' of
    // 'numTriangles' unique index triples are computed.  The indices are
    // relative to the array of unique vertices and each index triple
    // represents a triangle.  The output arrays are dynamically allocated.
    // The caller is responsible for deleting them.
    UniqueVerticesTriangles (int numInVertices,
        const Tuple<N,Real>* inVertices, int numTriangles,
        const int* inIndices, int& numOutVertices,
        Tuple<N,Real>*& outVertices, int*& outIndices);

    ~UniqueVerticesTriangles ();

    // The input vertices have indices 0 <= i < VInNum.  The output vertices
    // have indices 0 <= j < VOutNum.  The construction leads to a mapping of
    // input indices i to output indices j.  Duplicate vertices have different
    // input indices but the same output index.  The following function gives
    // you access to the mapping.  If the input index is invalid (i < 0 or
    // i >= VINum), the return value is -1.
    int GetOutputIndexFor (int inputIndex) const;

private:
    void ConstructUniqueVertices (int numInVertices,
        const Tuple<N,Real>* inVertices, int& numOutVertices,
        Tuple<N,Real>*& outVertices);

    int mNumInVertices, mNumOutVertices;
    int* mInToOutMapping;
};

#include "Wm5UniqueVerticesTriangles.inl"

}

#endif
