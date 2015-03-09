// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/07/29)

#ifndef WM5GRAPHICSLIB_H
#define WM5GRAPHICSLIB_H

#include "Wm5MathematicsLIB.h"

// Begin Microsoft Windows DLL support.
#if defined(WM5_GRAPHICS_DLL_EXPORT)
    // For the DLL library.
    #define WM5_GRAPHICS_ITEM __declspec(dllexport)
#elif defined(WM5_GRAPHICS_DLL_IMPORT)
    // For a client of the DLL library.
    #define WM5_GRAPHICS_ITEM __declspec(dllimport)
#else
    // For the static library and for Apple/Linux.
    #define WM5_GRAPHICS_ITEM
#endif
// End Microsoft Windows DLL support.

// Enable this define to verify that the camera axis directions form a
// right-handed orthonormal set.  The test is done only once, because
// numerical round-off errors during rotations of the frame over time
// may require a renormalization of the frame.
#ifdef _DEBUG
    #define WM5_VALIDATE_CAMERA_FRAME_ONCE
#endif

// Enable this define to test whether Shader::MAX_PROFILES has changed
// and affects the streamed files.
#ifdef _DEBUG
    #define WM5_ASSERT_ON_CHANGED_MAX_PROFILES
#endif

// Expose this define to allow resetting of render state and other state in
// the Renderer::Draw (const Visual*, const VisualEffectInstance*) call.
//#define WM5_RESET_STATE_AFTER_DRAW

// Enable this to allow counting the number of pixels drawn in
// Renderer::DrawPrimitive.
//#define WM5_QUERY_PIXEL_COUNT

// Enable this to draw text using display lists in OpenGL; otherwise, text is
// drawn manually using bitmapped fonts.
#ifdef WM5_USE_OPENGL
    #if defined(WIN32) && !defined(WM5_USE_GLUT)
        #define WM5_USE_TEXT_DISPLAY_LIST
    #endif

    // Some OpenGL 2.x drivers are not handling normal attributes correctly.
    // This is a problem should you want to use the normal vector to pass a
    // 4-tuple of information to the shader.  The OpenGL 1.x glNormalPointer
    // assumes the normals are 3-tuples.  If you know that your target
    // machines correctly support OpenGL 2.x normal attributes, expose the
    // following #define.  Otherwise, the renderer will use the OpenGL 1.x
    // glNormalPointer.
    //
    // On the Macintosh, whether PowerPC or Intel, with NVIDIA graphics cards,
    // the generic attributes for normals does not work.  This happens because
    // we are using OpenGL 1.x extensions for shader programming.  We need to
    // update to OpenGL 2.x and later.
    #ifndef __APPLE__
        #define WM5_USE_OPENGL2_NORMAL_ATTRIBUTES
    #endif
#endif

// Enables checking of various data in the DX9 renderer.
#ifdef WM5_USE_DX9
    #ifdef _DEBUG
        #define WM5_PDR_DEBUG
    #endif
#endif

#endif
