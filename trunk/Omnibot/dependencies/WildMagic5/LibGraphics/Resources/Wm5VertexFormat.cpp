// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VertexFormat.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, VertexFormat);
WM5_IMPLEMENT_STREAM(VertexFormat);
WM5_IMPLEMENT_FACTORY(VertexFormat);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, VertexFormat);

int VertexFormat::msComponentSize[AT_QUANTITY] =
{
    0,  // AT_NONE
    4,  // AT_FLOAT1
    4,  // AT_FLOAT2
    4,  // AT_FLOAT3
    4,  // AT_FLOAT4
    2,  // AT_HALF1
    2,  // AT_HALF2
    2,  // AT_HALF3
    2,  // AT_HALF4
    1,  // AT_UBYTE4
    2,  // AT_SHORT1
    2,  // AT_SHORT2
    2   // AT_SHORT4
};

int VertexFormat::msNumComponents[AT_QUANTITY] =
{
    0,  // AT_NONE
    1,  // AT_FLOAT1
    2,  // AT_FLOAT2
    3,  // AT_FLOAT3
    4,  // AT_FLOAT4
    1,  // AT_HALF1
    2,  // AT_HALF2
    3,  // AT_HALF3
    4,  // AT_HALF4
    4,  // AT_UBYTE4
    1,  // AT_SHORT1
    2,  // AT_SHORT2
    4   // AT_SHORT4
};

int VertexFormat::msTypeSize[AT_QUANTITY] =
{
    0,  // AT_NONE
    4,  // AT_FLOAT1
    8,  // AT_FLOAT2
    12, // AT_FLOAT3
    16, // AT_FLOAT4
    2,  // AT_HALF1
    4,  // AT_HALF2
    6,  // AT_HALF3
    8,  // AT_HALF4
    4,  // AT_UBYTE4
    2,  // AT_SHORT1
    4,  // AT_SHORT2
    8   // AT_SHORT4
};

//----------------------------------------------------------------------------
VertexFormat::VertexFormat (int numAttributes)
    :
    mNumAttributes(numAttributes),
    mStride(0)
{
    assertion(mNumAttributes > 0, "Number of attributes must be positive\n");

    for (int i = 0; i < AM_MAX_ATTRIBUTES; ++i)
    {
        mElements[i].StreamIndex = 0;
        mElements[i].Offset = 0;
        mElements[i].Type = AT_NONE;
        mElements[i].Usage = AU_NONE;
        mElements[i].UsageIndex = 0;
    }
}
//----------------------------------------------------------------------------
VertexFormat::~VertexFormat ()
{
    Renderer::UnbindAll(this);
}
//----------------------------------------------------------------------------
VertexFormat* VertexFormat::Create (int numAttributes, ...)
{
    VertexFormat* vformat = new0 VertexFormat(numAttributes);

    va_list arguments;
    va_start(arguments, numAttributes);

    unsigned int offset = 0;
    for (int i = 0; i < numAttributes; ++i)
    {
        int usage = va_arg(arguments, int);
        int type = va_arg(arguments, int);
        unsigned int usageIndex = va_arg(arguments, unsigned int);
        vformat->SetAttribute(i, 0, offset, (AttributeType)type,
            (AttributeUsage)usage, usageIndex);
        offset += msTypeSize[type];
    }
    vformat->SetStride((int)offset);

    va_end(arguments);

    return vformat;
}
//----------------------------------------------------------------------------
void VertexFormat::SetAttribute (int attribute, unsigned int streamIndex,
    unsigned int offset, AttributeType type, AttributeUsage usage,
    unsigned int usageIndex)
{
    assertion(0 <= attribute && attribute < mNumAttributes,
        "Invalid index in SetAttribute\n");

#ifdef _DEBUG
    if (attribute > 0)
    {
        assertion(offset > mElements[attribute-1].Offset,
            "Offsets must be increasing with attribute index.\n");
    }
    else
    {
        assertion(offset == 0,
            "The offset of the first attribute must be zero.\n");
    }
#endif

    Element& element = mElements[attribute];
    element.StreamIndex = streamIndex;
    element.Offset = offset;
    element.Type = type;
    element.Usage = usage;
    element.UsageIndex = usageIndex;
}
//----------------------------------------------------------------------------
void VertexFormat::SetStride (int stride)
{
    assertion(stride > 0, "Stride must be positive\n");
    mStride = stride;
}
//----------------------------------------------------------------------------
void VertexFormat::GetAttribute (int attribute, unsigned int& streamIndex,
    unsigned int& offset, AttributeType& type, AttributeUsage& usage,
    unsigned int& usageIndex) const
{
    assertion(0 <= attribute && attribute < mNumAttributes,
        "Invalid index in GetAttribute\n");

    const Element& element = mElements[attribute];
    streamIndex = element.StreamIndex;
    offset = element.Offset;
    type = element.Type;
    usage = element.Usage;
    usageIndex = element.UsageIndex;
}
//----------------------------------------------------------------------------
int VertexFormat::GetIndex (AttributeUsage usage, unsigned int usageIndex)
    const
{
    for (int i = 0; i < mNumAttributes; ++i)
    {
        if (mElements[i].Usage == usage
        &&  mElements[i].UsageIndex == usageIndex)
        {
            return i;
        }
    }

    return -1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
VertexFormat::VertexFormat (LoadConstructor value)
    :
    Object(value),
    mNumAttributes(0),
    mStride(0)
{
    for (int i = 0; i < AM_MAX_ATTRIBUTES; ++i)
    {
        mElements[i].StreamIndex = 0;
        mElements[i].Offset = 0;
        mElements[i].Type = AT_NONE;
        mElements[i].Usage = AU_NONE;
        mElements[i].UsageIndex = 0;
    }
}
//----------------------------------------------------------------------------
void VertexFormat::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.Read(mNumAttributes);

    for (int i = 0; i < AM_MAX_ATTRIBUTES; ++i)
    {
        source.Read(mElements[i].StreamIndex);
        source.Read(mElements[i].Offset);
        source.ReadEnum(mElements[i].Type);
        source.ReadEnum(mElements[i].Usage);
        source.Read(mElements[i].UsageIndex);
    }

    source.Read(mStride);

    WM5_END_DEBUG_STREAM_LOAD(VertexFormat, source);
}
//----------------------------------------------------------------------------
void VertexFormat::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void VertexFormat::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool VertexFormat::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void VertexFormat::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.Write(mNumAttributes);

    for (int i = 0; i < AM_MAX_ATTRIBUTES; ++i)
    {
        target.Write(mElements[i].StreamIndex);
        target.Write(mElements[i].Offset);
        target.WriteEnum(mElements[i].Type);
        target.WriteEnum(mElements[i].Usage);
        target.Write(mElements[i].UsageIndex);
    }

    target.Write(mStride);

    WM5_END_DEBUG_STREAM_SAVE(VertexFormat, target);
}
//----------------------------------------------------------------------------
int VertexFormat::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mNumAttributes);
    for (int i = 0; i < AM_MAX_ATTRIBUTES; ++i)
    {
        size += sizeof(mElements[i].StreamIndex);
        size += sizeof(mElements[i].Offset);
        size += WM5_ENUMSIZE(mElements[i].Type);
        size += WM5_ENUMSIZE(mElements[i].Usage);
        size += sizeof(mElements[i].UsageIndex);
    }
    size += sizeof(mStride);
    return size;
}
//----------------------------------------------------------------------------
