// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5GRIDGRAPH2_H
#define WM5GRIDGRAPH2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Math.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM GridGraph2
{
public:
    // The 2D grid is chosen to be 8-connected.  Each vertex (x,y) has 8
    // neighbors: (x-1,y-1), (x,y-1), (x+1,y-1), (x-1,y), (x+1,y),
    // (x-1,y+1), (x,y+1), (x+1,y+1).  The graph is undirected.
    GridGraph2 (int xSize, int ySize);
    virtual ~GridGraph2 ();

    // Access to the graph sizes.
    int GetXSize () const;
    int GetYSize () const;
    int GetVertexQuantity () const;

    // Basic access to the edge weights of the graph.
    void SetWeight (int x, int y, int dx, int dy, Real weight);
    Real GetWeight (int x, int y, int dx, int dy) const;

    // Compute the minimum-weight path from (x0,y0) to (x1,y1).
    void ComputeMinimumWeightPath (int x0, int y0, int x1, int y1);

    // The path is stored internally.  Access it using these functions.  If
    // the input index i is out of range, the returned values are -1.
    int GetPathQuantity () const;
    void GetPathPoint (int i, int& x, int& y) const;

    // A callback that is executed during relaxation step.
    typedef void (*RelaxationCallbackFunction)();
    RelaxationCallbackFunction RelaxationCallback;

    int GetNumProcessed () const;

protected:
    class WM5_MATHEMATICS_ITEM Vertex
    {
    public:
        Vertex ();
        void SetWeight (int dx, int dy, Real weight);
        Real GetWeight (int dx, int dy) const;

        // Support for minimum-weight paths.
        Real Estimate;
        int Predecessor;

    private:
        // Weights for the eight neighbors.
        //   weight[0] for (x-1,y-1)
        //   weight[1] for (x  ,y-1)
        //   weight[2] for (x+1,y-1)
        //   weight[3] for (x-1,y  )
        //   weight[4] for (x+1,y  )
        //   weight[5] for (x-1,y+1)
        //   weight[6] for (x  ,y+1)
        //   weight[7] for (x+1,y+1)
        Real mWeight[8];
    };

    // The 2-dimensional grid is stored as a 1-dimensional array.
    inline int GetIndex (int x, int y) const { return x + mXSize*y; }
    inline int GetX (int index) const { return index % mXSize; }
    inline int GetY (int index) const { return index / mXSize; }

    int mXSize, mYSize, mNumVertices;
    Vertex* mVertices;

    int mPathQuantity;
    int* mPath;
    int* mPending;
    int mNumProcessed;

    friend class Vertex;
    static const int msIndex[3][3];  // index[dy][dx]
};

typedef GridGraph2<float> GridGraph2f;
typedef GridGraph2<double> GridGraph2d;

}

#endif
