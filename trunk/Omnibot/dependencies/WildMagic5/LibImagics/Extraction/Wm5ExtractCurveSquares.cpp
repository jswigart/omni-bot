// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5ExtractCurveSquares.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
ExtractCurveSquares::ExtractCurveSquares (int xBound, int yBound, int* data)
    :
    mXBound(xBound),
    mYBound(yBound),
    mQuantity(xBound*yBound),
    mData(data)
{
}
//----------------------------------------------------------------------------
void ExtractCurveSquares::ExtractContour (int level,
    std::vector<Vector2f>& vertices, std::vector<EdgeKey>& edges)
{
    // Adjust the image so that the level set is F(x,y) = 0.
    int i;
    for (i = 0; i < mQuantity; ++i)
    {
        mData[i] -= level;
    }

    // The vertices are computed as rational numbers.
    std::vector<Vertex> ratVertices;

    int xBoundM1 = mXBound - 1, yBoundM1 = mYBound - 1;
    for (int y = 0, yp = 1; y < yBoundM1; ++y, ++yp)
    {
        for (int x = 0, xp = 1; x < xBoundM1; ++x, ++xp)
        {
            // Get the image values at the corners of the square.
            int i00 = x + mXBound*y;
            int i10 = i00 + 1;
            int i01 = i00 + mXBound;
            int i11 = i10 + mXBound;
            int f00 = mData[i00];
            int f10 = mData[i10];
            int f01 = mData[i01];
            int f11 = mData[i11];

            // Construct the vertices and edges of the level curve in the
            // square.
            ProcessSquare(ratVertices, edges, x, xp, y, yp, f00, f10, f11,
                f01);
        }
    }

    // Convert the rational vertices to floating-point vertices.
    const int numVertices = (int)ratVertices.size();
    vertices.resize(numVertices);
    for (i = 0; i < numVertices; ++i)
    {
        vertices[i][0] = ((float)ratVertices[i].XNumer) /
            (float)ratVertices[i].XDenom;

        vertices[i][1] = ((float)ratVertices[i].YNumer) /
            (float)ratVertices[i].YDenom;
    }

    // Restore the image values.
    for (i = 0; i < mQuantity; ++i)
    {
        mData[i] += level;
    }
}
//----------------------------------------------------------------------------
void ExtractCurveSquares::MakeUnique (std::vector<Vector2f>& vertices,
    std::vector<EdgeKey>& edges)
{
    int numVertices = (int)vertices.size();
    if (numVertices == 0)
    {
        return;
    }

    // Use maps to generate unique storage.
    typedef std::map<Vector2f,int> VMap;
    typedef std::map<Vector2f,int>::iterator VIterator;
    VMap vertexMap;
    for (int v = 0, nextVertex = 0; v < numVertices; ++v)
    {
        std::pair<VIterator,bool> result = vertexMap.insert(
            std::make_pair(vertices[v], nextVertex));

        if (result.second == true)
        {
            ++nextVertex;
        }
    }

    typedef std::map<EdgeKey,int> EMap;
    typedef std::map<EdgeKey,int>::iterator EIterator;
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
            assertion(vIter != vertexMap.end(), "Unexpected condition\n");
            edges[e].V[0] = vIter->second;
            vIter = vertexMap.find(vertices[edges[e].V[1]]);
            assertion(vIter != vertexMap.end(), "Unexpected condition\n");
            edges[e].V[1] = vIter->second;

            // Keep only unique edges.
            std::pair<EIterator,bool> result = edgeMap->insert(
                std::make_pair(edges[e], nextEdge));

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
void ExtractCurveSquares::AddVertex (std::vector<Vertex>& vertices,
    int xNumer, int xDenom, int yNumer, int yDenom)
{
    vertices.push_back(Vertex(xNumer, xDenom, yNumer, yDenom));
}
//----------------------------------------------------------------------------
void ExtractCurveSquares::AddEdge (std::vector<Vertex>& vertices,
    std::vector<EdgeKey>& edges, int xNumer0, int xDenom0, int yNumer0,
    int yDenom0, int xNumer1, int xDenom1, int yNumer1, int yDenom1)
{
    int v0 = (int)vertices.size();
    int v1 = v0 + 1;
    edges.push_back(EdgeKey(v0, v1));
    vertices.push_back(Vertex(xNumer0, xDenom0, yNumer0, yDenom0));
    vertices.push_back(Vertex(xNumer1, xDenom1, yNumer1, yDenom1));
}
//----------------------------------------------------------------------------
void ExtractCurveSquares::ProcessSquare (std::vector<Vertex>& vertices,
    std::vector<EdgeKey>& edges, int x, int xp, int y, int yp, int f00,
    int f10, int f11, int f01)
{
    int xn0, yn0, xn1, yn1, d0, d1, d2, d3, det;

    if (f00 != 0)
    {
        // convert to case "+***"
        if (f00 < 0)
        {
            f00 = -f00;
            f10 = -f10;
            f11 = -f11;
            f01 = -f01;
        }

        if (f10 > 0)
        {
            if (f11 > 0)
            {
                if (f01 > 0)
                {
                    // ++++
                    return;
                }
                else if (f01 < 0)
                {
                    // +++-
                    d0 = f11 - f01;
                    xn0 = f11*x - f01*xp;
                    d1 = f00 - f01;
                    yn1 = f00*yp - f01*y;
                    AddEdge(vertices, edges, xn0, d0, yp, 1, x, 1, yn1, d1);
                }
                else
                {
                    // +++0
                    AddVertex(vertices, x, 1, yp, 1);
                }
            }
            else if (f11 < 0)
            {
                d0 = f10 - f11;
                yn0 = f10*yp - f11*y;

                if (f01 > 0)
                {
                    // ++-+
                    d1 = f01 - f11;
                    xn1 = f01*xp - f11*x;
                    AddEdge(vertices, edges, xp, 1, yn0, d0, xn1, d1, yp, 1);
                }
                else if (f01 < 0)
                {
                    // ++--
                    d1 = f01 - f00;
                    yn1 = f01*y - f00*yp;
                    AddEdge(vertices, edges, x, 1, yn1, d1, xp, 1, yn0, d0);
                }
                else
                {
                    // ++-0
                    AddEdge(vertices, edges, x, 1, yp, 1, xp, 1, yn0, d0);
                }
            }
            else
            {
                if (f01 > 0)
                {
                    // ++0+
                    AddVertex(vertices, xp, 1, yp, 1);
                }
                else if (f01 < 0)
                {
                    // ++0-
                    d0 = f01 - f00;
                    yn0 = f01*y - f00*yp;
                    AddEdge(vertices, edges, xp, 1, yp, 1, x, 1, yn0, d0);
                }
                else
                {
                    // ++00
                    AddEdge(vertices, edges, xp, 1, yp, 1, x, 1, yp, 1);
                }
            }
        }
        else if (f10 < 0)
        {
            d0 = f00 - f10;
            xn0 = f00*xp - f10*x;

            if (f11 > 0)
            {
                d1 = f11 - f10;
                yn1 = f11*y - f10*yp;

                if (f01 > 0)
                {
                    // +-++
                    AddEdge(vertices, edges, xn0, d0, y, 1, xp, 1, yn1, d1);
                }
                else if (f01 < 0)
                {
                    // +-+-
                    d3 = f11 - f01;
                    xn1 = f11*x - f01*xp;
                    d2 = f01 - f00;
                    yn0 = f01*y - f00*yp;

                    if (d0*d3 > 0)
                    {
                        det = xn1*d0 - xn0*d3;
                    }
                    else
                    {
                        det = xn0*d3 - xn1*d0;
                    }

                    if (det > 0)
                    {
                        AddEdge(vertices, edges, xn1, d3, yp, 1, xp, 1, yn1, d1);
                        AddEdge(vertices, edges, xn0, d0, y, 1, x, 1, yn0, d2);
                    }
                    else if (det < 0)
                    {
                        AddEdge(vertices, edges, xn1, d3, yp, 1, x, 1, yn0, d2);
                        AddEdge(vertices, edges, xn0, d0, y, 1, xp, 1, yn1, d1);
                    }
                    else
                    {
                        AddEdge(vertices, edges, xn0, d0, yn0, d2, xn0, d0, y, 1);
                        AddEdge(vertices, edges, xn0, d0, yn0, d2, xn0, d0, yp, 1);
                        AddEdge(vertices, edges, xn0, d0, yn0, d2, x, 1, yn0, d2);
                        AddEdge(vertices, edges, xn0, d0, yn0, d2, xp, 1, yn0, d2);
                    }
                }
                else
                {
                    // +-+0
                    AddEdge(vertices, edges, xn0, d0, y, 1, xp, 1, yn1, d1);
                    AddVertex(vertices, x, 1, yp, 1);
                }
            }
            else if (f11 < 0)
            {
                if (f01 > 0)
                {
                    // +--+
                    d1 = f11 - f01;
                    xn1 = f11*x - f01*xp;
                    AddEdge(vertices, edges, xn0, d0, y, 1, xn1, d1, yp, 1);
                }
                else if (f01 < 0)
                {
                    // +---
                    d1 = f01 - f00;
                    yn1 = f01*y - f00*yp;
                    AddEdge(vertices, edges, x, 1, yn1, d1, xn0, d0, y, 1);
                }
                else
                {
                    // +--0
                    AddEdge(vertices, edges, x, 1, yp, 1, xn0, d0, y, 1);
                }
            }
            else
            {
                if (f01 > 0)
                {
                    // +-0+
                    AddEdge(vertices, edges, xp, 1, yp, 1, xn0, d0, y, 1);
                }
                else if (f01 < 0)
                {
                    // +-0-
                    d1 = f01 - f00;
                    yn1 = f01*y - f00*yp;
                    AddEdge(vertices, edges, x, 1, yn1, d1, xn0, d0, y, 1);
                    AddVertex(vertices, xp, 1, yp, 1);
                }
                else
                {
                    // +-00
                    AddEdge(vertices, edges, xp, 1, yp, 1, xn0, d0, yp, 1);
                    AddEdge(vertices, edges, xn0, d0, yp, 1, x, 1, yp, 1);
                    AddEdge(vertices, edges, xn0, d0, yp, 1, xn0, d0, y, 1);
                }
            }
        }
        else
        {
            if (f11 > 0)
            {
                if (f01 > 0)
                {
                    // +0++
                    AddVertex(vertices, xp, 1, y, 1);
                }
                else if (f01 < 0)
                {
                    // +0+-
                    d0 = f11 - f01;
                    xn0 = f11*x - f01*xp;
                    d1 = f00 - f01;
                    yn1 = f00*yp - f01*y;
                    AddEdge(vertices, edges, xn0, d0, yp, 1, x, 1, yn1, d1);
                    AddVertex(vertices, xp, 1, y, 1);
                }
                else
                {
                    // +0+0
                    AddVertex(vertices, xp, 1, y, 1);
                    AddVertex(vertices, x, 1, yp, 1);
                }
            }
            else if (f11 < 0)
            {
                if (f01 > 0)
                {
                    // +0-+
                    d0 = f11 - f01;
                    xn0 = f11*x - f01*xp;
                    AddEdge(vertices, edges, xp, 1, y, 1, xn0, d0, yp, 1);
                }
                else if (f01 < 0)
                {
                    // +0--
                    d0 = f01 - f00;
                    yn0 = f01*y - f00*yp;
                    AddEdge(vertices, edges, xp, 1, y, 1, x, 1, yn0, d0);
                }
                else
                {
                    // +0-0
                    AddEdge(vertices, edges, xp, 1, y, 1, x, 1, yp, 1);
                }
            }
            else
            {
                if (f01 > 0)
                {
                    // +00+
                    AddEdge(vertices, edges, xp, 1, y, 1, xp, 1, yp, 1);
                }
                else if (f01 < 0)
                {
                    // +00-
                    d0 = f00 - f01;
                    yn0 = f00*yp - f01*y;
                    AddEdge(vertices, edges, xp, 1, y, 1, xp, 1, yn0, d0);
                    AddEdge(vertices, edges, xp, 1, yn0, d0, xp, 1, yp, 1);
                    AddEdge(vertices, edges, xp, 1, yn0, d0, x, 1, yn0, d0);
                }
                else
                {
                    // +000
                    AddEdge(vertices, edges, x, 1, yp, 1, x, 1, y, 1);
                    AddEdge(vertices, edges, x, 1, y, 1, xp, 1, y, 1);
                }
            }
        }
    }
    else if (f10 != 0)
    {
        // convert to case 0+**
        if (f10 < 0)
        {
            f10 = -f10;
            f11 = -f11;
            f01 = -f01;
        }

        if (f11 > 0)
        {
            if (f01 > 0)
            {
                // 0+++
                AddVertex(vertices, x, 1, y, 1);
            }
            else if (f01 < 0)
            {
                // 0++-
                d0 = f11 - f01;
                xn0 = f11*x - f01*xp;
                AddEdge(vertices, edges, x, 1, y, 1, xn0, d0, yp, 1);
            }
            else
            {
                // 0++0
                AddEdge(vertices, edges, x, 1, yp, 1, x, 1, y, 1);
            }
        }
        else if (f11 < 0)
        {
            if (f01 > 0)
            {
                // 0+-+
                d0 = f10 - f11;
                yn0 = f10*yp - f11*y;
                d1 = f01 - f11;
                xn1 = f01*xp - f11*x;
                AddEdge(vertices, edges, xp, 1, yn0, d0, xn1, d1, yp, 1);
                AddVertex(vertices, x, 1, y, 1);
            }
            else if (f01 < 0)
            {
                // 0+--
                d0 = f10 - f11;
                yn0 = f10*yp - f11*y;
                AddEdge(vertices, edges, x, 1, y, 1, xp, 1, yn0, d0);
            }
            else
            {
                // 0+-0
                d0 = f10 - f11;
                yn0 = f10*yp - f11*y;
                AddEdge(vertices, edges, x, 1, y, 1, x, 1, yn0, d0);
                AddEdge(vertices, edges, x, 1, yn0, d0, x, 1, yp, 1);
                AddEdge(vertices, edges, x, 1, yn0, d0, xp, 1, yn0, d0);
            }
        }
        else
        {
            if (f01 > 0)
            {
                // 0+0+
                AddVertex(vertices, x, 1, y, 1);
                AddVertex(vertices, xp, 1, yp, 1);
            }
            else if (f01 < 0)
            {
                // 0+0-
                AddEdge(vertices, edges, x, 1, y, 1, xp, 1, yp, 1);
            }
            else
            {
                // 0+00
                AddEdge(vertices, edges, xp, 1, yp, 1, x, 1, yp, 1);
                AddEdge(vertices, edges, x, 1, yp, 1, x, 1, y, 1);
            }
        }
    }
    else if (f11 != 0)
    {
        // convert to case 00+*
        if (f11 < 0)
        {
            f11 = -f11;
            f01 = -f01;
        }

        if (f01 > 0)
        {
            // 00++
            AddEdge(vertices, edges, x, 1, y, 1, xp, 1, y, 1);
        }
        else if (f01 < 0)
        {
            // 00+-
            d0 = f01 - f11;
            xn0 = f01*xp - f11*x;
            AddEdge(vertices, edges, x, 1, y, 1, xn0, d0, y, 1);
            AddEdge(vertices, edges, xn0, d0, y, 1, xp, 1, y, 1);
            AddEdge(vertices, edges, xn0, d0, y, 1, xn0, d0, yp, 1);
        }
        else
        {
            // 00+0
            AddEdge(vertices, edges, xp, 1, y, 1, xp, 1, yp, 1);
            AddEdge(vertices, edges, xp, 1, yp, 1, x, 1, yp, 1);
        }
    }
    else if (f01 != 0)
    {
        // cases 000+ or 000-
        AddEdge(vertices, edges, x, 1, y, 1, xp, 1, y, 1);
        AddEdge(vertices, edges, xp, 1, y, 1, xp, 1, yp, 1);
    }
    else
    {
        // case 0000
        AddEdge(vertices, edges, x, 1, y, 1, xp, 1, y, 1);
        AddEdge(vertices, edges, xp, 1, y, 1, xp, 1, yp, 1);
        AddEdge(vertices, edges, xp, 1, yp, 1, x, 1, yp, 1);
        AddEdge(vertices, edges, x, 1, yp, 1, x, 1, y, 1);
    }
}
//----------------------------------------------------------------------------
ExtractCurveSquares::Vertex::Vertex (int xNumer, int xDenom, int yNumer,
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
