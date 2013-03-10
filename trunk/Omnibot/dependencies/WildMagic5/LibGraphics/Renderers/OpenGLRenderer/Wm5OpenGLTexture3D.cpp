// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/09)

#include "Wm5GraphicsPCH.h"
#include "Wm5OpenGLTexture3D.h"
#include "Wm5OpenGLMapping.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrTexture3D::PdrTexture3D (Renderer*, const Texture3D* texture)
    :
    mPreviousTexture(0)
{
    mInternalFormat = gOGLTextureInternalFormat[texture->GetFormat()];
    mFormat = gOGLTextureFormat[texture->GetFormat()];
    mType = gOGLTextureType[texture->GetFormat()];
    GLuint usage = gOGLBufferUsage[texture->GetUsage()];

    // Create pixel buffer objects to store the texture data.
    mNumLevels = texture->GetNumLevels();
    int level;
    for (level = 0; level < mNumLevels; ++level)
    {
        mNumLevelBytes[level] = texture->GetNumLevelBytes(level);

        glGenBuffers(1, &mBuffer[level]);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mBuffer[level]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, mNumLevelBytes[level], 0,
            usage);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

        mDimension[0][level] = texture->GetDimension(0, level);
        mDimension[1][level] = texture->GetDimension(1, level);
        mDimension[2][level] = texture->GetDimension(2, level);
        mLockedMemory[level] = 0;
        mWriteLock[level] = false;
    }
    for (/**/; level < Texture::MM_MAX_MIPMAP_LEVELS; ++level)
    {
        mBuffer[level] = 0;
        mNumLevelBytes[level] = 0;
        mDimension[0][level] = 0;
        mDimension[1][level] = 0;
        mDimension[2][level] = 0;
        mLockedMemory[level] = 0;
        mWriteLock[level] = false;
    }

    // Create a texture structure.
    glGenTextures(1, &mTexture);
    GLuint previousBind = BindTexture(Shader::ST_3D, mTexture);

    // Create the mipmap level structures.  No image initialization occurs.
    for (level = 0; level < mNumLevels; ++level)
    {
        glTexImage3D(GL_TEXTURE_3D, level, mInternalFormat,
            mDimension[0][level], mDimension[1][level], mDimension[2][level],
            0, mFormat, mType, 0);
    }

    glBindTexture(GL_TEXTURE_3D, previousBind);

    for (level = 0; level < mNumLevels; ++level)
    {
        void* data = Lock(level, Buffer::BL_WRITE_ONLY);
        memcpy(data, texture->GetData(level),
            texture->GetNumLevelBytes(level));
        Unlock(level);
    }
}
//----------------------------------------------------------------------------
PdrTexture3D::~PdrTexture3D ()
{
    for (int level = 0; level < mNumLevels; ++level)
    {
        glDeleteBuffers(1, &mBuffer[level]);
    }
    glDeleteTextures(1, &mTexture);
}
//----------------------------------------------------------------------------
void PdrTexture3D::Enable (Renderer*, int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    mPreviousTexture = BindTexture(Shader::ST_3D, mTexture);
}
//----------------------------------------------------------------------------
void PdrTexture3D::Disable (Renderer*, int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_3D, mPreviousTexture);
}
//----------------------------------------------------------------------------
void* PdrTexture3D::Lock (int level, Buffer::Locking mode)
{
    if (!mLockedMemory[level])
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mBuffer[level]);
        mLockedMemory[level] = glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,
            gOGLBufferLocking[mode]);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

        mWriteLock[level] = (mode != Buffer::BL_READ_ONLY);
    }
    return mLockedMemory[level];
}
//----------------------------------------------------------------------------
void PdrTexture3D::Unlock (int level)
{
    if (mLockedMemory[level])
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mBuffer[level]);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

        if (mWriteLock[level])
        {
            GLuint previousBind = BindTexture(Shader::ST_3D, mTexture);

            glTexSubImage3D(GL_TEXTURE_3D, level, 0, 0, 0,
                mDimension[0][level], mDimension[1][level],
                mDimension[2][level], mFormat, mType, 0);

            glBindTexture(GL_TEXTURE_3D, previousBind);
            mWriteLock[level] = false;
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        mLockedMemory[level] = 0;
    }
}
//----------------------------------------------------------------------------
