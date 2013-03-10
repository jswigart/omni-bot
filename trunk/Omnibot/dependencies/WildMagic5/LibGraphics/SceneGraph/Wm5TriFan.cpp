// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5TriFan.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Triangles, TriFan);
WM5_IMPLEMENT_STREAM(TriFan);
WM5_IMPLEMENT_FACTORY(TriFan);
WM5_IMPLEMENT_DEFAULT_NAMES(Triangles, TriFan);
WM5_IMPLEMENT_DEFAULT_STREAM(Triangles, TriFan);

//----------------------------------------------------------------------------
TriFan::TriFan (VertexFormat* vformat, VertexBuffer* vbuffer, int indexSize)
    :
    Triangles(PT_TRIFAN, vformat, vbuffer, 0)
{
    assertion(indexSize == 2 || indexSize == 4, "Invalid index size.\n");

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
TriFan::TriFan (VertexFormat* vformat, VertexBuffer* vbuffer,
    IndexBuffer* ibuffer)
    :
    Triangles(PT_TRIFAN, vformat, vbuffer, ibuffer)
{
    assertion(mIBuffer->GetNumElements() >= mVBuffer->GetNumElements(),
        "Not enough elements for the index buffer\n");
}
//----------------------------------------------------------------------------
TriFan::~TriFan ()
{
}
//----------------------------------------------------------------------------
int TriFan::GetNumTriangles () const
{
    return mIBuffer->GetNumElements() - 2;
}
//----------------------------------------------------------------------------
bool TriFan::GetTriangle (int i, int& v0, int& v1, int& v2) const
{
    if (0 <= i && i < GetNumTriangles())
    {
        int* indices = (int*)mIBuffer->GetData();
        v0 = indices[0];
        v1 = indices[i + 1];
        v2 = indices[i + 2];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
