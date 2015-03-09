// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/09)

#ifndef WM5OPENGLTEXTURE2D_H
#define WM5OPENGLTEXTURE2D_H

#include "Wm5OpenGLRendererLIB.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class Renderer;

class PdrTexture2D
{
public:
    // Construction and destruction.
    PdrTexture2D (Renderer* renderer, const Texture2D* texture);
    ~PdrTexture2D ();

    // Member access.
    inline GLuint GetTexture () { return mTexture; }

    // Texture operations.
    void Enable (Renderer* renderer, int textureUnit);
    void Disable (Renderer* renderer, int textureUnit);
    void* Lock (int level, Buffer::Locking mode);
    void Unlock (int level);

private:
    GLuint mTexture, mInternalFormat, mFormat, mType, mPreviousTexture;
    GLint mNumLevels;
    GLint mNumLevelBytes[Texture::MM_MAX_MIPMAP_LEVELS];
    GLint mDimension[2][Texture::MM_MAX_MIPMAP_LEVELS];
    GLuint mBuffer[Texture::MM_MAX_MIPMAP_LEVELS];
    void* mLockedMemory[Texture::MM_MAX_MIPMAP_LEVELS];
    bool mWriteLock[Texture::MM_MAX_MIPMAP_LEVELS];
    bool mIsCompressed;
};

}

#endif
