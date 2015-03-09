// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TEXTURECUBE_H
#define WM5TEXTURECUBE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Texture.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM TextureCube : public Texture
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(TextureCube);

public:
    // Construction and destruction.  The faces of the cube are necessarily
    // square textures of size dimension-by-dimension.
    TextureCube (Format tformat, int dimension, int numLevels,
        Buffer::Usage usage = Buffer::BU_TEXTURE);

    virtual ~TextureCube ();

    // Get the dimensions of the zero-th level mipmap.
    inline int GetWidth () const;
    inline int GetHeight () const;

    // Support for mipmap generation.
    void GenerateMipmaps ();
    bool HasMipmaps () const;

    // Note:  Texture::GetNumLevelBytes(level) returns the number of bytes
    // for a level of *one* face.  Texture::GetNumTotalBytes() returns the
    // number of bytes of all levels of *all* faces.

    // Access the texture data.
    char* GetData (int face, int level) const;

protected:
    // Support for mipmap generation.
    void ComputeNumLevelBytes ();
    void GenerateNextMipmap (int dimension, const char* texels,
        int dimensionNext, char* texelsNext, Float4* rgba);

// Support for the raw load of TextureCube.  Raw save is in Texture.
public:
    static TextureCube* LoadWMTF (const std::string& name,
        int mode = FileIO::FM_DEFAULT_READ);
// End support for raw load.
};

WM5_REGISTER_STREAM(TextureCube);
typedef Pointer0<TextureCube> TextureCubePtr;
#include "Wm5TextureCube.inl"

}

#endif
