// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ConvexHull.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull<Real>::ConvexHull (int numVertices, Real epsilon, bool owner,
    Query::Type queryType)
    :
    mQueryType(queryType),
    mNumVertices(numVertices),
    mDimension(0),
    mNumSimplices(0),
    mIndices(0),
    mEpsilon(epsilon),
    mOwner(owner)
{
    assertion(mNumVertices > 0 && mEpsilon >= (Real)0, "Invalid inputs\n");
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull<Real>::~ConvexHull ()
{
    delete1(mIndices);
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexHull<Real>::GetQueryType () const
{
    return mQueryType;
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexHull<Real>::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ConvexHull<Real>::GetEpsilon () const
{
    return mEpsilon;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull<Real>::GetOwner () const
{
    return mOwner;
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexHull<Real>::GetDimension () const
{
    return mDimension;
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexHull<Real>::GetNumSimplices () const
{
    return mNumSimplices;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* ConvexHull<Real>::GetIndices () const
{
    return mIndices;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull<Real>::Load (FileIO& inFile)
{
    delete1(mIndices);

    // Fixed-size members.
    int type;
    inFile.Read(sizeof(int), &type);
    mQueryType = (Query::Type)type;

    inFile.Read(sizeof(int), &mNumVertices);
    inFile.Read(sizeof(int), &mDimension);
    inFile.Read(sizeof(int), &mNumSimplices);
    inFile.Read(sizeof(Real), &mEpsilon);

    // Variable-size members.
    int numIndices;
    inFile.Read(sizeof(int), &numIndices);
    if (1 <= mDimension && mDimension <= 3)
    {
        assertion(numIndices == (mDimension+1)*mNumSimplices,
            "Inconsistent index count\n");
        mIndices = new1<int>(numIndices);
        inFile.Read(sizeof(int), numIndices, mIndices);
        return true;
    }

    mIndices = 0;
    return mDimension == 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull<Real>::Save (FileIO& outFile) const
{
    // Fixed-size members.
    int type = (int)mQueryType;
    outFile.Write(sizeof(int), &type);

    outFile.Write(sizeof(int), &mNumVertices);
    outFile.Write(sizeof(int), &mDimension);
    outFile.Write(sizeof(int), &mNumSimplices);
    outFile.Write(sizeof(Real), &mEpsilon);

    // The member mOwner is not streamed because on a Load call, this
    // object will allocate the vertices and own this memory.

    // Variable-size members.
    int numIndices;
    if (1 <= mDimension && mDimension <= 3)
    {
        numIndices = (mDimension+1)*mNumSimplices;
        outFile.Write(sizeof(int), &numIndices);
        outFile.Write(sizeof(int), numIndices, mIndices);
        return true;
    }

    numIndices = 0;
    outFile.Write(sizeof(int), &numIndices);
    return mDimension == 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ConvexHull<float>;

template WM5_MATHEMATICS_ITEM
class ConvexHull<double>;
//----------------------------------------------------------------------------
}
