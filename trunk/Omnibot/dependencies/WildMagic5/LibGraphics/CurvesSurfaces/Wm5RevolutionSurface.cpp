// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5RevolutionSurface.h"
#include "Wm5Renderer.h"
#include "Wm5StandardMesh.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TriMesh, RevolutionSurface);
WM5_IMPLEMENT_STREAM(RevolutionSurface);
WM5_IMPLEMENT_FACTORY(RevolutionSurface);
WM5_IMPLEMENT_DEFAULT_NAMES(TriMesh, RevolutionSurface);

//----------------------------------------------------------------------------
RevolutionSurface::RevolutionSurface (Curve2f* curve, float xCenter,
    TopologyType topology, int numCurveSamples, int numRadialSamples,
    bool sampleByArcLength, bool outsideView, VertexFormat* vformat)
    :
    mCurve(curve),
    mXCenter(xCenter),
    mTopology(topology),
    mNumCurveSamples(numCurveSamples),
    mNumRadialSamples(numRadialSamples),
    mSin(0),
    mCos(0),
    mSamples(0),
    mSampleByArcLength(sampleByArcLength)
{
    ComputeSampleData();

    // The topology of the meshes is all that matters.  The vertices will be
    // modified later based on the curve of revolution.
    StandardMesh stdmesh(vformat, !outsideView);
    TriMesh* mesh = 0;
    switch (mTopology)
    {
    case REV_DISK_TOPOLOGY:
        mesh = stdmesh.Disk(mNumCurveSamples, mNumRadialSamples, 1.0f);
        break;
    case REV_CYLINDER_TOPOLOGY:
        mesh = stdmesh.Cylinder(mNumCurveSamples, mNumRadialSamples, 1.0f,
            1.0f, true);
        break;
    case REV_SPHERE_TOPOLOGY:
        mesh = stdmesh.Sphere(mNumCurveSamples, mNumRadialSamples, 1.0f);
        break;
    case REV_TORUS_TOPOLOGY:
        mesh = stdmesh.Torus(mNumCurveSamples, mNumRadialSamples, 1.0f,
            0.25f);
        break;
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }
    assertion(mesh != 0, "Failed to construct mesh\n");

    mVFormat = vformat;
    mVBuffer = mesh->GetVertexBuffer();

    // Generate the actual surface by replacing the vertex values.  NOTE:
    // Setting mIBuffer to zero acts as a flag that tells UpdateSurface
    // *not* to call Renderer::UpdateVertexBuffer(mVBuffer).  Only when the
    // application has constructed a RevolutionSurface wlil the update occur.
    mIBuffer = 0;
    UpdateSurface();

    mIBuffer = mesh->GetIndexBuffer();
    delete0(mesh);
}
//----------------------------------------------------------------------------
RevolutionSurface::~RevolutionSurface ()
{
    delete1(mSin);
    delete1(mCos);
    delete1(mSamples);
}
//----------------------------------------------------------------------------
void RevolutionSurface::ComputeSampleData ()
{
    // Compute slice vertex coefficients.  The first and last coefficients
    // are duplicated to allow a closed cross section that has two different
    // pairs of texture coordinates at the shared vertex.
    mSin = new1<float>(mNumRadialSamples + 1);
    mCos = new1<float>(mNumRadialSamples + 1);
    float invNumRadialSamples = 1.0f/(float)mNumRadialSamples;
    for (int i = 0; i < mNumRadialSamples; ++i)
    {
        float angle = Mathf::TWO_PI*invNumRadialSamples*i;
        mCos[i] = Mathf::Cos(angle);
        mSin[i] = Mathf::Sin(angle);
    }
    mSin[mNumRadialSamples] = mSin[0];
    mCos[mNumRadialSamples] = mCos[0];

    // Allocate storage for curve samples.
    mSamples = new1<Float3>(mNumCurveSamples);
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateSurface ()
{
    // Parameters for evaluating curve.
    float tMin = mCurve->GetMinTime();
    float tRange = mCurve->GetMaxTime() - tMin;

    // Sampling by arc length requires the total length of the curve.
    float totalLength;
    if (mSampleByArcLength)
    {
        totalLength = mCurve->GetTotalLength();
    }
    else
    {
        totalLength = 0.0f;
    }

    // Sample the curve of revolution.
    float invNumCurveSamplesM1 = 1.0f/(float)(mNumCurveSamples - 1);
    for (int i = 0; i < mNumCurveSamples; ++i)
    {
        float t;
        if (mSampleByArcLength)
        {
            t = mCurve->GetTime(i*totalLength*invNumCurveSamplesM1);
        }
        else
        {
            t = tMin + i*tRange*invNumCurveSamplesM1;
        }

        Vector2f position = mCurve->GetPosition(t);
        mSamples[i][0] = position.X();
        mSamples[i][1] = 0.0f;
        mSamples[i][2] = position.Y();
    }

    // Store the samples and their rotated equivalents.  The storage layout
    // is dependent on the topology of the mesh.
    switch (mTopology)
    {
    case REV_DISK_TOPOLOGY:
        UpdateDisk();
        break;
    case REV_CYLINDER_TOPOLOGY:
        UpdateCylinder();
        break;
    case REV_SPHERE_TOPOLOGY:
        UpdateSphere();
        break;
    case REV_TORUS_TOPOLOGY:
        UpdateTorus();
        break;
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }

    UpdateModelSpace(Visual::GU_NORMALS);

    // See the comments in the constructor about mIBuffer and the vertex
    // buffer update.
    if (mIBuffer)
    {
        Renderer::UpdateAll(mVBuffer);
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateDisk ()
{
    VertexBufferAccessor vba(this);

    // Get the initial ray.
    int c;
    for (c = 0; c < mNumCurveSamples; c++)
    {
        vba.Position<Float3>(c) = mSamples[c];
    }

    // The remaining rays are obtained by revolution.
    int numCurveSamplesM1 = mNumCurveSamples - 1;
    for (int r = 1; r < mNumRadialSamples; ++r)
    {
        for (c = 1; c < mNumCurveSamples; ++c)
        {
            float radius = mSamples[c][0] - mXCenter;
            if (radius < 0.0f)
            {
                radius = 0.0f;
            }
            int i = c + numCurveSamplesM1*r;
            Float3 position(mXCenter + radius*mCos[r], radius*mSin[r],
                mSamples[c][2]);
            vba.Position<Float3>(i) = position;
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateSphere ()
{
    VertexBufferAccessor vba(this);

    int numVertices = mVBuffer->GetNumElements();

    // Set the South pole.
    vba.Position<Float3>(numVertices - 2) = mSamples[0];

    // Set the north pole.
    vba.Position<Float3>(numVertices - 1) = mSamples[mNumCurveSamples - 1];

    // Set the initial and final ray.
    int c, i;
    for (c = 1; c <= mNumCurveSamples - 2; ++c)
    {
        i = (c-1)*(mNumRadialSamples + 1);
        vba.Position<Float3>(i) = mSamples[c];

        i += mNumRadialSamples;
        vba.Position<Float3>(i) = mSamples[c];
    }

    // The remaining rays are obtained by revolution.
    for (int r = 1; r < mNumRadialSamples; ++r)
    {
        for (c = 1; c <= mNumCurveSamples-2; ++c)
        {
            float radius = mSamples[c][0] - mXCenter;
            if (radius < 0.0f)
            {
                radius = 0.0f;
            }
            i = (c - 1)*(mNumRadialSamples + 1) + r;
            Float3 position(mXCenter + radius*mCos[r], radius*mSin[r],
                mSamples[c][2]);
            vba.Position<Float3>(i) = position;
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateCylinder ()
{
    VertexBufferAccessor vba(this);

    // Set the initial and final ray.
    int c, i;
    for (c = 0; c < mNumCurveSamples; ++c)
    {
        i = c*(mNumRadialSamples + 1);
        vba.Position<Float3>(i) = mSamples[c];

        i += mNumRadialSamples;
        vba.Position<Float3>(i) = mSamples[c];
    }

    // The remaining rays are obtained by revolution.
    for (int r = 1; r < mNumRadialSamples; ++r)
    {
        for (c = 0; c < mNumCurveSamples; ++c)
        {
            float radius = mSamples[c][0] - mXCenter;
            if (radius < 0.0f)
            {
                radius = 0.0f;
            }
            i = c*(mNumRadialSamples + 1) + r;
            Float3 position(mXCenter + radius*mCos[r], radius*mSin[r],
                mSamples[c][2]);
            vba.Position<Float3>(i) = position;
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateTorus ()
{
    VertexBufferAccessor vba(this);

    // Set the initial and final ray.
    int numVertices = mVBuffer->GetNumElements();
    int c, i;
    for (c = 0; c < mNumCurveSamples; ++c)
    {
        i = c*(mNumRadialSamples + 1);
        vba.Position<Float3>(i) = mSamples[c];

        i += mNumRadialSamples;
        vba.Position<Float3>(i) = mSamples[c];
    }

    // The remaining rays are obtained by revolution.
    int r;
    for (r = 1; r < mNumRadialSamples; ++r)
    {
        for (c = 0; c < mNumCurveSamples; ++c)
        {
            float radius = mSamples[c][0] - mXCenter;
            if (radius < 0.0f)
            {
                radius = 0.0f;
            }
            i = c*(mNumRadialSamples + 1) + r;
            Float3 position(mXCenter + radius*mCos[r], radius*mSin[r],
                mSamples[c][2]);
            vba.Position<Float3>(i) = position;
        }
    }

    i = numVertices - (mNumRadialSamples + 1);
    for (r = 0; r <= mNumRadialSamples; ++r, ++i)
    {
        vba.Position<Float3>(i) = vba.Position<Float3>(r);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
RevolutionSurface::RevolutionSurface (LoadConstructor value)
    :
    TriMesh(value),
    mCurve(0),
    mXCenter(0.0f),
    mTopology(MAX_TOPOLOGY_TYPES),
    mNumCurveSamples(0),
    mNumRadialSamples(0),
    mSin(0),
    mCos(0),
    mSamples(0),
    mSampleByArcLength(false)
{
}
//----------------------------------------------------------------------------
void RevolutionSurface::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TriMesh::Load(source);

    source.Read(mXCenter);
    source.ReadEnum(mTopology);
    source.Read(mNumCurveSamples);
    source.Read(mNumRadialSamples);
    source.ReadVR(mNumRadialSamples + 1, mSin);
    source.ReadVR(mNumRadialSamples + 1, mCos);
    source.ReadBool(mSampleByArcLength);

    // TODO.  See note in RevolutionSurface::Save.
    mCurve = 0;

    WM5_END_DEBUG_STREAM_LOAD(RevolutionSurface, source);
}
//----------------------------------------------------------------------------
void RevolutionSurface::Link (InStream& source)
{
    TriMesh::Link(source);
}
//----------------------------------------------------------------------------
void RevolutionSurface::PostLink ()
{
    TriMesh::PostLink();
}
//----------------------------------------------------------------------------
bool RevolutionSurface::Register (OutStream& target) const
{
    return TriMesh::Register(target);
}
//----------------------------------------------------------------------------
void RevolutionSurface::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TriMesh::Save(target);

    target.Write(mXCenter);
    target.WriteEnum(mTopology);
    target.Write(mNumCurveSamples);
    target.Write(mNumRadialSamples);
    target.WriteN(mNumRadialSamples + 1, mSin);
    target.WriteN(mNumRadialSamples + 1, mCos);
    target.WriteBool(mSampleByArcLength);

    // TODO.  The class Curve2 is abstract and does not know about the data
    // representation for the derived-class object that is passed to the
    // RevolutionSurface constructor.  Because of this, any loaded
    // RevolutionSurface object will require the application to manually set
    // the curve via the Curve() member.
    //
    // Streaming support should be added to the curve classes.

    WM5_END_DEBUG_STREAM_SAVE(RevolutionSurface, target);
}
//----------------------------------------------------------------------------
int RevolutionSurface::GetStreamingSize () const
{
    int size = TriMesh::GetStreamingSize();
    size += sizeof(mXCenter);
    size += WM5_ENUMSIZE(mTopology);
    size += sizeof(mNumCurveSamples);
    size += sizeof(mNumRadialSamples);
    size += (mNumRadialSamples+1)*sizeof(mSin[0]);
    size += (mNumRadialSamples+1)*sizeof(mCos[0]);
    size += WM5_BOOLSIZE(mSampleByArcLength);
    return size;
}
//----------------------------------------------------------------------------
