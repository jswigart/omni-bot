// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONVEXHULL3_H
#define WM5CONVEXHULL3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5ConvexHull1.h"
#include "Wm5ConvexHull2.h"
#include "Wm5Query3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM ConvexHull3 : public ConvexHull<Real>
{
public:
    // The input to the constructor is the array of vertices whose convex hull
    // is required.  If you want ConvexHull3 to delete the vertices during
    // destruction, set bOwner to 'true'.  Otherwise, you own the vertices and
    // must delete them yourself.
    //
    // You have a choice of speed versus accuracy.  The fastest choice is
    // Query::QT_INT64, but it gives up a lot of precision, scaling the points
    // to [0,2^{20}]^3.  The choice Query::QT_INTEGER gives up less precision,
    // scaling the points to [0,2^{24}]^3.  The choice Query::QT_RATIONAL uses
    // exact arithmetic, but is the slowest choice.  The choice Query::QT_REAL
    // uses floating-point arithmetic, but is not robust in all cases.
    ConvexHull3 (int numVertices, Vector3<Real>* vertices, Real epsilon,
        bool bOwner, Query::Type eQueryType);
    virtual ~ConvexHull3 ();

    // If GetDimension() returns 1, then the points lie on a line.  You must
    // create a ConvexHull1 object using the function provided.
    const Vector3<Real>& GetLineOrigin () const;
    const Vector3<Real>& GetLineDirection () const;
    ConvexHull1<Real>* GetConvexHull1 () const;

    // If GetDimension() returns 2, then the points lie on a plane.  The plane
    // has two direction vectors (inputs 0 or 1).  You must create a
    // ConvexHull2 object using the function provided.
    const Vector3<Real>& GetPlaneOrigin () const;
    const Vector3<Real>& GetPlaneDirection (int i) const;
    ConvexHull2<Real>* GetConvexHull2 () const;

    // Support for streaming to/from disk.
    ConvexHull3 (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
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

    class Triangle
    {
    public:
        Triangle (int v0, int v1, int v2);

        int GetSign (int i, const Query3<Real>* query);
        void AttachTo (Triangle* adj0, Triangle* adj1, Triangle* adj2);
        int DetachFrom (int adjIndex, Triangle* adj);

        int V[3];
        Triangle* Adj[3];
        int Sign;
        int Time;
        bool OnStack;
    };

    bool Update (int i);
    void ExtractIndices ();
    void DeleteHull ();

    // The input points.
    Vector3<Real>* mVertices;

    // Support for robust queries.
    Vector3<Real>* mSVertices;
    Query3<Real>* mQuery;

    // The line of containment if the dimension is 1.
    Vector3<Real> mLineOrigin, mLineDirection;

    // The plane of containment if the dimension is 2.
    Vector3<Real> mPlaneOrigin, mPlaneDirection[2];

    // The current hull.
    std::set<Triangle*> mHull;

    class TerminatorData
    {
    public:
        TerminatorData (int v0 = -1, int v1 = -1, int nullIndex = -1,
            Triangle* tri = 0);

        int V[2];
        int NullIndex;
        Triangle* T;
    };
};

typedef ConvexHull3<float> ConvexHull3f;
typedef ConvexHull3<double> ConvexHull3d;

}

#endif
