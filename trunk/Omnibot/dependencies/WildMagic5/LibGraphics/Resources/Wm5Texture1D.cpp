// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/05/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Texture1D.h"
#include "Wm5BitHacks.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Texture, Texture1D);
WM5_IMPLEMENT_STREAM(Texture1D);
WM5_IMPLEMENT_FACTORY(Texture1D);
WM5_IMPLEMENT_DEFAULT_NAMES(Texture, Texture1D);
WM5_IMPLEMENT_DEFAULT_STREAM(Texture, Texture1D);

//----------------------------------------------------------------------------
Texture1D::Texture1D (Format tformat, int dimension0, int numLevels,
    Buffer::Usage usage)
    :
    Texture(tformat, TT_1D, usage, numLevels)
{
    assertion(dimension0 > 0, "Dimension0 must be positive\n");
    mDimension[0][0] = dimension0;

    unsigned int logDim0 = Log2OfPowerOfTwo((unsigned int)dimension0);
    int maxLevels = (int)logDim0 + 1;

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
Texture1D::~Texture1D ()
{
    Renderer::UnbindAll(this);
}
//----------------------------------------------------------------------------
void Texture1D::GenerateMipmaps ()
{
    // Destroy the texture bindings if it does not have the maximum miplevels.
    // The bindings will be recreated later.
    unsigned int logDim0 = Log2OfPowerOfTwo((unsigned int)mDimension[0][0]);
    int maxLevels = (int)logDim0 + 1;
    bool retainBindings = true;
    if (mNumLevels != maxLevels)
    {
        retainBindings = false;
        Renderer::UnbindAll(this);
        mNumLevels = maxLevels;
        ComputeNumLevelBytes();
        
        char* newData = new1<char>(mNumTotalBytes);
        memcpy(newData, mData, mNumLevelBytes[0]);
        delete1(mData);
        mData = newData;
    }

    // Temporary storage for generating mipmaps.
    Float4* rgba = new1<Float4>(mDimension[0][0]);

    char* texels = mData;
    int length = mDimension[0][0];
    int level;
    for (level = 1; level < mNumLevels; ++level)
    {
        char* texelsNext = mData + mLevelOffsets[level];
        int lengthNext = mDimension[0][level];

        GenerateNextMipmap(length, texels, lengthNext, texelsNext, rgba);

        length = lengthNext;
        texels = texelsNext;
    }

    delete1(rgba);

    if (retainBindings)
    {
        for (level = 0; level < mNumLevels; ++level)
        {
            Renderer::UpdateAll(this, level);
        }
    }
}
//----------------------------------------------------------------------------
bool Texture1D::HasMipmaps () const
{
    unsigned int logDim0 = Log2OfPowerOfTwo((unsigned int)mDimension[0][0]);
    int maxLevels = (int)logDim0 + 1;
    return mNumLevels == maxLevels;
}
//----------------------------------------------------------------------------
char* Texture1D::GetData (int level) const
{
    if (mData && 0 <= level && level < mNumLevels)
    {
        return mData + mLevelOffsets[level];
    }

    assertion(false, "Null pointer or invalid level in GetData\n");
    return 0;
}
//----------------------------------------------------------------------------
void Texture1D::ComputeNumLevelBytes ()
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
    else if (mFormat == TF_DXT1 || mFormat == TF_DXT3 || mFormat == TF_DXT5)
    {
        assertion(false, "No DXT compression for 1D textures\n");
        mNumLevels = 1;
    }
    else if (mFormat == TF_D24S8)
    {
        assertion(false, "Depth textures must be 2D\n");
        mNumLevels = 1;
    }

    int level;
    int dim0 = mDimension[0][0];
    mNumTotalBytes = 0;
    for (level = 0; level < mNumLevels; ++level)
    {
        mNumLevelBytes[level] = msPixelSize[mFormat]*dim0;
        mNumTotalBytes += mNumLevelBytes[level];
        mDimension[0][level] = dim0;
        mDimension[1][level] = 1;
        mDimension[2][level] = 1;

        if (dim0 > 1)
        {
            dim0 >>= 1;
        }
    }

    mLevelOffsets[0] = 0;
    for (level = 0; level < mNumLevels-1; ++level)
    {
        mLevelOffsets[level+1] = mLevelOffsets[level] + mNumLevelBytes[level];
    }
}
//----------------------------------------------------------------------------
void Texture1D::GenerateNextMipmap (int length, const char* texels,
    int lengthNext, char* texelsNext, Float4* rgba)
{
    // Convert the texels from native format to 32-bit RGBA.
    msConvertFrom[mFormat](length, texels, rgba);

    // Create the next miplevel in-place.
    for (int i = 0; i < lengthNext; ++i)
    {
        int base = 2*i;
        for (int c = 0; c < 4; ++c)
        {
            rgba[i][c] = 0.5f*(rgba[base][c] + rgba[base+1][c]);
        }
    }

    // Convert the texels from 32-bit RGBA to native format.
    msConvertTo[mFormat](lengthNext, rgba, texelsNext);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for load of raw data.
//----------------------------------------------------------------------------
Texture1D* Texture1D::LoadWMTF (const std::string& name, int mode)
{
    Texture* texture = Texture::LoadWMTF(name, mode);
    if (texture && texture->IsExactly(Texture1D::TYPE))
    {
        return (Texture1D*)texture;
    }
    delete0(texture);
    return 0;
}
//----------------------------------------------------------------------------
