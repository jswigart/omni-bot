// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONVEXHULL1_H
#define WM5CONVEXHULL1_H

// A fancy class to compute the minimum and maximum of a collection of
// real-valued numbers, but this provides some convenience for ConvexHull2 and
// ConvexHull3 when the input point set has intrinsic dimension smaller than
// the containing space.  The interface of ConvexHull1 is also the model for
// those of ConvexHull2 and ConvexHull3.

#include "Wm5MathematicsLIB.h"
#include "Wm5ConvexHull.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM ConvexHull1 : public ConvexHull<Real>
{
public:
    // The input to the constructor is the array of vertices you want to sort.
    // If you want ConvexHull1 to delete the array during destruction, set
    // 'owner' to 'true'.  Otherwise, you own the array and must delete it
    // yourself.  TO DO:  The computation type is currently ignored by this
    // class.  Add support for the various types later.
    ConvexHull1 (int numVertices, Real* vertices, Real epsilon,
        bool owner, Query::Type queryType);
    virtual ~ConvexHull1 ();

    // The input vertex array.
    const Real* GetVertices () const;

    // Support for streaming to/from disk.
    ConvexHull1 (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Load (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Save (const char* filename, int mode = FileIO::FM_DEFAULT_WRITE)
        const;

private:
    using ConvexHull<Real>::mNumVertices;
    using ConvexHull<Real>::mDimension;
    using ConvexHull<Real>::mNumSimplices;
    using ConvexHull<Real>::mIndices;
    using ConvexHull<Real>::mEpsilon;
    using ConvexHull<Real>::mOwner;

    Real* mVertices;

    class SortedVertex
    {
    public:
        Real Value;
        int Index;

        bool operator< (const SortedVertex& proj) const
        {
            return Value < proj.Value;
        }
    };
};

typedef ConvexHull1<float> ConvexHull1f;
typedef ConvexHull1<double> ConvexHull1d;

}

#endif
