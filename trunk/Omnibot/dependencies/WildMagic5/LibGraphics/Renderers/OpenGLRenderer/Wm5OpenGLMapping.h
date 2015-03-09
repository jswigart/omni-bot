// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/09)

#ifndef WM5OPENGLMAPPING_H
#define WM5OPENGLMAPPING_H

#include "Wm5OpenGLRendererLIB.h"
#include "Wm5AlphaState.h"
#include "Wm5Buffer.h"
#include "Wm5CullState.h"
#include "Wm5DepthState.h"
#include "Wm5Shader.h"
#include "Wm5StencilState.h"
#include "Wm5Texture.h"
#include "Wm5VertexFormat.h"
#include "Wm5Visual.h"

namespace Wm5
{
extern GLenum gOGLAlphaSrcBlend[AlphaState::SBM_QUANTITY];
extern GLenum gOGLAlphaDstBlend[AlphaState::DBM_QUANTITY];
extern GLenum gOGLAlphaCompare[AlphaState::CM_QUANTITY];
extern GLenum gOGLDepthCompare[DepthState::CM_QUANTITY];
extern GLenum gOGLStencilCompare[StencilState::CM_QUANTITY];
extern GLenum gOGLStencilOperation[StencilState::OT_QUANTITY];
extern GLint  gOGLAttributeChannels[VertexFormat::AT_QUANTITY];
extern GLuint gOGLAttributeType[VertexFormat::AT_QUANTITY];
extern GLuint gOGLBufferLocking[Buffer::BL_QUANTITY];
extern GLuint gOGLBufferUsage[Buffer::BU_QUANTITY];
extern GLint  gOGLMinFilter[Shader::SF_QUANTITY];
extern GLuint gOGLTextureFormat[Texture::TF_QUANTITY];
extern GLuint gOGLTextureInternalFormat[Texture::TF_QUANTITY];
extern GLuint gOGLTextureTarget[Shader::ST_QUANTITY];
extern GLuint gOGLTextureTargetBinding[Shader::ST_QUANTITY];
extern GLuint gOGLTextureType[Texture::TF_QUANTITY];
extern GLint  gOGLWrapMode[Shader::SC_QUANTITY];
extern GLenum gOGLPrimitiveType[Visual::PT_MAX_QUANTITY];

// Support for binding and rebinding.  BindTexture binds the input texture
// to the specified target and returns the previously bound texture for
// that target.  GetBoundTexture returns the currently bound texture for
// the specified target.
GLuint BindTexture (Shader::SamplerType target, GLuint texture);
GLuint GetBoundTexture (Shader::SamplerType target);
}

#endif
