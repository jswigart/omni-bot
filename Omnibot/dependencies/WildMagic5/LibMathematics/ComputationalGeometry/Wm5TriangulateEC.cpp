// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2012/07/08)

#include "Wm5MathematicsPCH.h"
#include "Wm5TriangulateEC.h"
#include "Wm5Query2Filtered.h"
#include "Wm5Query2Int64.h"
#include "Wm5Query2Integer.h"
#include "Wm5Query2Rational.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
TriangulateEC<Real>::TriangulateEC (const Positions& positions,
    Query::Type queryType, Real epsilon, Indices& triangles)
{
    // No extra elements are needed for triangulating a simple polygon.
    InitializePositions(positions, queryType, epsilon, 0);

    // Triangulate the unindexed polygon.
    int numVertices = (int)positions.size();
    const int* indices = 0;
    InitializeVertices(numVertices, indices);
    DoEarClipping(numVertices, indices, triangles);
}
//----------------------------------------------------------------------------
template <typename Real>
TriangulateEC<Real>::TriangulateEC (const Positions& positions,
    Query::Type queryType, Real epsilon, const Indices& polygon,
    Indices& triangles)
{
    // No extra elements are needed for triangulating a simple polygon.
    InitializePositions(positions, queryType, epsilon, 0);

    // Triangulate the indexed polygon.
    int numVertices = (int)polygon.size();
    const int* indices = &polygon[0];
    InitializeVertices(numVertices, indices);
    DoEarClipping(numVertices, indices, triangles);
}
//----------------------------------------------------------------------------
template <typename Real>
TriangulateEC<Real>::TriangulateEC (const Positions& positions,
    Query::Type queryType, Real epsilon, const Indices& outer,
    const Indices& inner, Indices& triangles)
{
    // Two extra elements are needed to duplicate the endpoints of the edge
    // introduced to combine outer and inner polygons.
    InitializePositions(positions, queryType, epsilon, 2);

    // Combine the outer polygon and the inner polygon into a simple polygon
    // by inserting two edges connecting mutually visible vertices, one from
    // the outer polygon and one from the inner polygon.
    int nextElement = (int)positions.size();  // next available element
    IndexMap indexMap;
    Indices combined;
    CombinePolygons(queryType, epsilon, nextElement, outer, inner, indexMap,
        combined);

    // The combined polygon is now in the format of a simple polygon, albeit
    // one with coincident edges.
    int numVertices = (int)combined.size();
    const int* indices = &combined[0];
    InitializeVertices(numVertices, indices);
    DoEarClipping(numVertices, indices, triangles);

    // Map the duplicate indices back to the original indices.
    RemapIndices(indexMap, triangles);
}
//----------------------------------------------------------------------------
template <typename Real>
TriangulateEC<Real>::TriangulateEC (const Positions& positions,
    Query::Type queryType, Real epsilon, const Indices& outer,
    const IndicesArray& inners, Indices& triangles)
{
    // Two extra elements per inner polygon are needed to duplicate the
    // endpoints of the edges introduced to combine outer and inner polygons.
    int numInners = (int)inners.size();
    int extraElements = 2*numInners;
    InitializePositions(positions, queryType, epsilon, extraElements);

    // Combine the outer polygon and the inner polygons into a simple polygon
    // by inserting two edges per inner polygon connecting mutually visible
    // vertices.
    int nextElement = (int)positions.size();
    IndexMap indexMap;
    Indices combined;
    ProcessOuterAndInners(queryType, epsilon, outer, inners, nextElement,
        indexMap, combined);

    // The combined polygon is now in the format of a simple polygon, albeit
    // with coincident edges.
    int numVertices = (int)combined.size();
    const int* indices = &combined[0];
    InitializeVertices(numVertices, indices);
    DoEarClipping(numVertices, indices, triangles);

    // Map the duplicate indices back to the original indices.
    RemapIndices(indexMap, triangles);
}
//----------------------------------------------------------------------------
template <typename Real>
TriangulateEC<Real>::TriangulateEC (const Positions& positions,
    Query::Type queryType, Real epsilon, const Tree* tree,
    Indices& triangles)
{
    // Two extra elements per inner polygon are needed to duplicate the
    // endpoints of the edges introduced to combine outer and inner polygons.
    int extraElements = GetExtraElements(tree);
    InitializePositions(positions, queryType, epsilon, extraElements);

    int nextElement = (int)positions.size();
    IndexMap indexMap;

    std::queue<const Tree*> treeQueue;
    treeQueue.push(tree);
    while (treeQueue.size() > 0)
    {
        const Tree* outerNode = treeQueue.front();
        treeQueue.pop();

        int numChildren = (int)outerNode->Child.size();
        int numVertices;
        const int* indices;

        if (numChildren == 0)
        {
            // The outer polygon is a simple polygon (no nested inner
            // polygons).  Triangulate the simple polygon.
            numVertices = (int)outerNode->Polygon.size();
            indices = &outerNode->Polygon[0];
            InitializeVertices(numVertices, indices);
            DoEarClipping(numVertices, indices, triangles);
        }
        else
        {
            // Place the next level of outer polygon nodes on the queue for
            // triangulation.
            std::vector<std::vector<int>*> inners(numChildren);
            for (int i = 0; i < numChildren; ++i)
            {
                const Tree* innerNode = outerNode->Child[i];
                inners[i] = (std::vector<int>*)&innerNode->Polygon;
                int numGrandChildren = (int)innerNode->Child.size();
                for (int j = 0; j < numGrandChildren; ++j)
                {
                    treeQueue.push(innerNode->Child[j]);
                }
            }

            // Combine the outer polygon and the inner polygons into a
            // simple polygon by inserting two edges per inner polygon
            // connecting mutually visible vertices.
            Indices combined;
            ProcessOuterAndInners(queryType, epsilon, outerNode->Polygon,
                inners, nextElement, indexMap, combined);

            // The combined polygon is now in the format of a simple polygon,
            // albeit with coincident edges.
            numVertices = (int)combined.size();
            indices = &combined[0];
            InitializeVertices(numVertices, indices);
            DoEarClipping(numVertices, indices, triangles);
        }
    }

    // Map the duplicate indices back to the original indices.
    RemapIndices(indexMap, triangles);
}
//----------------------------------------------------------------------------
template <typename Real>
TriangulateEC<Real>::~TriangulateEC ()
{
    delete0(mQuery);
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::InitializePositions (const Positions& positions,
    Query::Type queryType, Real epsilon, int extraElements)
{
    int numPositions = (int)positions.size();
    assertion(numPositions >= 3, "Must have at least one triangle\n");
    int numPosExtras = numPositions + extraElements;
    mSPositions.resize(numPosExtras);

    if (queryType == Query::QT_FILTERED)
    {
        assertion((Real)0 <= epsilon && epsilon <= (Real)1,
            "Epsilon must be in [0,1]\n");
    }

    Vector2<Real> minValue, maxValue, range;
    Real scale, rmax;
    int i;

    switch (queryType)
    {
    case Query::QT_INT64:
    {
        // Transform the vertices to the square [0,2^{20}]^2.
        Vector2<Real>::ComputeExtremes(numPositions, &positions[0], minValue,
            maxValue);
        range = maxValue - minValue;
        rmax = (range[0] >= range[1] ? range[0] : range[1]);
        scale = ((Real)(1 << 20))/rmax;
        for (i = 0; i < numPositions; ++i)
        {
            mSPositions[i] = (positions[i] - minValue)*scale;
        }

        mQuery = new0 Query2Int64<Real>(numPosExtras, &mSPositions[0]);
        return;
    }

    case Query::QT_INTEGER:
    {
        // Transform the vertices to the square [0,2^{24}]^2.
        Vector2<Real>::ComputeExtremes(numPositions, &positions[0], minValue,
            maxValue);
        range = maxValue - minValue;
        rmax = (range[0] >= range[1] ? range[0] : range[1]);
        scale = ((Real)(1 << 24))/rmax;
        for (i = 0; i < numPositions; ++i)
        {
            mSPositions[i] = (positions[i] - minValue)*scale;
        }

        mQuery = new0 Query2Integer<Real>(numPosExtras, &mSPositions[0]);
        return;
    }

    case Query::QT_REAL:
    {
        // No transformation of the input data.  Make a copy that can be
        // expanded when triangulating polygons with holes.
        for (i = 0; i < numPositions; ++i)
        {
            mSPositions[i] = positions[i];
        }

        mQuery = new0 Query2<Real>(numPosExtras, &mSPositions[0]);
        return;
    }

    case Query::QT_RATIONAL:
    {
        // No transformation of the input data.  Make a copy that can be
        // expanded when triangulating polygons with holes.
        for (i = 0; i < numPositions; ++i)
        {
            mSPositions[i] = positions[i];
        }

        mQuery = new0 Query2Rational<Real>(numPosExtras, &mSPositions[0]);
        return;
    }

    case Query::QT_FILTERED:
    {
        // No transformation of the input data.  Make a copy that can be
        // expanded when triangulating polygons with holes.
        for (i = 0; i < numPositions; ++i)
        {
            mSPositions[i] = positions[i];
        }

        mQuery = new0 Query2Filtered<Real>(numPosExtras, &mSPositions[0],
            epsilon);
        return;
    }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::InitializeVertices (int numVertices,
    const int* indices)
{
    mVertices.clear();
    mVertices.resize(numVertices);
    mCFirst = -1;
    mCLast = -1;
    mRFirst = -1;
    mRLast = -1;
    mEFirst = -1;
    mELast = -1;

    // Create a circular list of the polygon vertices for dynamic removal of
    // vertices.
    int numVerticesM1 = numVertices - 1;
    int i;
    for (i = 0; i <= numVerticesM1; ++i)
    {
        Vertex& vertex = V(i);
        vertex.Index = (indices ? indices[i] : i);
        vertex.VPrev = (i > 0 ? i-1 : numVerticesM1);
        vertex.VNext = (i < numVerticesM1 ? i+1 : 0);
    }

    // Create a circular list of the polygon vertices for dynamic removal of
    // vertices.  Keep track of two linear sublists, one for the convex
    // vertices and one for the reflex vertices.  This is an O(N) process
    // where N is the number of polygon vertices.
    for (i = 0; i <= numVerticesM1; ++i)
    {
        if (IsConvex(i))
        {
            InsertAfterC(i);
        }
        else
        {
            InsertAfterR(i);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::DoEarClipping (int numVertices, const int* indices,
    Indices& triangles)
{
    // If the polygon is convex, just create a triangle fan.
    int i;
    if (mRFirst == -1)
    {
        int numVerticesM1 = numVertices - 1;
        if (indices)
        {
            for (i = 1; i < numVerticesM1; ++i)
            {
                triangles.push_back(indices[0]);
                triangles.push_back(indices[i]);
                triangles.push_back(indices[i + 1]);
            }
        }
        else
        {
            for (i = 1; i < numVerticesM1; ++i)
            {
                triangles.push_back(0);
                triangles.push_back(i);
                triangles.push_back(i + 1);
            }
        }
        return;
    }

    // Identify the ears and build a circular list of them.  Let V0, V1, and
    // V2 be consecutive vertices forming a triangle T.  The vertex V1 is an
    // ear if no other vertices of the polygon lie inside T.  Although it is
    // enough to show that V1 is not an ear by finding at least one other
    // vertex inside T, it is sufficient to search only the reflex vertices.
    // This is an O(C*R) process, where C is the number of convex vertices and
    // R is the number of reflex vertices with N = C+R.  The order is O(N^2),
    // for example when C = R = N/2.
    for (i = mCFirst; i != -1; i = V(i).SNext)
    {
        if (IsEar(i))
        {
            InsertEndE(i);
        }
    }
    V(mEFirst).EPrev = mELast;
    V(mELast).ENext = mEFirst;

    // Remove the ears, one at a time.
    bool bRemoveAnEar = true;
    while (bRemoveAnEar)
    {
        // Add the triangle with the ear to the output list of triangles.
        int iVPrev = V(mEFirst).VPrev;
        int iVNext = V(mEFirst).VNext;
        triangles.push_back(V(iVPrev).Index);
        triangles.push_back(V(mEFirst).Index);
        triangles.push_back(V(iVNext).Index);

        // Remove the vertex corresponding to the ear.
        RemoveV(mEFirst);
        if (--numVertices == 3)
        {
            // Only one triangle remains, just remove the ear and copy it.
            mEFirst = RemoveE(mEFirst);
            iVPrev = V(mEFirst).VPrev;
            iVNext = V(mEFirst).VNext;
            triangles.push_back(V(iVPrev).Index);
            triangles.push_back(V(mEFirst).Index);
            triangles.push_back(V(iVNext).Index);
            bRemoveAnEar = false;
            continue;
        }

        // Removal of the ear can cause an adjacent vertex to become an ear
        // or to stop being an ear.
        Vertex& VPrev = V(iVPrev);
        if (VPrev.IsEar)
        {
            if (!IsEar(iVPrev))
            {
                RemoveE(iVPrev);
            }
        }
        else
        {
            bool wasReflex = !VPrev.IsConvex;
            if (IsConvex(iVPrev))
            {
                if (wasReflex)
                {
                    RemoveR(iVPrev);
                }

                if (IsEar(iVPrev))
                {
                    InsertBeforeE(iVPrev);
                }
            }
        }

        Vertex& VNext = V(iVNext);
        if (VNext.IsEar)
        {
            if (!IsEar(iVNext))
            {
                RemoveE(iVNext);
            }
        }
        else
        {
            bool wasReflex = !VNext.IsConvex;
            if (IsConvex(iVNext))
            {
                if (wasReflex)
                {
                    RemoveR(iVNext);
                }

                if (IsEar(iVNext))
                {
                    InsertAfterE(iVNext);
                }
            }
        }

        // Remove the ear.
        mEFirst = RemoveE(mEFirst);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int TriangulateEC<Real>::TriangleQuery (const Vector2<Real>& position,
    Query::Type queryType, Real epsilon, const Vector2<Real> triangle[3])
    const
{
    switch (queryType)
    {
    case Query::QT_INT64:
        return Query2Int64<Real>(3,triangle).ToTriangle(position, 0, 1, 2);

    case Query::QT_INTEGER:
        return Query2Integer<Real>(3,triangle).ToTriangle(position, 0, 1, 2);

    case Query::QT_REAL:
        return Query2<Real>(3,triangle).ToTriangle(position, 0, 1, 2);

    case Query::QT_RATIONAL:
        return Query2Rational<Real>(3,triangle).ToTriangle(position, 0, 1, 2);

    case Query::QT_FILTERED:
        return Query2Filtered<Real>(3,triangle,epsilon).ToTriangle(
            position, 0, 1, 2);
    }

    return 1;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::CombinePolygons (Query::Type queryType,
    Real epsilon, int nextElement, const Indices& outer,
    const Indices& inner, IndexMap& indexMap, Indices& combined)
{
    int numOuterVertices = (int)outer.size();
    int numInnerVertices = (int)inner.size();

    // Locate the inner-polygon vertex of maximum x-value, call this vertex M.
    Real xmax = mSPositions[inner[0]][0];
    int xmaxIndex = 0;
    int i;
    for (i = 1; i < numInnerVertices; ++i)
    {
        Real x = mSPositions[inner[i]][0];
        if (x > xmax)
        {
            xmax = x;
            xmaxIndex = i;
        }
    }
    Vector2<Real> M = mSPositions[inner[xmaxIndex]];

    // Find the edge whose intersection Intr with the ray M+t*(1,0) minimizes
    // the ray parameter t >= 0.
    Vector2<Real> intr(Math<Real>::MAX_REAL, M[1]);
    int v0min = -1, v1min = -1, endMin = -1;
    int i0, i1;
    Real s = Math<Real>::MAX_REAL;
    Real t = Math<Real>::MAX_REAL;
    for (i0 = numOuterVertices - 1, i1 = 0; i1 < numOuterVertices; i0 = i1++)
    {
        // Only consider edges for which the first vertex is below (or on)
        // the ray and the second vertex is above (or on) the ray.
        Vector2<Real> diff0 = mSPositions[outer[i0]] - M;
        if (diff0[1] > (Real)0)
        {
            continue;
        }
        Vector2<Real> diff1 = mSPositions[outer[i1]] - M;
        if (diff1[1] < (Real)0)
        {
            continue;
        }

        // At this time, diff0.Y() <= 0 and diff1.Y() >= 0.
        int currentEndMin = -1;
        if (diff0[1] < (Real)0)
        {
            if (diff1[1] > (Real)0)
            {
                // The intersection of the edge and ray occurs at an interior
                // edge point.
                s = diff0[1]/(diff0[1] - diff1[1]);
                t = diff0[0] + s*(diff1[0] - diff0[0]);
            }
            else  // diff1.Y() == 0
            {
                // The vertex Outer[i1] is the intersection of the edge and
                // the ray.
                t = diff1[0];
                currentEndMin = i1;
            }
        }
        else  // diff0.Y() == 0
        {
            if (diff1[1] > (Real)0)
            {
                // The vertex Outer[i0] is the intersection of the edge and
                // the ray;
                t = diff0[0];
                currentEndMin = i0;
            }
            else  // diff1.Y() == 0
            {
                if (diff0[0] < diff1[0])
                {
                    t = diff0[0];
                    currentEndMin = i0;
                }
                else
                {
                    t = diff1[0];
                    currentEndMin = i1;
                }
            }
        }

        if ((Real)0 <= t && t < intr[0])
        {
            intr[0] = t;
            v0min = i0;
            v1min = i1;
            if (currentEndMin == -1)
            {
                // The current closest point is an edge-interior point.
                endMin = -1;
            }
            else
            {
                // The current closest point is a vertex.
                endMin = currentEndMin;
            }
        }
        else if (t == intr[0])
        {
            // The current closest point is a vertex shared by multiple edges;
            // thus, endMin and currentMin refer to the same point.
            assertion(endMin != -1 && currentEndMin != -1,
                "Unexpected condition\n");

            // We need to select the edge closest to M.  The previous closest
            // edge is <outer[v0min],outer[v1min]>.  The current candidate is
            // <outer[i0],outer[i1]>.
            Vector2<Real> shared = mSPositions[outer[i1]];
            
            // For the previous closest edge, endMin refers to a vertex of
            // the edge.  Get the index of the other vertex.
            int other = (endMin == v0min ? v1min : v0min);

            // The new edge is closer if the other vertex of the old edge is
            // left-of the new edge.
            diff0 = mSPositions[outer[i0]] - shared;
            diff1 = mSPositions[outer[other]] - shared;
            Real dotperp = diff0.DotPerp(diff1);
            if (dotperp > (Real)0)
            {
                // The new edge is closer to M.
                v0min = i0;
                v1min = i1;
                endMin = currentEndMin;
            }
        }
    }

    // The intersection intr[0] stored only the t-value of the ray.  The
    // actual point is (mx,my)+t*(1,0), so intr[0] must be adjusted.
    intr[0] += M[0];

    int maxCosIndex;
    if (endMin == -1)
    {
        // If you reach this assert, there is a good chance that you have two
        // inner polygons that share a vertex or an edge.
        assertion(v0min >= 0 && v1min >= 0, "Unexpected condition\n");

        // Select one of Outer[v0min] and Outer[v1min] that has an x-value
        // larger than M.X(), call this vertex P.  The triangle <M,I,P> must
        // contain an outer-polygon vertex that is visible to M, which is
        // possibly P itself.
        Vector2<Real> sTriangle[3];  // <P,M,I> or <P,I,M>
        int pIndex;
        if (mSPositions[outer[v0min]][0] > mSPositions[outer[v1min]][0])
        {
            sTriangle[0] = mSPositions[outer[v0min]];
            sTriangle[1] = intr;
            sTriangle[2] = M;
            pIndex = v0min;
        }
        else
        {
            sTriangle[0] = mSPositions[outer[v1min]];
            sTriangle[1] = M;
            sTriangle[2] = intr;
            pIndex = v1min;
        }

        // If any outer-polygon vertices other than P are inside the triangle
        // <M,I,P>, then at least one of these vertices must be a reflex
        // vertex.  It is sufficient to locate the reflex vertex R (if any)
        // in <M,I,P> that minimizes the angle between R-M and (1,0).  The
        // data member mQuery is used for the reflex query.
        Vector2<Real> diff = sTriangle[0] - M;
        Real maxSqrLen = diff.SquaredLength();
        Real maxCos = diff[0]*diff[0]/maxSqrLen;
        maxCosIndex = pIndex;
        for (i = 0; i < numOuterVertices; ++i)
        {
            if (i == pIndex)
            {
                continue;
            }

            int curr = outer[i];
            int prev = outer[(i + numOuterVertices - 1) % numOuterVertices];
            int next = outer[(i + 1) % numOuterVertices];
            if (mQuery->ToLine(curr, prev, next) <= 0
            &&  TriangleQuery(mSPositions[curr], queryType, epsilon,
                    sTriangle) <= 0)
            {
                // The vertex is reflex and inside the <M,I,P> triangle.
                diff = mSPositions[curr] - M;
                Real sqrLen = diff.SquaredLength();
                Real cs = diff[0]*diff[0]/sqrLen;
                if (cs > maxCos)
                {
                    // The reflex vertex forms a smaller angle with the
                    // positive x-axis, so it becomes the new visible
                    // candidate.
                    maxSqrLen = sqrLen;
                    maxCos = cs;
                    maxCosIndex = i;
                }
                else if (cs == maxCos && sqrLen < maxSqrLen)
                {
                    // The reflex vertex has angle equal to the current
                    // minimum but the length is smaller, so it becomes the
                    // new visible candidate.
                    maxSqrLen = sqrLen;
                    maxCosIndex = i;
                }
            }
        }
    }
    else
    {
        maxCosIndex = endMin;
    }

    // The visible vertices are Position[Inner[xmaxIndex]] and
    // Position[Outer[maxCosIndex]].  Two coincident edges with these
    // endpoints are inserted to connect the outer and inner polygons into a
    // simple polygon.  Each of the two Position[] values must be duplicated,
    // because the original might be convex (or reflex) and the duplicate is
    // reflex (or convex).  The ear-clipping algorithm needs to distinguish
    // between them.
    combined.resize(numOuterVertices + numInnerVertices + 2);
    int cIndex = 0;
    for (i = 0; i <= maxCosIndex; ++i, ++cIndex)
    {
        combined[cIndex] = outer[i];
    }

    for (i = 0; i < numInnerVertices; ++i, ++cIndex)
    {
        int j = (xmaxIndex + i) % numInnerVertices;
        combined[cIndex] = inner[j];
    }

    int innerIndex = inner[xmaxIndex];
    mSPositions[nextElement] = mSPositions[innerIndex];
    combined[cIndex] = nextElement;
    IndexMap::iterator iter = indexMap.find(innerIndex);
    if (iter != indexMap.end())
    {
        innerIndex = iter->second;
    }
    indexMap[nextElement] = innerIndex;
    ++cIndex;
    ++nextElement;

    int outerIndex = outer[maxCosIndex];
    mSPositions[nextElement] = mSPositions[outerIndex];
    combined[cIndex] = nextElement;
    iter = indexMap.find(outerIndex);
    if (iter != indexMap.end())
    {
        outerIndex = iter->second;
    }
    indexMap[nextElement] = outerIndex;
    ++cIndex;
    ++nextElement;

    for (i = maxCosIndex + 1; i < numOuterVertices; ++i, ++cIndex)
    {
        combined[cIndex] = outer[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::ProcessOuterAndInners (Query::Type queryType,
    Real epsilon, const Indices& outer, const IndicesArray& inners,
    int& nextElement, IndexMap& indexMap, Indices& combined)
{
    // Sort the inner polygons based on maximum x-values.
    int numInners = (int)inners.size();
    std::vector<std::pair<Real,int> > pairs(numInners);
    int i;
    for (i = 0; i < numInners; ++i)
    {
        const Indices& inner = *inners[i];
        int numVertices = (int)inner.size();
        Real xmax = mSPositions[inner[0]][0];
        for (int j = 1; j < numVertices; ++j)
        {
            Real x = mSPositions[inner[j]][0];
            if (x > xmax)
            {
                xmax = x;
            }
        }
        pairs[i].first = xmax;
        pairs[i].second = i;
    }
    std::sort(pairs.begin(),pairs.end());

    // Merge the inner polygons with the outer polygon.
    Indices currentOuter = outer;
    for (i = numInners - 1; i >= 0; --i)
    {
        const Indices& inner = *inners[pairs[i].second];
        Indices currentCombined;
        CombinePolygons(queryType, epsilon, nextElement, currentOuter,
            inner, indexMap, currentCombined);
        currentOuter = currentCombined;
        nextElement += 2;
    }

    for (i = 0; i < (int)currentOuter.size(); ++i)
    {
        combined.push_back(currentOuter[i]);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::RemapIndices (const IndexMap& indexMap,
    Indices& triangles) const
{
    // The triangulation includes indices to the duplicated outer and inner
    // vertices.  These indices must be mapped back to the original ones.
    const int numTriangles = (int)triangles.size();
    for (int i = 0; i < numTriangles; ++i)
    {
        IndexMap::const_iterator iter = indexMap.find(triangles[i]);
        if (iter != indexMap.end())
        {
            triangles[i] = iter->second;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Vertex list handling
//----------------------------------------------------------------------------
template <typename Real>
typename TriangulateEC<Real>::Vertex& TriangulateEC<Real>::V (int i)
{
    return mVertices[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool TriangulateEC<Real>::IsConvex (int i)
{
    Vertex& vertex = V(i);
    int curr = vertex.Index;
    int prev = V(vertex.VPrev).Index;
    int next = V(vertex.VNext).Index;
    vertex.IsConvex = (mQuery->ToLine(curr, prev, next) > 0);
    return vertex.IsConvex;
}
//----------------------------------------------------------------------------
template <typename Real>
bool TriangulateEC<Real>::IsEar (int i)
{
    Vertex& vertex = V(i);

    if (mRFirst == -1)
    {
        // The remaining polygon is convex.
        vertex.IsEar = true;
        return true;
    }

    // Search the reflex vertices and test if any are in the triangle
    // <V[prev],V[curr],V[next]>.
    int prev = V(vertex.VPrev).Index;
    int curr = vertex.Index;
    int next = V(vertex.VNext).Index;
    vertex.IsEar = true;
    for (int j = mRFirst; j != -1; j = V(j).SNext)
    {
        // Check if the test vertex is already one of the triangle vertices.
        if (j == vertex.VPrev || j == i || j == vertex.VNext)
        {
            continue;
        }

        // V[j] has been ruled out as one of the original vertices of the
        // triangle <V[prev],V[curr],V[next]>.  When triangulating polygons
        // with holes, V[j] might be a duplicated vertex, in which case it
        // does not affect the earness of V[curr].
        int test = V(j).Index;
        if (mSPositions[test] == mSPositions[prev]
        ||  mSPositions[test] == mSPositions[curr]
        ||  mSPositions[test] == mSPositions[next])
        {
            continue;
        }

        // Test if the vertex is inside or on the triangle.  When it is, it
        // causes V[curr] not to be an ear.
        if (mQuery->ToTriangle(test, prev, curr, next) <= 0)
        {
            vertex.IsEar = false;
            break;
        }
    }

    return vertex.IsEar;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::InsertAfterC (int i)
{
    if (mCFirst == -1)
    {
        // add first convex vertex
        mCFirst = i;
    }
    else
    {
        V(mCLast).SNext = i;
        V(i).SPrev = mCLast;
    }
    mCLast = i;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::InsertAfterR (int i)
{
    if (mRFirst == -1)
    {
        // add first reflex vertex
        mRFirst = i;
    }
    else
    {
        V(mRLast).SNext = i;
        V(i).SPrev = mRLast;
    }
    mRLast = i;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::InsertEndE (int i)
{
    if (mEFirst == -1)
    {
        // add first ear
        mEFirst = i;
        mELast = i;
    }
    V(mELast).ENext = i;
    V(i).EPrev = mELast;
    mELast = i;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::InsertAfterE (int i)
{
    Vertex& first = V(mEFirst);
    int currENext = first.ENext;
    Vertex& vertex = V(i);
    vertex.EPrev = mEFirst;
    vertex.ENext = currENext;
    first.ENext = i;
    V(currENext).EPrev = i;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::InsertBeforeE (int i)
{
    Vertex& first = V(mEFirst);
    int currEPrev = first.EPrev;
    Vertex& vertex = V(i);
    vertex.EPrev = currEPrev;
    vertex.ENext = mEFirst;
    first.EPrev = i;
    V(currEPrev).ENext = i;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::RemoveV (int i)
{
    int currVPrev = V(i).VPrev;
    int currVNext = V(i).VNext;
    V(currVPrev).VNext = currVNext;
    V(currVNext).VPrev = currVPrev;
}
//----------------------------------------------------------------------------
template <typename Real>
int TriangulateEC<Real>::RemoveE (int i)
{
    int currEPrev = V(i).EPrev;
    int currENext = V(i).ENext;
    V(currEPrev).ENext = currENext;
    V(currENext).EPrev = currEPrev;
    return currENext;
}
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::RemoveR (int i)
{
    assertion(mRFirst != -1 && mRLast != -1, "Reflex vertices must exist\n");

    if (i == mRFirst)
    {
        mRFirst = V(i).SNext;
        if (mRFirst != -1)
        {
            V(mRFirst).SPrev = -1;
        }
        V(i).SNext = -1;
    }
    else if (i == mRLast)
    {
        mRLast = V(i).SPrev;
        if (mRLast != -1)
        {
            V(mRLast).SNext = -1;
        }
        V(i).SPrev = -1;
    }
    else
    {
        int currSPrev = V(i).SPrev;
        int currSNext = V(i).SNext;
        V(currSPrev).SNext = currSNext;
        V(currSNext).SPrev = currSPrev;
        V(i).SNext = -1;
        V(i).SPrev = -1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Tree support.
//----------------------------------------------------------------------------
template <typename Real>
void TriangulateEC<Real>::Delete (Tree*& root)
{
    if (root)
    {
        std::queue<Tree*> treeQueue;
        treeQueue.push(root);

        while (treeQueue.size() > 0)
        {
            Tree* tree = treeQueue.front();
            treeQueue.pop();
            const int numChildren = (int)tree->Child.size();
            for (int i = 0; i < numChildren; ++i)
            {
               treeQueue.push(tree->Child[i]);
            }
            delete0(tree);
        }

        root = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int TriangulateEC<Real>::GetExtraElements (const Tree* tree)
{
    int extraElements = 0;

    std::queue<const Tree*> treeQueue;
    treeQueue.push(tree);
    while (treeQueue.size() > 0)
    {
        const Tree* root = treeQueue.front();
        treeQueue.pop();
        const int numChildren = (int)root->Child.size();
        extraElements += 2*numChildren;

        for (int i = 0; i < numChildren; ++i)
        {
            const Tree* child = root->Child[i];
            const int numGrandChildren = (int)child->Child.size();
            for (int j = 0; j < numGrandChildren; ++j)
            {
                treeQueue.push(child->Child[j]);
            }
        }
    }

    return extraElements;
}
//----------------------------------------------------------------------------
template <typename Real>
TriangulateEC<Real>::Vertex::Vertex ()
    :
    Index(-1),
    IsConvex(false),
    IsEar(false),
    VPrev(-1),
    VNext(-1),
    SPrev(-1),
    SNext(-1),
    EPrev(-1),
    ENext(-1)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class TriangulateEC<float>;

template WM5_MATHEMATICS_ITEM
class TriangulateEC<double>;
//----------------------------------------------------------------------------
}
