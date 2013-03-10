// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TRIMESH_H
#define WM5TRIMESH_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Triangles.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM TriMesh : public Triangles
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(TriMesh);

protected:
    // Default construction for derived classes.
    TriMesh ();
public:
    // Construction and destruction.
    TriMesh (VertexFormat* vformat, VertexBuffer* vbuffer,
        IndexBuffer* ibuffer);

    virtual ~TriMesh ();

    // Interpretation of the index buffer data.
    virtual int GetNumTriangles () const;
    virtual bool GetTriangle (int i, int& v0, int& v1, int& v2) const;
};

WM5_REGISTER_STREAM(TriMesh);
typedef Pointer0<TriMesh> TriMeshPtr;

}

#endif
