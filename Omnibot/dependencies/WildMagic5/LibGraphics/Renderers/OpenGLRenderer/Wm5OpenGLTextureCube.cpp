// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/09)

#include "Wm5GraphicsPCH.h"
#include "Wm5OpenGLTextureCube.h"
#include "Wm5OpenGLMapping.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrTextureCube::PdrTextureCube (Renderer*, const TextureCube* texture)
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
        mDimension[0][level] = texture->GetDimension(0, level);
        mDimension[1][level] = texture->GetDimension(1, level);
    }
    for (/**/; level < Texture::MM_MAX_MIPMAP_LEVELS; ++level)
    {
        mNumLevelBytes[level] = 0;
        mDimension[0][level] = 0;
        mDimension[1][level] = 0;
    }

    int face;
    for (face = 0; face < 6; ++face)
    {
        for (level = 0; level < mNumLevels; ++level)
        {
            glGenBuffers(1, &mBuffer[face][level]);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mBuffer[face][level]);
            glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, mNumLevelBytes[level], 0,
                usage);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

            mLockedMemory[face][level] = 0;
        }
        for (/**/; level < Texture::MM_MAX_MIPMAP_LEVELS; ++level)
        {
            mBuffer[face][level] = 0;
            mLockedMemory[face][level] = 0;
        }
    }

    // Create a texture structure.
    glGenTextures(1, &mTexture);
    GLuint previousBind = BindTexture(Shader::ST_CUBE, mTexture);

    // Create the mipmap level structures.  No image initialization occurs.
    mIsCompressed = texture->IsCompressed();
    if (mIsCompressed)
    {
        for (face = 0; face < 6; ++face)
        {
            for (level = 0; level < mNumLevels; ++level)
            {
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                    level, mInternalFormat, mDimension[0][level],
                    mDimension[1][level], 0, mNumLevelBytes[level], 0);
            }
        }
    }
    else
    {
        for (face = 0; face < 6; ++face)
        {
            for (level = 0; level < mNumLevels; ++level)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level,
                    mInternalFormat, mDimension[0][level],
                    mDimension[1][level], 0, mFormat, mType, 0);
            }
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, previousBind);

    for (face = 0; face < 6; ++face)
    {
        for (level = 0; level < mNumLevels; ++level)
        {
            void* data = Lock(face, level, Buffer::BL_WRITE_ONLY);
            memcpy(data, texture->GetData(face, level),
                texture->GetNumLevelBytes(level));
            Unlock(face, level);
        }
    }
}
//----------------------------------------------------------------------------
PdrTextureCube::~PdrTextureCube ()
{
    for (int face = 0; face < 6; ++face)
    {
        for (int level = 0; level < mNumLevels; ++level)
        {
            glDeleteBuffers(1, &mBuffer[face][level]);
        }
    }
    glDeleteTextures(1, &mTexture);
}
//----------------------------------------------------------------------------
void PdrTextureCube::Enable (Renderer*, int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    mPreviousTexture = BindTexture(Shader::ST_CUBE, mTexture);
}
//----------------------------------------------------------------------------
void PdrTextureCube::Disable (Renderer*, int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mPreviousTexture);
}
//----------------------------------------------------------------------------
void* PdrTextureCube::Lock (int face, int level, Buffer::Locking mode)
{
    if (!mLockedMemory[face][level])
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mBuffer[face][level]);
        mLockedMemory[face][level] = glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,
            gOGLBufferLocking[mode]);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

        mWriteLock[face][level] = (mode != Buffer::BL_READ_ONLY);
    }
    return mLockedMemory[face][level];
}
//----------------------------------------------------------------------------
void PdrTextureCube::Unlock (int face, int level)
{
    if (mLockedMemory[face][level])
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mBuffer[face][level]);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

        if (mWriteLock[face][level])
        {
            GLuint previousBind = BindTexture(Shader::ST_CUBE, mTexture);

            if (mIsCompressed)
            {
                glCompressedTexSubImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, 0, 0,
                    mDimension[0][level], mDimension[1][level],
                    mInternalFormat, mNumLevelBytes[level], 0);
            }
            else
            {
                glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, 0,
                    0, mDimension[0][level], mDimension[1][level], mFormat,
                    mType, 0);
            }

            glBindTexture(GL_TEXTURE_CUBE_MAP, previousBind);
            mWriteLock[face][level] = false;
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        mLockedMemory[face][level] = 0;
    }
}
//----------------------------------------------------------------------------
