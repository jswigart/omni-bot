// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline Texture::Format Texture::GetFormat () const
{
    return mFormat;
}
//----------------------------------------------------------------------------
inline Texture::Type Texture::GetTextureType () const
{
    return mType;
}
//----------------------------------------------------------------------------
inline Buffer::Usage Texture::GetUsage () const
{
    return mUsage;
}
//----------------------------------------------------------------------------
inline int Texture::GetNumLevels () const
{
    return mNumLevels;
}
//----------------------------------------------------------------------------
inline int Texture::GetNumDimensions () const
{
    return msNumDimensions[mType];
}
//----------------------------------------------------------------------------
inline int Texture::GetDimension (int i, int level) const
{
    return mDimension[i][level];
}
//----------------------------------------------------------------------------
inline int Texture::GetNumLevelBytes (int level) const
{
    return mNumLevelBytes[level];
}
//----------------------------------------------------------------------------
inline int Texture::GetNumTotalBytes () const
{
    return mNumTotalBytes;
}
//----------------------------------------------------------------------------
inline int Texture::GetLevelOffset (int level) const
{
    return mLevelOffsets[level];
}
//----------------------------------------------------------------------------
inline int Texture::GetPixelSize () const
{
    return msPixelSize[mFormat];
}
//----------------------------------------------------------------------------
inline int Texture::GetPixelSize (Format tformat)
{
    return msPixelSize[tformat];
}
//----------------------------------------------------------------------------
inline bool Texture::IsCompressed () const
{
    return mFormat == TF_DXT1 || mFormat == TF_DXT3 || mFormat == TF_DXT5;
}
//----------------------------------------------------------------------------
inline bool Texture::IsMipmapable () const
{
    return msMipmapable[mFormat];
}
//----------------------------------------------------------------------------
inline const char* Texture::GetData () const
{
    return mData;
}
//----------------------------------------------------------------------------
inline void Texture::SetUserField (int i, int userField)
{
    mUserField[i] = userField;
}
//----------------------------------------------------------------------------
inline int Texture::GetUserField (int i) const
{
    return mUserField[i];
}
//----------------------------------------------------------------------------
