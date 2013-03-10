// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TEXTURE_H
#define WM5TEXTURE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5Buffer.h"
#include "Wm5FileIO.h"
#include "Wm5Float4.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Texture : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Texture);

public:
    // Currently supported formats.  More may be added as needed.
    enum WM5_GRAPHICS_ITEM Format
    {
        TF_NONE,

        // Small-bit color formats.
        TF_R5G6B5,
        TF_A1R5G5B5,
        TF_A4R4G4B4,

        // 8-bit integer formats.
        TF_A8,
        TF_L8,
        TF_A8L8,
        TF_R8G8B8,
        TF_A8R8G8B8,
        TF_A8B8G8R8,

        // 16-bit integer formats.
        TF_L16,
        TF_G16R16,
        TF_A16B16G16R16,

        // 16-bit floating-point formats ('half float' channels).
        TF_R16F,
        TF_G16R16F,
        TF_A16B16G16R16F,

        // 32-bit floating-point formats ('float' channels).
        TF_R32F,
        TF_G32R32F,
        TF_A32B32G32R32F,

        // DXT compressed formats.
        TF_DXT1,
        TF_DXT3,
        TF_DXT5,

        // Depth-stencil format.
        TF_D24S8,
        TF_QUANTITY
    };

    enum WM5_GRAPHICS_ITEM Type
    {
        TT_1D,
        TT_2D,
        TT_3D,
        TT_CUBE,
        TT_QUANTITY
    };

    // The maximum number of mipmap levels for a texture.  This is more than
    // enough for current-generation graphics cards.  A 4096x40096 image has
    // a maximum of 13 levels.
    enum
    {
        MM_MAX_MIPMAP_LEVELS = 16
    };

protected:
    // Abstract base class.  Construction and destruction.
    Texture (Format tformat, Type type, Buffer::Usage usage, int numLevels);
public:
    virtual ~Texture ();

    // Member access.
    inline Format GetFormat () const;
    inline Type GetTextureType () const;
    inline Buffer::Usage GetUsage () const;
    inline int GetNumLevels () const;
    inline int GetNumDimensions () const;
    inline int GetDimension (int i, int level) const;
    inline int GetNumLevelBytes (int level) const;
    inline int GetNumTotalBytes () const;
    inline int GetLevelOffset (int level) const;
    inline int GetPixelSize () const;
    inline static int GetPixelSize (Format tformat);
    inline bool IsCompressed () const;
    inline bool IsMipmapable () const;

    // Manage a copy of the texture in system memory.  The number of bytes is
    // queried with GetNumTotalBytes().  Queries of the system memory occur in
    // Texture1D, Texture2D, Texture3D, and TextureCube.  The GetData()
    // accessor should be for read-only data.  Do not typecast as "char*" and
    // write, otherwise the renderers will not know the data has been
    // modified.
    inline const char* GetData () const;

    // User fields.  These may be used to store application-specific
    // information that is unknown to the texture classes.
    enum { MAX_USER_FIELDS = 8 };
    inline void SetUserField (int i, int userField);
    inline int GetUserField (int i) const;

protected:
    // Members whose values are passed to the constructor.
    Format mFormat;
    Type mType;
    Buffer::Usage mUsage;
    int mNumLevels;

    // Members whose values are set or computed by the constructor and
    // derived-class constructors.
    int mNumDimensions;
    int mDimension[3][MM_MAX_MIPMAP_LEVELS];
    int mNumLevelBytes[MM_MAX_MIPMAP_LEVELS];
    int mNumTotalBytes;
    int mLevelOffsets[MM_MAX_MIPMAP_LEVELS];

    // User fields for storing application-specific data.
    int mUserField[MAX_USER_FIELDS];

    // System memory copy of the texture, which is used to load and
    // refresh video memory.
    char* mData;

    static int msNumDimensions[TT_QUANTITY];
    static int msPixelSize[TF_QUANTITY];
    static bool msMipmapable[TF_QUANTITY];

    // Support for mipmap generation.  Each format has its texels converted
    // to 32-bits per channel, the mipmap levels are generated at this
    // resolution, and the levels are converted back to the original format.

    typedef void (*ConvertFrom)(int, const char*, Float4*);
    static ConvertFrom msConvertFrom[TF_QUANTITY];

    typedef void (*ConvertTo)(int, const Float4*, char*);
    static ConvertTo msConvertTo[TF_QUANTITY];

// Support for the raw load/save of Texture.
public:
    // Load/save a texture from/to a *.wmtf file outside the streaming
    // system.  The texture object can be 1D, 2D, 3D or cube.  The caller
    // can query the returned Texture for the type.
    static Texture* LoadWMTF (const std::string& name,
        int mode = FileIO::FM_DEFAULT_READ);

    void SaveWMTF (const std::string& name,
        int mode = FileIO::FM_DEFAULT_WRITE);
// End support for raw load/save.
};

WM5_REGISTER_STREAM(Texture);
typedef Pointer0<Texture> TexturePtr;
#include "Wm5Texture.inl"

}

#endif
