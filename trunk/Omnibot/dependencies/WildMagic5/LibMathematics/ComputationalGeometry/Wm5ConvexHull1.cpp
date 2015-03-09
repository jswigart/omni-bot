// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ConvexHull1.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull1<Real>::ConvexHull1 (int numVertices, Real* vertices,
    Real epsilon, bool owner, Query::Type queryType)
    :
    ConvexHull<Real>(numVertices, epsilon, owner, queryType),
    mVertices(vertices)
{
    assertion(mVertices != 0, "Must provide vertices\n");

    std::vector<SortedVertex> sortedArray(mNumVertices);
    int i;
    for (i = 0; i < mNumVertices; ++i)
    {
        sortedArray[i].Value = mVertices[i];
        sortedArray[i].Index = i;
    }
    std::sort(sortedArray.begin(), sortedArray.end());

    Real range = sortedArray[mNumVertices-1].Value - sortedArray[0].Value;
    if (range >= mEpsilon)
    {
        mDimension = 1;
        mNumSimplices = 2;
        mIndices = new1<int>(2);
        mIndices[0] = sortedArray[0].Index;
        mIndices[1] = sortedArray[mNumVertices-1].Index;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull1<Real>::~ConvexHull1 ()
{
    if (mOwner)
    {
        delete1(mVertices);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* ConvexHull1<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull1<Real>::ConvexHull1 (const char* filename, int mode)
    :
    ConvexHull<Real>(0, (Real)0, false, Query::QT_REAL),
    mVertices(0)
{
    bool loaded = Load(filename, mode);
    assertion(loaded, "Failed to load file\n");
    WM5_UNUSED(loaded);
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull1<Real>::Load (const char* filename, int mode)
{
    FileIO inFile(filename, mode);
    if (!inFile)
    {
        return false;
    }

    ConvexHull<Real>::Load(inFile);

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
bool ConvexHull1<Real>::Save (const char* filename, int mode) const
{
    FileIO outFile(filename, mode);
    if (!outFile)
    {
        return false;
    }

    ConvexHull<Real>::Save(outFile);
    outFile.Write(sizeof(Real), mNumVertices, mVertices);
    outFile.Close();
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ConvexHull1<float>;

template WM5_MATHEMATICS_ITEM
class ConvexHull1<double>;
//----------------------------------------------------------------------------
}
