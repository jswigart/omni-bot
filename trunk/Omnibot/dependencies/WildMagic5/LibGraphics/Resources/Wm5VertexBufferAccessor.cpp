// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VertexBufferAccessor.h"
#include "Wm5Visual.h"
using namespace Wm5;

//----------------------------------------------------------------------------
VertexBufferAccessor::VertexBufferAccessor ()
    :
    mVFormat(0),
    mVBuffer(0),
    mStride(0),
    mData(0),
    mPosition(0),
    mNormal(0),
    mBinormal(0),
    mBlendIndices(0),
    mBlendWeight(0),
    mPositionChannels(0),
    mNormalChannels(0),
    mTangentChannels(0),
    mBinormalChannels(0)
{
    int i;
    for (i = 0; i < VertexFormat::AM_MAX_COLOR_UNITS; ++i)
    {
        mTCoord[i] = 0;
        mTCoordChannels[i] = 0;
    }
    for (i = 0; i < VertexFormat::AM_MAX_COLOR_UNITS; ++i)
    {
        mColor[i] = 0;
        mColorChannels[i] = 0;
    }
}
//----------------------------------------------------------------------------
VertexBufferAccessor::VertexBufferAccessor (VertexFormat* vformat,
    VertexBuffer* vbuffer)
{
    ApplyTo(vformat, vbuffer);
}
//----------------------------------------------------------------------------
VertexBufferAccessor::VertexBufferAccessor (Visual* visual)
{
    ApplyTo(visual);
}
//----------------------------------------------------------------------------
VertexBufferAccessor::~VertexBufferAccessor ()
{
}
//----------------------------------------------------------------------------
void VertexBufferAccessor::ApplyTo (VertexFormat* vformat,
    VertexBuffer* vbuffer)
{
    mVFormat = vformat;
    mVBuffer = vbuffer;
    Initialize();
}
//----------------------------------------------------------------------------
void VertexBufferAccessor::ApplyTo (Visual* visual)
{
    mVFormat = visual->GetVertexFormat();
    mVBuffer = visual->GetVertexBuffer();
    Initialize();
}
//----------------------------------------------------------------------------
void VertexBufferAccessor::Initialize ()
{
    mStride = mVFormat->GetStride();
    mData = mVBuffer->GetData();

    int baseType = (int)VertexFormat::AU_NONE;
    int type;

    int index = mVFormat->GetIndex(VertexFormat::AU_POSITION);
    if (index >= 0)
    {
        mPosition = mData + mVFormat->GetOffset(index);
        type = (int)mVFormat->GetAttributeType(index);
        mPositionChannels = type - baseType;
        if (mPositionChannels > 4)
        {
            mPositionChannels = 0;
        }
    }
    else
    {
        mPosition = 0;
        mPositionChannels = 0;
    }

    index = mVFormat->GetIndex(VertexFormat::AU_NORMAL);
    if (index >= 0)
    {
        mNormal = mData + mVFormat->GetOffset(index);
        type = (int)mVFormat->GetAttributeType(index);
        mNormalChannels = type - baseType;
        if (mNormalChannels > 4)
        {
            mNormalChannels = 0;
        }
    }
    else
    {
        mNormal = 0;
        mNormalChannels = 0;
    }

    index = mVFormat->GetIndex(VertexFormat::AU_TANGENT);
    if (index >= 0)
    {
        mTangent = mData + mVFormat->GetOffset(index);
        type = (int)mVFormat->GetAttributeType(index);
        mTangentChannels = type - baseType;
        if (mTangentChannels > 4)
        {
            mTangentChannels = 0;
        }
    }
    else
    {
        mTangent = 0;
        mTangentChannels = 0;
    }

    index = mVFormat->GetIndex(VertexFormat::AU_BINORMAL);
    if (index >= 0)
    {
        mBinormal = mData + mVFormat->GetOffset(index);
        type = (int)mVFormat->GetAttributeType(index);
        mBinormalChannels = type - baseType;
        if (mBinormalChannels > 4)
        {
            mBinormalChannels = 0;
        }
    }
    else
    {
        mBinormal = 0;
        mBinormalChannels = 0;
    }

    unsigned int unit;
    for (unit = 0; unit < VertexFormat::AM_MAX_TCOORD_UNITS; ++unit)
    {
        index = mVFormat->GetIndex(VertexFormat::AU_TEXCOORD, unit);
        if (index >= 0)
        {
            mTCoord[unit] = mData + mVFormat->GetOffset(index);
            type = (int)mVFormat->GetAttributeType(index);
            mTCoordChannels[unit] = type - baseType;
            if (mTCoordChannels[unit] > 4)
            {
                mTCoordChannels[unit] = 0;
            }
        }
        else
        {
            mTCoord[unit] = 0;
            mTCoordChannels[unit] = 0;
        }
    }

    for (unit = 0; unit < VertexFormat::AM_MAX_COLOR_UNITS; ++unit)
    {
        index = mVFormat->GetIndex(VertexFormat::AU_COLOR, unit);
        if (index >= 0)
        {
            mColor[unit] = mData + mVFormat->GetOffset(index);
            type = (int)mVFormat->GetAttributeType(index);
            mColorChannels[unit] = type - baseType;
            if (mColorChannels[unit] > 4)
            {
                mColorChannels[unit] = 0;
            }
        }
        else
        {
            mColor[unit] = 0;
            mColorChannels[unit] = 0;
        }
    }

    index = mVFormat->GetIndex(VertexFormat::AU_BLENDINDICES);
    if (index >= 0)
    {
        mBlendIndices = mData + mVFormat->GetOffset(index);
    }
    else
    {
        mBlendIndices = 0;
    }

    index = mVFormat->GetIndex(VertexFormat::AU_BLENDWEIGHT);
    if (index >= 0)
    {
        mBlendWeight = mData + mVFormat->GetOffset(index);
    }
    else
    {
        mBlendWeight = 0;
    }
}
//----------------------------------------------------------------------------
void VertexBufferAccessor::Read (FileIO& inFile)
{
    assertion(mStride == mVBuffer->GetElementSize(),
        "Format stride and vertex size must match (for now).\n");

    // Build a table for the attributes of the vertex buffer.  Each attribute
    // has an offset, a size for a component of the attribute, and the number
    // of components of that size.  For example, an attribute with offset 0
    // and type AT_FLOAT3 has (offset,size,numComponents) = (0,4,3).
    Tuple<3, unsigned int> table[VertexFormat::AM_MAX_ATTRIBUTES];
    const int numAttributes = mVFormat->GetNumAttributes();
    unsigned int streamIndex, offset, index;
    VertexFormat::AttributeType type;
    VertexFormat::AttributeUsage usage;
    int j;
    for (j = 0; j < numAttributes; ++j)
    {
        mVFormat->GetAttribute(j, streamIndex, offset, type, usage, index);
        table[j][0] = offset;
        table[j][1] = VertexFormat::GetComponentSize(type);
        table[j][2] = VertexFormat::GetNumComponents(type);
    }

    // Read vertices one at a time to allow for byte swapping (endianness).
    const int numElements = mVBuffer->GetNumElements();
    char* vertex = mData;
    for (int i = 0; i < numElements; ++i, vertex += mStride)
    {
        for (j = 0; j < numAttributes; ++j)
        {
            inFile.Read(table[j][1], table[j][2], vertex + table[j][0]);
        }
    }
}
//----------------------------------------------------------------------------
void VertexBufferAccessor::Write (FileIO& outFile)
{
    assertion(mStride == mVBuffer->GetElementSize(),
        "Format stride and vertex size must match (for now).\n");

    // Build a table for the attributes of the vertex buffer.  Each attribute
    // has an offset, a size for a component of the attribute, and the number
    // of components of that size.  For example, an attribute with offset 0
    // and type AT_FLOAT3 has (offset,size,numComponents) = (0,4,3).
    Tuple<3, unsigned int> table[VertexFormat::AM_MAX_ATTRIBUTES];
    const int numAttributes = mVFormat->GetNumAttributes();
    unsigned int streamIndex, offset, index;
    VertexFormat::AttributeType type;
    VertexFormat::AttributeUsage usage;
    int j;
    for (j = 0; j < numAttributes; ++j)
    {
        mVFormat->GetAttribute(j, streamIndex, offset, type, usage, index);
        table[j][0] = offset;
        table[j][1] = VertexFormat::GetComponentSize(type);
        table[j][2] = VertexFormat::GetNumComponents(type);
    }

    // Write vertices one at a time to allow for byte swapping (endianness).
    const int numElements = mVBuffer->GetNumElements();
    char* vertex = mData;
    for (int i = 0; i < numElements; ++i, vertex += mStride)
    {
        for (j = 0; j < numAttributes; ++j)
        {
            outFile.Write(table[j][1], table[j][2], vertex + table[j][0]);
        }
    }
}
//----------------------------------------------------------------------------
