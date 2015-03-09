// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TEXTURE3D_H
#define WM5TEXTURE3D_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Texture.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Texture3D : public Texture
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Texture3D);

public:
    // Construction and destruction.
    Texture3D (Format tformat, int dimension0, int dimension1, int dimension2,
        int numLevels, Buffer::Usage usage = Buffer::BU_TEXTURE);

    virtual ~Texture3D ();

    // Get the dimensions of the zero-th level mipmap.
    inline int GetWidth () const;
    inline int GetHeight () const;
    inline int GetThickness () const;

    // Support for mipmap generation.
    void GenerateMipmaps ();
    bool HasMipmaps () const;

    // Access the texture data.
    char* GetData (int level) const;

protected:
    // Support for mipmap generation.
    void ComputeNumLevelBytes ();
    void GenerateNextMipmap (int width, int height, int thickness,
        const char* texels, int widthNext, int heightNext, int thicknessNext,
        char* texelsNext, Float4* rgba);

// Support for the raw load of Texture3D.  Raw save is in Texture.
public:
    static Texture3D* LoadWMTF (const std::string& name,
        int mode = FileIO::FM_DEFAULT_READ);
// End support for raw load.
};

WM5_REGISTER_STREAM(Texture3D);
typedef Pointer0<Texture3D> Texture3DPtr;
#include "Wm5Texture3D.inl"

}

#endif
