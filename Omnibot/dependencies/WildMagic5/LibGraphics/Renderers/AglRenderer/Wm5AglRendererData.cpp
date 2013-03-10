// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5AglRendererData.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
void AglRendererData::LoadFont (const char* face, int size, bool fontBold,
    bool fontItalic)
{
    mFont.mQuantity = 256;
    mFont.mStart = glGenLists(mFont.mQuantity);
    mFont.mBase = 1;

    Str255 fontName;
    fontName[0] = (int)strlen(face);
    strcpy((char*)fontName + 1, face);

    short fontNum;
    GetFNum(fontName, &fontNum);

    Style style = normal;
    if (fontBold)
    {
        style |= bold;
    }
    if (fontItalic)
    {
        style |= italic;
    }

    if (!aglUseFont(mContext, fontNum, style, size,0, mFont.mQuantity,
        mFont.mStart))
    {
        glDeleteLists(mFont.mStart, mFont.mQuantity);
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}
//----------------------------------------------------------------------------
void AglRendererData::SetUpBufferRect (int winXPos, int winYPos,
    int winWidth, int winHeight)
{
    // If the context is smaller than the underlying surface, we clip it.
    CGrafPtr port = aglGetDrawable(mContext);
    Rect portRect = { 0 };
    GetPortBounds(port, &portRect);
    int portWidth = portRect.right - portRect.left;
    int portHeight = portRect.bottom - portRect.top;
    if (portWidth != winWidth || portHeight != winHeight)
    {
        int height = portHeight + 1;
        int y = height - winYPos;
        if (--y < 0)
        {
            y = 0;
        }
        GLint rect[4] = { winXPos, y - winHeight, winWidth, winHeight };
        if (!aglSetInteger(mContext, AGL_BUFFER_RECT, rect))
        {
            FatalErrorMessage("Cannot assign AGL_BUFFER_RECT.");
        }
        if (!aglEnable(mContext, AGL_BUFFER_RECT))
        {
            FatalErrorMessage("Cannot enable AGL_BUFFER_RECT.");
        }
        aglUpdateContext(mContext);
    }
    else
    {
        if (!aglDisable(mContext, AGL_BUFFER_RECT))
        {
            FatalErrorMessage("Cannot disable AGL_BUFFER_RECT.");
        }
        aglUpdateContext(mContext);
    }
}
//----------------------------------------------------------------------------
void AglRendererData::Finalize (Renderer* renderer)
{
    // Select the shader profiles.
    VertexShader::SetProfile(VertexShader::VP_ARBVP1);
    PixelShader::SetProfile(PixelShader::PP_ARBFP1);

    // Query the device for its capabilities.  TODO:  The number of images
    // turns out to be larger than what Shader Model 3 supports.  Why is this?
    mMaxVShaderImages = 0;
    mMaxPShaderImages = 0;
    mMaxCombinedImages = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &mMaxVShaderImages);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mMaxPShaderImages);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mMaxCombinedImages);

    // Set the default render states.
    mCurrentRS.Initialize(renderer->GetAlphaState(), renderer->GetCullState(),
        renderer->GetDepthState(), renderer->GetOffsetState(),
        renderer->GetStencilState(), renderer->GetWireState());
}
//----------------------------------------------------------------------------
void AglRendererData::FatalErrorMessage (const char* message)
{
    if (mDSpContext)
    {
        DSpContextState contextState;
        if (DSpContext_GetState(mDSpContext, &contextState) == noErr
        &&  contextState == kDSpContextState_Active)
        {
            DSpContext_SetState(mDSpContext, kDSpContextState_Inactive);
            DSpContext_Release(mDSpContext);
        }
    }

    char header[256];
    GLenum error = aglGetError();
    sprintf(header, "AglRenderer encountered error %ld.", (long)error);
    Str255 strHeader;
    strHeader[0] = strlen(header);
    strncpy((char*)strHeader + 1, header, strHeader[0]);

    int length = strlen(message);
    Str255 strMessage;
    strMessage[0] = length;
    strncpy((char*)strMessage + 1, message, length);
    SInt16 item;
    StandardAlert(kAlertStopAlert, strHeader, strMessage,0, &item);
    ExitToShell();
}
//----------------------------------------------------------------------------
