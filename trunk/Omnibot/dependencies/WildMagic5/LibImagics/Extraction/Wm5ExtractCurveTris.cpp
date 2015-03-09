// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5ExtractCurveTris.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
ExtractCurveTris::ExtractCurveTris (int xBound,  int yBound,  int* data)
    :
    mXBound(xBound), 
    mYBound(yBound), 
    mQuantity(xBound*yBound), 
    mData(data)
{
}
//----------------------------------------------------------------------------
void ExtractCurveTris::ExtractContour (int level, 
    std::vector<Vector2f>& vertices,  std::vector<EdgeKey>& edges)
{
    // Adjust the image so that the level set is F(x, y) = 0.
    int i;
    for (i = 0; i < mQuantity; ++i)
    {
        mData[i] -= level;
    }

    // The vertices are computed as rational numbers.
    std::vector<Vertex> ratVertices;

    int xBoundM1 = mXBound - 1,  yBoundM1 = mYBound - 1;
    for (int y = 0,  yp = 1; y < yBoundM1; ++y,  ++yp)
    {
        int yParity = (y & 1);

        for (int x = 0,  xp = 1; x < xBoundM1; ++x,  ++xp)
        {
            int xParity = (x & 1);

            // Get the image values at the corners of the square.
            int i00 = x + mXBound*y;
            int i10 = i00 + 1;
            int i01 = i00 + mXBound;
            int i11 = i10 + mXBound;
            int f00 = mData[i00];
            int f10 = mData[i10];
            int f01 = mData[i01];
            int f11 = mData[i11];

            if (xParity == yParity)
            {
                ProcessTriangle(ratVertices,  edges,  x,  y,  f00,  x,  yp,  f01, 
                    xp,  y,  f10);

                ProcessTriangle(ratVertices,  edges,  xp,  yp,  f11,  xp,  y,  f10, 
                    x,  yp,  f01);
            }
            else
            {
                ProcessTriangle(ratVertices,  edges,  x,  yp,  f01,  xp,  yp,  f11, 
                    x,  y,  f00);

                ProcessTriangle(ratVertices,  edges,  xp,  y,  f10,  x,  y,  f00, 
                    xp,  yp,  f11);
            }
        }
    }

    // Convert the rational vertices to floating-point vertices.
    int numVertices = (int)ratVertices.size();
    vertices.resize(numVertices);
    for (i = 0; i < numVertices; ++i)
    {
        vertices[i][0] = ((float)ratVertices[i].XNumer) /
            (float)ratVertices[i].XDenom;

        vertices[i][1] = ((float)ratVertices[i].YNumer) /
            (float)ratVertices[i].YDenom;
    }

    // Restore the image values.
    for (i = 0; i < mQuantity; i++)
    {
        mData[i] += level;
    }
}
//----------------------------------------------------------------------------
void ExtractCurveTris::MakeUnique (std::vector<Vector2f>& vertices, 
    std::vector<EdgeKey>& edges)
{
    int numVertices = (int)vertices.size();
    if (numVertices == 0)
    {
        return;
    }

    // Use maps to generate unique storage.
    typedef std::map<Vector2f, int> VMap;
    typedef std::map<Vector2f, int>::iterator VIterator;
    VMap vertexMap;
    for (int v = 0,  nextVertex = 0; v < numVertices; ++v)
    {
        std::pair<VIterator, bool> result = vertexMap.insert(
            std::make_pair(vertices[v],  nextVertex));

        if (result.second == true)
        {
            ++nextVertex;
        }
    }

    typedef std::map<EdgeKey, int> EMap;
    typedef std::map<EdgeKey, int>::iterator EIterator;
    EMap* edgeMap = 0;
    int e;
    VIterator vIter;

    int numEdges = (int)edges.size();
    if (numEdges)
    {
        edgeMap = new0 EMap();
        int nextEdge = 0;
        for (e = 0; e < numEdges; ++e)
        {
            // Replace old vertex indices by new ones.
            vIter = vertexMap.find(vertices[edges[e].V[0]]);
            assertion(vIter != vertexMap.end(),  "Unexpected condition\n");
            edges[e].V[0] = vIter->second;
            vIter = vertexMap.find(vertices[edges[e].V[1]]);
            assertion(vIter != vertexMap.end(),  "Unexpected condition\n");
            edges[e].V[1] = vIter->second;

            // Keep only unique edges.
            std::pair<EIterator, bool> result = edgeMap->insert(
                std::make_pair(edges[e],  nextEdge));

            if (result.second == true)
            {
                ++nextEdge;
            }
        }
    }

    // Pack the vertices into an array.
    numVertices = (int)vertexMap.size();
    vertices.resize(numVertices);
    for (vIter = vertexMap.begin(); vIter != vertexMap.end(); ++vIter)
    {
        vertices[vIter->second] = vIter->first;
    }

    // Pack the edges into an array.
    if (numEdges > 0)
    {
        numEdges = (int)edgeMap->size();
        edges.resize(numEdges);
        EIterator eIter;
        for (eIter = edgeMap->begin(); eIter != edgeMap->end(); ++eIter)
        {
            edges[eIter->second] = eIter->first;
        }
        delete0(edgeMap);
    }
    else
    {
        edges.clear();
    }
}
//----------------------------------------------------------------------------
void ExtractCurveTris::AddVertex (std::vector<Vertex>& vertices,  int xNumer, 
    int xDenom,  int yNumer,  int yDenom)
{
    vertices.push_back(Vertex(xNumer,  xDenom,  yNumer,  yDenom));
}
//----------------------------------------------------------------------------
void ExtractCurveTris::AddEdge (std::vector<Vertex>& vertices, 
    std::vector<EdgeKey>& edges,  int xNumer0,  int xDenom0,  int yNumer0, 
    int yDenom0,  int xNumer1,  int xDenom1,  int yNumer1,  int yDenom1)
{
    int v0 = (int)vertices.size();
    int v1 = v0 + 1;
    edges.push_back(EdgeKey(v0,  v1));
    vertices.push_back(Vertex(xNumer0,  xDenom0,  yNumer0,  yDenom0));
    vertices.push_back(Vertex(xNumer1,  xDenom1,  yNumer1,  yDenom1));
}
//----------------------------------------------------------------------------
void ExtractCurveTris::ProcessTriangle (std::vector<Vertex>& vertices, 
    std::vector<EdgeKey>& edges,  int x0,  int y0,  int f0,  int x1,  int y1, 
    int f1,  int x2,  int y2,  int f2)
{
    int xn0,  yn0,  xn1,  yn1,  d0,  d1;

    if (f0 != 0)
    {
        // convert to case "+**"
        if (f0 < 0)
        {
            f0 = -f0;
            f1 = -f1;
            f2 = -f2;
        }

        if (f1 > 0)
        {
            if (f2 > 0)
            {
                // +++
                return;
            }
            else if (f2 < 0)
            {
                // ++-
                d0 = f0 - f2;
                xn0 = f0*x2 - f2*x0;
                yn0 = f0*y2 - f2*y0;
                d1 = f1 - f2;
                xn1 = f1*x2 - f2*x1;
                yn1 = f1*y2 - f2*y1;
                AddEdge(vertices, edges, xn0, d0, yn0, d0, xn1, d1, yn1, d1);
            }
            else
            {
                // ++0
                AddVertex(vertices, x2, 1, y2, 1);
            }
        }
        else if (f1 < 0)
        {
            d0 = f0 - f1;
            xn0 = f0*x1 - f1*x0;
            yn0 = f0*y1 - f1*y0;

            if (f2 > 0)
            {
                // +-+
                d1 = f2 - f1;
                xn1 = f2*x1 - f1*x2;
                yn1 = f2*y1 - f1*y2;
                AddEdge(vertices, edges, xn0, d0, yn0, d0, xn1, d1, yn1, d1);
            }
            else if (f2 < 0)
            {
                // +--
                d1 = f2 - f0;
                xn1 = f2*x0 - f0*x2;
                yn1 = f2*y0 - f0*y2;
                AddEdge(vertices, edges, xn0, d0, yn0, d0, xn1, d1, yn1, d1);
            }
            else
            {
                // +-0
                AddEdge(vertices, edges, x2, 1, y2, 1, xn0, d0, yn0, d0);
            }
        }
        else
        {
            if (f2 > 0)
            {
                // +0+
                AddVertex(vertices, x1, 1, y1, 1);
            }
            else if (f2 < 0)
            {
                // +0-
                d0 = f2 - f0;
                xn0 = f2*x0 - f0*x2;
                yn0 = f2*y0 - f0*y2;
                AddEdge(vertices, edges, x1, 1, y1, 1, xn0, d0, yn0, d0);
            }
            else
            {
                // +00
                AddEdge(vertices, edges, x1, 1, y1, 1, x2, 1, y2, 1);
            }
        }
    }
    else if (f1 != 0)
    {
        // convert to case 0+*
        if (f1 < 0)
        {
            f1 = -f1;
            f2 = -f2;
        }

        if (f2 > 0)
        {
            // 0++
            AddVertex(vertices, x0, 1, y0, 1);
        }
        else if (f2 < 0)
        {
            // 0+-
            d0 = f1 - f2;
            xn0 = f1*x2 - f2*x1;
            yn0 = f1*y2 - f2*y1;
            AddEdge(vertices, edges, x0, 1, y0, 1, xn0, d0, yn0, d0);
        }
        else
        {
            // 0+0
            AddEdge(vertices, edges, x0, 1, y0, 1, x2, 1, y2, 1);
        }
    }
    else if (f2 != 0)
    {
        // cases 00+ or 00-
        AddEdge(vertices, edges, x0, 1, y0, 1, x1, 1, y1, 1);
    }
    else
    {
        // case 000
        AddEdge(vertices, edges, x0, 1, y0, 1, x1, 1, y1, 1);
        AddEdge(vertices, edges, x1, 1, y1, 1, x2, 1, y2, 1);
        AddEdge(vertices, edges, x2, 1, y2, 1, x0, 1, y0, 1);
    }
}
//----------------------------------------------------------------------------
ExtractCurveTris::Vertex::Vertex (int xNumer,  int xDenom,  int yNumer, 
    int yDenom)
{
    if (xDenom > 0)
    {
        XNumer = xNumer;
        XDenom = xDenom;
    }
    else
    {
        XNumer = -xNumer;
        XDenom = -xDenom;
    }

    if (yDenom > 0)
    {
        YNumer = yNumer;
        YDenom = yDenom;
    }
    else
    {
        YNumer = -yNumer;
        YDenom = -yDenom;
    }
}
//----------------------------------------------------------------------------
