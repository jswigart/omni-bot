// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Particles.h"
#include "Wm5Camera.h"
#include "Wm5Culler.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TriMesh, Particles);
WM5_IMPLEMENT_STREAM(Particles);
WM5_IMPLEMENT_FACTORY(Particles);
WM5_IMPLEMENT_DEFAULT_NAMES(TriMesh, Particles);

//----------------------------------------------------------------------------
Particles::Particles (VertexFormat* vformat, VertexBuffer* vbuffer,
    int indexSize, Float4* positionSizes, float sizeAdjust)
    :
    TriMesh(vformat, vbuffer, 0),
    mPositionSizes(positionSizes)
{
    assertion(indexSize == 2 || indexSize == 4, "Invalid index size.\n");

    int numVertices = mVBuffer->GetNumElements();
    assertion((numVertices % 4) == 0,
        "Number of vertices must be a multiple of 4\n");
    mNumParticles = numVertices/4;
    mNumActive = mNumParticles;

    SetSizeAdjust(sizeAdjust);

    // Get access to the texture coordinates.
    VertexBufferAccessor vba(mVFormat, mVBuffer);

    assertion(vba.HasTCoord(0),
        "Texture coordinates must exist and use channel 0\n");

    // Set the texture coordinates to the standard ones.
    int i, j;
    for (i = 0, j = 0; i < mNumParticles; ++i)
    {
        vba.TCoord<Float2>(0, j++) = Float2(0.0f, 0.0f);
        vba.TCoord<Float2>(0, j++) = Float2(1.0f, 0.0f);
        vba.TCoord<Float2>(0, j++) = Float2(1.0f, 1.0f);
        vba.TCoord<Float2>(0, j++) = Float2(0.0f, 1.0f);
    }

    // Generate the indices for the quadrilaterals.
    mIBuffer = new0 IndexBuffer(6*mNumParticles, indexSize);

    int iFI, iFIp1, iFIp2, iFIp3;

    if (indexSize == 2)
    {
        short* indices = (short*)mIBuffer->GetData();
        for (i = 0; i < mNumParticles; ++i)
        {
            iFI = 4*i;
            iFIp1 = iFI+1;
            iFIp2 = iFI+2;
            iFIp3 = iFI+3;
            *indices++ = (short)iFI;
            *indices++ = (short)iFIp1;
            *indices++ = (short)iFIp2;
            *indices++ = (short)iFI;
            *indices++ = (short)iFIp2;
            *indices++ = (short)iFIp3;
        }
    }
    else  // indexSize == 4
    {
        int* indices = (int*)mIBuffer->GetData();
        for (i = 0; i < mNumParticles; ++i)
        {
            iFI = 4*i;
            iFIp1 = iFI+1;
            iFIp2 = iFI+2;
            iFIp3 = iFI+3;
            *indices++ = iFI;
            *indices++ = iFIp1;
            *indices++ = iFIp2;
            *indices++ = iFI;
            *indices++ = iFIp2;
            *indices++ = iFIp3;
        }
    }

    // Compute a bounding sphere based only on the particle locations.
    mModelBound.ComputeFromData(mNumParticles, sizeof(Float4),
        (const char*)positionSizes);
}
//----------------------------------------------------------------------------
Particles::~Particles ()
{
    delete1(mPositionSizes);
}
//----------------------------------------------------------------------------
void Particles::SetSizeAdjust (float sizeAdjust)
{
    if (sizeAdjust > 0.0f)
    {
        mSizeAdjust = sizeAdjust;
    }
    else
    {
        assertion(false, "Invalid size-adjust parameter\n");
        mSizeAdjust = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Particles::SetNumActive (int numActive)
{
    if (0 <= numActive && numActive <= mNumParticles)
    {
        mNumActive = numActive;
    }
    else
    {
        mNumActive = mNumParticles;
    }

    mIBuffer->SetNumElements(6*mNumActive);
    mVBuffer->SetNumElements(4*mNumActive);
}
//----------------------------------------------------------------------------
void Particles::GenerateParticles (const Camera* camera)
{
    // Get access to the positions.
    VertexBufferAccessor vba(mVFormat, mVBuffer);
    assertion(vba.HasPosition(), "Positions must exist\n");

    // Get camera axis directions in model space of particles.
    AVector UpR = WorldTransform.Inverse()*(camera->GetUVector() +
        camera->GetRVector());
    AVector UmR = WorldTransform.Inverse()*(camera->GetUVector() -
        camera->GetRVector());

    // Generate quadrilaterals as pairs of triangles.
    for (int i = 0, j = 0; i < mNumActive; ++i)
    {
        Float4& posSize = mPositionSizes[i];
        APoint position(posSize[0], posSize[1], posSize[2]);
        float trueSize = mSizeAdjust*posSize[3];
        AVector scaledUpR = trueSize*UpR;
        AVector scaledUmR = trueSize*UmR;

        vba.Position<Float3>(j++) = position - scaledUpR;
        vba.Position<Float3>(j++) = position - scaledUmR;
        vba.Position<Float3>(j++) = position + scaledUpR;
        vba.Position<Float3>(j++) = position + scaledUmR;
    }

    UpdateModelSpace(GU_NORMALS);
    Renderer::UpdateAll(mVBuffer);
}
//----------------------------------------------------------------------------
void Particles::GetVisibleSet (Culler& culler, bool noCull)
{
    GenerateParticles(culler.GetCamera());
    TriMesh::GetVisibleSet(culler, noCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Particles::Particles (LoadConstructor value)
    :
    TriMesh(value),
    mNumParticles(0),
    mPositionSizes(0),
    mSizeAdjust(0.0f),
    mNumActive(0)
{
}
//----------------------------------------------------------------------------
void Particles::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TriMesh::Load(source);

    source.ReadRR(mNumParticles, mPositionSizes);
    source.Read(mSizeAdjust);
    source.Read(mNumActive);

    WM5_END_DEBUG_STREAM_LOAD(Particles, source);
}
//----------------------------------------------------------------------------
void Particles::Link (InStream& source)
{
    TriMesh::Link(source);
}
//----------------------------------------------------------------------------
void Particles::PostLink ()
{
    TriMesh::PostLink();
}
//----------------------------------------------------------------------------
bool Particles::Register (OutStream& target) const
{
    return TriMesh::Register(target);
}
//----------------------------------------------------------------------------
void Particles::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TriMesh::Save(target);

    target.WriteW(mNumParticles, mPositionSizes);
    target.Write(mSizeAdjust);
    target.Write(mNumActive);

    WM5_END_DEBUG_STREAM_SAVE(Particles, target);
}
//----------------------------------------------------------------------------
int Particles::GetStreamingSize () const
{
    int size = TriMesh::GetStreamingSize();
    size += sizeof(mNumParticles);
    size += mNumParticles*sizeof(mPositionSizes[0]);
    size += sizeof(mSizeAdjust);
    size += sizeof(mNumActive);
    return size;
}
//----------------------------------------------------------------------------
