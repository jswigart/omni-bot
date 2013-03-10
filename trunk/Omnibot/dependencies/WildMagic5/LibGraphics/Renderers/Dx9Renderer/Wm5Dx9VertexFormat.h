// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5DX9VERTEXFORMAT_H
#define WM5DX9VERTEXFORMAT_H

#include "Wm5Dx9RendererLIB.h"
#include "Wm5VertexFormat.h"

namespace Wm5
{

class Renderer;

class PdrVertexFormat
{
public:
    // Construction and destruction.
    PdrVertexFormat (Renderer* renderer, const VertexFormat* vformat);
    ~PdrVertexFormat ();

    // Vertex format operations.
    void Enable (Renderer* renderer);
    void Disable (Renderer* renderer);

private:
    D3DVERTEXELEMENT9 mElements[VertexFormat::AM_MAX_ATTRIBUTES + 1];
    IDirect3DVertexDeclaration9* mDeclaration;
};

}

#endif
