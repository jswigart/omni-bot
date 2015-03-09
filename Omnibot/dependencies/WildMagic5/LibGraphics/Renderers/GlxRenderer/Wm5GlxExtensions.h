// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5GLXEXTENSIONS_H
#define WM5GLXEXTENSIONS_H

#include "Wm5GlExtensions.h"

#ifdef __glxext_h_
#error glxext.h included before Wm5GlExtensions.h
#endif

#define __glxext_h_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>

extern "C"
{

//----------------------------------------------------------------------------
// GLX 1.0
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlx10;

#define GLX_USE_GL                                                  1
#define GLX_BUFFER_SIZE                                             2
#define GLX_LEVEL                                                   3
#define GLX_RGBA                                                    4
#define GLX_DOUBLEBUFFER                                            5
#define GLX_STEREO                                                  6
#define GLX_AUX_BUFFERS                                             7
#define GLX_RED_SIZE                                                8
#define GLX_GREEN_SIZE                                              9
#define GLX_BLUE_SIZE                                               10
#define GLX_ALPHA_SIZE                                              11
#define GLX_DEPTH_SIZE                                              12
#define GLX_STENCIL_SIZE                                            13
#define GLX_ACCUM_RED_SIZE                                          14
#define GLX_ACCUM_GREEN_SIZE                                        15
#define GLX_ACCUM_BLUE_SIZE                                         16
#define GLX_ACCUM_ALPHA_SIZE                                        17
#define GLX_BAD_SCREEN                                              1
#define GLX_BAD_ATTRIBUTE                                           2
#define GLX_NO_EXTENSION                                            3
#define GLX_BAD_VISUAL                                              4
#define GLX_BAD_CONTEXT                                             5
#define GLX_BAD_VALUE                                               6
#define GLX_BAD_ENUM                                                7

typedef XID GLXDrawable;
typedef XID GLXPixmap;
typedef struct __GLXcontextRec* GLXContext;

#define glXQueryExtension GTglXQueryExtension
#define glXQueryVersion GTglXQueryVersion
#define glXGetConfig GTglXGetConfig
#define glXChooseVisual GTglXChooseVisual
#define glXCreateGLXPixmap GTglXCreateGLXPixmap
#define glXDestroyGLXPixmap GTglXDestroyGLXPixmap
#define glXCreateContext GTglXCreateContext
#define glXDestroyContext GTglXDestroyContext
#define glXIsDirect GTglXIsDirect
#define glXCopyContext GTglXCopyContext
#define glXMakeCurrent GTglXMakeCurrent
#define glXGetCurrentContext GTglXGetCurrentContext
#define glXGetCurrentDrawable GTglXGetCurrentDrawable
#define glXWaitGL GTglXWaitGL
#define glXWaitX GTglXWaitX
#define glXSwapBuffers GTglXSwapBuffers
#define glXUseXFont GTglXUseXFont

GT_EXTERN Bool GTglXQueryExtension (Display* pkDisplay, int* piErrorBase,
    int* piEventBase);
GT_EXTERN Bool GTglXQueryVersion (Display* pkDisplay, int* piMajor,
    int* piMinor);
GT_EXTERN int GTglXGetConfig (Display* pkDisplay, XVisualInfo* pkVisual,
    int iAttribute, int* piValue);
GT_EXTERN XVisualInfo* GTglXChooseVisual (Display* pkDisplay, int iScreen,
    int* aiAttributeList);
GT_EXTERN GLXPixmap GTglXCreateGLXPixmap (Display* pkDisplay,
    XVisualInfo* pkVisual, Pixmap kPixmap);
GT_EXTERN void GTglXDestroyGLXPixmap (Display* pkDisplay, GLXPixmap kPixmap);
GT_EXTERN GLXContext GTglXCreateContext (Display* pkDisplay,
    XVisualInfo* pkVisual, GLXContext kShareList, Bool bDirect);
GT_EXTERN void GTglXDestroyContext (Display* pkDisplay, GLXContext kContext);
GT_EXTERN Bool GTglXIsDirect (Display* pkDisplay, GLXContext kContext);
GT_EXTERN void GTglXCopyContext (Display* pkDisplay, GLXContext kSrcContext,
    GLXContext kDstContext, GLuint uiMask);  /* RHLinux has "unsigned long" */
GT_EXTERN Bool GTglXMakeCurrent (Display* pkDisplay, GLXDrawable kDrawable,
    GLXContext kContext);
GT_EXTERN GLXContext GTglXGetCurrentContext ();
GT_EXTERN GLXDrawable GTglXGetCurrentDrawable ();
GT_EXTERN void GTglXWaitGL ();
GT_EXTERN void GTglXWaitX ();
GT_EXTERN void GTglXSwapBuffers (Display* pkDisplay, GLXDrawable kDrawable);
GT_EXTERN void GTglXUseXFont (Font kFont, int iFirst, int iCount,
    int iListBase);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.1
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlx11;

#define GLX_VENDOR                                                  1
#define GLX_VERSION                                                 2
#define GLX_EXTENSIONS                                              3

#define glXQueryExtensionsString GTglXQueryExtensionsString
#define glXGetClientString GTglXGetClientString
#define glXQueryServerString GTglXQueryServerString

GT_EXTERN const char* GTglXQueryExtensionsString (Display* pkDisplay,
    int iScreen);
GT_EXTERN const char* GTglXGetClientString (Display* pkDisplay, int iName);
GT_EXTERN const char* GTglXQueryServerString (Display* pkDisplay,
    int iScreen, int iName);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.2
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlx12;

#define glXGetCurrentDisplay GTglXGetCurrentDisplay

GT_EXTERN Display* GTglXGetCurrentDisplay ();
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.3
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlx13;

#define GLX_WINDOW_BIT                                              0x00000001
#define GLX_PIXMAP_BIT                                              0x00000002
#define GLX_PBUFFER_BIT                                             0x00000004
#define GLX_RGBA_BIT                                                0x00000001
#define GLX_COLOR_INDEX_BIT                                         0x00000002
#define GLX_PBUFFER_CLOBBER_MASK                                    0x08000000
#define GLX_FRONT_LEFT_BUFFER_BIT                                   0x00000001
#define GLX_FRONT_RIGHT_BUFFER_BIT                                  0x00000002
#define GLX_BACK_LEFT_BUFFER_BIT                                    0x00000004
#define GLX_BACK_RIGHT_BUFFER_BIT                                   0x00000008
#define GLX_AUX_BUFFERS_BIT                                         0x00000010
#define GLX_DEPTH_BUFFER_BIT                                        0x00000020
#define GLX_STENCIL_BUFFER_BIT                                      0x00000040
#define GLX_ACCUM_BUFFER_BIT                                        0x00000080
#define GLX_CONFIG_CAVEAT                                           0x20
#define GLX_X_VISUAL_TYPE                                           0x22
#define GLX_TRANSPARENT_TYPE                                        0x23
#define GLX_TRANSPARENT_INDEX_VALUE                                 0x24
#define GLX_TRANSPARENT_RED_VALUE                                   0x25
#define GLX_TRANSPARENT_GREEN_VALUE                                 0x26
#define GLX_TRANSPARENT_BLUE_VALUE                                  0x27
#define GLX_TRANSPARENT_ALPHA_VALUE                                 0x28
#define GLX_DONT_CARE                                               0xFFFFFFFF
#define GLX_NONE                                                    0x8000
#define GLX_SLOW_CONFIG                                             0x8001
#define GLX_TRUE_COLOR                                              0x8002
#define GLX_DIRECT_COLOR                                            0x8003
#define GLX_PSEUDO_COLOR                                            0x8004
#define GLX_STATIC_COLOR                                            0x8005
#define GLX_GRAY_SCALE                                              0x8006
#define GLX_STATIC_GRAY                                             0x8007
#define GLX_TRANSPARENT_RGB                                         0x8008
#define GLX_TRANSPARENT_INDEX                                       0x8009
#define GLX_VISUAL_ID                                               0x800B
#define GLX_SCREEN                                                  0x800C
#define GLX_NON_CONFORMANT_CONFIG                                   0x800D
#define GLX_DRAWABLE_TYPE                                           0x8010
#define GLX_RENDER_TYPE                                             0x8011
#define GLX_X_RENDERABLE                                            0x8012
#define GLX_FBCONFIG_ID                                             0x8013
#define GLX_RGBA_TYPE                                               0x8014
#define GLX_COLOR_INDEX_TYPE                                        0x8015
#define GLX_MAX_PBUFFER_WIDTH                                       0x8016
#define GLX_MAX_PBUFFER_HEIGHT                                      0x8017
#define GLX_MAX_PBUFFER_PIXELS                                      0x8018
#define GLX_PRESERVED_CONTENTS                                      0x801B
#define GLX_LARGEST_PBUFFER                                         0x801C
#define GLX_WIDTH                                                   0x801D
#define GLX_HEIGHT                                                  0x801E
#define GLX_EVENT_MASK                                              0x801F
#define GLX_DAMAGED                                                 0x8020
#define GLX_SAVED                                                   0x8021
#define GLX_WINDOW                                                  0x8022
#define GLX_PBUFFER                                                 0x8023
#define GLX_PBUFFER_HEIGHT                                          0x8040
#define GLX_PBUFFER_WIDTH                                           0x8041

typedef struct __GLXFBConfigRec* GLXFBConfig;
typedef XID GLXWindow;
typedef XID GLXPbuffer;

#define glXGetFBConfigs GTglXGetFBConfigs
#define glXChooseFBConfig GTglXChooseFBConfig
#define glXGetFBConfigAttrib GTglXGetFBConfigAttrib
#define glXGetVisualFromFBConfig GTglXGetVisualFromFBConfig
#define glXCreateWindow GTglXCreateWindow
#define glXDestroyWindow GTglXDestroyWindow
#define glXCreatePixmap GTglXCreatePixmap
#define glXDestroyPixmap GTglXDestroyPixmap
#define glXCreatePbuffer GTglXCreatePbuffer
#define glXDestroyPbuffer GTglXDestroyPbuffer
#define glXQueryDrawable GTglXQueryDrawable
#define glXCreateNewContext GTglXCreateNewContext
#define glXMakeContextCurrent GTglXMakeContextCurrent
#define glXGetCurrentReadDrawable GTglXGetCurrentReadDrawable
#define glXQueryContext GTglXQueryContext
#define glXSelectEvent GTglXSelectEvent
#define glXGetSelectedEvent GTglXGetSelectedEvent

GT_EXTERN GLXFBConfig* GTglXGetFBConfigs (Display* pkDisplay, int iScreen,
    int* piNumElements);
GT_EXTERN GLXFBConfig* GTglXChooseFBConfig (Display* pkDisplay, int iScreen,
    const int* aiAttributeList, int* piNumElements);
GT_EXTERN int GTglXGetFBConfigAttrib (Display* pkDisplay, GLXFBConfig kConfig,
    int iAttribute, int* piValue);
GT_EXTERN XVisualInfo* GTglXGetVisualFromFBConfig (Display* pkDisplay,
    GLXFBConfig kConfig);
GT_EXTERN GLXWindow GTglXCreateWindow (Display* pkDisplay,
    GLXFBConfig kConfig, Window iWindow, const int* aiAttributeList);
GT_EXTERN void GTglXDestroyWindow (Display* pkDisplay, GLXWindow iWindow);
GT_EXTERN GLXPixmap GTglXCreatePixmap (Display* pkDisplay,
    GLXFBConfig kConfig, Pixmap kPixmap, const int* aiAttributeList);
GT_EXTERN void GTglXDestroyPixmap (Display* pkDisplay, GLXPixmap kPixmap);
GT_EXTERN GLXPbuffer GTglXCreatePbuffer (Display* pkDisplay,
    GLXFBConfig kConfig, const int* aiAttributeList);
GT_EXTERN void GTglXDestroyPbuffer (Display* pkDisplay, GLXPbuffer kPBuffer);
GT_EXTERN void GTglXQueryDrawable (Display* pkDisplay, GLXDrawable kDrawable,
    int iAttribute, unsigned int* puiValue);
GT_EXTERN GLXContext GTglXCreateNewContext (Display* pkDisplay,
    GLXFBConfig kConfig, int iRenderType, GLXContext kShareList,
    Bool bDirect);
GT_EXTERN Bool GTglXMakeContextCurrent (Display* pkDisplay, GLXDrawable kDraw,
    GLXDrawable kRead, GLXContext kContext);
GT_EXTERN GLXDrawable GTglXGetCurrentReadDrawable ();
GT_EXTERN int GTglXQueryContext (Display* pkDisplay, GLXContext kContext,
    int iAttribute, int* piValue);
GT_EXTERN void GTglXSelectEvent (Display* pkDisplay, GLXDrawable kDrawable,
    unsigned long ulEventMask);
GT_EXTERN void GTglXGetSelectedEvent (Display* pkDisplay,
    GLXDrawable kDrawable, unsigned long* puiEventMask);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.4
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlx14;

#define GLX_SAMPLE_BUFFERS                                          100000
#define GLX_SAMPLES                                                 100001

extern void (*glXGetProcAddress (const GLubyte*))(void);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** ARB Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 02) GLX_ARB_get_proc_address
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxArbGetProcAddress;

extern void (*glXGetProcAddressARB (const GLubyte*))(void);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 05) GLX_ARB_multisample
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxArbMultisample;

#define GLX_SAMPLE_BUFFERS_ARB                                      100000
#define GLX_SAMPLES_ARB                                             100001
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 39) GLX_ARB_fbconfig_float
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxArbFBConfigFloat;

#define GLX_RGBA_FLOAT_TYPE_ARB                                     0x20B9
#define GLX_RGBA_FLOAT_BIT_ARB                                      0x00000004
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** EXT Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 028) GLX_EXT_visual_info
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxExtVisualInfo;

#define GLX_X_VISUAL_TYPE_EXT                                       0x22
#define GLX_TRANSPARENT_TYPE_EXT                                    0x23
#define GLX_TRANSPARENT_INDEX_VALUE_EXT                             0x24
#define GLX_TRANSPARENT_RED_VALUE_EXT                               0x25
#define GLX_TRANSPARENT_GREEN_VALUE_EXT                             0x26
#define GLX_TRANSPARENT_BLUE_VALUE_EXT                              0x27
#define GLX_TRANSPARENT_ALPHA_VALUE_EXT                             0x28
#define GLX_NONE_EXT                                                0x8000
#define GLX_TRUE_COLOR_EXT                                          0x8002
#define GLX_DIRECT_COLOR_EXT                                        0x8003
#define GLX_PSEUDO_COLOR_EXT                                        0x8004
#define GLX_STATIC_COLOR_EXT                                        0x8005
#define GLX_GRAY_SCALE_EXT                                          0x8006
#define GLX_STATIC_GRAY_EXT                                         0x8007
#define GLX_TRANSPARENT_RGB_EXT                                     0x8008
#define GLX_TRANSPARENT_INDEX_EXT                                   0x8009
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 044) GLX_EXT_visual_rating
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxExtVisualRating;

#define GLX_VISUAL_CAVEAT_EXT                                       0x20
#define GLX_SLOW_VISUAL_EXT                                         0x8001
#define GLX_NON_CONFORMANT_VISUAL_EXT                               0x800D
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 047) GLX_EXT_import_context
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxExtImportContext;

#define GLX_SHARE_CONTEXT_EXT                                       0x800A
#define GLX_VISUAL_ID_EXT                                           0x800B
#define GLX_SCREEN_EXT                                              0x800C

typedef XID GLXContextID;

#define glXGetCurrentDisplayEXT GTglXGetCurrentDisplayEXT
#define glXQueryContextInfoEXT GTglXQueryContextInfoEXT
#define glXGetContextIDEXT GTglXGetContextIDEXT
#define glXImportContextEXT GTglXImportContextEXT
#define glXFreeContextEXT GTglXFreeContextEXT

GT_EXTERN Display* GTglXGetCurrentDisplayEXT ();
GT_EXTERN int GTglXQueryContextInfoEXT (Display* pkDisplay,
    GLXContext kContext, int iAttribute, int* piValue);
GT_EXTERN GLXContextID GTglXGetContextIDEXT (const GLXContext kContext);
GT_EXTERN GLXContext GTglXImportContextEXT (Display* pkDisplay,
    GLXContextID kContextID);
GT_EXTERN void GTglXFreeContextEXT (Display* pkDisplay, GLXContext kContext);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** Other Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 49) GLX_SGIX_fbconfig
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxSgixFBConfig;

#define GLX_WINDOW_BIT_SGIX                                         0x00000001
#define GLX_PIXMAP_BIT_SGIX                                         0x00000002
#define GLX_RGBA_BIT_SGIX                                           0x00000001
#define GLX_COLOR_INDEX_BIT_SGIX                                    0x00000002
#define GLX_DRAWABLE_TYPE_SGIX                                      0x8010
#define GLX_RENDER_TYPE_SGIX                                        0x8011
#define GLX_X_RENDERABLE_SGIX                                       0x8012
#define GLX_FBCONFIG_ID_SGIX                                        0x8013
#define GLX_RGBA_TYPE_SGIX                                          0x8014
#define GLX_COLOR_INDEX_TYPE_SGIX                                   0x8015

typedef XID GLXFBConfigIDSGIX;
typedef struct __GLXFBConfigRec* GLXFBConfigSGIX;

#define glXGetFBConfigAttribSGIX GTglXGetFBConfigAttribSGIX
#define glXChooseFBConfigSGIX GTglXChooseFBConfigSGIX
#define glXCreateGLXPixmapWithConfigSGIX GTglXCreateGLXPixmapWithConfigSGIX
#define glXCreateContextWithConfigSGIX GTglXCreateContextWithConfigSGIX
#define glXGetVisualFromFBConfigSGIX GTglXGetVisualFromFBConfigSGIX
#define glXGetFBConfigFromVisualSGIX GTglXGetFBConfigFromVisualSGIX

GT_EXTERN int GTglXGetFBConfigAttribSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, int iAttribute, int* piValue);
GT_EXTERN GLXFBConfigSGIX* GTglXChooseFBConfigSGIX (Display* pkDisplay,
    int iScreen, int* aiAttributeList, int* piNumElements);
GT_EXTERN GLXPixmap GTglXCreateGLXPixmapWithConfigSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, Pixmap kPixmap);
GT_EXTERN GLXContext GTglXCreateContextWithConfigSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, int iRenderType, GLXContext kShareList,
    Bool bDirect);
GT_EXTERN XVisualInfo* GTglXGetVisualFromFBConfigSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig);
GT_EXTERN GLXFBConfigSGIX GTglXGetFBConfigFromVisualSGIX (Display* pkDisplay,
    XVisualInfo* pkVisual);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 50) GLX_SGIX_pbuffer
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlxSgixPBuffer;

#define GLX_PBUFFER_BIT_SGIX                                        0x00000004
#define GLX_BUFFER_CLOBBER_MASK_SGIX                                0x08000000
#define GLX_FRONT_LEFT_BUFFER_BIT_SGIX                              0x00000001
#define GLX_FRONT_RIGHT_BUFFER_BIT_SGIX                             0x00000002
#define GLX_BACK_LEFT_BUFFER_BIT_SGIX                               0x00000004
#define GLX_BACK_RIGHT_BUFFER_BIT_SGIX                              0x00000008
#define GLX_AUX_BUFFERS_BIT_SGIX                                    0x00000010
#define GLX_DEPTH_BUFFER_BIT_SGIX                                   0x00000020
#define GLX_STENCIL_BUFFER_BIT_SGIX                                 0x00000040
#define GLX_ACCUM_BUFFER_BIT_SGIX                                   0x00000080
#define GLX_SAMPLE_BUFFERS_BIT_SGIX                                 0x00000100
#define GLX_MAX_PBUFFER_WIDTH_SGIX                                  0x8016
#define GLX_MAX_PBUFFER_HEIGHT_SGIX                                 0x8017
#define GLX_MAX_PBUFFER_PIXELS_SGIX                                 0x8018
#define GLX_OPTIMAL_PBUFFER_WIDTH_SGIX                              0x8019
#define GLX_OPTIMAL_PBUFFER_HEIGHT_SGIX                             0x801A
#define GLX_PRESERVED_CONTENTS_SGIX                                 0x801B
#define GLX_LARGEST_PBUFFER_SGIX                                    0x801C
#define GLX_WIDTH_SGIX                                              0x801D
#define GLX_HEIGHT_SGIX                                             0x801E
#define GLX_EVENT_MASK_SGIX                                         0x801F
#define GLX_DAMAGED_SGIX                                            0x8020
#define GLX_SAVED_SGIX                                              0x8021
#define GLX_WINDOW_SGIX                                             0x8022
#define GLX_PBUFFER_SGIX                                            0x8023

typedef XID GLXPbufferSGIX;
typedef struct
{
    int type;
    unsigned long serial;
    Bool send_event;
    Display *display;
    GLXDrawable drawable;
    int event_type;
    int draw_type;
    unsigned int mask;
    int x, y;
    int width, height;
    int count;
}
GLXBufferClobberEventSGIX;

#define glXCreateGLXPbufferSGIX GTglXCreateGLXPbufferSGIX
#define glXDestroyGLXPbufferSGIX GTglXDestroyGLXPbufferSGIX
#define glXQueryGLXPbufferSGIX GTglXQueryGLXPbufferSGIX
#define glXSelectEventSGIX GTglXSelectEventSGIX
#define glXGetSelectedEventSGIX GTglXGetSelectedEventSGIX

GT_EXTERN GLXPbufferSGIX GTglXCreateGLXPbufferSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, unsigned int uiWidth, unsigned int uiHeight,
    int* aiAttributeList);
GT_EXTERN void GTglXDestroyGLXPbufferSGIX (Display* pkDisplay,
    GLXPbufferSGIX kPBuffer);
GT_EXTERN int GTglXQueryGLXPbufferSGIX (Display* pkDisplay,
    GLXPbufferSGIX kPBuffer, int iAttribute, unsigned int* auiValue);
GT_EXTERN void GTglXSelectEventSGIX (Display* pkDisplay,
    GLXDrawable kDrawable, unsigned long ulMask);
GT_EXTERN void GTglXGetSelectedEventSGIX (Display* pkDisplay,
    GLXDrawable kDrawable, unsigned long* pulMask);
//----------------------------------------------------------------------------

GT_EXTERN void GTInitGLX (const char* acInfoFile);

} // extern "C"

#endif
