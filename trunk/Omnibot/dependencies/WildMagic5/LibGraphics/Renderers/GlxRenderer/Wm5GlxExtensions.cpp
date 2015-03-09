// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GlxExtensions.h"
#include "Wm5GlUtility.h"

//----------------------------------------------------------------------------
void* GTGetFunctionPointer (const char* acFunction)
{
    return (void*)(*glXGetProcAddressARB)((const GLubyte*)acFunction);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.0
//----------------------------------------------------------------------------
int GTExistsGlx10 = 1;

#undef glXQueryExtension
#undef glXQueryVersion
#undef glXGetConfig
#undef glXChooseVisual
#undef glXCreateGLXPixmap
#undef glXDestroyGLXPixmap
#undef glXCreateContext
#undef glXDestroyContext
#undef glXIsDirect
#undef glXCopyContext
#undef glXMakeCurrent
#undef glXGetCurrentContext
#undef glXGetCurrentDrawable
#undef glXWaitGL
#undef glXWaitX
#undef glXSwapBuffers
#undef glXUseXFont

extern "C"
{

EXTERN Bool glXQueryExtension (Display*, int*, int*);
EXTERN Bool glXQueryVersion (Display*, int*, int*);
EXTERN int glXGetConfig (Display*, XVisualInfo*, int, int*);
EXTERN XVisualInfo* glXChooseVisual (Display*, int, int*);
EXTERN GLXPixmap glXCreateGLXPixmap (Display*, XVisualInfo*, Pixmap);
EXTERN void glXDestroyGLXPixmap (Display*, GLXPixmap);
EXTERN GLXContext glXCreateContext (Display*, XVisualInfo*, GLXContext, Bool);
EXTERN void glXDestroyContext (Display*, GLXContext);
EXTERN Bool glXIsDirect (Display*, GLXContext);
EXTERN void glXCopyContext (Display*, GLXContext, GLXContext, GLuint);
EXTERN Bool glXMakeCurrent (Display*, GLXDrawable, GLXContext);
EXTERN GLXContext glXGetCurrentContext (void);
EXTERN GLXDrawable glXGetCurrentDrawable (void);
EXTERN void glXWaitGL (void);
EXTERN void glXWaitX (void);
EXTERN void glXSwapBuffers (Display*, GLXDrawable);
EXTERN void glXUseXFont (Font, int, int, int);

}  // extern "C"

//----------------------------------------------------------------------------
Bool GTglXQueryExtension (Display* pkDisplay, int* piErrorBase,
    int* piEventBase)
{
    GT_PRE_BODY_A3_RET(glXQueryExtension,pkDisplay,piErrorBase,piEventBase,
        Bool);
}
//----------------------------------------------------------------------------
Bool GTglXQueryVersion (Display* pkDisplay, int* piMajor,
    int* piMinor)
{
    GT_PRE_BODY_A3_RET(glXQueryVersion,pkDisplay,piMajor,piMinor,Bool);
}
//----------------------------------------------------------------------------
int GTglXGetConfig (Display* pkDisplay, XVisualInfo* pkVisual,
    int iAttribute, int* piValue)
{
    GT_PRE_BODY_A4_RET(glXGetConfig,pkDisplay,pkVisual,iAttribute,piValue,
        int);
}
//----------------------------------------------------------------------------
XVisualInfo* GTglXChooseVisual (Display* pkDisplay, int iScreen,
    int* aiAttributeList)
{
    GT_PRE_BODY_A3_RET(glXChooseVisual,pkDisplay,iScreen,aiAttributeList,
        XVisualInfo*);
}
//----------------------------------------------------------------------------
GLXPixmap GTglXCreateGLXPixmap (Display* pkDisplay,
    XVisualInfo* pkVisual, Pixmap kPixmap)
{
    GT_PRE_BODY_A3_RET(glXCreateGLXPixmap,pkDisplay,pkVisual,kPixmap,
        GLXPixmap);
}
//----------------------------------------------------------------------------
void GTglXDestroyGLXPixmap (Display* pkDisplay, GLXPixmap kPixmap)
{
    GT_PRE_BODY_A2(glXDestroyGLXPixmap,pkDisplay,kPixmap);
}
//----------------------------------------------------------------------------
GLXContext GTglXCreateContext (Display* pkDisplay,
    XVisualInfo* pkVisual, GLXContext kShareList, Bool bDirect)
{
    GT_PRE_BODY_A4_RET(glXCreateContext,pkDisplay,pkVisual,kShareList,
        bDirect,GLXContext);
}
//----------------------------------------------------------------------------
void GTglXDestroyContext (Display* pkDisplay, GLXContext kContext)
{
    GT_PRE_BODY_A2(glXDestroyContext,pkDisplay,kContext);
}
//----------------------------------------------------------------------------
Bool GTglXIsDirect (Display* pkDisplay, GLXContext kContext)
{
    GT_PRE_BODY_A2_RET(glXIsDirect,pkDisplay,kContext,Bool);
}
//----------------------------------------------------------------------------
void GTglXCopyContext (Display* pkDisplay, GLXContext kSrcContext,
    GLXContext kDstContext, GLuint uiMask)
{
    GT_PRE_BODY_A4(glXCopyContext,pkDisplay,kSrcContext,kDstContext,uiMask);
}
//----------------------------------------------------------------------------
Bool GTglXMakeCurrent (Display* pkDisplay, GLXDrawable kDrawable,
    GLXContext kContext)
{
    GT_PRE_BODY_A3_RET(glXMakeCurrent,pkDisplay,kDrawable,kContext,Bool);
}
//----------------------------------------------------------------------------
GLXContext GTglXGetCurrentContext ()
{
    GT_PRE_BODY_A0_RET(glXGetCurrentContext,GLXContext);
}
//----------------------------------------------------------------------------
GLXDrawable GTglXGetCurrentDrawable ()
{
    GT_PRE_BODY_A0_RET(glXGetCurrentDrawable,GLXDrawable);
}
//----------------------------------------------------------------------------
void GTglXWaitGL ()
{
    GT_PRE_BODY_A0(glXWaitGL);
}
//----------------------------------------------------------------------------
void GTglXWaitX ()
{
    GT_PRE_BODY_A0(glXWaitX);
}
//----------------------------------------------------------------------------
void GTglXSwapBuffers (Display* pkDisplay, GLXDrawable kDrawable)
{
    GT_PRE_BODY_A2(glXSwapBuffers,pkDisplay,kDrawable);
}
//----------------------------------------------------------------------------
void GTglXUseXFont (Font kFont, int iFirst, int iCount,
    int iListBase)
{
    GT_PRE_BODY_A4(glXUseXFont,kFont,iFirst,iCount,iListBase);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.1
//----------------------------------------------------------------------------
int GTExistsGlx11 = 1;

#undef glXQueryExtensionsString
#undef glXGetClientString
#undef glXQueryServerString

extern "C"
{

EXTERN const char* glXQueryExtensionsString (Display*, int);
EXTERN const char* glXGetClientString (Display*, int);
EXTERN const char* glXQueryServerString (Display*, int, int);

}  // extern "C"

//----------------------------------------------------------------------------
const char* GTglXQueryExtensionsString (Display* pkDisplay, int iScreen)
{
    GT_PRE_BODY_A2_RET(glXQueryExtensionsString,pkDisplay,iScreen,
        const char*);
}
//----------------------------------------------------------------------------
const char* GTglXGetClientString (Display* pkDisplay, int iName)
{
    GT_PRE_BODY_A2_RET(glXGetClientString,pkDisplay,iName,const char*);
}
//----------------------------------------------------------------------------
const char* GTglXQueryServerString (Display* pkDisplay, int iScreen,
    int iName)
{
    GT_PRE_BODY_A3_RET(glXQueryServerString,pkDisplay,iScreen,iName,
        const char*);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.2
//----------------------------------------------------------------------------
int GTExistsGlx12 = 1;

#undef glXGetCurrentDisplay

extern "C"
{

EXTERN Display* glXGetCurrentDisplay ();

}  // extern "C"

//----------------------------------------------------------------------------
Display* GTglXGetCurrentDisplay ()
{
    GT_PRE_BODY_A0_RET(glXGetCurrentDisplay,Display*);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.3
//----------------------------------------------------------------------------
int GTExistsGlx13 = 1;

#undef glXGetFBConfigs
#undef glXChooseFBConfig
#undef glXGetFBConfigAttrib
#undef glXGetVisualFromFBConfig
#undef glXCreateWindow
#undef glXDestroyWindow
#undef glXCreatePixmap
#undef glXDestroyPixmap
#undef glXCreatePbuffer
#undef glXDestroyPbuffer
#undef glXQueryDrawable
#undef glXCreateNewContext
#undef glXMakeContextCurrent
#undef glXGetCurrentReadDrawable
#undef glXQueryContext
#undef glXSelectEvent
#undef glXGetSelectedEvent

extern "C"
{

EXTERN GLXFBConfig* glXGetFBConfigs (Display* pkDisplay, int iScreen,
    int* piNumElements);
EXTERN GLXFBConfig* glXChooseFBConfig (Display* pkDisplay, int iScreen,
    const int* aiAttributeList, int* piNumElements);
EXTERN int glXGetFBConfigAttrib (Display* pkDisplay, GLXFBConfig kConfig,
    int iAttribute, int* piValue);
EXTERN XVisualInfo* glXGetVisualFromFBConfig (Display* pkDisplay,
    GLXFBConfig kConfig);
EXTERN GLXWindow glXCreateWindow (Display* pkDisplay,
    GLXFBConfig kConfig, Window iWindow, const int* aiAttributeList);
EXTERN void glXDestroyWindow (Display* pkDisplay, GLXWindow iWindow);
EXTERN GLXPixmap glXCreatePixmap (Display* pkDisplay,
    GLXFBConfig kConfig, Pixmap kPixmap, const int* aiAttributeList);
EXTERN void glXDestroyPixmap (Display* pkDisplay, GLXPixmap kPixmap);
EXTERN GLXPbuffer glXCreatePbuffer (Display* pkDisplay,
    GLXFBConfig kConfig, const int* aiAttributeList);
EXTERN void glXDestroyPbuffer (Display* pkDisplay, GLXPbuffer kPBuffer);
EXTERN void glXQueryDrawable (Display* pkDisplay, GLXDrawable kDrawable,
    int iAttribute, unsigned int* puiValue);
EXTERN GLXContext glXCreateNewContext (Display* pkDisplay,
    GLXFBConfig kConfig, int iRenderType, GLXContext kShareList,
    Bool bDirect);
EXTERN Bool glXMakeContextCurrent (Display* pkDisplay, GLXDrawable kDraw,
    GLXDrawable kRead, GLXContext kContext);
EXTERN GLXDrawable glXGetCurrentReadDrawable ();
EXTERN int glXQueryContext (Display* pkDisplay, GLXContext kContext,
    int iAttribute, int* piValue);
EXTERN void glXSelectEvent (Display* pkDisplay, GLXDrawable kDrawable,
    unsigned long ulEventMask);
EXTERN void glXGetSelectedEvent (Display* pkDisplay,
    GLXDrawable kDrawable, unsigned long* puiEventMask);

}  // extern "C"

//----------------------------------------------------------------------------
GLXFBConfig* GTglXGetFBConfigs (Display* pkDisplay, int iScreen,
    int* piNumElements)
{
    GT_PRE_BODY_A3_RET(glXGetFBConfigs,pkDisplay,iScreen,piNumElements,
        GLXFBConfig*);
}
//----------------------------------------------------------------------------
GLXFBConfig* GTglXChooseFBConfig (Display* pkDisplay, int iScreen,
    const int* aiAttributeList, int* piNumElements)
{
    GT_PRE_BODY_A4_RET(glXChooseFBConfig,pkDisplay,iScreen,aiAttributeList,
        piNumElements,GLXFBConfig*);
}
//----------------------------------------------------------------------------
int GTglXGetFBConfigAttrib (Display* pkDisplay, GLXFBConfig kConfig,
    int iAttribute, int* piValue)
{
    GT_PRE_BODY_A4_RET(glXGetFBConfigAttrib,pkDisplay,kConfig,iAttribute,
        piValue,int);
}
//----------------------------------------------------------------------------
XVisualInfo* GTglXGetVisualFromFBConfig (Display* pkDisplay,
    GLXFBConfig kConfig)
{
    GT_PRE_BODY_A2_RET(glXGetVisualFromFBConfig,pkDisplay,kConfig,
        XVisualInfo*);
}
//----------------------------------------------------------------------------
GLXWindow GTglXCreateWindow (Display* pkDisplay,
    GLXFBConfig kConfig, Window iWindow, const int* aiAttributeList)
{
    GT_PRE_BODY_A4_RET(glXCreateWindow,pkDisplay,kConfig,iWindow,
        aiAttributeList,GLXWindow);
}
//----------------------------------------------------------------------------
void GTglXDestroyWindow (Display* pkDisplay, GLXWindow iWindow)
{
    GT_PRE_BODY_A2(glXDestroyWindow,pkDisplay,iWindow);
}
//----------------------------------------------------------------------------
GLXPixmap GTglXCreatePixmap (Display* pkDisplay,
    GLXFBConfig kConfig, Pixmap kPixmap, const int* aiAttributeList)
{
    GT_PRE_BODY_A4_RET(glXCreatePixmap,pkDisplay,kConfig,kPixmap,
        aiAttributeList,GLXPixmap);
}
//----------------------------------------------------------------------------
void GTglXDestroyPixmap (Display* pkDisplay, GLXPixmap kPixmap)
{
    GT_PRE_BODY_A2(glXDestroyPixmap,pkDisplay,kPixmap);
}
//----------------------------------------------------------------------------
GLXPbuffer GTglXCreatePbuffer (Display* pkDisplay,
    GLXFBConfig kConfig, const int* aiAttributeList)
{
    GT_PRE_BODY_A3_RET(glXCreatePbuffer,pkDisplay,kConfig,aiAttributeList,
        GLXPbuffer);
}
//----------------------------------------------------------------------------
void GTglXDestroyPbuffer (Display* pkDisplay, GLXPbuffer kPBuffer)
{
    GT_PRE_BODY_A2(glXDestroyPbuffer,pkDisplay,kPBuffer);
}
//----------------------------------------------------------------------------
void GTglXQueryDrawable (Display* pkDisplay, GLXDrawable kDrawable,
    int iAttribute, unsigned int* puiValue)
{
    GT_PRE_BODY_A4(glXQueryDrawable,pkDisplay,kDrawable,iAttribute,puiValue);
}
//----------------------------------------------------------------------------
GLXContext GTglXCreateNewContext (Display* pkDisplay,
    GLXFBConfig kConfig, int iRenderType, GLXContext kShareList,
    Bool bDirect)
{
    GT_PRE_BODY_A5_RET(glXCreateNewContext,pkDisplay,kConfig,iRenderType,
        kShareList,bDirect,GLXContext);
}
//----------------------------------------------------------------------------
Bool GTglXMakeContextCurrent (Display* pkDisplay, GLXDrawable kDraw,
    GLXDrawable kRead, GLXContext kContext)
{
    GT_PRE_BODY_A4_RET(glXMakeContextCurrent,pkDisplay,kDraw,kRead,kContext,
        Bool);
}
//----------------------------------------------------------------------------
GLXDrawable GTglXGetCurrentReadDrawable ()
{
    GT_PRE_BODY_A0_RET(glXGetCurrentReadDrawable,GLXDrawable);
}
//----------------------------------------------------------------------------
int GTglXQueryContext (Display* pkDisplay, GLXContext kContext,
    int iAttribute, int* piValue)
{
    GT_PRE_BODY_A4_RET(glXQueryContext,pkDisplay,kContext,iAttribute,piValue,
        int);
}
//----------------------------------------------------------------------------
void GTglXSelectEvent (Display* pkDisplay, GLXDrawable kDrawable,
    unsigned long ulEventMask)
{
    GT_PRE_BODY_A3(glXSelectEvent,pkDisplay,kDrawable,ulEventMask);
}
//----------------------------------------------------------------------------
void GTglXGetSelectedEvent (Display* pkDisplay,
    GLXDrawable kDrawable, unsigned long* puiEventMask)
{
    GT_PRE_BODY_A3(glXGetSelectedEvent,pkDisplay,kDrawable,puiEventMask);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLX 1.4
//----------------------------------------------------------------------------
int GTExistsGlx14 = 1;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** ARB Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 02) GLX_ARB_get_proc_address
//----------------------------------------------------------------------------
int GTExistsGlxArbGetProcAddress = 0;
//----------------------------------------------------------------------------
void GTInitGlxArbGetProcAddress ()
{
    if (GTSupportsExtension("GLX_ARB_get_proc_address"))
    {
        GTExistsGlxArbGetProcAddress = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 05) GLX_ARB_multisample
//----------------------------------------------------------------------------
int GTExistsGlxArbMultisample = 0;
//----------------------------------------------------------------------------
void GTInitGlxArbGetMultisample ()
{
    if (GTSupportsExtension("GLX_ARB_multisample"))
    {
        GTExistsGlxArbMultisample = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 39) GLX_ARB_fbconfig_float
//----------------------------------------------------------------------------
int GTExistsGlxArbFBConfigFloat = 0;
//----------------------------------------------------------------------------
void GTInitGlxArbFBConfigFloat ()
{
    if (GTSupportsExtension("GLX_ARB_fbconfig_float"))
    {
        GTExistsGlxArbFBConfigFloat = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** EXT Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 028) GLX_EXT_visual_info
//----------------------------------------------------------------------------
int GTExistsGlxExtVisualInfo = 0;
//----------------------------------------------------------------------------
void GTInitGlxExtVisualInfo ()
{
    if (GTSupportsExtension("GLX_EXT_visual_info"))
    {
        GTExistsGlxExtVisualInfo = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 044) GLX_EXT_visual_rating
//----------------------------------------------------------------------------
int GTExistsGlxExtVisualRating = 0;
//----------------------------------------------------------------------------
void GTInitGlxExtVisualRating ()
{
    if (GTSupportsExtension("GLX_EXT_visual_rating"))
    {
        GTExistsGlxExtVisualRating = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 047) GLX_EXT_import_context
//----------------------------------------------------------------------------
int GTExistsGlxExtImportContext = 0;

typedef Display* (*PsglXGetCurrentDisplayEXT) (void);
typedef int (*PsglXQueryContextInfoEXT) (Display*, GLXContext, int, int*);
typedef GLXContextID (*PsglXGetContextIDEXT) (const GLXContext);
typedef GLXContext (*PsglXImportContextEXT) (Display*, GLXContextID);
typedef void (*PsglXFreeContextEXT) (Display*, GLXContext);

static PsglXGetCurrentDisplayEXT sglXGetCurrentDisplayEXT = 0;
static PsglXQueryContextInfoEXT sglXQueryContextInfoEXT = 0;
static PsglXGetContextIDEXT sglXGetContextIDEXT = 0;
static PsglXImportContextEXT sglXImportContextEXT = 0;
static PsglXFreeContextEXT sglXFreeContextEXT = 0;
//----------------------------------------------------------------------------
Display* GTglXGetCurrentDisplayEXT ()
{
    GT_BODY_A0_RET(sglXGetCurrentDisplayEXT,Display*,0);
}
//----------------------------------------------------------------------------
int GTglXQueryContextInfoEXT (Display* pkDisplay,
    GLXContext kContext, int iAttribute, int* piValue)
{
    GT_BODY_A4_RET(sglXQueryContextInfoEXT,pkDisplay,kContext,iAttribute,
        piValue,int,0);
}
//----------------------------------------------------------------------------
GLXContextID GTglXGetContextIDEXT (const GLXContext kContext)
{
    GT_BODY_A1_RET(sglXGetContextIDEXT,kContext,GLXContextID,0);
}
//----------------------------------------------------------------------------
GLXContext GTglXImportContextEXT (Display* pkDisplay,
    GLXContextID kContextID)
{
    GT_BODY_A2_RET(sglXImportContextEXT,pkDisplay,kContextID,GLXContext,0);
}
//----------------------------------------------------------------------------
void GTglXFreeContextEXT (Display* pkDisplay, GLXContext kContext)
{
    GT_BODY_A2(sglXFreeContextEXT,pkDisplay,kContext);
}
//----------------------------------------------------------------------------
void GTInitGlxExtImportContext ()
{
    if (GTSupportsExtension("GLX_EXT_import_context"))
    {
        GTExistsGlxExtImportContext = 1;
        GT_GET_FUNCTION(sglXGetCurrentDisplayEXT);
        GT_GET_FUNCTION(sglXQueryContextInfoEXT);
        GT_GET_FUNCTION(sglXGetContextIDEXT);
        GT_GET_FUNCTION(sglXImportContextEXT);
        GT_GET_FUNCTION(sglXFreeContextEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** Other Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 49) GLX_SGIX_fbconfig
//----------------------------------------------------------------------------
int GTExistsGlxSgixFBConfig = 0;

typedef int (*PsglXGetFBConfigAttribSGIX) (Display*, GLXFBConfigSGIX, int,
    int*);
typedef GLXFBConfigSGIX* (*PsglXChooseFBConfigSGIX) (Display*, int, int*,
    int*);
typedef GLXPixmap (*PsglXCreateGLXPixmapWithConfigSGIX) (Display*,
    GLXFBConfigSGIX, Pixmap);
typedef GLXContext (*PsglXCreateContextWithConfigSGIX) (Display*,
    GLXFBConfigSGIX, int, GLXContext, Bool);
typedef XVisualInfo* (*PsglXGetVisualFromFBConfigSGIX) (Display*,
    GLXFBConfigSGIX);
typedef GLXFBConfigSGIX (*PsglXGetFBConfigFromVisualSGIX) (Display*,
    XVisualInfo*);

static PsglXGetFBConfigAttribSGIX sglXGetFBConfigAttribSGIX = 0;
static PsglXChooseFBConfigSGIX sglXChooseFBConfigSGIX = 0;
static PsglXCreateGLXPixmapWithConfigSGIX
    sglXCreateGLXPixmapWithConfigSGIX = 0;
static PsglXCreateContextWithConfigSGIX sglXCreateContextWithConfigSGIX = 0;
static PsglXGetVisualFromFBConfigSGIX sglXGetVisualFromFBConfigSGIX = 0;
static PsglXGetFBConfigFromVisualSGIX sglXGetFBConfigFromVisualSGIX = 0;
//----------------------------------------------------------------------------
int GTglXGetFBConfigAttribSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, int iAttribute, int* piValue)
{
    GT_BODY_A4_RET(sglXGetFBConfigAttribSGIX,pkDisplay,kConfig,iAttribute,
        piValue,int,0);
}
//----------------------------------------------------------------------------
GLXFBConfigSGIX* GTglXChooseFBConfigSGIX (Display* pkDisplay,
    int iScreen, int* aiAttributeList, int* piNumElements)
{
    GT_BODY_A4_RET(sglXChooseFBConfigSGIX,pkDisplay,iScreen,
        aiAttributeList,piNumElements,GLXFBConfigSGIX*,0);
}
//----------------------------------------------------------------------------
GLXPixmap GTglXCreateGLXPixmapWithConfigSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, Pixmap kPixmap)
{
    GT_BODY_A3_RET(sglXCreateGLXPixmapWithConfigSGIX,pkDisplay,kConfig,
        kPixmap,GLXPixmap,0);
}
//----------------------------------------------------------------------------
GLXContext GTglXCreateContextWithConfigSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, int iRenderType, GLXContext kShareList,
    Bool bDirect)
{
    GT_BODY_A5_RET(sglXCreateContextWithConfigSGIX,pkDisplay,kConfig,
        iRenderType,kShareList,bDirect,GLXContext,0);
}
//----------------------------------------------------------------------------
XVisualInfo* GTglXGetVisualFromFBConfigSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig)
{
    GT_BODY_A2_RET(sglXGetVisualFromFBConfigSGIX,pkDisplay,kConfig,
        XVisualInfo*,0);
}
//----------------------------------------------------------------------------
GLXFBConfigSGIX GTglXGetFBConfigFromVisualSGIX (Display* pkDisplay,
    XVisualInfo* pkVisual)
{
    GT_BODY_A2_RET(sglXGetFBConfigFromVisualSGIX,pkDisplay,pkVisual,
        GLXFBConfigSGIX,0);
}
//----------------------------------------------------------------------------
void GTInitGlxSgixFBConfig ()
{
    if (GTSupportsExtension("GLX_SGIX_fbconfig"))
    {
        GTExistsGlxSgixFBConfig = 1;
        GT_GET_FUNCTION(sglXGetFBConfigAttribSGIX);
        GT_GET_FUNCTION(sglXChooseFBConfigSGIX);
        GT_GET_FUNCTION(sglXCreateGLXPixmapWithConfigSGIX);
        GT_GET_FUNCTION(sglXCreateContextWithConfigSGIX);
        GT_GET_FUNCTION(sglXGetVisualFromFBConfigSGIX);
        GT_GET_FUNCTION(sglXGetFBConfigFromVisualSGIX);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 50) GLX_SGIX_pbuffer
//----------------------------------------------------------------------------
int GTExistsGlxSgixPBuffer = 0;

typedef GLXPbufferSGIX (*PsglXCreateGLXPbufferSGIX) (Display*,
    GLXFBConfigSGIX, unsigned int, unsigned int, int*);
typedef void (*PsglXDestroyGLXPbufferSGIX) (Display*, GLXPbufferSGIX);
typedef int (*PsglXQueryGLXPbufferSGIX) (Display*, GLXPbufferSGIX, int,
    unsigned int*);
typedef void (*PsglXSelectEventSGIX) (Display*, GLXDrawable, unsigned long);
typedef void (*PsglXGetSelectedEventSGIX) (Display*, GLXDrawable,
    unsigned long*);

static PsglXCreateGLXPbufferSGIX sglXCreateGLXPbufferSGIX = 0;
static PsglXDestroyGLXPbufferSGIX sglXDestroyGLXPbufferSGIX = 0;
static PsglXQueryGLXPbufferSGIX sglXQueryGLXPbufferSGIX = 0;
static PsglXSelectEventSGIX sglXSelectEventSGIX = 0;
static PsglXGetSelectedEventSGIX sglXGetSelectedEventSGIX = 0;
//----------------------------------------------------------------------------
GLXPbufferSGIX GTglXCreateGLXPbufferSGIX (Display* pkDisplay,
    GLXFBConfigSGIX kConfig, unsigned int uiWidth, unsigned int uiHeight,
    int* aiAttributeList)
{
    GT_BODY_A5_RET(sglXCreateGLXPbufferSGIX,pkDisplay,kConfig,uiWidth,
        uiHeight,aiAttributeList,GLXPbufferSGIX,0);
}
//----------------------------------------------------------------------------
void GTglXDestroyGLXPbufferSGIX (Display* pkDisplay, GLXPbufferSGIX kPBuffer)
{
    GT_BODY_A2(sglXDestroyGLXPbufferSGIX,pkDisplay,kPBuffer);
}
//----------------------------------------------------------------------------
int GTglXQueryGLXPbufferSGIX (Display* pkDisplay, GLXPbufferSGIX kPBuffer,
    int iAttribute, unsigned int* auiValue)
{
    GT_BODY_A4_RET(sglXQueryGLXPbufferSGIX,pkDisplay,kPBuffer,iAttribute,
        auiValue,int,0);
}
//----------------------------------------------------------------------------
void GTglXSelectEventSGIX (Display* pkDisplay, GLXDrawable kDrawable,
    unsigned long ulMask)
{
    GT_BODY_A3(sglXSelectEventSGIX,pkDisplay,kDrawable,ulMask);
}
//----------------------------------------------------------------------------
void GTglXGetSelectedEventSGIX (Display* pkDisplay, GLXDrawable kDrawable,
    unsigned long* pulMask)
{
    GT_BODY_A3(sglXGetSelectedEventSGIX,pkDisplay,kDrawable,pulMask);
}
//----------------------------------------------------------------------------
void GTInitGlxSgixPBuffer ()
{
    if (GTSupportsExtension("GLX_SGIX_pbuffer"))
    {
        GTExistsGlxSgixPBuffer = 1;
        GT_GET_FUNCTION(sglXCreateGLXPbufferSGIX);
        GT_GET_FUNCTION(sglXDestroyGLXPbufferSGIX);
        GT_GET_FUNCTION(sglXQueryGLXPbufferSGIX);
        GT_GET_FUNCTION(sglXSelectEventSGIX);
        GT_GET_FUNCTION(sglXGetSelectedEventSGIX);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Initialization of the GLX system
//----------------------------------------------------------------------------
void GTInitGLX (const char* acInfoFile)
{
    GTInitGL(acInfoFile);

    GTInitGlxArbGetProcAddress();
    GTInitGlxArbGetMultisample();
    GTInitGlxArbFBConfigFloat();

    GTInitGlxExtVisualInfo();
    GTInitGlxExtVisualRating();
    GTInitGlxExtImportContext();
    GTInitGlxSgixFBConfig();
    GTInitGlxSgixPBuffer();
}
//----------------------------------------------------------------------------
