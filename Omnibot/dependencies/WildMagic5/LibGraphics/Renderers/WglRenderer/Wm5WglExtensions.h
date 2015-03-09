// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/05/03)

#ifndef WM5WGLEXTENSIONS_H
#define WM5WGLEXTENSIONS_H

#include "Wm5GlExtensions.h"

// Disable various subsystems in windows.h.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

// Disable the 'min' and 'max' macros that are sucked in by the inclusion
// of windows.h.  These conflict with std::numeric_limits<type>::max().
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

extern "C"
{

//----------------------------------------------------------------------------
// ***** ARB Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 04) WGL_ARB_buffer_region
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbBufferRegion;

#define WGL_FRONT_COLOR_BUFFER_BIT_ARB                              0x00000001
#define WGL_BACK_COLOR_BUFFER_BIT_ARB                               0x00000002
#define WGL_DEPTH_BUFFER_BIT_ARB                                    0x00000004
#define WGL_STENCIL_BUFFER_BIT_ARB                                  0x00000008

#define wglCreateBufferRegionARB GTwglCreateBufferRegionARB
#define wglDeleteBufferRegionARB GTwglDeleteBufferRegionARB
#define wglSaveBufferRegionARB GTwglSaveBufferRegionARB
#define wglRestoreBufferRegionARB GTwglRestoreBufferRegionARB

HANDLE GTwglCreateBufferRegionARB (HDC hDC, int iLayerPlane, UINT uiType);
GT_EXTERN VOID GTwglDeleteBufferRegionARB (HANDLE hRegion);
GT_EXTERN BOOL GTwglSaveBufferRegionARB (HANDLE hRegion, int iX, int iY,
    int iWidth, int iHeight);
GT_EXTERN BOOL GTwglRestoreBufferRegionARB (HANDLE hRegion, int iX, int iY,
    int iWidth, int iHeight, int iXSrc, int iYSrc);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 05) WGL_ARB_multisample
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbMultisample;

#define WGL_SAMPLE_BUFFERS_ARB                                      0x2041
#define WGL_SAMPLES_ARB                                             0x2042
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 08) WGL_ARB_extensions_string
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbExtensionsString;

#define wglGetExtensionsStringARB GTwglGetExtensionsStringARB

GT_EXTERN const char* GTwglGetExtensionsStringARB (HDC hDC);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 09) WGL_ARB_pixel_format
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbPixelFormat;

#define WGL_NUMBER_PIXEL_FORMATS_ARB                                0x2000
#define WGL_DRAW_TO_WINDOW_ARB                                      0x2001
#define WGL_DRAW_TO_BITMAP_ARB                                      0x2002
#define WGL_ACCELERATION_ARB                                        0x2003
#define WGL_NEED_PALETTE_ARB                                        0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB                                 0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB                                  0x2006
#define WGL_SWAP_METHOD_ARB                                         0x2007
#define WGL_NUMBER_OVERLAYS_ARB                                     0x2008
#define WGL_NUMBER_UNDERLAYS_ARB                                    0x2009
#define WGL_TRANSPARENT_ARB                                         0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB                               0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB                             0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB                              0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB                             0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB                             0x203B
#define WGL_SHARE_DEPTH_ARB                                         0x200C
#define WGL_SHARE_STENCIL_ARB                                       0x200D
#define WGL_SHARE_ACCUM_ARB                                         0x200E
#define WGL_SUPPORT_GDI_ARB                                         0x200F
#define WGL_SUPPORT_OPENGL_ARB                                      0x2010
#define WGL_DOUBLE_BUFFER_ARB                                       0x2011
#define WGL_STEREO_ARB                                              0x2012
#define WGL_PIXEL_TYPE_ARB                                          0x2013
#define WGL_COLOR_BITS_ARB                                          0x2014
#define WGL_RED_BITS_ARB                                            0x2015
#define WGL_RED_SHIFT_ARB                                           0x2016
#define WGL_GREEN_BITS_ARB                                          0x2017
#define WGL_GREEN_SHIFT_ARB                                         0x2018
#define WGL_BLUE_BITS_ARB                                           0x2019
#define WGL_BLUE_SHIFT_ARB                                          0x201A
#define WGL_ALPHA_BITS_ARB                                          0x201B
#define WGL_ALPHA_SHIFT_ARB                                         0x201C
#define WGL_ACCUM_BITS_ARB                                          0x201D
#define WGL_ACCUM_RED_BITS_ARB                                      0x201E
#define WGL_ACCUM_GREEN_BITS_ARB                                    0x201F
#define WGL_ACCUM_BLUE_BITS_ARB                                     0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB                                    0x2021
#define WGL_DEPTH_BITS_ARB                                          0x2022
#define WGL_STENCIL_BITS_ARB                                        0x2023
#define WGL_AUX_BUFFERS_ARB                                         0x2024
#define WGL_NO_ACCELERATION_ARB                                     0x2025
#define WGL_GENERIC_ACCELERATION_ARB                                0x2026
#define WGL_FULL_ACCELERATION_ARB                                   0x2027
#define WGL_SWAP_EXCHANGE_ARB                                       0x2028
#define WGL_SWAP_COPY_ARB                                           0x2029
#define WGL_SWAP_UNDEFINED_ARB                                      0x202A
#define WGL_TYPE_RGBA_ARB                                           0x202B
#define WGL_TYPE_COLORINDEX_ARB                                     0x202C

#define wglGetPixelFormatAttribivARB GTwglGetPixelFormatAttribivARB
#define wglGetPixelFormatAttribfvARB GTwglGetPixelFormatAttribfvARB
#define wglChoosePixelFormatARB GTwglChoosePixelFormatARB

GT_EXTERN BOOL GTwglGetPixelFormatAttribivARB (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    int* aiValue);
GT_EXTERN BOOL GTwglGetPixelFormatAttribfvARB (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    FLOAT* afValue);
GT_EXTERN BOOL GTwglChoosePixelFormatARB (HDC hDC, const int* aiAttribIList,
    const FLOAT* afAttribFList, UINT uiMaxFormats, int* aiFormat,
    UINT* puiNumFormats);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 10) WGL_ARB_make_current_read
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbMakeCurrentRead;

#define ERROR_INVALID_PIXEL_TYPE_ARB                                0x2043
#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB                      0x2054

#define wglMakeContextCurrentARB GTwglMakeContextCurrentARB
#define wglGetCurrentReadDCARB GTwglGetCurrentReadDCARB

GT_EXTERN BOOL GTwglMakeContextCurrentARB (HDC hDrawDC, HDC hReadDC,
    HGLRC hWindowRC);
GT_EXTERN HDC GTwglGetCurrentReadDCARB ();
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 11) WGL_ARB_pbuffer
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbPBuffer;

#define WGL_DRAW_TO_PBUFFER_ARB                                     0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB                                  0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB                                   0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB                                  0x2030
#define WGL_PBUFFER_LARGEST_ARB                                     0x2033
#define WGL_PBUFFER_WIDTH_ARB                                       0x2034
#define WGL_PBUFFER_HEIGHT_ARB                                      0x2035
#define WGL_PBUFFER_LOST_ARB                                        0x2036
DECLARE_HANDLE(HPBUFFERARB);

#define wglCreatePbufferARB GTwglCreatePbufferARB
#define wglGetPbufferDCARB GTwglGetPbufferDCARB
#define wglReleasePbufferDCARB GTwglReleasePbufferDCARB
#define wglDestroyPbufferARB GTwglDestroyPbufferARB
#define wglQueryPbufferARB GTwglQueryPbufferARB

GT_EXTERN HPBUFFERARB GTwglCreatePbufferARB (HDC hDC, int iPixelFormat,
    int iWidth, int iHeight, const int* aiAttribList);
GT_EXTERN HDC GTwglGetPbufferDCARB (HPBUFFERARB hPBuffer);
GT_EXTERN int GTwglReleasePbufferDCARB (HPBUFFERARB hPBuffer, HDC hDC);
GT_EXTERN BOOL GTwglDestroyPbufferARB (HPBUFFERARB hPBuffer);
GT_EXTERN BOOL GTwglQueryPbufferARB (HPBUFFERARB hPBuffer, int iAttribute,
    int* piValue);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 20) WGL_ARB_render_texture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbRenderTexture;

#define WGL_BIND_TO_TEXTURE_RGB_ARB                                 0x2070
#define WGL_BIND_TO_TEXTURE_RGBA_ARB                                0x2071
#define WGL_TEXTURE_FORMAT_ARB                                      0x2072
#define WGL_TEXTURE_TARGET_ARB                                      0x2073
#define WGL_MIPMAP_TEXTURE_ARB                                      0x2074
#define WGL_TEXTURE_RGB_ARB                                         0x2075
#define WGL_TEXTURE_RGBA_ARB                                        0x2076
#define WGL_NO_TEXTURE_ARB                                          0x2077
#define WGL_TEXTURE_CUBE_MAP_ARB                                    0x2078
#define WGL_TEXTURE_1D_ARB                                          0x2079
#define WGL_TEXTURE_2D_ARB                                          0x207A
#define WGL_MIPMAP_LEVEL_ARB                                        0x207B
#define WGL_CUBE_MAP_FACE_ARB                                       0x207C
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB                         0x207D
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB                         0x207E
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB                         0x207F
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB                         0x2080
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB                         0x2081
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB                         0x2082
#define WGL_FRONT_LEFT_ARB                                          0x2083
#define WGL_FRONT_RIGHT_ARB                                         0x2084
#define WGL_BACK_LEFT_ARB                                           0x2085
#define WGL_BACK_RIGHT_ARB                                          0x2086
#define WGL_AUX0_ARB                                                0x2087
#define WGL_AUX1_ARB                                                0x2088
#define WGL_AUX2_ARB                                                0x2089
#define WGL_AUX3_ARB                                                0x208A
#define WGL_AUX4_ARB                                                0x208B
#define WGL_AUX5_ARB                                                0x208C
#define WGL_AUX6_ARB                                                0x208D
#define WGL_AUX7_ARB                                                0x208E
#define WGL_AUX8_ARB                                                0x208F
#define WGL_AUX9_ARB                                                0x2090

#define wglBindTexImageARB GTwglBindTexImageARB
#define wglReleaseTexImageARB GTwglReleaseTexImageARB
#define wglSetPbufferAttribARB GTwglSetPbufferAttribARB

GT_EXTERN BOOL GTwglBindTexImageARB (HPBUFFERARB hPBuffer, int iBuffer);
GT_EXTERN BOOL GTwglReleaseTexImageARB (HPBUFFERARB hPBuffer, int iBuffer);
GT_EXTERN BOOL GTwglSetPbufferAttribARB (HPBUFFERARB hPBuffer,
    const int* aiAttribList);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 39) WGL_ARB_pixel_format_float
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglArbPixelFormatFloat;

#define WGL_TYPE_RGBA_FLOAT_ARB                                     0x21A0
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** EXT Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 167) WGL_EXT_display_color_table
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtDisplayColorTable;

#define wglCreateDisplayColorTableEXT GTwglCreateDisplayColorTableEXT
#define wglLoadDisplayColorTableEXT GTwglLoadDisplayColorTableEXT
#define wglBindDisplayColorTableEXT GTwglBindDisplayColorTableEXT
#define wglDestroyDisplayColorTableEXT GTwglDestroyDisplayColorTableEXT

GT_EXTERN GLboolean GTwglCreateDisplayColorTableEXT (GLushort usID);
GT_EXTERN GLboolean GTwglLoadDisplayColorTableEXT (const GLushort* ausTable,
    GLuint uiLength);
GT_EXTERN GLboolean GTwglBindDisplayColorTableEXT (GLushort usID);
GT_EXTERN VOID GTwglDestroyDisplayColorTableEXT (GLushort usID);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 168) WGL_EXT_extensions_string
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtExtensionsString;

#define wglGetExtensionsStringEXT GTwglGetExtensionsStringEXT

GT_EXTERN const char* GTwglGetExtensionsStringEXT ();
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 169) WGL_EXT_make_current_read
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtMakeCurrentRead;

#define ERROR_INVALID_PIXEL_TYPE_EXT                                0x2043

#define wglMakeContextCurrentEXT GTwglMakeContextCurrentEXT
#define wglGetCurrentReadDCEXT GTwglGetCurrentReadDCEXT

GT_EXTERN BOOL GTwglMakeContextCurrentEXT (HDC hDrawDC, HDC hReadDC,
    HGLRC hWindowRC);
GT_EXTERN HDC GTwglGetCurrentReadDCEXT ();
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 170) WGL_EXT_pixel_format
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtPixelFormat;

#define WGL_NUMBER_PIXEL_FORMATS_EXT                                0x2000
#define WGL_DRAW_TO_WINDOW_EXT                                      0x2001
#define WGL_DRAW_TO_BITMAP_EXT                                      0x2002
#define WGL_ACCELERATION_EXT                                        0x2003
#define WGL_NEED_PALETTE_EXT                                        0x2004
#define WGL_NEED_SYSTEM_PALETTE_EXT                                 0x2005
#define WGL_SWAP_LAYER_BUFFERS_EXT                                  0x2006
#define WGL_SWAP_METHOD_EXT                                         0x2007
#define WGL_NUMBER_OVERLAYS_EXT                                     0x2008
#define WGL_NUMBER_UNDERLAYS_EXT                                    0x2009
#define WGL_TRANSPARENT_EXT                                         0x200A
#define WGL_TRANSPARENT_VALUE_EXT                                   0x200B
#define WGL_SHARE_DEPTH_EXT                                         0x200C
#define WGL_SHARE_STENCIL_EXT                                       0x200D
#define WGL_SHARE_ACCUM_EXT                                         0x200E
#define WGL_SUPPORT_GDI_EXT                                         0x200F
#define WGL_SUPPORT_OPENGL_EXT                                      0x2010
#define WGL_DOUBLE_BUFFER_EXT                                       0x2011
#define WGL_STEREO_EXT                                              0x2012
#define WGL_PIXEL_TYPE_EXT                                          0x2013
#define WGL_COLOR_BITS_EXT                                          0x2014
#define WGL_RED_BITS_EXT                                            0x2015
#define WGL_RED_SHIFT_EXT                                           0x2016
#define WGL_GREEN_BITS_EXT                                          0x2017
#define WGL_GREEN_SHIFT_EXT                                         0x2018
#define WGL_BLUE_BITS_EXT                                           0x2019
#define WGL_BLUE_SHIFT_EXT                                          0x201A
#define WGL_ALPHA_BITS_EXT                                          0x201B
#define WGL_ALPHA_SHIFT_EXT                                         0x201C
#define WGL_ACCUM_BITS_EXT                                          0x201D
#define WGL_ACCUM_RED_BITS_EXT                                      0x201E
#define WGL_ACCUM_GREEN_BITS_EXT                                    0x201F
#define WGL_ACCUM_BLUE_BITS_EXT                                     0x2020
#define WGL_ACCUM_ALPHA_BITS_EXT                                    0x2021
#define WGL_DEPTH_BITS_EXT                                          0x2022
#define WGL_STENCIL_BITS_EXT                                        0x2023
#define WGL_AUX_BUFFERS_EXT                                         0x2024
#define WGL_NO_ACCELERATION_EXT                                     0x2025
#define WGL_GENERIC_ACCELERATION_EXT                                0x2026
#define WGL_FULL_ACCELERATION_EXT                                   0x2027
#define WGL_SWAP_EXCHANGE_EXT                                       0x2028
#define WGL_SWAP_COPY_EXT                                           0x2029
#define WGL_SWAP_UNDEFINED_EXT                                      0x202A
#define WGL_TYPE_RGBA_EXT                                           0x202B
#define WGL_TYPE_COLORINDEX_EXT                                     0x202C

#define wglGetPixelFormatAttribivEXT GTwglGetPixelFormatAttribivEXT
#define wglGetPixelFormatAttribfvEXT GTwglGetPixelFormatAttribfvEXT
#define wglChoosePixelFormatEXT GTwglChoosePixelFormatEXT

GT_EXTERN BOOL GTwglGetPixelFormatAttribivEXT (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    int* aiValue);
GT_EXTERN BOOL GTwglGetPixelFormatAttribfvEXT (HDC hDC, int iPixelFormat,
    int iLayerPlane, UINT uiNumAttributes, const int* aiAttribute,
    FLOAT* afValue);
GT_EXTERN BOOL GTwglChoosePixelFormatEXT (HDC hDC, const int* aiAttribIList,
    const FLOAT* afAttribFList, UINT uiMaxFormats, int* aiFormat,
    UINT* puiNumFormats);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 171) WGL_EXT_pbuffer
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtPBuffer;

#define WGL_DRAW_TO_PBUFFER_EXT                                     0x202D
#define WGL_MAX_PBUFFER_PIXELS_EXT                                  0x202E
#define WGL_MAX_PBUFFER_WIDTH_EXT                                   0x202F
#define WGL_MAX_PBUFFER_HEIGHT_EXT                                  0x2030
#define WGL_OPTIMAL_PBUFFER_WIDTH_EXT                               0x2031
#define WGL_OPTIMAL_PBUFFER_HEIGHT_EXT                              0x2032
#define WGL_PBUFFER_LARGEST_EXT                                     0x2033
#define WGL_PBUFFER_WIDTH_EXT                                       0x2034
#define WGL_PBUFFER_HEIGHT_EXT                                      0x2035
DECLARE_HANDLE(HPBUFFEREXT);

#define wglCreatePbufferEXT GTwglCreatePbufferEXT
#define wglGetPbufferDCEXT GTwglGetPbufferDCEXT
#define wglReleasePbufferDCEXT GTwglReleasePbufferDCEXT
#define wglDestroyPbufferEXT GTwglDestroyPbufferEXT
#define wglQueryPbufferEXT GTwglQueryPbufferEXT

GT_EXTERN HPBUFFEREXT GTwglCreatePbufferEXT (HDC hDC, int iPixelFormat,
    int iWidth, int iHeight, const int* aiAttribList);
GT_EXTERN HDC GTwglGetPbufferDCEXT (HPBUFFEREXT hPBuffer);
GT_EXTERN int GTwglReleasePbufferDCEXT (HPBUFFEREXT hPBuffer, HDC hDC);
GT_EXTERN BOOL GTwglDestroyPbufferEXT (HPBUFFEREXT hPBuffer);
GT_EXTERN BOOL GTwglQueryPbufferEXT (HPBUFFEREXT hPBuffer, int iAttribute,
    int* piValue);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 172) WGL_EXT_swap_control
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtSwapControl;

#define wglSwapIntervalEXT GTwglSwapIntervalEXT
#define wglGetSwapIntervalEXT GTwglGetSwapIntervalEXT

GT_EXTERN BOOL GTwglSwapIntervalEXT (int iInterval);
GT_EXTERN int GTwglGetSwapIntervalEXT ();
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 177) WGL_EXT_depth_float
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtDepthFloat;

#define WGL_DEPTH_FLOAT_EXT                                         0x2040
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 209) WGL_EXT_multisample
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsWglExtMultisample;

#define WGL_SAMPLE_BUFFERS_EXT                                      0x2041
#define WGL_SAMPLES_EXT                                             0x2042
//----------------------------------------------------------------------------

GT_EXTERN void GTInitWGL (const char* acInfoFile);
GT_EXTERN const char* GTGetWglExtensionString ();
GT_EXTERN int GTSupportsWglExtension (const char* acWglExtension);

}  // extern "C"

#endif
