// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/09)

#ifndef WM5OPENGLTEXTURECUBE_H
#define WM5OPENGLTEXTURECUBE_H

#include "Wm5OpenGLRendererLIB.h"
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
    GLuint mTexture, mInternalFormat, mFormat, mType, mPreviousTexture;
    GLint mNumLevels;
    GLint mNumLevelBytes[Texture::MM_MAX_MIPMAP_LEVELS];
    GLint mDimension[2][Texture::MM_MAX_MIPMAP_LEVELS];
    GLuint mBuffer[6][Texture::MM_MAX_MIPMAP_LEVELS];
    void* mLockedMemory[6][Texture::MM_MAX_MIPMAP_LEVELS];
    bool mWriteLock[6][Texture::MM_MAX_MIPMAP_LEVELS];
    bool mIsCompressed;
};

}

#endif
