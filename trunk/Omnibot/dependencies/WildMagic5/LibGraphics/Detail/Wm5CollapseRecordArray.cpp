// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CollapseRecordArray.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, CollapseRecordArray);
WM5_IMPLEMENT_STREAM(CollapseRecordArray);
WM5_IMPLEMENT_FACTORY(CollapseRecordArray);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, CollapseRecordArray);

//----------------------------------------------------------------------------
CollapseRecordArray::CollapseRecordArray (int numRecords,
    CollapseRecord* records)
    :
    mNumRecords(numRecords),
    mRecords(records)
{
}
//----------------------------------------------------------------------------
CollapseRecordArray::~CollapseRecordArray ()
{
    delete1(mRecords);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
CollapseRecordArray::CollapseRecordArray (LoadConstructor value)
    :
    Object(value),
    mNumRecords(0),
    mRecords(0)
{
}
//----------------------------------------------------------------------------
void CollapseRecordArray::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.Read(mNumRecords);
    mRecords = new1<CollapseRecord>(mNumRecords);

    for (int i = 0; i < mNumRecords; ++i)
    {
        source.Read(mRecords[i].VKeep);
        source.Read(mRecords[i].VThrow);
        source.Read(mRecords[i].NumVertices);
        source.Read(mRecords[i].NumTriangles);
        source.Read(mRecords[i].NumIndices);
        if (mRecords[i].NumIndices > 0)
        {
            mRecords[i].Indices = new1<int>(mRecords[i].NumIndices);
            source.ReadVR(mRecords[i].NumIndices, mRecords[i].Indices);
        }
        else
        {
            mRecords[i].Indices = 0;
        }
    }

    WM5_END_DEBUG_STREAM_LOAD(CollapseRecordArray, source);
}
//----------------------------------------------------------------------------
void CollapseRecordArray::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void CollapseRecordArray::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool CollapseRecordArray::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void CollapseRecordArray::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.Write(mNumRecords);
    for (int i = 0; i < mNumRecords; ++i)
    {
        target.Write(mRecords[i].VKeep);
        target.Write(mRecords[i].VThrow);
        target.Write(mRecords[i].NumVertices);
        target.Write(mRecords[i].NumTriangles);
        target.Write(mRecords[i].NumIndices);
        if (mRecords[i].Indices)
        {
            target.WriteN(mRecords[i].NumIndices, mRecords[i].Indices);
        }
    }

    WM5_END_DEBUG_STREAM_SAVE(CollapseRecordArray, target);
}
//----------------------------------------------------------------------------
int CollapseRecordArray::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mNumRecords);
    for (int i = 0; i < mNumRecords; ++i)
    {
        size += sizeof(mRecords[i].VKeep);
        size += sizeof(mRecords[i].VThrow);
        size += sizeof(mRecords[i].NumVertices);
        size += sizeof(mRecords[i].NumIndices);
        size += sizeof(mRecords[i].Indices);
        size += mRecords[i].NumIndices*sizeof(mRecords[i].Indices[0]);
    }
    return size;
}
//----------------------------------------------------------------------------
