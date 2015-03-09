// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5TerrainPage.h"
#include "Wm5StandardMesh.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TriMesh, TerrainPage);
WM5_IMPLEMENT_STREAM(TerrainPage);
WM5_IMPLEMENT_FACTORY(TerrainPage);
WM5_IMPLEMENT_DEFAULT_NAMES(TriMesh, TerrainPage);

//----------------------------------------------------------------------------
TerrainPage::TerrainPage (VertexFormat* vformat, int size,
    unsigned short* heights, const Float2& origin, float minElevation,
    float maxElevation, float spacing)
    :
    mSize(size),
    mSizeM1(size - 1),
    mHeights(heights),
    mOrigin(origin),
    mMinElevation(minElevation),
    mMaxElevation(maxElevation),
    mSpacing(spacing)
{
    // size = 2^p + 1, p <= 7
    assertion(size ==  3 || size ==  5 || size ==   9 || size == 17
        || size == 33 || size == 65 || size == 129, "Invalid page size\n");

    mInvSpacing = 1.0f/mSpacing;
    mMultiplier = (mMaxElevation - mMinElevation)/65535.0f;

    // Create a mesh for the page.
    float ext = mSpacing*mSizeM1;
    TriMesh* mesh = StandardMesh(vformat).Rectangle(mSize, mSize, ext, ext);
    mVFormat = vformat;
    mVBuffer = mesh->GetVertexBuffer();
    mIBuffer = mesh->GetIndexBuffer();
    delete0(mesh);

    // Modify the vertices to use the terrain data.
    VertexBufferAccessor vba(mVFormat, mVBuffer);
    int numVertices = mVBuffer->GetNumElements();
    for (int i = 0; i < numVertices; ++i)
    {
        int x = i % mSize;
        int y = i / mSize;
        vba.Position<Float3>(i) = Float3(GetX(x), GetY(y), GetHeight(i));
    }

    UpdateModelSpace(Visual::GU_NORMALS);
}
//----------------------------------------------------------------------------
TerrainPage::~TerrainPage ()
{
    delete1(mHeights);
}
//----------------------------------------------------------------------------
float TerrainPage::GetHeight (float x, float y) const
{
    float xGrid = (x - mOrigin[0])*mInvSpacing;
    if (xGrid < 0.0f || xGrid >= (float)mSizeM1)
    {
        // Location not in page.
        return Mathf::MAX_REAL;
    }

    float yGrid = (y - mOrigin[1])*mInvSpacing;
    if (yGrid < 0.0f || yGrid >= (float)mSizeM1)
    {
        // Location not in page.
        return Mathf::MAX_REAL;
    }

    float fCol = Mathf::Floor(xGrid);
    int iCol = (int)fCol;
    float fRow = Mathf::Floor(yGrid);
    int iRow = (int)fRow;

    int index = iCol + mSize*iRow;
    float dx = xGrid - fCol;
    float dy = yGrid - fRow;
    float h00, h10, h01, h11, height;

    if ((iCol & 1) == (iRow & 1))
    {
        float diff = dx - dy;
        h00 = mMinElevation + mMultiplier*mHeights[index];
        h11 = mMinElevation + mMultiplier*mHeights[index + 1 + mSize];
        if (diff > 0.0f)
        {
            h10 = mMinElevation + mMultiplier*mHeights[index + 1];
            height = (1.0f - diff - dy)*h00 + diff*h10 + dy*h11;
        }
        else
        {
            h01 = mMinElevation + mMultiplier*mHeights[index + mSize];
            height = (1.0f + diff - dx)*h00 - diff*h01 + dx*h11;
        }
    }
    else
    {
        float sum = dx + dy;
        h10 = mMinElevation + mMultiplier*mHeights[index + 1];
        h01 = mMinElevation + mMultiplier*mHeights[index + mSize];
        if (sum <= 1.0f)
        {
            h00 = mMinElevation + mMultiplier*mHeights[index];
            height = (1.0f - sum)*h00 + dx*h10 + dy*h01;
        }
        else
        {
            h11 = mMinElevation + mMultiplier*mHeights[index + 1 + mSize];
            height = (sum - 1.0f)*h11 + (1.0f - dy)*h10 + (1.0f - dx)*h01;
        }
    }

    return height;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
TerrainPage::TerrainPage (LoadConstructor value)
    :
    TriMesh(value),
    mSize(0),
    mSizeM1(0),
    mHeights(0),
    mOrigin(Float2(0.0f, 0.0f)),
    mMinElevation(0.0f),
    mMaxElevation(0.0f),
    mSpacing(0.0f),
    mInvSpacing(0.0f),
    mMultiplier(0.0f)
{
}
//----------------------------------------------------------------------------
void TerrainPage::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TriMesh::Load(source);

    source.Read(mSize);
    int numVertices = mSize*mSize;
    source.ReadVR(numVertices, mHeights);
    source.ReadAggregate(mOrigin);
    source.Read(mMinElevation);
    source.Read(mMaxElevation);
    source.Read(mSpacing);

    mSizeM1 = mSize - 1;
    mInvSpacing = 1.0f/mSpacing;
    mMultiplier = (mMaxElevation - mMinElevation)/65535.0f;

    WM5_END_DEBUG_STREAM_LOAD(TerrainPage, source);
}
//----------------------------------------------------------------------------
void TerrainPage::Link (InStream& source)
{
    TriMesh::Link(source);
}
//----------------------------------------------------------------------------
void TerrainPage::PostLink ()
{
    TriMesh::PostLink();
}
//----------------------------------------------------------------------------
bool TerrainPage::Register (OutStream& target) const
{
    return TriMesh::Register(target);
}
//----------------------------------------------------------------------------
void TerrainPage::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TriMesh::Save(target);

    target.Write(mSize);
    int numVertices = mSize*mSize;
    target.WriteN(numVertices, mHeights);
    target.WriteAggregate(mOrigin);
    target.Write(mMinElevation);
    target.Write(mMaxElevation);
    target.Write(mSpacing);

    WM5_END_DEBUG_STREAM_SAVE(TerrainPage, target);
}
//----------------------------------------------------------------------------
int TerrainPage::GetStreamingSize () const
{
    int size = TriMesh::GetStreamingSize();
    size += sizeof(mSize);
    size += mSize*mSize*sizeof(mHeights[0]);
    size += sizeof(mOrigin);
    size += sizeof(mMinElevation);
    size += sizeof(mMaxElevation);
    size += sizeof(mSpacing);
    return size;
}
//----------------------------------------------------------------------------
