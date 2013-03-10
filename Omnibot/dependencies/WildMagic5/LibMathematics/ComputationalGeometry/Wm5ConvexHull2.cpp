// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ConvexHull2.h"
#include "Wm5Query2Filtered.h"
#include "Wm5Query2Int64.h"
#include "Wm5Query2Integer.h"
#include "Wm5Query2Rational.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull2<Real>::ConvexHull2 (int numVertices, Vector2<Real>* vertices,
    Real epsilon, bool owner, Query::Type queryType)
    :
    ConvexHull<Real>(numVertices, epsilon, owner, queryType),
    mVertices(vertices),
    mSVertices(0),
    mQuery(0),
    mLineOrigin(Vector2<Real>::ZERO),
    mLineDirection(Vector2<Real>::ZERO)
{
    typename Vector2<Real>::Information info;
    Vector2<Real>::GetInformation(mNumVertices, mVertices, mEpsilon, info);
    if (info.mDimension == 0)
    {
        // The values of mDimension and mIndices were already initialized by
        // the ConvexHull base class.
        return;
    }

    if (info.mDimension == 1)
    {
        // The set is (nearly) collinear.  The caller is responsible for
        // creating a ConvexHull1 object.
        mDimension = 1;
        mLineOrigin = info.mOrigin;
        mLineDirection = info.mDirection[0];
        return;
    }

    mDimension = 2;

    int i0 = info.mExtreme[0];
    int i1 = info.mExtreme[1];
    int i2 = info.mExtreme[2];

    mSVertices = new1<Vector2<Real> >(mNumVertices);
    int i;

    if (queryType != Query::QT_RATIONAL && queryType != Query::QT_FILTERED)
    {
        // Transform the vertices to the square [0,1]^2.
        Vector2<Real> minValue(info.mMin[0], info.mMin[1]);
        Real scale = ((Real)1)/info.mMaxRange;
        for (i = 0; i < mNumVertices; ++i)
        {
            mSVertices[i] = (mVertices[i] - minValue)*scale;
        }

        Real expand;
        if (queryType == Query::QT_INT64)
        {
            // Scale the vertices to the square [0,2^{20}]^2 to allow use of
            // 64-bit integers.
            expand = (Real)(1 << 20);
            mQuery = new0 Query2Int64<Real>(mNumVertices, mSVertices);
        }
        else if (queryType == Query::QT_INTEGER)
        {
            // Scale the vertices to the square [0,2^{24}]^2 to allow use of
            // Integer.
            expand = (Real)(1 << 24);
            mQuery = new0 Query2Integer<Real>(mNumVertices, mSVertices);
        }
        else  // queryType == Query::QT_REAL
        {
            // No scaling for floating point.
            expand = (Real)1;
            mQuery = new0 Query2<Real>(mNumVertices, mSVertices);
        }

        for (i = 0; i < mNumVertices; ++i)
        {
            mSVertices[i] *= expand;
        }
    }
    else
    {
        // No transformation needed for exact rational arithmetic or filtered
        // predicates.
        memcpy(mSVertices, mVertices, mNumVertices*sizeof(Vector2<Real>));

        if (queryType == Query::QT_RATIONAL)
        {
            mQuery = new0 Query2Rational<Real>(mNumVertices, mSVertices);
        }
        else // queryType == Query::QT_FILTERED
        {
            mQuery = new0 Query2Filtered<Real>(mNumVertices, mSVertices,
                mEpsilon);
        }
    }

    Edge* edge0;
    Edge* edge1;
    Edge* edge2;

    if (info.mExtremeCCW)
    {
        edge0 = new0 Edge(i0, i1);
        edge1 = new0 Edge(i1, i2);
        edge2 = new0 Edge(i2, i0);
    }
    else
    {
        edge0 = new0 Edge(i0, i2);
        edge1 = new0 Edge(i2, i1);
        edge2 = new0 Edge(i1, i0);
    }

    edge0->Insert(edge2, edge1);
    edge1->Insert(edge0, edge2);
    edge2->Insert(edge1, edge0);

    Edge* hull = edge0;
    for (i = 0; i < mNumVertices; ++i)
    {
        if (!Update(hull, i))
        {
            hull->DeleteAll();
            return;
        }
    }

    hull->GetIndices(mNumSimplices, mIndices);
    hull->DeleteAll();
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull2<Real>::~ConvexHull2 ()
{
    if (mOwner)
    {
        delete1(mVertices);
    }
    delete1(mSVertices);
    delete0(mQuery);
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& ConvexHull2<Real>::GetLineOrigin () const
{
    return mLineOrigin;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& ConvexHull2<Real>::GetLineDirection () const
{
    return mLineDirection;
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull1<Real>* ConvexHull2<Real>::GetConvexHull1 () const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return 0;
    }

    Real* projection = new1<Real>(mNumVertices);
    for (int i = 0; i < mNumVertices; ++i)
    {
        Vector2<Real> diff = mVertices[i] - mLineOrigin;
        projection[i] = mLineDirection.Dot(diff);
    }

    return new0 ConvexHull1<Real>(mNumVertices, projection, mEpsilon, true,
        mQueryType);
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull2<Real>::ConvexHull2 (const char* filename, int mode)
    :
    ConvexHull<Real>(0, (Real)0, false, Query::QT_REAL),
    mVertices(0),
    mSVertices(0),
    mQuery(0)
{
    bool loaded = Load(filename, mode);
    assertion(loaded, "Cannot open file %s\n", filename);
    WM5_UNUSED(loaded);
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull2<Real>::Load (const char* filename, int mode)
{
    FileIO inFile(filename, mode);
    if (!inFile)
    {
        return false;
    }

    ConvexHull<Real>::Load(inFile);

    delete0(mQuery);
    delete1(mSVertices);
    if (mOwner)
    {
        delete1(mVertices);
    }

    mOwner = true;
    mVertices = new1<Vector2<Real> >(mNumVertices);
    mSVertices = new1<Vector2<Real> >(mNumVertices);

    inFile.Read(sizeof(Real), 2*mNumVertices, mVertices);
    inFile.Read(sizeof(Real), 2*mNumVertices, mSVertices);
    inFile.Read(sizeof(Real), 2, &mLineOrigin);
    inFile.Read(sizeof(Real), 2, &mLineDirection);

    inFile.Close();

    switch (mQueryType)
    {
    case Query::QT_INT64:
    {
        mQuery = new0 Query2Int64<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_INTEGER:
    {
        mQuery = new0 Query2Integer<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_RATIONAL:
    {
        mQuery = new0 Query2Rational<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_REAL:
    {
        mQuery = new0 Query2<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_FILTERED:
    {
        mQuery = new0 Query2Filtered<Real>(mNumVertices, mSVertices,
            mEpsilon);
        break;
    }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull2<Real>::Save (const char* filename, int mode) const
{
    FileIO outFile(filename, mode);
    if (!outFile)
    {
        return false;
    }

    ConvexHull<Real>::Save(outFile);

    outFile.Write(sizeof(Real), 2*mNumVertices, mVertices);
    outFile.Write(sizeof(Real), 2*mNumVertices, mSVertices);
    outFile.Write(sizeof(Real), 2, &mLineOrigin);
    outFile.Write(sizeof(Real), 2, &mLineDirection);

    outFile.Close();
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull2<Real>::Update (Edge*& hull, int i)
{
    // Locate an edge visible to the input point (if possible).
    Edge* visible = 0;
    Edge* current = hull;
    do
    {
        if (current->GetSign(i, mQuery) > 0)
        {
            visible = current;
            break;
        }

        current = current->E[1];
    }
    while (current != hull);

    if (!visible)
    {
        // The point is inside the current hull; nothing to do.
        return true;
    }

    // Remove the visible edges.
    Edge* adj0 = visible->E[0];
    assertion(adj0 != 0, "Expecting nonnull adjacent\n");
    if (!adj0)
    {
        return false;
    }

    Edge* adj1 = visible->E[1];
    assertion(adj1 != 0, "Expecting nonnull adjacent\n");
    if (!adj1)
    {
        return false;
    }

    visible->DeleteSelf();

    while (adj0->GetSign(i, mQuery) > 0)
    {
        hull = adj0;
        adj0 = adj0->E[0];
        assertion(adj0 != 0, "Expecting nonnull adjacent\n");
        if (!adj0)
        {
            return false;
        }

        adj0->E[1]->DeleteSelf();
    }

    while (adj1->GetSign(i,mQuery) > 0)
    {
        hull = adj1;
        adj1 = adj1->E[1];
        assertion(adj1 != 0, "Expecting nonnull adjacent\n");
        if (!adj1)
        {
            return false;
        }

        adj1->E[0]->DeleteSelf();
    }

    // Insert the new edges formed by the input point and the end points of
    // the polyline of invisible edges.
    Edge* edge0 = new0 Edge(adj0->V[1], i);
    Edge* edge1 = new0 Edge(i, adj1->V[0]);
    edge0->Insert(adj0, edge1);
    edge1->Insert(edge0, adj1);
    hull = edge0;

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ConvexHull2::Edge
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull2<Real>::Edge::Edge (int v0, int v1)
    :
    Sign(0),
    Time(-1)
{
    V[0] = v0;
    V[1] = v1;
    E[0] = 0;
    E[1] = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexHull2<Real>::Edge::GetSign (int i, const Query2<Real>* query)
{
    if (i != Time)
    {
        Time = i;
        Sign = query->ToLine(i, V[0], V[1]);
    }

    return Sign;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexHull2<Real>::Edge::Insert (Edge* adj0, Edge* adj1)
{
    adj0->E[1] = this;
    adj1->E[0] = this;
    E[0] = adj0;
    E[1] = adj1;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexHull2<Real>::Edge::DeleteSelf ()
{
    if (E[0])
    {
        E[0]->E[1] = 0;
    }

    if (E[1])
    {
        E[1]->E[0] = 0;
    }

    Edge* tmpThis = this;
    delete0(tmpThis);
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexHull2<Real>::Edge::DeleteAll ()
{
    Edge* adj = E[1];
    while (adj && adj != this)
    {
        Edge* save = adj->E[1];
        delete0(adj);
        adj = save;
    }

    assertion(adj == this, "Unexpected condition\n");
    Edge* tmpThis = this;
    delete0(tmpThis);
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexHull2<Real>::Edge::GetIndices (int& numIndices, int*& indices)
{
    // Count the number of edge vertices and allocate the index array.
    numIndices = 0;
    Edge* current = this;
    do
    {
        ++numIndices;
        current = current->E[1];
    }
    while (current != this);
    indices = new1<int>(numIndices);

    // Fill the index array.
    numIndices = 0;
    current = this;
    do
    {
        indices[numIndices] = current->V[0];
        ++numIndices;
        current = current->E[1];
    }
    while (current != this);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ConvexHull2<float>;

template WM5_MATHEMATICS_ITEM
class ConvexHull2<double>;
//----------------------------------------------------------------------------
}
