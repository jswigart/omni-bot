// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5OpenGLVertexFormat.h"
#include "Wm5OpenGLMapping.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrVertexFormat::PdrVertexFormat (Renderer*, const VertexFormat* vformat)
{
    mStride = vformat->GetStride();

    VertexFormat::AttributeType type;

    int i = vformat->GetIndex(VertexFormat::AU_POSITION);
    if (i >= 0)
    {
        mHasPosition = 1;
        type = vformat->GetAttributeType(i);
        mPositionChannels = gOGLAttributeChannels[type];
        mPositionType = gOGLAttributeType[type];
        mPositionOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasPosition = 0;
        mPositionChannels = 0;
        mPositionType = 0;
        mPositionOffset = 0;
    }

    i = vformat->GetIndex(VertexFormat::AU_NORMAL);
    if (i >= 0)
    {
        mHasNormal = 1;
        type = vformat->GetAttributeType(i);
        mNormalChannels = gOGLAttributeChannels[type];
        mNormalType = gOGLAttributeType[vformat->GetAttributeType(i)];
        mNormalOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasNormal = 0;
        mNormalChannels = 0;
        mNormalType = 0;
        mNormalOffset = 0;
    }

    i = vformat->GetIndex(VertexFormat::AU_TANGENT);
    if (i >= 0)
    {
        mHasTangent = 1;
        type = vformat->GetAttributeType(i);
        mTangentChannels = gOGLAttributeChannels[type];
        mTangentType = gOGLAttributeType[vformat->GetAttributeType(i)];
        mTangentOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasTangent = 0;
        mTangentChannels = 0;
        mTangentType = 0;
        mTangentOffset = 0;
    }

    i = vformat->GetIndex(VertexFormat::AU_BINORMAL);
    if (i >= 0)
    {
        mHasBinormal = 1;
        type = vformat->GetAttributeType(i);
        mBinormalChannels = gOGLAttributeChannels[type];
        mBinormalType = gOGLAttributeType[vformat->GetAttributeType(i)];
        mBinormalOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasBinormal = 0;
        mBinormalChannels = 0;
        mBinormalType = 0;
        mBinormalOffset = 0;
    }

    unsigned int unit;
    for (unit = 0; unit < VertexFormat::AM_MAX_TCOORD_UNITS; ++unit)
    {
        i = vformat->GetIndex(VertexFormat::AU_TEXCOORD, unit);
        if (i >= 0)
        {
            mHasTCoord[unit] = 1;
            type = vformat->GetAttributeType(i);
            mTCoordChannels[unit] = gOGLAttributeChannels[type];
            mTCoordType[unit] =
                gOGLAttributeType[vformat->GetAttributeType(i)];
            mTCoordOffset[unit] = vformat->GetOffset(i);
        }
        else
        {
            mHasTCoord[unit] = 0;
            mTCoordChannels[unit] = 0;
            mTCoordType[unit] = 0;
            mTCoordOffset[unit] = 0;
        }
    }

    for (unit = 0; unit < VertexFormat::AM_MAX_COLOR_UNITS; ++unit)
    {
        i = vformat->GetIndex(VertexFormat::AU_COLOR, unit);
        if (i >= 0)
        {
            mHasColor[unit] = 1;
            type = vformat->GetAttributeType(i);
            mColorChannels[unit] = gOGLAttributeChannels[type];
            mColorType[unit] =
                gOGLAttributeType[vformat->GetAttributeType(i)];
            mColorOffset[unit] = vformat->GetOffset(i);
        }
        else
        {
            mHasColor[unit] = 0;
            mColorChannels[unit] = 0;
            mColorType[unit] = 0;
            mColorOffset[unit] = 0;
        }
    }

    i = vformat->GetIndex(VertexFormat::AU_BLENDINDICES);
    if (i >= 0)
    {
        mHasBlendIndices = 1;
        type = vformat->GetAttributeType(i);
        mBlendIndicesChannels = gOGLAttributeChannels[type];
        mBlendIndicesType = gOGLAttributeType[vformat->GetAttributeType(i)];
        mBlendIndicesOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasBlendIndices = 0;
        mBlendIndicesChannels = 0;
        mBlendIndicesType = 0;
        mBlendIndicesOffset = 0;
    }

    i = vformat->GetIndex(VertexFormat::AU_BLENDWEIGHT);
    if (i >= 0)
    {
        mHasBlendWeight = 1;
        type = vformat->GetAttributeType(i);
        mBlendWeightChannels = gOGLAttributeChannels[type];
        mBlendWeightType = gOGLAttributeType[vformat->GetAttributeType(i)];
        mBlendWeightOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasBlendWeight = 0;
        mBlendWeightChannels = 0;
        mBlendWeightType = 0;
        mBlendWeightOffset = 0;
    }

    i = vformat->GetIndex(VertexFormat::AU_FOGCOORD);
    if (i >= 0)
    {
        mHasFogCoord = 1;
        type = vformat->GetAttributeType(i);
        mFogCoordChannels = gOGLAttributeChannels[type];
        mFogCoordType = gOGLAttributeType[vformat->GetAttributeType(i)];
        mFogCoordOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasFogCoord = 0;
        mFogCoordChannels = 0;
        mFogCoordType = 0;
        mFogCoordOffset = 0;
    }

    i = vformat->GetIndex(VertexFormat::AU_PSIZE);
    if (i >= 0)
    {
        mHasPSize = 1;
        type = vformat->GetAttributeType(i);
        mPSizeChannels = gOGLAttributeChannels[type];
        mPSizeType = gOGLAttributeType[vformat->GetAttributeType(i)];
        mPSizeOffset = vformat->GetOffset(i);
    }
    else
    {
        mHasPSize = 0;
        mPSizeChannels = 0;
        mPSizeType = 0;
        mPSizeOffset = 0;
    }
}
//----------------------------------------------------------------------------
PdrVertexFormat::~PdrVertexFormat ()
{
}
//----------------------------------------------------------------------------
void PdrVertexFormat::Enable (Renderer*)
{
    // Use the enabled vertex buffer for data pointers.

    if (mHasPosition)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(mPositionChannels, mPositionType, mStride,
            (char*)0 + mPositionOffset);
    }

    if (mHasNormal)
    {
#ifdef WM5_USE_OPENGL2_NORMAL_ATTRIBUTES
        glEnableVertexAttribArrayARB(2);
        glVertexAttribPointerARB(2, mNormalChannels, mNormalType, GL_FALSE,
            mStride, (char*)0 + mNormalOffset);
#else
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(mNormalType, mStride, (char*)0 + mNormalOffset);
#endif
    }

    if (mHasTangent)
    {
        glEnableVertexAttribArrayARB(14);
        glVertexAttribPointerARB(14, mTangentChannels, mTangentType, GL_FALSE,
            mStride, (char*)0 + mTangentOffset);
    }

    if (mHasBinormal)
    {
        glEnableVertexAttribArrayARB(15);
        glVertexAttribPointerARB(15, mBinormalChannels, mBinormalType,
            GL_FALSE, mStride, (char*)0 + mBinormalOffset);
    }

    unsigned int unit;
    for (unit = 0; unit < VertexFormat::AM_MAX_TCOORD_UNITS; ++unit)
    {
        if (mHasTCoord[unit])
        {
            glClientActiveTexture(GL_TEXTURE0 + unit);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
            glTexCoordPointer(mTCoordChannels[unit], mTCoordType[unit],
                mStride, (char*)0 + mTCoordOffset[unit]);
        }
    }

    if (mHasColor[0])
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(mColorChannels[0], mColorType[0], mStride,
            (char*)0 + mColorOffset[0]);
    }

    if (mHasColor[1])
    {
        glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
        glSecondaryColorPointer(mColorChannels[1], mColorType[1], mStride,
            (char*)0 + mColorOffset[1]);
    }

    if (mHasBlendIndices)
    {
        glEnableClientState(GL_MATRIX_INDEX_ARRAY_ARB);
        glMatrixIndexPointerARB(mBlendIndicesChannels, mBlendIndicesType,
            mStride, (char*)0 + mBlendIndicesOffset);
    }

    if (mHasBlendWeight)
    {
        glEnableClientState(GL_WEIGHT_ARRAY_ARB);
        glWeightPointerARB(mBlendWeightChannels, mBlendWeightType, mStride,
            (char*)0 + mBlendWeightOffset);
    }

    if (mHasFogCoord)
    {
        glEnableVertexAttribArrayARB(5);
        glVertexAttribPointerARB(5, mFogCoordChannels, mFogCoordType,
            GL_FALSE, mStride, (char*)0 + mFogCoordOffset);

        // Pre-OpenGL2 call, but there is no ability to specify the number
        // of channels.
        //
        // glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
        // glFogCoordPointerEXT(mFogCoordType, mStride,
        //     (char*)0 + mFogCoordOffset);
    }

    if (mHasPSize)
    {
        glEnableVertexAttribArrayARB(6);
        glVertexAttribPointerARB(6, mPSizeChannels, mPSizeType, GL_FALSE,
            mStride, (char*)0 + mPSizeOffset);
    }
}
//----------------------------------------------------------------------------
void PdrVertexFormat::Disable (Renderer*)
{
    if (mHasPosition)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (mHasNormal)
    {
#ifdef WM5_USE_OPENGL2_NORMAL_ATTRIBUTES
        glDisableVertexAttribArrayARB(2);
#else
        glDisableClientState(GL_NORMAL_ARRAY);
#endif
    }

    if (mHasTangent)
    {
        glDisableVertexAttribArrayARB(14);
    }

    if (mHasBinormal)
    {
        glDisableVertexAttribArrayARB(15);
    }

    unsigned int unit;
    for (unit = 0; unit < VertexFormat::AM_MAX_TCOORD_UNITS; ++unit)
    {
        if (mHasTCoord[unit])
        {
            glClientActiveTexture(GL_TEXTURE0 + unit);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
        }
    }

    if (mHasColor[0])
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }

    if (mHasColor[1])
    {
        glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
    }

    if (mHasBlendIndices)
    {
        glDisableClientState(GL_MATRIX_INDEX_ARRAY_ARB);
    }

    if (mHasBlendWeight)
    {
        glDisableClientState(GL_WEIGHT_ARRAY_ARB);
    }

    if (mHasFogCoord)
    {
        glDisableVertexAttribArrayARB(5);

        // Pre-OpenGL2 call, but there is no ability to specify the number
        // of channels.
        //
        // glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
    }

    if (mHasPSize)
    {
        glDisableVertexAttribArrayARB(6);
    }
}
//----------------------------------------------------------------------------
