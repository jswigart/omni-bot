// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ConvexHull3.h"
#include "Wm5Query3Filtered.h"
#include "Wm5Query3Int64.h"
#include "Wm5Query3Integer.h"
#include "Wm5Query3Rational.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull3<Real>::ConvexHull3 (int numVertices, Vector3<Real>* vertices,
    Real epsilon, bool owner, Query::Type queryType)
    :
    ConvexHull<Real>(numVertices, epsilon, owner, queryType),
    mLineOrigin(Vector3<Real>::ZERO),
    mLineDirection(Vector3<Real>::ZERO),
    mPlaneOrigin(Vector3<Real>::ZERO)
{
    mVertices = vertices;
    mPlaneDirection[0] = Vector3<Real>::ZERO;
    mPlaneDirection[1] = Vector3<Real>::ZERO;
    mSVertices = 0;
    mQuery = 0;

    typename Vector3<Real>::Information info;
    Vector3<Real>::GetInformation(mNumVertices, mVertices, mEpsilon, info);
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

    if (info.mDimension == 2)
    {
        // The set is (nearly) coplanar.  The caller is responsible for
        // creating a ConvexHull2 object.
        mDimension = 2;
        mPlaneOrigin = info.mOrigin;
        mPlaneDirection[0] = info.mDirection[0];
        mPlaneDirection[1] = info.mDirection[1];
        return;
    }

    mDimension = 3;

    int i0 = info.mExtreme[0];
    int i1 = info.mExtreme[1];
    int i2 = info.mExtreme[2];
    int i3 = info.mExtreme[3];

    mSVertices = new1<Vector3<Real> >(mNumVertices);
    int i;

    if (queryType != Query::QT_RATIONAL && queryType != Query::QT_FILTERED)
    {
        // Transform the vertices to the cube [0,1]^3.
        Vector3<Real> minValue(info.mMin[0], info.mMin[1], info.mMin[2]);
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
            mQuery = new0 Query3Int64<Real>(mNumVertices, mSVertices);
        }
        else if (queryType == Query::QT_INTEGER)
        {
            // Scale the vertices to the square [0,2^{24}]^2 to allow use of
            // Integer.
            expand = (Real)(1 << 24);
            mQuery = new0 Query3Integer<Real>(mNumVertices, mSVertices);
        }
        else  // queryType == Query::QT_REAL
        {
            // No scaling for floating point.
            expand = (Real)1;
            mQuery = new0 Query3<Real>(mNumVertices, mSVertices);
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
        memcpy(mSVertices, mVertices, mNumVertices*sizeof(Vector3<Real>));

        if (queryType == Query::QT_RATIONAL)
        {
            mQuery = new0 Query3Rational<Real>(mNumVertices, mSVertices);
        }
        else // queryType == Query::QT_FILTERED
        {
            mQuery = new0 Query3Filtered<Real>(mNumVertices, mSVertices,
                mEpsilon);
        }
    }

    Triangle* tri0;
    Triangle* tri1;
    Triangle* tri2;
    Triangle* tri3;

    if (info.mExtremeCCW)
    {
        tri0 = new0 Triangle(i0, i1, i3);
        tri1 = new0 Triangle(i0, i2, i1);
        tri2 = new0 Triangle(i0, i3, i2);
        tri3 = new0 Triangle(i1, i2, i3);
        tri0->AttachTo(tri1, tri3, tri2);
        tri1->AttachTo(tri2, tri3, tri0);
        tri2->AttachTo(tri0, tri3, tri1);
        tri3->AttachTo(tri1, tri2, tri0);
    }
    else
    {
        tri0 = new0 Triangle(i0, i3, i1);
        tri1 = new0 Triangle(i0, i1, i2);
        tri2 = new0 Triangle(i0, i2, i3);
        tri3 = new0 Triangle(i1, i3, i2);
        tri0->AttachTo(tri2, tri3, tri1);
        tri1->AttachTo(tri0, tri3, tri2);
        tri2->AttachTo(tri1, tri3, tri0);
        tri3->AttachTo(tri0, tri2, tri1);
    }

    mHull.clear();
    mHull.insert(tri0);
    mHull.insert(tri1);
    mHull.insert(tri2);
    mHull.insert(tri3);

    for (i = 0; i < mNumVertices; ++i)
    {
        if (!Update(i))
        {
            DeleteHull();
            return;
        }
    }

    ExtractIndices();
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull3<Real>::~ConvexHull3 ()
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
const Vector3<Real>& ConvexHull3<Real>::GetLineOrigin () const
{
    return mLineOrigin;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& ConvexHull3<Real>::GetLineDirection () const
{
    return mLineDirection;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& ConvexHull3<Real>::GetPlaneOrigin () const
{
    return mPlaneOrigin;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& ConvexHull3<Real>::GetPlaneDirection (int i) const
{
    return mPlaneDirection[i];
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull1<Real>* ConvexHull3<Real>::GetConvexHull1 () const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return 0;
    }

    Real* projection = new1<Real>(mNumVertices);
    for (int i = 0; i < mNumVertices; ++i)
    {
        Vector3<Real> diff = mVertices[i] - mLineOrigin;
        projection[i] = mLineDirection.Dot(diff);
    }

    return new0 ConvexHull1<Real>(mNumVertices, projection, mEpsilon, true,
        mQueryType);
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull2<Real>* ConvexHull3<Real>::GetConvexHull2 () const
{
    assertion(mDimension == 2, "The dimension must be 2\n");
    if (mDimension != 2)
    {
        return 0;
    }

    Vector2<Real>* projection = new1<Vector2<Real> >(mNumVertices);
    for (int i = 0; i < mNumVertices; ++i)
    {
        Vector3<Real> diff = mVertices[i] - mPlaneOrigin;
        projection[i][0] = mPlaneDirection[0].Dot(diff);
        projection[i][1] = mPlaneDirection[1].Dot(diff);
    }

    return new0 ConvexHull2<Real>(mNumVertices, projection, mEpsilon, true,
        mQueryType);
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull3<Real>::ConvexHull3 (const char* filename, int mode)
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
bool ConvexHull3<Real>::Load (const char* filename, int mode)
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
    mVertices = new1<Vector3<Real> >(mNumVertices);
    mSVertices = new1<Vector3<Real> >(mNumVertices);

    inFile.Read(sizeof(Real), 3*mNumVertices, mVertices);
    inFile.Read(sizeof(Real), 3*mNumVertices, mSVertices);
    inFile.Read(sizeof(Real), 3, &mLineOrigin);
    inFile.Read(sizeof(Real), 3, &mLineDirection);
    inFile.Read(sizeof(Real), 3, &mPlaneOrigin);
    inFile.Read(sizeof(Real), 6, mPlaneDirection);

    inFile.Close();

    switch (mQueryType)
    {
    case Query::QT_INT64:
    {
        mQuery = new0 Query3Int64<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_INTEGER:
    {
        mQuery = new0 Query3Integer<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_RATIONAL:
    {
        mQuery = new0 Query3Rational<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_REAL:
    {
        mQuery = new0 Query3<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_FILTERED:
    {
        mQuery = new0 Query3Filtered<Real>(mNumVertices, mSVertices,
            mEpsilon);
        break;
    }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull3<Real>::Save (const char* filename, int mode) const
{
    FileIO outFile(filename, mode);
    if (!outFile)
    {
        return false;
    }

    ConvexHull<Real>::Save(outFile);

    outFile.Write(sizeof(Real), 3*mNumVertices, mVertices);
    outFile.Write(sizeof(Real), 3*mNumVertices, mSVertices);
    outFile.Write(sizeof(Real), 3, &mLineOrigin);
    outFile.Write(sizeof(Real), 3, &mLineDirection);
    outFile.Write(sizeof(Real), 3, &mPlaneOrigin);
    outFile.Write(sizeof(Real), 6, mPlaneDirection);

    outFile.Close();
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexHull3<Real>::Update (int i)
{
    // Locate a triangle visible to the input point (if possible).
    Triangle* visible = 0;
    Triangle* tri;
    typename std::set<Triangle*>::iterator iter = mHull.begin();
    typename std::set<Triangle*>::iterator end = mHull.end();
    for (/**/; iter != end; ++iter)
    {
        tri = *iter;
        if (tri->GetSign(i, mQuery) > 0)
        {
            visible = tri;
            break;
        }
    }

    if (!visible)
    {
        // The point is inside the current hull; nothing to do.
        return true;
    }

    // Locate and remove the visible triangles.
    std::stack<Triangle*> visibleSet;
    std::map<int,TerminatorData> terminator;
    visibleSet.push(visible);
    visible->OnStack = true;
    int j, v0, v1;
    while (!visibleSet.empty())
    {
        tri = visibleSet.top();
        visibleSet.pop();
        tri->OnStack = false;
        for (j = 0; j < 3; ++j)
        {
            Triangle* adj = tri->Adj[j];
            if (adj)
            {
                // Detach triangle and adjacent triangle from each other.
                int nullIndex = tri->DetachFrom(j, adj);

                if (adj->GetSign(i, mQuery) > 0)
                {
                    if (!adj->OnStack)
                    {
                        // Adjacent triangle is visible.
                        visibleSet.push(adj);
                        adj->OnStack = true;
                    }
                }
                else
                {
                    // Adjacent triangle is invisible.
                    v0 = tri->V[j];
                    v1 = tri->V[(j+1)%3];
                    terminator[v0] = TerminatorData(v0, v1, nullIndex, adj);
                }
            }
        }
        mHull.erase(tri);
        delete0(tri);
    }

    // Insert the new edges formed by the input point and the terminator
    // between visible and invisible triangles.
    int size = (int)terminator.size();
    assertion(size >= 3, "Terminator must be at least a triangle\n");
    typename std::map<int,TerminatorData>::iterator edge = terminator.begin();
    v0 = edge->second.V[0];
    v1 = edge->second.V[1];
    tri = new0 Triangle(i, v0, v1);
    mHull.insert(tri);

    // Save information for linking first/last inserted new triangles.
    int saveV0 = edge->second.V[0];
    Triangle* saveTri = tri;

    // Establish adjacency links across terminator edge.
    tri->Adj[1] = edge->second.T;
    edge->second.T->Adj[edge->second.NullIndex] = tri;
    for (j = 1; j < size; ++j)
    {
        edge = terminator.find(v1);
        assertion(edge != terminator.end(), "Unexpected condition\n");
        v0 = v1;
        v1 = edge->second.V[1];
        Triangle* next = new0 Triangle(i, v0, v1);
        mHull.insert(next);

        // Establish adjacency links across terminator edge.
        next->Adj[1] = edge->second.T;
        edge->second.T->Adj[edge->second.NullIndex] = next;

        // Establish adjacency links with previously inserted triangle.
        next->Adj[0] = tri;
        tri->Adj[2] = next;

        tri = next;
    }
    assertion(v1 == saveV0, "Expecting initial vertex\n");
    WM5_UNUSED(saveV0);

    // Establish adjacency links between first/last triangles.
    saveTri->Adj[0] = tri;
    tri->Adj[2] = saveTri;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexHull3<Real>::ExtractIndices ()
{
    mNumSimplices = (int)mHull.size();
    mIndices = new1<int>(3*mNumSimplices);

    typename std::set<Triangle*>::iterator iter = mHull.begin();
    typename std::set<Triangle*>::iterator end = mHull.end();
    for (int i = 0; iter != end; ++iter)
    {
        Triangle* tri = *iter;
        for (int j = 0; j < 3; ++j, ++i)
        {
            mIndices[i] = tri->V[j];
        }
        delete0(tri);
    }
    mHull.clear();
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexHull3<Real>::DeleteHull ()
{
    typename std::set<Triangle*>::iterator iter = mHull.begin();
    typename std::set<Triangle*>::iterator end = mHull.end();
    for (/**/; iter != end; ++iter)
    {
        Triangle* tri = *iter;
        delete0(tri);
    }
    mHull.clear();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ConvexHull3::Triangle
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull3<Real>::Triangle::Triangle (int v0, int v1, int v2)
    :
    Sign(0),
    Time(-1),
    OnStack(false)
{
    V[0] = v0;
    V[1] = v1;
    V[2] = v2;
    Adj[0] = 0;
    Adj[1] = 0;
    Adj[2] = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexHull3<Real>::Triangle::GetSign (int i, const Query3<Real>* query)
{
    if (i != Time)
    {
        Time = i;
        Sign = query->ToPlane(i, V[0], V[1], V[2]);
    }

    return Sign;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexHull3<Real>::Triangle::AttachTo (Triangle* adj0, Triangle* adj1,
    Triangle* adj2)
{
    // assert:  The input adjacent triangles are correctly ordered.
    Adj[0] = adj0;
    Adj[1] = adj1;
    Adj[2] = adj2;
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexHull3<Real>::Triangle::DetachFrom (int adjIndex, Triangle* adj)
{
    assertion(0 <= adjIndex && adjIndex < 3 && Adj[adjIndex] == adj,
        "Invalid inputs\n");

    Adj[adjIndex] = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (adj->Adj[i] == this)
        {
            adj->Adj[i] = 0;
            return i;
        }
    }
    return -1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ConvexHull3::TerminatorData
//----------------------------------------------------------------------------
template <typename Real>
ConvexHull3<Real>::TerminatorData::TerminatorData (int v0, int v1,
    int nullIndex, Triangle* tri)
    :
    NullIndex(nullIndex),
    T(tri)
{
    V[0] = v0;
    V[1] = v1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ConvexHull3<float>;

template WM5_MATHEMATICS_ITEM
class ConvexHull3<double>;
//----------------------------------------------------------------------------
}
