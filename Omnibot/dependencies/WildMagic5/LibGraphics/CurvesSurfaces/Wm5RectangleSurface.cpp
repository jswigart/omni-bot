// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5RectangleSurface.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TriMesh, RectangleSurface);
WM5_IMPLEMENT_STREAM(RectangleSurface);
WM5_IMPLEMENT_FACTORY(RectangleSurface);
WM5_IMPLEMENT_DEFAULT_NAMES(TriMesh, RectangleSurface);

//----------------------------------------------------------------------------
RectangleSurface::RectangleSurface (ParametricSurfacef* surface,
    int numUSamples, int numVSamples, VertexFormat* vformat,
    const Float2* tcoordMin, const Float2* tcoordMax)
    :
    mSurface(surface),
    mNumUSamples(numUSamples),
    mNumVSamples(numVSamples)
{
    assertion(surface && surface->IsRectangular(), "Invalid surface\n");

    mVFormat = vformat;

    float uMin = mSurface->GetUMin();
    float uRange = mSurface->GetUMax() - uMin;
    float uDelta = uRange/(float)(mNumUSamples - 1);
    float vMin = mSurface->GetVMin();
    float vRange = mSurface->GetVMax() - vMin;
    float vDelta = vRange/(float)(mNumVSamples - 1);

    // Compute the vertices, normals, and texture coordinates.
    int numVertices = mNumUSamples*mNumVSamples;
    int vstride = vformat->GetStride();
    mVBuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, mVBuffer);

    float tuDelta = 0.0f, tvDelta = 0.0f;
    if (tcoordMin && tcoordMax)
    {
        tuDelta = ((*tcoordMax)[0] - (*tcoordMin)[0])/uRange;
        tvDelta = ((*tcoordMax)[1] - (*tcoordMin)[1])/vRange;
    }

    int uIndex, vIndex, i;
    for (uIndex = 0, i = 0; uIndex < mNumUSamples; ++uIndex)
    {
        float uIncr = uDelta*uIndex;
        float u = uMin + uIncr;
        for (vIndex = 0; vIndex < mNumVSamples; ++vIndex, ++i)
        {
            float vIncr = vDelta*vIndex;
            float v = vMin + vIncr;
            vba.Position<Vector3f>(i) = mSurface->P(u, v);

            if (vba.HasNormal())
            {
                Vector3f pos, tan0, tan1, normal;
                mSurface->GetFrame(u, v, pos, tan0, tan1, normal);
                vba.Normal<Vector3f>(i) = normal;
            }

            const int numTCoords = VertexFormat::AM_MAX_TCOORD_UNITS;
            Float2 tcoord(
                (*tcoordMin)[0] + tuDelta*uIncr,
                (*tcoordMin)[1] + tvDelta*vIncr);
            for (int unit = 0; unit < numTCoords; ++unit)
            {
                if (vba.HasTCoord(unit))
                {
                    assertion(vba.GetTCoordChannels(unit) == 2,
                        "Texture coordinate must be 2D\n");
                    vba.TCoord<Float2>(unit, i) = tcoord;
                }
            }
        }
    }

    // Compute the surface triangle indices.
    int numTriangles = 2*(mNumUSamples-1)*(mNumVSamples-1);
    int numIndices = 3*numTriangles;
    mIBuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)mIBuffer->GetData();
    for (uIndex = 0, i = 0; uIndex < mNumUSamples - 1; ++uIndex)
    {
        int i0 = i;
        int i1 = i0 + 1;
        i += mNumVSamples;
        int i2 = i;
        int i3 = i2 + 1;
        for (vIndex = 0; vIndex < mNumVSamples - 1; ++vIndex, indices += 6)
        {
            indices[0] = i0;
            indices[1] = i1;
            indices[2] = i2;
            indices[3] = i1;
            indices[4] = i3;
            indices[5] = i2;
            i0++;
            i1++;
            i2++;
            i3++;
        }
    }

    UpdateModelSpace(Visual::GU_NORMALS);
}
//----------------------------------------------------------------------------
RectangleSurface::~RectangleSurface ()
{
    delete0(mSurface);
}
//----------------------------------------------------------------------------
void RectangleSurface::UpdateSurface ()
{
    float uMin = mSurface->GetUMin();
    float uDelta = (mSurface->GetUMax() - uMin)/(float)(mNumUSamples - 1);
    float vMin = mSurface->GetVMin();
    float vDelta = (mSurface->GetVMax() - vMin)/(float)(mNumVSamples - 1);

    VertexBufferAccessor vba(mVFormat, mVBuffer);
    for (int uIndex = 0, i = 0; uIndex < mNumUSamples; ++uIndex)
    {
        float u = uMin + uDelta*uIndex;
        for (int vIndex = 0; vIndex < mNumVSamples; ++vIndex, ++i)
        {
            float v = vMin + vDelta*vIndex;
            vba.Position<Vector3f>(i) = mSurface->P(u, v);

            if (vba.HasNormal())
            {
                Vector3f pos, tan0, tan1, normal;
                mSurface->GetFrame(u, v, pos, tan0, tan1, normal);
                vba.Normal<Vector3f>(i) = normal;
            }
        }
    }

    UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(mVBuffer);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
RectangleSurface::RectangleSurface (LoadConstructor value)
    :
    TriMesh(value),
    mSurface(0),
    mNumUSamples(0),
    mNumVSamples(0)
{
}
//----------------------------------------------------------------------------
void RectangleSurface::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TriMesh::Load(source);

    source.Read(mNumUSamples);
    source.Read(mNumVSamples);

    // TODO.  See note in RectangleSurface::Save.
    mSurface = 0;

    WM5_END_DEBUG_STREAM_LOAD(RectangleSurface, source);
}
//----------------------------------------------------------------------------
void RectangleSurface::Link (InStream& source)
{
    TriMesh::Link(source);
}
//----------------------------------------------------------------------------
void RectangleSurface::PostLink ()
{
    TriMesh::PostLink();
}
//----------------------------------------------------------------------------
bool RectangleSurface::Register (OutStream& target) const
{
    return TriMesh::Register(target);
}
//----------------------------------------------------------------------------
void RectangleSurface::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TriMesh::Save(target);

    target.Write(mNumUSamples);
    target.Write(mNumVSamples);

    // TODO.  The class ParametricSurface3 is abstract and does not know
    // about the data representation for the derived-class object that is
    // passed to the RectangleSurface constructor.  Because of this, any
    // loaded RectangleSurface object will require the application to
    // manually set the surface via the SetSurface() member function.
    //
    // Streaming support should be added to the surface classes.

    WM5_END_DEBUG_STREAM_SAVE(RectangleSurface, target);
}
//----------------------------------------------------------------------------
int RectangleSurface::GetStreamingSize () const
{
    int size = TriMesh::GetStreamingSize();
    size += sizeof(mNumUSamples);
    size += sizeof(mNumVSamples);
    return size;
}
//----------------------------------------------------------------------------
