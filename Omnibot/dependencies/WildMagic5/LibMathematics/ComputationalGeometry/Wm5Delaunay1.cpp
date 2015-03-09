// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Delaunay1.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Delaunay1<Real>::Delaunay1 (int numVertices, Real* vertices, Real epsilon,
    bool owner, Query::Type queryType)
    :
    Delaunay<Real>(numVertices, epsilon, owner, queryType),
    mVertices(vertices)
{
    std::vector<SortedVertex> sorted(mNumVertices);
    int i;
    for (i = 0; i < mNumVertices; ++i)
    {
        sorted[i].Value = mVertices[i];
        sorted[i].Index = i;
    }
    std::sort(sorted.begin(), sorted.end());

    Real range = sorted[mNumVertices - 1].Value - sorted[0].Value;
    if (range >= mEpsilon)
    {
        mDimension = 1;
        mNumSimplices = mNumVertices - 1;
        mIndices = new1<int>(2*mNumSimplices);
        for (i = 0; i < mNumSimplices; ++i)
        {
            mIndices[2*i] = sorted[i].Index;
            mIndices[2*i + 1] = sorted[i + 1].Index;
        }

        mAdjacencies = new1<int>(2*mNumSimplices);
        for (i = 0; i < mNumSimplices; ++i)
        {
            mAdjacencies[2*i] = i - 1;
            mAdjacencies[2*i + 1] = i + 1;
        }
        mAdjacencies[2*mNumSimplices - 1] = -1;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay1<Real>::~Delaunay1 ()
{
    if (mOwner)
    {
        delete1(mVertices);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* Delaunay1<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay1<Real>::GetHull (int hull[2])
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return false;
    }

    hull[0] = mIndices[0];
    hull[1] = mIndices[2*mNumSimplices - 1];
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay1<Real>::GetContainingSegment (const Real p) const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return -1;
    }

    if (p < mVertices[mIndices[0]])
    {
        return -1;
    }

    if (p > mVertices[mIndices[2*mNumSimplices - 1]])
    {
        return -1;
    }

    int i;
    for (i = 0; i < mNumSimplices; ++i)
    {
        if (p <= mVertices[mIndices[2*i + 1]])
        {
            break;
        }
    }

    assertion(i < mNumSimplices, "Input not in hull\n");
    return i;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay1<Real>::GetVertexSet (int i, Real vertices[2]) const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        vertices[0] = mVertices[mIndices[2*i]];
        vertices[1] = mVertices[mIndices[2*i + 1]];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay1<Real>::GetIndexSet (int i, int indices[2]) const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        indices[0] = mIndices[2*i];
        indices[1] = mIndices[2*i + 1];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay1<Real>::GetAdjacentSet (int i, int adjacencies[2]) const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        adjacencies[0] = mAdjacencies[2*i];
        adjacencies[1] = mAdjacencies[2*i + 1];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay1<Real>::GetBarycentricSet (int i, const Real p, Real bary[2])
    const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        Real v0 = mVertices[mIndices[2*i]];
        Real v1 = mVertices[mIndices[2*i + 1]];
        Real denom = v1 - v0;
        if (denom > mEpsilon)
        {
            bary[0] = (v1 - p)/denom;
        }
        else
        {
            bary[0] = (Real)1;
        }

        bary[1] = (Real)1 - bary[0];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay1<Real>::Delaunay1 (const char* filename, int mode)
    :
    Delaunay<Real>(0, (Real)0, false, Query::QT_REAL)
{
    mVertices = 0;
    bool loaded = Load(filename, mode);
    assertion(loaded, "Cannot open file %s\n", filename);
    WM5_UNUSED(loaded);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay1<Real>::Load (const char* filename, int mode)
{
    FileIO inFile(filename, mode);
    if (!inFile)
    {
        return false;
    }

    Delaunay<Real>::Load(inFile);

    if (mOwner)
    {
        delete1(mVertices);
    }

    mOwner = true;
    mVertices = new1<Real>(mNumVertices);
    inFile.Read(sizeof(Real), mNumVertices, mVertices);

    inFile.Close();
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay1<Real>::Save (const char* filename, int mode) const
{
    FileIO outFile(filename, mode);
    if (!outFile)
    {
        return false;
    }

    Delaunay<Real>::Save(outFile);

    outFile.Write(sizeof(Real), mNumVertices, mVertices);

    outFile.Close();
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Delaunay1<float>;

template WM5_MATHEMATICS_ITEM
class Delaunay1<double>;
//----------------------------------------------------------------------------
}
