// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/06/27)

#include "Wm5GraphicsPCH.h"
#include "Wm5Renderer.h"
#include "Wm5OpenGLBitmapFont.h"
#include "Wm5OpenGLMapping.h"
#include "Wm5OpenGLRendererData.h"
#include "Wm5Polypoint.h"
#include "Wm5Polysegment.h"
#include "Wm5TriFan.h"
#include "Wm5TriStrip.h"
using namespace Wm5;

//----------------------------------------------------------------------------
// Visual state management.
//----------------------------------------------------------------------------
void Renderer::SetAlphaState (const AlphaState* alphaState)
{
    if (!mOverrideAlphaState)
    {
        mAlphaState = alphaState;
    }
    else
    {
        mAlphaState = mOverrideAlphaState;
    }

    if (mAlphaState->BlendEnabled)
    {
        if (!mData->mCurrentRS.mAlphaBlendEnabled)
        {
            mData->mCurrentRS.mAlphaBlendEnabled = true;
            glEnable(GL_BLEND);
        }

        GLenum srcBlend = gOGLAlphaSrcBlend[mAlphaState->SrcBlend];
        GLenum dstBlend = gOGLAlphaDstBlend[mAlphaState->DstBlend];
        if (srcBlend != mData->mCurrentRS.mAlphaSrcBlend
        ||  dstBlend != mData->mCurrentRS.mAlphaDstBlend)
        {
            mData->mCurrentRS.mAlphaSrcBlend = srcBlend;
            mData->mCurrentRS.mAlphaDstBlend = dstBlend;
            glBlendFunc(srcBlend, dstBlend);
        }

        if (mAlphaState->ConstantColor != mData->mCurrentRS.mBlendColor)
        {
            mData->mCurrentRS.mBlendColor = mAlphaState->ConstantColor;
            glBlendColor(
                mData->mCurrentRS.mBlendColor[0],
                mData->mCurrentRS.mBlendColor[1],
                mData->mCurrentRS.mBlendColor[1],
                mData->mCurrentRS.mBlendColor[3]);
        }
    }
    else
    {
        if (mData->mCurrentRS.mAlphaBlendEnabled)
        {
            mData->mCurrentRS.mAlphaBlendEnabled = false;
            glDisable(GL_BLEND);
        }
    }

    if (mAlphaState->CompareEnabled)
    {
        if (!mData->mCurrentRS.mAlphaCompareEnabled)
        {
            mData->mCurrentRS.mAlphaCompareEnabled = true;
            glEnable(GL_ALPHA_TEST);
        }

        GLenum compare = gOGLAlphaCompare[mAlphaState->Compare];
        float reference = mAlphaState->Reference;
        if (compare != mData->mCurrentRS.mCompareFunction
        ||  reference != mData->mCurrentRS.mAlphaReference)
        {
            mData->mCurrentRS.mCompareFunction = compare;
            mData->mCurrentRS.mAlphaReference = reference;
            glAlphaFunc(compare, reference);
        }
    }
    else
    {
        if (mData->mCurrentRS.mAlphaCompareEnabled)
        {
            mData->mCurrentRS.mAlphaCompareEnabled = false;
            glDisable(GL_ALPHA_TEST);
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetCullState (const CullState* cullState)
{
    if (!mOverrideCullState)
    {
        mCullState = cullState;
    }
    else
    {
        mCullState = mOverrideCullState;
    }

    if (mCullState->Enabled)
    {
        if (!mData->mCurrentRS.mCullEnabled)
        {
            mData->mCurrentRS.mCullEnabled = true;
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
        }

        bool order = mCullState->CCWOrder;
        if (mReverseCullOrder)
        {
            order = !order;
        }

        if (order != mData->mCurrentRS.mCCWOrder)
        {
            mData->mCurrentRS.mCCWOrder = order;
            if (mData->mCurrentRS.mCCWOrder)
            {
                glCullFace(GL_BACK);
            }
            else
            {
                glCullFace(GL_FRONT);
            }
        }
    }
    else
    {
        if (mData->mCurrentRS.mCullEnabled)
        {
            mData->mCurrentRS.mCullEnabled = false;
            glDisable(GL_CULL_FACE);
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetDepthState (const DepthState* depthState)
{
    if (!mOverrideDepthState)
    {
        mDepthState = depthState;
    }
    else
    {
        mDepthState = mOverrideDepthState;
    }

    if (mDepthState->Enabled)
    {
        if (!mData->mCurrentRS.mDepthEnabled)
        {
            mData->mCurrentRS.mDepthEnabled = true;
            glEnable(GL_DEPTH_TEST);
        }

        GLenum compare = gOGLDepthCompare[mDepthState->Compare];
        if (compare != mData->mCurrentRS.mDepthCompareFunction)
        {
            mData->mCurrentRS.mDepthCompareFunction = compare;
            glDepthFunc(compare);
        }
    }
    else
    {
        if (mData->mCurrentRS.mDepthEnabled)
        {
            mData->mCurrentRS.mDepthEnabled = false;
            glDisable(GL_DEPTH_TEST);
        }
    }

    if (mDepthState->Writable)
    {
        if (!mData->mCurrentRS.mDepthWriteEnabled)
        {
            mData->mCurrentRS.mDepthWriteEnabled = true;
            glDepthMask(GL_TRUE);
        }
    }
    else
    {
        if (mData->mCurrentRS.mDepthWriteEnabled)
        {
            mData->mCurrentRS.mDepthWriteEnabled = false;
            glDepthMask(GL_FALSE);
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetOffsetState (const OffsetState* offsetState)
{
    if (!mOverrideOffsetState)
    {
        mOffsetState = offsetState;
    }
    else
    {
        mOffsetState = mOverrideOffsetState;
    }

    if (mOffsetState->FillEnabled)
    {
        if (!mData->mCurrentRS.mFillEnabled)
        {
            mData->mCurrentRS.mFillEnabled = true;
            glEnable(GL_POLYGON_OFFSET_FILL);
        }
    }
    else
    {
        if (mData->mCurrentRS.mFillEnabled)
        {
            mData->mCurrentRS.mFillEnabled = false;
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

    if (mOffsetState->LineEnabled)
    {
        if (!mData->mCurrentRS.mLineEnabled)
        {
            mData->mCurrentRS.mLineEnabled = true;
            glEnable(GL_POLYGON_OFFSET_LINE);
        }
    }
    else
    {
        if (mData->mCurrentRS.mLineEnabled)
        {
            mData->mCurrentRS.mLineEnabled = false;
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
    }

    if (mOffsetState->PointEnabled)
    {
        if (!mData->mCurrentRS.mPointEnabled)
        {
            mData->mCurrentRS.mPointEnabled = true;
            glEnable(GL_POLYGON_OFFSET_POINT);
        }
    }
    else
    {
        if (mData->mCurrentRS.mPointEnabled)
        {
            mData->mCurrentRS.mPointEnabled = false;
            glDisable(GL_POLYGON_OFFSET_POINT);
        }
    }

    if (mOffsetState->Scale != mData->mCurrentRS.mOffsetScale
    ||  mOffsetState->Bias != mData->mCurrentRS.mOffsetBias)
    {
        mData->mCurrentRS.mOffsetScale = mOffsetState->Scale;
        mData->mCurrentRS.mOffsetBias = mOffsetState->Bias;
        glPolygonOffset(mOffsetState->Scale, mOffsetState->Bias);
    }
}
//----------------------------------------------------------------------------
void Renderer::SetStencilState (const StencilState* stencilState)
{
    if (!mOverrideStencilState)
    {
        mStencilState = stencilState;
    }
    else
    {
        mStencilState = mOverrideStencilState;
    }

    if (mStencilState->Enabled)
    {
        if (!mData->mCurrentRS.mStencilEnabled)
        {
            mData->mCurrentRS.mStencilEnabled = true;
            glEnable(GL_STENCIL_TEST);
        }

        GLenum compare = gOGLStencilCompare[mStencilState->Compare];
        if (compare != mData->mCurrentRS.mStencilCompareFunction
        ||  mStencilState->Reference != mData->mCurrentRS.mStencilReference
        ||  mStencilState->Mask != mData->mCurrentRS.mStencilMask)
        {
            mData->mCurrentRS.mStencilCompareFunction = compare;
            mData->mCurrentRS.mStencilReference = mStencilState->Reference;
            mData->mCurrentRS.mStencilMask = mStencilState->Mask;
            glStencilFunc(compare, mStencilState->Reference,
                mStencilState->Mask);
        }

        if (mStencilState->WriteMask != mData->mCurrentRS.mStencilWriteMask)
        {
            mData->mCurrentRS.mStencilWriteMask = mStencilState->WriteMask;
            glStencilMask(mStencilState->WriteMask);
        }

        GLenum onFail = gOGLStencilOperation[mStencilState->OnFail];
        GLenum onZFail = gOGLStencilOperation[mStencilState->OnZFail];
        GLenum onZPass = gOGLStencilOperation[mStencilState->OnZPass];
        if (onFail != mData->mCurrentRS.mStencilOnFail
        ||  onZFail != mData->mCurrentRS.mStencilOnZFail
        ||  onZPass != mData->mCurrentRS.mStencilOnZPass)
        {
            mData->mCurrentRS.mStencilOnFail = onFail;
            mData->mCurrentRS.mStencilOnZFail = onZFail;
            mData->mCurrentRS.mStencilOnZPass = onZPass;
            glStencilOp(onFail, onZFail, onZPass);
        }
    }
    else
    {
        if (mData->mCurrentRS.mStencilEnabled)
        {
            mData->mCurrentRS.mStencilEnabled = false;
            glDisable(GL_STENCIL_TEST);
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetWireState (const WireState* wireState)
{
    if (!mOverrideWireState)
    {
        mWireState = wireState;
    }
    else
    {
        mWireState = mOverrideWireState;
    }

    if (mWireState->Enabled)
    {
        if (!mData->mCurrentRS.mWireEnabled)
        {
            mData->mCurrentRS.mWireEnabled = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
    }
    else
    {
        if (mData->mCurrentRS.mWireEnabled)
        {
            mData->mCurrentRS.mWireEnabled = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Viewport management.
//----------------------------------------------------------------------------
void Renderer::SetViewport (int xPosition, int yPosition, int width,
    int height)
{
    glViewport(xPosition, yPosition, width, height);
}
//----------------------------------------------------------------------------
void Renderer::GetViewport (int& xPosition, int& yPosition,
    int& width, int& height) const
{
    int param[4];
    glGetIntegerv(GL_VIEWPORT, param);

    xPosition = param[0];
    yPosition = param[1];
    width = param[2];
    height = param[3];
}
//----------------------------------------------------------------------------
void Renderer::SetDepthRange (float zMin, float zMax)
{
    glDepthRange((GLclampd)zMin, (GLclampd)zMax);
}
//----------------------------------------------------------------------------
void Renderer::GetDepthRange (float& zMin, float& zMax) const
{
    GLclampd param[2];
    glGetDoublev(GL_DEPTH_RANGE, param);

    zMin = (float)param[0];
    zMax = (float)param[1];
}
//----------------------------------------------------------------------------
void Renderer::Resize (int width, int height)
{
    mWidth = width;
    mHeight = height;

    int param[4];
    glGetIntegerv(GL_VIEWPORT, param);
    glViewport(param[0], param[1], width, height);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for clearing the color, depth, and stencil buffers.
//----------------------------------------------------------------------------
void Renderer::ClearColorBuffer ()
{
    glClearColor(mClearColor[0], mClearColor[1], mClearColor[2],
        mClearColor[3]);

    glClear(GL_COLOR_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void Renderer::ClearDepthBuffer ()
{
    glClearDepth((GLclampd)mClearDepth);

    glClear(GL_DEPTH_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void Renderer::ClearStencilBuffer ()
{
    glClearStencil((GLint)mClearStencil);

    glClear(GL_STENCIL_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void Renderer::ClearBuffers ()
{
    glClearColor(mClearColor[0], mClearColor[1], mClearColor[2],
        mClearColor[3]);

    glClearDepth((GLclampd)mClearDepth);

    glClearStencil((GLint)mClearStencil);

    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void Renderer::ClearColorBuffer (int x, int y, int w, int h)
{
    glClearColor(mClearColor[0], mClearColor[1], mClearColor[2],
        mClearColor[3]);

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void Renderer::ClearDepthBuffer (int x, int y, int w, int h)
{
    glClearDepth((GLclampd)mClearDepth);

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void Renderer::ClearStencilBuffer (int x, int y, int w, int h)
{
    glClearStencil((GLint)mClearStencil);

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glClear(GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void Renderer::ClearBuffers (int x, int y, int w, int h)
{
    glClearColor(mClearColor[0], mClearColor[1], mClearColor[2],
        mClearColor[3]);

    glClearDepth((GLclampd)mClearDepth);

    glClearStencil((GLint)mClearStencil);

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for masking color channels.
//----------------------------------------------------------------------------
void Renderer::SetColorMask (bool allowRed, bool allowGreen,
    bool allowBlue, bool allowAlpha)
{
    mAllowRed = allowRed;
    mAllowGreen = allowGreen;
    mAllowBlue = allowBlue;
    mAllowAlpha = allowAlpha;

    GLboolean rMask = (mAllowRed ? 1 : 0);
    GLboolean gMask = (mAllowGreen ? 1 : 0);
    GLboolean bMask = (mAllowBlue ? 1 : 0);
    GLboolean aMask = (mAllowAlpha ? 1 : 0);
    glColorMask(rMask, gMask, bMask, aMask);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for predraw and postdraw semantics.
//----------------------------------------------------------------------------
bool Renderer::PreDraw ()
{
    return true;
}
//----------------------------------------------------------------------------
void Renderer::PostDraw ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Drawing routines.
//----------------------------------------------------------------------------
void Renderer::Draw (const unsigned char* screenBuffer, bool reflectY)
{
    if (!screenBuffer)
    {
        assertion(false, "Incoming screen buffer is null.\n");
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)mWidth, 0.0, (double)mHeight, 0.0, 1.0);
    glRasterPos3f(0.0, 0.0, 0.0);
    if (!reflectY)
    {
        // Set raster position to window coord (0,H-1).  The hack here avoids
        // problems with invalid raster positions which would cause
        // glDrawPixels not to execute.  OpenGL uses right-handed screen
        // coordinates, so using (0,H-1) as the raster position followed by
        // glPixelZoom(1,-1) tells OpenGL to draw the screen in left-handed
        // coordinates starting at the top row of the screen and finishing
        // at the bottom row.
        GLubyte bitmap[1] = {0};
        glBitmap(0, 0, 0.0f, 0.0f, 0.0f, (float)mHeight, bitmap);
    }
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    if (!reflectY)
    {
        glPixelZoom(1.0f, -1.0f);
    }

    glDrawPixels(mWidth, mHeight, GL_BGRA, GL_UNSIGNED_BYTE, screenBuffer);

    if (!reflectY)
    {
        glPixelZoom(1.0f, 1.0f);
    }
}
//----------------------------------------------------------------------------
void Renderer::Draw (int x, int y, const Float4& color,
    const std::string& message)
{
    const char* text = message.c_str();
    assertion(text != 0, "Invalid message\n");
    int length = (int)strlen(text);

#ifdef WM5_USE_TEXT_DISPLAY_LIST
    GLint listBase;
    glGetIntegerv(GL_LIST_BASE, &listBase);
#endif

    // Switch to orthogonal view.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-0.5f, mWidth - 0.5f, -0.5f, mHeight - 0.5f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Set default render states, except for depth buffering that must be
    // disabled because text is always overlayed.
    SetAlphaState(mDefaultAlphaState);
    SetCullState(mDefaultCullState);
    SetOffsetState(mDefaultOffsetState);
    SetStencilState(mDefaultStencilState);
    SetWireState(mDefaultWireState);

    mData->mCurrentRS.mDepthEnabled = false;
    glDisable(GL_DEPTH_TEST);

    // Set the text color.
    glColor4fv((const float*)color);

    // Draw the text string (use right-handed coordinates).
    glRasterPos3i(x, mHeight - 1 - y, 0);
#ifdef WM5_USE_TEXT_DISPLAY_LIST
    glListBase(mData->mFont.mBase);
    glCallLists(length, GL_UNSIGNED_BYTE, text);
#else
    for (int i = 0; i < length; ++i)
    {
        mData->DrawCharacter(gVerdanaS16B0I0, text[i]);
    }
#endif

#ifdef WM5_RESET_STATE_AFTER_DRAW
    // Restore visual state.  Only depth buffering state varied from the
    // default state.
    mData->mCurrentRS.mDepthEnabled = true;
    glEnable(GL_DEPTH_TEST);
#endif

    // Restore matrices.
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

#ifdef WM5_USE_TEXT_DISPLAY_LIST
    glListBase(listBase);
#endif
}
//----------------------------------------------------------------------------
#ifdef WM5_QUERY_PIXEL_COUNT
static GLuint BeginQuery ()
{
    GLuint query = 0;
    glGenQueriesARB(1, &query);
    glBeginQueryARB(GL_SAMPLES_PASSED_ARB, query);
    return query;
}

static GLuint EndQuery (GLuint query)
{
    glEndQueryARB(GL_SAMPLES_PASSED_ARB);
    GLint available = 0;
    while (!available)
    {
        // Wait until query finishes.
        glGetQueryObjectivARB(query, GL_QUERY_RESULT_AVAILABLE_ARB,
            &available);
    }

    GLuint numPixelsDrawn = 0;
    glGetQueryObjectuivARB(query, GL_QUERY_RESULT_ARB, &numPixelsDrawn);
    glDeleteQueriesARB(1, &query);
    return numPixelsDrawn;
}

#define WM5_BEGIN_QUERY(query) \
    GLuint query = BeginQuery()

#define WM5_END_QUERY(query, numPixelsDrawn) \
    numPixelsDrawn = EndQuery(query)
#else
#define WM5_BEGIN_QUERY(query)
#define WM5_END_QUERY(query, numPixelsDrawn)
#endif
//----------------------------------------------------------------------------
void Renderer::DrawPrimitive (const Visual* visual)
{
    Visual::PrimitiveType type = visual->GetPrimitiveType();
    const VertexBuffer* vbuffer = visual->GetVertexBuffer();
    const IndexBuffer* ibuffer = visual->GetIndexBuffer();

    GLuint numPixelsDrawn;
    WM5_UNUSED(numPixelsDrawn);

    if (type == Visual::PT_TRIMESH
    ||  type == Visual::PT_TRISTRIP
    ||  type == Visual::PT_TRIFAN)
    {
        int numVertices = vbuffer->GetNumElements();
        int numIndices = ibuffer->GetNumElements();
        if (numVertices > 0 && numIndices > 0)
        {
            GLenum indexType;
            const GLvoid* indexData;
            if (ibuffer->GetElementSize() == 2)
            {
                indexType = GL_UNSIGNED_SHORT;
                indexData = (unsigned short*)0 + ibuffer->GetOffset();
            }
            else  // size is 4
            {
                indexType = GL_UNSIGNED_INT;
                indexData = (unsigned int*)0 + ibuffer->GetOffset();
            }

            WM5_BEGIN_QUERY(query);

            glDrawRangeElements(gOGLPrimitiveType[type], 0, numVertices-1,
                numIndices, indexType, indexData);

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else if (type == Visual::PT_POLYSEGMENTS_CONTIGUOUS)
    {
        int numSegments = StaticCast<Polysegment>(visual)->GetNumSegments();
        if (numSegments > 0)
        {
            WM5_BEGIN_QUERY(query);

            glDrawArrays(GL_LINE_STRIP, 0, numSegments + 1);

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else if (type == Visual::PT_POLYSEGMENTS_DISJOINT)
    {
        int numSegments = StaticCast<Polysegment>(visual)->GetNumSegments();
        if (numSegments > 0)
        {
            WM5_BEGIN_QUERY(query);

            glDrawArrays(GL_LINES, 0, 2*numSegments);

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else if (type == Visual::PT_POLYPOINT)
    {
        int numPoints = StaticCast<Polypoint>(visual)->GetNumPoints();
        if (numPoints > 0)
        {
            WM5_BEGIN_QUERY(query);

            glDrawArrays(GL_POINTS, 0, numPoints);

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else
    {
        assertion(false, "Invalid type.\n");
    }
}
//----------------------------------------------------------------------------
