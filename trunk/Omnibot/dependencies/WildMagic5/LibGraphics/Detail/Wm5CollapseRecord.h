// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5COLLAPSERECORD_H
#define WM5COLLAPSERECORD_H

#include "Wm5GraphicsLIB.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM CollapseRecord
{
public:
    // Construction and destruction.
    CollapseRecord (int vKeep = -1, int vThrow = -1, int numVertices = 0,
        int numTriangles = 0);

    ~CollapseRecord ();

    // Edge <VKeep,VThrow> collapses so that VThrow is replaced by VKeep.
    int VKeep, VThrow;

    // The number of vertices after the edge collapse.
    int NumVertices;

    // The number of triangles after the edge collapse.
    int NumTriangles;

    // The array of indices in [0..NumTriangles-1] that contain VThrow.
    int NumIndices;
    int* Indices;
};

}

#endif
