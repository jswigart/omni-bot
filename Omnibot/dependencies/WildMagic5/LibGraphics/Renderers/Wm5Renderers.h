// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5RENDERERS_H
#define WM5RENDERERS_H

#include "Wm5Renderer.h"

// This list of files includes private headers.  They need to be exposed to
// the application layer for debugging, because otherwise the linker strips
// the symbols from the application.  Consequently, you cannot watch
// variables in the private classes.

#ifdef WM5_USE_DX9
#include "Wm5Dx9IndexBuffer.h"
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9PixelShader.h"
#include "Wm5Dx9RenderTarget.h"
#include "Wm5Dx9Shader.h"
#include "Wm5Dx9Texture1D.h"
#include "Wm5Dx9Texture2D.h"
#include "Wm5Dx9Texture3D.h"
#include "Wm5Dx9TextureCube.h"
#include "Wm5Dx9VertexBuffer.h"
#include "Wm5Dx9VertexFormat.h"
#include "Wm5Dx9VertexShader.h"
#endif

#ifdef WM5_USE_OPENGL
#include "Wm5OpenGLBitmapFont.h"
#include "Wm5OpenGLIndexBuffer.h"
#include "Wm5OpenGLMapping.h"
#include "Wm5OpenGLPixelShader.h"
#include "Wm5OpenGLRenderTarget.h"
#include "Wm5OpenGLShader.h"
#include "Wm5OpenGLTexture1D.h"
#include "Wm5OpenGLTexture2D.h"
#include "Wm5OpenGLTexture3D.h"
#include "Wm5OpenGLTextureCube.h"
#include "Wm5OpenGLVertexBuffer.h"
#include "Wm5OpenGLVertexFormat.h"
#include "Wm5OpenGLVertexShader.h"
#endif

#endif
