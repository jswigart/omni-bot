// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5EXTRACTCURVESQUARES_H
#define WM5EXTRACTCURVESQUARES_H

#include "Wm5ImagicsLIB.h"
#include "Wm5Vector2.h"
#include "Wm5EdgeKey.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM ExtractCurveSquares
{
public:
    // The input is a 2D image with lexicographically ordered pixels (x,y)
    // stored in a linear array.  Pixel (x,y) is stored in the array at
    // location x+xbound*y.  The caller is responsible for deleting data if
    // it was dynamically allocated.
    ExtractCurveSquares (int xBound, int yBound, int* data);

    // Extract a level curve of the specified level value.
    void ExtractContour (int level, std::vector<Vector2f>& vertices,
        std::vector<EdgeKey>& edges);

    // The extraction has duplicate vertices on edges shared by pixels.  This
    // function will eliminate the duplication.
    void MakeUnique (std::vector<Vector2f>& vertices,
        std::vector<EdgeKey>& edges);

protected:
    // Since the level value is an integer, the vertices of the extracted
    // level curve are stored internally as rational numbers.
    class Vertex
    {
    public:
        Vertex (int xNumer = 0, int xDenom = 0, int yNumer = 0,
            int yDenom = 0);

        int XNumer, XDenom, YNumer, YDenom;
    };

    void AddVertex (std::vector<Vertex>& vertices, int xNumer, int xDenom,
        int yNumer, int yDenom);

    void AddEdge (std::vector<Vertex>& vertices, std::vector<EdgeKey>& edges,
        int xNumer0, int xDenom0, int yNumer0, int yDenom0, int xNumer1,
        int xDenom1, int yNumer1, int yDenom1);

    void ProcessSquare (std::vector<Vertex>& vertices,
        std::vector<EdgeKey>& edges, int x, int xp, int y, int yp, int f00,
        int f10, int f11, int f01);

    int mXBound, mYBound, mQuantity;
    int* mData;
};

}

#endif
