// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/07/09)

#include "Wm5GraphicsPCH.h"
#include "Wm5OpenGLMapping.h"

namespace Wm5
{

GLenum gOGLAlphaSrcBlend[AlphaState::SBM_QUANTITY] =
{
    GL_ZERO,                        // SBM_ZERO
    GL_ONE,                         // SBM_ONE
    GL_DST_COLOR,                   // SBM_DST_COLOR
    GL_ONE_MINUS_DST_COLOR,         // SBM_ONE_MINUS_DST_COLOR
    GL_SRC_ALPHA,                   // SBM_SRC_ALPHA
    GL_ONE_MINUS_SRC_ALPHA,         // SBM_ONE_MINUS_SRC_ALPHA
    GL_DST_ALPHA,                   // SBM_DST_ALPHA
    GL_ONE_MINUS_DST_ALPHA,         // SBM_ONE_MINUS_DST_ALPHA
    GL_SRC_ALPHA_SATURATE,          // SBM_SRC_ALPHA_SATURATE
    GL_CONSTANT_COLOR,              // SBM_CONSTANT_COLOR
    GL_ONE_MINUS_CONSTANT_COLOR,    // SBM_ONE_MINUS_CONSTANT_COLOR
    GL_CONSTANT_ALPHA,              // SBM_CONSTANT_ALPHA
    GL_ONE_MINUS_CONSTANT_ALPHA     // SBM_ONE_MINUS_CONSTANT_ALPHA
};

GLenum gOGLAlphaDstBlend[AlphaState::DBM_QUANTITY] =
{
    GL_ZERO,                        // DBM_ZERO
    GL_ONE,                         // DBM_ONE
    GL_SRC_COLOR,                   // DBM_SRC_COLOR
    GL_ONE_MINUS_SRC_COLOR,         // DBM_ONE_MINUS_SRC_COLOR
    GL_SRC_ALPHA,                   // DBM_SRC_ALPHA
    GL_ONE_MINUS_SRC_ALPHA,         // DBM_ONE_MINUS_SRC_ALPHA
    GL_DST_ALPHA,                   // DBM_DST_ALPHA
    GL_ONE_MINUS_DST_ALPHA,         // DBM_ONE_MINUS_DST_ALPHA
    GL_CONSTANT_COLOR,              // DBM_CONSTANT_COLOR
    GL_ONE_MINUS_CONSTANT_COLOR,    // DBM_ONE_MINUS_CONSTANT_COLOR
    GL_CONSTANT_ALPHA,              // DBM_CONSTANT_ALPHA
    GL_ONE_MINUS_CONSTANT_ALPHA     // DBM_ONE_MINUS_CONSTANT_ALPHA
};

GLenum gOGLAlphaCompare[AlphaState::CM_QUANTITY] =
{
    GL_NEVER,       // CM_NEVER
    GL_LESS,        // CM_LESS
    GL_EQUAL,       // CM_EQUAL
    GL_LEQUAL,      // CM_LEQUAL
    GL_GREATER,     // CM_GREATER
    GL_NOTEQUAL,    // CM_NOTEQUAL
    GL_GEQUAL,      // CM_GEQUAL
    GL_ALWAYS       // CM_ALWAYS
};

GLenum gOGLDepthCompare[DepthState::CM_QUANTITY] = 
{
    GL_NEVER,       // CM_NEVER
    GL_LESS,        // CM_LESS
    GL_EQUAL,       // CM_EQUAL
    GL_LEQUAL,      // CM_LEQUAL
    GL_GREATER,     // CM_GREATER
    GL_NOTEQUAL,    // CM_NOTEQUAL
    GL_GEQUAL,      // CM_GEQUAL
    GL_ALWAYS       // CM_ALWAYS
};

GLenum gOGLStencilCompare[StencilState::CM_QUANTITY] =
{
    GL_NEVER,       // CM_NEVER
    GL_LESS,        // CM_LESS
    GL_EQUAL,       // CM_EQUAL
    GL_LEQUAL,      // CM_LEQUAL
    GL_GREATER,     // CM_GREATER
    GL_NOTEQUAL,    // CM_NOTEQUAL
    GL_GEQUAL,      // CM_GEQUAL
    GL_ALWAYS       // CM_ALWAYS
};

GLenum gOGLStencilOperation[StencilState::OT_QUANTITY] =
{
    GL_KEEP,    // OT_KEEP
    GL_ZERO,    // OT_ZERO
    GL_REPLACE, // OT_REPLACE
    GL_INCR,    // OT_INCREMENT
    GL_DECR,    // OT_DECREMENT
    GL_INVERT   // OT_INVERT
};

GLint gOGLAttributeChannels[VertexFormat::AT_QUANTITY] =
{
    0,  // AT_NONE (unsupported)
    1,  // AT_FLOAT1
    2,  // AT_FLOAT2
    3,  // AT_FLOAT3
    4,  // AT_FLOAT4
    1,  // AT_HALF1
    2,  // AT_HALF2
    3,  // AT_HALF3
    4,  // AT_HALF4
    4,  // AT_UBYTE4
    1,  // AT_SHORT1
    2,  // AT_SHORT2
    4   // AT_SHORT4
};

GLuint gOGLAttributeType[VertexFormat::AT_QUANTITY] =
{
    0,                          // AT_NONE (unsupported)
    GL_FLOAT,                   // AT_FLOAT1
    GL_FLOAT,                   // AT_FLOAT2
    GL_FLOAT,                   // AT_FLOAT3
    GL_FLOAT,                   // AT_FLOAT4
    GL_HALF_FLOAT_ARB,          // AT_HALF1
    GL_HALF_FLOAT_ARB,          // AT_HALF2
    GL_HALF_FLOAT_ARB,          // AT_HALF3
    GL_HALF_FLOAT_ARB,          // AT_HALF4
    GL_UNSIGNED_BYTE,           // AT_UBYTE4
    GL_SHORT,                   // AT_SHORT1
    GL_SHORT,                   // AT_SHORT2
    GL_SHORT                    // AT_SHORT4
};

GLuint gOGLBufferLocking[Buffer::BL_QUANTITY] =
{
    GL_READ_ONLY,   // BL_READ_ONLY
    GL_WRITE_ONLY,  // BL_WRITE_ONLY
    GL_READ_WRITE   // BL_READ_WRITE
};

GLuint gOGLBufferUsage[Buffer::BU_QUANTITY] =
{
    GL_STATIC_DRAW,     // BU_STATIC
    GL_DYNAMIC_DRAW,    // BU_DYNAMIC
    GL_DYNAMIC_DRAW,    // BU_RENDERTARGET
    GL_DYNAMIC_DRAW,    // BU_DEPTHSTENCIL
    GL_DYNAMIC_DRAW     // BU_TEXTURE
};

GLint gOGLMinFilter[Shader::SF_QUANTITY] =
{
    0,                          // SF_NONE
    GL_NEAREST,                 // SF_NEAREST
    GL_LINEAR,                  // SF_LINEAR
    GL_NEAREST_MIPMAP_NEAREST,  // SF_NEAREST_NEAREST
    GL_NEAREST_MIPMAP_LINEAR,   // SF_NEAREST_LINEAR
    GL_LINEAR_MIPMAP_NEAREST,   // SF_LINEAR_NEAREST
    GL_LINEAR_MIPMAP_LINEAR     // SF_LINEAR_LINEAR
};

GLuint gOGLTextureInternalFormat[Texture::TF_QUANTITY] =
{
    0,                                  // TF_NONE
    GL_RGB5,                            // TF_R5G6B5
    GL_RGB5_A1,                         // TF_A1R5G5B5
    GL_RGBA4,                           // TF_A4R4G4B4
    GL_ALPHA8,                          // TF_A8
    GL_LUMINANCE8,                      // TF_L8
    GL_LUMINANCE8_ALPHA8,               // TF_A8L8
    GL_RGB8,                            // TF_R8G8B8
    GL_RGBA8,                           // TF_A8R8G8B8
    GL_RGBA8,                           // TF_A8B8G8R8
    GL_LUMINANCE16,                     // TF_L16
    GL_RG16,                            // TF_G16R16
    GL_RGBA16,                          // TF_A16B16G16R16
    GL_R16F,                            // TF_R16F
    GL_RG16F,                           // TF_G16R16F
    GL_RGBA16F_ARB,                     // TF_A16B16G16R16F
    GL_R32F,                            // TF_R32F
    GL_RG32F,                           // TF_G32R32F
    GL_RGBA32F_ARB,                     // TF_A32B32G32R32F
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,   // TF_DXT1
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,   // TF_DXT3
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // TF_DXT5
    GL_DEPTH24_STENCIL8_EXT             // TF_D24S8
};

GLuint gOGLTextureFormat[Texture::TF_QUANTITY] =
{
    0,                                  // TF_NONE
    GL_BGR,                             // TF_R5G6B5
    GL_RGBA,                            // TF_A1R5G5B5
    GL_RGBA,                            // TF_A4R4G4B4
    GL_ALPHA,                           // TF_A8
    GL_LUMINANCE,                       // TF_L8
    GL_LUMINANCE_ALPHA,                 // TF_A8L8
    GL_BGR,                             // TF_R8G8B8
    GL_BGRA,                            // TF_A8R8G8B8
    GL_RGBA,                            // TF_A8B8G8R8
    GL_LUMINANCE,                       // TF_L16
    GL_RG,                              // TF_G16R16
    GL_RGBA,                            // TF_A16B16G16R16
    GL_RED,                             // TF_R16F
    GL_RG,                              // TF_G16R16F
    GL_RGBA,                            // TF_A16B16G16R16F
    GL_RED,                             // TF_R32F
    GL_RG,                              // TF_G32R32F
    GL_RGBA,                            // TF_A32B32G32R32F
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,   // TF_DXT1
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,   // TF_DXT3
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // TF_DXT5
    GL_DEPTH_STENCIL_EXT                // TF_D24S8
};

GLuint gOGLTextureTarget[Shader::ST_QUANTITY] =
{
    0,                  // ST_NONE
    GL_TEXTURE_1D,      // ST_1D
    GL_TEXTURE_2D,      // ST_2D
    GL_TEXTURE_3D,      // ST_3D
    GL_TEXTURE_CUBE_MAP // ST_CUBE
};

GLuint gOGLTextureTargetBinding[Shader::ST_QUANTITY] =
{
    0,                          // ST_NONE
    GL_TEXTURE_BINDING_1D,      // ST_1D
    GL_TEXTURE_BINDING_2D,      // ST_2D
    GL_TEXTURE_BINDING_3D,      // ST_3D
    GL_TEXTURE_BINDING_CUBE_MAP // ST_CUBE
};

GLuint gOGLTextureType[Texture::TF_QUANTITY] =
{
    0,                              // TF_NONE
    GL_UNSIGNED_SHORT_5_6_5_REV,    // TF_R5G6B5
    GL_UNSIGNED_SHORT_1_5_5_5_REV,  // TF_A1R5G5B5
    GL_UNSIGNED_SHORT_4_4_4_4_REV,  // TF_A4R4G4B4
    GL_UNSIGNED_BYTE,               // TF_A8
    GL_UNSIGNED_BYTE,               // TF_L8
    GL_UNSIGNED_BYTE,               // TF_A8L8
    GL_UNSIGNED_BYTE,               // TF_R8G8B8
    GL_UNSIGNED_BYTE,               // TF_A8R8G8B8
    GL_UNSIGNED_BYTE,               // TF_A8B8G8R8
    GL_UNSIGNED_SHORT,              // TF_L16
    GL_UNSIGNED_SHORT,              // TF_G16R16
    GL_UNSIGNED_SHORT,              // TF_A16B16G16R16
    GL_HALF_FLOAT_ARB,              // TF_R16F
    GL_HALF_FLOAT_ARB,              // TF_G16R16F
    GL_HALF_FLOAT_ARB,              // TF_A16B16G16R16F
    GL_FLOAT,                       // TF_R32F
    GL_FLOAT,                       // TF_G32R32F
    GL_FLOAT,                       // TF_A32B32G32R32F
    GL_NONE,                        // TF_DXT1 (not needed)
    GL_NONE,                        // TF_DXT3 (not needed)
    GL_NONE,                        // TF_DXT5 (not needed)
    GL_UNSIGNED_INT_24_8_EXT        // TF_D24S8
};

GLint gOGLWrapMode[Shader::SC_QUANTITY] =
{
    GL_CLAMP,           // SC_NONE
    GL_CLAMP,           // SC_CLAMP
    GL_REPEAT,          // SC_REPEAT
    GL_MIRRORED_REPEAT, // SC_MIRRORED_REPEAT
    GL_CLAMP_TO_BORDER, // SC_CLAMP_BORDER
    GL_CLAMP_TO_EDGE,   // SC_CLAMP_EDGE
};

GLenum gOGLPrimitiveType[Visual::PT_MAX_QUANTITY] =
{
    0,                  // PT_NONE (not used)
    GL_POINTS,          // PT_POLYPOINT
    GL_LINES,           // PT_POLYSEGMENTS_DISJOINT
    GL_LINE_STRIP,      // PT_POLYSEGMENTS_CONTIGUOUS
    0,                  // PT_TRIANGLES (not used)
    GL_TRIANGLES,       // PT_TRIMESH
    GL_TRIANGLE_STRIP,  // PT_TRISTRIP
    GL_TRIANGLE_FAN     // PT_TRIFAN
};

//----------------------------------------------------------------------------
GLuint BindTexture (Shader::SamplerType target, GLuint texture)
{
    GLint current = 0;
    glGetIntegerv(gOGLTextureTargetBinding[target], &current);
    glBindTexture(gOGLTextureTarget[target], texture);
    return (GLuint)current;
}
//----------------------------------------------------------------------------
GLuint GetBoundTexture (Shader::SamplerType target)
{
    GLint current = 0;
    glGetIntegerv(gOGLTextureTargetBinding[target], &current);
    return (GLuint)current;
}
//----------------------------------------------------------------------------
}
