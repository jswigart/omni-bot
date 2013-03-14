// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DELAUNAY1_H
#define WM5DELAUNAY1_H

// A fancy class to sort a collection of real-valued numbers, but this
// provides some convenience for Delaunay2 and Delaunay3 when the input point
// set has intrinsic dimension smaller than the containing space.  The
// interface of Delaunay1 is also the model for those of Delaunay2 and
// Delaunay3.

#include "Wm5MathematicsLIB.h"
#include "Wm5Delaunay.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Delaunay1 : public Delaunay<Real>
{
public:
    // The input to the constructor is the array of vertices you want to sort.
    // If you want Delaunay1 to delete the array during destruction, set
    // 'owner' to 'true'.  Otherwise, you own the array and must delete it
    // yourself.  TO DO:  The query type is currently ignored by this class.
    // Add support for the various types later.
    Delaunay1 (int numVertices, Real* vertices, Real epsilon,  bool owner,
        Query::Type queryType);
    virtual ~Delaunay1 ();

    // The input vertex array.
    const Real* GetVertices () const;

    // The functions listed here are valid only for dimension 1.

    // The convex hull of the vertices is an interval.  This function returns
    // the indices of the vertices that form the interval.  The return value
    // is 'true' iff the dimension is 1.
    bool GetHull (int hull[2]);

    // Support for searching the sorted vertices for a interval that contains
    // P.  If there is a containing interval, the returned value is a index i
    // into the GetIndices() array with 0 <= i < GetSimplexQuantity().  If
    // there is not a containing segment, -1 is returned.
    int GetContainingSegment (const Real p) const;

    // Get the vertices for segment i.  The function returns 'true' if i is a
    // valid segment index, in which case the vertices are valid.  Otherwise,
    // the function returns 'false' and the vertices are invalid.
    bool GetVertexSet (int i, Real vertices[2]) const;

    // Get the vertex indices for segment i.  The function returns 'true' if
    // i is a valid segment index, in which case the vertices are valid.
    // Otherwise, the function returns 'false' and the vertices are invalid.
    bool GetIndexSet (int i, int indices[2]) const;

    // Get the indices for segments adjacent to segment i.  The function
    // returns 'true' if i is a valid segment index, in which case the
    // adjacencies are valid.  Otherwise, the function returns 'false' and
    // the adjacencies are invalid.
    bool GetAdjacentSet (int i, int adjacencies[2]) const;

    // Compute the barycentric coordinates of P with respect to segment i.
    // The function returns 'true' if i is a valid segment index, in which
    // case the coordinates are valid.  Otherwise, the function returns
    // 'false' and the coordinate array is invalid.
    bool GetBarycentricSet (int i, const Real p, Real bary[2]) const;

    // Support for streaming to/from disk.
    Delaunay1 (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Load (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Save (const char* filename, int mode = FileIO::FM_DEFAULT_WRITE)
        const;

private:
    using Delaunay<Real>::mNumVertices;
    using Delaunay<Real>::mDimension;
    using Delaunay<Real>::mNumSimplices;
    using Delaunay<Real>::mIndices;
    using Delaunay<Real>::mAdjacencies;
    using Delaunay<Real>::mEpsilon;
    using Delaunay<Real>::mOwner;

    class SortedVertex
    {
    public:
        Real Value;
        int Index;

        inline bool operator< (const SortedVertex& vertex) const
        {
            return Value < vertex.Value;
        }
    };

    Real* mVertices;
};

typedef Delaunay1<float> Delaunay1f;
typedef Delaunay1<double> Delaunay1d;

}

#endif
