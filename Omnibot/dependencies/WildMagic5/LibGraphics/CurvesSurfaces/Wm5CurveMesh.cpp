// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CurveMesh.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Polysegment, CurveMesh);
WM5_IMPLEMENT_STREAM(CurveMesh);
WM5_IMPLEMENT_FACTORY(CurveMesh);

//----------------------------------------------------------------------------
CurveMesh::CurveMesh (VertexFormat* vformat, VertexBuffer* vbuffer,
    CurveSegmentPtr* segments, FloatArray* params, bool allowDynamicChange)
    :
    Polysegment(vformat, vbuffer, true),
    mOrigVBuffer(vbuffer),
    mOrigParams(params),
    mSegments(segments),
    mNumFullVertices(vbuffer->GetNumElements()),
    mNumSegments(vbuffer->GetNumElements() - 1),
    mLevel(0),
    mAllowDynamicChange(allowDynamicChange)
{
    if (mAllowDynamicChange)
    {
        mCInfo = new1<CurveInfo>(mNumFullVertices);
        InitializeCurveInfo();
    }
    else
    {
        mCInfo = 0;
    }
}
//----------------------------------------------------------------------------
CurveMesh::~CurveMesh ()
{
    delete1(mSegments);
    delete1(mCInfo);
}
//----------------------------------------------------------------------------
void CurveMesh::SetLevel (int level)
{
    if (!mOrigVBuffer)
    {
        // The mesh is locked, so no subdivision is allowed.
        return;
    }

    if (level == mLevel)
    {
        return;
    }
    mLevel = level;

    // Allocate the storage for subdivision.
    int numTotalVertices, numTotalEdges;
    Edge* edges;
    Allocate(numTotalVertices, numTotalEdges, edges);

    // Subdivide the mesh.
    int numVertices = mNumSegments + 1;
    int numEdges = mNumSegments;
    int i;
    for (i = 0; i < mLevel; ++i)
    {
        int nextNumVertices = numVertices + numEdges;
        int nextNumEdges = 2*numEdges;
        Subdivide(numVertices, numEdges, edges);
        assertion(numVertices == nextNumVertices && numEdges == nextNumEdges,
            "Unexpected condition\n");
        numVertices = nextNumVertices;
        numEdges = nextNumEdges;
    }

    // Check to make sure the implementation is correct.
    assertion(numVertices == numTotalVertices && numEdges == numTotalEdges,
        "Unexpected condition\n");
    SetNumSegments(numTotalVertices - 1);
    mNumFullVertices = numTotalVertices;

    UpdateModelSpace(Visual::GU_NORMALS);

    delete1(edges);
    OnDynamicChange();
    Renderer::BindAll(mVBuffer);
}
//----------------------------------------------------------------------------
int CurveMesh::GetLevel () const
{
    return mLevel;
}
//----------------------------------------------------------------------------
void CurveMesh::Allocate (int& numTotalVertices, int& numTotalEdges,
    Edge*& edges)
{
    // The number of original vertices.
    int numOrigVertices = mNumSegments + 1;

    // The number of original edges.
    int numOrigEdges = mNumSegments;

    // Determine the various quantities needed for subdivision.
    numTotalVertices = numOrigVertices;
    numTotalEdges = numOrigEdges;
    int i;
    for (i = 0; i < mLevel; ++i)
    {
        numTotalVertices = numTotalVertices + numTotalEdges;
        numTotalEdges = 2*numTotalEdges;
    }

    // Allocate storage for the subdivision.
    int vstride = mVFormat->GetStride();
    mVBuffer = new0 VertexBuffer(numTotalVertices, vstride);
    edges = new1<Edge>(numTotalEdges);
    if (mAllowDynamicChange)
    {
        delete1(mCInfo);
        mCInfo = new1<CurveInfo>(numTotalVertices);
        InitializeCurveInfo();
    }

    // Initialize the edge data structures.  Copy the original vertices to
    // their proper locations in the new vertex buffer.
    const float* origParam = mOrigParams->GetData();
    char* origData = mOrigVBuffer->GetData();
    char* fullData = mVBuffer->GetData();
    int index = 0;
    int indexDelta = (1 << mLevel);
    int fullStride = vstride*indexDelta;
    for (i = 0; i < numOrigEdges; ++i)
    {
        Edge& edge = edges[i];

        // The curve affecting the edge.
        edge.Segment = mSegments[i];

        // Get the vertex indices for the edge.
        edge.V[0] = index;
        index += indexDelta;
        edge.V[1] = index;

        // Get the parameters associated with the vertices.
        edge.Param[0] = *origParam++;
        edge.Param[1] = *origParam++;

        // Copy the vertex for edge.V[0].
        memcpy(fullData, origData, vstride);
        origData += vstride;
        fullData += fullStride;
    }

    // Copy the last vertex.
    memcpy(fullData, origData, vstride);
}
//----------------------------------------------------------------------------
void CurveMesh::Subdivide (int& numVertices, int& numEdges, Edge* edges)
{
    // The subdivision is computed in-place, so you need to fill in the new
    // edge slots starting from the end of the array and working towards the
    // beginning.  This avoids overwriting old edges not yet subdivided.
    VertexBufferAccessor vba(mVFormat, mVBuffer);

    for (int old = numEdges-1, curr = 2*numEdges - 1; old >= 0; --old)
    {
        Edge& edge = edges[old];

        // Compute the vertex at the average of the parameter values.
        int v0 = edge.V[0];
        int v1 = edge.V[1];
        int vMid = (v0 + v1)/2;
        float paramMid = 0.5f*(edge.Param[0] + edge.Param[1]);
        vba.Position<Float3>(vMid) = edge.Segment->P(paramMid);

        // Compute the average of vertex attributes.
        if (vba.HasNormal())
        {
            // Use the curve normal.
            vba.Normal<Float3>(vMid) = edge.Segment->Normal(paramMid);
        }

        int unit, i;
        float* data0;
        float* data1;
        float* data;
        const int numColorUnits = VertexFormat::AM_MAX_COLOR_UNITS;
        for (unit = 0; unit < numColorUnits; ++unit)
        {
            if (vba.HasColor(unit))
            {
                data0 = vba.ColorTuple(unit, v0);
                data1 = vba.ColorTuple(unit, v1);
                data = vba.ColorTuple(unit, vMid);
                for (i = 0; i < vba.GetColorChannels(unit); ++i)
                {
                    data[i] = 0.5f*(data0[i] + data1[i]);
                }
            }
        }

        const int numTCoordUnits = VertexFormat::AM_MAX_TCOORD_UNITS;
        for (unit = 0; unit < numTCoordUnits; ++unit)
        {
            if (vba.HasTCoord(unit))
            {
                data0 = vba.TCoordTuple(unit, v0);
                data1 = vba.TCoordTuple(unit, v1);
                data = vba.TCoordTuple(unit, vMid);
                for (i = 0; i < vba.GetTCoordChannels(unit); ++i)
                {
                    data[i] = 0.5f*(data0[i] + data1[i]);
                }
            }
        }

        if (mAllowDynamicChange)
        {
            mCInfo[vMid].Segment = edge.Segment;
            mCInfo[vMid].Param = paramMid;
        }

        // Subdivide the edge.
        Edge& edge1 = edges[curr--];
        edge1.Segment = edge.Segment;
        edge1.V[0] = vMid;
        edge1.V[1] = edge.V[1];
        edge1.Param[0] = paramMid;
        edge1.Param[1] = edge.Param[1];

        Edge& edge0 = edges[curr--];
        edge0.Segment = edge.Segment;
        edge0.V[0] = edge.V[0];
        edge0.V[1] = vMid;
        edge0.Param[0] = edge.Param[0];
        edge0.Param[1] = paramMid;

        ++numVertices;
    }

    numEdges = 2*numEdges;
}
//----------------------------------------------------------------------------
void CurveMesh::OnDynamicChange ()
{
    if (mAllowDynamicChange)
    {
        VertexBufferAccessor vba(mVFormat, mVBuffer);

        for (int i = 0; i < mNumFullVertices; ++i)
        {
            CurveInfo& ci = mCInfo[i];
            vba.Position<Float3>(i) = ci.Segment->P(ci.Param);
        }

        UpdateModelSpace(Visual::GU_NORMALS);
    }
}
//----------------------------------------------------------------------------
void CurveMesh::Lock ()
{
    if (mOrigVBuffer)
    {
        mOrigVBuffer = 0;
        mOrigParams = 0;
        delete1(mSegments);
        mSegments = 0;
    }

    // The array mCInfo is not deleted.  This allows dynamic updates of
    // the vertices of the mesh, even though the mesh is "locked".
}
//----------------------------------------------------------------------------
bool CurveMesh::IsLocked () const
{
    return mOrigVBuffer == 0;
}
//----------------------------------------------------------------------------
void CurveMesh::InitializeCurveInfo ()
{
    // Because vertices are shared by edges, the last visited edge for a
    // vertex donates its segment and parameter values to that vertex.
    const float* params = mOrigParams->GetData();
    int index = 0;
    int indexDelta = (1 << mLevel);
    for (int i = 0; i < mNumSegments; ++i)
    {
        int v0 = index;
        index += indexDelta;
        int v1 = index;
        mCInfo[v0].Segment = mSegments[i];
        mCInfo[v0].Param = *params++;
        mCInfo[v1].Segment = mSegments[i];
        mCInfo[v1].Param = *params++;
    }
}
//----------------------------------------------------------------------------
CurveMesh::Edge::Edge ()
{
    Segment = 0;
    V[0] = -1;
    V[1] = -1;
    Param[0] = 0.0f;
    Param[1] = 0.0f;
}
//----------------------------------------------------------------------------
CurveMesh::CurveInfo::CurveInfo ()
{
    Segment = 0;
    Param = 0.0f;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* CurveMesh::GetObjectByName (const std::string& name)
{
    Object* found = Polysegment::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mOrigVBuffer, name, found);
    WM5_GET_OBJECT_BY_NAME(mOrigParams, name, found);
    if (mSegments)
    {
        for (int i = 0; i < mNumSegments; ++i)
        {
            WM5_GET_OBJECT_BY_NAME(mSegments[i], name, found);
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void CurveMesh::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Polysegment::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mOrigVBuffer, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mOrigParams, name, objects);
    if (mSegments)
    {
        for (int i = 0; i < mNumSegments; ++i)
        {
            WM5_GET_ALL_OBJECTS_BY_NAME(mSegments[i], name, objects);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
CurveMesh::CurveMesh (LoadConstructor value)
    :
    Polysegment(value),
    mSegments(0),
    mNumFullVertices(0),
    mNumSegments(0),
    mLevel(0),
    mAllowDynamicChange(false),
    mCInfo(0)
{
}
//----------------------------------------------------------------------------
void CurveMesh::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Polysegment::Load(source);

    source.Read(mNumFullVertices);
    source.Read(mNumSegments);
    source.Read(mLevel);
    source.ReadBool(mAllowDynamicChange);
    source.ReadPointer(mOrigVBuffer);
    source.ReadPointer(mOrigParams);

    bool locked = (mOrigVBuffer == 0);
    if (!locked)
    {
        source.ReadPointerVR(mNumSegments, mSegments);
    }

    if (mAllowDynamicChange)
    {
        mCInfo = new1<CurveInfo>(mNumFullVertices);
        for (int i = 0; i < mNumFullVertices; ++i)
        {
            source.ReadPointer(mCInfo[i].Segment);
            source.Read(mCInfo[i].Param);
        }
    }

    WM5_END_DEBUG_STREAM_LOAD(CurveMesh, source);
}
//----------------------------------------------------------------------------
void CurveMesh::Link (InStream& source)
{
    Polysegment::Link(source);

    source.ResolveLink(mOrigVBuffer);
    source.ResolveLink(mOrigParams);
    source.ResolveLink(mNumSegments, mSegments);
    if (mCInfo)
    {
        for (int i = 0; i < mNumFullVertices; ++i)
        {
            source.ResolveLink(mCInfo[i].Segment);
        }
    }
}
//----------------------------------------------------------------------------
void CurveMesh::PostLink ()
{
    Polysegment::PostLink();
}
//----------------------------------------------------------------------------
bool CurveMesh::Register (OutStream& target) const
{
    if (Polysegment::Register(target))
    {
        target.Register(mOrigVBuffer);
        target.Register(mOrigParams);
        target.Register(mNumSegments, mSegments);
        if (mCInfo)
        {
            for (int i = 0; i < mNumFullVertices; ++i)
            {
                target.Register(mCInfo[i].Segment);
            }
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void CurveMesh::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Polysegment::Save(target);

    target.Write(mNumFullVertices);
    target.Write(mNumSegments);
    target.Write(mLevel);
    target.WriteBool(mAllowDynamicChange);
    target.WritePointer(mOrigVBuffer);
    target.WritePointer(mOrigParams);
    target.WritePointerN(mNumSegments, mSegments);

    if (mCInfo)
    {
        for (int i = 0; i < mNumFullVertices; ++i)
        {
            target.WritePointer(mCInfo[i].Segment);
            target.Write(mCInfo[i].Param);
        }
    }

    WM5_END_DEBUG_STREAM_SAVE(CurveMesh, target);
}
//----------------------------------------------------------------------------
int CurveMesh::GetStreamingSize () const
{
    int size = Polysegment::GetStreamingSize();
    size += sizeof(mNumFullVertices);
    size += sizeof(mNumSegments);
    size += sizeof(mLevel);
    size += WM5_BOOLSIZE(mAllowDynamicChange);
    size += WM5_POINTERSIZE(mOrigVBuffer);
    size += WM5_POINTERSIZE(mOrigParams);

    if (mSegments)
    {
        size += mNumSegments*WM5_POINTERSIZE(mSegments[0]);
    }

    if (mCInfo)
    {
        size += mNumFullVertices*WM5_POINTERSIZE(mCInfo[0].Segment);
        size += mNumFullVertices*sizeof(mCInfo[0].Param);
    }

    return size;
}
//----------------------------------------------------------------------------
