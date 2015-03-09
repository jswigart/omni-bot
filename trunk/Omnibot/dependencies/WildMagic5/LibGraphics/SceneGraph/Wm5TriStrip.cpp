// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5TriStrip.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Triangles, TriStrip);
WM5_IMPLEMENT_STREAM(TriStrip);
WM5_IMPLEMENT_FACTORY(TriStrip);
WM5_IMPLEMENT_DEFAULT_NAMES(Triangles, TriStrip);
WM5_IMPLEMENT_DEFAULT_STREAM(Triangles, TriStrip);

//----------------------------------------------------------------------------
TriStrip::TriStrip (VertexFormat* vformat, VertexBuffer* vbuffer,
    int indexSize)
    :
    Triangles(PT_TRISTRIP, vformat, vbuffer, 0)
{
    int numVertices = mVBuffer->GetNumElements();
    mIBuffer = new0 IndexBuffer(numVertices, indexSize);
    int i;
    
    if (indexSize == 2)
    {
        short* indices = (short*)mIBuffer->GetData();
        for (i = 0; i < numVertices; ++i)
        {
            indices[i] = (short)i;
        }
    }
    else // indexSize == 4
    {
        int* indices = (int*)mIBuffer->GetData();
        for (i = 0; i < numVertices; ++i)
        {
            indices[i] = i;
        }
    }
}
//----------------------------------------------------------------------------
TriStrip::TriStrip (VertexFormat* vformat, VertexBuffer* vbuffer,
    IndexBuffer* ibuffer)
    :
    Triangles(PT_TRISTRIP, vformat, vbuffer, ibuffer)
{
    assertion(mIBuffer->GetNumElements() >= mVBuffer->GetNumElements(),
        "Not enough elements for the index buffer\n");
}
//----------------------------------------------------------------------------
TriStrip::~TriStrip ()
{
}
//----------------------------------------------------------------------------
int TriStrip::GetNumTriangles () const
{
    return mIBuffer->GetNumElements() - 2;
}
//----------------------------------------------------------------------------
bool TriStrip::GetTriangle (int i, int& v0, int& v1, int& v2) const
{
    if (0 <= i && i < GetNumTriangles())
    {
        int* indices = (int*)mIBuffer->GetData();
        v0 = indices[i];
        if (i & 1)
        {
            v1 = indices[i + 2];
            v2 = indices[i + 1];
        }
        else
        {
            v1 = indices[i + 1];
            v2 = indices[i + 2];
        }

        // Degenerate triangles are assumed to have been added for swaps and
        // turns in the triangle strip.  They are considered invalid for other
        // purposes in the engine.
        return (v0 != v1 && v0 != v2 && v1 != v2);
    }
    return false;
}
//----------------------------------------------------------------------------
