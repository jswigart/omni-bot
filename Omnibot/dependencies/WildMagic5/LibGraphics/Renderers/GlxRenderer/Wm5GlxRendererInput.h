// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5GLXRENDERERINPUT_H
#define WM5GLXRENDERERINPUT_H

#include "Wm5OpenGLRendererLIB.h"
#include "Wm5GlxExtensions.h"

namespace Wm5
{

class RendererInput
{
public:
    // Input parameter.
    Display* mDisplay;

    // Output parameter.
    XVisualInfo* mVisual;
    GLXContext mContext;
};

}

#endif
