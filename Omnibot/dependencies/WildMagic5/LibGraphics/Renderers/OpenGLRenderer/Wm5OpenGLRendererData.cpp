// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

#include "Wm5GraphicsPCH.h"
#include "Wm5OpenGLRendererData.h"
#include "Wm5OpenGLMapping.h"
using namespace Wm5;

#ifndef WM5_USE_TEXT_DISPLAY_LIST
//----------------------------------------------------------------------------
// Support for non-display-list fonts.
//----------------------------------------------------------------------------
void RendererData::DrawCharacter (const BitmapFont& font, char c)
{
    const BitmapFontChar* bfc = font.mCharacters[(unsigned int)c];

    // Save unpack state.
    GLint swapBytes, lsbFirst, rowLength, skipRows, skipPixels, alignment;
    glGetIntegerv(GL_UNPACK_SWAP_BYTES, &swapBytes);
    glGetIntegerv(GL_UNPACK_LSB_FIRST, &lsbFirst);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowLength);
    glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skipRows);
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);

    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBitmap(bfc->mXSize, bfc->mYSize, (float)bfc->mXOrigin,
        (float)bfc->mYOrigin, (float)bfc->mXSize, 0.0f,
        (const GLubyte*)bfc->mBitmap);

    // Restore unpack state.
    glPixelStorei(GL_UNPACK_SWAP_BYTES, swapBytes);
    glPixelStorei(GL_UNPACK_LSB_FIRST, lsbFirst);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, skipRows);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, skipPixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}
//----------------------------------------------------------------------------
#endif

//----------------------------------------------------------------------------
// Render state information to avoid redundant state changes.
//----------------------------------------------------------------------------
RendererData::RenderState::RenderState ()
{
    // Initialization must be deferred until an OpenGL context has been
    // created.
}
//----------------------------------------------------------------------------
void RendererData::RenderState::Initialize (const AlphaState* astate,
    const CullState* cstate, const DepthState* dstate,
    const OffsetState* ostate, const StencilState* sstate,
    const WireState* wstate)
{
    // AlphaState
    mAlphaBlendEnabled = astate->BlendEnabled;
    mAlphaSrcBlend = gOGLAlphaSrcBlend[astate->SrcBlend];
    mAlphaDstBlend = gOGLAlphaDstBlend[astate->DstBlend];
    mAlphaCompareEnabled = astate->CompareEnabled;
    mCompareFunction = gOGLAlphaCompare[astate->Compare];
    mAlphaReference = astate->Reference;
    mBlendColor = astate->ConstantColor;

    mAlphaBlendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFunc(mAlphaSrcBlend, GL_ONE_MINUS_SRC_ALPHA);
    mAlphaCompareEnabled ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
    glAlphaFunc(mCompareFunction, mAlphaReference);
    glBlendColor(mBlendColor[0], mBlendColor[1], mBlendColor[2],
        mBlendColor[3]);

    // CullState
    mCullEnabled = cstate->Enabled;
    mCCWOrder = cstate->CCWOrder;

    mCullEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(mCCWOrder ? GL_BACK : GL_FRONT);

    // DepthState
    mDepthEnabled = dstate->Enabled;
    mDepthWriteEnabled = dstate->Writable;
    mDepthCompareFunction = gOGLDepthCompare[dstate->Compare];

    mDepthEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glDepthMask(mDepthWriteEnabled ? GL_TRUE : GL_FALSE);
    glDepthFunc(mDepthCompareFunction);

    // OffsetState
    mFillEnabled = ostate->FillEnabled;
    mLineEnabled = ostate->LineEnabled;
    mPointEnabled = ostate->PointEnabled;
    mOffsetScale = ostate->Scale;
    mOffsetBias = ostate->Bias;

    mFillEnabled ? glEnable(GL_POLYGON_OFFSET_FILL) :
        glDisable(GL_POLYGON_OFFSET_FILL);
    mLineEnabled ? glEnable(GL_POLYGON_OFFSET_LINE) :
        glDisable(GL_POLYGON_OFFSET_LINE);
    mPointEnabled ? glEnable(GL_POLYGON_OFFSET_POINT) :
        glDisable(GL_POLYGON_OFFSET_POINT);
    glPolygonOffset(mOffsetScale, mOffsetBias);

    // StencilState
    mStencilEnabled = sstate->Enabled;
    mStencilCompareFunction = gOGLStencilCompare[sstate->Compare];
    mStencilReference = sstate->Reference;
    mStencilMask = sstate->Mask;
    mStencilWriteMask = sstate->WriteMask;
    mStencilOnFail = gOGLStencilOperation[sstate->OnFail];
    mStencilOnZFail = gOGLStencilOperation[sstate->OnZFail];
    mStencilOnZPass = gOGLStencilOperation[sstate->OnZPass];

    mStencilEnabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    glStencilFunc(mStencilCompareFunction, mStencilReference, mStencilMask);
    glStencilMask(mStencilWriteMask);
    glStencilOp(mStencilOnFail, mStencilOnZFail, mStencilOnZPass);

    // WireState
    mWireEnabled = wstate->Enabled;

    glPolygonMode(GL_FRONT_AND_BACK, mWireEnabled ? GL_LINE : GL_FILL);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Sampler state information to avoid redundant state changes.
//----------------------------------------------------------------------------
RendererData::SamplerState::SamplerState ()
    :
    mBorderColor(0.0f, 0.0f, 0.0f, 0.0f)
{
    mAnisotropy = 1.0f;
    mLodBias = 0.0f;
    mMagFilter = GL_LINEAR;
    mMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    mWrap[0] = GL_REPEAT;
    mWrap[1] = GL_REPEAT;
    mWrap[2] = GL_REPEAT;
}
//----------------------------------------------------------------------------
void RendererData::SamplerState::GetCurrent (GLenum target)
{
    glGetTexParameterfv(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, &mAnisotropy);
    glGetTexEnvfv(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, &mLodBias);
    glGetTexParameteriv(target, GL_TEXTURE_MAG_FILTER, &mMagFilter);
    glGetTexParameteriv(target, GL_TEXTURE_MIN_FILTER, &mMinFilter);
    glGetTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, mBorderColor);
    glGetTexParameteriv(target, GL_TEXTURE_WRAP_S, &mWrap[0]);
    glGetTexParameteriv(target, GL_TEXTURE_WRAP_T, &mWrap[1]);
    glGetTexParameteriv(target, GL_TEXTURE_WRAP_R, &mWrap[2]);
}
//----------------------------------------------------------------------------
