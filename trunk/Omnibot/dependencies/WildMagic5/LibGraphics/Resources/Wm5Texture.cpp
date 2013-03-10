// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/05/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Texture.h"
#include "Wm5Texture1D.h"
#include "Wm5Texture2D.h"
#include "Wm5Texture3D.h"
#include "Wm5TextureCube.h"
#include "Wm5BitHacks.h"
#include "Wm5Color.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Texture);
WM5_IMPLEMENT_STREAM(Texture);
WM5_IMPLEMENT_ABSTRACT_FACTORY(Texture);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, Texture);

int Texture::msNumDimensions[TT_QUANTITY] =
{
    1,  // TT_1D
    2,  // TT_2D
    3,  // TT_3D
    2   // TT_CUBE
};

int Texture::msPixelSize[TF_QUANTITY] =
{
    0,   // TF_NONE
    2,   // TF_R5G6B5
    2,   // TF_A1R5G5B5
    2,   // TF_A4R4G4B4
    1,   // TF_A8
    1,   // TF_L8
    2,   // TF_A8L8
    3,   // TF_R8G8B8
    4,   // TF_A8R8G8B8
    4,   // TF_A8B8G8R8
    2,   // TF_L16
    4,   // TF_G16R16
    8,   // TF_A16B16G16R16
    2,   // TF_R16F
    4,   // TF_G16R16F
    8,   // TF_A16B16G16R16F
    4,   // TF_R32F
    8,   // TF_G32R32F
    16,  // TF_A32B32G32R32F,
    0,   // TF_DXT1 (special handling)
    0,   // TF_DXT3 (special handling)
    0,   // TF_DXT5 (special handling)
    4    // TF_D24S8
};

bool Texture::msMipmapable[TF_QUANTITY] =
{
    false,  // TF_NONE
    true,   // TF_R5G6B5
    true,   // TF_A1R5G5B5
    true,   // TF_A4R4G4B4
    true,   // TF_A8
    true,   // TF_L8
    true,   // TF_A8L8
    true,   // TF_R8G8B8
    true,   // TF_A8R8G8B8
    true,   // TF_A8B8G8R8
    true,   // TF_L16
    true,   // TF_G16R16
    true,   // TF_A16B16G16R16
    true,   // TF_R16F
    true,   // TF_G16R16F
    true,   // TF_A16B16G16R16F
    false,  // TF_R32F
    false,  // TF_G32R32F
    false,  // TF_A32B32G32R32F,
    true,   // TF_DXT1 (special handling)
    true,   // TF_DXT3 (special handling)
    true,   // TF_DXT5 (special handling)
    false   // TF_D24S8
};

Texture::ConvertFrom Texture::msConvertFrom[TF_QUANTITY] =
{
    0,                                    // TF_NONE
    &Color::ConvertFromR5G6B5,            // TF_R5G6B5
    &Color::ConvertFromA1R5G5B5,          // TF_A1R5G5B5
    &Color::ConvertFromA4R4G4B4,          // TF_A4R4G4B4
    &Color::ConvertFromA8,                // TF_A8
    &Color::ConvertFromL8,                // TF_L8
    &Color::ConvertFromA8L8,              // TF_A8L8
    &Color::ConvertFromR8G8B8,            // TF_R8G8B8
    &Color::ConvertFromA8R8G8B8,          // TF_A8R8G8B8
    &Color::ConvertFromA8B8G8R8,          // TF_A8B8G8R8
    &Color::ConvertFromL16,               // TF_L16
    &Color::ConvertFromG16R16,            // TF_G16R16
    &Color::ConvertFromA16B16G16R16,      // TF_A16B16G16R16
    &Color::ConvertFromR16F,              // TF_R16F
    &Color::ConvertFromG16R16F,           // TF_G16R16F
    &Color::ConvertFromA16B16G16R16F,     // TF_A16B16G16R16F
    &Color::ConvertFromR32F,              // TF_R32F
    &Color::ConvertFromG32R32F,           // TF_G32R32F
    &Color::ConvertFromA32B32G32R32F,     // TF_A32B32G32R32F
    0,                                    // TF_DXT1
    0,                                    // TF_DXT3
    0,                                    // TF_DXT5
    0                                     // TF_D24S8
};

Texture::ConvertTo Texture::msConvertTo[TF_QUANTITY] =
{
    0,                                    // TF_NONE
    &Color::ConvertToR5G6B5,              // TF_R5G6B5
    &Color::ConvertToA1R5G5B5,            // TF_A1R5G5B5
    &Color::ConvertToA4R4G4B4,            // TF_A4R4G4B4
    &Color::ConvertToA8,                  // TF_A8
    &Color::ConvertToL8,                  // TF_L8
    &Color::ConvertToA8L8,                // TF_A8L8
    &Color::ConvertToR8G8B8,              // TF_R8G8B8
    &Color::ConvertToA8R8G8B8,            // TF_A8R8G8B8
    &Color::ConvertToA8B8G8R8,            // TF_A8B8G8R8
    &Color::ConvertToL16,                 // TF_L16
    &Color::ConvertToG16R16,              // TF_G16R16
    &Color::ConvertToA16B16G16R16,        // TF_A16B16G16R16
    &Color::ConvertToR16F,                // TF_R16F
    &Color::ConvertToG16R16F,             // TF_G16R16F
    &Color::ConvertToA16B16G16R16F,       // TF_A16B16G16R16F
    &Color::ConvertToR32F,                // TF_R32F
    &Color::ConvertToG32R32F,             // TF_G32R32F
    &Color::ConvertToA32B32G32R32F,       // TF_A32B32G32R32F
    0,                                    // TF_DXT1
    0,                                    // TF_DXT3
    0,                                    // TF_DXT5
    0                                     // TF_D24S8
};

//----------------------------------------------------------------------------
Texture::Texture (Format tformat, Type type, Buffer::Usage usage,
    int numLevels)
    :
    mFormat(tformat),
    mType(type),
    mUsage(usage),
    mNumLevels(numLevels),
    mNumDimensions(msNumDimensions[type]),
    mNumTotalBytes(0),
    mData(0)
{
    for (int level = 0; level < MM_MAX_MIPMAP_LEVELS; ++level)
    {
        mDimension[0][level] = 0;
        mDimension[1][level] = 0;
        mDimension[2][level] = 0;
        mNumLevelBytes[level] = 0;
        mLevelOffsets[level] = 0;
    }

    for (int i = 0; i < MAX_USER_FIELDS; ++i)
    {
        mUserField[i] = 0;
    }
}
//----------------------------------------------------------------------------
Texture::~Texture ()
{
    delete1(mData);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for load/save of raw data.
//----------------------------------------------------------------------------
Texture* Texture::LoadWMTF (const std::string& name, int mode)
{
    FileIO inFile(name, mode);
    if (!inFile)
    {
        assertion(false, "Failed to load file %s\n", name.c_str());
        return 0;
    }

    // Read the texture header information first.  Some of the header fields
    // are not necessary to create the texture, but they exist in the file in
    // case other tools need the information without having to create a
    // Texture object.
    int format, type, usage, numLevels, numDimensions, numTotalBytes;
    int dimension[3][MM_MAX_MIPMAP_LEVELS];
    int numLevelBytes[MM_MAX_MIPMAP_LEVELS];
    int levelOffsets[MM_MAX_MIPMAP_LEVELS];
    int userField[MAX_USER_FIELDS];
    inFile.Read(sizeof(int), &format);
    inFile.Read(sizeof(int), &type);
    inFile.Read(sizeof(int), &usage);
    inFile.Read(sizeof(int), &numLevels);
    inFile.Read(sizeof(int), &numDimensions);
    inFile.Read(sizeof(int), MM_MAX_MIPMAP_LEVELS, dimension[0]);
    inFile.Read(sizeof(int), MM_MAX_MIPMAP_LEVELS, dimension[1]);
    inFile.Read(sizeof(int), MM_MAX_MIPMAP_LEVELS, dimension[2]);
    inFile.Read(sizeof(int), MM_MAX_MIPMAP_LEVELS, numLevelBytes);
    inFile.Read(sizeof(int), &numTotalBytes);
    inFile.Read(sizeof(int), MM_MAX_MIPMAP_LEVELS, levelOffsets);
    inFile.Read(sizeof(int), MAX_USER_FIELDS, userField);

    Texture* texture = 0;

    switch ((Type)type)
    {
    case TT_1D:
    {
        texture = new0 Texture1D((Format)format, dimension[0][0], numLevels,
            (Buffer::Usage)usage);
        break;
    }
    case TT_2D:
    {
        texture = new0 Texture2D((Format)format, dimension[0][0],
            dimension[1][0], numLevels, (Buffer::Usage)usage);
        break;
    }
    case TT_3D:
    {
        texture = new0 Texture3D((Format)format, dimension[0][0],
            dimension[1][0], dimension[2][0], numLevels,
            (Buffer::Usage)usage);

        break;
    }
    case TT_CUBE:
    {
        texture = new0 TextureCube((Format)format, dimension[0][0],
            numLevels, (Buffer::Usage)usage);
        break;
    }
    default:
        assertion(false, "Unknown texture type.\n");
        return 0;
    }

    // TODO.  Big endian machines might require swapping the red and blue
    // channels.
    switch (texture->mFormat)
    {
    // Small-bit color formats.
    case TF_R5G6B5:
    case TF_A1R5G5B5:
    case TF_A4R4G4B4:
        inFile.Read(2, numTotalBytes/2, texture->mData);
        break;

    // 8-bit integer formats.
    case TF_A8:
    case TF_L8:
    case TF_A8L8:
    case TF_R8G8B8:
    case TF_A8R8G8B8:
    case TF_A8B8G8R8:
        inFile.Read(1, numTotalBytes, texture->mData);
        break;

    // 16-bit formats.
    case TF_L16:
    case TF_G16R16:
    case TF_A16B16G16R16:
    case TF_R16F:
    case TF_G16R16F:
    case TF_A16B16G16R16F:
        inFile.Read(2, numTotalBytes/2, texture->mData);
        break;

    // 32-bit formats.
    case TF_R32F:
    case TF_G32R32F:
    case TF_A32B32G32R32F:
    case TF_D24S8:
        inFile.Read(4, numTotalBytes/4, texture->mData);
        break;

    // DXT compressed formats.  TODO: How to handle?
    case TF_DXT1:
    case TF_DXT3:
    case TF_DXT5:
        inFile.Read(1, numTotalBytes, texture->mData);
        break;

    default:
        assertion(false, "Unknown texture format.\n");
        return 0;
    }

    for (int i = 0; i < MAX_USER_FIELDS; ++i)
    {
        texture->mUserField[i] = userField[i];
    }

    inFile.Close();

    return texture;
}
//----------------------------------------------------------------------------
void Texture::SaveWMTF (const std::string& name, int mode)
{
    FileIO outFile(name, mode);
    if (!outFile)
    {
        assertion(false, "Failed to save file %s\n", name.c_str());
        return;
    }

    int format = (int)mFormat;
    outFile.Write(sizeof(int), &format);

    int type = (int)mType;
    outFile.Write(sizeof(int), &type);

    int usage = (int)mUsage;
    outFile.Write(sizeof(int), &usage);

    outFile.Write(sizeof(int), &mNumLevels);
    outFile.Write(sizeof(int), &mNumDimensions);
    outFile.Write(sizeof(int), MM_MAX_MIPMAP_LEVELS, mDimension[0]);
    outFile.Write(sizeof(int), MM_MAX_MIPMAP_LEVELS, mDimension[1]);
    outFile.Write(sizeof(int), MM_MAX_MIPMAP_LEVELS, mDimension[2]);
    outFile.Write(sizeof(int), MM_MAX_MIPMAP_LEVELS, mNumLevelBytes);
    outFile.Write(sizeof(int), &mNumTotalBytes);
    outFile.Write(sizeof(int), MM_MAX_MIPMAP_LEVELS, mLevelOffsets);
    outFile.Write(sizeof(int), MAX_USER_FIELDS, mUserField);

    // TODO.  Big endian machines might require swapping the red and blue
    // channels.
    switch (mFormat)
    {
    // Small-bit color formats.
    case TF_R5G6B5:
    case TF_A1R5G5B5:
    case TF_A4R4G4B4:
        outFile.Write(2, mNumTotalBytes/2, mData);
        break;

    // 8-bit integer formats.
    case TF_A8:
    case TF_L8:
    case TF_A8L8:
    case TF_R8G8B8:
    case TF_A8R8G8B8:
    case TF_A8B8G8R8:
        outFile.Write(1, mNumTotalBytes, mData);
        break;

    // 16-bit formats.
    case TF_L16:
    case TF_G16R16:
    case TF_A16B16G16R16:
    case TF_R16F:
    case TF_G16R16F:
    case TF_A16B16G16R16F:
        outFile.Write(2, mNumTotalBytes/2, mData);
        break;

    // 32-bit formats.
    case TF_R32F:
    case TF_G32R32F:
    case TF_A32B32G32R32F:
    case TF_D24S8:
        outFile.Write(4, mNumTotalBytes/4, mData);
        break;

    // DXT compressed formats.  TODO: How to handle?
    case TF_DXT1:
    case TF_DXT3:
    case TF_DXT5:
        outFile.Write(1, mNumTotalBytes, mData);
        break;

    default:
        assertion(false, "Unknown texture type\n");
        break;
    }

    outFile.Close();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Texture::Texture (LoadConstructor value)
    :
    Object(value),
    mFormat(TF_NONE),
    mType(TT_QUANTITY),
    mUsage(Buffer::BU_QUANTITY),
    mNumLevels(0),
    mNumDimensions(0),
    mData(0)
{
    for (int level = 0; level < MM_MAX_MIPMAP_LEVELS; ++level)
    {
        mNumLevelBytes[level] = 0;
        mDimension[0][level] = 0;
        mDimension[1][level] = 0;
        mDimension[2][level] = 0;
        mLevelOffsets[level] = 0;
    }

    for (int i = 0; i < MAX_USER_FIELDS; ++i)
    {
        mUserField[i] = 0;
    }
}
//----------------------------------------------------------------------------
void Texture::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadEnum(mFormat);
    source.ReadEnum(mType);
    source.ReadEnum(mUsage);
    source.Read(mNumLevels);
    source.Read(mNumDimensions);
    source.ReadVV(MM_MAX_MIPMAP_LEVELS, mDimension[0]);
    source.ReadVV(MM_MAX_MIPMAP_LEVELS, mDimension[1]);
    source.ReadVV(MM_MAX_MIPMAP_LEVELS, mDimension[2]);
    source.ReadVV(MM_MAX_MIPMAP_LEVELS, mNumLevelBytes);
    source.Read(mNumTotalBytes);
    source.ReadVV(MM_MAX_MIPMAP_LEVELS, mLevelOffsets);
    source.ReadVV(MAX_USER_FIELDS, mUserField);
    source.ReadVR(mNumTotalBytes, mData);

    WM5_END_DEBUG_STREAM_LOAD(Texture, source);
}
//----------------------------------------------------------------------------
void Texture::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void Texture::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool Texture::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void Texture::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteEnum(mFormat);
    target.WriteEnum(mType);
    target.WriteEnum(mUsage);
    target.Write(mNumLevels);
    target.Write(mNumDimensions);
    target.WriteN(MM_MAX_MIPMAP_LEVELS, mDimension[0]);
    target.WriteN(MM_MAX_MIPMAP_LEVELS, mDimension[1]);
    target.WriteN(MM_MAX_MIPMAP_LEVELS, mDimension[2]);
    target.WriteN(MM_MAX_MIPMAP_LEVELS, mNumLevelBytes);
    target.Write(mNumTotalBytes);
    target.WriteN(MM_MAX_MIPMAP_LEVELS, mLevelOffsets);
    target.WriteN(MAX_USER_FIELDS, mUserField);
    target.WriteN(mNumTotalBytes, mData);

    WM5_END_DEBUG_STREAM_SAVE(Texture, target);
}
//----------------------------------------------------------------------------
int Texture::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_ENUMSIZE(mFormat);
    size += WM5_ENUMSIZE(mType);
    size += WM5_ENUMSIZE(mUsage);
    size += sizeof(mNumLevels);
    size += sizeof(mNumDimensions);
    size += MM_MAX_MIPMAP_LEVELS*sizeof(mDimension[0][0]);
    size += MM_MAX_MIPMAP_LEVELS*sizeof(mDimension[1][0]);
    size += MM_MAX_MIPMAP_LEVELS*sizeof(mDimension[2][0]);
    size += MM_MAX_MIPMAP_LEVELS*sizeof(mNumLevelBytes[0]);
    size += sizeof(mNumTotalBytes);
    size += MM_MAX_MIPMAP_LEVELS*sizeof(mLevelOffsets[0]);
    size += MAX_USER_FIELDS*sizeof(mUserField[0]);
    size += mNumTotalBytes;
    return size;
}
//----------------------------------------------------------------------------
