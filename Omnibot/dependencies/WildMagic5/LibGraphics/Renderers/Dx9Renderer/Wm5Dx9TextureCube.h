// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5DX9TEXTURECUBE_H
#define WM5DX9TEXTURECUBE_H

#include "Wm5Dx9RendererLIB.h"
#include "Wm5TextureCube.h"

namespace Wm5
{

class Renderer;

class PdrTextureCube
{
public:
    // Construction and destruction.
    PdrTextureCube (Renderer* renderer, const TextureCube* texture);
    ~PdrTextureCube ();

    // Texture operations.
    void Enable (Renderer* renderer, int textureUnit);
    void Disable (Renderer* renderer, int textureUnit);
    void* Lock (int face, int level, Buffer::Locking mode);
    void Unlock (int face, int level);

private:
    IDirect3DCubeTexture9* mTexture;
};

}

#endif
