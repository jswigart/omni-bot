// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.4 (2010/12/22)

#include "Wm5MathematicsPCH.h"
#include "Wm5Delaunay3.h"
#include "Wm5Delaunay2.h"
#include "Wm5Query3Filtered.h"
#include "Wm5Query3Int64.h"
#include "Wm5Query3Integer.h"
#include "Wm5Query3Rational.h"

// Indexing for the vertices of the triangle opposite a vertex.  The triangle
// opposite vertex j is
//   <gsIndex[j][0], gsIndex[j][1], gsIndex[j][2]>
// and is listed in counterclockwise order when viewed from outside the
// tetrahedron.
static const int gsIndex[4][3] = { {1,2,3}, {0,3,2}, {0,1,3}, {0,2,1} };

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Delaunay3<Real>::Delaunay3 (int numVertices, Vector3<Real>* vertices,
    Real epsilon, bool owner, Query::Type queryType)
    :
    Delaunay<Real>(numVertices, epsilon, owner, queryType),
    mVertices(vertices),
    mNumUniqueVertices(0),
    mSVertices(0),
    mQuery(0),
    mLineOrigin(Vector3<Real>::ZERO),
    mLineDirection(Vector3<Real>::ZERO),
    mPlaneOrigin(Vector3<Real>::ZERO),
    mPathLast(-1),
    mPath(0),
    mLastFaceV0(-1),
    mLastFaceV1(-1),
    mLastFaceV2(-1),
    mLastFaceOpposite(-1),
    mLastFaceOppositeIndex(-1)
{
    mPlaneDirection[0] = Vector3<Real>::ZERO;
    mPlaneDirection[1] = Vector3<Real>::ZERO;

    typename Vector3<Real>::Information info;
    Vector3<Real>::GetInformation(mNumVertices, mVertices, mEpsilon, info);
    if (info.mDimension == 0)
    {
        // The values of mDimension, mIndices, and mAdjacencies were
        // already initialized by the Delaunay base class.
        return;
    }

    if (info.mDimension == 1)
    {
        // The set is (nearly) collinear.  The caller is responsible for
        // creating a Delaunay1 object.
        mDimension = 1;
        mLineOrigin = info.mOrigin;
        mLineDirection = info.mDirection[0];
        return;
    }

    if (info.mDimension == 2)
    {
        // The set is (nearly) coplanar.  The caller is responsible for
        // creating a Delaunay2 object.
        mDimension = 2;
        mPlaneOrigin = info.mOrigin;
        mPlaneDirection[0] = info.mDirection[0];
        mPlaneDirection[1] = info.mDirection[1];
        return;
    }

    mDimension = 3;

    // Allocate storage for the input vertices and the supertetrahedron
    // vertices.
    mSVertices = new1<Vector3<Real> >(mNumVertices + 4);
    int i;

    if (queryType != Query::QT_RATIONAL && queryType != Query::QT_FILTERED)
    {
        // Transform the vertices to the cube [0,1]^3.
        mMin = Vector3<Real>(info.mMin[0], info.mMin[1], info.mMin[2]);
        mScale = ((Real)1)/info.mMaxRange;
        for (i = 0; i < mNumVertices; ++i)
        {
            mSVertices[i] = (mVertices[i] - mMin)*mScale;
        }

        // Construct the supertetrahedron to contain [0,1]^3.
        mSupervertices[0] = mNumVertices++;
        mSupervertices[1] = mNumVertices++;
        mSupervertices[2] = mNumVertices++;
        mSupervertices[3] = mNumVertices++;
        mSVertices[mSupervertices[0]] = Vector3<Real>((Real)-1,(Real)-1,
            (Real)-1);
        mSVertices[mSupervertices[1]] = Vector3<Real>((Real)+6,(Real)-1,
            (Real)-1);
        mSVertices[mSupervertices[2]] = Vector3<Real>((Real)-1,(Real)+6,
            (Real)-1);
        mSVertices[mSupervertices[3]] = Vector3<Real>((Real)-1,(Real)-1,
            (Real)+6);

        Real expand;
        if (queryType == Query::QT_INT64)
        {
            // Scale the vertices to the cube [0,2^{10}]^3 to allow use of
            // 64-bit integers for tetrahedralization.
            expand = (Real)(1 << 10);
            mQuery = new0 Query3Int64<Real>(mNumVertices, mSVertices);
        }
        else if (queryType == Query::QT_INTEGER)
        {
            // Scale the vertices to the cube [0,2^{20}]^3 to get more
            // precision for TInteger than for 64-bit integers for
            // tetrahedralization.
            expand = (Real)(1 << 20);
            mQuery = new0 Query3Integer<Real>(mNumVertices, mSVertices);
        }
        else // queryType == Query::QT_REAL
        {
            // No scaling for floating point.
            expand = (Real)1;
            mQuery = new0 Query3<Real>(mNumVertices, mSVertices);
        }

        mScale *= expand;
        for (i = 0; i < mNumVertices; ++i)
        {
            mSVertices[i] *= expand;
        }
    }
    else
    {
        // No transformation needed for exact rational arithmetic.
        mMin = Vector3<Real>::ZERO;
        mScale = (Real)1;
        memcpy(mSVertices, mVertices, mNumVertices*sizeof(Vector3<Real>));

        // Construct the supertriangle to contain [min,max].
        Vector3<Real> minValue =
            Vector3<Real>(info.mMin[0], info.mMin[1], info.mMin[2]);
        Vector3<Real> maxValue =
            Vector3<Real>(info.mMax[0], info.mMax[1], info.mMax[2]);
        Vector3<Real> delta = maxValue - minValue;
        Vector3<Real> superMin = minValue - delta;
        Vector3<Real> superMax = maxValue + ((Real)5.0)*delta;
        mSupervertices[0] = mNumVertices++;
        mSupervertices[1] = mNumVertices++;
        mSupervertices[2] = mNumVertices++;
        mSupervertices[3] = mNumVertices++;
        mSVertices[mSupervertices[0]] = superMin;
        mSVertices[mSupervertices[1]] = Vector3<Real>(superMax[0],
            superMin[1], superMin[2]);
        mSVertices[mSupervertices[2]] = Vector3<Real>(superMin[0],
            superMax[1], superMin[2]);
        mSVertices[mSupervertices[3]] = Vector3<Real>(superMin[0],
            superMin[1], superMax[2]);

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

    Tetrahedron* tetra = new0 Tetrahedron(mSupervertices[0],
        mSupervertices[1], mSupervertices[2], mSupervertices[3]);
    mTetrahedra.insert(tetra);

    // Incrementally update the tetrahedralization.  The set of processed
    // points is maintained to eliminate duplicates, either in the original
    // input points or in the points obtained by snap rounding.
    std::set<Vector3<Real> > processed;
    for (i = 0; i < mNumVertices - 4; ++i)
    {
        if (processed.find(mSVertices[i]) == processed.end())
        {
            Update(i);
            processed.insert(mSVertices[i]);
        }
    }
    mNumUniqueVertices = (int)processed.size();

    // Remove tetrahedra sharing a vertex of the supertetrahedron.
    RemoveTetrahedra();

    // Assign integer values to the tetrahedra for use by the caller.
    std::map<Tetrahedron*,int> permute;
    typename std::set<Tetrahedron*>::iterator iter = mTetrahedra.begin();
    typename std::set<Tetrahedron*>::iterator end = mTetrahedra.end();
    for (i = 0; iter != end; ++iter)
    {
        tetra = *iter;
        permute[tetra] = i++;
    }
    permute[(Tetrahedron*)0] = -1;

    // Put Delaunay tetrahedra into an array (vertices and adjacency info).
    mNumSimplices = (int)mTetrahedra.size();
    if (mNumSimplices > 0)
    {
        mIndices = new1<int>(4*mNumSimplices);
        mAdjacencies = new1<int>(4*mNumSimplices);
        i = 0;
        iter = mTetrahedra.begin();
        end = mTetrahedra.end();
        for (/**/; iter != end; ++iter)
        {
            tetra = *iter;
            mIndices[i] = tetra->V[0];
            mAdjacencies[i] = permute[tetra->Adj[0]];
            ++i;
            mIndices[i] = tetra->V[1];
            mAdjacencies[i] = permute[tetra->Adj[1]];
            ++i;
            mIndices[i] = tetra->V[2];
            mAdjacencies[i] = permute[tetra->Adj[2]];
            ++i;
            mIndices[i] = tetra->V[3];
            mAdjacencies[i] = permute[tetra->Adj[3]];
            ++i;
        }
        assertion(i == 4*mNumSimplices, "Unexpected mismatch\n");

        mPathLast = -1;
        mPath = new1<int>(mNumSimplices + 1);
        memset(mPath, 0, (mNumSimplices + 1)*sizeof(int));
    }

    // Restore the vertex count to the original (discards the vertices of the
    // supertetrahedron).
    mNumVertices -= 4;

    iter = mTetrahedra.begin();
    end = mTetrahedra.end();
    for (/**/; iter != end; ++iter)
    {
        tetra = *iter;
        delete0(tetra);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay3<Real>::~Delaunay3 ()
{
    delete0(mQuery);
    delete1(mSVertices);
    delete1(mPath);
    if (mOwner)
    {
        delete1(mVertices);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* Delaunay3<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay3<Real>::GetNumUniqueVertices () const
{
    return mNumUniqueVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& Delaunay3<Real>::GetLineOrigin () const
{
    return mLineOrigin;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& Delaunay3<Real>::GetLineDirection () const
{
    return mLineDirection;
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay1<Real>* Delaunay3<Real>::GetDelaunay1 () const
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

    return new0 Delaunay1<Real>(mNumVertices, projection, mEpsilon, true,
        mQueryType);
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& Delaunay3<Real>::GetPlaneOrigin () const
{
    return mPlaneOrigin;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& Delaunay3<Real>::GetPlaneDirection (int i) const
{
    return mPlaneDirection[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay2<Real>* Delaunay3<Real>::GetDelaunay2 () const
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

    return new0 Delaunay2<Real>(mNumVertices, projection, mEpsilon, true,
        mQueryType);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::GetHull (int& numTriangles, int*& indices) const
{
    assertion(mDimension == 3, "The dimension must be 3\n");
    if (mDimension != 3)
    {
        return false;
    }

    numTriangles = 0;
    indices = 0;

    // Count the number of triangles that are not shared by two tetrahedra.
    int i, numAdjacent = 4*mNumSimplices;
    for (i = 0; i < numAdjacent; ++i)
    {
        if (mAdjacencies[i] == -1)
        {
            numTriangles++;
        }
    }
    assertion(numTriangles > 0, "There must be at least one tetrahedron\n");
    if (numTriangles == 0)
    {
        return false;
    }

    // Enumerate the triangles.  The prototypical case is the single
    // tetrahedron V[0] = (0,0,0), V[1] = (1,0,0), V[2] = (0,1,0), and
    // V[3] = (0,0,1) with no adjacent tetrahedra.  The mIndices[] array
    // is <0,1,2,3>.
    //   i = 0, face = 0:
    //     skip index 0, <x,1,2,3>, no swap, triangle = <1,2,3>
    //   i = 1, face = 1:
    //     skip index 1, <0,x,2,3>, swap,    triangle = <0,3,2>
    //   i = 2, face = 2:
    //     skip index 2, <0,1,x,3>, no swap, triangle = <0,1,3>
    //   i = 3, face = 3:
    //     skip index 3, <0,1,2,x>, swap,    triangle = <0,2,1>
    // To guarantee counterclockwise order of triangles when viewed outside
    // the tetrahedron, the swap of the last two indices occurs when
    // iFace is an odd number:  (iFace % 2) != 0
    indices = new1<int>(3*numTriangles);
    int* currentIndex = indices;
    for (i = 0; i < numAdjacent; ++i)
    {
        if (mAdjacencies[i] == -1)
        {
            int tetra = i/4, face = i%4;
            for (int j = 0; j < 4; ++j)
            {
                if (j != face)
                {
                    *currentIndex++ = mIndices[4*tetra + j];
                }
            }
            if ((face % 2) != 0)
            {
                int save = *(currentIndex-1);
                *(currentIndex-1) = *(currentIndex-2);
                *(currentIndex-2) = save;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay3<Real>::GetContainingTetrahedron (const Vector3<Real>& p) const
{
    assertion(mDimension == 3, "The dimension must be 3\n");
    if (mDimension != 3)
    {
        return -1;
    }

    // Convert to scaled coordinates.
    Vector3<Real> scP = (p - mMin)*mScale;

    // Start at first tetrahedron in mesh.
    int index = (mPathLast >= 0 ? mPath[mPathLast] : 0);
    mPathLast = -1;
    mLastFaceV0 = -1;
    mLastFaceV1 = -1;
    mLastFaceV2 = -1;
    mLastFaceOpposite = -1;
    mLastFaceOppositeIndex = -1;

    // Use tetrahedron faces as binary separating planes.
    for (int i = 0; i < mNumSimplices; ++i)
    {
        mPath[++mPathLast] = index;

        int* vertices = &mIndices[4*index];

        // <V1,V2,V3> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(scP, vertices[1], vertices[2], vertices[3]) > 0)
        {
            index = mAdjacencies[4*index];
            if (index == -1)
            {
                mLastFaceV0 = vertices[1];
                mLastFaceV1 = vertices[2];
                mLastFaceV2 = vertices[3];
                mLastFaceOpposite = vertices[0];
                mLastFaceOppositeIndex = 0;
                return -1;
            }
            continue;
        }

        // <V0,V3,V2> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(scP, vertices[0], vertices[2], vertices[3]) < 0)
        {
            index = mAdjacencies[4*index + 1];
            if (index == -1)
            {
                mLastFaceV0 = vertices[0];
                mLastFaceV1 = vertices[2];
                mLastFaceV2 = vertices[3];
                mLastFaceOpposite = vertices[1];
                mLastFaceOppositeIndex = 1;
                return -1;
            }
            continue;
        }

        // <V0,V1,V3> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(scP, vertices[0], vertices[1], vertices[3]) > 0)
        {
            index = mAdjacencies[4*index + 2];
            if (index == -1)
            {
                mLastFaceV0 = vertices[0];
                mLastFaceV1 = vertices[1];
                mLastFaceV2 = vertices[3];
                mLastFaceOpposite = vertices[2];
                mLastFaceOppositeIndex = 2;
                return -1;
            }
            continue;
        }

        // <V0,V2,V1> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(scP, vertices[0], vertices[1], vertices[2]) < 0)
        {
            index = mAdjacencies[4*index + 3];
            if (index == -1)
            {
                mLastFaceV0 = vertices[0];
                mLastFaceV1 = vertices[1];
                mLastFaceV2 = vertices[2];
                mLastFaceOpposite = vertices[3];
                mLastFaceOppositeIndex = 3;
                return -1;
            }
            continue;
        }

        mLastFaceV0 = -1;
        mLastFaceV1 = -1;
        mLastFaceV2 = -1;
        mLastFaceOppositeIndex = -1;
        return index;
    }

    return -1;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay3<Real>::GetPathLast () const
{
    return mPathLast;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* Delaunay3<Real>::GetPath () const
{
    return mPath;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay3<Real>::GetLastFace (int& v0, int& v1, int& v2, int& v3) const
{
    v0 = mLastFaceV0;
    v1 = mLastFaceV1;
    v2 = mLastFaceV2;
    v3 = mLastFaceOpposite;
    return mLastFaceOppositeIndex;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::GetVertexSet (int i, Vector3<Real> vertices[4]) const
{
    assertion(mDimension == 3, "The dimension must be 3\n");
    if (mDimension != 3)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        vertices[0] = mVertices[mIndices[4*i  ]];
        vertices[1] = mVertices[mIndices[4*i + 1]];
        vertices[2] = mVertices[mIndices[4*i + 2]];
        vertices[3] = mVertices[mIndices[4*i + 3]];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::GetIndexSet (int i, int indices[4]) const
{
    assertion(mDimension == 3, "The dimension must be 3\n");
    if (mDimension != 3)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        indices[0] = mIndices[4*i  ];
        indices[1] = mIndices[4*i + 1];
        indices[2] = mIndices[4*i + 2];
        indices[3] = mIndices[4*i + 3];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::GetAdjacentSet (int i, int adjacencies[4]) const
{
    assertion(mDimension == 3, "The dimension must be 3\n");
    if (mDimension != 3)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        adjacencies[0] = mAdjacencies[4*i  ];
        adjacencies[1] = mAdjacencies[4*i + 1];
        adjacencies[2] = mAdjacencies[4*i + 2];
        adjacencies[3] = mAdjacencies[4*i + 3];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::GetBarycentricSet (int i, const Vector3<Real>& p,
    Real bary[4]) const
{
    assertion(mDimension == 3, "The dimension must be 3\n");
    if (mDimension != 3)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        Vector3<Real> v0 = mVertices[mIndices[4*i  ]];
        Vector3<Real> v1 = mVertices[mIndices[4*i + 1]];
        Vector3<Real> v2 = mVertices[mIndices[4*i + 2]];
        Vector3<Real> v3 = mVertices[mIndices[4*i + 3]];
        p.GetBarycentrics(v0, v1, v2, v3, bary);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay3<Real>::Delaunay3 (const char* filename, int mode)
    :
    Delaunay<Real>(0, (Real)0, false, Query::QT_REAL),
    mVertices(0),
    mSVertices(0),
    mQuery(0),
    mPath(0)
{
    bool loaded = Load(filename, mode);
    assertion(loaded, "Cannot open file %s\n", filename);
    WM5_UNUSED(loaded);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::Load (const char* filename, int mode)
{
    FileIO inFile(filename, mode);
    if (!inFile)
    {
        return false;
    }

    Delaunay<Real>::Load(inFile);

    delete0(mQuery);
    delete1(mSVertices);
    delete1(mPath);
    if (mOwner)
    {
        delete1(mVertices);
    }

    mOwner = true;
    mVertices = new1<Vector3<Real> >(mNumVertices);
    mSVertices = new1<Vector3<Real> >(mNumVertices + 4);
    mPath = new1<int>(mNumSimplices + 1);

    inFile.Read(sizeof(int), &mNumUniqueVertices);
    inFile.Read(sizeof(int), 4, mSupervertices);
    inFile.Read(sizeof(int), &mPathLast);
    inFile.Read(sizeof(int), &mLastFaceV0);
    inFile.Read(sizeof(int), &mLastFaceV1);
    inFile.Read(sizeof(int), &mLastFaceV2);
    inFile.Read(sizeof(int), &mLastFaceOpposite);
    inFile.Read(sizeof(int), &mLastFaceOppositeIndex);
    inFile.Read(sizeof(int), mNumSimplices + 1, mPath);

    inFile.Read(sizeof(Real), 3*mNumVertices, mVertices);
    inFile.Read(sizeof(Real), 3*(mNumVertices + 4), mSVertices);
    inFile.Read(sizeof(Real), 3, &mMin);
    inFile.Read(sizeof(Real), 3, &mScale);
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
bool Delaunay3<Real>::Save (const char* filename, int mode) const
{
    FileIO outFile(filename, mode);
    if (!outFile)
    {
        return false;
    }

    Delaunay<Real>::Save(outFile);

    outFile.Write(sizeof(int), &mNumUniqueVertices);
    outFile.Write(sizeof(int), 4, mSupervertices);
    outFile.Write(sizeof(int), &mPathLast);
    outFile.Write(sizeof(int), &mLastFaceV0);
    outFile.Write(sizeof(int), &mLastFaceV1);
    outFile.Write(sizeof(int), &mLastFaceV2);
    outFile.Write(sizeof(int), &mLastFaceOpposite);
    outFile.Write(sizeof(int), &mLastFaceOpposite);
    outFile.Write(sizeof(int), mNumSimplices + 1 ,mPath);

    outFile.Write(sizeof(Real), 3*mNumVertices, mVertices);
    outFile.Write(sizeof(Real), 3*(mNumVertices + 4), mSVertices);
    outFile.Write(sizeof(Real), 3, &mMin);
    outFile.Write(sizeof(Real), 3, &mScale);
    outFile.Write(sizeof(Real), 3, &mLineOrigin);
    outFile.Write(sizeof(Real), 3, &mLineDirection);
    outFile.Write(sizeof(Real), 3, &mPlaneOrigin);
    outFile.Write(sizeof(Real), 6, mPlaneDirection);

    outFile.Close();
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void Delaunay3<Real>::Update (int i)
{
    // Locate the tetrahedron containing vertex i.
    Tetrahedron* tetra = GetContainingTetrahedron(i);

    // Locate and remove the tetrahedra forming the insertion polyhedron.
    std::stack<Tetrahedron*> tetraStack;
    ETManifoldMesh polyhedron(0, Face::TCreator);
    tetraStack.push(tetra);
    tetra->OnStack = true;
    int j, v0, v1, v2;
    Face* face;
    while (!tetraStack.empty())
    {
        tetra = tetraStack.top();
        tetraStack.pop();
        tetra->OnStack = false;
        for (j = 0; j < 4; ++j)
        {
            Tetrahedron* adj = tetra->Adj[j];
            if (adj)
            {
                // Detach tetrahedron and adjacent tetrahedron from each
                // other.
                int nullIndex = tetra->DetachFrom(j, adj);

                if (adj->IsInsertionComponent(i, tetra, mQuery,
                    mSupervertices))
                {
                    if (!adj->OnStack)
                    {
                        // Adjacent triangle inside insertion polyhedron.
                        tetraStack.push(adj);
                        adj->OnStack = true;
                    }
                }
                else
                {
                    // Adjacent tetrahedron outside insertion polyhedron.
                    v0 = tetra->V[gsIndex[j][0]];
                    v1 = tetra->V[gsIndex[j][1]];
                    v2 = tetra->V[gsIndex[j][2]];
                    face = (Face*)polyhedron.InsertTriangle(v0, v1, v2);
                    face->NullIndex = nullIndex;
                    face->Tetra = adj;
                }
            }
            else
            {
                // The tetrahedron is in the insertion polyhedron, but the
                // adjacent one does not exist.  This means one of two things:
                // (1) We are at a face of the supertetrahedron, and that
                //     face is part of the insertion polyhedron.
                // (2) We are at a face that was recently shared by the
                //     tetrahedron and the adjacent, but we detached those
                //     tetrahedra from each other.  These faces should be
                //     ignored.

                v0 = tetra->V[gsIndex[j][0]];
                if (IsSupervertex(v0))
                {
                    v1 = tetra->V[gsIndex[j][1]];
                    if (IsSupervertex(v1))
                    {
                        v2 = tetra->V[gsIndex[j][2]];
                        if (IsSupervertex(v2))
                        {
                            face = (Face*)polyhedron.InsertTriangle(v0, v1,
                                v2);
                            face->NullIndex = -1;
                            face->Tetra = 0;
                        }
                    }
                }
            }
        }
        mTetrahedra.erase(tetra);
        delete0(tetra);
    }

    // Insert the new tetrahedra formed by the input point and the faces of
    // the insertion polyhedron.
    const ETManifoldMesh::TMap& tetraMap = polyhedron.GetTriangles();
    assertion(tetraMap.size() >= 4 && polyhedron.IsClosed(),
        "Polyhedron must be at least a tetrahedron\n");
    typename ETManifoldMesh::TMapCIterator iter = tetraMap.begin();
    typename ETManifoldMesh::TMapCIterator end = tetraMap.end();
    for (/**/; iter != end; ++iter)
    {
        face = (Face*)iter->second;

        // Create and insert the new tetrahedron.
        tetra = new0 Tetrahedron(i, face->V[0], face->V[1], face->V[2]);
        mTetrahedra.insert(tetra);

        // Establish the adjacency links across the polyhedron face.
        tetra->Adj[0] = face->Tetra;
        if (face->Tetra)
        {
            face->Tetra->Adj[face->NullIndex] = tetra;
        }

        // Update the faces's tetrahedron pointer to point to the newly
        // created tetrahedron.  This information is used later to establish
        // the links between the new tetrahedra.
        face->Tetra = tetra;
    }

    // Establish the adjacency links between the new tetrahedra.
    Face* adjFace;
    iter = tetraMap.begin();
    end = tetraMap.end();
    for (/**/; iter != end; ++iter)
    {
        face = (Face*)iter->second;

        adjFace = (Face*)face->T[0];
        face->Tetra->Adj[3] = adjFace->Tetra;
        assertion(SharesFace(3, face->Tetra, adjFace->Tetra),
            "Face sharing is incorrect\n");

        adjFace = (Face*)face->T[1];
        face->Tetra->Adj[1] = adjFace->Tetra;
        assertion(SharesFace(1, face->Tetra, adjFace->Tetra),
            "Face sharing is incorrect\n");

        adjFace = (Face*)face->T[2];
        face->Tetra->Adj[2] = adjFace->Tetra;
        assertion(SharesFace(2, face->Tetra, adjFace->Tetra),
            "Face sharing is incorrect\n");
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Delaunay3<Real>::Tetrahedron*
Delaunay3<Real>::GetContainingTetrahedron (int i) const
{
    // Locate which tetrahedron in the current mesh contains vertex i.  By
    // construction, there must be such a tetrahedron (the vertex cannot be
    // outside the supertetrahedron).

    Tetrahedron* tetra = *mTetrahedra.begin();
    const int numTriangles = (int)mTetrahedra.size();
    for (int t = 0; t < numTriangles; ++t)
    {
        int* vertices = tetra->V;

        // <V1,V2,V3> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(i, vertices[1], vertices[2], vertices[3]) > 0)
        {
            tetra = tetra->Adj[0];
            if (!tetra)
            {
                break;
            }
            continue;
        }

        // <V0,V3,V2> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(i, vertices[0], vertices[2], vertices[3]) < 0)
        {
            tetra = tetra->Adj[1];
            if (!tetra)
            {
                break;
            }
            continue;
        }

        // <V0,V1,V3> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(i, vertices[0], vertices[1], vertices[3]) > 0)
        {
            tetra = tetra->Adj[2];
            if (!tetra)
            {
                break;
            }
            continue;
        }

        // <V0,V2,V1> counterclockwise when viewed outside tetrahedron.
        if (mQuery->ToPlane(i, vertices[0], vertices[1], vertices[2]) < 0)
        {
            tetra = tetra->Adj[3];
            if (!tetra)
            {
                break;
            }
            continue;
        }

        return tetra;
    }

    assertion(false, "Delaunay vertices must lie in some tetrahedron\n");
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
void Delaunay3<Real>::RemoveTetrahedra ()
{
    // Identify those triangles sharing a vertex of the supertetrahedron.
    std::set<Tetrahedron*> removeTetra;
    Tetrahedron* tetra;
    int j;

    typename std::set<Tetrahedron*>::iterator iter = mTetrahedra.begin();
    typename std::set<Tetrahedron*>::iterator end = mTetrahedra.end();
    for (/**/; iter != end; ++iter)
    {
        tetra = *iter;
        for (j = 0; j < 4; ++j)
        {
            if (IsSupervertex(tetra->V[j]))
            {
                removeTetra.insert(tetra);
                break;
            }
        }
    }

    // Remove the tetrahedra from the mesh.
    iter = removeTetra.begin();
    end = removeTetra.end();
    for (/**/; iter != end; ++iter)
    {
        tetra = *iter;
        for (j = 0; j < 4; ++j)
        {
            // Break the links with adjacent tetrahedra.
            Tetrahedron* adj = tetra->Adj[j];
            if (adj)
            {
                for (int k = 0; k < 4; ++k)
                {
                    if (adj->Adj[k] == tetra)
                    {
                        adj->Adj[k] = 0;
                        break;
                    }
                }
            }
        }
        mTetrahedra.erase(tetra);
        delete0(tetra);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::IsSupervertex (int i) const
{
    for (int j = 0; j < 4; j++)
    {
        if (i == mSupervertices[j])
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::SharesFace (int i, Tetrahedron* face, Tetrahedron* adj)
{
    int faceIndex[3], count = 0, j;
    for (j = 0; j < 4; ++j)
    {
        if (j != i)
        {
            faceIndex[count] = face->V[j];
            ++count;
        }
    }

    for (i = 0; i < 4; ++i)
    {
        if (adj->V[i] != faceIndex[0] &&
            adj->V[i] != faceIndex[1] &&
            adj->V[i] != faceIndex[2])
        {
            break;
        }
    }
    if (i == 4)
    {
        return false;
    }

    int adjIndex[3];
    for (j = 0, count = 0; j < 4; ++j)
    {
        if (j != i)
        {
            adjIndex[count] = adj->V[j];
            ++count;
        }
    }

    if (faceIndex[0] > faceIndex[1])
    {
        j = faceIndex[0];
        faceIndex[0] = faceIndex[1];
        faceIndex[1] = j;
    }
    if (faceIndex[1] > faceIndex[2])
    {
        j = faceIndex[1];
        faceIndex[1] = faceIndex[2];
        faceIndex[2] = j;
    }
    if (faceIndex[0] > faceIndex[1])
    {
        j = faceIndex[0];
        faceIndex[0] = faceIndex[1];
        faceIndex[1] = j;
    }

    if (adjIndex[0] > adjIndex[1])
    {
        j = adjIndex[0];
        adjIndex[0] = adjIndex[1];
        adjIndex[1] = j;
    }
    if (adjIndex[1] > adjIndex[2])
    {
        j = adjIndex[1];
        adjIndex[1] = adjIndex[2];
        adjIndex[2] = j;
    }
    if (adjIndex[0] > adjIndex[1])
    {
        j = adjIndex[0];
        adjIndex[0] = adjIndex[1];
        adjIndex[1] = j;
    }

    if (adjIndex[0] != faceIndex[0]
    ||  adjIndex[1] != faceIndex[1]
    ||  adjIndex[2] != faceIndex[2])
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Delaunay3::Tetrahedron
//----------------------------------------------------------------------------
template <typename Real>
Delaunay3<Real>::Tetrahedron::Tetrahedron (int v0, int v1, int v2, int v3)
{
    V[0] = v0;
    V[1] = v1;
    V[2] = v2;
    V[3] = v3;
    Adj[0] = 0;
    Adj[1] = 0;
    Adj[2] = 0;
    Adj[3] = 0;
    Time = -1;
    IsComponent = false;
    OnStack = false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay3<Real>::Tetrahedron::IsInsertionComponent (int i,
    Tetrahedron* adj, const Query3<Real>* query, const int* supervertices)
{
    if (i != Time)
    {
        Time = i;

        // Determine if the circumsphere of the tetrahedron contains the
        // input point.
        int relation = query->ToCircumsphere(i, V[0], V[1],
            V[2], V[3]);
        IsComponent = (relation <= 0);
        if (IsComponent)
        {
            return true;
        }

        // It is possible that a tetrahedron that shares a supervertex does
        // not have the circumsphere-containing property, but all faces of
        // it (other than the shared one with the calling tetrahedron) are
        // visible.  These are also included in the insertion polyhedron.
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                if (V[j] == supervertices[k])
                {
                    // Tetrahedron shares a supervertex.
                    int numInvisible = 0;
                    for (int k0 = 0; k0 < 4; ++k0)
                    {
                        if (Adj[k] != adj)
                        {
                            int v0 = V[gsIndex[k0][0]];
                            int v1 = V[gsIndex[k0][1]];
                            int v2 = V[gsIndex[k0][2]];
                            relation = query->ToPlane(i, v0, v1, v2);
                            if (relation > 0)
                            {
                                numInvisible++;
                            }
                        }
                    }
                    IsComponent = (numInvisible == 0);
                    return IsComponent;
                }
            }
        }
    }

    return IsComponent;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay3<Real>::Tetrahedron::DetachFrom (int adjIndex, Tetrahedron* adj)
{
    assertion(0 <= adjIndex && adjIndex < 4 && Adj[adjIndex] == adj,
        "Invalid input index.\n");

    Adj[adjIndex] = 0;
    for (int i = 0; i < 4; ++i)
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
// Delaunay3::Face
//----------------------------------------------------------------------------
template <typename Real>
Delaunay3<Real>::Face::Face (int v0, int v1, int v2, int nullIndex,
    Tetrahedron* tetra)
    :
    ETManifoldMesh::Triangle(v0, v1, v2),
    NullIndex(nullIndex),
    Tetra(tetra)
{
}
//----------------------------------------------------------------------------
template <typename Real>
ETManifoldMesh::TPtr Delaunay3<Real>::Face::TCreator (int v0, int v1, int v2)
{
    return new0 Face(v0, v1, v2, 0, 0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Delaunay3<float>;

template WM5_MATHEMATICS_ITEM
class Delaunay3<double>;
//----------------------------------------------------------------------------
}
