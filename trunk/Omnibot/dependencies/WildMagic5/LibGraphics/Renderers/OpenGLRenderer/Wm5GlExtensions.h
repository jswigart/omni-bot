// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2010/10/01)

#ifndef WM5GLEXTENSIONS_H
#define WM5GLEXTENSIONS_H

#if defined(__gl_h_) || defined(__GL_H__)
#error gl.h included before Wm5GlExtensions.h
#endif
#if defined(__glext_h_) || defined(__GLEXT_H_)
#error glext.h included before Wm5GlExtensions.h
#endif
#if defined(__gl_ATI_h_)
#error glATI.h included before Wm5GlExtensions.h
#endif
#define __gl_h_
#define __GL_H__
#define __glext_h_
#define __GLEXT_H_
#define __gl_ATI_h_

#ifdef WIN32
// Microsoft Windows
#ifdef WM5_GRAPHICS_DLL_EXPORT
#define GT_EXTERN extern __declspec(dllexport)
#else
#ifdef WM5_GRAPHICS_DLL_IMPORT
#define GT_EXTERN extern __declspec(dllimport)
#else
#define GT_EXTERN extern
#endif
#endif
#else
// Macintosh, Linux
#define GT_EXTERN extern
#endif

#include <cstddef>
extern "C"
{

//----------------------------------------------------------------------------
// OpenGL 1.1
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL11;

#define GL_ACCUM                                                    0x0100
#define GL_LOAD                                                     0x0101
#define GL_RETURN                                                   0x0102
#define GL_MULT                                                     0x0103
#define GL_ADD                                                      0x0104
#define GL_NEVER                                                    0x0200
#define GL_LESS                                                     0x0201
#define GL_EQUAL                                                    0x0202
#define GL_LEQUAL                                                   0x0203
#define GL_GREATER                                                  0x0204
#define GL_NOTEQUAL                                                 0x0205
#define GL_GEQUAL                                                   0x0206
#define GL_ALWAYS                                                   0x0207
#define GL_CURRENT_BIT                                              0x00000001
#define GL_POINT_BIT                                                0x00000002
#define GL_LINE_BIT                                                 0x00000004
#define GL_POLYGON_BIT                                              0x00000008
#define GL_POLYGON_STIPPLE_BIT                                      0x00000010
#define GL_PIXEL_MODE_BIT                                           0x00000020
#define GL_LIGHTING_BIT                                             0x00000040
#define GL_FOG_BIT                                                  0x00000080
#define GL_DEPTH_BUFFER_BIT                                         0x00000100
#define GL_ACCUM_BUFFER_BIT                                         0x00000200
#define GL_STENCIL_BUFFER_BIT                                       0x00000400
#define GL_VIEWPORT_BIT                                             0x00000800
#define GL_TRANSFORM_BIT                                            0x00001000
#define GL_ENABLE_BIT                                               0x00002000
#define GL_COLOR_BUFFER_BIT                                         0x00004000
#define GL_HINT_BIT                                                 0x00008000
#define GL_EVAL_BIT                                                 0x00010000
#define GL_LIST_BIT                                                 0x00020000
#define GL_TEXTURE_BIT                                              0x00040000
#define GL_SCISSOR_BIT                                              0x00080000
#define GL_ALL_ATTRIB_BITS                                          0x000fffff
#define GL_POINTS                                                   0x0000
#define GL_LINES                                                    0x0001
#define GL_LINE_LOOP                                                0x0002
#define GL_LINE_STRIP                                               0x0003
#define GL_TRIANGLES                                                0x0004
#define GL_TRIANGLE_STRIP                                           0x0005
#define GL_TRIANGLE_FAN                                             0x0006
#define GL_QUADS                                                    0x0007
#define GL_QUAD_STRIP                                               0x0008
#define GL_POLYGON                                                  0x0009
#define GL_ZERO                                                     0
#define GL_ONE                                                      1
#define GL_SRC_COLOR                                                0x0300
#define GL_ONE_MINUS_SRC_COLOR                                      0x0301
#define GL_SRC_ALPHA                                                0x0302
#define GL_ONE_MINUS_SRC_ALPHA                                      0x0303
#define GL_DST_ALPHA                                                0x0304
#define GL_ONE_MINUS_DST_ALPHA                                      0x0305
#define GL_DST_COLOR                                                0x0306
#define GL_ONE_MINUS_DST_COLOR                                      0x0307
#define GL_SRC_ALPHA_SATURATE                                       0x0308
#define GL_TRUE                                                     1
#define GL_FALSE                                                    0
#define GL_CLIP_PLANE0                                              0x3000
#define GL_CLIP_PLANE1                                              0x3001
#define GL_CLIP_PLANE2                                              0x3002
#define GL_CLIP_PLANE3                                              0x3003
#define GL_CLIP_PLANE4                                              0x3004
#define GL_CLIP_PLANE5                                              0x3005
#define GL_BYTE                                                     0x1400
#define GL_UNSIGNED_BYTE                                            0x1401
#define GL_SHORT                                                    0x1402
#define GL_UNSIGNED_SHORT                                           0x1403
#define GL_INT                                                      0x1404
#define GL_UNSIGNED_INT                                             0x1405
#define GL_FLOAT                                                    0x1406
#define GL_2_BYTES                                                  0x1407
#define GL_3_BYTES                                                  0x1408
#define GL_4_BYTES                                                  0x1409
#define GL_DOUBLE                                                   0x140A
#define GL_NONE                                                     0
#define GL_FRONT_LEFT                                               0x0400
#define GL_FRONT_RIGHT                                              0x0401
#define GL_BACK_LEFT                                                0x0402
#define GL_BACK_RIGHT                                               0x0403
#define GL_FRONT                                                    0x0404
#define GL_BACK                                                     0x0405
#define GL_LEFT                                                     0x0406
#define GL_RIGHT                                                    0x0407
#define GL_FRONT_AND_BACK                                           0x0408
#define GL_AUX0                                                     0x0409
#define GL_AUX1                                                     0x040A
#define GL_AUX2                                                     0x040B
#define GL_AUX3                                                     0x040C
#define GL_NO_ERROR                                                 0
#define GL_INVALID_ENUM                                             0x0500
#define GL_INVALID_VALUE                                            0x0501
#define GL_INVALID_OPERATION                                        0x0502
#define GL_STACK_OVERFLOW                                           0x0503
#define GL_STACK_UNDERFLOW                                          0x0504
#define GL_OUT_OF_MEMORY                                            0x0505
#define GL_2D                                                       0x0600
#define GL_3D                                                       0x0601
#define GL_3D_COLOR                                                 0x0602
#define GL_3D_COLOR_TEXTURE                                         0x0603
#define GL_4D_COLOR_TEXTURE                                         0x0604
#define GL_PASS_THROUGH_TOKEN                                       0x0700
#define GL_POINT_TOKEN                                              0x0701
#define GL_LINE_TOKEN                                               0x0702
#define GL_POLYGON_TOKEN                                            0x0703
#define GL_BITMAP_TOKEN                                             0x0704
#define GL_DRAW_PIXEL_TOKEN                                         0x0705
#define GL_COPY_PIXEL_TOKEN                                         0x0706
#define GL_LINE_RESET_TOKEN                                         0x0707
#define GL_EXP                                                      0x0800
#define GL_EXP2                                                     0x0801
#define GL_CW                                                       0x0900
#define GL_CCW                                                      0x0901
#define GL_COEFF                                                    0x0A00
#define GL_ORDER                                                    0x0A01
#define GL_DOMAIN                                                   0x0A02
#define GL_CURRENT_COLOR                                            0x0B00
#define GL_CURRENT_INDEX                                            0x0B01
#define GL_CURRENT_NORMAL                                           0x0B02
#define GL_CURRENT_TEXTURE_COORDS                                   0x0B03
#define GL_CURRENT_RASTER_COLOR                                     0x0B04
#define GL_CURRENT_RASTER_INDEX                                     0x0B05
#define GL_CURRENT_RASTER_TEXTURE_COORDS                            0x0B06
#define GL_CURRENT_RASTER_POSITION                                  0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID                            0x0B08
#define GL_CURRENT_RASTER_DISTANCE                                  0x0B09
#define GL_POINT_SMOOTH                                             0x0B10
#define GL_POINT_SIZE                                               0x0B11
#define GL_POINT_SIZE_RANGE                                         0x0B12
#define GL_POINT_SIZE_GRANULARITY                                   0x0B13
#define GL_LINE_SMOOTH                                              0x0B20
#define GL_LINE_WIDTH                                               0x0B21
#define GL_LINE_WIDTH_RANGE                                         0x0B22
#define GL_LINE_WIDTH_GRANULARITY                                   0x0B23
#define GL_LINE_STIPPLE                                             0x0B24
#define GL_LINE_STIPPLE_PATTERN                                     0x0B25
#define GL_LINE_STIPPLE_REPEAT                                      0x0B26
#define GL_LIST_MODE                                                0x0B30
#define GL_MAX_LIST_NESTING                                         0x0B31
#define GL_LIST_BASE                                                0x0B32
#define GL_LIST_INDEX                                               0x0B33
#define GL_POLYGON_MODE                                             0x0B40
#define GL_POLYGON_SMOOTH                                           0x0B41
#define GL_POLYGON_STIPPLE                                          0x0B42
#define GL_EDGE_FLAG                                                0x0B43
#define GL_CULL_FACE                                                0x0B44
#define GL_CULL_FACE_MODE                                           0x0B45
#define GL_FRONT_FACE                                               0x0B46
#define GL_LIGHTING                                                 0x0B50
#define GL_LIGHT_MODEL_LOCAL_VIEWER                                 0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE                                     0x0B52
#define GL_LIGHT_MODEL_AMBIENT                                      0x0B53
#define GL_SHADE_MODEL                                              0x0B54
#define GL_COLOR_MATERIAL_FACE                                      0x0B55
#define GL_COLOR_MATERIAL_PARAMETER                                 0x0B56
#define GL_COLOR_MATERIAL                                           0x0B57
#define GL_FOG                                                      0x0B60
#define GL_FOG_INDEX                                                0x0B61
#define GL_FOG_DENSITY                                              0x0B62
#define GL_FOG_START                                                0x0B63
#define GL_FOG_END                                                  0x0B64
#define GL_FOG_MODE                                                 0x0B65
#define GL_FOG_COLOR                                                0x0B66
#define GL_DEPTH_RANGE                                              0x0B70
#define GL_DEPTH_TEST                                               0x0B71
#define GL_DEPTH_WRITEMASK                                          0x0B72
#define GL_DEPTH_CLEAR_VALUE                                        0x0B73
#define GL_DEPTH_FUNC                                               0x0B74
#define GL_ACCUM_CLEAR_VALUE                                        0x0B80
#define GL_STENCIL_TEST                                             0x0B90
#define GL_STENCIL_CLEAR_VALUE                                      0x0B91
#define GL_STENCIL_FUNC                                             0x0B92
#define GL_STENCIL_VALUE_MASK                                       0x0B93
#define GL_STENCIL_FAIL                                             0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL                                  0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS                                  0x0B96
#define GL_STENCIL_REF                                              0x0B97
#define GL_STENCIL_WRITEMASK                                        0x0B98
#define GL_MATRIX_MODE                                              0x0BA0
#define GL_NORMALIZE                                                0x0BA1
#define GL_VIEWPORT                                                 0x0BA2
#define GL_MODELVIEW_STACK_DEPTH                                    0x0BA3
#define GL_PROJECTION_STACK_DEPTH                                   0x0BA4
#define GL_TEXTURE_STACK_DEPTH                                      0x0BA5
#define GL_MODELVIEW_MATRIX                                         0x0BA6
#define GL_PROJECTION_MATRIX                                        0x0BA7
#define GL_TEXTURE_MATRIX                                           0x0BA8
#define GL_ATTRIB_STACK_DEPTH                                       0x0BB0
#define GL_CLIENT_ATTRIB_STACK_DEPTH                                0x0BB1
#define GL_ALPHA_TEST                                               0x0BC0
#define GL_ALPHA_TEST_FUNC                                          0x0BC1
#define GL_ALPHA_TEST_REF                                           0x0BC2
#define GL_DITHER                                                   0x0BD0
#define GL_BLEND_DST                                                0x0BE0
#define GL_BLEND_SRC                                                0x0BE1
#define GL_BLEND                                                    0x0BE2
#define GL_LOGIC_OP_MODE                                            0x0BF0
#define GL_INDEX_LOGIC_OP                                           0x0BF1
#define GL_COLOR_LOGIC_OP                                           0x0BF2
#define GL_AUX_BUFFERS                                              0x0C00
#define GL_DRAW_BUFFER                                              0x0C01
#define GL_READ_BUFFER                                              0x0C02
#define GL_SCISSOR_BOX                                              0x0C10
#define GL_SCISSOR_TEST                                             0x0C11
#define GL_INDEX_CLEAR_VALUE                                        0x0C20
#define GL_INDEX_WRITEMASK                                          0x0C21
#define GL_COLOR_CLEAR_VALUE                                        0x0C22
#define GL_COLOR_WRITEMASK                                          0x0C23
#define GL_INDEX_MODE                                               0x0C30
#define GL_RGBA_MODE                                                0x0C31
#define GL_DOUBLEBUFFER                                             0x0C32
#define GL_STEREO                                                   0x0C33
#define GL_RENDER_MODE                                              0x0C40
#define GL_PERSPECTIVE_CORRECTION_HINT                              0x0C50
#define GL_POINT_SMOOTH_HINT                                        0x0C51
#define GL_LINE_SMOOTH_HINT                                         0x0C52
#define GL_POLYGON_SMOOTH_HINT                                      0x0C53
#define GL_FOG_HINT                                                 0x0C54
#define GL_TEXTURE_GEN_S                                            0x0C60
#define GL_TEXTURE_GEN_T                                            0x0C61
#define GL_TEXTURE_GEN_R                                            0x0C62
#define GL_TEXTURE_GEN_Q                                            0x0C63
#define GL_PIXEL_MAP_I_TO_I                                         0x0C70
#define GL_PIXEL_MAP_S_TO_S                                         0x0C71
#define GL_PIXEL_MAP_I_TO_R                                         0x0C72
#define GL_PIXEL_MAP_I_TO_G                                         0x0C73
#define GL_PIXEL_MAP_I_TO_B                                         0x0C74
#define GL_PIXEL_MAP_I_TO_A                                         0x0C75
#define GL_PIXEL_MAP_R_TO_R                                         0x0C76
#define GL_PIXEL_MAP_G_TO_G                                         0x0C77
#define GL_PIXEL_MAP_B_TO_B                                         0x0C78
#define GL_PIXEL_MAP_A_TO_A                                         0x0C79
#define GL_PIXEL_MAP_I_TO_I_SIZE                                    0x0CB0
#define GL_PIXEL_MAP_S_TO_S_SIZE                                    0x0CB1
#define GL_PIXEL_MAP_I_TO_R_SIZE                                    0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE                                    0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE                                    0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE                                    0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE                                    0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE                                    0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE                                    0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE                                    0x0CB9
#define GL_UNPACK_SWAP_BYTES                                        0x0CF0
#define GL_UNPACK_LSB_FIRST                                         0x0CF1
#define GL_UNPACK_ROW_LENGTH                                        0x0CF2
#define GL_UNPACK_SKIP_ROWS                                         0x0CF3
#define GL_UNPACK_SKIP_PIXELS                                       0x0CF4
#define GL_UNPACK_ALIGNMENT                                         0x0CF5
#define GL_PACK_SWAP_BYTES                                          0x0D00
#define GL_PACK_LSB_FIRST                                           0x0D01
#define GL_PACK_ROW_LENGTH                                          0x0D02
#define GL_PACK_SKIP_ROWS                                           0x0D03
#define GL_PACK_SKIP_PIXELS                                         0x0D04
#define GL_PACK_ALIGNMENT                                           0x0D05
#define GL_MAP_COLOR                                                0x0D10
#define GL_MAP_STENCIL                                              0x0D11
#define GL_INDEX_SHIFT                                              0x0D12
#define GL_INDEX_OFFSET                                             0x0D13
#define GL_RED_SCALE                                                0x0D14
#define GL_RED_BIAS                                                 0x0D15
#define GL_ZOOM_X                                                   0x0D16
#define GL_ZOOM_Y                                                   0x0D17
#define GL_GREEN_SCALE                                              0x0D18
#define GL_GREEN_BIAS                                               0x0D19
#define GL_BLUE_SCALE                                               0x0D1A
#define GL_BLUE_BIAS                                                0x0D1B
#define GL_ALPHA_SCALE                                              0x0D1C
#define GL_ALPHA_BIAS                                               0x0D1D
#define GL_DEPTH_SCALE                                              0x0D1E
#define GL_DEPTH_BIAS                                               0x0D1F
#define GL_MAX_EVAL_ORDER                                           0x0D30
#define GL_MAX_LIGHTS                                               0x0D31
#define GL_MAX_CLIP_PLANES                                          0x0D32
#define GL_MAX_TEXTURE_SIZE                                         0x0D33
#define GL_MAX_PIXEL_MAP_TABLE                                      0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH                                   0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH                                0x0D36
#define GL_MAX_NAME_STACK_DEPTH                                     0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH                               0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH                                  0x0D39
#define GL_MAX_VIEWPORT_DIMS                                        0x0D3A
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH                            0x0D3B
#define GL_SUBPIXEL_BITS                                            0x0D50
#define GL_INDEX_BITS                                               0x0D51
#define GL_RED_BITS                                                 0x0D52
#define GL_GREEN_BITS                                               0x0D53
#define GL_BLUE_BITS                                                0x0D54
#define GL_ALPHA_BITS                                               0x0D55
#define GL_DEPTH_BITS                                               0x0D56
#define GL_STENCIL_BITS                                             0x0D57
#define GL_ACCUM_RED_BITS                                           0x0D58
#define GL_ACCUM_GREEN_BITS                                         0x0D59
#define GL_ACCUM_BLUE_BITS                                          0x0D5A
#define GL_ACCUM_ALPHA_BITS                                         0x0D5B
#define GL_NAME_STACK_DEPTH                                         0x0D70
#define GL_AUTO_NORMAL                                              0x0D80
#define GL_MAP1_COLOR_4                                             0x0D90
#define GL_MAP1_INDEX                                               0x0D91
#define GL_MAP1_NORMAL                                              0x0D92
#define GL_MAP1_TEXTURE_COORD_1                                     0x0D93
#define GL_MAP1_TEXTURE_COORD_2                                     0x0D94
#define GL_MAP1_TEXTURE_COORD_3                                     0x0D95
#define GL_MAP1_TEXTURE_COORD_4                                     0x0D96
#define GL_MAP1_VERTEX_3                                            0x0D97
#define GL_MAP1_VERTEX_4                                            0x0D98
#define GL_MAP2_COLOR_4                                             0x0DB0
#define GL_MAP2_INDEX                                               0x0DB1
#define GL_MAP2_NORMAL                                              0x0DB2
#define GL_MAP2_TEXTURE_COORD_1                                     0x0DB3
#define GL_MAP2_TEXTURE_COORD_2                                     0x0DB4
#define GL_MAP2_TEXTURE_COORD_3                                     0x0DB5
#define GL_MAP2_TEXTURE_COORD_4                                     0x0DB6
#define GL_MAP2_VERTEX_3                                            0x0DB7
#define GL_MAP2_VERTEX_4                                            0x0DB8
#define GL_MAP1_GRID_DOMAIN                                         0x0DD0
#define GL_MAP1_GRID_SEGMENTS                                       0x0DD1
#define GL_MAP2_GRID_DOMAIN                                         0x0DD2
#define GL_MAP2_GRID_SEGMENTS                                       0x0DD3
#define GL_TEXTURE_1D                                               0x0DE0
#define GL_TEXTURE_2D                                               0x0DE1
#define GL_FEEDBACK_BUFFER_POINTER                                  0x0DF0
#define GL_FEEDBACK_BUFFER_SIZE                                     0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE                                     0x0DF2
#define GL_SELECTION_BUFFER_POINTER                                 0x0DF3
#define GL_SELECTION_BUFFER_SIZE                                    0x0DF4
#define GL_TEXTURE_WIDTH                                            0x1000
#define GL_TEXTURE_HEIGHT                                           0x1001
#define GL_TEXTURE_INTERNAL_FORMAT                                  0x1003
#define GL_TEXTURE_BORDER_COLOR                                     0x1004
#define GL_TEXTURE_BORDER                                           0x1005
#define GL_DONT_CARE                                                0x1100
#define GL_FASTEST                                                  0x1101
#define GL_NICEST                                                   0x1102
#define GL_LIGHT0                                                   0x4000
#define GL_LIGHT1                                                   0x4001
#define GL_LIGHT2                                                   0x4002
#define GL_LIGHT3                                                   0x4003
#define GL_LIGHT4                                                   0x4004
#define GL_LIGHT5                                                   0x4005
#define GL_LIGHT6                                                   0x4006
#define GL_LIGHT7                                                   0x4007
#define GL_AMBIENT                                                  0x1200
#define GL_DIFFUSE                                                  0x1201
#define GL_SPECULAR                                                 0x1202
#define GL_POSITION                                                 0x1203
#define GL_SPOT_DIRECTION                                           0x1204
#define GL_SPOT_EXPONENT                                            0x1205
#define GL_SPOT_CUTOFF                                              0x1206
#define GL_CONSTANT_ATTENUATION                                     0x1207
#define GL_LINEAR_ATTENUATION                                       0x1208
#define GL_QUADRATIC_ATTENUATION                                    0x1209
#define GL_COMPILE                                                  0x1300
#define GL_COMPILE_AND_EXECUTE                                      0x1301
#define GL_CLEAR                                                    0x1500
#define GL_AND                                                      0x1501
#define GL_AND_REVERSE                                              0x1502
#define GL_COPY                                                     0x1503
#define GL_AND_INVERTED                                             0x1504
#define GL_NOOP                                                     0x1505
#define GL_XOR                                                      0x1506
#define GL_OR                                                       0x1507
#define GL_NOR                                                      0x1508
#define GL_EQUIV                                                    0x1509
#define GL_INVERT                                                   0x150A
#define GL_OR_REVERSE                                               0x150B
#define GL_COPY_INVERTED                                            0x150C
#define GL_OR_INVERTED                                              0x150D
#define GL_NAND                                                     0x150E
#define GL_SET                                                      0x150F
#define GL_EMISSION                                                 0x1600
#define GL_SHININESS                                                0x1601
#define GL_AMBIENT_AND_DIFFUSE                                      0x1602
#define GL_COLOR_INDEXES                                            0x1603
#define GL_MODELVIEW                                                0x1700
#define GL_PROJECTION                                               0x1701
#define GL_TEXTURE                                                  0x1702
#define GL_COLOR                                                    0x1800
#define GL_DEPTH                                                    0x1801
#define GL_STENCIL                                                  0x1802
#define GL_COLOR_INDEX                                              0x1900
#define GL_STENCIL_INDEX                                            0x1901
#define GL_DEPTH_COMPONENT                                          0x1902
#define GL_RED                                                      0x1903
#define GL_GREEN                                                    0x1904
#define GL_BLUE                                                     0x1905
#define GL_ALPHA                                                    0x1906
#define GL_RGB                                                      0x1907
#define GL_RGBA                                                     0x1908
#define GL_LUMINANCE                                                0x1909
#define GL_LUMINANCE_ALPHA                                          0x190A
#define GL_BITMAP                                                   0x1A00
#define GL_POINT                                                    0x1B00
#define GL_LINE                                                     0x1B01
#define GL_FILL                                                     0x1B02
#define GL_RENDER                                                   0x1C00
#define GL_FEEDBACK                                                 0x1C01
#define GL_SELECT                                                   0x1C02
#define GL_FLAT                                                     0x1D00
#define GL_SMOOTH                                                   0x1D01
#define GL_KEEP                                                     0x1E00
#define GL_REPLACE                                                  0x1E01
#define GL_INCR                                                     0x1E02
#define GL_DECR                                                     0x1E03
#define GL_VENDOR                                                   0x1F00
#define GL_RENDERER                                                 0x1F01
#define GL_VERSION                                                  0x1F02
#define GL_EXTENSIONS                                               0x1F03
#define GL_S                                                        0x2000
#define GL_T                                                        0x2001
#define GL_R                                                        0x2002
#define GL_Q                                                        0x2003
#define GL_MODULATE                                                 0x2100
#define GL_DECAL                                                    0x2101
#define GL_TEXTURE_ENV_MODE                                         0x2200
#define GL_TEXTURE_ENV_COLOR                                        0x2201
#define GL_TEXTURE_ENV                                              0x2300
#define GL_EYE_LINEAR                                               0x2400
#define GL_OBJECT_LINEAR                                            0x2401
#define GL_SPHERE_MAP                                               0x2402
#define GL_TEXTURE_GEN_MODE                                         0x2500
#define GL_OBJECT_PLANE                                             0x2501
#define GL_EYE_PLANE                                                0x2502
#define GL_NEAREST                                                  0x2600
#define GL_LINEAR                                                   0x2601
#define GL_NEAREST_MIPMAP_NEAREST                                   0x2700
#define GL_LINEAR_MIPMAP_NEAREST                                    0x2701
#define GL_NEAREST_MIPMAP_LINEAR                                    0x2702
#define GL_LINEAR_MIPMAP_LINEAR                                     0x2703
#define GL_TEXTURE_MAG_FILTER                                       0x2800
#define GL_TEXTURE_MIN_FILTER                                       0x2801
#define GL_TEXTURE_WRAP_S                                           0x2802
#define GL_TEXTURE_WRAP_T                                           0x2803
#define GL_CLAMP                                                    0x2900
#define GL_REPEAT                                                   0x2901
#define GL_CLIENT_PIXEL_STORE_BIT                                   0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT                                  0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS                                   0xffffffff
#define GL_POLYGON_OFFSET_FACTOR                                    0x8038
#define GL_POLYGON_OFFSET_UNITS                                     0x2A00
#define GL_POLYGON_OFFSET_POINT                                     0x2A01
#define GL_POLYGON_OFFSET_LINE                                      0x2A02
#define GL_POLYGON_OFFSET_FILL                                      0x8037
#define GL_ALPHA4                                                   0x803B
#define GL_ALPHA8                                                   0x803C
#define GL_ALPHA12                                                  0x803D
#define GL_ALPHA16                                                  0x803E
#define GL_LUMINANCE4                                               0x803F
#define GL_LUMINANCE8                                               0x8040
#define GL_LUMINANCE12                                              0x8041
#define GL_LUMINANCE16                                              0x8042
#define GL_LUMINANCE4_ALPHA4                                        0x8043
#define GL_LUMINANCE6_ALPHA2                                        0x8044
#define GL_LUMINANCE8_ALPHA8                                        0x8045
#define GL_LUMINANCE12_ALPHA4                                       0x8046
#define GL_LUMINANCE12_ALPHA12                                      0x8047
#define GL_LUMINANCE16_ALPHA16                                      0x8048
#define GL_INTENSITY                                                0x8049
#define GL_INTENSITY4                                               0x804A
#define GL_INTENSITY8                                               0x804B
#define GL_INTENSITY12                                              0x804C
#define GL_INTENSITY16                                              0x804D
#define GL_R3_G3_B2                                                 0x2A10
#define GL_RGB4                                                     0x804F
#define GL_RGB5                                                     0x8050
#define GL_RGB8                                                     0x8051
#define GL_RGB10                                                    0x8052
#define GL_RGB12                                                    0x8053
#define GL_RGB16                                                    0x8054
#define GL_RGBA2                                                    0x8055
#define GL_RGBA4                                                    0x8056
#define GL_RGB5_A1                                                  0x8057
#define GL_RGBA8                                                    0x8058
#define GL_RGB10_A2                                                 0x8059
#define GL_RGBA12                                                   0x805A
#define GL_RGBA16                                                   0x805B
#define GL_TEXTURE_RED_SIZE                                         0x805C
#define GL_TEXTURE_GREEN_SIZE                                       0x805D
#define GL_TEXTURE_BLUE_SIZE                                        0x805E
#define GL_TEXTURE_ALPHA_SIZE                                       0x805F
#define GL_TEXTURE_LUMINANCE_SIZE                                   0x8060
#define GL_TEXTURE_INTENSITY_SIZE                                   0x8061
#define GL_PROXY_TEXTURE_1D                                         0x8063
#define GL_PROXY_TEXTURE_2D                                         0x8064
#define GL_TEXTURE_PRIORITY                                         0x8066
#define GL_TEXTURE_RESIDENT                                         0x8067
#define GL_TEXTURE_BINDING_1D                                       0x8068
#define GL_TEXTURE_BINDING_2D                                       0x8069
#define GL_VERTEX_ARRAY                                             0x8074
#define GL_NORMAL_ARRAY                                             0x8075
#define GL_COLOR_ARRAY                                              0x8076
#define GL_INDEX_ARRAY                                              0x8077
#define GL_TEXTURE_COORD_ARRAY                                      0x8078
#define GL_EDGE_FLAG_ARRAY                                          0x8079
#define GL_VERTEX_ARRAY_SIZE                                        0x807A
#define GL_VERTEX_ARRAY_TYPE                                        0x807B
#define GL_VERTEX_ARRAY_STRIDE                                      0x807C
#define GL_NORMAL_ARRAY_TYPE                                        0x807E
#define GL_NORMAL_ARRAY_STRIDE                                      0x807F
#define GL_COLOR_ARRAY_SIZE                                         0x8081
#define GL_COLOR_ARRAY_TYPE                                         0x8082
#define GL_COLOR_ARRAY_STRIDE                                       0x8083
#define GL_INDEX_ARRAY_TYPE                                         0x8085
#define GL_INDEX_ARRAY_STRIDE                                       0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE                                 0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE                                 0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE                               0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE                                   0x808C
#define GL_VERTEX_ARRAY_POINTER                                     0x808E
#define GL_NORMAL_ARRAY_POINTER                                     0x808F
#define GL_COLOR_ARRAY_POINTER                                      0x8090
#define GL_INDEX_ARRAY_POINTER                                      0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER                              0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER                                  0x8093
#define GL_V2F                                                      0x2A20
#define GL_V3F                                                      0x2A21
#define GL_C4UB_V2F                                                 0x2A22
#define GL_C4UB_V3F                                                 0x2A23
#define GL_C3F_V3F                                                  0x2A24
#define GL_N3F_V3F                                                  0x2A25
#define GL_C4F_N3F_V3F                                              0x2A26
#define GL_T2F_V3F                                                  0x2A27
#define GL_T4F_V4F                                                  0x2A28
#define GL_T2F_C4UB_V3F                                             0x2A29
#define GL_T2F_C3F_V3F                                              0x2A2A
#define GL_T2F_N3F_V3F                                              0x2A2B
#define GL_T2F_C4F_N3F_V3F                                          0x2A2C
#define GL_T4F_C4F_N3F_V4F                                          0x2A2D
#define GL_LOGIC_OP                                                 0x0BF1
#define GL_TEXTURE_COMPONENTS                                       0x1003
#define GL_COLOR_INDEX1_EXT                                         0x80E2
#define GL_COLOR_INDEX2_EXT                                         0x80E3
#define GL_COLOR_INDEX4_EXT                                         0x80E4
#define GL_COLOR_INDEX8_EXT                                         0x80E5
#define GL_COLOR_INDEX12_EXT                                        0x80E6
#define GL_COLOR_INDEX16_EXT                                        0x80E7

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

#define glAccum GTglAccum
#define glAlphaFunc GTglAlphaFunc
#define glAreTexturesResident GTglAreTexturesResident
#define glArrayElement GTglArrayElement
#define glBegin GTglBegin
#define glBindTexture GTglBindTexture
#define glBitmap GTglBitmap
#define glBlendFunc GTglBlendFunc
#define glCallList GTglCallList
#define glCallLists GTglCallLists
#define glClear GTglClear
#define glClearAccum GTglClearAccum
#define glClearColor GTglClearColor
#define glClearDepth GTglClearDepth
#define glClearIndex GTglClearIndex
#define glClearStencil GTglClearStencil
#define glClipPlane GTglClipPlane
#define glColor3b GTglColor3b
#define glColor3bv GTglColor3bv
#define glColor3d GTglColor3d
#define glColor3dv GTglColor3dv
#define glColor3f GTglColor3f
#define glColor3fv GTglColor3fv
#define glColor3i GTglColor3i
#define glColor3iv GTglColor3iv
#define glColor3s GTglColor3s
#define glColor3sv GTglColor3sv
#define glColor3ub GTglColor3ub
#define glColor3ubv GTglColor3ubv
#define glColor3ui GTglColor3ui
#define glColor3uiv GTglColor3uiv
#define glColor3us GTglColor3us
#define glColor3usv GTglColor3usv
#define glColor4b GTglColor4b
#define glColor4bv GTglColor4bv
#define glColor4d GTglColor4d
#define glColor4dv GTglColor4dv
#define glColor4f GTglColor4f
#define glColor4fv GTglColor4fv
#define glColor4i GTglColor4i
#define glColor4iv GTglColor4iv
#define glColor4s GTglColor4s
#define glColor4sv GTglColor4sv
#define glColor4ub GTglColor4ub
#define glColor4ubv GTglColor4ubv
#define glColor4ui GTglColor4ui
#define glColor4uiv GTglColor4uiv
#define glColor4us GTglColor4us
#define glColor4usv GTglColor4usv
#define glColorMask GTglColorMask
#define glColorMaterial GTglColorMaterial
#define glColorPointer GTglColorPointer
#define glCopyPixels GTglCopyPixels
#define glCopyTexImage1D GTglCopyTexImage1D
#define glCopyTexImage2D GTglCopyTexImage2D
#define glCopyTexSubImage1D GTglCopyTexSubImage1D
#define glCopyTexSubImage2D GTglCopyTexSubImage2D
#define glCullFace GTglCullFace
#define glDeleteLists GTglDeleteLists
#define glDeleteTextures GTglDeleteTextures
#define glDepthFunc GTglDepthFunc
#define glDepthMask GTglDepthMask
#define glDepthRange GTglDepthRange
#define glDisable GTglDisable
#define glDisableClientState GTglDisableClientState
#define glDrawArrays GTglDrawArrays
#define glDrawBuffer GTglDrawBuffer
#define glDrawElements GTglDrawElements
#define glDrawPixels GTglDrawPixels
#define glEdgeFlag GTglEdgeFlag
#define glEdgeFlagPointer GTglEdgeFlagPointer
#define glEdgeFlagv GTglEdgeFlagv
#define glEnable GTglEnable
#define glEnableClientState GTglEnableClientState
#define glEnd GTglEnd
#define glEndList GTglEndList
#define glEvalCoord1d GTglEvalCoord1d
#define glEvalCoord1dv GTglEvalCoord1dv
#define glEvalCoord1f GTglEvalCoord1f
#define glEvalCoord1fv GTglEvalCoord1fv
#define glEvalCoord2d GTglEvalCoord2d
#define glEvalCoord2dv GTglEvalCoord2dv
#define glEvalCoord2f GTglEvalCoord2f
#define glEvalCoord2fv GTglEvalCoord2fv
#define glEvalMesh1 GTglEvalMesh1
#define glEvalMesh2 GTglEvalMesh2
#define glEvalPoint1 GTglEvalPoint1
#define glEvalPoint2 GTglEvalPoint2
#define glFeedbackBuffer GTglFeedbackBuffer
#define glFinish GTglFinish
#define glFlush GTglFlush
#define glFogf GTglFogf
#define glFogfv GTglFogfv
#define glFogi GTglFogi
#define glFogiv GTglFogiv
#define glFrontFace GTglFrontFace
#define glFrustum GTglFrustum
#define glGenLists GTglGenLists
#define glGenTextures GTglGenTextures
#define glGetBooleanv GTglGetBooleanv
#define glGetClipPlane GTglGetClipPlane
#define glGetDoublev GTglGetDoublev
#define glGetError GTglGetError
#define glGetFloatv GTglGetFloatv
#define glGetIntegerv GTglGetIntegerv
#define glGetLightfv GTglGetLightfv
#define glGetLightiv GTglGetLightiv
#define glGetMapdv GTglGetMapdv
#define glGetMapfv GTglGetMapfv
#define glGetMapiv GTglGetMapiv
#define glGetMaterialfv GTglGetMaterialfv
#define glGetMaterialiv GTglGetMaterialiv
#define glGetPixelMapfv GTglGetPixelMapfv
#define glGetPixelMapuiv GTglGetPixelMapuiv
#define glGetPixelMapusv GTglGetPixelMapusv
#define glGetPointerv GTglGetPointerv
#define glGetPolygonStipple GTglGetPolygonStipple
#define glGetString GTglGetString
#define glGetTexEnvfv GTglGetTexEnvfv
#define glGetTexEnviv GTglGetTexEnviv
#define glGetTexGendv GTglGetTexGendv
#define glGetTexGenfv GTglGetTexGenfv
#define glGetTexGeniv GTglGetTexGeniv
#define glGetTexImage GTglGetTexImage
#define glGetTexLevelParameterfv GTglGetTexLevelParameterfv
#define glGetTexLevelParameteriv GTglGetTexLevelParameteriv
#define glGetTexParameterfv GTglGetTexParameterfv
#define glGetTexParameteriv GTglGetTexParameteriv
#define glHint GTglHint
#define glIndexMask GTglIndexMask
#define glIndexPointer GTglIndexPointer
#define glIndexd GTglIndexd
#define glIndexdv GTglIndexdv
#define glIndexf GTglIndexf
#define glIndexfv GTglIndexfv
#define glIndexi GTglIndexi
#define glIndexiv GTglIndexiv
#define glIndexs GTglIndexs
#define glIndexsv GTglIndexsv
#define glIndexub GTglIndexub
#define glIndexubv GTglIndexubv
#define glInitNames GTglInitNames
#define glInterleavedArrays GTglInterleavedArrays
#define glIsEnabled GTglIsEnabled
#define glIsList GTglIsList
#define glIsTexture GTglIsTexture
#define glLightModelf GTglLightModelf
#define glLightModelfv GTglLightModelfv
#define glLightModeli GTglLightModeli
#define glLightModeliv GTglLightModeliv
#define glLightf GTglLightf
#define glLightfv GTglLightfv
#define glLighti GTglLighti
#define glLightiv GTglLightiv
#define glLineStipple GTglLineStipple
#define glLineWidth GTglLineWidth
#define glListBase GTglListBase
#define glLoadIdentity GTglLoadIdentity
#define glLoadMatrixd GTglLoadMatrixd
#define glLoadMatrixf GTglLoadMatrixf
#define glLoadName GTglLoadName
#define glLogicOp GTglLogicOp
#define glMap1d GTglMap1d
#define glMap1f GTglMap1f
#define glMap2d GTglMap2d
#define glMap2f GTglMap2f
#define glMapGrid1d GTglMapGrid1d
#define glMapGrid1f GTglMapGrid1f
#define glMapGrid2d GTglMapGrid2d
#define glMapGrid2f GTglMapGrid2f
#define glMaterialf GTglMaterialf
#define glMaterialfv GTglMaterialfv
#define glMateriali GTglMateriali
#define glMaterialiv GTglMaterialiv
#define glMatrixMode GTglMatrixMode
#define glMultMatrixd GTglMultMatrixd
#define glMultMatrixf GTglMultMatrixf
#define glNewList GTglNewList
#define glNormal3b GTglNormal3b
#define glNormal3bv GTglNormal3bv
#define glNormal3d GTglNormal3d
#define glNormal3dv GTglNormal3dv
#define glNormal3f GTglNormal3f
#define glNormal3fv GTglNormal3fv
#define glNormal3i GTglNormal3i
#define glNormal3iv GTglNormal3iv
#define glNormal3s GTglNormal3s
#define glNormal3sv GTglNormal3sv
#define glNormalPointer GTglNormalPointer
#define glOrtho GTglOrtho
#define glPassThrough GTglPassThrough
#define glPixelMapfv GTglPixelMapfv
#define glPixelMapuiv GTglPixelMapuiv
#define glPixelMapusv GTglPixelMapusv
#define glPixelStoref GTglPixelStoref
#define glPixelStorei GTglPixelStorei
#define glPixelTransferf GTglPixelTransferf
#define glPixelTransferi GTglPixelTransferi
#define glPixelZoom GTglPixelZoom
#define glPointSize GTglPointSize
#define glPolygonMode GTglPolygonMode
#define glPolygonOffset GTglPolygonOffset
#define glPolygonStipple GTglPolygonStipple
#define glPopAttrib GTglPopAttrib
#define glPopClientAttrib GTglPopClientAttrib
#define glPopMatrix GTglPopMatrix
#define glPopName GTglPopName
#define glPrioritizeTextures GTglPrioritizeTextures
#define glPushAttrib GTglPushAttrib
#define glPushClientAttrib GTglPushClientAttrib
#define glPushMatrix GTglPushMatrix
#define glPushName GTglPushName
#define glRasterPos2d GTglRasterPos2d
#define glRasterPos2dv GTglRasterPos2dv
#define glRasterPos2f GTglRasterPos2f
#define glRasterPos2fv GTglRasterPos2fv
#define glRasterPos2i GTglRasterPos2i
#define glRasterPos2iv GTglRasterPos2iv
#define glRasterPos2s GTglRasterPos2s
#define glRasterPos2sv GTglRasterPos2sv
#define glRasterPos3d GTglRasterPos3d
#define glRasterPos3dv GTglRasterPos3dv
#define glRasterPos3f GTglRasterPos3f
#define glRasterPos3fv GTglRasterPos3fv
#define glRasterPos3i GTglRasterPos3i
#define glRasterPos3iv GTglRasterPos3iv
#define glRasterPos3s GTglRasterPos3s
#define glRasterPos3sv GTglRasterPos3sv
#define glRasterPos4d GTglRasterPos4d
#define glRasterPos4dv GTglRasterPos4dv
#define glRasterPos4f GTglRasterPos4f
#define glRasterPos4fv GTglRasterPos4fv
#define glRasterPos4i GTglRasterPos4i
#define glRasterPos4iv GTglRasterPos4iv
#define glRasterPos4s GTglRasterPos4s
#define glRasterPos4sv GTglRasterPos4sv
#define glReadBuffer GTglReadBuffer
#define glReadPixels GTglReadPixels
#define glRectd GTglRectd
#define glRectdv GTglRectdv
#define glRectf GTglRectf
#define glRectfv GTglRectfv
#define glRecti GTglRecti
#define glRectiv GTglRectiv
#define glRects GTglRects
#define glRectsv GTglRectsv
#define glRenderMode GTglRenderMode
#define glRotated GTglRotated
#define glRotatef GTglRotatef
#define glScaled GTglScaled
#define glScalef GTglScalef
#define glScissor GTglScissor
#define glSelectBuffer GTglSelectBuffer
#define glShadeModel GTglShadeModel
#define glStencilFunc GTglStencilFunc
#define glStencilMask GTglStencilMask
#define glStencilOp GTglStencilOp
#define glTexCoord1d GTglTexCoord1d
#define glTexCoord1dv GTglTexCoord1dv
#define glTexCoord1f GTglTexCoord1f
#define glTexCoord1fv GTglTexCoord1fv
#define glTexCoord1i GTglTexCoord1i
#define glTexCoord1iv GTglTexCoord1iv
#define glTexCoord1s GTglTexCoord1s
#define glTexCoord1sv GTglTexCoord1sv
#define glTexCoord2d GTglTexCoord2d
#define glTexCoord2dv GTglTexCoord2dv
#define glTexCoord2f GTglTexCoord2f
#define glTexCoord2fv GTglTexCoord2fv
#define glTexCoord2i GTglTexCoord2i
#define glTexCoord2iv GTglTexCoord2iv
#define glTexCoord2s GTglTexCoord2s
#define glTexCoord2sv GTglTexCoord2sv
#define glTexCoord3d GTglTexCoord3d
#define glTexCoord3dv GTglTexCoord3dv
#define glTexCoord3f GTglTexCoord3f
#define glTexCoord3fv GTglTexCoord3fv
#define glTexCoord3i GTglTexCoord3i
#define glTexCoord3iv GTglTexCoord3iv
#define glTexCoord3s GTglTexCoord3s
#define glTexCoord3sv GTglTexCoord3sv
#define glTexCoord4d GTglTexCoord4d
#define glTexCoord4dv GTglTexCoord4dv
#define glTexCoord4f GTglTexCoord4f
#define glTexCoord4fv GTglTexCoord4fv
#define glTexCoord4i GTglTexCoord4i
#define glTexCoord4iv GTglTexCoord4iv
#define glTexCoord4s GTglTexCoord4s
#define glTexCoord4sv GTglTexCoord4sv
#define glTexCoordPointer GTglTexCoordPointer
#define glTexEnvf GTglTexEnvf
#define glTexEnvfv GTglTexEnvfv
#define glTexEnvi GTglTexEnvi
#define glTexEnviv GTglTexEnviv
#define glTexGend GTglTexGend
#define glTexGendv GTglTexGendv
#define glTexGenf GTglTexGenf
#define glTexGenfv GTglTexGenfv
#define glTexGeni GTglTexGeni
#define glTexGeniv GTglTexGeniv
#define glTexImage1D GTglTexImage1D
#define glTexImage2D GTglTexImage2D
#define glTexParameterf GTglTexParameterf
#define glTexParameterfv GTglTexParameterfv
#define glTexParameteri GTglTexParameteri
#define glTexParameteriv GTglTexParameteriv
#define glTexSubImage1D GTglTexSubImage1D
#define glTexSubImage2D GTglTexSubImage2D
#define glTranslated GTglTranslated
#define glTranslatef GTglTranslatef
#define glVertex2d GTglVertex2d
#define glVertex2dv GTglVertex2dv
#define glVertex2f GTglVertex2f
#define glVertex2fv GTglVertex2fv
#define glVertex2i GTglVertex2i
#define glVertex2iv GTglVertex2iv
#define glVertex2s GTglVertex2s
#define glVertex2sv GTglVertex2sv
#define glVertex3d GTglVertex3d
#define glVertex3dv GTglVertex3dv
#define glVertex3f GTglVertex3f
#define glVertex3fv GTglVertex3fv
#define glVertex3i GTglVertex3i
#define glVertex3iv GTglVertex3iv
#define glVertex3s GTglVertex3s
#define glVertex3sv GTglVertex3sv
#define glVertex4d GTglVertex4d
#define glVertex4dv GTglVertex4dv
#define glVertex4f GTglVertex4f
#define glVertex4fv GTglVertex4fv
#define glVertex4i GTglVertex4i
#define glVertex4iv GTglVertex4iv
#define glVertex4s GTglVertex4s
#define glVertex4sv GTglVertex4sv
#define glVertexPointer GTglVertexPointer
#define glViewport GTglViewport

GT_EXTERN void GTglAccum (GLenum eOperation, GLfloat fValue);
GT_EXTERN void GTglAlphaFunc (GLenum eFunction, GLclampf fReference);
GT_EXTERN GLboolean GTglAreTexturesResident (GLsizei iNumTextures,
    const GLuint* auiTextures, GLboolean* aucResidences);
GT_EXTERN void GTglArrayElement (GLint i);
GT_EXTERN void GTglBegin (GLenum eMode);
GT_EXTERN void GTglBindTexture (GLenum eTarget, GLuint uiTexture);
GT_EXTERN void GTglBitmap (GLsizei iWidth, GLsizei iHeight, GLfloat fXOrigin,
    GLfloat fYOrigin, GLfloat fXMove, GLfloat fYMove,
    const GLubyte* aucBitmap);
GT_EXTERN void GTglBlendFunc (GLenum eSrcFunction, GLenum eDstFunction);
GT_EXTERN void GTglCallList (GLuint uiList);
GT_EXTERN void GTglCallLists (GLsizei iNumLists, GLenum eType,
    const GLvoid* pvLists);
GT_EXTERN void GTglClear (GLbitfield uiMask);
GT_EXTERN void GTglClearAccum (GLfloat fRed, GLfloat fGreen, GLfloat fBlue,
    GLfloat fAlpha);
GT_EXTERN void GTglClearColor (GLclampf fRed, GLclampf fGreen, GLclampf fBlue,
    GLclampf fAlpha);
GT_EXTERN void GTglClearDepth (GLclampd dDepth);
GT_EXTERN void GTglClearIndex (GLfloat fIndex);
GT_EXTERN void GTglClearStencil (GLint iStencil);
GT_EXTERN void GTglClipPlane (GLenum ePlane, const GLdouble* adEquation);
GT_EXTERN void GTglColor3b (GLbyte cRed, GLbyte cGreen, GLbyte cBlue);
GT_EXTERN void GTglColor3bv (const GLbyte* acRGB);
GT_EXTERN void GTglColor3d (GLdouble dRed, GLdouble dGreen, GLdouble dBlue);
GT_EXTERN void GTglColor3dv (const GLdouble* adRGB);
GT_EXTERN void GTglColor3f (GLfloat fRed, GLfloat fGreen, GLfloat fBlue);
GT_EXTERN void GTglColor3fv (const GLfloat* afRGB);
GT_EXTERN void GTglColor3i (GLint iRed, GLint iGreen, GLint iBlue);
GT_EXTERN void GTglColor3iv (const GLint* aiRGB);
GT_EXTERN void GTglColor3s (GLshort sRed, GLshort sGreen, GLshort sBlue);
GT_EXTERN void GTglColor3sv (const GLshort* asRGB);
GT_EXTERN void GTglColor3ub (GLubyte ucRed, GLubyte ucGreen, GLubyte ucBlue);
GT_EXTERN void GTglColor3ubv (const GLubyte* aucRGB);
GT_EXTERN void GTglColor3ui (GLuint uiRed, GLuint uiGreen, GLuint uiBlue);
GT_EXTERN void GTglColor3uiv (const GLuint* auiRGB);
GT_EXTERN void GTglColor3us (GLushort usRed, GLushort usGreen,
    GLushort usBlue);
GT_EXTERN void GTglColor3usv (const GLushort* ausRGB);
GT_EXTERN void GTglColor4b (GLbyte cRed, GLbyte cGreen, GLbyte cBlue,
    GLbyte cAlpha);
GT_EXTERN void GTglColor4bv (const GLbyte* acRGBA);
GT_EXTERN void GTglColor4d (GLdouble dRed, GLdouble dGreen, GLdouble dBlue,
    GLdouble dAlpha);
GT_EXTERN void GTglColor4dv (const GLdouble* adRGBA);
GT_EXTERN void GTglColor4f (GLfloat fRed, GLfloat fGreen, GLfloat fBlue,
    GLfloat fAlpha);
GT_EXTERN void GTglColor4fv (const GLfloat* afRGBA);
GT_EXTERN void GTglColor4i (GLint iRed, GLint iGreen, GLint iBlue,
    GLint iAlpha);
GT_EXTERN void GTglColor4iv (const GLint* aiRGBA);
GT_EXTERN void GTglColor4s (GLshort sRed, GLshort sGreen, GLshort sBlue,
    GLshort sAlpha);
GT_EXTERN void GTglColor4sv (const GLshort* asRGBA);
GT_EXTERN void GTglColor4ub (GLubyte ucRed, GLubyte ucGreen, GLubyte ucBlue,
    GLubyte ucAlpha);
GT_EXTERN void GTglColor4ubv (const GLubyte* aucRGBA);
GT_EXTERN void GTglColor4ui (GLuint uiRed, GLuint uiGreen, GLuint uiBlue,
    GLuint uiAlpha);
GT_EXTERN void GTglColor4uiv (const GLuint* auiRGBA);
GT_EXTERN void GTglColor4us (GLushort usRed, GLushort usGreen,
    GLushort usBlue, GLushort usAlpha);
GT_EXTERN void GTglColor4usv (const GLushort* ausRGBA);
GT_EXTERN void GTglColorMask (GLboolean ucRed, GLboolean ucGreen,
    GLboolean ucBlue, GLboolean ucAlpha);
GT_EXTERN void GTglColorMaterial (GLenum eFace, GLenum eMode);
GT_EXTERN void GTglColorPointer (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
GT_EXTERN void GTglCopyPixels (GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight, GLenum eType);
GT_EXTERN void GTglCopyTexImage1D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth,
    GLint iBorder);
GT_EXTERN void GTglCopyTexImage2D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat,  GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight, GLint iBorder);
GT_EXTERN void GTglCopyTexSubImage1D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iX, GLint iY, GLsizei iWidth);
GT_EXTERN void GTglCopyTexSubImage2D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight);
GT_EXTERN void GTglCullFace (GLenum eMode);
GT_EXTERN void GTglDeleteLists (GLuint uiList, GLsizei iRange);
GT_EXTERN void GTglDeleteTextures (GLsizei iNumTextures,
    const GLuint* auiTextures);
GT_EXTERN void GTglDepthFunc (GLenum eFunction);
GT_EXTERN void GTglDepthMask (GLboolean ucMask);
GT_EXTERN void GTglDepthRange (GLclampd dZNear, GLclampd dZFar);
GT_EXTERN void GTglDisable (GLenum eCapability);
GT_EXTERN void GTglDisableClientState (GLenum eArray);
GT_EXTERN void GTglDrawArrays (GLenum eMode, GLint iFirst, GLsizei iCount);
GT_EXTERN void GTglDrawBuffer (GLenum eMode);
GT_EXTERN void GTglDrawElements (GLenum eMode, GLsizei iCount, GLenum eType,
    const GLvoid* pvIndices);
GT_EXTERN void GTglDrawPixels (GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLenum eType, const GLvoid* pvPixels);
GT_EXTERN void GTglEdgeFlag (GLboolean ucFlag);
GT_EXTERN void GTglEdgeFlagPointer (GLsizei iStride, const GLvoid* pvPointer);
GT_EXTERN void GTglEdgeFlagv (const GLboolean* pucFlag);
GT_EXTERN void GTglEnable (GLenum eCapability);
GT_EXTERN void GTglEnableClientState (GLenum eArray);
GT_EXTERN void GTglEnd ();
GT_EXTERN void GTglEndList ();
GT_EXTERN void GTglEvalCoord1d (GLdouble dU);
GT_EXTERN void GTglEvalCoord1dv (const GLdouble* pdU);
GT_EXTERN void GTglEvalCoord1f (GLfloat fU);
GT_EXTERN void GTglEvalCoord1fv (const GLfloat* pfU);
GT_EXTERN void GTglEvalCoord2d (GLdouble dU, GLdouble dV);
GT_EXTERN void GTglEvalCoord2dv (const GLdouble* adUV);
GT_EXTERN void GTglEvalCoord2f (GLfloat fU, GLfloat fV);
GT_EXTERN void GTglEvalCoord2fv (const GLfloat* afUV);
GT_EXTERN void GTglEvalMesh1 (GLenum eMode, GLint i1, GLint i2);
GT_EXTERN void GTglEvalMesh2 (GLenum eMode, GLint i1, GLint i2, GLint j1,
    GLint j2);
GT_EXTERN void GTglEvalPoint1 (GLint i);
GT_EXTERN void GTglEvalPoint2 (GLint i, GLint j);
GT_EXTERN void GTglFeedbackBuffer (GLsizei iSize, GLenum eType,
    GLfloat* afBuffer);
GT_EXTERN void GTglFinish ();
GT_EXTERN void GTglFlush ();
GT_EXTERN void GTglFogf (GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglFogfv (GLenum ePName, const GLfloat* afParam);
GT_EXTERN void GTglFogi (GLenum ePName, GLint iParam);
GT_EXTERN void GTglFogiv (GLenum ePName, const GLint* aiParam);
GT_EXTERN void GTglFrontFace (GLenum eMode);
GT_EXTERN void GTglFrustum (GLdouble dLeft, GLdouble dRight, GLdouble dBottom,
    GLdouble dTop, GLdouble dZNear, GLdouble dZFar);
GT_EXTERN GLuint GTglGenLists (GLsizei iRange);
GT_EXTERN void GTglGenTextures (GLsizei iNumTextures, GLuint* auiTexture);
GT_EXTERN void GTglGetBooleanv (GLenum ePName, GLboolean* aucParam);
GT_EXTERN void GTglGetClipPlane (GLenum ePlane, GLdouble* adEquation);
GT_EXTERN void GTglGetDoublev (GLenum ePName, GLdouble* adParam);
GT_EXTERN GLenum GTglGetError ();
GT_EXTERN void GTglGetFloatv (GLenum ePName, GLfloat* afParam);
GT_EXTERN void GTglGetIntegerv (GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGetLightfv (GLenum eLight, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetLightiv (GLenum eLight, GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGetMapdv (GLenum eTarget, GLenum eQuery,
    GLdouble* adValue);
GT_EXTERN void GTglGetMapfv (GLenum eTarget, GLenum eQuery, GLfloat* afValue);
GT_EXTERN void GTglGetMapiv (GLenum eTarget, GLenum eQuery, GLint* aiValue);
GT_EXTERN void GTglGetMaterialfv (GLenum eFace, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetMaterialiv (GLenum eFace, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetPixelMapfv (GLenum eMap, GLfloat* afValue);
GT_EXTERN void GTglGetPixelMapuiv (GLenum eMap, GLuint* auiValue);
GT_EXTERN void GTglGetPixelMapusv (GLenum eMap, GLushort* ausValue);
GT_EXTERN void GTglGetPointerv (GLenum ePName, GLvoid** apvParam);
GT_EXTERN void GTglGetPolygonStipple (GLubyte* aucPattern);
GT_EXTERN const GLubyte* GTglGetString (GLenum eName);
GT_EXTERN void GTglGetTexEnvfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetTexEnviv (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetTexGendv (GLenum eCoord, GLenum ePName,
    GLdouble* adParam);
GT_EXTERN void GTglGetTexGenfv (GLenum eCoord, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetTexGeniv (GLenum eCoord, GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGetTexImage (GLenum eTarget, GLint iLevel, GLenum eFormat,
    GLenum eType, GLvoid* pvPixels);
GT_EXTERN void GTglGetTexLevelParameterfv (GLenum eTarget, GLint iLevel,
    GLenum ePName, GLfloat* afParam);
GT_EXTERN void GTglGetTexLevelParameteriv (GLenum eTarget, GLint iLevel,
    GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGetTexParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetTexParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglHint (GLenum eTarget, GLenum eMode);
GT_EXTERN void GTglIndexMask (GLuint uiMask);
GT_EXTERN void GTglIndexPointer (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
GT_EXTERN void GTglIndexd (GLdouble dIndex);
GT_EXTERN void GTglIndexdv (const GLdouble* pdIndex);
GT_EXTERN void GTglIndexf (GLfloat fIndex);
GT_EXTERN void GTglIndexfv (const GLfloat* pfIndex);
GT_EXTERN void GTglIndexi (GLint iIndex);
GT_EXTERN void GTglIndexiv (const GLint* piIndex);
GT_EXTERN void GTglIndexs (GLshort sIndex);
GT_EXTERN void GTglIndexsv (const GLshort* psIndex);
GT_EXTERN void GTglIndexub (GLubyte ucIndex);
GT_EXTERN void GTglIndexubv (const GLubyte* pucIndex);
GT_EXTERN void GTglInitNames ();
GT_EXTERN void GTglInterleavedArrays (GLenum eFormat, GLsizei iStride,
    const GLvoid* pvPointer);
GT_EXTERN GLboolean GTglIsEnabled (GLenum eCapability);
GT_EXTERN GLboolean GTglIsList (GLuint uiList);
GT_EXTERN GLboolean GTglIsTexture (GLuint uiTexture);
GT_EXTERN void GTglLightModelf (GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglLightModelfv (GLenum ePName, const GLfloat* afParam);
GT_EXTERN void GTglLightModeli (GLenum ePName, GLint iParam);
GT_EXTERN void GTglLightModeliv (GLenum ePName, const GLint* aiParam);
GT_EXTERN void GTglLightf (GLenum eLight, GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglLightfv (GLenum eLight, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglLighti (GLenum eLight, GLenum ePName, GLint iParam);
GT_EXTERN void GTglLightiv (GLenum eLight, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglLineStipple (GLint iFactor, GLushort usPattern);
GT_EXTERN void GTglLineWidth (GLfloat fWidth);
GT_EXTERN void GTglListBase (GLuint uiBase);
GT_EXTERN void GTglLoadIdentity ();
GT_EXTERN void GTglLoadMatrixd (const GLdouble* adMatrix);
GT_EXTERN void GTglLoadMatrixf (const GLfloat* afMatrix);
GT_EXTERN void GTglLoadName (GLuint uiName);
GT_EXTERN void GTglLogicOp (GLenum eOpCode);
GT_EXTERN void GTglMap1d (GLenum eTarget, GLdouble dU1, GLdouble dU2,
    GLint iStride, GLint iOrder, const GLdouble* adPoints);
GT_EXTERN void GTglMap1f (GLenum eTarget, GLfloat fU1, GLfloat fU2,
    GLint iStride, GLint iOrder, const GLfloat* afPoints);
GT_EXTERN void GTglMap2d (GLenum eTarget, GLdouble dU1, GLdouble dU2,
    GLint iUStride, GLint iUOrder, GLdouble dV1, GLdouble dV2,
    GLint iVStride, GLint iVOrder, const GLdouble* adPoints);
GT_EXTERN void GTglMap2f (GLenum eTarget, GLfloat fU1, GLfloat fU2,
    GLint iUStride, GLint iUOrder, GLfloat fV1, GLfloat fV2, GLint iVStride,
    GLint iVOrder, const GLfloat* afPoints);
GT_EXTERN void GTglMapGrid1d (GLint iNumUPartitions, GLdouble dU1,
    GLdouble dU2);
GT_EXTERN void GTglMapGrid1f (GLint iNumUPartitions, GLfloat fU1,
    GLfloat fU2);
GT_EXTERN void GTglMapGrid2d (GLint iNumUPartitions, GLdouble dU1,
    GLdouble dU2, GLint iNumVPartitions, GLdouble dV1, GLdouble dV2);
GT_EXTERN void GTglMapGrid2f (GLint iNumUPartitions, GLfloat fU1, GLfloat fU2,
    GLint iNumVPartitions, GLfloat fV1, GLfloat fV2);
GT_EXTERN void GTglMaterialf (GLenum eFace, GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglMaterialfv (GLenum eFace, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglMateriali (GLenum eFace, GLenum ePName, GLint iParam);
GT_EXTERN void GTglMaterialiv (GLenum eFace, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglMatrixMode (GLenum eMode);
GT_EXTERN void GTglMultMatrixd (const GLdouble* adMatrix);
GT_EXTERN void GTglMultMatrixf (const GLfloat* afMatrix);
GT_EXTERN void GTglNewList (GLuint uiList, GLenum eMode);
GT_EXTERN void GTglNormal3b (GLbyte cX, GLbyte cY, GLbyte cZ);
GT_EXTERN void GTglNormal3bv (const GLbyte* acXYZ);
GT_EXTERN void GTglNormal3d (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglNormal3dv (const GLdouble* adXYZ);
GT_EXTERN void GTglNormal3f (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglNormal3fv (const GLfloat* afXYZ);
GT_EXTERN void GTglNormal3i (GLint iX, GLint iY, GLint iZ);
GT_EXTERN void GTglNormal3iv (const GLint* aiXYZ);
GT_EXTERN void GTglNormal3s (GLshort sX, GLshort sY, GLshort sZ);
GT_EXTERN void GTglNormal3sv (const GLshort* asXYZ);
GT_EXTERN void GTglNormalPointer (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
GT_EXTERN void GTglOrtho (GLdouble dLeft, GLdouble dRight, GLdouble dBottom,
    GLdouble dTop, GLdouble dZNear, GLdouble dZFar);
GT_EXTERN void GTglPassThrough (GLfloat fToken);
GT_EXTERN void GTglPixelMapfv (GLenum eMap, GLsizei iSize,
    const GLfloat* afValue);
GT_EXTERN void GTglPixelMapuiv (GLenum eMap, GLsizei iSize,
    const GLuint* auiValue);
GT_EXTERN void GTglPixelMapusv (GLenum eMap, GLsizei iSize,
    const GLushort* ausValue);
GT_EXTERN void GTglPixelStoref (GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglPixelStorei (GLenum ePName, GLint iParam);
GT_EXTERN void GTglPixelTransferf (GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglPixelTransferi (GLenum ePName, GLint iParam);
GT_EXTERN void GTglPixelZoom (GLfloat fXFactor, GLfloat fYFactor);
GT_EXTERN void GTglPointSize (GLfloat fSize);
GT_EXTERN void GTglPolygonMode (GLenum eFace, GLenum eMode);
GT_EXTERN void GTglPolygonOffset (GLfloat fScale, GLfloat fBias);
GT_EXTERN void GTglPolygonStipple (const GLubyte* aucPattern);
GT_EXTERN void GTglPopAttrib ();
GT_EXTERN void GTglPopClientAttrib ();
GT_EXTERN void GTglPopMatrix ();
GT_EXTERN void GTglPopName ();
GT_EXTERN void GTglPrioritizeTextures (GLsizei iNumTextures,
    const GLuint* auiTextures, const GLclampf* afPriorities);
GT_EXTERN void GTglPushAttrib (GLbitfield uiMask);
GT_EXTERN void GTglPushClientAttrib (GLbitfield uiMask);
GT_EXTERN void GTglPushMatrix ();
GT_EXTERN void GTglPushName (GLuint uiName);
GT_EXTERN void GTglRasterPos2d (GLdouble dX, GLdouble dY);
GT_EXTERN void GTglRasterPos2dv (const GLdouble* adXY);
GT_EXTERN void GTglRasterPos2f (GLfloat fX, GLfloat fY);
GT_EXTERN void GTglRasterPos2fv (const GLfloat* afXY);
GT_EXTERN void GTglRasterPos2i (GLint iX, GLint iY);
GT_EXTERN void GTglRasterPos2iv (const GLint* aiXY);
GT_EXTERN void GTglRasterPos2s (GLshort sX, GLshort sY);
GT_EXTERN void GTglRasterPos2sv (const GLshort* asXY);
GT_EXTERN void GTglRasterPos3d (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglRasterPos3dv (const GLdouble* adXYZ);
GT_EXTERN void GTglRasterPos3f (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglRasterPos3fv (const GLfloat* afXYZ);
GT_EXTERN void GTglRasterPos3i (GLint iX, GLint iY, GLint iZ);
GT_EXTERN void GTglRasterPos3iv (const GLint* aiXYZ);
GT_EXTERN void GTglRasterPos3s (GLshort sX, GLshort sY, GLshort sZ);
GT_EXTERN void GTglRasterPos3sv (const GLshort* asXYZ);
GT_EXTERN void GTglRasterPos4d (GLdouble dX, GLdouble dY, GLdouble dZ,
    GLdouble dW);
GT_EXTERN void GTglRasterPos4dv (const GLdouble* adXYZW);
GT_EXTERN void GTglRasterPos4f (GLfloat fX, GLfloat fY, GLfloat fZ,
    GLfloat fW);
GT_EXTERN void GTglRasterPos4fv (const GLfloat* afXYZW);
GT_EXTERN void GTglRasterPos4i (int iX, int iY, int iZ, int iW);
GT_EXTERN void GTglRasterPos4iv (const GLint* aiXYZW);
GT_EXTERN void GTglRasterPos4s (GLshort sX, GLshort sY, GLshort sZ,
    GLshort sW);
GT_EXTERN void GTglRasterPos4sv (const GLshort* asXYZW);
GT_EXTERN void GTglReadBuffer (GLenum eMode);
GT_EXTERN void GTglReadPixels (GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight, GLenum eFormat, GLenum eType, GLvoid* pvPixels);
GT_EXTERN void GTglRectd (GLdouble dX1, GLdouble dY1, GLdouble dX2,
    GLdouble dY2);
GT_EXTERN void GTglRectdv (const GLdouble* adX1Y1, const GLdouble* adX2Y2);
GT_EXTERN void GTglRectf (GLfloat fX1, GLfloat fY1, GLfloat fX2, GLfloat fY2);
GT_EXTERN void GTglRectfv (const GLfloat* afX1Y1, const GLfloat* afX2Y2);
GT_EXTERN void GTglRecti (GLint iX1, GLint iY1, GLint iX2, GLint iY2);
GT_EXTERN void GTglRectiv (const GLint* aiX1Y1, const GLint* aiX2Y2);
GT_EXTERN void GTglRects (GLshort sX1, GLshort sY1, GLshort sX2, GLshort sY2);
GT_EXTERN void GTglRectsv (const GLshort* asX1Y1, const GLshort* asX2Y2);
GT_EXTERN GLint GTglRenderMode (GLenum eMode);
GT_EXTERN void GTglRotated (GLdouble dAngle, GLdouble dX, GLdouble dY,
    GLdouble dZ);
GT_EXTERN void GTglRotatef (GLfloat fAngle, GLfloat fX, GLfloat fY,
    GLfloat fZ);
GT_EXTERN void GTglScaled (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglScalef (GLfloat fAngle, GLfloat fX, GLfloat fY);
GT_EXTERN void GTglScissor (GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight);
GT_EXTERN void GTglSelectBuffer (GLsizei iSize, GLuint* auiBuffer);
GT_EXTERN void GTglShadeModel (GLenum eMode);
GT_EXTERN void GTglStencilFunc (GLenum eFunction, GLint iReference,
    GLuint uiMask);
GT_EXTERN void GTglStencilMask (GLuint uiMask);
GT_EXTERN void GTglStencilOp (GLenum eFail, GLenum eZFail, GLenum eZPass);
GT_EXTERN void GTglTexCoord1d (GLdouble dS);
GT_EXTERN void GTglTexCoord1dv (const GLdouble* pdS);
GT_EXTERN void GTglTexCoord1f (GLfloat fS);
GT_EXTERN void GTglTexCoord1fv (const GLfloat* pfS);
GT_EXTERN void GTglTexCoord1i (GLint iS);
GT_EXTERN void GTglTexCoord1iv (const GLint* piS);
GT_EXTERN void GTglTexCoord1s (GLshort sS);
GT_EXTERN void GTglTexCoord1sv (const GLshort* psS);
GT_EXTERN void GTglTexCoord2d (GLdouble dS, GLdouble dT);
GT_EXTERN void GTglTexCoord2dv (const GLdouble* asDT);
GT_EXTERN void GTglTexCoord2f (GLfloat fS, GLfloat fT);
GT_EXTERN void GTglTexCoord2fv (const GLfloat* afST);
GT_EXTERN void GTglTexCoord2i (GLint iS, GLint iT);
GT_EXTERN void GTglTexCoord2iv (const GLint* aiST);
GT_EXTERN void GTglTexCoord2s (GLshort sS, GLshort sT);
GT_EXTERN void GTglTexCoord2sv (const GLshort* asST);
GT_EXTERN void GTglTexCoord3d (GLdouble dS, GLdouble dT, GLdouble dR);
GT_EXTERN void GTglTexCoord3dv (const GLdouble* adSTR);
GT_EXTERN void GTglTexCoord3f (GLfloat fS, GLfloat fT, GLfloat fR);
GT_EXTERN void GTglTexCoord3fv (const GLfloat* afSTR);
GT_EXTERN void GTglTexCoord3i (GLint iS, GLint iT, GLint iR);
GT_EXTERN void GTglTexCoord3iv (const GLint* aiSTR);
GT_EXTERN void GTglTexCoord3s (GLshort sS, GLshort sT, GLshort sR);
GT_EXTERN void GTglTexCoord3sv (const GLshort* asSTR);
GT_EXTERN void GTglTexCoord4d (GLdouble dS, GLdouble dT, GLdouble dR,
    GLdouble dQ);
GT_EXTERN void GTglTexCoord4dv (const GLdouble* adSTRQ);
GT_EXTERN void GTglTexCoord4f (GLfloat fS, GLfloat fT, GLfloat fR,
    GLfloat fQ);
GT_EXTERN void GTglTexCoord4fv (const GLfloat* afSTRQ);
GT_EXTERN void GTglTexCoord4i (GLint iS, GLint iT, GLint iR, GLint iQ);
GT_EXTERN void GTglTexCoord4iv (const GLint* aiSTRQ);
GT_EXTERN void GTglTexCoord4s (GLshort sS, GLshort sT, GLshort sR,
    GLshort sQ);
GT_EXTERN void GTglTexCoord4sv (const GLshort* asSTRQ);
GT_EXTERN void GTglTexCoordPointer (GLint iSize, GLenum eType,
    GLsizei iStride, const GLvoid* pvPointer);
GT_EXTERN void GTglTexEnvf (GLenum eTarget, GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglTexEnvfv (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglTexEnvi (GLenum eTarget, GLenum ePName, GLint iParam);
GT_EXTERN void GTglTexEnviv (GLenum eTarget, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglTexGend (GLenum eCoord, GLenum ePName, GLdouble dParam);
GT_EXTERN void GTglTexGendv (GLenum eCoord, GLenum ePName,
    const GLdouble* adParam);
GT_EXTERN void GTglTexGenf (GLenum eCoord, GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglTexGenfv (GLenum eCoord, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglTexGeni (GLenum eCoord, GLenum ePName, GLint iParam);
GT_EXTERN void GTglTexGeniv (GLenum eCoord, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglTexImage1D (GLenum eTarget, GLint iLevel,
    GLint iInternalFormat, GLsizei iWidth, GLint iBorder, GLenum eFormat,
    GLenum eType, const GLvoid* pvPixels);
GT_EXTERN void GTglTexImage2D (GLenum eTarget, GLint iLevel,
    GLint iInternalFormat, GLsizei iWidth, GLsizei iHeight, GLint iBorder,
    GLenum eFormat, GLenum eType, const GLvoid* pvPixels);
GT_EXTERN void GTglTexParameterf (GLenum eTarget, GLenum ePName,
    GLfloat fParam);
GT_EXTERN void GTglTexParameterfv (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglTexParameteri (GLenum eTarget, GLenum ePName,
    GLint iParam);
GT_EXTERN void GTglTexParameteriv (GLenum eTarget, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglTexSubImage1D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLsizei iWidth, GLenum eFormat, GLenum eType,
    const GLvoid* pvPixels);
GT_EXTERN void GTglTexSubImage2D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLenum eType, const GLvoid* pvPixels);
GT_EXTERN void GTglTranslated (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglTranslatef (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglVertex2d (GLdouble dX, GLdouble dY);
GT_EXTERN void GTglVertex2dv (const GLdouble* adXY);
GT_EXTERN void GTglVertex2f (GLfloat fX, GLfloat fY);
GT_EXTERN void GTglVertex2fv (const GLfloat* afXY);
GT_EXTERN void GTglVertex2i (GLint iX, GLint iY);
GT_EXTERN void GTglVertex2iv (const GLint* aiXY);
GT_EXTERN void GTglVertex2s (GLshort sX, GLshort sY);
GT_EXTERN void GTglVertex2sv (const GLshort* asXY);
GT_EXTERN void GTglVertex3d (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglVertex3dv (const GLdouble* adXYZ);
GT_EXTERN void GTglVertex3f (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglVertex3fv (const GLfloat* afXYZ);
GT_EXTERN void GTglVertex3i (GLint iX, GLint iY, GLint iZ);
GT_EXTERN void GTglVertex3iv (const GLint* aiXYZ);
GT_EXTERN void GTglVertex3s (GLshort sX, GLshort sY, GLshort sZ);
GT_EXTERN void GTglVertex3sv (const GLshort* asXYZ);
GT_EXTERN void GTglVertex4d (GLdouble dX, GLdouble dY, GLdouble dZ,
    GLdouble dW);
GT_EXTERN void GTglVertex4dv (const GLdouble* adXYZW);
GT_EXTERN void GTglVertex4f (GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW);
GT_EXTERN void GTglVertex4fv (const GLfloat* afXYZW);
GT_EXTERN void GTglVertex4i (GLint iX, GLint iY, GLint iZ, GLint iW);
GT_EXTERN void GTglVertex4iv (const GLint* aiXYZW);
GT_EXTERN void GTglVertex4s (GLshort sX, GLshort sY, GLshort sZ, GLshort sW);
GT_EXTERN void GTglVertex4sv (const GLshort* asXYZW);
GT_EXTERN void GTglVertexPointer (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
GT_EXTERN void GTglViewport (GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.2
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL12;

#define GL_SMOOTH_POINT_SIZE_RANGE                                  0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY                            0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE                                  0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY                            0x0B23
#define GL_UNSIGNED_BYTE_3_3_2                                      0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4                                   0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1                                   0x8034
#define GL_UNSIGNED_INT_8_8_8_8                                     0x8035
#define GL_UNSIGNED_INT_10_10_10_2                                  0x8036
#define GL_RESCALE_NORMAL                                           0x803A
#define GL_TEXTURE_BINDING_3D                                       0x806A
#define GL_PACK_SKIP_IMAGES                                         0x806B
#define GL_PACK_IMAGE_HEIGHT                                        0x806C
#define GL_UNPACK_SKIP_IMAGES                                       0x806D
#define GL_UNPACK_IMAGE_HEIGHT                                      0x806E
#define GL_TEXTURE_3D                                               0x806F
#define GL_PROXY_TEXTURE_3D                                         0x8070
#define GL_TEXTURE_DEPTH                                            0x8071
#define GL_TEXTURE_WRAP_R                                           0x8072
#define GL_MAX_3D_TEXTURE_SIZE                                      0x8073
#define GL_BGR                                                      0x80E0
#define GL_BGRA                                                     0x80E1
#define GL_MAX_ELEMENTS_VERTICES                                    0x80E8
#define GL_MAX_ELEMENTS_INDICES                                     0x80E9
#define GL_CLAMP_TO_EDGE                                            0x812F
#define GL_TEXTURE_MIN_LOD                                          0x813A
#define GL_TEXTURE_MAX_LOD                                          0x813B
#define GL_TEXTURE_BASE_LEVEL                                       0x813C
#define GL_TEXTURE_MAX_LEVEL                                        0x813D
#define GL_LIGHT_MODEL_COLOR_CONTROL                                0x81F8
#define GL_SINGLE_COLOR                                             0x81F9
#define GL_SEPARATE_SPECULAR_COLOR                                  0x81FA
#define GL_UNSIGNED_BYTE_2_3_3_REV                                  0x8362
#define GL_UNSIGNED_SHORT_5_6_5                                     0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV                                 0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV                               0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV                               0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV                                 0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV                              0x8368
#define GL_ALIASED_POINT_SIZE_RANGE                                 0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE                                 0x846E

#define glDrawRangeElements GTglDrawRangeElements
#define glTexImage3D GTglTexImage3D
#define glTexSubImage3D GTglTexSubImage3D
#define glCopyTexSubImage3D GTglCopyTexSubImage3D

GT_EXTERN void GTglDrawRangeElements (GLenum eMode, GLuint uiStart,
    GLuint uiEnd, GLsizei iCount, GLenum eType, const GLvoid* pvIndices);
GT_EXTERN void GTglTexImage3D (GLenum eTarget, GLint iLevel,
    GLint iInternalFormat, GLsizei iWidth, GLsizei iHeight, GLsizei iDepth,
    GLint iBorder, GLenum eFormat, GLenum eType, const GLvoid* pvData);
GT_EXTERN void GTglTexSubImage3D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLsizei iWidth,
    GLsizei iHeight, GLsizei iDepth, GLenum eFormat, GLenum eType,
    const GLvoid* pvData);
GT_EXTERN void GTglCopyTexSubImage3D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLint iX, GLint iY,
    GLsizei iWidth, GLsizei iHeight);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Imaging Subset (GL_ARB_imaging)
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbImaging;

#define GL_CONSTANT_COLOR                                           0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR                                 0x8002
#define GL_CONSTANT_ALPHA                                           0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA                                 0x8004
#define GL_BLEND_COLOR                                              0x8005
#define GL_FUNC_ADD                                                 0x8006
#define GL_MIN                                                      0x8007
#define GL_MAX                                                      0x8008
#define GL_BLEND_EQUATION                                           0x8009
#define GL_FUNC_SUBTRACT                                            0x800A
#define GL_FUNC_REVERSE_SUBTRACT                                    0x800B
#define GL_CONVOLUTION_1D                                           0x8010
#define GL_CONVOLUTION_2D                                           0x8011
#define GL_SEPARABLE_2D                                             0x8012
#define GL_CONVOLUTION_BORDER_MODE                                  0x8013
#define GL_CONVOLUTION_FILTER_SCALE                                 0x8014
#define GL_CONVOLUTION_FILTER_BIAS                                  0x8015
#define GL_REDUCE                                                   0x8016
#define GL_CONVOLUTION_FORMAT                                       0x8017
#define GL_CONVOLUTION_WIDTH                                        0x8018
#define GL_CONVOLUTION_HEIGHT                                       0x8019
#define GL_MAX_CONVOLUTION_WIDTH                                    0x801A
#define GL_MAX_CONVOLUTION_HEIGHT                                   0x801B
#define GL_POST_CONVOLUTION_RED_SCALE                               0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE                             0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE                              0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE                             0x801F
#define GL_POST_CONVOLUTION_RED_BIAS                                0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS                              0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS                               0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS                              0x8023
#define GL_HISTOGRAM                                                0x8024
#define GL_PROXY_HISTOGRAM                                          0x8025
#define GL_HISTOGRAM_WIDTH                                          0x8026
#define GL_HISTOGRAM_FORMAT                                         0x8027
#define GL_HISTOGRAM_RED_SIZE                                       0x8028
#define GL_HISTOGRAM_GREEN_SIZE                                     0x8029
#define GL_HISTOGRAM_BLUE_SIZE                                      0x802A
#define GL_HISTOGRAM_ALPHA_SIZE                                     0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE                                 0x802C
#define GL_HISTOGRAM_SINK                                           0x802D
#define GL_MINMAX                                                   0x802E
#define GL_MINMAX_FORMAT                                            0x802F
#define GL_MINMAX_SINK                                              0x8030
#define GL_TABLE_TOO_LARGE                                          0x8031
#define GL_COLOR_MATRIX                                             0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH                                 0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH                             0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE                              0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE                            0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE                             0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE                            0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS                               0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS                             0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS                              0x80BA
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS                             0x80BB
#define GL_COLOR_TABLE                                              0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE                             0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE                            0x80D2
#define GL_PROXY_COLOR_TABLE                                        0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE                       0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE                      0x80D5
#define GL_COLOR_TABLE_SCALE                                        0x80D6
#define GL_COLOR_TABLE_BIAS                                         0x80D7
#define GL_COLOR_TABLE_FORMAT                                       0x80D8
#define GL_COLOR_TABLE_WIDTH                                        0x80D9
#define GL_COLOR_TABLE_RED_SIZE                                     0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE                                   0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE                                    0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE                                   0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE                               0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE                               0x80DF
#define GL_CONSTANT_BORDER                                          0x8151
#define GL_REPLICATE_BORDER                                         0x8153
#define GL_CONVOLUTION_BORDER_COLOR                                 0x8154

#define glBlendColor GTglBlendColor
#define glBlendEquation GTglBlendEquation
#define glColorTable GTglColorTable
#define glColorTableParameterfv GTglColorTableParameterfv
#define glColorTableParameteriv GTglColorTableParameteriv
#define glCopyColorTable GTglCopyColorTable
#define glGetColorTable GTglGetColorTable
#define glGetColorTableParameterfv GTglGetColorTableParameterfv
#define glGetColorTableParameteriv GTglGetColorTableParameteriv
#define glColorSubTable GTglColorSubTable
#define glCopyColorSubTable GTglCopyColorSubTable
#define glConvolutionFilter1D GTglConvolutionFilter1D
#define glConvolutionFilter2D GTglConvolutionFilter2D
#define glConvolutionParameterf GTglConvolutionParameterf
#define glConvolutionParameterfv GTglConvolutionParameterfv
#define glConvolutionParameteri GTglConvolutionParameteri
#define glConvolutionParameteriv GTglConvolutionParameteriv
#define glCopyConvolutionFilter1D GTglCopyConvolutionFilter1D
#define glCopyConvolutionFilter2D GTglCopyConvolutionFilter2D
#define glGetConvolutionFilter GTglGetConvolutionFilter
#define glGetConvolutionParameterfv GTglGetConvolutionParameterfv
#define glGetConvolutionParameteriv GTglGetConvolutionParameteriv
#define glGetSeparableFilter GTglGetSeparableFilter
#define glSeparableFilter2D GTglSeparableFilter2D
#define glGetHistogram GTglGetHistogram
#define glGetHistogramParameterfv GTglGetHistogramParameterfv
#define glGetHistogramParameteriv GTglGetHistogramParameteriv
#define glGetMinmax GTglGetMinmax
#define glGetMinmaxParameterfv GTglGetMinmaxParameterfv
#define glGetMinmaxParameteriv GTglGetMinmaxParameteriv
#define glHistogram GTglHistogram
#define glMinmax GTglMinmax
#define glResetHistogram GTglResetHistogram
#define glResetMinmax GTglResetMinmax

GT_EXTERN void GTglBlendColor (GLclampf fRed, GLclampf fGreen,
    GLclampf fBlue, GLclampf fAlpha);
GT_EXTERN void GTglBlendEquation (GLenum eMode);
GT_EXTERN void GTglColorTable (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLenum eFormat, GLenum eType, const GLvoid* pvTable);
GT_EXTERN void GTglColorTableParameterfv (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglColorTableParameteriv (GLenum eTarget, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglCopyColorTable (GLenum eTarget, GLenum eInternalFormat,
    GLint iX, GLint iY, GLsizei iWidth);
GT_EXTERN void GTglGetColorTable (GLenum eTarget, GLenum eFormat,
    GLenum eType, GLvoid* pvTable);
GT_EXTERN void GTglGetColorTableParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetColorTableParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglColorSubTable (GLenum eTarget, GLsizei iStart,
    GLsizei iCount, GLenum eFormat, GLenum eType, const GLvoid* pvData);
GT_EXTERN void GTglCopyColorSubTable (GLenum eTarget, GLsizei iStart,
    GLint iX, GLint iY, GLsizei iWidth);
GT_EXTERN void GTglConvolutionFilter1D (GLenum eTarget,
    GLenum eInternalFormat, GLsizei iWidth, GLenum eFormat, GLenum eType,
    const GLvoid* pvImage);
GT_EXTERN void GTglConvolutionFilter2D (GLenum eTarget,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLenum eFormat,
    GLenum eType, const GLvoid* pvImage);
GT_EXTERN void GTglConvolutionParameterf (GLenum eTarget, GLenum ePName,
    GLfloat fParam);
GT_EXTERN void GTglConvolutionParameterfv (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglConvolutionParameteri (GLenum eTarget, GLenum ePName,
    GLint iParam);
GT_EXTERN void GTglConvolutionParameteriv (GLenum eTarget, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglCopyConvolutionFilter1D (GLenum eTarget,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth);
GT_EXTERN void GTglCopyConvolutionFilter2D (GLenum eTarget,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight);
GT_EXTERN void GTglGetConvolutionFilter (GLenum eTarget, GLenum eFormat,
    GLenum eType, GLvoid* pvImage);
GT_EXTERN void GTglGetConvolutionParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetConvolutionParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetSeparableFilter (GLenum eTarget, GLenum eFormat,
    GLenum eType, GLvoid* pvRow, GLvoid* pvColumn, GLvoid* pvSpan);
GT_EXTERN void GTglSeparableFilter2D (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLenum eFormat, GLenum eType,
    const GLvoid* pvRow, const GLvoid* pvColumn);
GT_EXTERN void GTglGetHistogram (GLenum eTarget, GLboolean ucReset,
    GLenum eFormat, GLenum eType, GLvoid* pvValues);
GT_EXTERN void GTglGetHistogramParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetHistogramParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetMinmax (GLenum eTarget, GLboolean ucReset,
    GLenum eFormat, GLenum eType, GLvoid* pvValues);
GT_EXTERN void GTglGetMinmaxParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetMinmaxParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglHistogram (GLenum eTarget, GLsizei iWidth,
    GLenum eInternalFormat, GLboolean ucSink);
GT_EXTERN void GTglMinmax (GLenum eTarget, GLenum eInternalFormat,
    GLboolean ucSink);
GT_EXTERN void GTglResetHistogram (GLenum eTarget);
GT_EXTERN void GTglResetMinmax (GLenum eTarget);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.3
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL13;

#define GL_TEXTURE0                                                 0x84C0
#define GL_TEXTURE1                                                 0x84C1
#define GL_TEXTURE2                                                 0x84C2
#define GL_TEXTURE3                                                 0x84C3
#define GL_TEXTURE4                                                 0x84C4
#define GL_TEXTURE5                                                 0x84C5
#define GL_TEXTURE6                                                 0x84C6
#define GL_TEXTURE7                                                 0x84C7
#define GL_TEXTURE8                                                 0x84C8
#define GL_TEXTURE9                                                 0x84C9
#define GL_TEXTURE10                                                0x84CA
#define GL_TEXTURE11                                                0x84CB
#define GL_TEXTURE12                                                0x84CC
#define GL_TEXTURE13                                                0x84CD
#define GL_TEXTURE14                                                0x84CE
#define GL_TEXTURE15                                                0x84CF
#define GL_TEXTURE16                                                0x84D0
#define GL_TEXTURE17                                                0x84D1
#define GL_TEXTURE18                                                0x84D2
#define GL_TEXTURE19                                                0x84D3
#define GL_TEXTURE20                                                0x84D4
#define GL_TEXTURE21                                                0x84D5
#define GL_TEXTURE22                                                0x84D6
#define GL_TEXTURE23                                                0x84D7
#define GL_TEXTURE24                                                0x84D8
#define GL_TEXTURE25                                                0x84D9
#define GL_TEXTURE26                                                0x84DA
#define GL_TEXTURE27                                                0x84DB
#define GL_TEXTURE28                                                0x84DC
#define GL_TEXTURE29                                                0x84DD
#define GL_TEXTURE30                                                0x84DE
#define GL_TEXTURE31                                                0x84DF
#define GL_ACTIVE_TEXTURE                                           0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE                                    0x84E1
#define GL_MAX_TEXTURE_UNITS                                        0x84E2
#define GL_TRANSPOSE_MODELVIEW_MATRIX                               0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX                              0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX                                 0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX                                   0x84E6
#define GL_MULTISAMPLE                                              0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE                                 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE                                      0x809F
#define GL_SAMPLE_COVERAGE                                          0x80A0
#define GL_SAMPLE_BUFFERS                                           0x80A8
#define GL_SAMPLES                                                  0x80A9
#define GL_SAMPLE_COVERAGE_VALUE                                    0x80AA
#define GL_SAMPLE_COVERAGE_INVERT                                   0x80AB
#define GL_MULTISAMPLE_BIT                                          0x20000000
#define GL_NORMAL_MAP                                               0x8511
#define GL_REFLECTION_MAP                                           0x8512
#define GL_TEXTURE_CUBE_MAP                                         0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP                                 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X                              0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X                              0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y                              0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y                              0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z                              0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z                              0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP                                   0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE                                0x851C
#define GL_COMPRESSED_ALPHA                                         0x84E9
#define GL_COMPRESSED_LUMINANCE                                     0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA                               0x84EB
#define GL_COMPRESSED_INTENSITY                                     0x84EC
#define GL_COMPRESSED_RGB                                           0x84ED
#define GL_COMPRESSED_RGBA                                          0x84EE
#define GL_TEXTURE_COMPRESSION_HINT                                 0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE                            0x86A0
#define GL_TEXTURE_COMPRESSED                                       0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS                           0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS                               0x86A3
#define GL_CLAMP_TO_BORDER                                          0x812D
#define GL_COMBINE                                                  0x8570
#define GL_COMBINE_RGB                                              0x8571
#define GL_COMBINE_ALPHA                                            0x8572
#define GL_SOURCE0_RGB                                              0x8580
#define GL_SOURCE1_RGB                                              0x8581
#define GL_SOURCE2_RGB                                              0x8582
#define GL_SOURCE0_ALPHA                                            0x8588
#define GL_SOURCE1_ALPHA                                            0x8589
#define GL_SOURCE2_ALPHA                                            0x858A
#define GL_OPERAND0_RGB                                             0x8590
#define GL_OPERAND1_RGB                                             0x8591
#define GL_OPERAND2_RGB                                             0x8592
#define GL_OPERAND0_ALPHA                                           0x8598
#define GL_OPERAND1_ALPHA                                           0x8599
#define GL_OPERAND2_ALPHA                                           0x859A
#define GL_RGB_SCALE                                                0x8573
#define GL_ADD_SIGNED                                               0x8574
#define GL_INTERPOLATE                                              0x8575
#define GL_SUBTRACT                                                 0x84E7
#define GL_CONSTANT                                                 0x8576
#define GL_PRIMARY_COLOR                                            0x8577
#define GL_PREVIOUS                                                 0x8578
#define GL_DOT3_RGB                                                 0x86AE
#define GL_DOT3_RGBA                                                0x86AF

#define glActiveTexture GTglActiveTexture
#define glClientActiveTexture GTglClientActiveTexture
#define glMultiTexCoord1d GTglMultiTexCoord1d
#define glMultiTexCoord1dv GTglMultiTexCoord1dv
#define glMultiTexCoord1f GTglMultiTexCoord1f
#define glMultiTexCoord1fv GTglMultiTexCoord1fv
#define glMultiTexCoord1i GTglMultiTexCoord1i
#define glMultiTexCoord1iv GTglMultiTexCoord1iv
#define glMultiTexCoord1s GTglMultiTexCoord1s
#define glMultiTexCoord1sv GTglMultiTexCoord1sv
#define glMultiTexCoord2d GTglMultiTexCoord2d
#define glMultiTexCoord2dv GTglMultiTexCoord2dv
#define glMultiTexCoord2f GTglMultiTexCoord2f
#define glMultiTexCoord2fv GTglMultiTexCoord2fv
#define glMultiTexCoord2i GTglMultiTexCoord2i
#define glMultiTexCoord2iv GTglMultiTexCoord2iv
#define glMultiTexCoord2s GTglMultiTexCoord2s
#define glMultiTexCoord2sv GTglMultiTexCoord2sv
#define glMultiTexCoord3d GTglMultiTexCoord3d
#define glMultiTexCoord3dv GTglMultiTexCoord3dv
#define glMultiTexCoord3f GTglMultiTexCoord3f
#define glMultiTexCoord3fv GTglMultiTexCoord3fv
#define glMultiTexCoord3i GTglMultiTexCoord3i
#define glMultiTexCoord3iv GTglMultiTexCoord3iv
#define glMultiTexCoord3s GTglMultiTexCoord3s
#define glMultiTexCoord3sv GTglMultiTexCoord3sv
#define glMultiTexCoord4d GTglMultiTexCoord4d
#define glMultiTexCoord4dv GTglMultiTexCoord4dv
#define glMultiTexCoord4f GTglMultiTexCoord4f
#define glMultiTexCoord4fv GTglMultiTexCoord4fv
#define glMultiTexCoord4i GTglMultiTexCoord4i
#define glMultiTexCoord4iv GTglMultiTexCoord4iv
#define glMultiTexCoord4s GTglMultiTexCoord4s
#define glMultiTexCoord4sv GTglMultiTexCoord4sv
#define glLoadTransposeMatrixd GTglLoadTransposeMatrixd
#define glLoadTransposeMatrixf GTglLoadTransposeMatrixf
#define glMultTransposeMatrixd GTglMultTransposeMatrixd
#define glMultTransposeMatrixf GTglMultTransposeMatrixf
#define glSampleCoverage GTglSampleCoverage
#define glCompressedTexImage1D GTglCompressedTexImage1D
#define glCompressedTexImage2D GTglCompressedTexImage2D
#define glCompressedTexImage3D GTglCompressedTexImage3D
#define glCompressedTexSubImage1D GTglCompressedTexSubImage1D
#define glCompressedTexSubImage2D GTglCompressedTexSubImage2D
#define glCompressedTexSubImage3D GTglCompressedTexSubImage3D
#define glGetCompressedTexImage GTglGetCompressedTexImage

GT_EXTERN void GTglActiveTexture (GLenum eTexture);
GT_EXTERN void GTglClientActiveTexture (GLenum eTexture);
GT_EXTERN void GTglMultiTexCoord1d (GLenum eTarget, GLdouble dS);
GT_EXTERN void GTglMultiTexCoord1dv (GLenum eTarget, const GLdouble* pdS);
GT_EXTERN void GTglMultiTexCoord1f (GLenum eTarget, GLfloat fS);
GT_EXTERN void GTglMultiTexCoord1fv (GLenum eTarget, const GLfloat* pfS);
GT_EXTERN void GTglMultiTexCoord1i (GLenum eTarget, GLint iS);
GT_EXTERN void GTglMultiTexCoord1iv (GLenum eTarget, const GLint* piS);
GT_EXTERN void GTglMultiTexCoord1s (GLenum eTarget, GLshort sS);
GT_EXTERN void GTglMultiTexCoord1sv (GLenum eTarget, const GLshort* psS);
GT_EXTERN void GTglMultiTexCoord2d (GLenum eTarget, GLdouble dS, GLdouble dT);
GT_EXTERN void GTglMultiTexCoord2dv (GLenum eTarget, const GLdouble* asDT);
GT_EXTERN void GTglMultiTexCoord2f (GLenum eTarget, GLfloat fS, GLfloat fT);
GT_EXTERN void GTglMultiTexCoord2fv (GLenum eTarget, const GLfloat* afST);
GT_EXTERN void GTglMultiTexCoord2i (GLenum eTarget, GLint iS, GLint iT);
GT_EXTERN void GTglMultiTexCoord2iv (GLenum eTarget, const GLint* aiST);
GT_EXTERN void GTglMultiTexCoord2s (GLenum eTarget, GLshort sS, GLshort sT);
GT_EXTERN void GTglMultiTexCoord2sv (GLenum eTarget, const GLshort* asST);
GT_EXTERN void GTglMultiTexCoord3d (GLenum eTarget, GLdouble dS, GLdouble dT,
    GLdouble dR);
GT_EXTERN void GTglMultiTexCoord3dv (GLenum eTarget, const GLdouble* adSTR);
GT_EXTERN void GTglMultiTexCoord3f (GLenum eTarget, GLfloat fS, GLfloat fT,
    GLfloat fR);
GT_EXTERN void GTglMultiTexCoord3fv (GLenum eTarget, const GLfloat* afSTR);
GT_EXTERN void GTglMultiTexCoord3i (GLenum eTarget, GLint iS, GLint iT,
    GLint iR);
GT_EXTERN void GTglMultiTexCoord3iv (GLenum eTarget, const GLint* aiSTR);
GT_EXTERN void GTglMultiTexCoord3s (GLenum eTarget, GLshort sS, GLshort sT,
    GLshort sR);
GT_EXTERN void GTglMultiTexCoord3sv (GLenum eTarget, const GLshort* asSTR);
GT_EXTERN void GTglMultiTexCoord4d (GLenum eTarget, GLdouble dS, GLdouble dT,
    GLdouble dR, GLdouble dQ);
GT_EXTERN void GTglMultiTexCoord4dv (GLenum eTarget, const GLdouble* asSTRQ);
GT_EXTERN void GTglMultiTexCoord4f (GLenum eTarget, GLfloat fS, GLfloat fT,
    GLfloat fR, GLfloat fQ);
GT_EXTERN void GTglMultiTexCoord4fv (GLenum eTarget, const GLfloat* afSTRQ);
GT_EXTERN void GTglMultiTexCoord4i (GLenum eTarget, GLint iS, GLint iT,
    GLint iR, GLint iQ);
GT_EXTERN void GTglMultiTexCoord4iv (GLenum eTarget, const GLint* aiSTRQ);
GT_EXTERN void GTglMultiTexCoord4s (GLenum eTarget, GLshort sS, GLshort sT,
    GLshort sR, GLshort sQ);
GT_EXTERN void GTglMultiTexCoord4sv (GLenum eTarget, const GLshort* asSTRQ);
GT_EXTERN void GTglLoadTransposeMatrixd (const GLdouble* adMatrix);
GT_EXTERN void GTglLoadTransposeMatrixf (const GLfloat* afMatrix);
GT_EXTERN void GTglMultTransposeMatrixd (const GLdouble* adMatrix);
GT_EXTERN void GTglMultTransposeMatrixf (const GLfloat* afMatrix);
GT_EXTERN void GTglSampleCoverage (GLclampf fValue, GLboolean ucInvert);
GT_EXTERN void GTglCompressedTexImage1D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLint iBorder, GLsizei iImageSize,
    const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexImage2D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLint iBorder,
    GLsizei iImageSize, const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexImage3D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLsizei iDepth,
    GLint iBorder, GLsizei iImageSize, const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexSubImage1D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLsizei iWidth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexSubImage2D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLsizei iImageSize, const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexSubImage3D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLsizei iWidth,
    GLsizei iHeight, GLsizei iDepth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData);
GT_EXTERN void GTglGetCompressedTexImage (GLenum eTarget, GLint iLevel,
    GLvoid* pvData);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.4
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL14;

#define GL_BLEND_DST_RGB                                            0x80C8
#define GL_BLEND_SRC_RGB                                            0x80C9
#define GL_BLEND_DST_ALPHA                                          0x80CA
#define GL_BLEND_SRC_ALPHA                                          0x80CB
#define GL_POINT_SIZE_MIN                                           0x8126
#define GL_POINT_SIZE_MAX                                           0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE                                0x8128
#define GL_POINT_DISTANCE_ATTENUATION                               0x8129
#define GL_GENERATE_MIPMAP                                          0x8191
#define GL_GENERATE_MIPMAP_HINT                                     0x8192
#define GL_DEPTH_COMPONENT16                                        0x81A5
#define GL_DEPTH_COMPONENT24                                        0x81A6
#define GL_DEPTH_COMPONENT32                                        0x81A7
#define GL_MIRRORED_REPEAT                                          0x8370
#define GL_FOG_COORDINATE_SOURCE                                    0x8450
#define GL_FOG_COORDINATE                                           0x8451
#define GL_FRAGMENT_DEPTH                                           0x8452
#define GL_CURRENT_FOG_COORDINATE                                   0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE                                0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE                              0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER                             0x8456
#define GL_FOG_COORDINATE_ARRAY                                     0x8457
#define GL_COLOR_SUM                                                0x8458
#define GL_CURRENT_SECONDARY_COLOR                                  0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE                               0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE                               0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE                             0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER                            0x845D
#define GL_SECONDARY_COLOR_ARRAY                                    0x845E
#define GL_MAX_TEXTURE_LOD_BIAS                                     0x84FD
#define GL_TEXTURE_FILTER_CONTROL                                   0x8500
#define GL_TEXTURE_LOD_BIAS                                         0x8501
#define GL_INCR_WRAP                                                0x8507
#define GL_DECR_WRAP                                                0x8508
#define GL_TEXTURE_DEPTH_SIZE                                       0x884A
#define GL_DEPTH_TEXTURE_MODE                                       0x884B
#define GL_TEXTURE_COMPARE_MODE                                     0x884C
#define GL_TEXTURE_COMPARE_FUNC                                     0x884D
#define GL_COMPARE_R_TO_TEXTURE                                     0x884E

#define glBlendFuncSeparate GTglBlendFuncSeparate
#define glFogCoordd GTglFogCoordd
#define glFogCoorddv GTglFogCoorddv
#define glFogCoordf GTglFogCoordf
#define glFogCoordfv GTglFogCoordfv
#define glFogCoordPointer GTglFogCoordPointer
#define glMultiDrawArrays GTglMultiDrawArrays
#define glMultiDrawElements GTglMultiDrawElements
#define glPointParameterf GTglPointParameterf
#define glPointParameterfv GTglPointParameterfv
#define glPointParameteri GTglPointParameteri
#define glPointParameteriv GTglPointParameteriv
#define glSecondaryColor3b GTglSecondaryColor3b
#define glSecondaryColor3bv GTglSecondaryColor3bv
#define glSecondaryColor3d GTglSecondaryColor3d
#define glSecondaryColor3dv GTglSecondaryColor3dv
#define glSecondaryColor3f GTglSecondaryColor3f
#define glSecondaryColor3fv GTglSecondaryColor3fv
#define glSecondaryColor3i GTglSecondaryColor3i
#define glSecondaryColor3iv GTglSecondaryColor3iv
#define glSecondaryColor3s GTglSecondaryColor3s
#define glSecondaryColor3sv GTglSecondaryColor3sv
#define glSecondaryColor3ub GTglSecondaryColor3ub
#define glSecondaryColor3ubv GTglSecondaryColor3ubv
#define glSecondaryColor3ui GTglSecondaryColor3ui
#define glSecondaryColor3uiv GTglSecondaryColor3uiv
#define glSecondaryColor3us GTglSecondaryColor3us
#define glSecondaryColor3usv GTglSecondaryColor3usv
#define glSecondaryColorPointer GTglSecondaryColorPointer
#define glWindowPos2d GTglWindowPos2d
#define glWindowPos2dv GTglWindowPos2dv
#define glWindowPos2f GTglWindowPos2f
#define glWindowPos2fv GTglWindowPos2fv
#define glWindowPos2i GTglWindowPos2i
#define glWindowPos2iv GTglWindowPos2iv
#define glWindowPos2s GTglWindowPos2s
#define glWindowPos2sv GTglWindowPos2sv
#define glWindowPos3d GTglWindowPos3d
#define glWindowPos3dv GTglWindowPos3dv
#define glWindowPos3f GTglWindowPos3f
#define glWindowPos3fv GTglWindowPos3fv
#define glWindowPos3i GTglWindowPos3i
#define glWindowPos3iv GTglWindowPos3iv
#define glWindowPos3s GTglWindowPos3s
#define glWindowPos3sv GTglWindowPos3sv

GT_EXTERN void GTglBlendFuncSeparate (GLenum eSrcFactorRGB,
    GLenum eDstFactorRGB, GLenum eSrcFactorA, GLenum eDstFactorA);
GT_EXTERN void GTglFogCoordd (GLdouble dCoord);
GT_EXTERN void GTglFogCoorddv (const GLdouble *adCoord);
GT_EXTERN void GTglFogCoordf (GLfloat fCoord);
GT_EXTERN void GTglFogCoordfv (const GLfloat *afCoord);
GT_EXTERN void GTglFogCoordPointer (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
GT_EXTERN void GTglMultiDrawArrays (GLenum eMode, GLint* aiFirst,
    GLsizei* aiCount, GLsizei iPrimitiveCount);
GT_EXTERN void GTglMultiDrawElements (GLenum eMode, const GLsizei* aiCount,
    GLenum eType, const GLvoid** apvIndices, GLsizei iPrimitiveCount);
GT_EXTERN void GTglPointParameterf (GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglPointParameterfv (GLenum ePName, const GLfloat* afParam);
GT_EXTERN void GTglPointParameteri (GLenum ePName, int iParam);
GT_EXTERN void GTglPointParameteriv (GLenum ePName, const int* aiParam);
GT_EXTERN void GTglSecondaryColor3b (GLbyte cR, GLbyte cG, GLbyte cB);
GT_EXTERN void GTglSecondaryColor3bv (const GLbyte* acRGB);
GT_EXTERN void GTglSecondaryColor3d (GLdouble dR, GLdouble dG, GLdouble dB);
GT_EXTERN void GTglSecondaryColor3dv (const GLdouble* adRGB);
GT_EXTERN void GTglSecondaryColor3f (GLfloat fR, GLfloat fG, GLfloat fB);
GT_EXTERN void GTglSecondaryColor3fv (const GLfloat* afRGB);
GT_EXTERN void GTglSecondaryColor3i (GLint iR, GLint iG, GLint iB);
GT_EXTERN void GTglSecondaryColor3iv (const GLint* aiRGB);
GT_EXTERN void GTglSecondaryColor3s (GLshort sR, GLshort sG, GLshort sB);
GT_EXTERN void GTglSecondaryColor3sv (const GLshort* asRGB);
GT_EXTERN void GTglSecondaryColor3ub (GLubyte ucR, GLubyte ucG, GLubyte ucB);
GT_EXTERN void GTglSecondaryColor3ubv (const GLubyte* aucRGB);
GT_EXTERN void GTglSecondaryColor3ui (GLuint uiR, GLuint uiG, GLuint uiB);
GT_EXTERN void GTglSecondaryColor3uiv (const GLuint* auiRGB);
GT_EXTERN void GTglSecondaryColor3us (GLushort usR, GLushort usG,
    GLushort usB);
GT_EXTERN void GTglSecondaryColor3usv (const GLushort* ausRGB);
GT_EXTERN void GTglSecondaryColorPointer (GLint iSize, GLenum eType,
    GLsizei iStride, const GLvoid* pvPointer);
GT_EXTERN void GTglWindowPos2d (GLdouble dX, GLdouble dY);
GT_EXTERN void GTglWindowPos2dv (const GLdouble* adXY);
GT_EXTERN void GTglWindowPos2f (GLfloat fX, GLfloat fY);
GT_EXTERN void GTglWindowPos2fv (const GLfloat* afXY);
GT_EXTERN void GTglWindowPos2i (GLint iX, GLint iY);
GT_EXTERN void GTglWindowPos2iv (const GLint* aiXY);
GT_EXTERN void GTglWindowPos2s (GLshort sX, GLshort sY);
GT_EXTERN void GTglWindowPos2sv (const GLshort* asXY);
GT_EXTERN void GTglWindowPos3d (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglWindowPos3dv (const GLdouble* adXYZ);
GT_EXTERN void GTglWindowPos3f (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglWindowPos3fv (const GLfloat* afXYZ);
GT_EXTERN void GTglWindowPos3i (GLint iX, GLint iY, GLint iZ);
GT_EXTERN void GTglWindowPos3iv (const GLint* aiXYZ);
GT_EXTERN void GTglWindowPos3s (GLshort sX, GLshort sY, GLshort sZ);
GT_EXTERN void GTglWindowPos3sv (const GLshort* asXYZ);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.5
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL15;

#define GL_BUFFER_SIZE                                              0x8764
#define GL_BUFFER_USAGE                                             0x8765
#define GL_QUERY_COUNTER_BITS                                       0x8864
#define GL_CURRENT_QUERY                                            0x8865
#define GL_QUERY_RESULT                                             0x8866
#define GL_QUERY_RESULT_AVAILABLE                                   0x8867
#define GL_ARRAY_BUFFER                                             0x8892
#define GL_ELEMENT_ARRAY_BUFFER                                     0x8893
#define GL_ARRAY_BUFFER_BINDING                                     0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING                             0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING                              0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING                              0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING                               0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING                               0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING                       0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING                           0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING                     0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING                      0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING                              0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING                       0x889F
#define GL_READ_ONLY                                                0x88B8
#define GL_WRITE_ONLY                                               0x88B9
#define GL_READ_WRITE                                               0x88BA
#define GL_BUFFER_ACCESS                                            0x88BB
#define GL_BUFFER_MAPPED                                            0x88BC
#define GL_BUFFER_MAP_POINTER                                       0x88BD
#define GL_STREAM_DRAW                                              0x88E0
#define GL_STREAM_READ                                              0x88E1
#define GL_STREAM_COPY                                              0x88E2
#define GL_STATIC_DRAW                                              0x88E4
#define GL_STATIC_READ                                              0x88E5
#define GL_STATIC_COPY                                              0x88E6
#define GL_DYNAMIC_DRAW                                             0x88E8
#define GL_DYNAMIC_READ                                             0x88E9
#define GL_DYNAMIC_COPY                                             0x88EA
#define GL_SAMPLES_PASSED                                           0x8914
#define GL_FOG_COORD_SRC                                            0x8450
#define GL_FOG_COORD                                                0x8451
#define GL_CURRENT_FOG_COORD                                        0x8453
#define GL_FOG_COORD_ARRAY_TYPE                                     0x8454
#define GL_FOG_COORD_ARRAY_STRIDE                                   0x8455
#define GL_FOG_COORD_ARRAY_POINTER                                  0x8456
#define GL_FOG_COORD_ARRAY                                          0x8457
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING                           0x889D
#define GL_SRC0_RGB                                                 0x8580
#define GL_SRC1_RGB                                                 0x8581
#define GL_SRC2_RGB                                                 0x8582
#define GL_SRC0_ALPHA                                               0x8588
#define GL_SRC1_ALPHA                                               0x8589
#define GL_SRC2_ALPHA                                               0x858A

typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define glGenQueries GTglGenQueries
#define glDeleteQueries GTglDeleteQueries
#define glIsQuery GTglIsQuery
#define glBeginQuery GTglBeginQuery
#define glEndQuery GTglEndQuery
#define glGetQueryiv GTglGetQueryiv
#define glGetQueryObjectiv GTglGetQueryObjectiv
#define glGetQueryObjectuiv GTglGetQueryObjectuiv
#define glBindBuffer GTglBindBuffer
#define glDeleteBuffers GTglDeleteBuffers
#define glGenBuffers GTglGenBuffers
#define glIsBuffer GTglIsBuffer
#define glBufferData GTglBufferData
#define glBufferSubData GTglBufferSubData
#define glGetBufferSubData GTglGetBufferSubData
#define glMapBuffer GTglMapBuffer
#define glUnmapBuffer GTglUnmapBuffer
#define glGetBufferParameteriv GTglGetBufferParameteriv
#define glGetBufferPointerv GTglGetBufferPointerv

GT_EXTERN void GTglGenQueries (GLsizei iNumQueries, GLuint* auiID);
GT_EXTERN void GTglDeleteQueries (GLsizei iNumQueries, const GLuint* auiID);
GLboolean GTglIsQuery (GLuint uiID);
GT_EXTERN void GTglBeginQuery (GLenum eTarget, GLuint uiID);
GT_EXTERN void GTglEndQuery (GLenum eTarget);
GT_EXTERN void GTglGetQueryiv (GLenum eTarget, GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGetQueryObjectiv (GLuint uiID, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetQueryObjectuiv (GLuint uiID, GLenum ePName,
    GLuint* auiParam);
GT_EXTERN void GTglBindBuffer (GLenum eTarget, GLuint uiBuffer);
GT_EXTERN void GTglDeleteBuffers (GLsizei iNumBuffers,
    const GLuint* auiBuffer);
GT_EXTERN void GTglGenBuffers (GLsizei iNumBuffers, GLuint* auiBuffer);
GT_EXTERN GLboolean GTglIsBuffer (GLuint uiBuffer);
GT_EXTERN void GTglBufferData (GLenum eTarget, GLsizeiptr piSize,
    const GLvoid* pvData, GLenum eUsage);
GT_EXTERN void GTglBufferSubData (GLenum eTarget, GLintptr piOffset,
    GLsizeiptr piSize, const GLvoid* pvData);
GT_EXTERN void GTglGetBufferSubData (GLenum eTarget, GLintptr piOffset,
    GLsizeiptr piSize, GLvoid* pvData);
GT_EXTERN GLvoid* GTglMapBuffer (GLenum eTarget, GLenum eAccess);
GT_EXTERN GLboolean GTglUnmapBuffer (GLenum eTarget);
GT_EXTERN void GTglGetBufferParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetBufferPointerv (GLenum eTarget, GLenum ePName,
    GLvoid** apvParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 2.0
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL20;

#define GL_BLEND_EQUATION_RGB                                       0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED                              0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE                                 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE                               0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE                                 0x8625
#define GL_CURRENT_VERTEX_ATTRIB                                    0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE                                0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE                                  0x8643
#define GL_VERTEX_ATTRIB_ARRAY_POINTER                              0x8645
#define GL_STENCIL_BACK_FUNC                                        0x8800
#define GL_STENCIL_BACK_FAIL                                        0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL                             0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS                             0x8803
#define GL_MAX_DRAW_BUFFERS                                         0x8824
#define GL_DRAW_BUFFER0                                             0x8825
#define GL_DRAW_BUFFER1                                             0x8826
#define GL_DRAW_BUFFER2                                             0x8827
#define GL_DRAW_BUFFER3                                             0x8828
#define GL_DRAW_BUFFER4                                             0x8829
#define GL_DRAW_BUFFER5                                             0x882A
#define GL_DRAW_BUFFER6                                             0x882B
#define GL_DRAW_BUFFER7                                             0x882C
#define GL_DRAW_BUFFER8                                             0x882D
#define GL_DRAW_BUFFER9                                             0x882E
#define GL_DRAW_BUFFER10                                            0x882F
#define GL_DRAW_BUFFER11                                            0x8830
#define GL_DRAW_BUFFER12                                            0x8831
#define GL_DRAW_BUFFER13                                            0x8832
#define GL_DRAW_BUFFER14                                            0x8833
#define GL_DRAW_BUFFER15                                            0x8834
#define GL_BLEND_EQUATION_ALPHA                                     0x883D
#define GL_POINT_SPRITE                                             0x8861
#define GL_COORD_REPLACE                                            0x8862
#define GL_MAX_VERTEX_ATTRIBS                                       0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED                           0x886A
#define GL_MAX_TEXTURE_COORDS                                       0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS                                  0x8872
#define GL_FRAGMENT_SHADER                                          0x8B30
#define GL_VERTEX_SHADER                                            0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS                          0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS                            0x8B4A
#define GL_MAX_VARYING_FLOATS                                       0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS                           0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS                         0x8B4D
#define GL_SHADER_TYPE                                              0x8B4F
#define GL_FLOAT_VEC2                                               0x8B50
#define GL_FLOAT_VEC3                                               0x8B51
#define GL_FLOAT_VEC4                                               0x8B52
#define GL_INT_VEC2                                                 0x8B53
#define GL_INT_VEC3                                                 0x8B54
#define GL_INT_VEC4                                                 0x8B55
#define GL_BOOL                                                     0x8B56
#define GL_BOOL_VEC2                                                0x8B57
#define GL_BOOL_VEC3                                                0x8B58
#define GL_BOOL_VEC4                                                0x8B59
#define GL_FLOAT_MAT2                                               0x8B5A
#define GL_FLOAT_MAT3                                               0x8B5B
#define GL_FLOAT_MAT4                                               0x8B5C
#define GL_SAMPLER_1D                                               0x8B5D
#define GL_SAMPLER_2D                                               0x8B5E
#define GL_SAMPLER_3D                                               0x8B5F
#define GL_SAMPLER_CUBE                                             0x8B60
#define GL_SAMPLER_1D_SHADOW                                        0x8B61
#define GL_SAMPLER_2D_SHADOW                                        0x8B62
#define GL_DELETE_STATUS                                            0x8B80
#define GL_COMPILE_STATUS                                           0x8B81
#define GL_LINK_STATUS                                              0x8B82
#define GL_VALIDATE_STATUS                                          0x8B83
#define GL_INFO_LOG_LENGTH                                          0x8B84
#define GL_ATTACHED_SHADERS                                         0x8B85
#define GL_ACTIVE_UNIFORMS                                          0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH                                0x8B87
#define GL_SHADER_SOURCE_LENGTH                                     0x8B88
#define GL_ACTIVE_ATTRIBUTES                                        0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH                              0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT                          0x8B8B
#define GL_SHADING_LANGUAGE_VERSION                                 0x8B8C
#define GL_CURRENT_PROGRAM                                          0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN                                0x8CA0
#define GL_LOWER_LEFT                                               0x8CA1
#define GL_UPPER_LEFT                                               0x8CA2
#define GL_STENCIL_BACK_REF                                         0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK                                  0x8CA4
#define GL_STENCIL_BACK_WRITEMASK                                   0x8CA5

typedef char GLchar;

#define glBlendEquationSeparate GTglBlendEquationSeparate
#define glDrawBuffers GTglDrawBuffers
#define glStencilOpSeparate GTglStencilOpSeparate
#define glStencilFuncSeparate GTglStencilFuncSeparate
#define glStencilMaskSeparate GTglStencilMaskSeparate
#define glAttachShader GTglAttachShader
#define glBindAttribLocation GTglBindAttribLocation
#define glCompileShader GTglCompileShader
#define glCreateProgram GTglCreateProgram
#define glCreateShader GTglCreateShader
#define glDeleteProgram GTglDeleteProgram
#define glDeleteShader GTglDeleteShader
#define glDetachShader GTglDetachShader
#define glDisableVertexAttribArray GTglDisableVertexAttribArray
#define glEnableVertexAttribArray GTglEnableVertexAttribArray
#define glGetActiveAttrib GTglGetActiveAttrib
#define glGetActiveUniform GTglGetActiveUniform
#define glGetAttachedShaders GTglGetAttachedShaders
#define glGetAttribLocation GTglGetAttribLocation
#define glGetProgramiv GTglGetProgramiv
#define glGetProgramInfoLog GTglGetProgramInfoLog
#define glGetShaderiv GTglGetShaderiv
#define glGetShaderInfoLog GTglGetShaderInfoLog
#define glGetShaderSource GTglGetShaderSource
#define glGetUniformLocation GTglGetUniformLocation
#define glGetUniformfv GTglGetUniformfv
#define glGetUniformiv GTglGetUniformiv
#define glGetVertexAttribdv GTglGetVertexAttribdv
#define glGetVertexAttribfv GTglGetVertexAttribfv
#define glGetVertexAttribiv GTglGetVertexAttribiv
#define glGetVertexAttribPointerv GTglGetVertexAttribPointerv
#define glIsProgram GTglIsProgram
#define glIsShader GTglIsShader
#define glLinkProgram GTglLinkProgram
#define glShaderSource GTglShaderSource
#define glUseProgram GTglUseProgram
#define glUniform1f GTglUniform1f
#define glUniform2f GTglUniform2f
#define glUniform3f GTglUniform3f
#define glUniform4f GTglUniform4f
#define glUniform1i GTglUniform1i
#define glUniform2i GTglUniform2i
#define glUniform3i GTglUniform3i
#define glUniform4i GTglUniform4i
#define glUniform1fv GTglUniform1fv
#define glUniform2fv GTglUniform2fv
#define glUniform3fv GTglUniform3fv
#define glUniform4fv GTglUniform4fv
#define glUniform1iv GTglUniform1iv
#define glUniform2iv GTglUniform2iv
#define glUniform3iv GTglUniform3iv
#define glUniform4iv GTglUniform4iv
#define glUniformMatrix2fv GTglUniformMatrix2fv
#define glUniformMatrix3fv GTglUniformMatrix3fv
#define glUniformMatrix4fv GTglUniformMatrix4fv
#define glValidateProgram GTglValidateProgram
#define glVertexAttrib1d GTglVertexAttrib1d
#define glVertexAttrib1dv GTglVertexAttrib1dv
#define glVertexAttrib1f GTglVertexAttrib1f
#define glVertexAttrib1fv GTglVertexAttrib1fv
#define glVertexAttrib1s GTglVertexAttrib1s
#define glVertexAttrib1sv GTglVertexAttrib1sv
#define glVertexAttrib2d GTglVertexAttrib2d
#define glVertexAttrib2dv GTglVertexAttrib2dv
#define glVertexAttrib2f GTglVertexAttrib2f
#define glVertexAttrib2fv GTglVertexAttrib2fv
#define glVertexAttrib2s GTglVertexAttrib2s
#define glVertexAttrib2sv GTglVertexAttrib2sv
#define glVertexAttrib3d GTglVertexAttrib3d
#define glVertexAttrib3dv GTglVertexAttrib3dv
#define glVertexAttrib3f GTglVertexAttrib3f
#define glVertexAttrib3fv GTglVertexAttrib3fv
#define glVertexAttrib3s GTglVertexAttrib3s
#define glVertexAttrib3sv GTglVertexAttrib3sv
#define glVertexAttrib4Nbv GTglVertexAttrib4Nbv
#define glVertexAttrib4Niv GTglVertexAttrib4Niv
#define glVertexAttrib4Nsv GTglVertexAttrib4Nsv
#define glVertexAttrib4Nub GTglVertexAttrib4Nub
#define glVertexAttrib4Nubv GTglVertexAttrib4Nubv
#define glVertexAttrib4Nuiv GTglVertexAttrib4Nuiv
#define glVertexAttrib4Nusv GTglVertexAttrib4Nusv
#define glVertexAttrib4bv GTglVertexAttrib4bv
#define glVertexAttrib4d GTglVertexAttrib4d
#define glVertexAttrib4dv GTglVertexAttrib4dv
#define glVertexAttrib4f GTglVertexAttrib4f
#define glVertexAttrib4fv GTglVertexAttrib4fv
#define glVertexAttrib4iv GTglVertexAttrib4iv
#define glVertexAttrib4s GTglVertexAttrib4s
#define glVertexAttrib4sv GTglVertexAttrib4sv
#define glVertexAttrib4ubv GTglVertexAttrib4ubv
#define glVertexAttrib4uiv GTglVertexAttrib4uiv
#define glVertexAttrib4usv GTglVertexAttrib4usv
#define glVertexAttribPointer GTglVertexAttribPointer

GT_EXTERN void GTglBlendEquationSeparate (GLenum eModeRGB, GLenum eModeAlpha);
GT_EXTERN void GTglDrawBuffers (GLsizei iNumBuffers, const GLenum* aeBuffer);
GT_EXTERN void GTglStencilOpSeparate (GLenum eFace, GLenum eSFail,
    GLenum eDPFail, GLenum eDPPass);
GT_EXTERN void GTglStencilFuncSeparate (GLenum eFrontFunc, GLenum eBackFunc,
    GLint iReference, GLuint uiMask);
GT_EXTERN void GTglStencilMaskSeparate (GLenum eFace, GLuint uiMask);
GT_EXTERN void GTglAttachShader (GLuint uiProgram, GLuint uiShader);
GT_EXTERN void GTglBindAttribLocation (GLuint uiProgram, GLuint uiIndex,
    const GLchar* acName);
GT_EXTERN void GTglCompileShader (GLuint uiShader);
GT_EXTERN GLuint GTglCreateProgram ();
GT_EXTERN GLuint GTglCreateShader (GLenum eType);
GT_EXTERN void GTglDeleteProgram (GLuint uiProgram);
GT_EXTERN void GTglDeleteShader (GLuint uiShader);
GT_EXTERN void GTglDetachShader (GLuint uiProgram, GLuint uiShader);
GT_EXTERN void GTglDisableVertexAttribArray (GLuint uiIndex);
GT_EXTERN void GTglEnableVertexAttribArray (GLuint uiIndex);
GT_EXTERN void GTglGetActiveAttrib (GLuint uiProgram, GLuint uiIndex,
    GLsizei iBufSize, GLsizei* piLength, GLint* piSize, GLenum* peType,
    GLchar* acName);
GT_EXTERN void GTglGetActiveUniform (GLuint uiProgram, GLuint uiIndex,
    GLsizei iBufSize, GLsizei* piLength, GLint* piSize, GLenum* peType,
    GLchar* acName);
GT_EXTERN void GTglGetAttachedShaders (GLuint uiProgram, GLsizei iMaxCount,
    GLsizei* aiCount, GLuint* aiObj);
GT_EXTERN GLint GTglGetAttribLocation (GLuint uiProgram,
    const GLchar* acName);
GT_EXTERN void GTglGetProgramiv (GLuint uiProgram, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetProgramInfoLog (GLuint uiProgram, GLsizei iBufSize,
    GLsizei* piLength, GLchar* acInfoLog);
GT_EXTERN void GTglGetShaderiv (GLuint uiShader, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetShaderInfoLog (GLuint uiShader, GLsizei iBufSize,
    GLsizei* piLength, GLchar* acInfoLog);
GT_EXTERN void GTglGetShaderSource (GLuint uiShader, GLsizei iBufSize,
    GLsizei* piLength, GLchar* acSource);
GT_EXTERN GLint GTglGetUniformLocation (GLuint uiProgram,
    const GLchar* acName);
GT_EXTERN void GTglGetUniformfv (GLuint uiProgram, GLint uiLocation,
    GLfloat* afParam);
GT_EXTERN void GTglGetUniformiv (GLuint uiProgram, GLint uiLocation,
    GLint* aiParam);
GT_EXTERN void GTglGetVertexAttribdv (GLuint uiIndex, GLenum ePName,
    GLdouble* adParam);
GT_EXTERN void GTglGetVertexAttribfv (GLuint uiIndex, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetVertexAttribiv (GLuint uiIndex, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetVertexAttribPointerv (GLuint uiIndex, GLenum ePName,
    GLvoid** apvParam);
GT_EXTERN GLboolean GTglIsProgram (GLuint uiProgram);
GT_EXTERN GLboolean GTglIsShader (GLuint uiShader);
GT_EXTERN void GTglLinkProgram (GLuint uiProgram);
GT_EXTERN void GTglShaderSource (GLuint uiShader, GLsizei iCount,
    const GLchar** aacString, const GLint* piLength);
GT_EXTERN void GTglUseProgram (GLuint uiProgram);
GT_EXTERN void GTglUniform1f (GLint uiLocation, GLfloat fV0);
GT_EXTERN void GTglUniform2f (GLint uiLocation, GLfloat fV0, GLfloat fV1);
GT_EXTERN void GTglUniform3f (GLint uiLocation, GLfloat fV0, GLfloat fV1,
    GLfloat fV2);
GT_EXTERN void GTglUniform4f (GLint uiLocation, GLfloat fV0, GLfloat fV1,
    GLfloat fV2, GLfloat fV3);
GT_EXTERN void GTglUniform1i (GLint uiLocation, GLint iV0);
GT_EXTERN void GTglUniform2i (GLint uiLocation, GLint iV0, GLint iV1);
GT_EXTERN void GTglUniform3i (GLint uiLocation, GLint iV0, GLint iV1,
    GLint iV2);
GT_EXTERN void GTglUniform4i (GLint uiLocation, GLint iV0, GLint iV1,
    GLint iV2, GLint iV3);
GT_EXTERN void GTglUniform1fv (GLint uiLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform2fv (GLint uiLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform3fv (GLint uiLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform4fv (GLint uiLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform1iv (GLint uiLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniform2iv (GLint uiLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniform3iv (GLint uiLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniform4iv (GLint uiLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniformMatrix2fv (GLint uiLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue);
GT_EXTERN void GTglUniformMatrix3fv (GLint uiLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue);
GT_EXTERN void GTglUniformMatrix4fv (GLint uiLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue);
GT_EXTERN void GTglValidateProgram (GLuint uiProgram);
GT_EXTERN void GTglVertexAttrib1d (GLuint uiIndex, GLdouble dX);
GT_EXTERN void GTglVertexAttrib1dv (GLuint uiIndex, const GLdouble* pdX);
GT_EXTERN void GTglVertexAttrib1f (GLuint uiIndex, GLfloat fX);
GT_EXTERN void GTglVertexAttrib1fv (GLuint uiIndex, const GLfloat* pfX);
GT_EXTERN void GTglVertexAttrib1s (GLuint uiIndex, GLshort sX);
GT_EXTERN void GTglVertexAttrib1sv (GLuint uiIndex, const GLshort* psX);
GT_EXTERN void GTglVertexAttrib2d (GLuint uiIndex, GLdouble dX, GLdouble dY);
GT_EXTERN void GTglVertexAttrib2dv (GLuint uiIndex, const GLdouble* adXY);
GT_EXTERN void GTglVertexAttrib2f (GLuint uiIndex, GLfloat fX, GLfloat fY);
GT_EXTERN void GTglVertexAttrib2fv (GLuint uiIndex, const GLfloat* afXY);
GT_EXTERN void GTglVertexAttrib2s (GLuint uiIndex, GLshort sX, GLshort sY);
GT_EXTERN void GTglVertexAttrib2sv (GLuint uiIndex, const GLshort* asXY);
GT_EXTERN void GTglVertexAttrib3d (GLuint uiIndex, GLdouble dX, GLdouble dY,
    GLdouble dZ);
GT_EXTERN void GTglVertexAttrib3dv (GLuint uiIndex, const GLdouble* adXYZ);
GT_EXTERN void GTglVertexAttrib3f (GLuint uiIndex, GLfloat fX, GLfloat fY,
    GLfloat fZ);
GT_EXTERN void GTglVertexAttrib3fv (GLuint uiIndex, const GLfloat* afXYZ);
GT_EXTERN void GTglVertexAttrib3s (GLuint uiIndex, GLshort sX, GLshort sY,
    GLshort sZ);
GT_EXTERN void GTglVertexAttrib3sv (GLuint uiIndex, const GLshort* asXYZ);
GT_EXTERN void GTglVertexAttrib4Nbv (GLuint uiIndex, const GLbyte* acXYZW);
GT_EXTERN void GTglVertexAttrib4Niv (GLuint uiIndex, const GLint* aiXYZW);
GT_EXTERN void GTglVertexAttrib4Nsv (GLuint uiIndex, const GLshort* asXYZW);
GT_EXTERN void GTglVertexAttrib4Nub (GLuint uiIndex, GLubyte ucX, GLubyte ucY,
    GLubyte ucZ, GLubyte ucW);
GT_EXTERN void GTglVertexAttrib4Nubv (GLuint uiIndex, const GLubyte* aucXYZW);
GT_EXTERN void GTglVertexAttrib4Nuiv (GLuint uiIndex, const GLuint* auiXYZW);
GT_EXTERN void GTglVertexAttrib4Nusv (GLuint uiIndex,
    const GLushort* ausXYZW);
GT_EXTERN void GTglVertexAttrib4bv (GLuint uiIndex, const GLbyte* acXYZW);
GT_EXTERN void GTglVertexAttrib4d (GLuint uiIndex, GLdouble dX, GLdouble dY,
    GLdouble dZ, GLdouble dW);
GT_EXTERN void GTglVertexAttrib4dv (GLuint uiIndex, const GLdouble* adXYZW);
GT_EXTERN void GTglVertexAttrib4f (GLuint uiIndex, GLfloat fX, GLfloat fY,
    GLfloat fZ, GLfloat fW);
GT_EXTERN void GTglVertexAttrib4fv (GLuint uiIndex, const GLfloat* afXYZW);
GT_EXTERN void GTglVertexAttrib4iv (GLuint uiIndex, const GLint* aiXYZW);
GT_EXTERN void GTglVertexAttrib4s (GLuint uiIndex, GLshort sX, GLshort sY,
    GLshort sZ, GLshort sW);
GT_EXTERN void GTglVertexAttrib4sv (GLuint uiIndex, const GLshort* asXYZW);
GT_EXTERN void GTglVertexAttrib4ubv (GLuint uiIndex, const GLubyte* aucXYZW);
GT_EXTERN void GTglVertexAttrib4uiv (GLuint uiIndex, const GLuint* auiXYZW);
GT_EXTERN void GTglVertexAttrib4usv (GLuint uiIndex, const GLushort* ausXYZW);
GT_EXTERN void GTglVertexAttribPointer (GLuint uiIndex, GLint iSize,
    GLenum eType, GLboolean ucNormalized, GLsizei iStride,
    const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 2.1
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL21;

#define GL_PIXEL_PACK_BUFFER                                        0x88EB
#define GL_PIXEL_UNPACK_BUFFER                                      0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING                                0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING                              0x88EF
#define GL_FLOAT_MAT2x3                                             0x8B65
#define GL_FLOAT_MAT2x4                                             0x8B66
#define GL_FLOAT_MAT3x2                                             0x8B67
#define GL_FLOAT_MAT3x4                                             0x8B68
#define GL_FLOAT_MAT4x2                                             0x8B69
#define GL_FLOAT_MAT4x3                                             0x8B6A
#define GL_SRGB                                                     0x8C40
#define GL_SRGB8                                                    0x8C41
#define GL_SRGB_ALPHA                                               0x8C42
#define GL_SRGB8_ALPHA8                                             0x8C43
#define GL_COMPRESSED_SRGB                                          0x8C48
#define GL_COMPRESSED_SRGB_ALPHA                                    0x8C49

#define glUniformMatrix2x3fv GTglUniformMatrix2x3fv
#define glUniformMatrix3x2fv GTglUniformMatrix3x2fv
#define glUniformMatrix2x4fv GTglUniformMatrix2x4fv
#define glUniformMatrix4x2fv GTglUniformMatrix4x2fv
#define glUniformMatrix3x4fv GTglUniformMatrix3x4fv
#define glUniformMatrix4x3fv GTglUniformMatrix4x3fv

GT_EXTERN void GTglUniformMatrix2x3fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value);
GT_EXTERN void GTglUniformMatrix3x2fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value);
GT_EXTERN void GTglUniformMatrix2x4fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value);
GT_EXTERN void GTglUniformMatrix4x2fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value);
GT_EXTERN void GTglUniformMatrix3x4fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value);
GT_EXTERN void GTglUniformMatrix4x3fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.0
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL30;

#define GL_COMPARE_REF_TO_TEXTURE                                   0x884E
#define GL_CLIP_DISTANCE0                                           0x3000
#define GL_CLIP_DISTANCE1                                           0x3001
#define GL_CLIP_DISTANCE2                                           0x3002
#define GL_CLIP_DISTANCE3                                           0x3003
#define GL_CLIP_DISTANCE4                                           0x3004
#define GL_CLIP_DISTANCE5                                           0x3005
#define GL_CLIP_DISTANCE6                                           0x3006
#define GL_CLIP_DISTANCE7                                           0x3007
#define GL_MAX_CLIP_DISTANCES                                       0x0D32
#define GL_MAJOR_VERSION                                            0x821B
#define GL_MINOR_VERSION                                            0x821C
#define GL_NUM_EXTENSIONS                                           0x821D
#define GL_CONTEXT_FLAGS                                            0x821E
#define GL_DEPTH_BUFFER                                             0x8223
#define GL_STENCIL_BUFFER                                           0x8224
#define GL_COMPRESSED_RED                                           0x8225
#define GL_COMPRESSED_RG                                            0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT                      0x0001
#define GL_RGBA32F                                                  0x8814
#define GL_RGB32F                                                   0x8815
#define GL_RGBA16F                                                  0x881A
#define GL_RGB16F                                                   0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER                              0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS                                 0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET                                 0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET                                 0x8905
#define GL_CLAMP_READ_COLOR                                         0x891C
#define GL_FIXED_ONLY                                               0x891D
#define GL_MAX_VARYING_COMPONENTS                                   0x8B4B
#define GL_TEXTURE_1D_ARRAY                                         0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY                                   0x8C19
#define GL_TEXTURE_2D_ARRAY                                         0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY                                   0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY                                 0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY                                 0x8C1D
#define GL_R11F_G11F_B10F                                           0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV                             0x8C3B
#define GL_RGB9_E5                                                  0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV                                 0x8C3E
#define GL_TEXTURE_SHARED_SIZE                                      0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH                    0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE                           0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS               0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS                              0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START                          0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE                           0x8C85
#define GL_PRIMITIVES_GENERATED                                     0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN                    0x8C88
#define GL_RASTERIZER_DISCARD                                       0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS            0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS                  0x8C8B
#define GL_INTERLEAVED_ATTRIBS                                      0x8C8C
#define GL_SEPARATE_ATTRIBS                                         0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER                                0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING                        0x8C8F
#define GL_RGBA32UI                                                 0x8D70
#define GL_RGB32UI                                                  0x8D71
#define GL_RGBA16UI                                                 0x8D76
#define GL_RGB16UI                                                  0x8D77
#define GL_RGBA8UI                                                  0x8D7C
#define GL_RGB8UI                                                   0x8D7D
#define GL_RGBA32I                                                  0x8D82
#define GL_RGB32I                                                   0x8D83
#define GL_RGBA16I                                                  0x8D88
#define GL_RGB16I                                                   0x8D89
#define GL_RGBA8I                                                   0x8D8E
#define GL_RGB8I                                                    0x8D8F
#define GL_RED_INTEGER                                              0x8D94
#define GL_GREEN_INTEGER                                            0x8D95
#define GL_BLUE_INTEGER                                             0x8D96
#define GL_RGB_INTEGER                                              0x8D98
#define GL_RGBA_INTEGER                                             0x8D99
#define GL_BGR_INTEGER                                              0x8D9A
#define GL_BGRA_INTEGER                                             0x8D9B
#define GL_SAMPLER_1D_ARRAY                                         0x8DC0
#define GL_SAMPLER_2D_ARRAY                                         0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW                                  0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW                                  0x8DC4
#define GL_SAMPLER_CUBE_SHADOW                                      0x8DC5
#define GL_UNSIGNED_INT_VEC2                                        0x8DC6
#define GL_UNSIGNED_INT_VEC3                                        0x8DC7
#define GL_UNSIGNED_INT_VEC4                                        0x8DC8
#define GL_INT_SAMPLER_1D                                           0x8DC9
#define GL_INT_SAMPLER_2D                                           0x8DCA
#define GL_INT_SAMPLER_3D                                           0x8DCB
#define GL_INT_SAMPLER_CUBE                                         0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY                                     0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY                                     0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D                                  0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D                                  0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D                                  0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE                                0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY                            0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY                            0x8DD7
#define GL_QUERY_WAIT                                               0x8E13
#define GL_QUERY_NO_WAIT                                            0x8E14
#define GL_QUERY_BY_REGION_WAIT                                     0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT                                  0x8E16
#define GL_BUFFER_ACCESS_FLAGS                                      0x911F
#define GL_BUFFER_MAP_LENGTH                                        0x9120
#define GL_BUFFER_MAP_OFFSET                                        0x9121

#define glColorMaski GTglColorMaski
#define glGetBooleani_v GTglGetBooleani_v
#define glGetIntegeri_v GTglGetIntegeri_v
#define glEnablei GTglEnablei
#define glDisablei GTglDisablei
#define glIsEnabledi GTglIsEnabledi
#define glBeginTransformFeedback GTglBeginTransformFeedback
#define glEndTransformFeedback GTglEndTransformFeedback
#define glBindBufferRange GTglBindBufferRange
#define glBindBufferBase GTglBindBufferBase
#define glTransformFeedbackVaryings GTglTransformFeedbackVaryings
#define glGetTransformFeedbackVarying GTglGetTransformFeedbackVarying
#define glClampColor GTglClampColor
#define glBeginConditionalRender GTglBeginConditionalRender
#define glEndConditionalRender GTglEndConditionalRender
#define glVertexAttribIPointer GTglVertexAttribIPointer
#define glGetVertexAttribIiv GTglGetVertexAttribIiv
#define glGetVertexAttribIuiv GTglGetVertexAttribIuiv
#define glVertexAttribI1i GTglVertexAttribI1i
#define glVertexAttribI2i GTglVertexAttribI2i
#define glVertexAttribI3i GTglVertexAttribI3i
#define glVertexAttribI4i GTglVertexAttribI4i
#define glVertexAttribI1ui GTglVertexAttribI1ui
#define glVertexAttribI2ui GTglVertexAttribI2ui
#define glVertexAttribI3ui GTglVertexAttribI3ui
#define glVertexAttribI4ui GTglVertexAttribI4ui
#define glVertexAttribI1iv GTglVertexAttribI1iv
#define glVertexAttribI2iv GTglVertexAttribI2iv
#define glVertexAttribI3iv GTglVertexAttribI3iv
#define glVertexAttribI4iv GTglVertexAttribI4iv
#define glVertexAttribI1uiv GTglVertexAttribI1uiv
#define glVertexAttribI2uiv GTglVertexAttribI2uiv
#define glVertexAttribI3uiv GTglVertexAttribI3uiv
#define glVertexAttribI4uiv GTglVertexAttribI4uiv
#define glVertexAttribI4bv GTglVertexAttribI4bv
#define glVertexAttribI4sv GTglVertexAttribI4sv
#define glVertexAttribI4ubv GTglVertexAttribI4ubv
#define glVertexAttribI4usv GTglVertexAttribI4usv
#define glGetUniformuiv GTglGetUniformuiv
#define glBindFragDataLocation GTglBindFragDataLocation
#define glGetFragDataLocation GTglGetFragDataLocation
#define glUniform1ui GTglUniform1ui
#define glUniform2ui GTglUniform2ui
#define glUniform3ui GTglUniform3ui
#define glUniform4ui GTglUniform4ui
#define glUniform1uiv GTglUniform1uiv
#define glUniform2uiv GTglUniform2uiv
#define glUniform3uiv GTglUniform3uiv
#define glUniform4uiv GTglUniform4uiv
#define glTexParameterIiv GTglTexParameterIiv
#define glTexParameterIuiv GTglTexParameterIuiv
#define glGetTexParameterIiv GTglGetTexParameterIiv
#define glGetTexParameterIuiv GTglGetTexParameterIuiv
#define glClearBufferiv GTglClearBufferiv
#define glClearBufferuiv GTglClearBufferuiv
#define glClearBufferfv GTglClearBufferfv
#define glClearBufferfi GTglClearBufferfi
#define glGetStringi GTglGetStringi

GT_EXTERN void GTglColorMaski (GLuint index, GLboolean r, GLboolean g,
    GLboolean b, GLboolean a);
GT_EXTERN void GTglGetBooleani_v (GLenum target, GLuint index,
    GLboolean* data);
GT_EXTERN void GTglGetIntegeri_v (GLenum target, GLuint index, GLint* data);
GT_EXTERN void GTglEnablei (GLenum target, GLuint index);
GT_EXTERN void GTglDisablei (GLenum target, GLuint index);
GT_EXTERN GLboolean GTglIsEnabledi (GLenum target, GLuint index);
GT_EXTERN void GTglBeginTransformFeedback (GLenum primitiveMode);
GT_EXTERN void GTglEndTransformFeedback (void);
GT_EXTERN void GTglBindBufferRange (GLenum target, GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size);
GT_EXTERN void GTglBindBufferBase (GLenum target, GLuint index,
    GLuint buffer);
GT_EXTERN void GTglTransformFeedbackVaryings (GLuint program, GLsizei count,
    const GLchar** varyings, GLenum bufferMode);
GT_EXTERN void GTglGetTransformFeedbackVarying (GLuint program, GLuint index,
    GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type,
    GLchar* name);
GT_EXTERN void GTglClampColor (GLenum target, GLenum clamp);
GT_EXTERN void GTglBeginConditionalRender (GLuint id, GLenum mode);
GT_EXTERN void GTglEndConditionalRender (void);
GT_EXTERN void GTglVertexAttribIPointer (GLuint index, GLint size,
    GLenum type, GLsizei stride, const GLvoid* pointer);
GT_EXTERN void GTglGetVertexAttribIiv (GLuint index, GLenum pname,
    GLint* params);
GT_EXTERN void GTglGetVertexAttribIuiv (GLuint index, GLenum pname,
    GLuint* params);
GT_EXTERN void GTglVertexAttribI1i (GLuint index, GLint x);
GT_EXTERN void GTglVertexAttribI2i (GLuint index, GLint x, GLint y);
GT_EXTERN void GTglVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z);
GT_EXTERN void GTglVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z,
    GLint w);
GT_EXTERN void GTglVertexAttribI1ui (GLuint index, GLuint x);
GT_EXTERN void GTglVertexAttribI2ui (GLuint index, GLuint x, GLuint y);
GT_EXTERN void GTglVertexAttribI3ui (GLuint index, GLuint x, GLuint y,
    GLuint z);
GT_EXTERN void GTglVertexAttribI4ui (GLuint index, GLuint x, GLuint y,
    GLuint z, GLuint w);
GT_EXTERN void GTglVertexAttribI1iv (GLuint index, const GLint* v);
GT_EXTERN void GTglVertexAttribI2iv (GLuint index, const GLint* v);
GT_EXTERN void GTglVertexAttribI3iv (GLuint index, const GLint* v);
GT_EXTERN void GTglVertexAttribI4iv (GLuint index, const GLint* v);
GT_EXTERN void GTglVertexAttribI1uiv (GLuint index, const GLuint* v);
GT_EXTERN void GTglVertexAttribI2uiv (GLuint index, const GLuint* v);
GT_EXTERN void GTglVertexAttribI3uiv (GLuint index, const GLuint* v);
GT_EXTERN void GTglVertexAttribI4uiv (GLuint index, const GLuint* v);
GT_EXTERN void GTglVertexAttribI4bv (GLuint index, const GLbyte* v);
GT_EXTERN void GTglVertexAttribI4sv (GLuint index, const GLshort* v);
GT_EXTERN void GTglVertexAttribI4ubv (GLuint index, const GLubyte* v);
GT_EXTERN void GTglVertexAttribI4usv (GLuint index, const GLushort* v);
GT_EXTERN void GTglGetUniformuiv (GLuint program, GLint location,
    GLuint* params);
GT_EXTERN void GTglBindFragDataLocation (GLuint program, GLuint color,
    const GLchar* name);
GT_EXTERN GLint GTglGetFragDataLocation (GLuint program, const GLchar* name);
GT_EXTERN void GTglUniform1ui (GLint location, GLuint v0);
GT_EXTERN void GTglUniform2ui (GLint location, GLuint v0, GLuint v1);
GT_EXTERN void GTglUniform3ui (GLint location, GLuint v0, GLuint v1,
    GLuint v2);
GT_EXTERN void GTglUniform4ui (GLint location, GLuint v0, GLuint v1,
    GLuint v2, GLuint v3);
GT_EXTERN void GTglUniform1uiv (GLint location, GLsizei count,
    const GLuint* value);
GT_EXTERN void GTglUniform2uiv (GLint location, GLsizei count,
    const GLuint* value);
GT_EXTERN void GTglUniform3uiv (GLint location, GLsizei count,
    const GLuint* value);
GT_EXTERN void GTglUniform4uiv (GLint location, GLsizei count,
    const GLuint* value);
GT_EXTERN void GTglTexParameterIiv (GLenum target, GLenum pname,
    const GLint* params);
GT_EXTERN void GTglTexParameterIuiv (GLenum target, GLenum pname,
    const GLuint* params);
GT_EXTERN void GTglGetTexParameterIiv (GLenum target, GLenum pname,
    GLint* params);
GT_EXTERN void GTglGetTexParameterIuiv (GLenum target, GLenum pname,
    GLuint* params);
GT_EXTERN void GTglClearBufferiv (GLenum buffer, GLint drawbuffer,
    const GLint* value);
GT_EXTERN void GTglClearBufferuiv (GLenum buffer, GLint drawbuffer,
    const GLuint* value);
GT_EXTERN void GTglClearBufferfv (GLenum buffer, GLint drawbuffer,
    const GLfloat* value);
GT_EXTERN void GTglClearBufferfi (GLenum buffer, GLint drawbuffer,
    GLfloat depth, GLint stencil);
GT_EXTERN const GLubyte* GTglGetStringi (GLenum name, GLuint index);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.1
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL31;

#define GL_SAMPLER_2D_RECT                                          0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW                                   0x8B64
#define GL_SAMPLER_BUFFER                                           0x8DC2
#define GL_INT_SAMPLER_2D_RECT                                      0x8DCD
#define GL_INT_SAMPLER_BUFFER                                       0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT                             0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER                              0x8DD8
#define GL_TEXTURE_BUFFER                                           0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE                                  0x8C2B
#define GL_TEXTURE_BINDING_BUFFER                                   0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING                        0x8C2D
#define GL_TEXTURE_BUFFER_FORMAT                                    0x8C2E
#define GL_TEXTURE_RECTANGLE                                        0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE                                0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE                                  0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE                               0x84F8
#define GL_RED_SNORM                                                0x8F90
#define GL_RG_SNORM                                                 0x8F91
#define GL_RGB_SNORM                                                0x8F92
#define GL_RGBA_SNORM                                               0x8F93
#define GL_R8_SNORM                                                 0x8F94
#define GL_RG8_SNORM                                                0x8F95
#define GL_RGB8_SNORM                                               0x8F96
#define GL_RGBA8_SNORM                                              0x8F97
#define GL_R16_SNORM                                                0x8F98
#define GL_RG16_SNORM                                               0x8F99
#define GL_RGB16_SNORM                                              0x8F9A
#define GL_RGBA16_SNORM                                             0x8F9B
#define GL_SIGNED_NORMALIZED                                        0x8F9C
#define GL_PRIMITIVE_RESTART                                        0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX                                  0x8F9E

#define glDrawArraysInstanced GTglDrawArraysInstanced
#define glDrawElementsInstanced GTglDrawElementsInstanced
#define glTexBuffer GTglTexBuffer
#define glPrimitiveRestartIndex GTglPrimitiveRestartIndex

GT_EXTERN void GTglDrawArraysInstanced (GLenum mode, GLint first,
    GLsizei count, GLsizei primcount);
GT_EXTERN void GTglDrawElementsInstanced (GLenum mode, GLsizei count,
    GLenum type, const GLvoid* indices, GLsizei primcount);
GT_EXTERN void GTglTexBuffer (GLenum target, GLenum internalformat,
    GLuint buffer);
GT_EXTERN void GTglPrimitiveRestartIndex (GLuint index);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.2
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL32;

#define GL_CONTEXT_CORE_PROFILE_BIT                                 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT                        0x00000002
#define GL_LINES_ADJACENCY                                          0x000A
#define GL_LINE_STRIP_ADJACENCY                                     0x000B
#define GL_TRIANGLES_ADJACENCY                                      0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY                                 0x000D
#define GL_PROGRAM_POINT_SIZE                                       0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS                         0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED                           0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS                     0x8DA8
#define GL_GEOMETRY_SHADER                                          0x8DD9
#define GL_GEOMETRY_VERTICES_OUT                                    0x8916
#define GL_GEOMETRY_INPUT_TYPE                                      0x8917
#define GL_GEOMETRY_OUTPUT_TYPE                                     0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS                          0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES                             0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS                     0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS                             0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS                            0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS                           0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS                            0x9125
#define GL_CONTEXT_PROFILE_MASK                                     0x9126

/* Logic is from Em1SourceLIB.h */
#if defined(_WIN32) || defined(WIN32)
typedef __int64             int64_t;
typedef unsigned __int64    uint64_t;
#endif
#if defined(__LINUX__)
#include <inttypes.h>
#endif

typedef int64_t GLint64;
typedef uint64_t GLuint64;

#define glGetInteger64i_v GTglGetInteger64i_v
#define glGetBufferParameteri64v GTglGetBufferParameteri64v
#define glProgramParameteri GTglProgramParameteri
#define glFramebufferTexture GTglFramebufferTexture

GT_EXTERN void GTglGetInteger64i_v (GLenum target, GLuint index,
    GLint64* data);
GT_EXTERN void GTglGetBufferParameteri64v (GLenum target, GLenum pname,
    GLint64* params);
GT_EXTERN void GTglProgramParameteri (GLuint program, GLenum pname,
    GLint value);
GT_EXTERN void GTglFramebufferTexture (GLenum target, GLenum attachment,
    GLuint texture, GLint level);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.3
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL33;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 4.0
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsOpenGL40;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** ARB Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 01) GL_ARB_multitexture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbMultitexture;

#define GL_TEXTURE0_ARB                                             0x84C0
#define GL_TEXTURE1_ARB                                             0x84C1
#define GL_TEXTURE2_ARB                                             0x84C2
#define GL_TEXTURE3_ARB                                             0x84C3
#define GL_TEXTURE4_ARB                                             0x84C4
#define GL_TEXTURE5_ARB                                             0x84C5
#define GL_TEXTURE6_ARB                                             0x84C6
#define GL_TEXTURE7_ARB                                             0x84C7
#define GL_TEXTURE8_ARB                                             0x84C8
#define GL_TEXTURE9_ARB                                             0x84C9
#define GL_TEXTURE10_ARB                                            0x84CA
#define GL_TEXTURE11_ARB                                            0x84CB
#define GL_TEXTURE12_ARB                                            0x84CC
#define GL_TEXTURE13_ARB                                            0x84CD
#define GL_TEXTURE14_ARB                                            0x84CE
#define GL_TEXTURE15_ARB                                            0x84CF
#define GL_TEXTURE16_ARB                                            0x84D0
#define GL_TEXTURE17_ARB                                            0x84D1
#define GL_TEXTURE18_ARB                                            0x84D2
#define GL_TEXTURE19_ARB                                            0x84D3
#define GL_TEXTURE20_ARB                                            0x84D4
#define GL_TEXTURE21_ARB                                            0x84D5
#define GL_TEXTURE22_ARB                                            0x84D6
#define GL_TEXTURE23_ARB                                            0x84D7
#define GL_TEXTURE24_ARB                                            0x84D8
#define GL_TEXTURE25_ARB                                            0x84D9
#define GL_TEXTURE26_ARB                                            0x84DA
#define GL_TEXTURE27_ARB                                            0x84DB
#define GL_TEXTURE28_ARB                                            0x84DC
#define GL_TEXTURE29_ARB                                            0x84DD
#define GL_TEXTURE30_ARB                                            0x84DE
#define GL_TEXTURE31_ARB                                            0x84DF
#define GL_ACTIVE_TEXTURE_ARB                                       0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB                                0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB                                    0x84E2

#define glActiveTextureARB GTglActiveTextureARB
#define glClientActiveTextureARB GTglClientActiveTextureARB
#define glMultiTexCoord1dARB GTglMultiTexCoord1dARB
#define glMultiTexCoord1dvARB GTglMultiTexCoord1dvARB
#define glMultiTexCoord1fARB GTglMultiTexCoord1fARB
#define glMultiTexCoord1fvARB GTglMultiTexCoord1fvARB
#define glMultiTexCoord1iARB GTglMultiTexCoord1iARB
#define glMultiTexCoord1ivARB GTglMultiTexCoord1ivARB
#define glMultiTexCoord1sARB GTglMultiTexCoord1sARB
#define glMultiTexCoord1svARB GTglMultiTexCoord1svARB
#define glMultiTexCoord2dARB GTglMultiTexCoord2dARB
#define glMultiTexCoord2dvARB GTglMultiTexCoord2dvARB
#define glMultiTexCoord2fARB GTglMultiTexCoord2fARB
#define glMultiTexCoord2fvARB GTglMultiTexCoord2fvARB
#define glMultiTexCoord2iARB GTglMultiTexCoord2iARB
#define glMultiTexCoord2ivARB GTglMultiTexCoord2ivARB
#define glMultiTexCoord2sARB GTglMultiTexCoord2sARB
#define glMultiTexCoord2svARB GTglMultiTexCoord2svARB
#define glMultiTexCoord3dARB GTglMultiTexCoord3dARB
#define glMultiTexCoord3dvARB GTglMultiTexCoord3dvARB
#define glMultiTexCoord3fARB GTglMultiTexCoord3fARB
#define glMultiTexCoord3fvARB GTglMultiTexCoord3fvARB
#define glMultiTexCoord3iARB GTglMultiTexCoord3iARB
#define glMultiTexCoord3ivARB GTglMultiTexCoord3ivARB
#define glMultiTexCoord3sARB GTglMultiTexCoord3sARB
#define glMultiTexCoord3svARB GTglMultiTexCoord3svARB
#define glMultiTexCoord4dARB GTglMultiTexCoord4dARB
#define glMultiTexCoord4dvARB GTglMultiTexCoord4dvARB
#define glMultiTexCoord4fARB GTglMultiTexCoord4fARB
#define glMultiTexCoord4fvARB GTglMultiTexCoord4fvARB
#define glMultiTexCoord4iARB GTglMultiTexCoord4iARB
#define glMultiTexCoord4ivARB GTglMultiTexCoord4ivARB
#define glMultiTexCoord4sARB GTglMultiTexCoord4sARB
#define glMultiTexCoord4svARB GTglMultiTexCoord4svARB

GT_EXTERN void GTglActiveTextureARB (GLenum eTexture);
GT_EXTERN void GTglClientActiveTextureARB (GLenum eTexture);
GT_EXTERN void GTglMultiTexCoord1dARB (GLenum eTarget, GLdouble dS);
GT_EXTERN void GTglMultiTexCoord1dvARB (GLenum eTarget, const GLdouble* pdS);
GT_EXTERN void GTglMultiTexCoord1fARB (GLenum eTarget, GLfloat fS);
GT_EXTERN void GTglMultiTexCoord1fvARB (GLenum eTarget, const GLfloat* pfS);
GT_EXTERN void GTglMultiTexCoord1iARB (GLenum eTarget, GLint iS);
GT_EXTERN void GTglMultiTexCoord1ivARB (GLenum eTarget, const GLint* piS);
GT_EXTERN void GTglMultiTexCoord1sARB (GLenum eTarget, GLshort sS);
GT_EXTERN void GTglMultiTexCoord1svARB (GLenum eTarget, const GLshort* psS);
GT_EXTERN void GTglMultiTexCoord2dARB (GLenum eTarget, GLdouble dS,
    GLdouble dT);
GT_EXTERN void GTglMultiTexCoord2dvARB (GLenum eTarget, const GLdouble* asDT);
GT_EXTERN void GTglMultiTexCoord2fARB (GLenum eTarget, GLfloat fS,
    GLfloat fT);
GT_EXTERN void GTglMultiTexCoord2fvARB (GLenum eTarget, const GLfloat* afST);
GT_EXTERN void GTglMultiTexCoord2iARB (GLenum eTarget, GLint iS, GLint iT);
GT_EXTERN void GTglMultiTexCoord2ivARB (GLenum eTarget, const GLint* aiST);
GT_EXTERN void GTglMultiTexCoord2sARB (GLenum eTarget, GLshort sS,
    GLshort sT);
GT_EXTERN void GTglMultiTexCoord2svARB (GLenum eTarget, const GLshort* asST);
GT_EXTERN void GTglMultiTexCoord3dARB (GLenum eTarget, GLdouble dS,
    GLdouble dT, GLdouble dR);
GT_EXTERN void GTglMultiTexCoord3dvARB (GLenum eTarget,
    const GLdouble* adSTR);
GT_EXTERN void GTglMultiTexCoord3fARB (GLenum eTarget, GLfloat fS, GLfloat fT,
    GLfloat fR);
GT_EXTERN void GTglMultiTexCoord3fvARB (GLenum eTarget, const GLfloat* afSTR);
GT_EXTERN void GTglMultiTexCoord3iARB (GLenum eTarget, GLint iS, GLint iT,
    GLint iR);
GT_EXTERN void GTglMultiTexCoord3ivARB (GLenum eTarget, const GLint* aiSTR);
GT_EXTERN void GTglMultiTexCoord3sARB (GLenum eTarget, GLshort sS, GLshort sT,
    GLshort sR);
GT_EXTERN void GTglMultiTexCoord3svARB (GLenum eTarget, const GLshort* asSTR);
GT_EXTERN void GTglMultiTexCoord4dARB (GLenum eTarget, GLdouble dS,
    GLdouble dT, GLdouble dR, GLdouble dQ);
GT_EXTERN void GTglMultiTexCoord4dvARB (GLenum eTarget,
    const GLdouble* asSTRQ);
GT_EXTERN void GTglMultiTexCoord4fARB (GLenum eTarget, GLfloat fS, GLfloat fT,
    GLfloat fR,GLfloat fQ);
GT_EXTERN void GTglMultiTexCoord4fvARB (GLenum eTarget,
    const GLfloat* afSTRQ);
GT_EXTERN void GTglMultiTexCoord4iARB (GLenum eTarget, GLint iS, GLint iT,
    GLint iR, GLint iQ);
GT_EXTERN void GTglMultiTexCoord4ivARB (GLenum eTarget, const GLint* aiSTRQ);
GT_EXTERN void GTglMultiTexCoord4sARB (GLenum eTarget, GLshort sS, GLshort sT,
    GLshort sR, GLshort sQ);
GT_EXTERN void GTglMultiTexCoord4svARB (GLenum eTarget,
    const GLshort* asSTRQ);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 03) GL_ARB_transpose_matrix
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTransposeMatrix;

#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB                           0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB                          0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB                             0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX_ARB                               0x84E6

#define glLoadTransposeMatrixdARB GTglLoadTransposeMatrixdARB
#define glLoadTransposeMatrixfARB GTglLoadTransposeMatrixfARB
#define glMultTransposeMatrixdARB GTglMultTransposeMatrixdARB
#define glMultTransposeMatrixfARB GTglMultTransposeMatrixfARB

GT_EXTERN void GTglLoadTransposeMatrixdARB (const GLdouble* adMatrix);
GT_EXTERN void GTglLoadTransposeMatrixfARB (const GLfloat* afMatrix);
GT_EXTERN void GTglMultTransposeMatrixdARB (const GLdouble* adMatrix);
GT_EXTERN void GTglMultTransposeMatrixfARB (const GLfloat* afMatrix);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 05) GL_ARB_multisample
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbMultisample;

#define GL_MULTISAMPLE_ARB                                          0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB                             0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_ARB                                  0x809F
#define GL_SAMPLE_COVERAGE_ARB                                      0x80A0
#define GL_SAMPLE_BUFFERS_ARB                                       0x80A8
#define GL_SAMPLES_ARB                                              0x80A9
#define GL_SAMPLE_COVERAGE_VALUE_ARB                                0x80AA
#define GL_SAMPLE_COVERAGE_INVERT_ARB                               0x80AB
#define GL_MULTISAMPLE_BIT_ARB                                      0x20000000

#define glSampleCoverageARB GTglSampleCoverageARB

GT_EXTERN void GTglSampleCoverageARB (GLclampf fValue, GLboolean ucInvert);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 06) GL_ARB_texture_env_add
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureEnvAdd;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 07) GL_ARB_texture_cube_map
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureCubeMap;

#define GL_NORMAL_MAP_ARB                                           0x8511
#define GL_REFLECTION_MAP_ARB                                       0x8512
#define GL_TEXTURE_CUBE_MAP_ARB                                     0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB                             0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB                          0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB                          0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB                          0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB                          0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB                          0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB                          0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB                               0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB                            0x851C
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 12) GL_ARB_texture_compression
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureCompression;

#define GL_COMPRESSED_ALPHA_ARB                                     0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB                                 0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB                           0x84EB
#define GL_COMPRESSED_INTENSITY_ARB                                 0x84EC
#define GL_COMPRESSED_RGB_ARB                                       0x84ED
#define GL_COMPRESSED_RGBA_ARB                                      0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB                             0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB                        0x86A0
#define GL_TEXTURE_COMPRESSED_ARB                                   0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB                       0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB                           0x86A3

#define glCompressedTexImage1DARB GTglCompressedTexImage1DARB
#define glCompressedTexImage2DARB GTglCompressedTexImage2DARB
#define glCompressedTexImage3DARB GTglCompressedTexImage3DARB
#define glCompressedTexSubImage1DARB GTglCompressedTexSubImage1DARB
#define glCompressedTexSubImage2DARB GTglCompressedTexSubImage2DARB
#define glCompressedTexSubImage3DARB GTglCompressedTexSubImage3DARB
#define glGetCompressedTexImageARB GTglGetCompressedTexImageARB

GT_EXTERN void GTglCompressedTexImage1DARB (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLint iBorder,
    GLsizei iImageSize, const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexImage2DARB (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLint iBorder,
    GLsizei iImageSize, const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexImage3DARB (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLsizei iDepth,
    GLint iBorder, GLsizei iImageSize, const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexSubImage1DARB (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLsizei iWidth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexSubImage2DARB (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLsizei iImageSize, const GLvoid* pvData);
GT_EXTERN void GTglCompressedTexSubImage3DARB (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLsizei iWidth,
    GLsizei iHeight, GLsizei iDepth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData);
GT_EXTERN void GTglGetCompressedTexImageARB (GLenum eTarget, GLint iLevel,
    GLvoid* pvData);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 13) GL_ARB_texture_border_clamp
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureBorderClamp;

#define GL_CLAMP_TO_BORDER_ARB                                      0x812D
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 14) GL_ARB_point_parameters
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbPointParameters;

#define GL_POINT_SIZE_MIN_ARB                                       0x8126
#define GL_POINT_SIZE_MAX_ARB                                       0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_ARB                            0x8128
#define GL_POINT_DISTANCE_ATTENUATION_ARB                           0x8129

#define glPointParameterfARB GTglPointParameterfARB
#define glPointParameterfvARB GTglPointParameterfvARB

GT_EXTERN void GTglPointParameterfARB (GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglPointParameterfvARB (GLenum ePName,
    const GLfloat* afParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 15) GL_ARB_vertex_blend
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbVertexBlend;

#define GL_MAX_VERTEX_UNITS_ARB                                     0x86A4
#define GL_ACTIVE_VERTEX_UNITS_ARB                                  0x86A5
#define GL_WEIGHT_SUM_UNITY_ARB                                     0x86A6
#define GL_VERTEX_BLEND_ARB                                         0x86A7
#define GL_CURRENT_WEIGHT_ARB                                       0x86A8
#define GL_WEIGHT_ARRAY_TYPE_ARB                                    0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_ARB                                  0x86AA
#define GL_WEIGHT_ARRAY_SIZE_ARB                                    0x86AB
#define GL_WEIGHT_ARRAY_POINTER_ARB                                 0x86AC
#define GL_WEIGHT_ARRAY_ARB                                         0x86AD
#define GL_MODELVIEW0_ARB                                           0x1700
#define GL_MODELVIEW1_ARB                                           0x850A
#define GL_MODELVIEW2_ARB                                           0x8722
#define GL_MODELVIEW3_ARB                                           0x8723
#define GL_MODELVIEW4_ARB                                           0x8724
#define GL_MODELVIEW5_ARB                                           0x8725
#define GL_MODELVIEW6_ARB                                           0x8726
#define GL_MODELVIEW7_ARB                                           0x8727
#define GL_MODELVIEW8_ARB                                           0x8728
#define GL_MODELVIEW9_ARB                                           0x8729
#define GL_MODELVIEW10_ARB                                          0x872A
#define GL_MODELVIEW11_ARB                                          0x872B
#define GL_MODELVIEW12_ARB                                          0x872C
#define GL_MODELVIEW13_ARB                                          0x872D
#define GL_MODELVIEW14_ARB                                          0x872E
#define GL_MODELVIEW15_ARB                                          0x872F
#define GL_MODELVIEW16_ARB                                          0x8730
#define GL_MODELVIEW17_ARB                                          0x8731
#define GL_MODELVIEW18_ARB                                          0x8732
#define GL_MODELVIEW19_ARB                                          0x8733
#define GL_MODELVIEW20_ARB                                          0x8734
#define GL_MODELVIEW21_ARB                                          0x8735
#define GL_MODELVIEW22_ARB                                          0x8736
#define GL_MODELVIEW23_ARB                                          0x8737
#define GL_MODELVIEW24_ARB                                          0x8738
#define GL_MODELVIEW25_ARB                                          0x8739
#define GL_MODELVIEW26_ARB                                          0x873A
#define GL_MODELVIEW27_ARB                                          0x873B
#define GL_MODELVIEW28_ARB                                          0x873C
#define GL_MODELVIEW29_ARB                                          0x873D
#define GL_MODELVIEW30_ARB                                          0x873E
#define GL_MODELVIEW31_ARB                                          0x873F

#define glWeightbvARB GTglWeightbvARB
#define glWeightsvARB GTglWeightsvARB
#define glWeightivARB GTglWeightivARB
#define glWeightfvARB GTglWeightfvARB
#define glWeightdvARB GTglWeightdvARB
#define glWeightubvARB GTglWeightubvARB
#define glWeightusvARB GTglWeightusvARB
#define glWeightuivARB GTglWeightuivARB
#define glWeightPointerARB GTglWeightPointerARB
#define glVertexBlendARB GTglVertexBlendARB

GT_EXTERN void GTglWeightbvARB (GLint iSize, const GLbyte* acWeight);
GT_EXTERN void GTglWeightsvARB (GLint iSize, const GLshort* asWeight);
GT_EXTERN void GTglWeightivARB (GLint iSize, const GLint* aiWeight);
GT_EXTERN void GTglWeightfvARB (GLint iSize, const GLfloat* afWeight);
GT_EXTERN void GTglWeightdvARB (GLint iSize, const GLdouble* adWeight);
GT_EXTERN void GTglWeightubvARB (GLint iSize, const GLubyte* aucWeight);
GT_EXTERN void GTglWeightusvARB (GLint iSize, const GLushort* ausWeight);
GT_EXTERN void GTglWeightuivARB (GLint iSize, const GLuint* auiWeight);
GT_EXTERN void GTglWeightPointerARB (GLint iSize, GLenum eType,
    GLsizei iStride, const GLvoid* pvPointer);
GT_EXTERN void GTglVertexBlendARB (GLint iCount);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 16) GL_ARB_matrix_palette
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbMatrixPalette;

#define GL_MATRIX_PALETTE_ARB                                       0x8840
#define GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB                       0x8841
#define GL_MAX_PALETTE_MATRICES_ARB                                 0x8842
#define GL_CURRENT_PALETTE_MATRIX_ARB                               0x8843
#define GL_MATRIX_INDEX_ARRAY_ARB                                   0x8844
#define GL_CURRENT_MATRIX_INDEX_ARB                                 0x8845
#define GL_MATRIX_INDEX_ARRAY_SIZE_ARB                              0x8846
#define GL_MATRIX_INDEX_ARRAY_TYPE_ARB                              0x8847
#define GL_MATRIX_INDEX_ARRAY_STRIDE_ARB                            0x8848
#define GL_MATRIX_INDEX_ARRAY_POINTER_ARB                           0x8849

#define glCurrentPaletteMatrixARB GTglCurrentPaletteMatrixARB
#define glMatrixIndexubvARB GTglMatrixIndexubvARB
#define glMatrixIndexusvARB GTglMatrixIndexusvARB
#define glMatrixIndexuivARB GTglMatrixIndexuivARB
#define glMatrixIndexPointerARB GTglMatrixIndexPointerARB

GT_EXTERN void GTglCurrentPaletteMatrixARB (GLint iIndex);
GT_EXTERN void GTglMatrixIndexubvARB (GLint iSize, const GLubyte* aucIndex);
GT_EXTERN void GTglMatrixIndexusvARB (GLint iSize, const GLushort* ausIndex);
GT_EXTERN void GTglMatrixIndexuivARB (GLint iSize, const GLuint* auiIndex);
GT_EXTERN void GTglMatrixIndexPointerARB (GLint iSize, GLenum eType,
    GLsizei iStride, const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 17) GL_ARB_texture_env_combine
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureEnvCombine;

#define GL_SUBTRACT_ARB                                             0x84E7
#define GL_COMBINE_ARB                                              0x8570
#define GL_COMBINE_RGB_ARB                                          0x8571
#define GL_COMBINE_ALPHA_ARB                                        0x8572
#define GL_RGB_SCALE_ARB                                            0x8573
#define GL_ADD_SIGNED_ARB                                           0x8574
#define GL_INTERPOLATE_ARB                                          0x8575
#define GL_CONSTANT_ARB                                             0x8576
#define GL_PRIMARY_COLOR_ARB                                        0x8577
#define GL_PREVIOUS_ARB                                             0x8578
#define GL_SOURCE0_RGB_ARB                                          0x8580
#define GL_SOURCE1_RGB_ARB                                          0x8581
#define GL_SOURCE2_RGB_ARB                                          0x8582
#define GL_SOURCE0_ALPHA_ARB                                        0x8588
#define GL_SOURCE1_ALPHA_ARB                                        0x8589
#define GL_SOURCE2_ALPHA_ARB                                        0x858A
#define GL_OPERAND0_RGB_ARB                                         0x8590
#define GL_OPERAND1_RGB_ARB                                         0x8591
#define GL_OPERAND2_RGB_ARB                                         0x8592
#define GL_OPERAND0_ALPHA_ARB                                       0x8598
#define GL_OPERAND1_ALPHA_ARB                                       0x8599
#define GL_OPERAND2_ALPHA_ARB                                       0x859A
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 18) GL_ARB_texture_env_crossbar
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureEnvCrossbar;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 19) GL_ARB_texture_env_dot3
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureEnvDot3;

#define GL_DOT3_RGB_ARB                                             0x86AE
#define GL_DOT3_RGBA_ARB                                            0x86AF
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 21) GL_ARB_texture_mirrored_repeat
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureMirroredRepeat;

#define GL_MIRRORED_REPEAT_ARB                                      0x8370
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 22) GL_ARB_depth_texture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbDepthTexture;

#define GL_DEPTH_COMPONENT16_ARB                                    0x81A5
#define GL_DEPTH_COMPONENT24_ARB                                    0x81A6
#define GL_DEPTH_COMPONENT32_ARB                                    0x81A7
#define GL_TEXTURE_DEPTH_SIZE_ARB                                   0x884A
#define GL_DEPTH_TEXTURE_MODE_ARB                                   0x884B
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 23) GL_ARB_shadow
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbShadow;

#define GL_TEXTURE_COMPARE_MODE_ARB                                 0x884C
#define GL_TEXTURE_COMPARE_FUNC_ARB                                 0x884D
#define GL_COMPARE_R_TO_TEXTURE_ARB                                 0x884E
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 24) GL_ARB_shadow_ambient
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbShadowAmbient;

#define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB                           0x80BF
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 25) GL_ARB_window_pos
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbWindowPos;

#define glWindowPos2dARB GTglWindowPos2dARB
#define glWindowPos2dvARB GTglWindowPos2dvARB
#define glWindowPos2fARB GTglWindowPos2fARB
#define glWindowPos2fvARB GTglWindowPos2fvARB
#define glWindowPos2iARB GTglWindowPos2iARB
#define glWindowPos2ivARB GTglWindowPos2ivARB
#define glWindowPos2sARB GTglWindowPos2sARB
#define glWindowPos2svARB GTglWindowPos2svARB
#define glWindowPos3dARB GTglWindowPos3dARB
#define glWindowPos3dvARB GTglWindowPos3dvARB
#define glWindowPos3fARB GTglWindowPos3fARB
#define glWindowPos3fvARB GTglWindowPos3fvARB
#define glWindowPos3iARB GTglWindowPos3iARB
#define glWindowPos3ivARB GTglWindowPos3ivARB
#define glWindowPos3sARB GTglWindowPos3sARB
#define glWindowPos3svARB GTglWindowPos3svARB

GT_EXTERN void GTglWindowPos2dARB (GLdouble dX, GLdouble dY);
GT_EXTERN void GTglWindowPos2dvARB (const GLdouble* adXY);
GT_EXTERN void GTglWindowPos2fARB (GLfloat fX, GLfloat fY);
GT_EXTERN void GTglWindowPos2fvARB (const GLfloat* afXY);
GT_EXTERN void GTglWindowPos2iARB (GLint iX, GLint iY);
GT_EXTERN void GTglWindowPos2ivARB (const GLint* aiXY);
GT_EXTERN void GTglWindowPos2sARB (GLshort sX, GLshort sY);
GT_EXTERN void GTglWindowPos2svARB (const GLshort* asXY);
GT_EXTERN void GTglWindowPos3dARB (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglWindowPos3dvARB (const GLdouble* adXYZ);
GT_EXTERN void GTglWindowPos3fARB (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglWindowPos3fvARB (const GLfloat* afXYZ);
GT_EXTERN void GTglWindowPos3iARB (GLint iX, GLint iY, GLint iZ);
GT_EXTERN void GTglWindowPos3ivARB (const GLint* aiXYZ);
GT_EXTERN void GTglWindowPos3sARB (GLshort sX, GLshort sY, GLshort sZ);
GT_EXTERN void GTglWindowPos3svARB (const GLshort* asXYZ);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 26) GL_ARB_vertex_program
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbVertexProgram;

#define GL_COLOR_SUM_ARB                                            0x8458
#define GL_VERTEX_PROGRAM_ARB                                       0x8620
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB                          0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB                             0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB                           0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB                             0x8625
#define GL_CURRENT_VERTEX_ATTRIB_ARB                                0x8626
#define GL_PROGRAM_LENGTH_ARB                                       0x8627
#define GL_PROGRAM_STRING_ARB                                       0x8628
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB                       0x862E
#define GL_MAX_PROGRAM_MATRICES_ARB                                 0x862F
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB                           0x8640
#define GL_CURRENT_MATRIX_ARB                                       0x8641
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB                            0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB                              0x8643
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB                          0x8645
#define GL_PROGRAM_ERROR_POSITION_ARB                               0x864B
#define GL_PROGRAM_BINDING_ARB                                      0x8677
#define GL_MAX_VERTEX_ATTRIBS_ARB                                   0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB                       0x886A
#define GL_PROGRAM_ERROR_STRING_ARB                                 0x8874
#define GL_PROGRAM_FORMAT_ASCII_ARB                                 0x8875
#define GL_PROGRAM_FORMAT_ARB                                       0x8876
#define GL_PROGRAM_INSTRUCTIONS_ARB                                 0x88A0
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB                             0x88A1
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB                          0x88A2
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB                      0x88A3
#define GL_PROGRAM_TEMPORARIES_ARB                                  0x88A4
#define GL_MAX_PROGRAM_TEMPORARIES_ARB                              0x88A5
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB                           0x88A6
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB                       0x88A7
#define GL_PROGRAM_PARAMETERS_ARB                                   0x88A8
#define GL_MAX_PROGRAM_PARAMETERS_ARB                               0x88A9
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB                            0x88AA
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB                        0x88AB
#define GL_PROGRAM_ATTRIBS_ARB                                      0x88AC
#define GL_MAX_PROGRAM_ATTRIBS_ARB                                  0x88AD
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB                               0x88AE
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB                           0x88AF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB                            0x88B0
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB                        0x88B1
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB                     0x88B2
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB                 0x88B3
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB                         0x88B4
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB                           0x88B5
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB                          0x88B6
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB                             0x88B7
#define GL_MATRIX0_ARB                                              0x88C0
#define GL_MATRIX1_ARB                                              0x88C1
#define GL_MATRIX2_ARB                                              0x88C2
#define GL_MATRIX3_ARB                                              0x88C3
#define GL_MATRIX4_ARB                                              0x88C4
#define GL_MATRIX5_ARB                                              0x88C5
#define GL_MATRIX6_ARB                                              0x88C6
#define GL_MATRIX7_ARB                                              0x88C7
#define GL_MATRIX8_ARB                                              0x88C8
#define GL_MATRIX9_ARB                                              0x88C9
#define GL_MATRIX10_ARB                                             0x88CA
#define GL_MATRIX11_ARB                                             0x88CB
#define GL_MATRIX12_ARB                                             0x88CC
#define GL_MATRIX13_ARB                                             0x88CD
#define GL_MATRIX14_ARB                                             0x88CE
#define GL_MATRIX15_ARB                                             0x88CF
#define GL_MATRIX16_ARB                                             0x88D0
#define GL_MATRIX17_ARB                                             0x88D1
#define GL_MATRIX18_ARB                                             0x88D2
#define GL_MATRIX19_ARB                                             0x88D3
#define GL_MATRIX20_ARB                                             0x88D4
#define GL_MATRIX21_ARB                                             0x88D5
#define GL_MATRIX22_ARB                                             0x88D6
#define GL_MATRIX23_ARB                                             0x88D7
#define GL_MATRIX24_ARB                                             0x88D8
#define GL_MATRIX25_ARB                                             0x88D9
#define GL_MATRIX26_ARB                                             0x88DA
#define GL_MATRIX27_ARB                                             0x88DB
#define GL_MATRIX28_ARB                                             0x88DC
#define GL_MATRIX29_ARB                                             0x88DD
#define GL_MATRIX30_ARB                                             0x88DE
#define GL_MATRIX31_ARB                                             0x88DF

#define glBindProgramARB GTglBindProgramARB
#define glDeleteProgramsARB GTglDeleteProgramsARB
#define glDisableVertexAttribArrayARB GTglDisableVertexAttribArrayARB
#define glEnableVertexAttribArrayARB GTglEnableVertexAttribArrayARB
#define glGenProgramsARB GTglGenProgramsARB
#define glGetProgramivARB GTglGetProgramivARB
#define glGetProgramEnvParameterdvARB GTglGetProgramEnvParameterdvARB
#define glGetProgramEnvParameterfvARB GTglGetProgramEnvParameterfvARB
#define glGetProgramLocalParameterdvARB GTglGetProgramLocalParameterdvARB
#define glGetProgramLocalParameterfvARB GTglGetProgramLocalParameterfvARB
#define glGetProgramStringARB GTglGetProgramStringARB
#define glGetVertexAttribdvARB GTglGetVertexAttribdvARB
#define glGetVertexAttribfvARB GTglGetVertexAttribfvARB
#define glGetVertexAttribivARB GTglGetVertexAttribivARB
#define glGetVertexAttribPointervARB GTglGetVertexAttribPointervARB
#define glIsProgramARB GTglIsProgramARB
#define glProgramEnvParameter4dARB GTglProgramEnvParameter4dARB
#define glProgramEnvParameter4dvARB GTglProgramEnvParameter4dvARB
#define glProgramEnvParameter4fARB GTglProgramEnvParameter4fARB
#define glProgramEnvParameter4fvARB GTglProgramEnvParameter4fvARB
#define glProgramLocalParameter4dARB GTglProgramLocalParameter4dARB
#define glProgramLocalParameter4dvARB GTglProgramLocalParameter4dvARB
#define glProgramLocalParameter4fARB GTglProgramLocalParameter4fARB
#define glProgramLocalParameter4fvARB GTglProgramLocalParameter4fvARB
#define glProgramStringARB GTglProgramStringARB
#define glVertexAttrib1dARB GTglVertexAttrib1dARB
#define glVertexAttrib1dvARB GTglVertexAttrib1dvARB
#define glVertexAttrib1fARB GTglVertexAttrib1fARB
#define glVertexAttrib1fvARB GTglVertexAttrib1fvARB
#define glVertexAttrib1sARB GTglVertexAttrib1sARB
#define glVertexAttrib1svARB GTglVertexAttrib1svARB
#define glVertexAttrib2dARB GTglVertexAttrib2dARB
#define glVertexAttrib2dvARB GTglVertexAttrib2dvARB
#define glVertexAttrib2fARB GTglVertexAttrib2fARB
#define glVertexAttrib2fvARB GTglVertexAttrib2fvARB
#define glVertexAttrib2sARB GTglVertexAttrib2sARB
#define glVertexAttrib2svARB GTglVertexAttrib2svARB
#define glVertexAttrib3dARB GTglVertexAttrib3dARB
#define glVertexAttrib3dvARB GTglVertexAttrib3dvARB
#define glVertexAttrib3fARB GTglVertexAttrib3fARB
#define glVertexAttrib3fvARB GTglVertexAttrib3fvARB
#define glVertexAttrib3sARB GTglVertexAttrib3sARB
#define glVertexAttrib3svARB GTglVertexAttrib3svARB
#define glVertexAttrib4NbvARB GTglVertexAttrib4NbvARB
#define glVertexAttrib4NivARB GTglVertexAttrib4NivARB
#define glVertexAttrib4NsvARB GTglVertexAttrib4NsvARB
#define glVertexAttrib4NubARB GTglVertexAttrib4NubARB
#define glVertexAttrib4NubvARB GTglVertexAttrib4NubvARB
#define glVertexAttrib4NuivARB GTglVertexAttrib4NuivARB
#define glVertexAttrib4NusvARB GTglVertexAttrib4NusvARB
#define glVertexAttrib4bvARB GTglVertexAttrib4bvARB
#define glVertexAttrib4dARB GTglVertexAttrib4dARB
#define glVertexAttrib4dvARB GTglVertexAttrib4dvARB
#define glVertexAttrib4fARB GTglVertexAttrib4fARB
#define glVertexAttrib4fvARB GTglVertexAttrib4fvARB
#define glVertexAttrib4ivARB GTglVertexAttrib4ivARB
#define glVertexAttrib4sARB GTglVertexAttrib4sARB
#define glVertexAttrib4svARB GTglVertexAttrib4svARB
#define glVertexAttrib4ubvARB GTglVertexAttrib4ubvARB
#define glVertexAttrib4uivARB GTglVertexAttrib4uivARB
#define glVertexAttrib4usvARB GTglVertexAttrib4usvARB
#define glVertexAttribPointerARB GTglVertexAttribPointerARB

GT_EXTERN void GTglBindProgramARB (GLenum eTarget, GLuint uiProgram);
GT_EXTERN void GTglDeleteProgramsARB (GLsizei iNumPrograms,
    const GLuint* auiPrograms);
GT_EXTERN void GTglDisableVertexAttribArrayARB (GLuint uiIndex);
GT_EXTERN void GTglEnableVertexAttribArrayARB (GLuint uiIndex);
GT_EXTERN void GTglGenProgramsARB (GLsizei iNumPrograms, GLuint* auiPrograms);
GT_EXTERN void GTglGetProgramivARB (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetProgramEnvParameterdvARB (GLenum eTarget,
    GLuint uiIndex, GLdouble* adParam);
GT_EXTERN void GTglGetProgramEnvParameterfvARB (GLenum eTarget,
    GLuint uiIndex, GLfloat* afParam);
GT_EXTERN void GTglGetProgramLocalParameterdvARB (GLenum eTarget,
    GLuint uiIndex, GLdouble* adParam);
GT_EXTERN void GTglGetProgramLocalParameterfvARB (GLenum eTarget,
    GLuint uiIndex, GLfloat* afParam);
GT_EXTERN void GTglGetProgramStringARB (GLenum eTarget, GLenum ePName,
    GLvoid* pvString);
GT_EXTERN void GTglGetVertexAttribdvARB (GLuint uiIndex, GLenum ePName,
    GLdouble* adParam);
GT_EXTERN void GTglGetVertexAttribfvARB (GLuint uiIndex, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetVertexAttribivARB (GLuint uiIndex, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetVertexAttribPointervARB (GLuint uiIndex, GLenum ePName,
    GLvoid** apvPointer);
GT_EXTERN GLboolean GTglIsProgramARB (GLuint uiProgram);
GT_EXTERN void GTglProgramEnvParameter4dARB (GLenum eTarget, GLuint uiIndex,
    GLdouble dX, GLdouble dY, GLdouble dZ, GLdouble dW);
GT_EXTERN void GTglProgramEnvParameter4dvARB (GLenum eTarget, GLuint uiIndex,
    const GLdouble* adXYZW);
GT_EXTERN void GTglProgramEnvParameter4fARB (GLenum eTarget, GLuint uiIndex,
    GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW);
GT_EXTERN void GTglProgramEnvParameter4fvARB (GLenum eTarget, GLuint uiIndex,
    const GLfloat* afXYZW);
GT_EXTERN void GTglProgramLocalParameter4dARB (GLenum eTarget, GLuint uiIndex,
    GLdouble dX, GLdouble dY, GLdouble dZ, GLdouble dW);
GT_EXTERN void GTglProgramLocalParameter4dvARB (GLenum eTarget,
    GLuint uiIndex, const GLdouble* adXYZW);
GT_EXTERN void GTglProgramLocalParameter4fARB (GLenum eTarget, GLuint uiIndex,
    GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW);
GT_EXTERN void GTglProgramLocalParameter4fvARB (GLenum eTarget,
    GLuint uiIndex, const GLfloat* afXYZW);
GT_EXTERN void GTglProgramStringARB (GLenum eTarget, GLenum eFormat,
    GLsizei iLength, const GLvoid* pvString);
GT_EXTERN void GTglVertexAttrib1dARB (GLuint uiIndex, GLdouble dX);
GT_EXTERN void GTglVertexAttrib1dvARB (GLuint uiIndex, const GLdouble* pdX);
GT_EXTERN void GTglVertexAttrib1fARB (GLuint uiIndex, GLfloat fX);
GT_EXTERN void GTglVertexAttrib1fvARB (GLuint uiIndex, const GLfloat* pfX);
GT_EXTERN void GTglVertexAttrib1sARB (GLuint uiIndex, GLshort sX);
GT_EXTERN void GTglVertexAttrib1svARB (GLuint uiIndex, const GLshort* psX);
GT_EXTERN void GTglVertexAttrib2dARB (GLuint uiIndex, GLdouble dX,
    GLdouble dY);
GT_EXTERN void GTglVertexAttrib2dvARB (GLuint uiIndex, const GLdouble* adXY);
GT_EXTERN void GTglVertexAttrib2fARB (GLuint uiIndex, GLfloat fX, GLfloat fY);
GT_EXTERN void GTglVertexAttrib2fvARB (GLuint uiIndex, const GLfloat* afXY);
GT_EXTERN void GTglVertexAttrib2sARB (GLuint uiIndex, GLshort sX, GLshort sY);
GT_EXTERN void GTglVertexAttrib2svARB (GLuint uiIndex, const GLshort* asXY);
GT_EXTERN void GTglVertexAttrib3dARB (GLuint uiIndex, GLdouble dX,
    GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglVertexAttrib3dvARB (GLuint uiIndex, const GLdouble* adXYZ);
GT_EXTERN void GTglVertexAttrib3fARB (GLuint uiIndex, GLfloat fX, GLfloat fY,
    GLfloat fZ);
GT_EXTERN void GTglVertexAttrib3fvARB (GLuint uiIndex, const GLfloat* afXYZ);
GT_EXTERN void GTglVertexAttrib3sARB (GLuint uiIndex, GLshort sX, GLshort sY,
    GLshort sZ);
GT_EXTERN void GTglVertexAttrib3svARB (GLuint uiIndex, const GLshort* asXYZ);
GT_EXTERN void GTglVertexAttrib4NbvARB (GLuint uiIndex, const GLbyte* acXYZW);
GT_EXTERN void GTglVertexAttrib4NivARB (GLuint uiIndex, const GLint* aiXYZW);
GT_EXTERN void GTglVertexAttrib4NsvARB (GLuint uiIndex,
    const GLshort* asXYZW);
GT_EXTERN void GTglVertexAttrib4NubARB (GLuint uiIndex, GLubyte ucX,
    GLubyte ucY, GLubyte ucZ, GLubyte ucW);
GT_EXTERN void GTglVertexAttrib4NubvARB (GLuint uiIndex,
    const GLubyte* aucXYZW);
GT_EXTERN void GTglVertexAttrib4NuivARB (GLuint uiIndex,
    const GLuint* auiXYZW);
GT_EXTERN void GTglVertexAttrib4NusvARB (GLuint uiIndex,
    const GLushort* ausXYZW);
GT_EXTERN void GTglVertexAttrib4bvARB (GLuint uiIndex, const GLbyte* acXYZW);
GT_EXTERN void GTglVertexAttrib4dARB (GLuint uiIndex, GLdouble dX,
    GLdouble dY, GLdouble dZ, GLdouble dW);
GT_EXTERN void GTglVertexAttrib4dvARB (GLuint uiIndex,
    const GLdouble* adXYZW);
GT_EXTERN void GTglVertexAttrib4fARB (GLuint uiIndex, GLfloat fX, GLfloat fY,
    GLfloat fZ, GLfloat fW);
GT_EXTERN void GTglVertexAttrib4fvARB (GLuint uiIndex, const GLfloat* afXYZW);
GT_EXTERN void GTglVertexAttrib4ivARB (GLuint uiIndex, const GLint* aiXYZW);
GT_EXTERN void GTglVertexAttrib4sARB (GLuint uiIndex, GLshort sX, GLshort sY,
    GLshort sZ, GLshort sW);
GT_EXTERN void GTglVertexAttrib4svARB (GLuint uiIndex, const GLshort* asXYZW);
GT_EXTERN void GTglVertexAttrib4ubvARB (GLuint uiIndex,
    const GLubyte* aucXYZW);
GT_EXTERN void GTglVertexAttrib4uivARB (GLuint uiIndex,
    const GLuint* auiXYZW);
GT_EXTERN void GTglVertexAttrib4usvARB (GLuint uiIndex,
    const GLushort* ausXYZW);
GT_EXTERN void GTglVertexAttribPointerARB (GLuint uiIndex, GLint iSize,
    GLenum eType, GLboolean ucNormalized, GLsizei iStride,
    const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 27) GL_ARB_fragment_program
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbFragmentProgram;

#define GL_FRAGMENT_PROGRAM_ARB                                     0x8804
#define GL_PROGRAM_ALU_INSTRUCTIONS_ARB                             0x8805
#define GL_PROGRAM_TEX_INSTRUCTIONS_ARB                             0x8806
#define GL_PROGRAM_TEX_INDIRECTIONS_ARB                             0x8807
#define GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB                      0x8808
#define GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB                      0x8809
#define GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB                      0x880A
#define GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB                         0x880B
#define GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB                         0x880C
#define GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB                         0x880D
#define GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB                  0x880E
#define GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB                  0x880F
#define GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB                  0x8810
#define GL_MAX_TEXTURE_COORDS_ARB                                   0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB                              0x8872
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 28) GL_ARB_vertex_buffer_object
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbVertexBufferObject;

#define GL_BUFFER_SIZE_ARB                                          0x8764
#define GL_BUFFER_USAGE_ARB                                         0x8765
#define GL_ARRAY_BUFFER_ARB                                         0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB                                 0x8893
#define GL_ARRAY_BUFFER_BINDING_ARB                                 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB                         0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB                          0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB                          0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB                           0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB                           0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB                   0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB                       0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB                 0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB                  0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB                          0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB                   0x889F
#define GL_READ_ONLY_ARB                                            0x88B8
#define GL_WRITE_ONLY_ARB                                           0x88B9
#define GL_READ_WRITE_ARB                                           0x88BA
#define GL_BUFFER_ACCESS_ARB                                        0x88BB
#define GL_BUFFER_MAPPED_ARB                                        0x88BC
#define GL_BUFFER_MAP_POINTER_ARB                                   0x88BD
#define GL_STREAM_DRAW_ARB                                          0x88E0
#define GL_STREAM_READ_ARB                                          0x88E1
#define GL_STREAM_COPY_ARB                                          0x88E2
#define GL_STATIC_DRAW_ARB                                          0x88E4
#define GL_STATIC_READ_ARB                                          0x88E5
#define GL_STATIC_COPY_ARB                                          0x88E6
#define GL_DYNAMIC_DRAW_ARB                                         0x88E8
#define GL_DYNAMIC_READ_ARB                                         0x88E9
#define GL_DYNAMIC_COPY_ARB                                         0x88EA

typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;

#define glBindBufferARB GTglBindBufferARB
#define glDeleteBuffersARB GTglDeleteBuffersARB
#define glGenBuffersARB GTglGenBuffersARB
#define glIsBufferARB GTglIsBufferARB
#define glBufferDataARB GTglBufferDataARB
#define glBufferSubDataARB GTglBufferSubDataARB
#define glGetBufferSubDataARB GTglGetBufferSubDataARB
#define glMapBufferARB GTglMapBufferARB
#define glUnmapBufferARB GTglUnmapBufferARB
#define glGetBufferParameterivARB GTglGetBufferParameterivARB
#define glGetBufferPointervARB GTglGetBufferPointervARB

GT_EXTERN void GTglBindBufferARB (GLenum eTarget, GLuint uiBuffer);
GT_EXTERN void GTglDeleteBuffersARB (GLsizei iNumBuffers,
    const GLuint* auiBuffer);
GT_EXTERN void GTglGenBuffersARB (GLsizei iNumBuffers, GLuint* auiBuffer);
GT_EXTERN GLboolean GTglIsBufferARB (GLuint uiBuffer);
GT_EXTERN void GTglBufferDataARB (GLenum eTarget, GLsizeiptrARB piSize,
    const GLvoid* pvData, GLenum eUsage);
GT_EXTERN void GTglBufferSubDataARB (GLenum eTarget, GLintptrARB piOffset,
    GLsizeiptrARB piSize, const GLvoid* pvData);
GT_EXTERN void GTglGetBufferSubDataARB (GLenum eTarget, GLintptrARB piOffset,
    GLsizeiptrARB piSize, GLvoid* pvData);
GT_EXTERN GLvoid* GTglMapBufferARB (GLenum eTarget, GLenum eAccess);
GT_EXTERN GLboolean GTglUnmapBufferARB (GLenum eTarget);
GT_EXTERN void GTglGetBufferParameterivARB (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetBufferPointervARB (GLenum eTarget, GLenum ePName,
    GLvoid** apvParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 29) GL_ARB_occlusion_query
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbOcclusionQuery;

#define GL_QUERY_COUNTER_BITS_ARB                                   0x8864
#define GL_CURRENT_QUERY_ARB                                        0x8865
#define GL_QUERY_RESULT_ARB                                         0x8866
#define GL_QUERY_RESULT_AVAILABLE_ARB                               0x8867
#define GL_SAMPLES_PASSED_ARB                                       0x8914

#define glGenQueriesARB GTglGenQueriesARB
#define glDeleteQueriesARB GTglDeleteQueriesARB
#define glIsQueryARB GTglIsQueryARB
#define glBeginQueryARB GTglBeginQueryARB
#define glEndQueryARB GTglEndQueryARB
#define glGetQueryivARB GTglGetQueryivARB
#define glGetQueryObjectivARB GTglGetQueryObjectivARB
#define glGetQueryObjectuivARB GTglGetQueryObjectuivARB

GT_EXTERN void GTglGenQueriesARB (GLsizei iNumQueries, GLuint* auiID);
GT_EXTERN void GTglDeleteQueriesARB (GLsizei iNumQueries,
    const GLuint* auiID);
GT_EXTERN GLboolean GTglIsQueryARB (GLuint uiID);
GT_EXTERN void GTglBeginQueryARB (GLenum eTarget, GLuint uiID);
GT_EXTERN void GTglEndQueryARB (GLenum eTarget);
GT_EXTERN void GTglGetQueryivARB (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetQueryObjectivARB (GLuint uiID, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetQueryObjectuivARB (GLuint uiID, GLenum ePName,
    GLuint* auiParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 29) GL_ARB_shader_objects
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbShaderObjects;

#define GL_PROGRAM_OBJECT_ARB                                       0x8B40
#define GL_SHADER_OBJECT_ARB                                        0x8B48
#define GL_OBJECT_TYPE_ARB                                          0x8B4E
#define GL_OBJECT_SUBTYPE_ARB                                       0x8B4F
#define GL_FLOAT_VEC2_ARB                                           0x8B50
#define GL_FLOAT_VEC3_ARB                                           0x8B51
#define GL_FLOAT_VEC4_ARB                                           0x8B52
#define GL_INT_VEC2_ARB                                             0x8B53
#define GL_INT_VEC3_ARB                                             0x8B54
#define GL_INT_VEC4_ARB                                             0x8B55
#define GL_BOOL_ARB                                                 0x8B56
#define GL_BOOL_VEC2_ARB                                            0x8B57
#define GL_BOOL_VEC3_ARB                                            0x8B58
#define GL_BOOL_VEC4_ARB                                            0x8B59
#define GL_FLOAT_MAT2_ARB                                           0x8B5A
#define GL_FLOAT_MAT3_ARB                                           0x8B5B
#define GL_FLOAT_MAT4_ARB                                           0x8B5C
#define GL_SAMPLER_1D_ARB                                           0x8B5D
#define GL_SAMPLER_2D_ARB                                           0x8B5E
#define GL_SAMPLER_3D_ARB                                           0x8B5F
#define GL_SAMPLER_CUBE_ARB                                         0x8B60
#define GL_SAMPLER_1D_SHADOW_ARB                                    0x8B61
#define GL_SAMPLER_2D_SHADOW_ARB                                    0x8B62
#define GL_SAMPLER_2D_RECT_ARB                                      0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW_ARB                               0x8B64
#define GL_OBJECT_DELETE_STATUS_ARB                                 0x8B80
#define GL_OBJECT_COMPILE_STATUS_ARB                                0x8B81
#define GL_OBJECT_LINK_STATUS_ARB                                   0x8B82
#define GL_OBJECT_VALIDATE_STATUS_ARB                               0x8B83
#define GL_OBJECT_INFO_LOG_LENGTH_ARB                               0x8B84
#define GL_OBJECT_ATTACHED_OBJECTS_ARB                              0x8B85
#define GL_OBJECT_ACTIVE_UNIFORMS_ARB                               0x8B86
#define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB                     0x8B87
#define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB                          0x8B88

typedef char GLcharARB;
typedef unsigned int GLhandleARB;

#define glAttachObjectARB GTglAttachObjectARB
#define glCompileShaderARB GTglCompileShaderARB
#define glCreateProgramObjectARB GTglCreateProgramObjectARB
#define glCreateShaderObjectARB GTglCreateShaderObjectARB
#define glDeleteObjectARB GTglDeleteObjectARB
#define glDetachObjectARB GTglDetachObjectARB
#define glGetActiveUniformARB GTglGetActiveUniformARB
#define glGetAttachedObjectsARB GTglGetAttachedObjectsARB
#define glGetHandleARB GTglGetHandleARB
#define glGetInfoLogARB GTglGetInfoLogARB
#define glGetObjectParameterfvARB GTglGetObjectParameterfvARB
#define glGetObjectParameterivARB GTglGetObjectParameterivARB
#define glGetShaderSourceARB GTglGetShaderSourceARB
#define glGetUniformfvARB GTglGetUniformfvARB
#define glGetUniformivARB GTglGetUniformivARB
#define glGetUniformLocationARB GTglGetUniformLocationARB
#define glLinkProgramARB GTglLinkProgramARB
#define glShaderSourceARB GTglShaderSourceARB
#define glUniform1fARB GTglUniform1fARB
#define glUniform2fARB GTglUniform2fARB
#define glUniform3fARB GTglUniform3fARB
#define glUniform4fARB GTglUniform4fARB
#define glUniform1iARB GTglUniform1iARB
#define glUniform2iARB GTglUniform2iARB
#define glUniform3iARB GTglUniform3iARB
#define glUniform4iARB GTglUniform4iARB
#define glUniform1fvARB GTglUniform1fvARB
#define glUniform2fvARB GTglUniform2fvARB
#define glUniform3fvARB GTglUniform3fvARB
#define glUniform4fvARB GTglUniform4fvARB
#define glUniform1ivARB GTglUniform1ivARB
#define glUniform2ivARB GTglUniform2ivARB
#define glUniform3ivARB GTglUniform3ivARB
#define glUniform4ivARB GTglUniform4ivARB
#define glUniformMatrix2fvARB GTglUniformMatrix2fvARB
#define glUniformMatrix3fvARB GTglUniformMatrix3fvARB
#define glUniformMatrix4fvARB GTglUniformMatrix4fvARB
#define glUseProgramObjectARB GTglUseProgramObjectARB
#define glValidateProgramARB GTglValidateProgramARB

GT_EXTERN void GTglAttachObjectARB (GLhandleARB uiContainer,
    GLhandleARB uiObject);
GT_EXTERN void GTglCompileShaderARB (GLhandleARB uiShader);
GT_EXTERN GLhandleARB GTglCreateProgramObjectARB ();
GT_EXTERN GLhandleARB GTglCreateShaderObjectARB (GLenum eType);
GT_EXTERN void GTglDeleteObjectARB (GLhandleARB uiObject);
GT_EXTERN void GTglDetachObjectARB (GLhandleARB uiContainer,
    GLhandleARB uiObject);
GT_EXTERN void GTglGetActiveUniformARB (GLhandleARB uiProgram, GLuint uiIndex,
    GLsizei iMaxLength, GLsizei* piLength, GLint* piSize, GLenum* peType,
    GLcharARB* acName);
GT_EXTERN void GTglGetAttachedObjectsARB (GLhandleARB uiContainer,
    GLsizei iMaxCount, GLsizei* aiCount, GLhandleARB* auiObject);
GT_EXTERN GLhandleARB GTglGetHandleARB (GLenum ePName);
GT_EXTERN void GTglGetInfoLogARB (GLhandleARB uiObject, GLsizei iMaxLength,
    GLsizei* piLength, GLcharARB* acInfoLog);
GT_EXTERN void GTglGetObjectParameterfvARB (GLhandleARB uiObject,
    GLenum ePName, GLfloat* afParam);
GT_EXTERN void GTglGetObjectParameterivARB (GLhandleARB uiObject,
    GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGetShaderSourceARB (GLhandleARB uiObject,
    GLsizei iMaxLength, GLsizei* piLength, GLcharARB* acSource);
GT_EXTERN GLint GTglGetUniformLocationARB (GLhandleARB uiProgram,
    const GLcharARB* acName);
GT_EXTERN void GTglGetUniformfvARB (GLhandleARB uiProgram, GLint iLocation,
    GLfloat* afParam);
GT_EXTERN void GTglGetUniformivARB (GLhandleARB uiProgram, GLint iLocation,
    GLint* aiParam);
GT_EXTERN void GTglLinkProgramARB (GLhandleARB uiProgram);
GT_EXTERN void GTglShaderSourceARB (GLhandleARB uiShader, GLsizei iCount,
    const GLcharARB** aacString, const GLint* aiLength);
GT_EXTERN void GTglUniform1fARB (GLint iLocation, GLfloat fV0);
GT_EXTERN void GTglUniform2fARB (GLint iLocation, GLfloat fV0, GLfloat fV1);
GT_EXTERN void GTglUniform3fARB (GLint iLocation, GLfloat fV0, GLfloat fV1,
    GLfloat fV2);
GT_EXTERN void GTglUniform4fARB (GLint iLocation, GLfloat fV0, GLfloat fV1,
    GLfloat fV2, GLfloat fV3);
GT_EXTERN void GTglUniform1iARB (GLint iLocation, GLint iV0);
GT_EXTERN void GTglUniform2iARB (GLint iLocation, GLint iV0, GLint iV1);
GT_EXTERN void GTglUniform3iARB (GLint iLocation, GLint iV0, GLint iV1,
    GLint iV2);
GT_EXTERN void GTglUniform4iARB (GLint iLocation, GLint iV0, GLint iV1,
    GLint iV2, GLint iV3);
GT_EXTERN void GTglUniform1fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform2fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform3fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform4fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue);
GT_EXTERN void GTglUniform1ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniform2ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniform3ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniform4ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue);
GT_EXTERN void GTglUniformMatrix2fvARB (GLint iLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue);
GT_EXTERN void GTglUniformMatrix3fvARB (GLint iLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue);
GT_EXTERN void GTglUniformMatrix4fvARB (GLint iLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue);
GT_EXTERN void GTglUseProgramObjectARB (GLhandleARB uiProgram);
GT_EXTERN void GTglValidateProgramARB (GLhandleARB uiProgram);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 31) GL_ARB_vertex_shader
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbVertexShader;

#define GL_VERTEX_SHADER_ARB                                        0x8B31
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB                        0x8B4A
#define GL_MAX_VARYING_FLOATS_ARB                                   0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB                       0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB                     0x8B4D
#define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB                             0x8B89
#define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB                   0x8B8A

#define glBindAttribLocationARB GTglBindAttribLocationARB
#define glGetActiveAttribARB GTglGetActiveAttribARB
#define glGetAttribLocationARB GTglGetAttribLocationARB

GT_EXTERN void GTglBindAttribLocationARB (GLhandleARB uiProgram,
    GLuint uiIndex, const GLcharARB* acName);
GT_EXTERN void GTglGetActiveAttribARB (GLhandleARB uiProgram, GLuint uiIndex,
    GLsizei iMaxLength, GLsizei* piLength, GLint* piSize, GLenum* peType,
    GLcharARB* acName);
GT_EXTERN GLint GTglGetAttribLocationARB (GLhandleARB uiProgram,
    const GLcharARB* acName);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 32) GL_ARB_fragment_shader
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbFragmentShader;

#define GL_FRAGMENT_SHADER_ARB                                      0x8B30
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB                      0x8B49
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB                      0x8B8B
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 33) GL_ARB_shading_language_100
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbShadingLanguage100;

#define GL_SHADING_LANGUAGE_VERSION_ARB                             0x8B8C
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 34) GL_ARB_texture_non_power_of_two
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureNonpowerOfTwo;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 35) GL_ARB_point_sprite
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbPointSprite;

#define GL_POINT_SPRITE_ARB                                         0x8861
#define GL_COORD_REPLACE_ARB                                        0x8862
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 36) GL_ARB_fragment_program_shadow
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbFragmentProgramShadow;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 37) GL_ARB_draw_buffers
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbDrawBuffers;

#define GL_MAX_DRAW_BUFFERS_ARB                                     0x8824
#define GL_DRAW_BUFFER0_ARB                                         0x8825
#define GL_DRAW_BUFFER1_ARB                                         0x8826
#define GL_DRAW_BUFFER2_ARB                                         0x8827
#define GL_DRAW_BUFFER3_ARB                                         0x8828
#define GL_DRAW_BUFFER4_ARB                                         0x8829
#define GL_DRAW_BUFFER5_ARB                                         0x882A
#define GL_DRAW_BUFFER6_ARB                                         0x882B
#define GL_DRAW_BUFFER7_ARB                                         0x882C
#define GL_DRAW_BUFFER8_ARB                                         0x882D
#define GL_DRAW_BUFFER9_ARB                                         0x882E
#define GL_DRAW_BUFFER10_ARB                                        0x882F
#define GL_DRAW_BUFFER11_ARB                                        0x8830
#define GL_DRAW_BUFFER12_ARB                                        0x8831
#define GL_DRAW_BUFFER13_ARB                                        0x8832
#define GL_DRAW_BUFFER14_ARB                                        0x8833
#define GL_DRAW_BUFFER15_ARB                                        0x8834

#define glDrawBuffersARB GTglDrawBuffersARB

GT_EXTERN void GTglDrawBuffersARB (GLsizei iNumBuffers,
    const GLenum* aeBuffer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 38) GL_ARB_texture_rectangle
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureRectangle;

#define GL_TEXTURE_RECTANGLE_ARB                                    0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_ARB                            0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_ARB                              0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB                           0x84F8
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 39) GL_ARB_color_buffer_float
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbColorBufferFloat;

#define GL_RGBA_FLOAT_MODE_ARB                                      0x8820
#define GL_CLAMP_VERTEX_COLOR_ARB                                   0x891A
#define GL_CLAMP_FRAGMENT_COLOR_ARB                                 0x891B
#define GL_CLAMP_READ_COLOR_ARB                                     0x891C
#define GL_FIXED_ONLY_ARB                                           0x891D

#define glClampColorARB GTglClampColorARB

GT_EXTERN void GTglClampColorARB (GLenum eTarget, GLenum eClamp);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 40) GL_ARB_half_float_pixel
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbHalfFloatPixel;

#define GL_HALF_FLOAT_ARB                                           0x140B
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 41) GL_ARB_texture_float
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbTextureFloat;

#define GL_TEXTURE_RED_TYPE_ARB                                     0x8C10
#define GL_TEXTURE_GREEN_TYPE_ARB                                   0x8C11
#define GL_TEXTURE_BLUE_TYPE_ARB                                    0x8C12
#define GL_TEXTURE_ALPHA_TYPE_ARB                                   0x8C13
#define GL_TEXTURE_LUMINANCE_TYPE_ARB                               0x8C14
#define GL_TEXTURE_INTENSITY_TYPE_ARB                               0x8C15
#define GL_TEXTURE_DEPTH_TYPE_ARB                                   0x8C16
#define GL_UNSIGNED_NORMALIZED_ARB                                  0x8C17
#define GL_RGBA32F_ARB                                              0x8814
#define GL_RGB32F_ARB                                               0x8815
#define GL_ALPHA32F_ARB                                             0x8816
#define GL_INTENSITY32F_ARB                                         0x8817
#define GL_LUMINANCE32F_ARB                                         0x8818
#define GL_LUMINANCE_ALPHA32F_ARB                                   0x8819
#define GL_RGBA16F_ARB                                              0x881A
#define GL_RGB16F_ARB                                               0x881B
#define GL_ALPHA16F_ARB                                             0x881C
#define GL_INTENSITY16F_ARB                                         0x881D
#define GL_LUMINANCE16F_ARB                                         0x881E
#define GL_LUMINANCE_ALPHA16F_ARB                                   0x881F
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 42) GL_ARB_pixel_buffer_object
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbPixelBufferObject;

#define GL_PIXEL_PACK_BUFFER_ARB                                    0x88EB
#define GL_PIXEL_UNPACK_BUFFER_ARB                                  0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING_ARB                            0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING_ARB                          0x88EF
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 50) GL_ARB_map_buffer_range
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlArbMapBufferRange;

#define GL_MAP_READ_BIT                                             0x0001
#define GL_MAP_WRITE_BIT                                            0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT                                 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT                                0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT                                   0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT                                   0x0020

#define glMapBufferRange GTglMapBufferRange
#define glFlushMappedBufferRange GTglFlushMappedBufferRange

GT_EXTERN GLvoid* GTglMapBufferRange (GLenum target, GLintptr offset,
    GLsizeiptr length, GLbitfield access);
GT_EXTERN void GTglFlushMappedBufferRange (GLenum target, GLintptr offset,
    GLsizeiptr length);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 53) GL_ARB_texture_rg
//----------------------------------------------------------------------------
#define GL_RG                                                       0x8227
#define GL_RG_INTEGER                                               0x8228
#define GL_R8                                                       0x8229
#define GL_R16                                                      0x822A
#define GL_RG8                                                      0x822B
#define GL_RG16                                                     0x822C
#define GL_R16F                                                     0x822D
#define GL_R32F                                                     0x822E
#define GL_RG16F                                                    0x822F
#define GL_RG32F                                                    0x8230
#define GL_R8I                                                      0x8231
#define GL_R8UI                                                     0x8232
#define GL_R16I                                                     0x8233
#define GL_R16UI                                                    0x8234
#define GL_R32I                                                     0x8235
#define GL_R32UI                                                    0x8236
#define GL_RG8I                                                     0x8237
#define GL_RG8UI                                                    0x8238
#define GL_RG16I                                                    0x8239
#define GL_RG16UI                                                   0x823A
#define GL_RG32I                                                    0x823B
#define GL_RG32UI                                                   0x823C
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** EXT Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 001) GL_EXT_abgr
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtABGR;

#define GL_ABGR_EXT                                                 0x8000
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 002) GL_EXT_blend_color
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtBlendColor;

#define GL_CONSTANT_COLOR_EXT                                       0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT                             0x8002
#define GL_CONSTANT_ALPHA_EXT                                       0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT                             0x8004
#define GL_BLEND_COLOR_EXT                                          0x8005

#define glBlendColorEXT GTglBlendColorEXT

GT_EXTERN void GTglBlendColorEXT (GLclampf fRed, GLclampf fGreen,
    GLclampf fBlue, GLclampf fAlpha);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 003) GL_EXT_polygon_offset
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPolygonOffset;

#define GL_POLYGON_OFFSET_EXT                                       0x8037
#define GL_POLYGON_OFFSET_FACTOR_EXT                                0x8038
#define GL_POLYGON_OFFSET_BIAS_EXT                                  0x8039

#define glPolygonOffsetEXT GTglPolygonOffsetEXT

GT_EXTERN void GTglPolygonOffsetEXT (GLfloat fScale, GLfloat fBias);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 004) GL_EXT_texture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTexture;

#define GL_ALPHA4_EXT                                               0x803B
#define GL_ALPHA8_EXT                                               0x803C
#define GL_ALPHA12_EXT                                              0x803D
#define GL_ALPHA16_EXT                                              0x803E
#define GL_LUMINANCE4_EXT                                           0x803F
#define GL_LUMINANCE8_EXT                                           0x8040
#define GL_LUMINANCE12_EXT                                          0x8041
#define GL_LUMINANCE16_EXT                                          0x8042
#define GL_LUMINANCE4_ALPHA4_EXT                                    0x8043
#define GL_LUMINANCE6_ALPHA2_EXT                                    0x8044
#define GL_LUMINANCE8_ALPHA8_EXT                                    0x8045
#define GL_LUMINANCE12_ALPHA4_EXT                                   0x8046
#define GL_LUMINANCE12_ALPHA12_EXT                                  0x8047
#define GL_LUMINANCE16_ALPHA16_EXT                                  0x8048
#define GL_INTENSITY_EXT                                            0x8049
#define GL_INTENSITY4_EXT                                           0x804A
#define GL_INTENSITY8_EXT                                           0x804B
#define GL_INTENSITY12_EXT                                          0x804C
#define GL_INTENSITY16_EXT                                          0x804D
#define GL_RGB2_EXT                                                 0x804E
#define GL_RGB4_EXT                                                 0x804F
#define GL_RGB5_EXT                                                 0x8050
#define GL_RGB8_EXT                                                 0x8051
#define GL_RGB10_EXT                                                0x8052
#define GL_RGB12_EXT                                                0x8053
#define GL_RGB16_EXT                                                0x8054
#define GL_RGBA2_EXT                                                0x8055
#define GL_RGBA4_EXT                                                0x8056
#define GL_RGB5_A1_EXT                                              0x8057
#define GL_RGBA8_EXT                                                0x8058
#define GL_RGB10_A2_EXT                                             0x8059
#define GL_RGBA12_EXT                                               0x805A
#define GL_RGBA16_EXT                                               0x805B
#define GL_TEXTURE_RED_SIZE_EXT                                     0x805C
#define GL_TEXTURE_GREEN_SIZE_EXT                                   0x805D
#define GL_TEXTURE_BLUE_SIZE_EXT                                    0x805E
#define GL_TEXTURE_ALPHA_SIZE_EXT                                   0x805F
#define GL_TEXTURE_LUMINANCE_SIZE_EXT                               0x8060
#define GL_TEXTURE_INTENSITY_SIZE_EXT                               0x8061
#define GL_REPLACE_EXT                                              0x8062
#define GL_PROXY_TEXTURE_1D_EXT                                     0x8063
#define GL_PROXY_TEXTURE_2D_EXT                                     0x8064
#define GL_TEXTURE_TOO_LARGE_EXT                                    0x8065
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 006) GL_EXT_texture3D
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTexture3D;

#define GL_PACK_SKIP_IMAGES_EXT                                     0x806B
#define GL_PACK_IMAGE_HEIGHT_EXT                                    0x806C
#define GL_UNPACK_SKIP_IMAGES_EXT                                   0x806D
#define GL_UNPACK_IMAGE_HEIGHT_EXT                                  0x806E
#define GL_TEXTURE_3D_EXT                                           0x806F
#define GL_PROXY_TEXTURE_3D_EXT                                     0x8070
#define GL_TEXTURE_DEPTH_EXT                                        0x8071
#define GL_TEXTURE_WRAP_R_EXT                                       0x8072
#define GL_MAX_3D_TEXTURE_SIZE_EXT                                  0x8073

#define glTexImage3DEXT GTglTexImage3DEXT
#define glTexSubImage3DEXT GTglTexSubImage3DEXT

GT_EXTERN void GTglTexImage3DEXT (GLenum eTarget, GLint iLevel,
    GLint iInternalFormat, GLsizei iWidth, GLsizei iHeight, GLsizei iDepth,
    GLint iBorder, GLenum eFormat, GLenum eType, const GLvoid* pvData);
GT_EXTERN void GTglTexSubImage3DEXT (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLsizei iWidth,
    GLsizei iHeight, GLsizei iDepth, GLenum eFormat, GLenum eType,
    const GLvoid* pvData);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 009) GL_EXT_subtexture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtSubtexture;

#define glTexSubImage1DEXT GTglTexSubImage1DEXT
#define glTexSubImage2DEXT GTglTexSubImage2DEXT

GT_EXTERN void GTglTexSubImage1DEXT (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLsizei iWidth, GLenum eFormat, GLenum eType,
    const GLvoid* pvData);
GT_EXTERN void GTglTexSubImage2DEXT (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLenum eType, const GLvoid* pvData);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 010) GL_EXT_copy_texture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtCopyTexture;

#define glCopyTexImage1DEXT GTglCopyTexImage1DEXT
#define glCopyTexImage2DEXT GTglCopyTexImage2DEXT
#define glCopyTexSubImage1DEXT GTglCopyTexSubImage1DEXT
#define glCopyTexSubImage2DEXT GTglCopyTexSubImage2DEXT
#define glCopyTexSubImage3DEXT GTglCopyTexSubImage3DEXT

GT_EXTERN void GTglCopyTexImage1DEXT (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth,
    GLint iBorder);
GT_EXTERN void GTglCopyTexImage2DEXT (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight, GLint iBorder);
GT_EXTERN void GTglCopyTexSubImage1DEXT (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iX, GLint iY, GLsizei iWidth);
GT_EXTERN void GTglCopyTexSubImage2DEXT (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight);
GT_EXTERN void GTglCopyTexSubImage3DEXT (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLint iX, GLint iY,
    GLsizei iWidth, GLsizei iHeight);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 011) GL_EXT_histogram
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtHistogram;

#define GL_HISTOGRAM_EXT                                            0x8024
#define GL_PROXY_HISTOGRAM_EXT                                      0x8025
#define GL_HISTOGRAM_WIDTH_EXT                                      0x8026
#define GL_HISTOGRAM_FORMAT_EXT                                     0x8027
#define GL_HISTOGRAM_RED_SIZE_EXT                                   0x8028
#define GL_HISTOGRAM_GREEN_SIZE_EXT                                 0x8029
#define GL_HISTOGRAM_BLUE_SIZE_EXT                                  0x802A
#define GL_HISTOGRAM_ALPHA_SIZE_EXT                                 0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE_EXT                             0x802C
#define GL_HISTOGRAM_SINK_EXT                                       0x802D
#define GL_MINMAX_EXT                                               0x802E
#define GL_MINMAX_FORMAT_EXT                                        0x802F
#define GL_MINMAX_SINK_EXT                                          0x8030
#define GL_TABLE_TOO_LARGE_EXT                                      0x8031

#define glGetHistogramEXT GTglGetHistogramEXT
#define glGetHistogramParameterfvEXT GTglGetHistogramParameterfvEXT
#define glGetHistogramParameterivEXT GTglGetHistogramParameterivEXT
#define glGetMinmaxEXT GTglGetMinmaxEXT
#define glGetMinmaxParameterfvEXT GTglGetMinmaxParameterfvEXT
#define glGetMinmaxParameterivEXT GTglGetMinmaxParameterivEXT
#define glHistogramEXT GTglHistogramEXT
#define glMinmaxEXT GTglMinmaxEXT
#define glResetHistogramEXT GTglResetHistogramEXT
#define glResetMinmaxEXT GTglResetMinmaxEXT

GT_EXTERN void GTglGetHistogramEXT (GLenum eTarget, GLboolean ucReset,
    GLenum eFormat, GLenum eType, GLvoid* pvValues);
GT_EXTERN void GTglGetHistogramParameterfvEXT (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetHistogramParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglGetMinmaxEXT (GLenum eTarget, GLboolean ucReset,
    GLenum eFormat, GLenum eType, GLvoid* pvValues);
GT_EXTERN void GTglGetMinmaxParameterfvEXT (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetMinmaxParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
GT_EXTERN void GTglHistogramEXT (GLenum eTarget, GLsizei iWidth,
    GLenum eInternalFormat, GLboolean ucSink);
GT_EXTERN void GTglMinmaxEXT (GLenum eTarget, GLenum eInternalFormat,
    GLboolean ucSink);
GT_EXTERN void GTglResetHistogramEXT (GLenum eTarget);
GT_EXTERN void GTglResetMinmaxEXT (GLenum eTarget);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 012) GL_EXT_convolution
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtConvolution;

#define GL_CONVOLUTION_1D_EXT                                       0x8010
#define GL_CONVOLUTION_2D_EXT                                       0x8011
#define GL_SEPARABLE_2D_EXT                                         0x8012
#define GL_CONVOLUTION_BORDER_MODE_EXT                              0x8013
#define GL_CONVOLUTION_FILTER_SCALE_EXT                             0x8014
#define GL_CONVOLUTION_FILTER_BIAS_EXT                              0x8015
#define GL_REDUCE_EXT                                               0x8016
#define GL_CONVOLUTION_FORMAT_EXT                                   0x8017
#define GL_CONVOLUTION_WIDTH_EXT                                    0x8018
#define GL_CONVOLUTION_HEIGHT_EXT                                   0x8019
#define GL_MAX_CONVOLUTION_WIDTH_EXT                                0x801A
#define GL_MAX_CONVOLUTION_HEIGHT_EXT                               0x801B
#define GL_POST_CONVOLUTION_RED_SCALE_EXT                           0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE_EXT                         0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE_EXT                          0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE_EXT                         0x801F
#define GL_POST_CONVOLUTION_RED_BIAS_EXT                            0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS_EXT                          0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS_EXT                           0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS_EXT                          0x8023

#define glConvolutionFilter1DEXT GTglConvolutionFilter1DEXT
#define glConvolutionFilter2DEXT GTglConvolutionFilter2DEXT
#define glConvolutionParameterfEXT GTglConvolutionParameterfEXT
#define glConvolutionParameterfvEXT GTglConvolutionParameterfvEXT
#define glConvolutionParameteriEXT GTglConvolutionParameteriEXT
#define glConvolutionParameterivEXT GTglConvolutionParameterivEXT
#define glCopyConvolutionFilter1DEXT GTglCopyConvolutionFilter1DEXT
#define glCopyConvolutionFilter2DEXT GTglCopyConvolutionFilter2DEXT
#define glGetConvolutionFilterEXT GTglGetConvolutionFilterEXT
#define glGetConvolutionParameterfvEXT GTglGetConvolutionParameterfvEXT
#define glGetConvolutionParameterivEXT GTglGetConvolutionParameterivEXT
#define glGetSeparableFilterEXT GTglGetSeparableFilterEXT
#define glSeparableFilter2DEXT GTglSeparableFilter2DEXT

GT_EXTERN void GTglConvolutionFilter1DEXT (GLenum eTarget,
    GLenum eInternalFormat, GLsizei iWidth, GLenum eFormat, GLenum eType,
    const GLvoid* pvImage);
GT_EXTERN void GTglConvolutionFilter2DEXT (GLenum eTarget,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLenum eFormat,
    GLenum eType, const GLvoid* pvImage);
GT_EXTERN void GTglConvolutionParameterfEXT (GLenum eTarget, GLenum ePName,
    GLfloat fParam);
GT_EXTERN void GTglConvolutionParameterfvEXT (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam);
GT_EXTERN void GTglConvolutionParameteriEXT (GLenum eTarget, GLenum ePName,
    GLint iParam);
GT_EXTERN void GTglConvolutionParameterivEXT (GLenum eTarget, GLenum ePName,
    const GLint* aiParam);
GT_EXTERN void GTglCopyConvolutionFilter1DEXT (GLenum eTarget,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth);
GT_EXTERN void GTglCopyConvolutionFilter2DEXT (GLenum eTarget,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight);
GT_EXTERN void GTglGetConvolutionFilterEXT (GLenum eTarget, GLenum eFormat,
    GLenum eType, GLvoid* pvImage);
GT_EXTERN void GTglGetConvolutionParameterfvEXT (GLenum eTarget,
    GLenum ePName, GLfloat* afParam);
GT_EXTERN void GTglGetConvolutionParameterivEXT (GLenum eTarget,
    GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGetSeparableFilterEXT (GLenum eTarget, GLenum eFormat,
    GLenum eType, GLvoid* pvRow, GLvoid* pvColumn, GLvoid* pvSpan);
GT_EXTERN void GTglSeparableFilter2DEXT (GLenum eTarget,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLenum eFormat,
    GLenum eType, const GLvoid* pvRow, const GLvoid* pvColumn);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 018) GL_EXT_cmyka
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtCMYKA;

#define GL_CMYK_EXT                                                 0x800C
#define GL_CMYKA_EXT                                                0x800D
#define GL_PACK_CMYK_HINT_EXT                                       0x800E
#define GL_UNPACK_CMYK_HINT_EXT                                     0x800F
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 020) GL_EXT_texture3D
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureObject;

#define GL_TEXTURE_PRIORITY_EXT                                     0x8066
#define GL_TEXTURE_RESIDENT_EXT                                     0x8067
#define GL_TEXTURE_1D_BINDING_EXT                                   0x8068
#define GL_TEXTURE_2D_BINDING_EXT                                   0x8069
#define GL_TEXTURE_3D_BINDING_EXT                                   0x806A

#define glAreTexturesResidentEXT GTglAreTexturesResidentEXT
#define glBindTextureEXT GTglBindTextureEXT
#define glDeleteTexturesEXT GTglDeleteTexturesEXT
#define glGenTexturesEXT GTglGenTexturesEXT
#define glIsTextureEXT GTglIsTextureEXT
#define glPrioritizeTexturesEXT GTglPrioritizeTexturesEXT

GT_EXTERN GLboolean GTglAreTexturesResidentEXT (GLsizei iNumTextures,
    const GLuint* auiTextures, GLboolean* aucResidences);
GT_EXTERN void GTglBindTextureEXT (GLenum eTarget, GLuint uiTexture);
GT_EXTERN void GTglDeleteTexturesEXT (GLsizei iNumTextures,
    const GLuint* auiTextures);
GT_EXTERN void GTglGenTexturesEXT (GLsizei iNumTextures, GLuint* auiTexture);
GT_EXTERN GLboolean GTglIsTextureEXT (GLuint uiTexture);
GT_EXTERN void GTglPrioritizeTexturesEXT (GLsizei iNumTextures,
    const GLuint* auiTextures, const GLclampf* afPriorities);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 023) GL_EXT_packed_pixels
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPackedPixels;

#define GL_UNSIGNED_BYTE_3_3_2_EXT                                  0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT                               0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT                               0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT                                 0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT                              0x8036
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 027) GL_EXT_rescale_normal
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtRescaleNormal;

#define GL_RESCALE_NORMAL_EXT                                       0x803A
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 030) GL_EXT_vertex_array
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtVertexArray;

#define GL_VERTEX_ARRAY_EXT                                         0x8074
#define GL_NORMAL_ARRAY_EXT                                         0x8075
#define GL_COLOR_ARRAY_EXT                                          0x8076
#define GL_INDEX_ARRAY_EXT                                          0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT                                  0x8078
#define GL_EDGE_FLAG_ARRAY_EXT                                      0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT                                    0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT                                    0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT                                  0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT                                   0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT                                    0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT                                  0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT                                   0x8080
#define GL_COLOR_ARRAY_SIZE_EXT                                     0x8081
#define GL_COLOR_ARRAY_TYPE_EXT                                     0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT                                   0x8083
#define GL_COLOR_ARRAY_COUNT_EXT                                    0x8084
#define GL_INDEX_ARRAY_TYPE_EXT                                     0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT                                   0x8086
#define GL_INDEX_ARRAY_COUNT_EXT                                    0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT                             0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT                             0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT                           0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT                            0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT                               0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT                                0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT                                 0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT                                 0x808F
#define GL_COLOR_ARRAY_POINTER_EXT                                  0x8090
#define GL_INDEX_ARRAY_POINTER_EXT                                  0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT                          0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT                              0x8093

#define glArrayElementEXT GTglArrayElementEXT
#define glColorPointerEXT GTglColorPointerEXT
#define glDrawArraysEXT GTglDrawArraysEXT
#define glEdgeFlagPointerEXT GTglEdgeFlagPointerEXT
#define glGetPointervEXT GTglGetPointervEXT
#define glIndexPointerEXT GTglIndexPointerEXT
#define glNormalPointerEXT GTglNormalPointerEXT
#define glTexCoordPointerEXT GTglTexCoordPointerEXT
#define glVertexPointerEXT GTglVertexPointerEXT

GT_EXTERN void GTglArrayElementEXT (GLint i);
GT_EXTERN void GTglColorPointerEXT (GLint iSize, GLenum eType,
    GLsizei iStride, GLsizei iCount, const GLvoid* pvPointer);
GT_EXTERN void GTglDrawArraysEXT (GLenum eMode, GLint iFirst, GLsizei iCount);
GT_EXTERN void GTglEdgeFlagPointerEXT (GLsizei iStride, GLsizei iCount,
    const GLboolean* pvPointer);
GT_EXTERN void GTglGetPointervEXT (GLenum ePName, GLvoid** apvParam);
GT_EXTERN void GTglIndexPointerEXT (GLenum eType, GLsizei iStride,
    GLsizei iCount, const GLvoid* pvPointer);
GT_EXTERN void GTglNormalPointerEXT (GLenum eType, GLsizei iStride,
    GLsizei iCount, const GLvoid* pvPointer);
GT_EXTERN void GTglTexCoordPointerEXT (GLint iSize, GLenum eType,
    GLsizei iStride, GLsizei iCount, const GLvoid* pvPointer);
GT_EXTERN void GTglVertexPointerEXT (GLint iSize, GLenum eType,
    GLsizei iStride, GLsizei iCount, const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 031) GL_EXT_misc_attribute
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtMiscAttribute;

// The specification document mentions
//     GL_MISC_BIT_EXT
// but has no value assigned to it.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 037) GL_EXT_blend_minmax
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtBlendMinMax;

#define GL_FUNC_ADD_EXT                                             0x8006
#define GL_MIN_EXT                                                  0x8007
#define GL_MAX_EXT                                                  0x8008
#define GL_BLEND_EQUATION_EXT                                       0x8009

#define glBlendEquationEXT GTglBlendEquationEXT

GT_EXTERN void GTglBlendEquationEXT (GLenum eMode);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 038) GL_EXT_blend_subtract
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtBlendSubtract;

#define GL_FUNC_SUBTRACT_EXT                                        0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT                                0x800B
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 039) GL_EXT_blend_logic_op
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtBlendLogicOp;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 054) GL_EXT_point_parameters
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPointParameters;

#define GL_POINT_SIZE_MIN_EXT                                       0x8126
#define GL_POINT_SIZE_MAX_EXT                                       0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT                            0x8128
#define GL_DISTANCE_ATTENUATION_EXT                                 0x8129

#define glPointParameterfEXT GTglPointParameterfEXT
#define glPointParameterfvEXT GTglPointParameterfvEXT

GT_EXTERN void GTglPointParameterfEXT (GLenum ePName, GLfloat fParam);
GT_EXTERN void GTglPointParameterfvEXT (GLenum ePName,
    const GLfloat* afParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 074) GL_EXT_color_subtable
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtColorSubtable;

#define glColorSubTableEXT GTglColorSubTableEXT
#define glCopyColorSubTableEXT GTglCopyColorSubTableEXT

GT_EXTERN void GTglColorSubTable (GLenum eTarget, GLsizei iStart,
    GLsizei iCount, GLenum eFormat, GLenum eType, const GLvoid* pvData);
GT_EXTERN void GTglCopyColorSubTable (GLenum eTarget, GLsizei iStart,
    GLint iX, GLint iY, GLsizei iWidth);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 078) GL_EXT_paletted_texture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPalettedTexture;

#define GL_COLOR_INDEX1_EXT                                         0x80E2
#define GL_COLOR_INDEX2_EXT                                         0x80E3
#define GL_COLOR_INDEX4_EXT                                         0x80E4
#define GL_COLOR_INDEX8_EXT                                         0x80E5
#define GL_COLOR_INDEX12_EXT                                        0x80E6
#define GL_COLOR_INDEX16_EXT                                        0x80E7
#define GL_TEXTURE_INDEX_SIZE_EXT                                   0x80ED

#define glColorTableEXT GTglColorTableEXT
#define glGetColorTableEXT GTglGetColorTableEXT
#define glGetColorTableParameterfvEXT GTglGetColorTableParameterfvEXT
#define glGetColorTableParameterivEXT GTglGetColorTableParameterivEXT

GT_EXTERN void GTglColorTableEXT (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLenum eFormat, GLenum eType, const GLvoid* pvTable);
GT_EXTERN void GTglGetColorTableEXT (GLenum eTarget, GLenum eFormat,
    GLenum eType, GLvoid* pvTable);
GT_EXTERN void GTglGetColorTableParameterfvEXT (GLenum eTarget, GLenum ePName,
    GLfloat* afParam);
GT_EXTERN void GTglGetColorTableParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 079) GL_EXT_clip_volume_hint
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtClipVolumeHint;

#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT                            0x80F0
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 093) GL_EXT_index_texture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtIndexTexture;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 094) GL_EXT_index_material
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtIndexMaterial;

#define GL_INDEX_MATERIAL_EXT                                       0x81B8
#define GL_INDEX_MATERIAL_PARAMETER_EXT                             0x81B9
#define GL_INDEX_MATERIAL_FACE_EXT                                  0x81BA

#define glIndexMaterialEXT GTglIndexMaterialEXT

GT_EXTERN void GTglIndexMaterialEXT (GLenum eFace, GLenum eMode);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 095) GL_EXT_index_func
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtIndexFunc;

#define GL_INDEX_TEST_EXT                                           0x81B5
#define GL_INDEX_TEST_FUNC_EXT                                      0x81B6
#define GL_INDEX_TEST_REF_EXT                                       0x81B7

#define glIndexFuncEXT GTglIndexFuncEXT

GT_EXTERN void GTglIndexFuncEXT (GLenum eFunction, GLclampf fReference);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 096) GL_EXT_index_array_formats
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtIndexArrayFormats;

#define GL_IUI_V2F_EXT                                              0x81AD
#define GL_IUI_V3F_EXT                                              0x81AE
#define GL_IUI_N3F_V2F_EXT                                          0x81AF
#define GL_IUI_N3F_V3F_EXT                                          0x81B0
#define GL_T2F_IUI_V2F_EXT                                          0x81B1
#define GL_T2F_IUI_V3F_EXT                                          0x81B2
#define GL_T2F_IUI_N3F_V2F_EXT                                      0x81B3
#define GL_T2F_IUI_N3F_V3F_EXT                                      0x81B4
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 097) GL_EXT_compiled_vertex_array
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtCompiledVertexArray;

#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT                             0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT                             0x81A9

#define glLockArraysEXT GTglLockArraysEXT
#define glUnlockArraysEXT GTglUnlockArraysEXT

GT_EXTERN void GTglLockArraysEXT (GLint iFirst, GLsizei iCount);
GT_EXTERN void GTglUnlockArraysEXT ();
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 098) GL_EXT_cull_vertex
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtCullVertex;

#define GL_CULL_VERTEX_EXT                                          0x81AA
#define GL_CULL_VERTEX_EYE_POSITION_EXT                             0x81AB
#define GL_CULL_VERTEX_OBJECT_POSITION_EXT                          0x81AC

#define glCullParameterdvEXT GTglCullParameterdvEXT
#define glCullParameterfvEXT GTglCullParameterfvEXT

GT_EXTERN void GTglCullParameterdvEXT (GLenum ePName, GLdouble* adParam);
GT_EXTERN void GTglCullParameterfvEXT (GLenum ePName, GLfloat* afParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 112) GL_EXT_draw_range_elements
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtDrawRangeElements;

#define GL_MAX_ELEMENTS_VERTICES_EXT                                0x80E8
#define GL_MAX_ELEMENTS_INDICES_EXT                                 0x80E9

#define glDrawRangeElementsEXT GTglDrawRangeElementsEXT

GT_EXTERN void GTglDrawRangeElementsEXT (GLenum eMode, GLuint uiStart,
    GLuint uiEnd, GLsizei iCount, GLenum eType, const GLvoid* pvIndices);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 117) GL_EXT_light_texture
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtLightTexture;

#define GL_FRAGMENT_MATERIAL_EXT                                    0x8349
#define GL_FRAGMENT_NORMAL_EXT                                      0x834A
#define GL_FRAGMENT_COLOR_EXT                                       0x834C
#define GL_ATTENUATION_EXT                                          0x834D
#define GL_SHADOW_ATTENUATION_EXT                                   0x834E
#define GL_TEXTURE_APPLICATION_MODE_EXT                             0x834F
#define GL_TEXTURE_LIGHT_EXT                                        0x8350
#define GL_TEXTURE_MATERIAL_FACE_EXT                                0x8351
#define GL_TEXTURE_MATERIAL_PARAMETER_EXT                           0x8352

#define glApplyTextureEXT GTglApplyTextureEXT
#define glTextureLightEXT GTglTextureLightEXT
#define glTextureMaterialEXT GTglTextureMaterialEXT

GT_EXTERN void GTglApplyTextureEXT (GLenum eMode);
GT_EXTERN void GTglTextureLightEXT (GLenum ePName);
GT_EXTERN void GTglTextureMaterialEXT (GLenum eFace, GLenum eMode);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 129) GL_EXT_bgra
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtBGRA;

#define GL_BGR_EXT                                                  0x80E0
#define GL_BGRA_EXT                                                 0x80E1
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 138) GL_EXT_pixel_transform
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPixelTransform;

#define GL_PIXEL_TRANSFORM_2D_EXT                                   0x8330
#define GL_PIXEL_MAG_FILTER_EXT                                     0x8331
#define GL_PIXEL_MIN_FILTER_EXT                                     0x8332
#define GL_PIXEL_CUBIC_WEIGHT_EXT                                   0x8333
#define GL_CUBIC_EXT                                                0x8334
#define GL_AVERAGE_EXT                                              0x8335
#define GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT                       0x8336
#define GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT                   0x8337
#define GL_PIXEL_TRANSFORM_2D_MATRIX_EXT                            0x8338

#define glPixelTransformParameteriEXT GTglPixelTransformParameteriEXT
#define glPixelTransformParameterfEXT GTglPixelTransformParameterfEXT
#define glPixelTransformParameterivEXT GTglPixelTransformParameterivEXT
#define glPixelTransformParameterfvEXT GTglPixelTransformParameterfvEXT

GT_EXTERN void GTglPixelTransformParameteriEXT (GLenum eTarget, GLenum ePName,
    GLint iParam);
GT_EXTERN void GTglPixelTransformParameterfEXT (GLenum eTarget, GLenum ePName,
    GLfloat fParam);
GT_EXTERN void GTglPixelTransformParameterivEXT (GLenum eTarget,
    GLenum ePName, const GLint* aiParam);
GT_EXTERN void GTglPixelTransformParameterfvEXT (GLenum eTarget,
    GLenum ePName, const GLfloat* afParam);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 139) GL_EXT_pixel_transform_color_table
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPixelTransformColorTable;

// The specification document mentions
//     GL_PIXEL_TRANSFORM_COLOR_TABLE_EXT
//     GL_PROXY_PIXEL_TRANSFORM_COLOR_TABLE_EXT
// but has no values assigned to them.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 141) GL_EXT_shared_texture_palette
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtSharedTexturePalette;

#define GL_SHARED_TEXTURE_PALETTE_EXT                               0x81FB
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 144) GL_EXT_separate_specular_color
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtSeparateSpecularColor;

#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT                            0x81F8
#define GL_SINGLE_COLOR_EXT                                         0x81F9
#define GL_SEPARATE_SPECULAR_COLOR_EXT                              0x81FA
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 145) GL_EXT_secondary_color
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtSecondaryColor;

#define GL_COLOR_SUM_EXT                                            0x8458
#define GL_CURRENT_SECONDARY_COLOR_EXT                              0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT                           0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT                           0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT                         0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT                        0x845D
#define GL_SECONDARY_COLOR_ARRAY_EXT                                0x845E

#define glSecondaryColor3bEXT GTglSecondaryColor3bEXT
#define glSecondaryColor3bvEXT GTglSecondaryColor3bvEXT
#define glSecondaryColor3dEXT GTglSecondaryColor3dEXT
#define glSecondaryColor3dvEXT GTglSecondaryColor3dvEXT
#define glSecondaryColor3fEXT GTglSecondaryColor3fEXT
#define glSecondaryColor3fvEXT GTglSecondaryColor3fvEXT
#define glSecondaryColor3iEXT GTglSecondaryColor3iEXT
#define glSecondaryColor3ivEXT GTglSecondaryColor3ivEXT
#define glSecondaryColor3sEXT GTglSecondaryColor3sEXT
#define glSecondaryColor3svEXT GTglSecondaryColor3svEXT
#define glSecondaryColor3ubEXT GTglSecondaryColor3ubEXT
#define glSecondaryColor3ubvEXT GTglSecondaryColor3ubvEXT
#define glSecondaryColor3uiEXT GTglSecondaryColor3uiEXT
#define glSecondaryColor3uivEXT GTglSecondaryColor3uivEXT
#define glSecondaryColor3usEXT GTglSecondaryColor3usEXT
#define glSecondaryColor3usvEXT GTglSecondaryColor3usvEXT
#define glSecondaryColorPointerEXT GTglSecondaryColorPointerEXT

GT_EXTERN void GTglSecondaryColor3bEXT (GLbyte cR, GLbyte cG, GLbyte cB);
GT_EXTERN void GTglSecondaryColor3bvEXT (const GLbyte* acRGB);
GT_EXTERN void GTglSecondaryColor3dEXT (GLdouble dR, GLdouble dG,
    GLdouble dB);
GT_EXTERN void GTglSecondaryColor3dvEXT (const GLdouble* adRGB);
GT_EXTERN void GTglSecondaryColor3fEXT (GLfloat fR, GLfloat fG, GLfloat fB);
GT_EXTERN void GTglSecondaryColor3fvEXT (const GLfloat* afRGB);
GT_EXTERN void GTglSecondaryColor3iEXT (GLint iR, GLint iG, GLint iB);
GT_EXTERN void GTglSecondaryColor3ivEXT (const GLint* aiRGB);
GT_EXTERN void GTglSecondaryColor3sEXT (GLshort sR, GLshort sG, GLshort sB);
GT_EXTERN void GTglSecondaryColor3svEXT (const GLshort* asRGB);
GT_EXTERN void GTglSecondaryColor3ubEXT (GLubyte ucR, GLubyte ucG,
    GLubyte ucB);
GT_EXTERN void GTglSecondaryColor3ubvEXT (const GLubyte* aucRGB);
GT_EXTERN void GTglSecondaryColor3uiEXT (GLuint uiR, GLuint uiG, GLuint uiB);
GT_EXTERN void GTglSecondaryColor3uivEXT (const GLuint* auiRGB);
GT_EXTERN void GTglSecondaryColor3usEXT (GLushort usR, GLushort usG,
    GLushort usB);
GT_EXTERN void GTglSecondaryColor3usvEXT (const GLushort* ausRGB);
GT_EXTERN void GTglSecondaryColorPointerEXT (GLint iSize, GLenum eType,
    GLsizei iStride, const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 147) GL_EXT_texture_perturb_normal
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTexturePerturbNormal;

#define GL_PERTURB_EXT                    0x85AE
#define GL_TEXTURE_NORMAL_EXT             0x85AF

#define glTextureNormalEXT GTglTextureNormalEXT

GT_EXTERN void GTglTextureNormalEXT (GLenum eMode);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 148) GL_EXT_multi_draw_arrays
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtMultidrawArrays;

#define glMultiDrawArraysEXT GTglMultiDrawArraysEXT
#define glMultiDrawElementsEXT GTglMultiDrawElementsEXT

GT_EXTERN void GTglMultiDrawArraysEXT (GLenum eMode, GLint* aiFirst,
    GLsizei* aiCount, GLsizei iPrimitiveCount);
GT_EXTERN void GTglMultiDrawElementsEXT (GLenum eMode, const GLsizei* aiCount,
    GLenum eType, const GLvoid** apvIndices, GLsizei iPrimitiveCount);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 149) GL_EXT_fog_coord
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtFogCoord;

#define GL_FOG_COORDINATE_SOURCE_EXT                                0x8450
#define GL_FOG_COORDINATE_EXT                                       0x8451
#define GL_FRAGMENT_DEPTH_EXT                                       0x8452
#define GL_CURRENT_FOG_COORDINATE_EXT                               0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT                            0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT                          0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT                         0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT                                 0x8457

#define glFogCoorddEXT GTglFogCoorddEXT
#define glFogCoorddvEXT GTglFogCoorddvEXT
#define glFogCoordfEXT GTglFogCoordfEXT
#define glFogCoordfvEXT GTglFogCoordfvEXT
#define glFogCoordPointerEXT GTglFogCoordPointerEXT

GT_EXTERN void GTglFogCoorddEXT (GLdouble dCoord);
GT_EXTERN void GTglFogCoorddvEXT (const GLdouble *adCoord);
GT_EXTERN void GTglFogCoordfEXT (GLfloat fCoord);
GT_EXTERN void GTglFogCoordfvEXT (const GLfloat *afCoord);
GT_EXTERN void GTglFogCoordPointerEXT (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 156) GL_EXT_coordinate_frame
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtCoordinateFrame;

#define GL_TANGENT_ARRAY_EXT                                        0x8439
#define GL_BINORMAL_ARRAY_EXT                                       0x843A
#define GL_CURRENT_TANGENT_EXT                                      0x843B
#define GL_CURRENT_BINORMAL_EXT                                     0x843C
#define GL_TANGENT_ARRAY_TYPE_EXT                                   0x843E
#define GL_TANGENT_ARRAY_STRIDE_EXT                                 0x843F
#define GL_BINORMAL_ARRAY_TYPE_EXT                                  0x8440
#define GL_BINORMAL_ARRAY_STRIDE_EXT                                0x8441
#define GL_TANGENT_ARRAY_POINTER_EXT                                0x8442
#define GL_BINORMAL_ARRAY_POINTER_EXT                               0x8443
#define GL_MAP1_TANGENT_EXT                                         0x8444
#define GL_MAP2_TANGENT_EXT                                         0x8445
#define GL_MAP1_BINORMAL_EXT                                        0x8446
#define GL_MAP2_BINORMAL_EXT                                        0x8447

#define glTangent3bEXT GTglTangent3bEXT
#define glTangent3bvEXT GTglTangent3bvEXT
#define glTangent3dEXT GTglTangent3dEXT
#define glTangent3dvEXT GTglTangent3dvEXT
#define glTangent3fEXT GTglTangent3fEXT
#define glTangent3fvEXT GTglTangent3fvEXT
#define glTangent3iEXT GTglTangent3iEXT
#define glTangent3ivEXT GTglTangent3ivEXT
#define glTangent3sEXT GTglTangent3sEXT
#define glTangent3svEXT GTglTangent3svEXT
#define glBinormal3bEXT GTglBinormal3bEXT
#define glBinormal3bvEXT GTglBinormal3bvEXT
#define glBinormal3dEXT GTglBinormal3dEXT
#define glBinormal3dvEXT GTglBinormal3dvEXT
#define glBinormal3fEXT GTglBinormal3fEXT
#define glBinormal3fvEXT GTglBinormal3fvEXT
#define glBinormal3iEXT GTglBinormal3iEXT
#define glBinormal3ivEXT GTglBinormal3ivEXT
#define glBinormal3sEXT GTglBinormal3sEXT
#define glBinormal3svEXT GTglBinormal3svEXT
#define glTangentPointerEXT GTglTangentPointerEXT
#define glBinormalPointerEXT GTglBinormalPointerEXT

GT_EXTERN void GTglTangent3bEXT (GLbyte cX, GLbyte cY, GLbyte cZ);
GT_EXTERN void GTglTangent3bvEXT (const GLbyte* acXYZ);
GT_EXTERN void GTglTangent3dEXT (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglTangent3dvEXT (const GLdouble* adXYZ);
GT_EXTERN void GTglTangent3fEXT (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglTangent3fvEXT (const GLfloat* afXYZ);
GT_EXTERN void GTglTangent3iEXT (GLint iX, GLint iY, GLint iZ);
GT_EXTERN void GTglTangent3ivEXT (const GLint* aiXYZ);
GT_EXTERN void GTglTangent3sEXT (GLshort sX, GLshort sY, GLshort sZ);
GT_EXTERN void GTglTangent3svEXT (const GLshort* asXYZ);
GT_EXTERN void GTglBinormal3bEXT (GLbyte cX, GLbyte cY, GLbyte cZ);
GT_EXTERN void GTglBinormal3bvEXT (const GLbyte* acXYZ);
GT_EXTERN void GTglBinormal3dEXT (GLdouble dX, GLdouble dY, GLdouble dZ);
GT_EXTERN void GTglBinormal3dvEXT (const GLdouble* adXYZ);
GT_EXTERN void GTglBinormal3fEXT (GLfloat fX, GLfloat fY, GLfloat fZ);
GT_EXTERN void GTglBinormal3fvEXT (const GLfloat* afXYZ);
GT_EXTERN void GTglBinormal3iEXT (GLint iX, GLint iY, GLint iZ);
GT_EXTERN void GTglBinormal3ivEXT (const GLint* aiXYZ);
GT_EXTERN void GTglBinormal3sEXT (GLshort sX, GLshort sY, GLshort sZ);
GT_EXTERN void GTglBinormal3svEXT (const GLshort* asXYZ);
GT_EXTERN void GTglTangentPointerEXT (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
GT_EXTERN void GTglBinormalPointerEXT (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 158) GL_EXT_texture_env_combine
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureEnvCombine;

#define GL_COMBINE_EXT                                              0x8570
#define GL_COMBINE_RGB_EXT                                          0x8571
#define GL_COMBINE_ALPHA_EXT                                        0x8572
#define GL_RGB_SCALE_EXT                                            0x8573
#define GL_ADD_SIGNED_EXT                                           0x8574
#define GL_INTERPOLATE_EXT                                          0x8575
#define GL_CONSTANT_EXT                                             0x8576
#define GL_PRIMARY_COLOR_EXT                                        0x8577
#define GL_PREVIOUS_EXT                                             0x8578
#define GL_SOURCE0_RGB_EXT                                          0x8580
#define GL_SOURCE1_RGB_EXT                                          0x8581
#define GL_SOURCE2_RGB_EXT                                          0x8582
#define GL_SOURCE0_ALPHA_EXT                                        0x8588
#define GL_SOURCE1_ALPHA_EXT                                        0x8589
#define GL_SOURCE2_ALPHA_EXT                                        0x858A
#define GL_OPERAND0_RGB_EXT                                         0x8590
#define GL_OPERAND1_RGB_EXT                                         0x8591
#define GL_OPERAND2_RGB_EXT                                         0x8592
#define GL_OPERAND0_ALPHA_EXT                                       0x8598
#define GL_OPERAND1_ALPHA_EXT                                       0x8599
#define GL_OPERAND2_ALPHA_EXT                                       0x859A
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 173) GL_EXT_blend_func_separate
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtBlendFuncSeparate;

#define GL_BLEND_DST_RGB_EXT                                        0x80C8
#define GL_BLEND_SRC_RGB_EXT                                        0x80C9
#define GL_BLEND_DST_ALPHA_EXT                                      0x80CA
#define GL_BLEND_SRC_ALPHA_EXT                                      0x80CB

#define glBlendFuncSeparateEXT GTglBlendFuncSeparateEXT

GT_EXTERN void GTglBlendFuncSeparateEXT (GLenum eSrcFactorRGB,
    GLenum eDstFactorRGB, GLenum eSrcFactorA, GLenum eDstFactorA);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 176) GL_EXT_stencil_wrap
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtStencilWrap;

#define GL_INCR_WRAP_EXT                                            0x8507
#define GL_DECR_WRAP_EXT                                            0x8508
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 178) GL_EXT_422_pixels
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExt422Pixels;

#define GL_422_EXT                                                  0x80CC
#define GL_422_REV_EXT                                              0x80CD
#define GL_422_AVERAGE_EXT                                          0x80CE
#define GL_422_REV_AVERAGE_EXT                                      0x80CF
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 185) GL_EXT_texture_env_add
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureEnvAdd;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 186) GL_EXT_texture_lod_bias
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureLodBias;

#define GL_MAX_TEXTURE_LOD_BIAS_EXT                                 0x84FD
#define GL_TEXTURE_FILTER_CONTROL_EXT                               0x8500
#define GL_TEXTURE_LOD_BIAS_EXT                                     0x8501
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 187) GL_EXT_texture_filter_anisotropic
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureFilterAnisotropic;

#define GL_TEXTURE_MAX_ANISOTROPY_EXT                               0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT                           0x84FF
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 188) GL_EXT_vertex_weighting
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtVertexWeighting;

#define GL_MODELVIEW0_STACK_DEPTH_EXT                               0x0BA3
#define GL_MODELVIEW1_STACK_DEPTH_EXT                               0x8502
#define GL_MODELVIEW0_MATRIX_EXT                                    0x0BA6
#define GL_MODELVIEW1_MATRIX_EXT                                    0x8506
#define GL_VERTEX_WEIGHTING_EXT                                     0x8509
#define GL_MODELVIEW0_EXT                                           0x1700
#define GL_MODELVIEW1_EXT                                           0x850A
#define GL_CURRENT_VERTEX_WEIGHT_EXT                                0x850B
#define GL_VERTEX_WEIGHT_ARRAY_EXT                                  0x850C
#define GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT                             0x850D
#define GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT                             0x850E
#define GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT                           0x850F
#define GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT                          0x8510

#define glVertexWeightfEXT GTglVertexWeightfEXT
#define glVertexWeightfvEXT GTglVertexWeightfvEXT
#define glVertexWeightPointerEXT GTglVertexWeightPointerEXT

GT_EXTERN void GTglVertexWeightfEXT (GLfloat fWeight);
GT_EXTERN void GTglVertexWeightfvEXT (const GLfloat* afWeight);
GT_EXTERN void GTglVertexWeightPointerEXT (GLsizei iSize, GLenum eType,
    GLsizei iStride, const GLvoid* pvPointer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 198) GL_EXT_texture_compression_s3tc
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureCompressionS3TC;

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT                             0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT                            0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT                            0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT                            0x83F3
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 209) GL_EXT_multisample
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtMultisample;

#define GL_MULTISAMPLE_EXT                                          0x809D
#define GL_SAMPLE_ALPHA_TO_MASK_EXT                                 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_EXT                                  0x809F
#define GL_SAMPLE_MASK_EXT                                          0x80A0
#define GL_1PASS_EXT                                                0x80A1
#define GL_2PASS_0_EXT                                              0x80A2
#define GL_2PASS_1_EXT                                              0x80A3
#define GL_4PASS_0_EXT                                              0x80A4
#define GL_4PASS_1_EXT                                              0x80A5
#define GL_4PASS_2_EXT                                              0x80A6
#define GL_4PASS_3_EXT                                              0x80A7
#define GL_SAMPLE_BUFFERS_EXT                                       0x80A8
#define GL_SAMPLES_EXT                                              0x80A9
#define GL_SAMPLE_MASK_VALUE_EXT                                    0x80AA
#define GL_SAMPLE_MASK_INVERT_EXT                                   0x80AB
#define GL_SAMPLE_PATTERN_EXT                                       0x80AC
#define GL_MULTISAMPLE_BIT_EXT                                      0x20000000

#define glSampleMaskEXT GTglSampleMaskEXT
#define glSamplePatternEXT GTglSamplePatternEXT

GT_EXTERN void GTglSampleMaskEXT (GLclampf fValue, GLboolean ucInvert);
GT_EXTERN void GTglSamplePatternEXT (GLenum ePattern);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 220) GL_EXT_texture_env_dot3
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureEnvDot3;

#define GL_DOT3_RGB_EXT                                             0x8740
#define GL_DOT3_RGBA_EXT                                            0x8741
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 267) GL_EXT_shadow_funcs
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtShadowFuncs;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 268) GL_EXT_stencil_two_side
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtStencilTwoSide;

#define GL_STENCIL_TEST_TWO_SIDE_EXT                                0x8910
#define GL_ACTIVE_STENCIL_FACE_EXT                                  0x8911

#define glActiveStencilFaceEXT GTglActiveStencilFaceEXT

GT_EXTERN void GTglActiveStencilFaceEXT (GLenum eFace);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 297) GL_EXT_depth_bounds_test
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtDepthBoundsTest;

#define GL_DEPTH_BOUNDS_TEST_EXT                                    0x8890
#define GL_DEPTH_BOUNDS_EXT                                         0x8891

#define glDepthBoundsEXT GTglDepthBoundsEXT

GT_EXTERN void GTglDepthBoundsEXT (GLclampd dMin, GLclampd dMax);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 298) GL_EXT_texture_mirror_clamp
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureMirrorClamp;

#define GL_MIRROR_CLAMP_EXT                                         0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_EXT                                 0x8743
#define GL_MIRROR_CLAMP_TO_BORDER_EXT                               0x8912
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 299) GL_EXT_blend_equation_separate
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtBlendEquationSeparate;

#define GL_BLEND_EQUATION_RGB_EXT                                   0x8009
#define GL_BLEND_EQUATION_ALPHA_EXT                                 0x883D

#define glBlendEquationSeparateEXT GTglBlendEquationSeparateEXT

GT_EXTERN void GTglBlendEquationSeparateEXT (GLenum eModeRGB,
    GLenum eModeAlpha);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 302) GL_EXT_pixel_buffer_object
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPixelBufferObject;

#define GL_PIXEL_PACK_BUFFER_EXT                                    0x88EB
#define GL_PIXEL_UNPACK_BUFFER_EXT                                  0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING_EXT                            0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING_EXT                          0x88EF
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 309) GL_EXT_texture_compression_dxt1
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureCompressionDXT1;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 310) GL_EXT_framebuffer_object
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtFrameBufferObject;

#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT                        0x0506
#define GL_MAX_RENDERBUFFER_SIZE_EXT                                0x84E8
#define GL_FRAMEBUFFER_BINDING_EXT                                  0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT                                 0x8CA7
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT                   0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT                   0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT                 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT         0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT            0x8CD4
#define GL_FRAMEBUFFER_COMPLETE_EXT                                 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENTS_EXT                   0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT            0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT          0x8CD8
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT                    0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                       0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT                   0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT                   0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                              0x8CDD
#define GL_FRAMEBUFFER_STATUS_ERROR_EXT                             0x8CDE
#define GL_MAX_COLOR_ATTACHMENTS_EXT                                0x8CDF
#define GL_COLOR_ATTACHMENT0_EXT                                    0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT                                    0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT                                    0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT                                    0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT                                    0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT                                    0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT                                    0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT                                    0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT                                    0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT                                    0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT                                   0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT                                   0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT                                   0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT                                   0x8CED
#define GL_COLOR_ATTACHMENT14_EXT                                   0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT                                   0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT                                     0x8D00
#define GL_STENCIL_ATTACHMENT_EXT                                   0x8D20
#define GL_FRAMEBUFFER_EXT                                          0x8D40
#define GL_RENDERBUFFER_EXT                                         0x8D41
#define GL_RENDERBUFFER_WIDTH_EXT                                   0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT                                  0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT                         0x8D44
#define GL_STENCIL_INDEX_EXT                                        0x8D45
#define GL_STENCIL_INDEX1_EXT                                       0x8D46
#define GL_STENCIL_INDEX4_EXT                                       0x8D47
#define GL_STENCIL_INDEX8_EXT                                       0x8D48
#define GL_STENCIL_INDEX16_EXT                                      0x8D49

#define glIsRenderbufferEXT GTglIsRenderbufferEXT
#define glBindRenderbufferEXT GTglBindRenderbufferEXT
#define glDeleteRenderbuffersEXT GTglDeleteRenderbuffersEXT
#define glGenRenderbuffersEXT GTglGenRenderbuffersEXT
#define glRenderbufferStorageEXT GTglRenderbufferStorageEXT
#define glGetRenderbufferParameterivEXT GTglGetRenderbufferParameterivEXT
#define glIsFramebufferEXT GTglIsFramebufferEXT
#define glBindFramebufferEXT GTglBindFramebufferEXT
#define glDeleteFramebuffersEXT GTglDeleteFramebuffersEXT
#define glGenFramebuffersEXT GTglGenFramebuffersEXT
#define glCheckFramebufferStatusEXT GTglCheckFramebufferStatusEXT
#define glFramebufferTexture1DEXT GTglFramebufferTexture1DEXT
#define glFramebufferTexture2DEXT GTglFramebufferTexture2DEXT
#define glFramebufferTexture3DEXT GTglFramebufferTexture3DEXT
#define glFramebufferRenderbufferEXT GTglFramebufferRenderbufferEXT
#define glGetFramebufferAttachmentParameterivEXT\
    GTglGetFramebufferAttachmentParameterivEXT
#define glGenerateMipmapEXT GTglGenerateMipmapEXT

GT_EXTERN GLboolean GTglIsRenderbufferEXT (GLuint uiRenderBuffer);
GT_EXTERN void GTglBindRenderbufferEXT (GLenum eTarget,
    GLuint uiRenderBuffer);
GT_EXTERN void GTglDeleteRenderbuffersEXT (GLsizei iNumBuffers,
    const GLuint* auiRenderBuffer);
GT_EXTERN void GTglGenRenderbuffersEXT (GLsizei iNumBuffers,
    GLuint* auiRenderBuffer);
GT_EXTERN void GTglRenderbufferStorageEXT (GLenum eTarget,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight);
GT_EXTERN void GTglGetRenderbufferParameterivEXT (GLenum eTarget,
    GLenum ePName, GLint* aiParam);
GT_EXTERN GLboolean GTglIsFramebufferEXT (GLuint uiFrameBuffer);
GT_EXTERN void GTglBindFramebufferEXT (GLenum eTarget, GLuint uiFrameBuffer);
GT_EXTERN void GTglDeleteFramebuffersEXT (GLsizei iNumBuffers,
    const GLuint* auiFrameBuffer);
GT_EXTERN void GTglGenFramebuffersEXT (GLsizei iNumBuffers,
    GLuint* auiFrameBuffer);
GT_EXTERN GLenum GTglCheckFramebufferStatusEXT (GLenum eTarget);
GT_EXTERN void GTglFramebufferTexture1DEXT (GLenum eTarget,
    GLenum eAttachment, GLenum eTextureTarget, GLuint uiTexture,
    GLint iLevel);
GT_EXTERN void GTglFramebufferTexture2DEXT (GLenum eTarget,
    GLenum eAttachment, GLenum eTextureTarget, GLuint uiTexture,
    GLint iLevel);
GT_EXTERN void GTglFramebufferTexture3DEXT (GLenum eTarget,
    GLenum eAttachment, GLenum eTextureTarget, GLuint uiTexture, GLint iLevel,
    GLint iZOffset);
GT_EXTERN void GTglFramebufferRenderbufferEXT (GLenum eTarget,
    GLenum eAttachment, GLenum eRenderBufferTarget, GLuint uiRenderBuffer);
GT_EXTERN void GTglGetFramebufferAttachmentParameterivEXT (GLenum eTarget,
    GLenum eAttachment, GLenum ePName, GLint* aiParam);
GT_EXTERN void GTglGenerateMipmapEXT (GLenum eTarget);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 312) GL_EXT_packed_depth_stencil
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtPackedDepthStencil;

#define GL_DEPTH_STENCIL_EXT                                        0x84F9
#define GL_UNSIGNED_INT_24_8_EXT                                    0x84FA
#define GL_DEPTH24_STENCIL8_EXT                                     0x88F0
#define GL_TEXTURE_STENCIL_SIZE_EXT                                 0x88F1
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 314) GL_EXT_stencil_clear_tag
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtStencilClearTag;

// The specification file mentions the information here but none of the tokens
// or functions were found in the file glext.h (version 29).  I suppose it is
// possible that glext.h is not current relative to the extensions posted at
// the OpenGL extension registry.

#define GL_STENCIL_TAG_BITS_EXT                                     0x88F2
#define GL_STENCIL_CLEAR_TAG_VALUE_EXT                              0x88F3

#define glStencilClearTagEXT GTglStencilClearTagEXT

GT_EXTERN void GTglStencilClearTagEXT (GLsizei iStencilTagBits,
    GLuint uiStencilClearTag);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 315) GL_EXT_texture_sRGB
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtTextureSRGB;

#define GL_SRGB_EXT                                                 0x8C40
#define GL_SRGB8_EXT                                                0x8C41
#define GL_SRGB_ALPHA_EXT                                           0x8C42
#define GL_SRGB8_ALPHA8_EXT                                         0x8C43
#define GL_SLUMINANCE_ALPHA_EXT                                     0x8C44
#define GL_SLUMINANCE8_ALPHA8_EXT                                   0x8C45
#define GL_SLUMINANCE_EXT                                           0x8C46
#define GL_SLUMINANCE8_EXT                                          0x8C47
#define GL_COMPRESSED_SRGB_EXT                                      0x8C48
#define GL_COMPRESSED_SRGB_ALPHA_EXT                                0x8C49
#define GL_COMPRESSED_SLUMINANCE_EXT                                0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA_EXT                          0x8C4B
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT                            0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT                      0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT                      0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT                      0x8C4F
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 316) GL_EXT_framebuffer_blit
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtFrameBufferBlit;

// The specification file mentions the information here but none of the tokens
// or functions were found in the file glext.h (version 29).  I suppose it is
// possible that glext.h is not current relative to the extensions posted at
// the OpenGL extension registry.

#define GL_READ_FRAMEBUFFER_EXT                                     0x8CA8
#define GL_DRAW_FRAMEBUFFER_EXT                                     0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_EXT                             0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_EXT                             0x8CAA

#define glBlitFramebufferEXT GTglBlitFramebufferEXT

GT_EXTERN void GTglBlitFramebufferEXT (GLint iSrcX0, GLint iSrcY0,
    GLint iSrcX1, GLint iSrcY1, GLint iDstX0, GLint iDstY0, GLint iDstX1,
    GLint iDstY1, GLbitfield uiMask, GLenum eFilter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 317) GL_EXT_framebuffer_multisample
//----------------------------------------------------------------------------
GT_EXTERN int GTExistsGlExtFrameBufferMultisample;

// The specification file mentions the information here but none of the tokens
// or functions were found in the file glext.h (version 29).  I suppose it is
// possible that glext.h is not current relative to the extensions posted at
// the OpenGL extension registry.

#define GL_RENDERBUFFER_SAMPLES_EXT                                 0x8CAB

#define glRenderbufferStorageMultisampleEXT\
    GTglRenderbufferStorageMultisampleEXT

GT_EXTERN void GTglRenderbufferStorageMultisampleEXT (GLenum eTarget,
    GLsizei iSamples, GLenum eInternalFormat, GLsizei iWidth,
    GLsizei iHeight);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GLU
//----------------------------------------------------------------------------
#ifdef WIN32
// Microsoft Windows

#ifndef APIENTRY
#define GT_DEFINED_APIENTRY
#define APIENTRY __stdcall
#endif

#ifndef CALLBACK
#define GT_DEFINED_CALLBACK
#define CALLBACK __stdcall
#endif

#ifndef GLAPI
#define GT_DEFINED_GLAPI
#define GLAPI extern
#endif

#else
// Macintosh, Linux
#define APIENTRY
#define CALLBACK
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#ifdef GT_DEFINED_APIENTRY
#undef APIENTRY
#endif

#ifdef GT_DEFINED_CALLBACK
#undef CALLBACK
#endif

#ifdef GT_DEFINED_GLAPI
#undef GLAPI
#endif
//----------------------------------------------------------------------------

GT_EXTERN const char* GTGetVendorString ();
GT_EXTERN const char* GTGetRendererString ();
GT_EXTERN const char* GTGetVersionString ();
GT_EXTERN const char* GTGetExtensionString ();
GT_EXTERN void GTInitGL (const char* acInfoFile);

#include "Wm5GlUtility.h"

}  // extern "C"

#endif
