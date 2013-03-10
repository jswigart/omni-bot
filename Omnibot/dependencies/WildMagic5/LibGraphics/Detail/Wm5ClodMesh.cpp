// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ClodMesh.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TriMesh, ClodMesh);
WM5_IMPLEMENT_STREAM(ClodMesh);
WM5_IMPLEMENT_FACTORY(ClodMesh);

//----------------------------------------------------------------------------
ClodMesh::ClodMesh (TriMesh* mesh, CollapseRecordArray* recordArray)
    :
    TriMesh(mesh->GetVertexFormat(), mesh->GetVertexBuffer(), 0),
    mCurrentRecord(0),
    mTargetRecord(0),
    mRecordArray(recordArray)
{
    assertion(recordArray != 0, "Record array is needed for construction.\n");

    // Make a copy of the indices.
    IndexBuffer* ibuffer = mesh->GetIndexBuffer();
    int numIndices = ibuffer->GetNumElements();
    int elementSize = ibuffer->GetElementSize();
    assertion(elementSize == 4, "Invalid indices.\n");

    char* srcIndices = ibuffer->GetData();
    mIBuffer = new0 IndexBuffer(numIndices, elementSize);
    char* trgIndices = mIBuffer->GetData();
    memcpy(trgIndices, srcIndices, numIndices*elementSize);
}
//----------------------------------------------------------------------------
ClodMesh::~ClodMesh ()
{
}
//----------------------------------------------------------------------------
void ClodMesh::SelectLevelOfDetail ()
{
    const CollapseRecord* records = mRecordArray->GetRecords();

    // Get the target record.  The virtual function may be overridden by a
    // derived class to obtain a desired automated change in the target.
    int targetRecord = GetAutomatedTargetRecord();

    // Collapse the mesh, if necessary.
    int* indices = (int*)mIBuffer->GetData();
    bool ibufferChanged = (mCurrentRecord != targetRecord);

    int i, c;
    while (mCurrentRecord < targetRecord)
    {
        ++mCurrentRecord;

        // Replace indices in the connectivity array.
        const CollapseRecord& record = records[mCurrentRecord];
        for (i = 0; i < record.NumIndices; ++i)
        {
            c = record.Indices[i];
            assertion(indices[c] == record.VThrow,
                "Inconsistent record in SelectLevelOfDetail.\n");

            indices[c] = record.VKeep;
        }

        // Reduce the vertex count; the vertices are properly ordered.
        mVBuffer->SetNumElements(record.NumVertices);

        // Reduce the triangle count; the triangles are properly ordered.
        mIBuffer->SetNumElements(3*record.NumTriangles);
    }

    // Expand the mesh, if necessary.
    while (mCurrentRecord > targetRecord)
    {
        // Restore indices in the connectivity array.
        const CollapseRecord& record = records[mCurrentRecord];
        for (i = 0; i < record.NumIndices; ++i)
        {
            c = record.Indices[i];
            assertion(indices[c] == record.VKeep,
                "Inconsistent record in SelectLevelOfDetail.\n");

            indices[c] = record.VThrow;
        }

        --mCurrentRecord;
        const CollapseRecord& prevRecord = records[mCurrentRecord];

        // Increase the vertex count; the vertices are properly ordered.
        mVBuffer->SetNumElements(prevRecord.NumVertices);

        // Increase the triangle count; the triangles are properly ordered.
        mIBuffer->SetNumElements(3*prevRecord.NumTriangles);
    }

    if (ibufferChanged)
    {
        Renderer::UpdateAll(mIBuffer);
    }
}
//----------------------------------------------------------------------------
void ClodMesh::GetVisibleSet (Culler& culler, bool noCull)
{
    SelectLevelOfDetail();
    TriMesh::GetVisibleSet(culler, noCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* ClodMesh::GetObjectByName (const std::string& name)
{
    Object* found = TriMesh::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mRecordArray, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void ClodMesh::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    TriMesh::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mRecordArray, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
ClodMesh::ClodMesh (LoadConstructor value)
    :
    TriMesh(value),
    mCurrentRecord(0),
    mTargetRecord(0)
{
}
//----------------------------------------------------------------------------
void ClodMesh::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TriMesh::Load(source);

    source.Read(mCurrentRecord);
    source.Read(mTargetRecord);
    source.ReadPointer(mRecordArray);

    WM5_END_DEBUG_STREAM_LOAD(ClodMesh, source);
}
//----------------------------------------------------------------------------
void ClodMesh::Link (InStream& source)
{
    TriMesh::Link(source);

    source.ResolveLink(mRecordArray);
}
//----------------------------------------------------------------------------
void ClodMesh::PostLink ()
{
    TriMesh::PostLink();
}
//----------------------------------------------------------------------------
bool ClodMesh::Register (OutStream& target) const
{
    if (TriMesh::Register(target))
    {
        target.Register(mRecordArray);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void ClodMesh::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TriMesh::Save(target);

    target.Write(mCurrentRecord);
    target.Write(mTargetRecord);
    target.WritePointer(mRecordArray);

    WM5_END_DEBUG_STREAM_SAVE(ClodMesh, target);
}
//----------------------------------------------------------------------------
int ClodMesh::GetStreamingSize () const
{
    int size = TriMesh::GetStreamingSize();
    size += sizeof(mCurrentRecord);
    size += sizeof(mTargetRecord);
    size += WM5_POINTERSIZE(mRecordArray);
    return size;
}
//----------------------------------------------------------------------------
