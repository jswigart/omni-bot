// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5TubeSurface.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TriMesh, TubeSurface);
WM5_IMPLEMENT_STREAM(TubeSurface);
WM5_IMPLEMENT_FACTORY(TubeSurface);
WM5_IMPLEMENT_DEFAULT_NAMES(TriMesh, TubeSurface);

//----------------------------------------------------------------------------
TubeSurface::TubeSurface (Curve3f* medial, RadialFunction radial,
    bool closed, const Vector3f& upVector, int numMedialSamples,
    int numSliceSamples, bool sampleByArcLength, bool insideView,
    const Float2* tcoordMin, const Float2* tcoordMax,
    VertexFormat* vformat)
    :
    mMedial(medial),
    mRadial(radial),
    mNumMedialSamples(numMedialSamples),
    mNumSliceSamples(numSliceSamples),
    mUpVector(upVector),
    mSin(0),
    mCos(0),
    mClosed(closed),
    mSampleByArcLength(sampleByArcLength)
{
    // Compute the surface vertices.
    int numVertices;
    if (mClosed)
    {
        numVertices = (mNumSliceSamples + 1)*(mNumMedialSamples + 1);
    }
    else
    {
        numVertices = (mNumSliceSamples + 1)*mNumMedialSamples;
    }

    mVFormat = vformat;
    int vstride = vformat->GetStride();
    mVBuffer = new0 VertexBuffer(numVertices, vstride);

    ComputeSinCos();
    ComputeVertices();

    // Compute the surface normals.
    if (mVFormat->GetIndex(VertexFormat::AU_NORMAL) >= 0)
    {
        ComputeNormals();
    }

    // Compute the surface textures coordinates.
    if (tcoordMin && tcoordMax)
    {
        ComputeUVs(*tcoordMin, *tcoordMax);
    }

    // Compute the surface triangle connectivity.
    ComputeIndices(insideView);

    UpdateModelSpace(Visual::GU_NORMALS);
}
//----------------------------------------------------------------------------
TubeSurface::~TubeSurface ()
{
    delete0(mMedial);
    delete1(mSin);
    delete1(mCos);
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeSinCos ()
{
    // Compute slice vertex coefficients.  The first and last coefficients
    // are duplicated to allow a closed cross section that has two different
    // pairs of texture coordinates at the shared vertex.

    mSin = new1<float>(mNumSliceSamples + 1);
    mCos = new1<float>(mNumSliceSamples + 1);

    float invSliceSamples = 1.0f/(float)mNumSliceSamples;
    for (int i = 0; i < mNumSliceSamples; ++i)
    {
        float angle = Mathf::TWO_PI*invSliceSamples*i;
        mCos[i] = Mathf::Cos(angle);
        mSin[i] = Mathf::Sin(angle);
    }
    mSin[mNumSliceSamples] = mSin[0];
    mCos[mNumSliceSamples] = mCos[0];
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeVertices ()
{
    float tMin = mMedial->GetMinTime();
    float tRange = mMedial->GetMaxTime() - tMin;

    // Sampling by arc length requires the total length of the curve.
    float totalLength;
    if (mSampleByArcLength)
    {
        totalLength = mMedial->GetTotalLength();
    }
    else
    {
        totalLength = 0.0f;
    }

    // Vertex construction requires a normalized time (uses a division).
    float denom;
    if (mClosed)
    {
        denom = 1.0f/(float)mNumMedialSamples;
    }
    else
    {
        denom = 1.0f/(float)(mNumMedialSamples - 1);
    }

    VertexBufferAccessor vba(this);

    for (int m = 0, v = 0; m < mNumMedialSamples; ++m, ++v)
    {
        float t;
        if (mSampleByArcLength)
        {
            t = mMedial->GetTime(m*totalLength*denom);
        }
        else
        {
            t = tMin + m*tRange*denom;
        }

        float radius = mRadial(t);

        // Compute frame.
        Vector3f position, tangent, normal, binormal;
        if (mUpVector != Vector3f::ZERO)
        {
            // Always use 'up' vector N rather than curve normal.  You must
            // constrain the curve so that T and N are never parallel.  To
            // build the frame from this, let
            //     B = Cross(T,N)/Length(Cross(T,N))
            // and replace
            //     N = Cross(B,T)/Length(Cross(B,T)).
            position = mMedial->GetPosition(t);
            tangent = mMedial->GetTangent(t);
            binormal = tangent.UnitCross(mUpVector);
            normal = binormal.UnitCross(tangent);
        }
        else
        {
            // Use Frenet frame to create slices.
            mMedial->GetFrame(t, position, tangent, normal, binormal);
        }

        // Compute slice vertices, duplication at end point as noted earlier.
        int save = v;
        for (int i = 0; i < mNumSliceSamples; ++i, ++v)
        {
            vba.Position<Vector3f>(v) = position + radius*(mCos[i]*normal +
                mSin[i]*binormal);
        }
        vba.Position<Vector3f>(v) = vba.Position<Vector3f>(save);
    }

    if (mClosed)
    {
        for (int i = 0; i <= mNumSliceSamples; ++i)
        {
            int i1 = Index(i, mNumMedialSamples);
            int i0 = Index(i, 0);
            vba.Position<Vector3f>(i1) = vba.Position<Vector3f>(i0);
        }
    }
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeNormals ()
{
    int s, sM1, sP1, m, mM1, mP1;
    Vector3f dir0, dir1;

    VertexBufferAccessor vba(this);

    // Compute the interior normals (central differences).
    for (m = 1; m <= mNumMedialSamples - 2; ++m)
    {
        for (s = 0; s < mNumSliceSamples; ++s)
        {
            sM1 = (s > 0 ? s - 1 : mNumSliceSamples - 1);
            sP1 = s + 1;
            mM1 = m - 1;
            mP1 = m + 1;
            dir0 = vba.Position<Vector3f>(Index(sM1, m)) -
                vba.Position<Vector3f>(Index(sP1, m));
            dir1 = vba.Position<Vector3f>(Index(s, mM1)) -
                vba.Position<Vector3f>(Index(s, mP1));
            vba.Normal<Vector3f>(Index(s, m)) = dir0.UnitCross(dir1);
        }
        vba.Normal<Vector3f>(Index(mNumSliceSamples, m)) =
            vba.Normal<Vector3f>(Index(0, m));
    }

    // Compute the boundary normals.
    if (mClosed)
    {
        // Compute with central differences.
        for (s = 0; s < mNumSliceSamples; ++s)
        {
            sM1 = (s > 0 ? s - 1 : mNumSliceSamples - 1);
            sP1 = s + 1;

            // m = 0
            dir0 = vba.Position<Vector3f>(Index(sM1, 0)) -
                vba.Position<Vector3f>(Index(sP1, 0));
            dir1 = vba.Position<Vector3f>(Index(s, mNumMedialSamples - 1)) -
                vba.Position<Vector3f>(Index(s, 1));
            vba.Normal<Vector3f>(s) = dir0.UnitCross(dir1);

            // m = max
            vba.Normal<Vector3f>(Index(s, mNumMedialSamples)) =
                vba.Normal<Vector3f>(Index(s, 0));
        }
        vba.Normal<Vector3f>(Index(mNumSliceSamples, 0)) =
            vba.Normal<Vector3f>(Index(0, 0));
        vba.Normal<Vector3f>(Index(mNumSliceSamples, mNumMedialSamples)) =
            vba.Normal<Vector3f>(Index(0, mNumMedialSamples));
    }
    else
    {
        // Compute with one-sided differences.

        // m = 0
        for (s = 0; s < mNumSliceSamples; ++s)
        {
            sM1 = (s > 0 ? s - 1 : mNumSliceSamples - 1);
            sP1 = s + 1;
            dir0 = vba.Position<Vector3f>(Index(sM1, 0)) -
                vba.Position<Vector3f>(Index(sP1, 0));
            dir1 = vba.Position<Vector3f>(Index(s, 0)) -
                vba.Position<Vector3f>(Index(s, 1));
            vba.Normal<Vector3f>(Index(s, 0)) = dir0.UnitCross(dir1);
        }
        vba.Normal<Vector3f>(Index(mNumSliceSamples, 0)) =
            vba.Normal<Vector3f>(Index(0, 0));

        // m = max-1
        for (s = 0; s < mNumSliceSamples; ++s)
        {
            sM1 = (s > 0 ? s - 1 : mNumSliceSamples - 1);
            sP1 = s + 1;
            dir0 = vba.Position<Vector3f>(Index(sM1, mNumMedialSamples - 1)) -
                vba.Position<Vector3f>(Index(sP1, mNumMedialSamples - 1));
            dir1 = vba.Position<Vector3f>(Index(s, mNumMedialSamples - 2)) -
                vba.Position<Vector3f>(Index(s, mNumMedialSamples - 1));
            vba.Normal<Vector3f>(s) = dir0.UnitCross(dir1);
        }
        vba.Normal<Vector3f>(Index(mNumSliceSamples, mNumMedialSamples - 1)) =
            vba.Normal<Vector3f>(Index(0, mNumMedialSamples - 1));
    }
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeUVs (const Float2& tcoordMin,
    const Float2& tcoordMax)
{
    VertexBufferAccessor vba(this);

    Float2 tcoordRange(tcoordMax[0] - tcoordMin[0],
        tcoordMax[1] - tcoordMin[1]);
    int mMax = (mClosed ? mNumMedialSamples : mNumMedialSamples - 1);
    for (int m = 0, v = 0; m <= mMax; m++)
    {
        float mRatio = ((float)m)/((float)mMax);
        float mValue = tcoordMin[1] + mRatio*tcoordRange[1];
        for (int s = 0; s <= mNumSliceSamples; ++s, ++v)
        {
            float sRatio = ((float)s)/((float)mNumSliceSamples);
            float sValue = tcoordMin[0] + sRatio*tcoordRange[0];
            Float2 tcoord(sValue, mValue);
            const int numTCoords = VertexFormat::AM_MAX_TCOORD_UNITS;
            for (int unit = 0; unit < numTCoords; ++unit)
            {
                if (vba.HasTCoord(unit))
                {
                    assertion(vba.GetTCoordChannels(unit) == 2,
                        "Texture coordinate must be 2D\n");
                    vba.TCoord<Float2>(unit, v) = tcoord;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeIndices (bool insideView)
{
    int numTriangles;
    if (mClosed)
    {
        numTriangles = 2*mNumSliceSamples*mNumMedialSamples;
    }
    else
    {
        numTriangles = 2*mNumSliceSamples*(mNumMedialSamples-1);
    }

    mIBuffer = new0 IndexBuffer(3*numTriangles, sizeof(int));
    int* indices = (int*)mIBuffer->GetData();
    int m, mStart, i0, i1, i2, i3, i;
    for (m = 0, mStart = 0; m < mNumMedialSamples - 1; ++m)
    {
        i0 = mStart;
        i1 = i0 + 1;
        mStart += mNumSliceSamples + 1;
        i2 = mStart;
        i3 = i2 + 1;
        for (i = 0; i < mNumSliceSamples; ++i, indices += 6)
        {
            if (insideView)
            {
                indices[0] = i0++;
                indices[1] = i2;
                indices[2] = i1;
                indices[3] = i1++;
                indices[4] = i2++;
                indices[5] = i3++;
            }
            else  // outside view
            {
                indices[0] = i0++;
                indices[1] = i1;
                indices[2] = i2;
                indices[3] = i1++;
                indices[4] = i3++;
                indices[5] = i2++;
            }
        }
    }

    if (mClosed)
    {
        i0 = mStart;
        i1 = i0 + 1;
        i2 = 0;
        i3 = i2 + 1;
        for (i = 0; i < mNumSliceSamples; ++i, indices += 6)
        {
            if (insideView)
            {
                indices[0] = i0++;
                indices[1] = i2;
                indices[2] = i1;
                indices[3] = i1++;
                indices[4] = i2++;
                indices[5] = i3++;
            }
            else  // outside view
            {
                indices[0] = i0++;
                indices[1] = i1;
                indices[2] = i2;
                indices[3] = i1++;
                indices[4] = i3++;
                indices[5] = i2++;
            }
        }
    }
}
//----------------------------------------------------------------------------
void TubeSurface::GetTMinSlice (Vector3f* slice)
{
    VertexBufferAccessor vba(this);
    for (int i = 0; i <= mNumSliceSamples; ++i)
    {
        slice[i] = vba.Position<Vector3f>(i);
    }
}
//----------------------------------------------------------------------------
void TubeSurface::GetTMaxSlice (Vector3f* slice)
{
    VertexBufferAccessor vba(this);
    int j = mVBuffer->GetNumElements() - mNumSliceSamples - 1;
    for (int i = 0; i <= mNumSliceSamples; ++i, ++j)
    {
        slice[i] = vba.Position<Vector3f>(j);
    }
}
//----------------------------------------------------------------------------
void TubeSurface::UpdateSurface ()
{
    ComputeVertices();
    UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);

    if (mVFormat->GetIndex(VertexFormat::AU_NORMAL) >= 0)
    {
        ComputeNormals();
    }

    Renderer::UpdateAll(mVBuffer);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
TubeSurface::TubeSurface (LoadConstructor value)
    :
    TriMesh(value),
    mMedial(0),
    mRadial(0),
    mNumMedialSamples(0),
    mNumSliceSamples(0),
    mUpVector(Vector3f::ZERO),
    mSin(0),
    mCos(0),
    mClosed(false),
    mSampleByArcLength(false)
{
}
//----------------------------------------------------------------------------
void TubeSurface::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TriMesh::Load(source);

    source.Read(mNumMedialSamples);
    source.Read(mNumSliceSamples);
    source.ReadAggregate(mUpVector);
    source.ReadVR(mNumSliceSamples+1, mSin);
    source.ReadVR(mNumSliceSamples+1, mCos);
    source.ReadBool(mClosed);
    source.ReadBool(mSampleByArcLength);

    // TODO.  See note in TubeSurface::Save.
    mMedial = 0;
    mRadial = 0;

    WM5_END_DEBUG_STREAM_LOAD(TubeSurface, source);
}
//----------------------------------------------------------------------------
void TubeSurface::Link (InStream& source)
{
    TriMesh::Link(source);
}
//----------------------------------------------------------------------------
void TubeSurface::PostLink ()
{
    TriMesh::PostLink();
}
//----------------------------------------------------------------------------
bool TubeSurface::Register (OutStream& target) const
{
    return TriMesh::Register(target);
}
//----------------------------------------------------------------------------
void TubeSurface::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TriMesh::Save(target);

    target.Write(mNumMedialSamples);
    target.Write(mNumSliceSamples);
    target.WriteAggregate(mUpVector);
    target.WriteN(mNumSliceSamples + 1, mSin);
    target.WriteN(mNumSliceSamples + 1, mCos);
    target.WriteBool(mClosed);
    target.WriteBool(mSampleByArcLength);

    // TODO.  The class Curve3 is abstract and does not know about the data
    // representation for the derived-class object that is passed to the
    // TubeSurface constructor.  RadialFunction is a type of function.  Saving
    // the function pointer is useless since on loading, there is no current
    // way to 'link' to the correct function pointer.  Because of this, any
    // loaded TubeSurface object will require the application to manually set
    // the curve and function via the Medial() and Radial() members.
    //
    // Streaming support should  be added to the curve classes.

    WM5_END_DEBUG_STREAM_SAVE(TubeSurface, target);
}
//----------------------------------------------------------------------------
int TubeSurface::GetStreamingSize () const
{
    int size = TriMesh::GetStreamingSize();
    size += sizeof(mNumMedialSamples);
    size += sizeof(mNumSliceSamples);
    size += sizeof(mUpVector);
    size += (mNumSliceSamples+1)*sizeof(mSin[0]);
    size += (mNumSliceSamples+1)*sizeof(mCos[0]);
    size += WM5_BOOLSIZE(mClosed);
    size += WM5_BOOLSIZE(mSampleByArcLength);
    return size;
}
//----------------------------------------------------------------------------
