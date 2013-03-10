// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5WglExtensions.h"

#if defined(_MSC_VER)
// Disable the warning about "conditional expression is constant".  Without
// this, my macros will generate about 10 warnings.
#pragma warning( disable : 4127 )
#endif

//----------------------------------------------------------------------------
void* GTGetFunctionPointer (const char* acFunction)
{
    return (void*)wglGetProcAddress(acFunction);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** ARB Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 04) WGL_ARB_buffer_region
//----------------------------------------------------------------------------
int GTExistsWglArbBufferRegion = 0;

typedef HANDLE (STDCALL* PswglCreateBufferRegionARB) (HDC, int, UINT);
typedef VOID (STDCALL* PswglDeleteBufferRegionARB) (HANDLE);
typedef BOOL (STDCALL* PswglSaveBufferRegionARB) (HANDLE, int, int, int, int);
typedef BOOL (STDCALL* PswglRestoreBufferRegionARB) (HANDLE, int, int, int,
    int, int, int);

static PswglCreateBufferRegionARB swglCreateBufferRegionARB = 0;
static PswglDeleteBufferRegionARB swglDeleteBufferRegionARB = 0;
static PswglSaveBufferRegionARB swglSaveBufferRegionARB = 0;
static PswglRestoreBufferRegionARB swglRestoreBufferRegionARB = 0;
//----------------------------------------------------------------------------
HANDLE GTwglCreateBufferRegionARB (HDC hDC, int iLayerPlane, UINT uiType)
{
    GT_BODY_A3_RET(swglCreateBufferRegionARB,hDC,iLayerPlane,uiType,
        HANDLE,0);
}
//----------------------------------------------------------------------------
VOID GTwglDeleteBufferRegionARB (HANDLE hRegion)
{
    GT_BODY_A1(swglDeleteBufferRegionARB,hRegion);
}
//----------------------------------------------------------------------------
BOOL GTwglSaveBufferRegionARB (HANDLE hRegion, int iX, int iY, int iWidth,
    int iHeight)
{
    GT_BODY_A5_RET(swglSaveBufferRegionARB,hRegion,iX,iY,iWidth,iHeight,
        BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglRestoreBufferRegionARB (HANDLE hRegion, int iX, int iY, int iWidth,
    int iHeight, int iXSrc, int iYSrc)
{
    GT_BODY_A7_RET(swglRestoreBufferRegionARB,hRegion,iX,iY,iWidth,iHeight,
        iXSrc,iYSrc,BOOL,FALSE);
}
//----------------------------------------------------------------------------
void GTInitWglArbBufferRegion ()
{
    if (GTSupportsWglExtension("WGL_ARB_buffer_region")
    ||  GTSupportsExtension("WGL_ARB_buffer_region"))
    {
        GTExistsWglArbBufferRegion = 1;
        GT_GET_FUNCTION(swglCreateBufferRegionARB);
        GT_GET_FUNCTION(swglDeleteBufferRegionARB);
        GT_GET_FUNCTION(swglSaveBufferRegionARB);
        GT_GET_FUNCTION(swglRestoreBufferRegionARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 05) WGL_ARB_multisample
//----------------------------------------------------------------------------
int GTExistsWglArbMultisample = 0;
//----------------------------------------------------------------------------
void GTInitWglArbMultisample ()
{
    if (GTSupportsWglExtension("WGL_ARB_multisample")
    ||  GTSupportsExtension("WGL_ARB_multisample"))
    {
        GTExistsWglArbMultisample = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 08) WGL_ARB_extensions_string
//----------------------------------------------------------------------------
int GTExistsWglArbExtensionsString = 0;

typedef const char* (STDCALL* PswglGetExtensionsStringARB) (HDC);

static PswglGetExtensionsStringARB swglGetExtensionsStringARB = 0;
//----------------------------------------------------------------------------
const char* GTwglGetExtensionsStringARB (HDC hDC)
{
    GT_BODY_A1_RET(swglGetExtensionsStringARB,hDC,const char*,0);
}
//----------------------------------------------------------------------------
void GTInitWglArbExtensionsString ()
{
	GT_GET_FUNCTION(swglGetExtensionsStringARB);
	if (swglGetExtensionsStringARB)
    {
        GTExistsWglArbExtensionsString = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 09) WGL_ARB_pixel_format
//----------------------------------------------------------------------------
int GTExistsWglArbPixelFormat = 0;

typedef BOOL (STDCALL* PswglGetPixelFormatAttribivARB) (HDC, int, int, UINT,
    const int*, int*);
typedef BOOL (STDCALL* PswglGetPixelFormatAttribfvARB) (HDC, int, int, UINT,
    const int*, FLOAT*);
typedef BOOL (STDCALL* PswglChoosePixelFormatARB) (HDC, const int*,
    const FLOAT*, UINT, int*, UINT*);

static PswglGetPixelFormatAttribivARB swglGetPixelFormatAttribivARB = 0;
static PswglGetPixelFormatAttribfvARB swglGetPixelFormatAttribfvARB = 0;
static PswglChoosePixelFormatARB swglChoosePixelFormatARB = 0;
//----------------------------------------------------------------------------
BOOL GTwglGetPixelFormatAttribivARB (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    int* aiValue)
{
    GT_BODY_A6_RET(swglGetPixelFormatAttribivARB,hDC,iPixelFormat,iLayerPlane,
        uiNumAttributes,aiAttribute,aiValue,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglGetPixelFormatAttribfvARB (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    FLOAT* afValue)
{
    GT_BODY_A6_RET(swglGetPixelFormatAttribfvARB,hDC,iPixelFormat,iLayerPlane,
        uiNumAttributes,aiAttribute,afValue,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglChoosePixelFormatARB (HDC hDC, const int* aiAttribIList,
    const FLOAT* afAttribFList, UINT uiMaxFormats, int* aiFormat,
    UINT* puiNumFormats)
{
    GT_BODY_A6_RET(swglChoosePixelFormatARB,hDC,aiAttribIList,afAttribFList,
        uiMaxFormats,aiFormat,puiNumFormats,BOOL,FALSE);
}
//----------------------------------------------------------------------------
void GTInitWglArbPixelFormat ()
{
    if (GTSupportsWglExtension("WGL_ARB_pixel_format")
    ||  GTSupportsExtension("WGL_ARB_pixel_format"))
    {
        GTExistsWglArbPixelFormat = 1;
        GT_GET_FUNCTION(swglGetPixelFormatAttribivARB);
        GT_GET_FUNCTION(swglGetPixelFormatAttribfvARB);
        GT_GET_FUNCTION(swglChoosePixelFormatARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 10) WGL_ARB_make_current_read
//----------------------------------------------------------------------------
int GTExistsWglArbMakeCurrentRead = 0;

typedef BOOL (STDCALL* PswglMakeContextCurrentARB) (HDC, HDC, HGLRC);
typedef HDC (STDCALL* PswglGetCurrentReadDCARB) ();

static PswglMakeContextCurrentARB swglMakeContextCurrentARB = 0;
static PswglGetCurrentReadDCARB swglGetCurrentReadDCARB = 0;
//----------------------------------------------------------------------------
BOOL GTwglMakeContextCurrentARB (HDC hDrawDC, HDC hReadDC, HGLRC hWindowRC)
{
    GT_BODY_A3_RET(swglMakeContextCurrentARB,hDrawDC,hReadDC,hWindowRC,
        BOOL,FALSE);
}
//----------------------------------------------------------------------------
HDC GTwglGetCurrentReadDCARB ()
{
    GT_BODY_A0_RET(swglGetCurrentReadDCARB,HDC,0);
}
//----------------------------------------------------------------------------
void GTInitWglArbMakeCurrentRead ()
{
    if (GTSupportsWglExtension("WGL_ARB_make_current_read")
    ||  GTSupportsExtension("WGL_ARB_make_current_read"))
    {
        GTExistsWglArbMakeCurrentRead = 1;
        GT_GET_FUNCTION(swglMakeContextCurrentARB);
        GT_GET_FUNCTION(swglGetCurrentReadDCARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 11) WGL_ARB_pbuffer
//----------------------------------------------------------------------------
int GTExistsWglArbPBuffer = 0;

typedef HPBUFFERARB (STDCALL* PswglCreatePbufferARB) (HDC, int, int, int,
    const int*);
typedef HDC (STDCALL* PswglGetPbufferDCARB) (HPBUFFERARB);
typedef int (STDCALL* PswglReleasePbufferDCARB) (HPBUFFERARB, HDC);
typedef BOOL (STDCALL* PswglDestroyPbufferARB) (HPBUFFERARB);
typedef BOOL (STDCALL* PswglQueryPbufferARB) (HPBUFFERARB, int, int*);

static PswglCreatePbufferARB swglCreatePbufferARB = 0;
static PswglGetPbufferDCARB swglGetPbufferDCARB = 0;
static PswglReleasePbufferDCARB swglReleasePbufferDCARB = 0;
static PswglDestroyPbufferARB swglDestroyPbufferARB = 0;
static PswglQueryPbufferARB swglQueryPbufferARB = 0;
//----------------------------------------------------------------------------
HPBUFFERARB GTwglCreatePbufferARB (HDC hDC, int iPixelFormat, int iWidth,
    int iHeight, const int* aiAttribList)
{
    GT_BODY_A5_RET(swglCreatePbufferARB,hDC,iPixelFormat,iWidth,iHeight,
        aiAttribList,HPBUFFERARB,0);
}
//----------------------------------------------------------------------------
HDC GTwglGetPbufferDCARB (HPBUFFERARB hPBuffer)
{
    GT_BODY_A1_RET(swglGetPbufferDCARB,hPBuffer,HDC,0);
}
//----------------------------------------------------------------------------
int GTwglReleasePbufferDCARB (HPBUFFERARB hPBuffer, HDC hDC)
{
    GT_BODY_A2_RET(swglReleasePbufferDCARB,hPBuffer,hDC,int,0);
}
//----------------------------------------------------------------------------
BOOL GTwglDestroyPbufferARB (HPBUFFERARB hPBuffer)
{
    GT_BODY_A1_RET(swglDestroyPbufferARB,hPBuffer,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglQueryPbufferARB (HPBUFFERARB hPBuffer, int iAttribute,
    int* piValue)
{
    GT_BODY_A3_RET(swglQueryPbufferARB,hPBuffer,iAttribute,piValue,BOOL,
        FALSE);
}
//----------------------------------------------------------------------------
void GTInitWglArbPBuffer ()
{
    if (GTSupportsWglExtension("WGL_ARB_pbuffer")
     || GTSupportsExtension("WGL_ARB_pbuffer"))
    {
        GTExistsWglArbPBuffer = 1;
        GT_GET_FUNCTION(swglCreatePbufferARB);
        GT_GET_FUNCTION(swglGetPbufferDCARB);
        GT_GET_FUNCTION(swglReleasePbufferDCARB);
        GT_GET_FUNCTION(swglDestroyPbufferARB);
        GT_GET_FUNCTION(swglQueryPbufferARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 20) WGL_ARB_render_texture
//----------------------------------------------------------------------------
int GTExistsWglArbRenderTexture = 0;

typedef BOOL (STDCALL* PswglBindTexImageARB) (HPBUFFERARB, int);
typedef BOOL (STDCALL* PswglReleaseTexImageARB) (HPBUFFERARB, int);
typedef BOOL (STDCALL* PswglSetPbufferAttribARB) (HPBUFFERARB, const int*);

static PswglBindTexImageARB swglBindTexImageARB = 0;
static PswglReleaseTexImageARB swglReleaseTexImageARB = 0;
static PswglSetPbufferAttribARB swglSetPbufferAttribARB = 0;
//----------------------------------------------------------------------------
BOOL GTwglBindTexImageARB (HPBUFFERARB hPBuffer, int iBuffer)
{
    GT_BODY_A2_RET(swglBindTexImageARB,hPBuffer,iBuffer,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglReleaseTexImageARB (HPBUFFERARB hPBuffer, int iBuffer)
{
    GT_BODY_A2_RET(swglReleaseTexImageARB,hPBuffer,iBuffer,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglSetPbufferAttribARB (HPBUFFERARB hPBuffer, const int* aiAttribList)
{
    GT_BODY_A2_RET(swglSetPbufferAttribARB,hPBuffer,aiAttribList,BOOL,FALSE);
}
//----------------------------------------------------------------------------
void GTInitWglArbRenderTexture ()
{
    if (GTSupportsWglExtension("WGL_ARB_render_texture")
    ||  GTSupportsExtension("WGL_ARB_render_texture"))
    {
        GTExistsWglArbRenderTexture = 1;
        GT_GET_FUNCTION(swglBindTexImageARB);
        GT_GET_FUNCTION(swglReleaseTexImageARB);
        GT_GET_FUNCTION(swglSetPbufferAttribARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 39) WGL_ARB_pixel_format_float
//----------------------------------------------------------------------------
int GTExistsWglArbPixelFormatFloat = 0;
//----------------------------------------------------------------------------
void GTInitWglArbPixelFormatFloat ()
{
    if (GTSupportsWglExtension("WGL_ARB_pixel_format_float")
    ||  GTSupportsExtension("WGL_ARB_pixel_format_float"))
    {
        GTExistsWglArbPixelFormatFloat = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** EXT Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 167) WGL_EXT_display_color_table
//----------------------------------------------------------------------------
int GTExistsWglExtDisplayColorTable = 0;

typedef GLboolean (STDCALL* PswglCreateDisplayColorTableEXT) (GLushort);
typedef GLboolean (STDCALL* PswglLoadDisplayColorTableEXT) (const GLushort*,
    GLuint);
typedef GLboolean (STDCALL* PswglBindDisplayColorTableEXT) (GLushort);
typedef VOID (STDCALL* PswglDestroyDisplayColorTableEXT) (GLushort);

static PswglCreateDisplayColorTableEXT swglCreateDisplayColorTableEXT = 0;
static PswglLoadDisplayColorTableEXT swglLoadDisplayColorTableEXT = 0;
static PswglBindDisplayColorTableEXT swglBindDisplayColorTableEXT = 0;
static PswglDestroyDisplayColorTableEXT swglDestroyDisplayColorTableEXT = 0;
//----------------------------------------------------------------------------
GLboolean GTwglCreateDisplayColorTableEXT (GLushort usID)
{
    GT_BODY_A1_RET(swglCreateDisplayColorTableEXT,usID,GLboolean,0);
}
//----------------------------------------------------------------------------
GLboolean GTwglLoadDisplayColorTableEXT (const GLushort* ausTable,
    GLuint uiLength)
{
    GT_BODY_A2_RET(swglLoadDisplayColorTableEXT,ausTable,uiLength,GLboolean,
        0);
}
//----------------------------------------------------------------------------
GLboolean GTwglBindDisplayColorTableEXT (GLushort usID)
{
    GT_BODY_A1_RET(swglBindDisplayColorTableEXT,usID,GLboolean,0);
}
//----------------------------------------------------------------------------
VOID GTwglDestroyDisplayColorTableEXT (GLushort usID)
{
    GT_BODY_A1(swglDestroyDisplayColorTableEXT,usID);
}
//----------------------------------------------------------------------------
void GTInitWglExtDisplayColorTable ()
{
    if (GTSupportsWglExtension("WGL_EXT_display_color_table")
    ||  GTSupportsExtension("WGL_EXT_display_color_table"))
    {
        GTExistsWglExtDisplayColorTable = 1;
        GT_GET_FUNCTION(swglCreateDisplayColorTableEXT);
        GT_GET_FUNCTION(swglLoadDisplayColorTableEXT);
        GT_GET_FUNCTION(swglBindDisplayColorTableEXT);
        GT_GET_FUNCTION(swglDestroyDisplayColorTableEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 168) WGL_EXT_extensions_string
//----------------------------------------------------------------------------
int GTExistsWglExtExtensionsString = 0;

typedef const char* (STDCALL* PswglGetExtensionsStringEXT) ();

static PswglGetExtensionsStringEXT swglGetExtensionsStringEXT = 0;
//----------------------------------------------------------------------------
const char* GTwglGetExtensionsStringEXT ()
{
    GT_BODY_A0_RET(swglGetExtensionsStringEXT,const char*,0);
}
//----------------------------------------------------------------------------
void GTInitWglExtExtensionsString ()
{
	GT_GET_FUNCTION(swglGetExtensionsStringEXT);
	if (swglGetExtensionsStringEXT)
	{
        GTExistsWglExtExtensionsString = 1;
	}
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 169) WGL_EXT_make_current_read
//----------------------------------------------------------------------------
int GTExistsWglExtMakeCurrentRead = 0;

typedef BOOL (STDCALL* PswglMakeContextCurrentEXT) (HDC, HDC, HGLRC);
typedef HDC (STDCALL* PswglGetCurrentReadDCEXT) ();

static PswglMakeContextCurrentEXT swglMakeContextCurrentEXT = 0;
static PswglGetCurrentReadDCEXT swglGetCurrentReadDCEXT = 0;
//----------------------------------------------------------------------------
BOOL GTwglMakeContextCurrentEXT (HDC hDrawDC, HDC hReadDC, HGLRC hWindowRC)
{
    GT_BODY_A3_RET(swglMakeContextCurrentEXT,hDrawDC,hReadDC,hWindowRC,BOOL,
        FALSE);
}
//----------------------------------------------------------------------------
HDC GTwglGetCurrentReadDCEXT ()
{
    GT_BODY_A0_RET(swglGetCurrentReadDCEXT,HDC,0);
}
//----------------------------------------------------------------------------
void GTInitWglExtMakeCurrentRead ()
{
    if (GTSupportsWglExtension("WGL_EXT_make_current_read")
    ||  GTSupportsExtension("WGL_EXT_make_current_read"))
    {
        GTExistsWglExtMakeCurrentRead = 1;

        GT_GET_FUNCTION(swglMakeContextCurrentEXT);
        if (GT_EXT_OVERRIDE && !swglMakeContextCurrentARB)
        {
            swglMakeContextCurrentARB =
                (PswglMakeContextCurrentARB)swglMakeContextCurrentEXT;
        }

        GT_GET_FUNCTION(swglGetCurrentReadDCEXT);
        if (GT_EXT_OVERRIDE && !swglGetCurrentReadDCARB)
        {
            swglGetCurrentReadDCARB =
                (PswglGetCurrentReadDCARB)swglGetCurrentReadDCEXT;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 170) WGL_EXT_pixel_format
//----------------------------------------------------------------------------
int GTExistsWglExtPixelFormat = 0;

typedef BOOL (STDCALL* PswglGetPixelFormatAttribivEXT) (HDC, int, int, UINT,
    const int*, int*);
typedef BOOL (STDCALL* PswglGetPixelFormatAttribfvEXT) (HDC, int, int, UINT,
    const int*, FLOAT*);
typedef BOOL (STDCALL* PswglChoosePixelFormatEXT) (HDC, const int*,
    const FLOAT*, UINT, int*, UINT*);

static PswglGetPixelFormatAttribivEXT swglGetPixelFormatAttribivEXT = 0;
static PswglGetPixelFormatAttribfvEXT swglGetPixelFormatAttribfvEXT = 0;
static PswglChoosePixelFormatEXT swglChoosePixelFormatEXT = 0;
//----------------------------------------------------------------------------
BOOL GTwglGetPixelFormatAttribivEXT (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    int* aiValue)
{
    GT_BODY_A6_RET(swglGetPixelFormatAttribivEXT,hDC,iPixelFormat,iLayerPlane,
        uiNumAttributes,aiAttribute,aiValue,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglGetPixelFormatAttribfvEXT (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    FLOAT* afValue)
{
    GT_BODY_A6_RET(swglGetPixelFormatAttribfvEXT,hDC,iPixelFormat,iLayerPlane,
        uiNumAttributes,aiAttribute,afValue,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglChoosePixelFormatEXT (HDC hDC, const int* aiAttribIList,
    const FLOAT* afAttribFList, UINT uiMaxFormats, int* aiFormat,
    UINT* puiNumFormats)
{
    GT_BODY_A6_RET(swglChoosePixelFormatEXT,hDC,aiAttribIList,afAttribFList,
        uiMaxFormats,aiFormat,puiNumFormats,BOOL,FALSE);
}
//----------------------------------------------------------------------------
void GTInitWglExtPixelFormat ()
{
    if (GTSupportsWglExtension("WGL_EXT_pixel_format")
    ||  GTSupportsExtension("WGL_EXT_pixel_format"))
    {
        GTExistsWglExtPixelFormat = 1;

        GT_GET_FUNCTION(swglGetPixelFormatAttribivEXT);
        if (GT_EXT_OVERRIDE && !swglGetPixelFormatAttribivARB)
        {
            swglGetPixelFormatAttribivARB =
                (PswglGetPixelFormatAttribivARB)swglGetPixelFormatAttribivEXT;
        }

        GT_GET_FUNCTION(swglGetPixelFormatAttribfvEXT);
        if (GT_EXT_OVERRIDE && !swglGetPixelFormatAttribfvARB)
        {
            swglGetPixelFormatAttribfvARB =
                (PswglGetPixelFormatAttribfvARB)swglGetPixelFormatAttribfvEXT;
        }

        GT_GET_FUNCTION(swglChoosePixelFormatEXT);
        if (GT_EXT_OVERRIDE && !swglChoosePixelFormatARB)
        {
            swglChoosePixelFormatARB =
                (PswglChoosePixelFormatARB)swglChoosePixelFormatEXT;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 171) WGL_EXT_pbuffer
//----------------------------------------------------------------------------
int GTExistsWglExtPBuffer = 0;

typedef HPBUFFEREXT (STDCALL* PswglCreatePbufferEXT) (HDC, int, int, int,
    const int*);
typedef HDC (STDCALL* PswglGetPbufferDCEXT) (HPBUFFEREXT);
typedef int (STDCALL* PswglReleasePbufferDCEXT) (HPBUFFEREXT, HDC);
typedef BOOL (STDCALL* PswglDestroyPbufferEXT) (HPBUFFEREXT);
typedef BOOL (STDCALL* PswglQueryPbufferEXT) (HPBUFFEREXT, int, int*);

static PswglCreatePbufferEXT swglCreatePbufferEXT = 0;
static PswglGetPbufferDCEXT swglGetPbufferDCEXT = 0;
static PswglReleasePbufferDCEXT swglReleasePbufferDCEXT = 0;
static PswglDestroyPbufferEXT swglDestroyPbufferEXT = 0;
static PswglQueryPbufferEXT swglQueryPbufferEXT = 0;
//----------------------------------------------------------------------------
HPBUFFEREXT GTwglCreatePbufferEXT (HDC hDC, int iPixelFormat, int iWidth,
    int iHeight, const int* aiAttribList)
{
    GT_BODY_A5_RET(swglCreatePbufferEXT,hDC,iPixelFormat,iWidth,iHeight,
        aiAttribList,HPBUFFEREXT,0);
}
//----------------------------------------------------------------------------
HDC GTwglGetPbufferDCEXT (HPBUFFEREXT hPBuffer)
{
    GT_BODY_A1_RET(swglGetPbufferDCEXT,hPBuffer,HDC,0);
}
//----------------------------------------------------------------------------
int GTwglReleasePbufferDCEXT (HPBUFFEREXT hPBuffer, HDC hDC)
{
    GT_BODY_A2_RET(swglReleasePbufferDCEXT,hPBuffer,hDC,int,0);
}
//----------------------------------------------------------------------------
BOOL GTwglDestroyPbufferEXT (HPBUFFEREXT hPBuffer)
{
    GT_BODY_A1_RET(swglDestroyPbufferEXT,hPBuffer,BOOL,FALSE);
}
//----------------------------------------------------------------------------
BOOL GTwglQueryPbufferEXT (HPBUFFEREXT hPBuffer, int iAttribute,
    int* piValue)
{
    GT_BODY_A3_RET(swglQueryPbufferEXT,hPBuffer,iAttribute,piValue,BOOL,
        FALSE);
}
//----------------------------------------------------------------------------
void GTInitWglExtPBuffer ()
{
    if (GTSupportsWglExtension("WGL_EXT_pbuffer")
    ||  GTSupportsExtension("WGL_EXT_pbuffer"))
    {
        GTExistsWglExtPBuffer = 1;

        GT_GET_FUNCTION(swglCreatePbufferEXT);
        if (GT_EXT_OVERRIDE && !swglCreatePbufferARB)
        {
            swglCreatePbufferARB =
                (PswglCreatePbufferARB)swglCreatePbufferEXT;
        }

        GT_GET_FUNCTION(swglGetPbufferDCEXT);
        if (GT_EXT_OVERRIDE && !swglGetPbufferDCARB)
        {
            swglGetPbufferDCARB =
                (PswglGetPbufferDCARB)swglGetPbufferDCEXT;
        }

        GT_GET_FUNCTION(swglReleasePbufferDCEXT);
        if (GT_EXT_OVERRIDE && !swglReleasePbufferDCARB)
        {
            swglReleasePbufferDCARB =
                (PswglReleasePbufferDCARB)swglReleasePbufferDCEXT;
        }

        GT_GET_FUNCTION(swglDestroyPbufferEXT);
        if (GT_EXT_OVERRIDE && !swglDestroyPbufferARB)
        {
            swglDestroyPbufferARB =
                (PswglDestroyPbufferARB)swglDestroyPbufferEXT;
        }

        GT_GET_FUNCTION(swglQueryPbufferEXT);
        if (GT_EXT_OVERRIDE && !swglQueryPbufferARB)
        {
            swglQueryPbufferARB =
                (PswglQueryPbufferARB)swglQueryPbufferEXT;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 172) WGL_EXT_swap_control
//----------------------------------------------------------------------------
int GTExistsWglExtSwapControl = 0;

typedef BOOL (STDCALL* PswglSwapIntervalEXT) (int);
typedef int (STDCALL* PswglGetSwapIntervalEXT) ();

static PswglSwapIntervalEXT swglSwapIntervalEXT = 0;
static PswglGetSwapIntervalEXT swglGetSwapIntervalEXT = 0;
//----------------------------------------------------------------------------
BOOL GTwglSwapIntervalEXT (int iInterval)
{
    BOOL tResult;
    GT_ENTER_FUNCTION(swglSwapIntervalEXT);
    if (swglSwapIntervalEXT)
    {
        // The application can run with vsync disabled.
        GT_NONNULL_FUNCTION(swglSwapIntervalEXT);
        tResult = swglSwapIntervalEXT(iInterval);
    }
    else
    {
        // The application must run with vsync enabled.  Do not use the
        // macro GT_NULL_FUNCTION(swglSwapIntervalEXT) here.
        tResult = FALSE;
    }
    GT_EXIT_FUNCTION(swglSwapIntervalEXT);
    return tResult;
}
//----------------------------------------------------------------------------
int GTwglGetSwapIntervalEXT ()
{
    GT_BODY_A0_RET(swglGetSwapIntervalEXT,int,0);
}
//----------------------------------------------------------------------------
void GTInitWglExtSwapControl ()
{
    if (GTSupportsWglExtension("WGL_EXT_swap_control")
    ||  GTSupportsExtension("WGL_EXT_swap_control"))
    {
        GTExistsWglExtSwapControl = 1;
        GT_GET_FUNCTION(swglSwapIntervalEXT);
        GT_GET_FUNCTION(swglGetSwapIntervalEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 177) WGL_EXT_depth_float
//----------------------------------------------------------------------------
int GTExistsWglExtDepthFloat = 0;
//----------------------------------------------------------------------------
void GTInitWglExtDepthFloat ()
{
    if (GTSupportsWglExtension("WGL_EXT_depth_float")
    ||  GTSupportsExtension("WGL_EXT_depth_float"))
    {
        GTExistsWglExtDepthFloat = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 209) WGL_EXT_multisample
//----------------------------------------------------------------------------
int GTExistsWglExtMultisample = 0;
//----------------------------------------------------------------------------
void GTInitWglExtMultisample ()
{
    if (GTSupportsWglExtension("WGL_EXT_multisample")
    ||  GTSupportsExtension("WGL_EXT_multisample"))
    {
        GTExistsWglExtMultisample = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Initialization of the WGL system
//----------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>

// Disable the MSVC8 warnings about deprecated functions.
#pragma warning( push )
#pragma warning( disable : 4996 )

void GTInitWGL (const char* acInfoFile)
{
    GTInitGL(acInfoFile);

    GTInitWglArbExtensionsString();
    GTInitWglExtExtensionsString();

	if (acInfoFile)
    {
        FILE* pkInfo = fopen(acInfoFile,"at");
        if (pkInfo)
        {
            const char* acWglExtensions;

            acWglExtensions = GTGetWglExtensionString();
            if (acWglExtensions)
            {
                char* acExt;
                char* pcToken;

                fprintf(pkInfo,"WGLextensions =\n");
                acExt = (char*)malloc((int)strlen(acWglExtensions)+1);
                strcpy(acExt,acWglExtensions);
                pcToken = strtok(acExt," \t\n");
                while (pcToken)
                {
                    fprintf(pkInfo,"    %s\n",pcToken);
                    pcToken = strtok(0," \t\n");
                }
                free(acExt);
            }
            else
            {
                fprintf(pkInfo,"WGLextensions = <null>\n");
            }

            fclose(pkInfo);
        }
    }

    GTInitWglArbBufferRegion();
    GTInitWglArbMultisample();
    GTInitWglArbPixelFormat();
    GTInitWglArbMakeCurrentRead();
    GTInitWglArbPBuffer();
    GTInitWglArbRenderTexture();
    GTInitWglArbPixelFormatFloat();

    GTInitWglExtDisplayColorTable();
    GTInitWglExtMakeCurrentRead();
    GTInitWglExtPixelFormat();
    GTInitWglExtPBuffer();
    GTInitWglExtSwapControl();
    GTInitWglExtDepthFloat();
    GTInitWglExtMultisample();
}
#pragma warning( pop )
//----------------------------------------------------------------------------
const char* GTGetWglExtensionString ()
{
	if (GTExistsWglExtExtensionsString && GT_EXT_OVERRIDE)
    {
		return GTwglGetExtensionsStringEXT();
    }
	else if (GTExistsWglArbExtensionsString)
    {
		return GTwglGetExtensionsStringARB(wglGetCurrentDC());
    }
	else
    {
		return 0;
    }
}
//----------------------------------------------------------------------------
int GTSupportsWglExtension (const char* acWglExtension)
{
    const char* acWglExtensionString;
    char* acBegin;
    char cEnd;

    if (!acWglExtension)
    {
        return 0;
    }

    acWglExtensionString = GTGetWglExtensionString();
    if (!acWglExtensionString)
    {
        return 0;
    }

    acBegin = (char*)strstr(acWglExtensionString,acWglExtension);
    if (!acBegin)
    {
        return 0;
    }

    // The extension was found, but make sure it is not a proper substring
    // of another extension.
    cEnd = *(acBegin + strlen(acWglExtension));
    return cEnd == ' ' || cEnd == 0 || cEnd == '\t' || cEnd == '\n';
}
//----------------------------------------------------------------------------
