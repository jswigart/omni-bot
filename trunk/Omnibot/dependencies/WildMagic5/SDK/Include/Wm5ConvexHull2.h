// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONVEXHULL2_H
#define WM5CONVEXHULL2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5ConvexHull1.h"
#include "Wm5Query2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM ConvexHull2 : public ConvexHull<Real>
{
public:
    // The input to the constructor is the array of vertices whose convex hull
    // is required.  If you want ConvexHull2 to delete the vertices during
    // destruction, set bOwner to 'true'.  Otherwise, you own the vertices and
    // must delete them yourself.
    //
    // You have a choice of speed versus accuracy.  The fastest choice is
    // Query::QT_INT64, but it gives up a lot of precision, scaling the points
    // to [0,2^{20}]^3.  The choice Query::QT_INTEGER gives up less precision,
    // scaling the points to [0,2^{24}]^3.  The choice Query::QT_RATIONAL uses
    // exact arithmetic, but is the slowest choice.  The choice Query::QT_REAL
    // uses floating-point arithmetic, but is not robust in all cases.
    ConvexHull2 (int numVertices, Vector2<Real>* vertices, Real epsilon,
        bool owner, Query::Type queryType);
    virtual ~ConvexHull2 ();

    // If GetDimension() returns 1, then the points lie on a line.  You must
    // create a ConvexHull1 object using the function provided.
    const Vector2<Real>& GetLineOrigin () const;
    const Vector2<Real>& GetLineDirection () const;
    ConvexHull1<Real>* GetConvexHull1 () const;

    // Support for streaming to/from disk.
    ConvexHull2 (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Load (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Save (const char* filename, int mode = FileIO::FM_DEFAULT_WRITE)
        const;

private:
    using ConvexHull<Real>::mQueryType;
    using ConvexHull<Real>::mNumVertices;
    using ConvexHull<Real>::mDimension;
    using ConvexHull<Real>::mNumSimplices;
    using ConvexHull<Real>::mIndices;
    using ConvexHull<Real>::mEpsilon;
    using ConvexHull<Real>::mOwner;

    class Edge
    {
    public:
        Edge (int v0, int v1);

        int GetSign (int i, const Query2<Real>* query);
        void Insert (Edge* adj0, Edge* adj1);
        void DeleteSelf ();
        void DeleteAll ();
        void GetIndices (int& numIndices, int*& indices);

        int V[2];
        Edge* E[2];
        int Sign;
        int Time;
    };

    bool Update (Edge*& hull, int i);

    // The input points.
    Vector2<Real>* mVertices;

    // Support for robust queries.
    Vector2<Real>* mSVertices;
    Query2<Real>* mQuery;

    // The line of containment if the dimension is 1.
    Vector2<Real> mLineOrigin, mLineDirection;
};

typedef ConvexHull2<float> ConvexHull2f;
typedef ConvexHull2<double> ConvexHull2d;

}

#endif
