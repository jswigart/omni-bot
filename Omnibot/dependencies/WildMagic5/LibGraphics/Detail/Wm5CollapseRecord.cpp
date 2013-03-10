// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CollapseRecord.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
CollapseRecord::CollapseRecord (int vKeep, int vThrow, int numVertices,
    int numTriangles)
    :
    VKeep(vKeep),
    VThrow(vThrow),
    NumVertices(numVertices),
    NumTriangles(numTriangles),
    NumIndices(0),
    Indices(0)
{
}
//----------------------------------------------------------------------------
CollapseRecord::~CollapseRecord ()
{
    delete1(Indices);
}
//----------------------------------------------------------------------------
