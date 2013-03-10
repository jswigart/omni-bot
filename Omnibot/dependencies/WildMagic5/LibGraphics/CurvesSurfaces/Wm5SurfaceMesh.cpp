// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5SurfaceMesh.h"
#include "Wm5EdgeKey.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TriMesh, SurfaceMesh);
WM5_IMPLEMENT_STREAM(SurfaceMesh);
WM5_IMPLEMENT_FACTORY(SurfaceMesh);

//----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh (VertexFormat* vformat, VertexBuffer* vbuffer,
    IndexBuffer* ibuffer, Float2Array* params, SurfacePatchPtr* patches,
    bool allowDynamicChange)
    :
    TriMesh(vformat, vbuffer, ibuffer),
    mOrigVBuffer(vbuffer),
    mOrigIBuffer(ibuffer),
    mOrigParams(params),
    mPatches(patches),
    mNumFullVertices(vbuffer->GetNumElements()),
    mNumPatches(ibuffer->GetNumElements()/3),
    mLevel(0),
    mAllowDynamicChange(allowDynamicChange)
{
    if (mAllowDynamicChange)
    {
        mSInfo = new1<SurfaceInfo>(mNumFullVertices);
        InitializeSurfaceInfo();
    }
    else
    {
        mSInfo = 0;
    }
}
//----------------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh ()
{
    delete1(mPatches);
    delete1(mSInfo);
}
//----------------------------------------------------------------------------
void SurfaceMesh::SetLevel (int level)
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
    int numTotalVertices, numTotalEdges, numTotalTriangles;
    EdgeMap edgeMap;
    Triangle* triangles;
    Allocate(numTotalVertices, numTotalEdges, edgeMap, numTotalTriangles,
        triangles);

    // Subdivide the mesh.
    int numVertices = mOrigVBuffer->GetNumElements();
    int numEdges = (int)edgeMap.size();
    int numTriangles = mOrigIBuffer->GetNumElements()/3;
    int i;
    for (i = 0; i < mLevel; ++i)
    {
        int nextNumVertices = numVertices + numEdges;
        int nextNumEdges = 2*numEdges + 3*numTriangles;
        int nextNumTriangles = 4*numTriangles;
        Subdivide(numVertices, numEdges, edgeMap, numTriangles, triangles);
        assertion(numVertices == nextNumVertices
            && numEdges == nextNumEdges && numTriangles == nextNumTriangles,
            "Unexpected condition\n");
        numVertices = nextNumVertices;
        numEdges = nextNumEdges;
        numTriangles = nextNumTriangles;
    }

    // Check to make sure the implementation is correct.
    assertion(numVertices == numTotalVertices
        && numEdges == numTotalEdges && numTriangles == numTotalTriangles,
        "Unexpected condition\n");
    mNumFullVertices = numTotalVertices;

    int numTotalIndices = 3*numTotalTriangles;
    mIBuffer = new0 IndexBuffer(numTotalIndices, sizeof(int));
    int* indices = (int*)mIBuffer->GetData();
    for (i = 0; i < numTotalTriangles; ++i)
    {
        Triangle& tri = triangles[i];
        for (int j = 0; j < 3; ++j)
        {
            *indices++ = tri.V[j];
        }
    }
    Renderer::UpdateAll(mIBuffer);

    delete1(triangles);
    OnDynamicChange();
    UpdateModelSpace(Visual::GU_NORMALS);
}
//----------------------------------------------------------------------------
int SurfaceMesh::GetLevel () const
{
    return mLevel;
}
//----------------------------------------------------------------------------
void SurfaceMesh::Allocate (int& numTotalVertices, int& numTotalEdges,
    EdgeMap& edgeMap, int& numTotalTriangles, Triangle*& triangles)
{
    // The number of original vertices.
    int numOrigVertices = mOrigVBuffer->GetNumElements();

    // The number of original indices and triangles.
    int numOrigIndices = mOrigIBuffer->GetNumElements();
    int numOrigTriangles = numOrigIndices/3;

    // Count the number of unique edges in the mesh.
    int* indices = (int*)mOrigIBuffer->GetData();
    std::set<EdgeKey> eKeys;
    int i, v0, v1, v2;
    for (i = 0; i < numOrigTriangles; ++i)
    {
        // Get the vertex indices for the triangle.
        v0 = *indices++;
        v1 = *indices++;
        v2 = *indices++;

        // Store the edges with the vertex indices.
        eKeys.insert(EdgeKey(v0, v1));
        eKeys.insert(EdgeKey(v1, v2));
        eKeys.insert(EdgeKey(v2, v0));
    }

    int numOrigEdges = (int)eKeys.size();

    // Determine the various quantities needed for subdivision.
    numTotalVertices = numOrigVertices;
    numTotalEdges = numOrigEdges;
    numTotalTriangles = numOrigTriangles;
    for (i = 0; i < mLevel; ++i)
    {
        numTotalVertices = numTotalVertices + numTotalEdges;
        numTotalEdges = 2*numTotalEdges + 3*numTotalTriangles;
        numTotalTriangles = 4*numTotalTriangles;
    }

    // Allocate storage for the subdivision.
    int vstride = mVFormat->GetStride();
    mVBuffer = new0 VertexBuffer(numTotalVertices, vstride);
    triangles = new1<Triangle>(numTotalTriangles);
    if (mAllowDynamicChange)
    {
        delete1(mSInfo);
        mSInfo = new1<SurfaceInfo>(numTotalVertices);
        InitializeSurfaceInfo();
    }

    // Copy the original vertices to the storage.
    char* origData = mOrigVBuffer->GetData();
    char* fullData = mVBuffer->GetData();
    int numOrigBytes = mOrigVBuffer->GetNumBytes();
    memcpy(fullData, origData, numOrigBytes);

    // Initialize the edge and triangle data structures.
    const Float2* origParam = mOrigParams->GetData();
    indices = (int*)mOrigIBuffer->GetData();
    for (i = 0; i < numOrigTriangles; ++i)
    {
        Triangle& tri = triangles[i];

        // Surface affecting the triangle.
        tri.Patch = mPatches[i];

        // Get the vertex indices for the triangle.
        v0 = *indices++;
        v1 = *indices++;
        v2 = *indices++;
        tri.V[0] = v0;
        tri.V[1] = v1;
        tri.V[2] = v2;

        // Get the parameters associated with the vertices.
        const Float2& param0 = *origParam++;
        const Float2& param1 = *origParam++;
        const Float2& param2 = *origParam++;

        // Store the edges with the surface and parameter values.
        InsertInto(edgeMap, tri.Patch, v0, v1, param0, param1, 1);
        InsertInto(edgeMap, tri.Patch, v1, v2, param1, param2, 1);
        InsertInto(edgeMap, tri.Patch, v2, v0, param2, param0, 1);
    }

    assertion(numOrigEdges == (int)edgeMap.size(), "Unexpected condition\n");
}
//----------------------------------------------------------------------------
void SurfaceMesh::Subdivide (int& numVertices, int& numEdges,
    EdgeMap& edgeMap, int& numTriangles, Triangle* triangles)
{
    VertexBufferAccessor vba(mVFormat, mVBuffer);

    EdgeMap::iterator iter = edgeMap.begin();
    EdgeMap::iterator end = edgeMap.end();
    for (/**/; iter != end; ++iter)
    {
        // Compute the vertex at the average of the parameter values.  The
        // index and parameter values of the new vertex will be used when
        // subdividing the triangles.
        Edge& edge = iter->second;
        edge.ParamMid[0] = 0.5f*(edge.Param[0][0] + edge.Param[1][0]);
        edge.ParamMid[1] = 0.5f*(edge.Param[0][1] + edge.Param[1][1]);
        vba.Position<Float3>(numVertices) = edge.Patch->P(
            edge.ParamMid[0], edge.ParamMid[1]);

        // Compute the average of vertex attributes.
        if (vba.HasNormal())
        {
            // Use the surface normal.
            vba.Normal<Float3>(numVertices) = edge.Patch->Normal(
                edge.ParamMid[0], edge.ParamMid[1]);
        }

        int v0 = edge.V[0];
        int v1 = edge.V[1];
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
                data = vba.ColorTuple(unit, numVertices);
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
                data = vba.TCoordTuple(unit, numVertices);
                for (i = 0; i < vba.GetTCoordChannels(unit); ++i)
                {
                    data[i] = 0.5f*(data0[i] + data1[i]);
                }
            }
        }

        if (mAllowDynamicChange)
        {
            mSInfo[numVertices].Patch = edge.Patch;
            mSInfo[numVertices].Param = edge.ParamMid;
        }

        edge.VMid = numVertices++;
    }

    // This allows numTriangles to change inside the loop.
    const int tmpNumTriangles = numTriangles;
    int i;
    for (i = 0; i < tmpNumTriangles; ++i)
    {
        Triangle& tri0 = triangles[i];

        // Get the vertices.
        int v0 = tri0.V[0];
        int v1 = tri0.V[1];
        int v2 = tri0.V[2];

        // Get the edge information.
        EdgeKey key01(v0, v1);
        EdgeMap::iterator iter = edgeMap.find(key01);
        Edge& edge01 = iter->second;

        EdgeKey key12(v1, v2);
        iter = edgeMap.find(key12);
        Edge& edge12 = iter->second;

        EdgeKey key20(v2, v0);
        iter = edgeMap.find(key20);
        Edge& edge20 = iter->second;

        Float2 param0 = edge01.Param[edge01.V[0] == v0 ? 0 : 1];
        Float2 param1 = edge12.Param[edge12.V[0] == v1 ? 0 : 1];
        Float2 param2 = edge20.Param[edge20.V[0] == v2 ? 0 : 1];

        // Get the midpoint information.
        int v01 = edge01.VMid;
        Float2 param01 = edge01.ParamMid;
        int v12 = edge12.VMid;
        Float2 param12 = edge12.ParamMid;
        int v20 = edge20.VMid;
        Float2 param20 = edge20.ParamMid;

        // Tf done with edges, remove them.
        if (--edge01.References == 0)
        {
            edgeMap.erase(key01);
        }
        if (--edge12.References == 0)
        {
            edgeMap.erase(key12);
        }
        if (--edge20.References == 0)
        {
            edgeMap.erase(key20);
        }

        // Insert the six new boundary edges.
        InsertInto(edgeMap, tri0.Patch, v0, v01, param0, param01, 1);
        InsertInto(edgeMap, tri0.Patch, v01, v1, param01, param1, 1);
        InsertInto(edgeMap, tri0.Patch, v1, v12, param1, param12, 1);
        InsertInto(edgeMap, tri0.Patch, v12, v2, param12, param2, 1);
        InsertInto(edgeMap, tri0.Patch, v2, v20, param2, param20, 1);
        InsertInto(edgeMap, tri0.Patch, v20, v0, param20, param0, 1);

        // Insert the three new interior edges.
        InsertInto(edgeMap, tri0.Patch, v01, v12, param01, param12, 2);
        InsertInto(edgeMap, tri0.Patch, v12, v20, param12, param20, 2);
        InsertInto(edgeMap, tri0.Patch, v20, v01, param20, param01, 2);

        // The old triangle is replaced by the new interior triangle.
        tri0.V[0] = v01;
        tri0.V[1] = v12;
        tri0.V[2] = v20;

        // Insert the remaining triangles in new slots.
        Triangle& tri1 = triangles[numTriangles++];
        tri1.Patch = tri0.Patch;
        tri1.V[0] = v0;
        tri1.V[1] = v01;
        tri1.V[2] = v20;

        Triangle& tri2 = triangles[numTriangles++];
        tri2.Patch = tri0.Patch;
        tri2.V[0] = v01;
        tri2.V[1] = v1;
        tri2.V[2] = v12;

        Triangle& tri3 = triangles[numTriangles++];
        tri3.Patch = tri0.Patch;
        tri3.V[0] = v20;
        tri3.V[1] = v12;
        tri3.V[2] = v2;
    }

    numEdges = (int)edgeMap.size();
}
//----------------------------------------------------------------------------
void SurfaceMesh::InsertInto (EdgeMap& edgeMap, SurfacePatch* patch, int v0,
    int v1, const Float2& param0, const Float2& param1, int newReferences)
{
    EdgeKey key(v0, v1);
    EdgeMap::iterator iter = edgeMap.find(key);
    if (iter == edgeMap.end())
    {
        Edge edge(v0, v1);
        edge.Patch = patch;
        edge.Param[0] = param0;
        edge.Param[1] = param1;
        edge.References = newReferences;
        edgeMap.insert(std::make_pair(key, edge));
    }
    else
    {
        iter->second.References += newReferences;
    }
}
//----------------------------------------------------------------------------
void SurfaceMesh::OnDynamicChange ()
{
    if (mAllowDynamicChange)
    {
        VertexBufferAccessor vba(mVFormat, mVBuffer);

        for (int i = 0; i < mNumFullVertices; ++i)
        {
            SurfaceInfo& si = mSInfo[i];
            vba.Position<Float3>(i) = si.Patch->P(si.Param[0], si.Param[1]);
        }

        UpdateModelSpace(Visual::GU_NORMALS);
    }
}
//----------------------------------------------------------------------------
void SurfaceMesh::Lock ()
{
    if (mOrigVBuffer)
    {
        mOrigVBuffer = 0;
        mOrigIBuffer = 0;
        mOrigParams = 0;
        delete1(mPatches);
        mPatches = 0;
    }

    // The array mSInfo is not deleted.  This allows dynamic updates of
    // the vertices of the mesh, even though the mesh is "locked".
}
//----------------------------------------------------------------------------
bool SurfaceMesh::IsLocked () const
{
    return mOrigVBuffer == 0;
}
//----------------------------------------------------------------------------
void SurfaceMesh::InitializeSurfaceInfo ()
{
    // Because vertices are shared by triangles, the last visited triangle
    // for a vertex donates its patch and parameter values to that vertex.
    int* indices = (int*)mOrigIBuffer->GetData();
    const Float2* params = mOrigParams->GetData();
    for (int i = 0; i < mNumPatches; ++i)
    {
        int v0 = *indices++;
        int v1 = *indices++;
        int v2 = *indices++;
        mSInfo[v0].Patch = mPatches[i];
        mSInfo[v0].Param = *params++;
        mSInfo[v1].Patch = mPatches[i];
        mSInfo[v1].Param = *params++;
        mSInfo[v2].Patch = mPatches[i];
        mSInfo[v2].Param = *params++;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// SurfaceMesh::Edge
//----------------------------------------------------------------------------
SurfaceMesh::Edge::Edge (int v0, int v1)
    :
    Patch(0),
    VMid(-1),
    ParamMid(0.0f, 0.0f),
    References(0)
{
    V[0] = v0;
    V[1] = v1;
    Param[0] = Float2(0.0f, 0.0f);
    Param[1] = Float2(0.0f, 0.0f);
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Edge::operator< (const Edge& edge) const
{
    return EdgeKey(V[0], V[1]) < EdgeKey(edge.V[0], edge.V[1]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// SurfaceMesh::Triangle
//----------------------------------------------------------------------------
SurfaceMesh::Triangle::Triangle ()
    :
    Patch(0)
{
    for (int i = 0; i < 3; ++i)
    {
        V[i] = -1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// SurfaceMesh::SurfaceInfo
//----------------------------------------------------------------------------
SurfaceMesh::SurfaceInfo::SurfaceInfo ()
    :
    Patch(0),
    Param(0.0f, 0.0f)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* SurfaceMesh::GetObjectByName (const std::string& name)
{
    Object* found = TriMesh::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mOrigVBuffer, name, found);
    WM5_GET_OBJECT_BY_NAME(mOrigIBuffer, name, found);
    WM5_GET_OBJECT_BY_NAME(mOrigParams, name, found);
    if (mPatches)
    {
        for (int i = 0; i < mNumPatches; ++i)
        {
            WM5_GET_OBJECT_BY_NAME(mPatches[i], name, found);
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void SurfaceMesh::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    TriMesh::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mOrigVBuffer, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mOrigIBuffer, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mOrigParams, name, objects);
    if (mPatches)
    {
        for (int i = 0; i < mNumPatches; ++i)
        {
            WM5_GET_ALL_OBJECTS_BY_NAME(mPatches[i], name, objects);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh (LoadConstructor value)
    :
    TriMesh(value),
    mPatches(0),
    mNumFullVertices(0),
    mNumPatches(0),
    mLevel(0),
    mAllowDynamicChange(false),
    mSInfo(0)
{
}
//----------------------------------------------------------------------------
void SurfaceMesh::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TriMesh::Load(source);

    source.Read(mNumFullVertices);
    source.Read(mNumPatches);
    source.Read(mLevel);
    source.ReadBool(mAllowDynamicChange);
    source.ReadPointer(mOrigVBuffer);
    source.ReadPointer(mOrigIBuffer);
    source.ReadPointer(mOrigParams);

    bool locked = (mOrigVBuffer == 0);
    if (!locked)
    {
        source.ReadPointerVR(mNumPatches, mPatches);
    }

    if (mAllowDynamicChange)
    {
        mSInfo = new1<SurfaceInfo>(mNumFullVertices);
        for (int i = 0; i < mNumFullVertices; ++i)
        {
            source.ReadPointer(mSInfo[i].Patch);
            source.Read(mSInfo[i].Param);
        }
    }

    WM5_END_DEBUG_STREAM_LOAD(SurfaceMesh, source);
}
//----------------------------------------------------------------------------
void SurfaceMesh::Link (InStream& source)
{
    TriMesh::Link(source);

    source.ResolveLink(mOrigVBuffer);
    source.ResolveLink(mOrigIBuffer);
    source.ResolveLink(mOrigParams);
    source.ResolveLink(mNumPatches, mPatches);
    if (mSInfo)
    {
        for (int i = 0; i < mNumFullVertices; ++i)
        {
            source.ResolveLink(mSInfo[i].Patch);
        }
    }
}
//----------------------------------------------------------------------------
void SurfaceMesh::PostLink ()
{
    TriMesh::PostLink();
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Register (OutStream& target) const
{
    if (TriMesh::Register(target))
    {
        target.Register(mOrigVBuffer);
        target.Register(mOrigIBuffer);
        target.Register(mOrigParams);
        target.Register(mNumPatches, mPatches);
        if (mSInfo)
        {
            for (int i = 0; i < mNumFullVertices; ++i)
            {
                target.Register(mSInfo[i].Patch);
            }
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void SurfaceMesh::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TriMesh::Save(target);

    target.Write(mNumFullVertices);
    target.Write(mNumPatches);
    target.Write(mLevel);
    target.WriteBool(mAllowDynamicChange);
    target.WritePointer(mOrigVBuffer);
    target.WritePointer(mOrigIBuffer);
    target.WritePointer(mOrigParams);
    target.WritePointerN(mNumPatches, mPatches);

    if (mSInfo)
    {
        for (int i = 0; i < mNumFullVertices; i++)
        {
            target.WritePointer(mSInfo[i].Patch);
            target.Write(mSInfo[i].Param);
        }
    }

    WM5_END_DEBUG_STREAM_SAVE(SurfaceMesh, target);
}
//----------------------------------------------------------------------------
int SurfaceMesh::GetStreamingSize () const
{
    int size = TriMesh::GetStreamingSize();
    size += sizeof(mNumFullVertices);
    size += sizeof(mNumPatches);
    size += sizeof(mLevel);
    size += WM5_BOOLSIZE(mAllowDynamicChange);
    size += WM5_POINTERSIZE(mOrigVBuffer);
    size += WM5_POINTERSIZE(mOrigIBuffer);
    size += WM5_POINTERSIZE(mOrigParams);

    if (mPatches)
    {
        size += mNumPatches*WM5_POINTERSIZE(mPatches[0]);
    }

    if (mSInfo)
    {
        size += mNumFullVertices*WM5_POINTERSIZE(mSInfo[0].Patch);
        size += mNumFullVertices*sizeof(mSInfo[0].Param);
    }

    return size;
}
//----------------------------------------------------------------------------
