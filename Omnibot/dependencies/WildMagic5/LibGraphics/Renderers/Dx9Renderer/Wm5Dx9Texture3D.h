// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5DX9TEXTURE3D_H
#define WM5DX9TEXTURE3D_H

#include "Wm5Dx9RendererLIB.h"
#include "Wm5Texture3D.h"

namespace Wm5
{

class Renderer;

class PdrTexture3D
{
public:
    // Construction and destruction.
    PdrTexture3D (Renderer* renderer, const Texture3D* texture);
    ~PdrTexture3D ();

    // Texture operations.
    void Enable (Renderer* renderer, int textureUnit);
    void Disable (Renderer* renderer, int textureUnit);
    void* Lock (int level, Buffer::Locking mode);
    void Unlock (int level);

private:
    IDirect3DVolumeTexture9* mTexture;
};

}

#endif
