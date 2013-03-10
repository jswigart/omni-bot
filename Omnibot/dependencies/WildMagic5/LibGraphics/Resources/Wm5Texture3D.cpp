// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/05/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Texture3D.h"
#include "Wm5BitHacks.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Texture, Texture3D);
WM5_IMPLEMENT_STREAM(Texture3D);
WM5_IMPLEMENT_FACTORY(Texture3D);
WM5_IMPLEMENT_DEFAULT_NAMES(Texture, Texture3D);
WM5_IMPLEMENT_DEFAULT_STREAM(Texture, Texture3D);

//----------------------------------------------------------------------------
Texture3D::Texture3D (Format tformat, int dimension0, int dimension1,
    int dimension2, int numLevels, Buffer::Usage usage)
    :
    Texture(tformat, TT_3D, usage, numLevels)
{
    assertion(dimension0 > 0, "Dimension0 must be positive\n");
    assertion(dimension1 > 0, "Dimension1 must be positive\n");
    assertion(dimension2 > 0, "Dimension2 must be positive\n");
    mDimension[0][0] = dimension0;
    mDimension[1][0] = dimension1;
    mDimension[2][0] = dimension2;

    unsigned int logDim0 = Log2OfPowerOfTwo((unsigned int)dimension0);
    unsigned int logDim1 = Log2OfPowerOfTwo((unsigned int)dimension1);
    unsigned int logDim2 = Log2OfPowerOfTwo((unsigned int)dimension2);
    int maxLevels = logDim0;
    if ((int)logDim1 > maxLevels)
    {
        maxLevels = logDim1;
    }
    if ((int)logDim2 > maxLevels)
    {
        maxLevels = logDim2;
    }
    ++maxLevels;

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
Texture3D::~Texture3D ()
{
    Renderer::UnbindAll(this);
}
//----------------------------------------------------------------------------
void Texture3D::GenerateMipmaps ()
{
    // Recreate the texture if it does not have the maximum miplevels.
    unsigned int logDim0 = Log2OfPowerOfTwo((unsigned int)mDimension[0][0]);
    unsigned int logDim1 = Log2OfPowerOfTwo((unsigned int)mDimension[1][0]);
    unsigned int logDim2 = Log2OfPowerOfTwo((unsigned int)mDimension[2][0]);
    int maxLevels = logDim0;
    if ((int)logDim1 > maxLevels)
    {
        maxLevels = logDim1;
    }
    if ((int)logDim2 > maxLevels)
    {
        maxLevels = logDim2;
    }
    ++maxLevels;

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
    Float4* rgba = new1<Float4>(
        mDimension[0][0]*mDimension[1][0]*mDimension[2][0]);

    char* texels = mData;
    int width = mDimension[0][0];
    int height = mDimension[1][0];
    int thickness = mDimension[2][0];
    int level;
    for (level = 1; level < mNumLevels; ++level)
    {
        char* texelsNext = mData + mLevelOffsets[level];
        int widthNext = mDimension[0][level];
        int heightNext = mDimension[1][level];
        int thicknessNext = mDimension[2][level];

        GenerateNextMipmap(width, height, thickness, texels, widthNext,
            heightNext, thicknessNext, texelsNext, rgba);

        width = widthNext;
        height = heightNext;
        thickness = thicknessNext;
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
bool Texture3D::HasMipmaps () const
{
    unsigned int logDim0 = Log2OfPowerOfTwo((unsigned int)mDimension[0][0]);
    unsigned int logDim1 = Log2OfPowerOfTwo((unsigned int)mDimension[1][0]);
    unsigned int logDim2 = Log2OfPowerOfTwo((unsigned int)mDimension[2][0]);
    int maxLevels = logDim0;
    if ((int)logDim1 > maxLevels)
    {
        maxLevels = logDim1;
    }
    if ((int)logDim2 > maxLevels)
    {
        maxLevels = logDim2;
    }
    ++maxLevels;
    return mNumLevels == maxLevels;
}
//----------------------------------------------------------------------------
char* Texture3D::GetData (int level) const
{
    if (mData && 0 <= level && level < mNumLevels)
    {
        return mData + mLevelOffsets[level];
    }

    assertion(false, "Null pointer or invalid level in GetData\n");
    return 0;
}
//----------------------------------------------------------------------------
void Texture3D::ComputeNumLevelBytes ()
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
    int dim1 = mDimension[1][0];
    int dim2 = mDimension[2][0];
    mNumTotalBytes = 0;
    for (level = 0; level < mNumLevels; ++level)
    {
        mNumLevelBytes[level] = msPixelSize[mFormat]*dim0*dim1*dim2;
        mNumTotalBytes += mNumLevelBytes[level];
        mDimension[0][level] = dim0;
        mDimension[1][level] = dim1;
        mDimension[2][level] = dim2;

        if (dim0 > 1)
        {
            dim0 >>= 1;
        }
        if (dim1 > 1)
        {
            dim1 >>= 1;
        }
        if (dim2 > 1)
        {
            dim2 >>= 1;
        }
    }

    mLevelOffsets[0] = 0;
    for (level = 0; level < mNumLevels-1; ++level)
    {
        mLevelOffsets[level+1] = mLevelOffsets[level] + mNumLevelBytes[level];
    }
}
//----------------------------------------------------------------------------
void Texture3D::GenerateNextMipmap (int width, int height, int thickness,
    const char* texels, int widthNext, int heightNext, int thicknessNext,
    char* texelsNext, Float4* rgba)
{
    // Convert the texels from native format to 32-bit RGBA.
    int widthHeight = width*height;
    int numTexels = widthHeight*thickness;
    msConvertFrom[mFormat](numTexels, texels, rgba);

    // Create the next miplevel in-place.
    for (int i2 = 0; i2 < thicknessNext; ++i2)
    {
        for (int i1 = 0; i1 < heightNext; ++i1)
        {
            for (int i0 = 0; i0 < widthNext; ++i0)
            {
                int j = i0 + widthNext*(i1 + heightNext*i2);
                int base = 2*(i0 + width*(i1 + height*i2));
                for (int c = 0; c < 4; ++c)
                {
                    rgba[j][c] = 0.125f*(
                        rgba[base][c] +
                        rgba[base + 1][c] +
                        rgba[base + width][c] +
                        rgba[base + width + 1][c] +
                        rgba[base + widthHeight][c] +
                        rgba[base + 1 + widthHeight][c] +
                        rgba[base + width + widthHeight][c] +
                        rgba[base + width + 1 + widthHeight][c]
                    );
                }
            }
        }
    }

    // Convert the texels from 32-bit RGBA to native format.
    int numTexelsNext = widthNext*heightNext*thicknessNext;
    msConvertTo[mFormat](numTexelsNext, rgba, texelsNext);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for load of raw data.
//----------------------------------------------------------------------------
Texture3D* Texture3D::LoadWMTF (const std::string& name, int mode)
{
    Texture* texture = Texture::LoadWMTF(name, mode);
    if (texture && texture->IsExactly(Texture3D::TYPE))
    {
        return (Texture3D*)texture;
    }
    delete0(texture);
    return 0;
}
//----------------------------------------------------------------------------
