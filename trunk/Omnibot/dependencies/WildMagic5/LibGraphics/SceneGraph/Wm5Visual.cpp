// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Visual.h"
#include "Wm5Culler.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Spatial, Visual);
WM5_IMPLEMENT_STREAM(Visual);
WM5_IMPLEMENT_ABSTRACT_FACTORY(Visual);

//----------------------------------------------------------------------------
Visual::Visual (PrimitiveType type)
    :
    mType(type),
    mVFormat(0),
    mVBuffer(0),
    mIBuffer(0),
    mEffect(0)
{
}
//----------------------------------------------------------------------------
Visual::Visual (PrimitiveType type, VertexFormat* vformat,
    VertexBuffer* vbuffer, IndexBuffer* ibuffer)
    :
    mType(type),
    mVFormat(vformat),
    mVBuffer(vbuffer),
    mIBuffer(ibuffer),
    mEffect(0)
{
    UpdateModelSpace(GU_MODEL_BOUND_ONLY);
}
//----------------------------------------------------------------------------
Visual::~Visual ()
{
}
//----------------------------------------------------------------------------
void Visual::UpdateModelSpace (UpdateType)
{
    UpdateModelBound();
}
//----------------------------------------------------------------------------
void Visual::UpdateWorldBound ()
{
    mModelBound.TransformBy(WorldTransform, WorldBound);
}
//----------------------------------------------------------------------------
void Visual::UpdateModelBound ()
{
    int numVertices = mVBuffer->GetNumElements();
    int stride = mVFormat->GetStride();

    int posIndex = mVFormat->GetIndex(VertexFormat::AU_POSITION);
    if (posIndex == -1)
    {
        assertion(false, "Update requires vertex positions\n");
        return;
    }

    VertexFormat::AttributeType posType =
        mVFormat->GetAttributeType(posIndex);
    if (posType != VertexFormat::AT_FLOAT3
    &&  posType != VertexFormat::AT_FLOAT4)
    {
        assertion(false, "Positions must be 3-tuples or 4-tuples\n");
        return;
    }

    char* data = mVBuffer->GetData();
    int posOffset = mVFormat->GetOffset(posIndex);
    mModelBound.ComputeFromData(numVertices, stride, data + posOffset);
}
//----------------------------------------------------------------------------
void Visual::GetVisibleSet (Culler& culler, bool)
{
    culler.Insert(this);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for load/save of raw data.
//----------------------------------------------------------------------------
void Visual::LoadWMVF (const std::string& name, PrimitiveType& type,
    VertexFormat*& vformat, VertexBuffer*& vbuffer, IndexBuffer*& ibuffer,
    int mode)
{
    FileIO inFile(name, mode);
    if (!inFile)
    {
        assertion(false, "Failed to open file %s\n", name.c_str());
        return;
    }

    int iType;
    inFile.Read(sizeof(int), &iType);
    type = (PrimitiveType)iType;

    vformat = LoadVertexFormat(inFile);
    vbuffer = LoadVertexBuffer(inFile, vformat);
    ibuffer = LoadIndexBuffer(inFile);

    inFile.Close();
}
//----------------------------------------------------------------------------
void Visual::SaveWMVF (const std::string& name, int mode)
{
    FileIO outFile(name, mode);
    if (!outFile)
    {
        assertion(false, "Failed to open file %s\n", name.c_str());
        return;
    }

    int type = (int)mType;
    outFile.Write(sizeof(int), &type);

    SaveVertexFormat(outFile);
    SaveVertexBuffer(outFile);
    SaveIndexBuffer(outFile);

    outFile.Close();
}
//----------------------------------------------------------------------------
VertexFormat* Visual::LoadVertexFormat (FileIO& inFile)
{
    int numAttributes;
    inFile.Read(sizeof(int), &numAttributes);

    VertexFormat* vformat = new0 VertexFormat(numAttributes);
    for (int i = 0; i < numAttributes; ++i)
    {
        unsigned int streamIndex, offset, usageIndex;
        int type, usage;

        inFile.Read(sizeof(unsigned int), &streamIndex);
        inFile.Read(sizeof(unsigned int), &offset);
        inFile.Read(sizeof(int), &type);
        inFile.Read(sizeof(int), &usage);
        inFile.Read(sizeof(unsigned int), &usageIndex);

        vformat->SetAttribute(i, streamIndex, offset,
            (VertexFormat::AttributeType)type,
            (VertexFormat::AttributeUsage)usage, usageIndex);
    }

    int stride;
    inFile.Read(sizeof(int), &stride);
    vformat->SetStride(stride);

    return vformat;
}
//----------------------------------------------------------------------------
VertexBuffer* Visual::LoadVertexBuffer (FileIO& inFile, VertexFormat* vformat)
{
    int numElements, elementSize, usage;
    inFile.Read(sizeof(int), &numElements);
    inFile.Read(sizeof(int), &elementSize);
    inFile.Read(sizeof(int), &usage);

    VertexBuffer* vbuffer = new0 VertexBuffer(numElements, elementSize,
        (Buffer::Usage)usage);

    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Read(inFile);

    return vbuffer;
}
//----------------------------------------------------------------------------
IndexBuffer* Visual::LoadIndexBuffer (FileIO& inFile)
{
    int numElements;
    inFile.Read(sizeof(int), &numElements);

    if (numElements > 0)
    {
        int elementSize, usage, offset;
        inFile.Read(sizeof(int), &elementSize);
        inFile.Read(sizeof(int), &usage);
        inFile.Read(sizeof(int), &offset);

        IndexBuffer* ibuffer = new0 IndexBuffer(numElements, elementSize,
            (Buffer::Usage)usage);
        ibuffer->SetOffset(offset);

        inFile.Read(elementSize, ibuffer->GetNumBytes()/elementSize,
            ibuffer->GetData());

        return ibuffer;
    }

    return 0;
}
//----------------------------------------------------------------------------
void Visual::SaveVertexFormat (FileIO& outFile)
{
    int numAttributes = mVFormat->GetNumAttributes();
    outFile.Write(sizeof(int), &numAttributes);

    for (int i = 0; i < numAttributes; ++i)
    {
        unsigned int streamIndex, offset, usageIndex;
        VertexFormat::AttributeType vftype;
        VertexFormat::AttributeUsage vfusage;
        mVFormat->GetAttribute(i, streamIndex, offset, vftype, vfusage,
            usageIndex);
        int type = (int)vftype;
        int usage = (int)vfusage;

        outFile.Write(sizeof(unsigned int), &streamIndex);
        outFile.Write(sizeof(unsigned int), &offset);
        outFile.Write(sizeof(int), &type);
        outFile.Write(sizeof(int), &usage);
        outFile.Write(sizeof(unsigned int), &usageIndex);
    }

    int stride = mVFormat->GetStride();
    outFile.Write(sizeof(int), &stride);
}
//----------------------------------------------------------------------------
void Visual::SaveVertexBuffer (FileIO& outFile)
{
    int numElements = mVBuffer->GetNumElements();
    int elementSize = mVBuffer->GetElementSize();
    Buffer::Usage vbusage = mVBuffer->GetUsage();
    int usage = (int)vbusage;
    outFile.Write(sizeof(int), &numElements);
    outFile.Write(sizeof(int), &elementSize);
    outFile.Write(sizeof(int), &usage);

    VertexBufferAccessor vba(mVFormat, mVBuffer);
    vba.Write(outFile);
}
//----------------------------------------------------------------------------
void Visual::SaveIndexBuffer (FileIO& outFile)
{
    if (mIBuffer)
    {
        int numElements = mIBuffer->GetNumElements();
        int elementSize = mIBuffer->GetElementSize();
        Buffer::Usage ibusage = mIBuffer->GetUsage();
        int usage = (int)ibusage;
        int offset = mIBuffer->GetOffset();
        outFile.Write(sizeof(int), &numElements);
        outFile.Write(sizeof(int), &elementSize);
        outFile.Write(sizeof(int), &usage);
        outFile.Write(sizeof(int), &offset);

        outFile.Write(elementSize, mIBuffer->GetNumBytes()/elementSize,
            mIBuffer->GetData());
    }
    else
    {
        int numElements = 0;
        outFile.Write(sizeof(int), &numElements);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* Visual::GetObjectByName (const std::string& name)
{
    Object* found = Spatial::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mVFormat, name, found);
    WM5_GET_OBJECT_BY_NAME(mVBuffer, name, found);
    WM5_GET_OBJECT_BY_NAME(mIBuffer, name, found);
    WM5_GET_OBJECT_BY_NAME(mEffect, name, found);

    return 0;
}
//----------------------------------------------------------------------------
void Visual::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Spatial::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mVFormat, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mVBuffer, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mIBuffer, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mEffect, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Visual::Visual (LoadConstructor value)
    :
    Spatial(value),
    mType(PT_NONE),
    mVFormat(0),
    mVBuffer(0),
    mIBuffer(0),
    mEffect(0)
{
}
//----------------------------------------------------------------------------
void Visual::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Spatial::Load(source);

    source.ReadEnum(mType);
    source.ReadAggregate(mModelBound);
    source.ReadPointer(mVFormat);
    source.ReadPointer(mVBuffer);
    source.ReadPointer(mIBuffer);
    source.ReadPointer(mEffect);

    WM5_END_DEBUG_STREAM_LOAD(Visual, source);
}
//----------------------------------------------------------------------------
void Visual::Link (InStream& source)
{
    Spatial::Link(source);

    source.ResolveLink(mVFormat);
    source.ResolveLink(mVBuffer);
    source.ResolveLink(mIBuffer);
    source.ResolveLink(mEffect);
}
//----------------------------------------------------------------------------
void Visual::PostLink ()
{
    Spatial::PostLink();
}
//----------------------------------------------------------------------------
bool Visual::Register (OutStream& target) const
{
    if (Spatial::Register(target))
    {
        target.Register(mVFormat);
        target.Register(mVBuffer);
        target.Register(mIBuffer);
        target.Register(mEffect);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Visual::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Spatial::Save(target);

    target.WriteEnum(mType);
    target.WriteAggregate(mModelBound);
    target.WritePointer(mVFormat);
    target.WritePointer(mVBuffer);
    target.WritePointer(mIBuffer);
    target.WritePointer(mEffect);

    WM5_END_DEBUG_STREAM_SAVE(Visual, target);
}
//----------------------------------------------------------------------------
int Visual::GetStreamingSize () const
{
    int size = Spatial::GetStreamingSize();
    size += WM5_ENUMSIZE(mType);
    size += sizeof(mModelBound);
    size += WM5_POINTERSIZE(mVFormat);
    size += WM5_POINTERSIZE(mVBuffer);
    size += WM5_POINTERSIZE(mIBuffer);
    size += WM5_POINTERSIZE(mEffect);
    return size;
}
//----------------------------------------------------------------------------
