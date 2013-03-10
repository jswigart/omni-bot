// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/05/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5TextureCube.h"
#include "Wm5BitHacks.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Texture, TextureCube);
WM5_IMPLEMENT_STREAM(TextureCube);
WM5_IMPLEMENT_FACTORY(TextureCube);
WM5_IMPLEMENT_DEFAULT_NAMES(Texture, TextureCube);
WM5_IMPLEMENT_DEFAULT_STREAM(Texture, TextureCube);

//----------------------------------------------------------------------------
TextureCube::TextureCube (Format tformat, int dimension, int numLevels,
    Buffer::Usage usage)
    :
    Texture(tformat, TT_CUBE, usage, numLevels)
{
    assertion(dimension > 0, "Dimension must be positive\n");
    mDimension[0][0] = dimension;
    mDimension[1][0] = dimension;

    unsigned int logDim = Log2OfPowerOfTwo((unsigned int)dimension);
    int maxLevels = (int)logDim + 1;

    if (numLevels == 0)
    {
        // The user wants the maximum number of levels.
        mNumLevels = maxLevels;
    }
    else if (numLevels <= maxLevels)
    {
        // The user wants the specified number of levels.
        mNumLevels = numLevels;
    }
    else
    {
        assertion(false, "Invalid number of levels\n");
    }

    ComputeNumLevelBytes();
    mData = new1<char>(mNumTotalBytes);
}
//----------------------------------------------------------------------------
TextureCube::~TextureCube ()
{
    Renderer::UnbindAll(this);
}
//----------------------------------------------------------------------------
void TextureCube::GenerateMipmaps ()
{
    // Destroy the texture bindings if it does not have the maximum miplevels.
    // The bindings will be recreated later.
    unsigned int logDim = Log2OfPowerOfTwo((unsigned int)mDimension[0][0]);
    int maxLevels = (int)logDim + 1;
    int face, faceOffset, level;
    bool retainBindings = true;
    if (mNumLevels != maxLevels)
    {
        retainBindings = false;
        Renderer::UnbindAll(this);
        mNumLevels = maxLevels;
        int oldNumTotalBytes = mNumTotalBytes/6;
        ComputeNumLevelBytes();

        char* newData = new1<char>(mNumTotalBytes);
        for (face = 0; face < 6; ++face)
        {
            int oldFaceOffset = face*oldNumTotalBytes;
            faceOffset = face*mNumTotalBytes/6;
            memcpy(newData + faceOffset, mData + oldFaceOffset,
                mNumLevelBytes[0]);
        }
        delete1(mData);
        mData = newData;
    }

    // Temporary storage for generating mipmaps.
    Float4* rgba = new1<Float4>(mDimension[0][0]*mDimension[1][0]);

    for (face = 0; face < 6; ++face)
    {
        faceOffset = face*mNumTotalBytes/6;
        char* texels = mData + faceOffset;
        int dimension = mDimension[0][0];
        for (level = 1; level < mNumLevels; ++level)
        {
            char* texelsNext = mData + faceOffset + mLevelOffsets[level];
            int dimensionNext = mDimension[0][level];

            GenerateNextMipmap(dimension, texels, dimensionNext, texelsNext,
                rgba);

            dimension = dimensionNext;
            texels = texelsNext;
        }
    }

    delete1(rgba);

    if (retainBindings)
    {
        for (face = 0; face < 6; ++face)
        {
            for (level = 0; level < mNumLevels; ++level)
            {
                Renderer::UpdateAll(this, face, level);
            }
        }
    }
}
//----------------------------------------------------------------------------
bool TextureCube::HasMipmaps () const
{
    unsigned int logDim = Log2OfPowerOfTwo((unsigned int)mDimension[0][0]);
    int maxLevels = (int)logDim + 1;
    return mNumLevels == maxLevels;
}
//----------------------------------------------------------------------------
char* TextureCube::GetData (int face, int level) const
{
    if (mData && 0 <= face && face < 6 && 0 <= level && level < mNumLevels)
    {
        int faceOffset = face*mNumTotalBytes/6;
        return mData + faceOffset + mLevelOffsets[level];
    }

    assertion(false, "Null pointer or invalid level in GetData\n");
    return 0;
}
//----------------------------------------------------------------------------
void TextureCube::ComputeNumLevelBytes ()
{
    if (mFormat == TF_R32F
    ||  mFormat == TF_G32R32F
    ||  mFormat == TF_A32B32G32R32F)
    {
        if (mNumLevels > 1)
        {
            assertion(false, "No mipmaps for 32-bit float textures\n");
            mNumLevels = 1;
        }
    }
    else if (mFormat == TF_D24S8)
    {
        if (mNumLevels > 1)
        {
            assertion(false, "No mipmaps for 2D depth textures\n");
            mNumLevels = 1;
        }
    }

    int dim0 = mDimension[0][0];
    int dim1 = mDimension[1][0];
    int level;
    mNumTotalBytes = 0;

    if (mFormat == TF_DXT1)
    {
        for (level = 0; level < mNumLevels; ++level)
        {
            int max0 = dim0/4;
            if (max0 < 1)
            {
                max0 = 1;
            }
            int max1 = dim1/4;
            if (max1 < 1)
            {
                max1 = 1;
            }

            mNumLevelBytes[level] = 8*max0*max1;
            mNumTotalBytes += mNumLevelBytes[level];
            mDimension[0][level] = dim0;
            mDimension[1][level] = dim1;

            if (dim0 > 1)
            {
                dim0 >>= 1;
            }
            if (dim1 > 1)
            {
                dim1 >>= 1;
            }
        }
    }
    else if (mFormat == TF_DXT3 || mFormat == TF_DXT5)
    {
        for (level = 0; level < mNumLevels; ++level)
        {
            int max0 = dim0/4;
            if (max0 < 1)
            {
                max0 = 1;
            }
            int max1 = dim1/4;
            if (max1 < 1)
            {
                max1 = 1;
            }

            mNumLevelBytes[level] = 16*max0*max1;
            mNumTotalBytes += mNumLevelBytes[level];
            mDimension[0][level] = dim0;
            mDimension[1][level] = dim1;

            if (dim0 > 1)
            {
                dim0 >>= 1;
            }
            if (dim1 > 1)
            {
                dim1 >>= 1;
            }
        }
    }
    else
    {
        for (level = 0; level < mNumLevels; ++level)
        {
            mNumLevelBytes[level] = msPixelSize[mFormat]*dim0*dim1;
            mNumTotalBytes += mNumLevelBytes[level];
            mDimension[0][level] = dim0;
            mDimension[1][level] = dim1;

            if (dim0 > 1)
            {
                dim0 >>= 1;
            }
            if (dim1 > 1)
            {
                dim1 >>= 1;
            }
        }
    }

    mNumTotalBytes *= 6;

    mLevelOffsets[0] = 0;
    for (level = 0; level < mNumLevels-1; ++level)
    {
        mLevelOffsets[level+1] = mLevelOffsets[level] + mNumLevelBytes[level];
    }
}
//----------------------------------------------------------------------------
void TextureCube::GenerateNextMipmap (int dimension, const char* texels,
    int dimensionNext, char* texelsNext, Float4* rgba)
{
    // Convert the texels from native format to 32-bit RGBA.
    int numTexels = dimension*dimension;
    msConvertFrom[mFormat](numTexels, texels, rgba);

    // Create the next miplevel in-place.
    for (int i1 = 0; i1 < dimensionNext; ++i1)
    {
        for (int i0 = 0; i0 < dimensionNext; ++i0)
        {
            int j = i0 + dimensionNext*i1;
            int base = 2*(i0 + dimension*i1);
            for (int c = 0; c < 4; ++c)
            {
                rgba[j][c] = 0.25f*(
                    rgba[base][c] +
                    rgba[base + 1][c] +
                    rgba[base + dimension][c] +
                    rgba[base + dimension + 1][c]);
            }
        }
    }

    // Convert the texels from 32-bit RGBA to native format.
    int numTexelsNext = dimensionNext*dimensionNext;
    msConvertTo[mFormat](numTexelsNext, rgba, texelsNext);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for load of raw data.
//----------------------------------------------------------------------------
TextureCube* TextureCube::LoadWMTF (const std::string& name, int mode)
{
    Texture* texture = Texture::LoadWMTF(name, mode);
    if (texture && texture->IsExactly(TextureCube::TYPE))
    {
        return (TextureCube*)texture;
    }
    delete0(texture);
    return 0;
}
//----------------------------------------------------------------------------
