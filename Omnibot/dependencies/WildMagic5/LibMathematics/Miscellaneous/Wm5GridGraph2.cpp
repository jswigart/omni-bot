// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#include "Wm5MathematicsPCH.h"
#include "Wm5GridGraph2.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
GridGraph2<Real>::GridGraph2 (int xSize, int ySize)
{
    assertion(xSize > 0 && ySize > 0, "Invalid input\n");

    mXSize = xSize;
    mYSize = ySize;
    mNumVertices = mXSize*mYSize;
    mVertices = new1<Vertex>(mNumVertices);

    mPathQuantity = 0;
    mPath = new1<int>(mNumVertices);
    mPending = new1<int>(mNumVertices);

    RelaxationCallback = 0;
    mNumProcessed = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
GridGraph2<Real>::~GridGraph2 ()
{
    delete1(mVertices);
    delete1(mPath);
    delete1(mPending);
}
//----------------------------------------------------------------------------
template <typename Real>
int GridGraph2<Real>::GetXSize () const
{
    return mXSize;
}
//----------------------------------------------------------------------------
template <typename Real>
int GridGraph2<Real>::GetYSize () const
{
    return mYSize;
}
//----------------------------------------------------------------------------
template <typename Real>
int GridGraph2<Real>::GetVertexQuantity () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
void GridGraph2<Real>::SetWeight (int x, int y, int dx, int dy, Real weight)
{
    assertion(0 <= x && x < mXSize && 0 <= y && y < mYSize,
        "Invalid input\n");
    assertion(abs(dx) <= 1 && abs(dy) <= 1, "Invalid input\n");

    mVertices[GetIndex(x, y)].SetWeight(dx, dy, weight);
}
//----------------------------------------------------------------------------
template <typename Real>
Real GridGraph2<Real>::GetWeight (int x, int y, int dx, int dy) const
{
    assertion(0 <= x && x < mXSize && 0 <= y && y < mYSize,
        "Invalid input\n");
    assertion(abs(dx) <= 1 && abs(dy) <= 1, "Invalid input\n");

    return mVertices[GetIndex(x, y)].GetWeight(dx, dy);
}
//----------------------------------------------------------------------------
template <typename Real>
void GridGraph2<Real>::ComputeMinimumWeightPath (int x0, int y0, int x1,
    int y1)
{
    mPathQuantity = 0;

    if (x0 < 0 || x0 >= mXSize || y0 < 0 || y0 >= mYSize
    ||  x1 < 0 || x1 >= mXSize || y1 < 0 || y1 >= mYSize)
    {
        return;
    }

    // Initialize the minimum weight estimates and the predecessors for the
    // graph vertices.
    int pendingQuantity = mNumVertices;
    int i;
    for (i = 0; i < mNumVertices; ++i)
    {
        Vertex& vertex = mVertices[i];
        vertex.Estimate = Math<Real>::MAX_REAL;
        vertex.Predecessor = -1;
        mPending[i] = i;
    }
    mVertices[GetIndex(x0, y0)].Estimate = (Real)0;

    mNumProcessed = 0;
    if (RelaxationCallback)
    {
        RelaxationCallback();
    }

    while (pendingQuantity > 0)
    {
        // ***** TO DO.  Replace this by an efficient priority queue.
        // Find the vertex with minimum estimate.
        Real minimum = mVertices[mPending[0]].Estimate;
        int minIndex = 0;
        for (i = 1; i < pendingQuantity; ++i)
        {
            Real value = mVertices[mPending[i]].Estimate;
            if (value < minimum)
            {
                minimum = value;
                minIndex = i;
            }
        }

        // Remove the minimum-estimate vertex from the pending array.  Keep
        // the array elements contiguous.
        int minVertexIndex = mPending[minIndex];
        --pendingQuantity;
        if (minIndex != pendingQuantity)
        {
            mPending[minIndex] = mPending[pendingQuantity];
        }
        // ***** END TO DO.

        // Relax the paths from the minimum-estimate vertex.
        Vertex& minVertex = mVertices[minVertexIndex];
        int xmin = GetX(minVertexIndex);
        int ymin = GetY(minVertexIndex);
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                if (dx != 0 || dy != 0)
                {
                    int xadj = xmin + dx;
                    int yadj = ymin + dy;
                    if (0 <= xadj && xadj < mXSize
                    &&  0 <= yadj && yadj < mYSize)
                    {
                        Vertex& adjacent = mVertices[GetIndex(xadj, yadj)];

                        Real weight = GetWeight(xmin, ymin, dx, dy);
                        Real newEstimate = minVertex.Estimate + weight;
                        if (adjacent.Estimate > newEstimate)
                        {
                            adjacent.Estimate = newEstimate;
                            adjacent.Predecessor = minVertexIndex;
                        }
                    }
                }
            }
        }

        ++mNumProcessed;
        if (RelaxationCallback)
        {
            RelaxationCallback();
        }
    }

    // Construct the path.
    int* current = mPath;
    i = GetIndex(x1, y1);
    while (i >= 0)
    {
        ++mPathQuantity;
        *current++ = i;
        i = mVertices[i].Predecessor;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int GridGraph2<Real>::GetPathQuantity () const
{
    return mPathQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
void GridGraph2<Real>::GetPathPoint (int i, int& x, int& y) const
{
    if (0 <= i && i < mPathQuantity)
    {
        x = GetX(mPath[i]);
        y = GetY(mPath[i]);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int GridGraph2<Real>::GetNumProcessed () const
{
    return mNumProcessed;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GridGraph2::Vertex
//----------------------------------------------------------------------------
template <typename Real>
GridGraph2<Real>::Vertex::Vertex ()
{
    for (int i = 0; i < 8; ++i)
    {
        mWeight[i] = Math<Real>::MAX_REAL;
    }
    Estimate = Math<Real>::MAX_REAL;
    Predecessor = -1;
}
//----------------------------------------------------------------------------
template <typename Real>
void GridGraph2<Real>::Vertex::SetWeight (int dx, int dy, Real weight)
{
    mWeight[GridGraph2<Real>::msIndex[dy+1][dx+1]] = weight;
}
//----------------------------------------------------------------------------
template <typename Real>
Real GridGraph2<Real>::Vertex::GetWeight (int dx, int dy) const
{
    return mWeight[GridGraph2<Real>::msIndex[dy+1][dx+1]];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template<>
const int GridGraph2<float>::msIndex[3][3] =
{
    {0,1,2}, {3,-1,4}, {5,6,7}
};

template WM5_MATHEMATICS_ITEM
class GridGraph2<float>;

template<>
const int GridGraph2<double>::msIndex[3][3] =
{
    {0,1,2}, {3,-1,4}, {5,6,7}
};

template WM5_MATHEMATICS_ITEM
class GridGraph2<double>;
//----------------------------------------------------------------------------
}
