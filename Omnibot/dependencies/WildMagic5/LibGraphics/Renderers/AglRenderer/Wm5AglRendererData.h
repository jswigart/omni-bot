// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5AGLRENDERERDATA_H
#define WM5AGLRENDERERDATA_H

#include "Wm5OpenGLRendererLIB.h"
#include "Wm5OpenGLRendererData.h"
#include <AGL/agl.h>
#include <Carbon/Carbon.h>
#include <DrawSprocket/DrawSprocket.h>

namespace Wm5
{

class Renderer;

class AglRendererData : public RendererData
{
public:
    // Platform-dependent data.
    AGLDevice mDevice;
    WindowRef mWindow;
    AGLDrawable mDrawable;
    AGLContext mContext;
    DSpContextReference mDSpContext;
    GWorldPtr mBack;
    static Boolean msDSpStarted;

    // Create a font.
    void LoadFont (const char* face, int size, bool fontBold,
        bool fontItalic);

    // Set up drawing area.
    void SetUpBufferRect (int winXPos, int winYPos, int winWidth,
        int winHeight);

    // Finalization after a context has been created.
    void Finalize (Renderer* renderer);

    // Display error message and terminate.
    void FatalErrorMessage (const char* message);
};

}

#endif
