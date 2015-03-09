// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5TriMesh.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Triangles, TriMesh);
WM5_IMPLEMENT_STREAM(TriMesh);
WM5_IMPLEMENT_FACTORY(TriMesh);
WM5_IMPLEMENT_DEFAULT_NAMES(Triangles, TriMesh);
WM5_IMPLEMENT_DEFAULT_STREAM(Triangles, TriMesh);

//----------------------------------------------------------------------------
TriMesh::TriMesh ()
    :
    Triangles(PT_TRIMESH)
{
}
//----------------------------------------------------------------------------
TriMesh::TriMesh (VertexFormat* vformat, VertexBuffer* vbuffer,
    IndexBuffer* ibuffer)
    :
    Triangles(PT_TRIMESH, vformat, vbuffer, ibuffer)
{
}
//----------------------------------------------------------------------------
TriMesh::~TriMesh ()
{
}
//----------------------------------------------------------------------------
int TriMesh::GetNumTriangles () const
{
    return mIBuffer->GetNumElements()/3;
}
//----------------------------------------------------------------------------
bool TriMesh::GetTriangle (int i, int& v0, int& v1, int& v2) const
{
    if (0 <= i && i < GetNumTriangles())
    {
        int* indices = 3*i + (int*)mIBuffer->GetData();
        v0 = *indices++;
        v1 = *indices++;
        v2 = *indices;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
