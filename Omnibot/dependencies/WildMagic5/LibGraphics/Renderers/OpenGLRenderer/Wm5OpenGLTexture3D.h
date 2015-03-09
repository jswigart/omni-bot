// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/09)

#ifndef WM5OPENGLTEXTURE3D_H
#define WM5OPENGLTEXTURE3D_H

#include "Wm5OpenGLRendererLIB.h"
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
    GLuint mTexture, mInternalFormat, mFormat, mType, mPreviousTexture;
    GLint mNumLevels;
    GLint mNumLevelBytes[Texture::MM_MAX_MIPMAP_LEVELS];
    GLint mDimension[3][Texture::MM_MAX_MIPMAP_LEVELS];
    GLuint mBuffer[Texture::MM_MAX_MIPMAP_LEVELS];
    void* mLockedMemory[Texture::MM_MAX_MIPMAP_LEVELS];
    bool mWriteLock[Texture::MM_MAX_MIPMAP_LEVELS];
};

}

#endif
