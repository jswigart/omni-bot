/*
===========================================================================

Wolfenstein: Enemy Territory GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Wolfenstein: Enemy Territory GPL Source Code (Wolf ET Source Code).  

Wolf ET Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolf ET Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolf ET Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Wolf: ET Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Wolf ET Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

/*
	Copyright:  (c) 1999 by Apple Computer, Inc., all rights reserved.
*/

#ifndef __glext_h_
#define __glext_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
**
** http://oss.sgi.com/projects/FreeB
**
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
**
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
**
** Additional Notice Provisions: This software was created using the
** OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
** not been independently verified as being compliant with the OpenGL(R)
** version 1.2.1 Specification.
*/

#define GL_APPLE_specular_vector            1
#define GL_APPLE_transform_hint             1
#define GL_APPLE_packed_pixels              1
#define GL_APPLE_client_storage             1
#define GL_APPLE_ycbcr_422                  1
#define GL_APPLE_texture_range              1
#define GL_APPLE_fence                      1
#define GL_APPLE_vertex_array_range         1
#define GL_APPLE_vertex_array_object        1
#define GL_APPLE_element_array              1
#define GL_APPLE_vertex_program_evaluators  1
#define GL_ARB_imaging                      1
#define GL_ARB_transpose_matrix             1
#define GL_ARB_multitexture                 1
#define GL_ARB_texture_env_add              1
#define GL_ARB_texture_env_combine          1
#define GL_ARB_texture_env_dot3             1
#define GL_ARB_texture_env_crossbar         1
#define GL_ARB_texture_cube_map             1
#define GL_ARB_texture_compression          1
#define GL_ARB_multisample                  1
#define GL_ARB_texture_border_clamp         1
#define GL_ARB_point_parameters             1
#define GL_ARB_vertex_program               1
#define GL_ARB_texture_mirrored_repeat      1
#define GL_ARB_depth_texture                1
#define GL_ARB_shadow                       1
#define GL_ARB_vertex_blend                 1
#define GL_EXT_clip_volume_hint             1
#define GL_EXT_rescale_normal               1
#define GL_EXT_blend_color                  1
#define GL_EXT_blend_minmax                 1
#define GL_EXT_blend_subtract               1
#define GL_EXT_compiled_vertex_array        1
#define GL_EXT_texture_lod_bias             1
#define GL_EXT_texture_env_add              1
#define GL_EXT_abgr                         1
#define GL_EXT_bgra                         1
#define GL_EXT_texture_filter_anisotropic   1
#define GL_EXT_paletted_texture             1
#define GL_EXT_shared_texture_palette       1
#define GL_EXT_secondary_color              1
#define GL_EXT_texture_compression_s3tc     1
#define GL_EXT_texture_rectangle            1
#define GL_EXT_fog_coord                    1
#define GL_EXT_draw_range_elements          1
#define GL_EXT_stencil_wrap                 1
#define GL_EXT_blend_func_separate          1
#define GL_SGIS_texture_edge_clamp          1
#define GL_SGIS_generate_mipmap             1
#define GL_SGIS_texture_lod                 1
#define GL_ATI_point_cull_mode              1
#define GL_ATI_texture_mirror_once          1
#define GL_ATIX_pn_triangles                1
#define GL_ATI_text_fragment_shader         1
#define GL_ATI_blend_equation_separate      1
#define GL_ATI_blend_weighted_minmax        1
#define GL_NV_point_sprite                  1
#define GL_NV_register_combiners            1
#define GL_NV_register_combiners2           1
#define GL_NV_blend_square                  1
#define GL_NV_fog_distance                  1
#define GL_NV_multisample_filter_hint       1
#define GL_NV_texgen_reflection             1
#define GL_NV_texture_shader                1
#define GL_NV_texture_shader2               1
#define GL_NV_texture_shader3               1
#define GL_NV_depth_clamp                   1
#define GL_IBM_rasterpos_clip               1

/*************************************************************/
#define GL_GLEXT_VERSION 7

#if GL_ARB_multitexture
#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4
#define GL_TEXTURE5_ARB                   0x84C5
#define GL_TEXTURE6_ARB                   0x84C6
#define GL_TEXTURE7_ARB                   0x84C7
#define GL_TEXTURE8_ARB                   0x84C8
#define GL_TEXTURE9_ARB                   0x84C9
#define GL_TEXTURE10_ARB                  0x84CA
#define GL_TEXTURE11_ARB                  0x84CB
#define GL_TEXTURE12_ARB                  0x84CC
#define GL_TEXTURE13_ARB                  0x84CD
#define GL_TEXTURE14_ARB                  0x84CE
#define GL_TEXTURE15_ARB                  0x84CF
#define GL_TEXTURE16_ARB                  0x84D0
#define GL_TEXTURE17_ARB                  0x84D1
#define GL_TEXTURE18_ARB                  0x84D2
#define GL_TEXTURE19_ARB                  0x84D3
#define GL_TEXTURE20_ARB                  0x84D4
#define GL_TEXTURE21_ARB                  0x84D5
#define GL_TEXTURE22_ARB                  0x84D6
#define GL_TEXTURE23_ARB                  0x84D7
#define GL_TEXTURE24_ARB                  0x84D8
#define GL_TEXTURE25_ARB                  0x84D9
#define GL_TEXTURE26_ARB                  0x84DA
#define GL_TEXTURE27_ARB                  0x84DB
#define GL_TEXTURE28_ARB                  0x84DC
#define GL_TEXTURE29_ARB                  0x84DD
#define GL_TEXTURE30_ARB                  0x84DE
#define GL_TEXTURE31_ARB                  0x84DF
#define GL_ACTIVE_TEXTURE_ARB             0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB      0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB          0x84E2
#endif

#if GL_ARB_transpose_matrix
#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB 0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB 0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB   0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX_ARB     0x84E6
#endif

#if GL_ARB_multisample
#define GL_MULTISAMPLE_ARB                0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB   0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_ARB        0x809F
#define GL_SAMPLE_COVERAGE_ARB            0x80A0
#define GL_SAMPLE_BUFFERS_ARB             0x80A8
#define GL_SAMPLES_ARB                    0x80A9
#define GL_SAMPLE_COVERAGE_VALUE_ARB      0x80AA
#define GL_SAMPLE_COVERAGE_INVERT_ARB     0x80AB
#define GL_MULTISAMPLE_BIT_ARB            0x20000000
#endif

#if GL_ARB_texture_cube_map
#define GL_NORMAL_MAP_ARB                 0x8511
#define GL_REFLECTION_MAP_ARB             0x8512
#define GL_TEXTURE_CUBE_MAP_ARB           0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB   0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB     0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB  0x851C
#endif

#if GL_ARB_texture_compression
#define GL_COMPRESSED_ALPHA_ARB           0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB       0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB 0x84EB
#define GL_COMPRESSED_INTENSITY_ARB       0x84EC
#define GL_COMPRESSED_RGB_ARB             0x84ED
#define GL_COMPRESSED_RGBA_ARB            0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB   0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB 0x86A0
#define GL_TEXTURE_COMPRESSED_ARB         0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3
#endif

#if GL_ARB_vertex_blend
#define GL_MAX_VERTEX_UNITS_ARB           0x86A4
#define GL_ACTIVE_VERTEX_UNITS_ARB        0x86A5
#define GL_WEIGHT_SUM_UNITY_ARB           0x86A6
#define GL_VERTEX_BLEND_ARB               0x86A7
#define GL_CURRENT_WEIGHT_ARB             0x86A8
#define GL_WEIGHT_ARRAY_TYPE_ARB          0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_ARB        0x86AA
#define GL_WEIGHT_ARRAY_SIZE_ARB          0x86AB
#define GL_WEIGHT_ARRAY_POINTER_ARB       0x86AC
#define GL_WEIGHT_ARRAY_ARB               0x86AD
#define GL_MODELVIEW0_ARB                 0x1700
#define GL_MODELVIEW1_ARB                 0x850A
#define GL_MODELVIEW2_ARB                 0x8722
#define GL_MODELVIEW3_ARB                 0x8723
#define GL_MODELVIEW4_ARB                 0x8724
#define GL_MODELVIEW5_ARB                 0x8725
#define GL_MODELVIEW6_ARB                 0x8726
#define GL_MODELVIEW7_ARB                 0x8727
#define GL_MODELVIEW8_ARB                 0x8728
#define GL_MODELVIEW9_ARB                 0x8729
#define GL_MODELVIEW10_ARB                0x872A
#define GL_MODELVIEW11_ARB                0x872B
#define GL_MODELVIEW12_ARB                0x872C
#define GL_MODELVIEW13_ARB                0x872D
#define GL_MODELVIEW14_ARB                0x872E
#define GL_MODELVIEW15_ARB                0x872F
#define GL_MODELVIEW16_ARB                0x8730
#define GL_MODELVIEW17_ARB                0x8731
#define GL_MODELVIEW18_ARB                0x8732
#define GL_MODELVIEW19_ARB                0x8733
#define GL_MODELVIEW20_ARB                0x8734
#define GL_MODELVIEW21_ARB                0x8735
#define GL_MODELVIEW22_ARB                0x8736
#define GL_MODELVIEW23_ARB                0x8737
#define GL_MODELVIEW24_ARB                0x8738
#define GL_MODELVIEW25_ARB                0x8739
#define GL_MODELVIEW26_ARB                0x873A
#define GL_MODELVIEW27_ARB                0x873B
#define GL_MODELVIEW28_ARB                0x873C
#define GL_MODELVIEW29_ARB                0x873D
#define GL_MODELVIEW30_ARB                0x873E
#define GL_MODELVIEW31_ARB                0x873F
#endif

#if GL_ARB_texture_border_clamp
#define GL_CLAMP_TO_BORDER_ARB           0x812D
#endif

#if GL_ARB_depth_texture
#define GL_DEPTH_COMPONENT16_ARB          0x81A5
#define GL_DEPTH_COMPONENT24_ARB          0x81A6
#define GL_DEPTH_COMPONENT32_ARB          0x81A7
#define GL_TEXTURE_DEPTH_SIZE_ARB         0x884A
#define GL_DEPTH_TEXTURE_MODE_ARB         0x884B
#endif

#if GL_ARB_shadow
#define GL_TEXTURE_COMPARE_MODE_ARB       0x884C
#define GL_TEXTURE_COMPARE_FUNC_ARB       0x884D
#define GL_COMPARE_R_TO_TEXTURE_ARB       0x884E
#endif

#if GL_ARB_shadow_ambient
#define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB 0x80BF
#endif

#if GL_EXT_abgr
#define GL_ABGR_EXT                       0x8000
#endif

#if GL_EXT_blend_color
#define GL_CONSTANT_COLOR_EXT             0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT   0x8002
#define GL_CONSTANT_ALPHA_EXT             0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT   0x8004
#define GL_BLEND_COLOR_EXT                0x8005
#endif

#if GL_EXT_polygon_offset
#define GL_POLYGON_OFFSET_EXT             0x8037
#define GL_POLYGON_OFFSET_FACTOR_EXT      0x8038
#define GL_POLYGON_OFFSET_BIAS_EXT        0x8039
#endif

#if GL_EXT_texture
#define GL_ALPHA4_EXT                     0x803B
#define GL_ALPHA8_EXT                     0x803C
#define GL_ALPHA12_EXT                    0x803D
#define GL_ALPHA16_EXT                    0x803E
#define GL_LUMINANCE4_EXT                 0x803F
#define GL_LUMINANCE8_EXT                 0x8040
#define GL_LUMINANCE12_EXT                0x8041
#define GL_LUMINANCE16_EXT                0x8042
#define GL_LUMINANCE4_ALPHA4_EXT          0x8043
#define GL_LUMINANCE6_ALPHA2_EXT          0x8044
#define GL_LUMINANCE8_ALPHA8_EXT          0x8045
#define GL_LUMINANCE12_ALPHA4_EXT         0x8046
#define GL_LUMINANCE12_ALPHA12_EXT        0x8047
#define GL_LUMINANCE16_ALPHA16_EXT        0x8048
#define GL_INTENSITY_EXT                  0x8049
#define GL_INTENSITY4_EXT                 0x804A
#define GL_INTENSITY8_EXT                 0x804B
#define GL_INTENSITY12_EXT                0x804C
#define GL_INTENSITY16_EXT                0x804D
#define GL_RGB2_EXT                       0x804E
#define GL_RGB4_EXT                       0x804F
#define GL_RGB5_EXT                       0x8050
#define GL_RGB8_EXT                       0x8051
#define GL_RGB10_EXT                      0x8052
#define GL_RGB12_EXT                      0x8053
#define GL_RGB16_EXT                      0x8054
#define GL_RGBA2_EXT                      0x8055
#define GL_RGBA4_EXT                      0x8056
#define GL_RGB5_A1_EXT                    0x8057
#define GL_RGBA8_EXT                      0x8058
#define GL_RGB10_A2_EXT                   0x8059
#define GL_RGBA12_EXT                     0x805A
#define GL_RGBA16_EXT                     0x805B
#define GL_TEXTURE_RED_SIZE_EXT           0x805C
#define GL_TEXTURE_GREEN_SIZE_EXT         0x805D
#define GL_TEXTURE_BLUE_SIZE_EXT          0x805E
#define GL_TEXTURE_ALPHA_SIZE_EXT         0x805F
#define GL_TEXTURE_LUMINANCE_SIZE_EXT     0x8060
#define GL_TEXTURE_INTENSITY_SIZE_EXT     0x8061
#define GL_REPLACE_EXT                    0x8062
#define GL_PROXY_TEXTURE_1D_EXT           0x8063
#define GL_PROXY_TEXTURE_2D_EXT           0x8064
#define GL_TEXTURE_TOO_LARGE_EXT          0x8065
#endif

#if GL_EXT_texture3D
#define GL_PACK_SKIP_IMAGES_EXT           0x806B
#define GL_PACK_IMAGE_HEIGHT_EXT          0x806C
#define GL_UNPACK_SKIP_IMAGES_EXT         0x806D
#define GL_UNPACK_IMAGE_HEIGHT_EXT        0x806E
#define GL_TEXTURE_3D_EXT                 0x806F
#define GL_PROXY_TEXTURE_3D_EXT           0x8070
#define GL_TEXTURE_DEPTH_EXT              0x8071
#define GL_TEXTURE_WRAP_R_EXT             0x8072
#define GL_MAX_3D_TEXTURE_SIZE_EXT        0x8073
#endif

#if GL_SGIS_texture_filter4
#define GL_FILTER4_SGIS                   0x8146
#define GL_TEXTURE_FILTER4_SIZE_SGIS      0x8147
#endif

#if GL_EXT_histogram
#define GL_HISTOGRAM_EXT                  0x8024
#define GL_PROXY_HISTOGRAM_EXT            0x8025
#define GL_HISTOGRAM_WIDTH_EXT            0x8026
#define GL_HISTOGRAM_FORMAT_EXT           0x8027
#define GL_HISTOGRAM_RED_SIZE_EXT         0x8028
#define GL_HISTOGRAM_GREEN_SIZE_EXT       0x8029
#define GL_HISTOGRAM_BLUE_SIZE_EXT        0x802A
#define GL_HISTOGRAM_ALPHA_SIZE_EXT       0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE_EXT   0x802C
#define GL_HISTOGRAM_SINK_EXT             0x802D
#define GL_MINMAX_EXT                     0x802E
#define GL_MINMAX_FORMAT_EXT              0x802F
#define GL_MINMAX_SINK_EXT                0x8030
#define GL_TABLE_TOO_LARGE_EXT            0x8031
#endif

#if GL_EXT_convolution
#define GL_CONVOLUTION_1D_EXT             0x8010
#define GL_CONVOLUTION_2D_EXT             0x8011
#define GL_SEPARABLE_2D_EXT               0x8012
#define GL_CONVOLUTION_BORDER_MODE_EXT    0x8013
#define GL_CONVOLUTION_FILTER_SCALE_EXT   0x8014
#define GL_CONVOLUTION_FILTER_BIAS_EXT    0x8015
#define GL_REDUCE_EXT                     0x8016
#define GL_CONVOLUTION_FORMAT_EXT         0x8017
#define GL_CONVOLUTION_WIDTH_EXT          0x8018
#define GL_CONVOLUTION_HEIGHT_EXT         0x8019
#define GL_MAX_CONVOLUTION_WIDTH_EXT      0x801A
#define GL_MAX_CONVOLUTION_HEIGHT_EXT     0x801B
#define GL_POST_CONVOLUTION_RED_SCALE_EXT 0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE_EXT 0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE_EXT 0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE_EXT 0x801F
#define GL_POST_CONVOLUTION_RED_BIAS_EXT  0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS_EXT 0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS_EXT 0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS_EXT 0x8023
#endif

#if GL_SGI_color_matrix
#define GL_COLOR_MATRIX_SGI               0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH_SGI   0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI 0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE_SGI 0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI 0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI 0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI 0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS_SGI 0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI 0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI 0x80BA
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI 0x80BB
#endif

#if GL_SGI_color_table
#define GL_COLOR_TABLE_SGI                0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE_SGI 0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI 0x80D2
#define GL_PROXY_COLOR_TABLE_SGI          0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI 0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI 0x80D5
#define GL_COLOR_TABLE_SCALE_SGI          0x80D6
#define GL_COLOR_TABLE_BIAS_SGI           0x80D7
#define GL_COLOR_TABLE_FORMAT_SGI         0x80D8
#define GL_COLOR_TABLE_WIDTH_SGI          0x80D9
#define GL_COLOR_TABLE_RED_SIZE_SGI       0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_SGI     0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_SGI      0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_SGI     0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_SGI 0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_SGI 0x80DF
#endif

#if GL_SGIS_pixel_texture
#define GL_PIXEL_TEXTURE_SGIS             0x8353
#define GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS 0x8354
#define GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS 0x8355
#define GL_PIXEL_GROUP_COLOR_SGIS         0x8356
#endif

#if GL_SGIX_pixel_texture
#define GL_PIXEL_TEX_GEN_SGIX             0x8139
#define GL_PIXEL_TEX_GEN_MODE_SGIX        0x832B
#endif

#if GL_SGIS_texture4D
#define GL_PACK_SKIP_VOLUMES_SGIS         0x8130
#define GL_PACK_IMAGE_DEPTH_SGIS          0x8131
#define GL_UNPACK_SKIP_VOLUMES_SGIS       0x8132
#define GL_UNPACK_IMAGE_DEPTH_SGIS        0x8133
#define GL_TEXTURE_4D_SGIS                0x8134
#define GL_PROXY_TEXTURE_4D_SGIS          0x8135
#define GL_TEXTURE_4DSIZE_SGIS            0x8136
#define GL_TEXTURE_WRAP_Q_SGIS            0x8137
#define GL_MAX_4D_TEXTURE_SIZE_SGIS       0x8138
#define GL_TEXTURE_4D_BINDING_SGIS        0x814F
#endif

#if GL_SGI_texture_color_table
#define GL_TEXTURE_COLOR_TABLE_SGI        0x80BC
#define GL_PROXY_TEXTURE_COLOR_TABLE_SGI  0x80BD
#endif

#if GL_EXT_cmyka
#define GL_CMYK_EXT                       0x800C
#define GL_CMYKA_EXT                      0x800D
#define GL_PACK_CMYK_HINT_EXT             0x800E
#define GL_UNPACK_CMYK_HINT_EXT           0x800F
#endif

#if GL_EXT_texture_object
#define GL_TEXTURE_PRIORITY_EXT           0x8066
#define GL_TEXTURE_RESIDENT_EXT           0x8067
#define GL_TEXTURE_1D_BINDING_EXT         0x8068
#define GL_TEXTURE_2D_BINDING_EXT         0x8069
#define GL_TEXTURE_3D_BINDING_EXT         0x806A
#endif

#if GL_SGIS_detail_texture
#define GL_DETAIL_TEXTURE_2D_SGIS         0x8095
#define GL_DETAIL_TEXTURE_2D_BINDING_SGIS 0x8096
#define GL_LINEAR_DETAIL_SGIS             0x8097
#define GL_LINEAR_DETAIL_ALPHA_SGIS       0x8098
#define GL_LINEAR_DETAIL_COLOR_SGIS       0x8099
#define GL_DETAIL_TEXTURE_LEVEL_SGIS      0x809A
#define GL_DETAIL_TEXTURE_MODE_SGIS       0x809B
#define GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS 0x809C
#endif

#if GL_SGIS_sharpen_texture
#define GL_LINEAR_SHARPEN_SGIS            0x80AD
#define GL_LINEAR_SHARPEN_ALPHA_SGIS      0x80AE
#define GL_LINEAR_SHARPEN_COLOR_SGIS      0x80AF
#define GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS 0x80B0
#endif

#if GL_EXT_packed_pixels
#define GL_UNSIGNED_BYTE_3_3_2_EXT        0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT     0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT     0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT       0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT    0x8036
#endif

#if GL_SGIS_texture_lod
#define GL_TEXTURE_MIN_LOD_SGIS           0x813A
#define GL_TEXTURE_MAX_LOD_SGIS           0x813B
#define GL_TEXTURE_BASE_LEVEL_SGIS        0x813C
#define GL_TEXTURE_MAX_LEVEL_SGIS         0x813D
#endif

#if GL_SGIS_multisample
#define GL_MULTISAMPLE_SGIS               0x809D
#define GL_SAMPLE_ALPHA_TO_MASK_SGIS      0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_SGIS       0x809F
#define GL_SAMPLE_MASK_SGIS               0x80A0
#define GL_1PASS_SGIS                     0x80A1
#define GL_2PASS_0_SGIS                   0x80A2
#define GL_2PASS_1_SGIS                   0x80A3
#define GL_4PASS_0_SGIS                   0x80A4
#define GL_4PASS_1_SGIS                   0x80A5
#define GL_4PASS_2_SGIS                   0x80A6
#define GL_4PASS_3_SGIS                   0x80A7
#define GL_SAMPLE_BUFFERS_SGIS            0x80A8
#define GL_SAMPLES_SGIS                   0x80A9
#define GL_SAMPLE_MASK_VALUE_SGIS         0x80AA
#define GL_SAMPLE_MASK_INVERT_SGIS        0x80AB
#define GL_SAMPLE_PATTERN_SGIS            0x80AC
#endif

#if GL_EXT_rescale_normal
#define GL_RESCALE_NORMAL_EXT             0x803A
#endif

#if GL_EXT_vertex_array
#define GL_VERTEX_ARRAY_EXT               0x8074
#define GL_NORMAL_ARRAY_EXT               0x8075
#define GL_COLOR_ARRAY_EXT                0x8076
#define GL_INDEX_ARRAY_EXT                0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT        0x8078
#define GL_EDGE_FLAG_ARRAY_EXT            0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT          0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT          0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT        0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT         0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT          0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT        0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT         0x8080
#define GL_COLOR_ARRAY_SIZE_EXT           0x8081
#define GL_COLOR_ARRAY_TYPE_EXT           0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT         0x8083
#define GL_COLOR_ARRAY_COUNT_EXT          0x8084
#define GL_INDEX_ARRAY_TYPE_EXT           0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT         0x8086
#define GL_INDEX_ARRAY_COUNT_EXT          0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT   0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT   0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT 0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT  0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT     0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT      0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT       0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT       0x808F
#define GL_COLOR_ARRAY_POINTER_EXT        0x8090
#define GL_INDEX_ARRAY_POINTER_EXT        0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT    0x8093
#endif

#if GL_SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP_SGIS           0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS      0x8192
#endif

#if GL_SGIX_clipmap
#define GL_LINEAR_CLIPMAP_LINEAR_SGIX     0x8170
#define GL_TEXTURE_CLIPMAP_CENTER_SGIX    0x8171
#define GL_TEXTURE_CLIPMAP_FRAME_SGIX     0x8172
#define GL_TEXTURE_CLIPMAP_OFFSET_SGIX    0x8173
#define GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX 0x8174
#define GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX 0x8175
#define GL_TEXTURE_CLIPMAP_DEPTH_SGIX     0x8176
#define GL_MAX_CLIPMAP_DEPTH_SGIX         0x8177
#define GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX 0x8178
#define GL_NEAREST_CLIPMAP_NEAREST_SGIX   0x844D
#define GL_NEAREST_CLIPMAP_LINEAR_SGIX    0x844E
#define GL_LINEAR_CLIPMAP_NEAREST_SGIX    0x844F
#endif

#if GL_SGIX_shadow
#define GL_TEXTURE_COMPARE_SGIX           0x819A
#define GL_TEXTURE_COMPARE_OPERATOR_SGIX  0x819B
#define GL_TEXTURE_LEQUAL_R_SGIX          0x819C
#define GL_TEXTURE_GEQUAL_R_SGIX          0x819D
#endif

#if GL_SGIS_texture_edge_clamp
#define GL_CLAMP_TO_EDGE_SGIS             0x812F
#endif

#if GL_SGIS_texture_border_clamp
#define GL_CLAMP_TO_BORDER_SGIS           0x812D
#endif

#if GL_EXT_blend_minmax
#define GL_FUNC_ADD_EXT                   0x8006
#define GL_MIN_EXT                        0x8007
#define GL_MAX_EXT                        0x8008
#define GL_BLEND_EQUATION_EXT             0x8009
#endif

#if GL_ATI_blend_weighted_minmax
#define GL_MIN_WEIGHTED_ATI               0x877D
#define GL_MAX_WEIGHTED_ATI               0x877E
#endif

#if GL_EXT_blend_subtract
#define GL_FUNC_SUBTRACT_EXT              0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT      0x800B
#endif

#if GL_SGIX_interlace
#define GL_INTERLACE_SGIX                 0x8094
#endif

#if GL_SGIX_pixel_tiles
#define GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX 0x813E
#define GL_PIXEL_TILE_CACHE_INCREMENT_SGIX 0x813F
#define GL_PIXEL_TILE_WIDTH_SGIX          0x8140
#define GL_PIXEL_TILE_HEIGHT_SGIX         0x8141
#define GL_PIXEL_TILE_GRID_WIDTH_SGIX     0x8142
#define GL_PIXEL_TILE_GRID_HEIGHT_SGIX    0x8143
#define GL_PIXEL_TILE_GRID_DEPTH_SGIX     0x8144
#define GL_PIXEL_TILE_CACHE_SIZE_SGIX     0x8145
#endif

#if GL_SGIS_texture_select
#define GL_DUAL_ALPHA4_SGIS               0x8110
#define GL_DUAL_ALPHA8_SGIS               0x8111
#define GL_DUAL_ALPHA12_SGIS              0x8112
#define GL_DUAL_ALPHA16_SGIS              0x8113
#define GL_DUAL_LUMINANCE4_SGIS           0x8114
#define GL_DUAL_LUMINANCE8_SGIS           0x8115
#define GL_DUAL_LUMINANCE12_SGIS          0x8116
#define GL_DUAL_LUMINANCE16_SGIS          0x8117
#define GL_DUAL_INTENSITY4_SGIS           0x8118
#define GL_DUAL_INTENSITY8_SGIS           0x8119
#define GL_DUAL_INTENSITY12_SGIS          0x811A
#define GL_DUAL_INTENSITY16_SGIS          0x811B
#define GL_DUAL_LUMINANCE_ALPHA4_SGIS     0x811C
#define GL_DUAL_LUMINANCE_ALPHA8_SGIS     0x811D
#define GL_QUAD_ALPHA4_SGIS               0x811E
#define GL_QUAD_ALPHA8_SGIS               0x811F
#define GL_QUAD_LUMINANCE4_SGIS           0x8120
#define GL_QUAD_LUMINANCE8_SGIS           0x8121
#define GL_QUAD_INTENSITY4_SGIS           0x8122
#define GL_QUAD_INTENSITY8_SGIS           0x8123
#define GL_DUAL_TEXTURE_SELECT_SGIS       0x8124
#define GL_QUAD_TEXTURE_SELECT_SGIS       0x8125
#endif

#if GL_SGIX_sprite
#define GL_SPRITE_SGIX                    0x8148
#define GL_SPRITE_MODE_SGIX               0x8149
#define GL_SPRITE_AXIS_SGIX               0x814A
#define GL_SPRITE_TRANSLATION_SGIX        0x814B
#define GL_SPRITE_AXIAL_SGIX              0x814C
#define GL_SPRITE_OBJECT_ALIGNED_SGIX     0x814D
#define GL_SPRITE_EYE_ALIGNED_SGIX        0x814E
#endif

#if GL_SGIX_texture_multi_buffer
#define GL_TEXTURE_MULTI_BUFFER_HINT_SGIX 0x812E
#endif

#if GL_SGIS_point_parameters
#define GL_POINT_SIZE_MIN_EXT             0x8126
#define GL_POINT_SIZE_MIN_SGIS            0x8126
#define GL_POINT_SIZE_MAX_EXT             0x8127
#define GL_POINT_SIZE_MAX_SGIS            0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT  0x8128
#define GL_POINT_FADE_THRESHOLD_SIZE_SGIS 0x8128
#define GL_DISTANCE_ATTENUATION_EXT       0x8129
#define GL_DISTANCE_ATTENUATION_SGIS      0x8129
#endif

#if GL_SGIX_instruments
#define GL_INSTRUMENT_BUFFER_POINTER_SGIX 0x8180
#define GL_INSTRUMENT_MEASUREMENTS_SGIX   0x8181
#endif

#if GL_SGIX_texture_scale_bias
#define GL_POST_TEXTURE_FILTER_BIAS_SGIX  0x8179
#define GL_POST_TEXTURE_FILTER_SCALE_SGIX 0x817A
#define GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX 0x817B
#define GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX 0x817C
#endif

#if GL_SGIX_framezoom
#define GL_FRAMEZOOM_SGIX                 0x818B
#define GL_FRAMEZOOM_FACTOR_SGIX          0x818C
#define GL_MAX_FRAMEZOOM_FACTOR_SGIX      0x818D
#endif

#if GL_FfdMaskSGIX
#define GL_TEXTURE_DEFORMATION_BIT_SGIX   0x00000001
#define GL_GEOMETRY_DEFORMATION_BIT_SGIX  0x00000002
#endif

#if GL_SGIX_polynomial_ffd
#define GL_GEOMETRY_DEFORMATION_SGIX      0x8194
#define GL_TEXTURE_DEFORMATION_SGIX       0x8195
#define GL_DEFORMATIONS_MASK_SGIX         0x8196
#define GL_MAX_DEFORMATION_ORDER_SGIX     0x8197
#endif

#if GL_SGIX_reference_plane
#define GL_REFERENCE_PLANE_SGIX           0x817D
#define GL_REFERENCE_PLANE_EQUATION_SGIX  0x817E
#endif

#if GL_SGIX_depth_texture
#define GL_DEPTH_COMPONENT16_SGIX         0x81A5
#define GL_DEPTH_COMPONENT24_SGIX         0x81A6
#define GL_DEPTH_COMPONENT32_SGIX         0x81A7
#endif

#if GL_SGIS_fog_function
#define GL_FOG_FUNC_SGIS                  0x812A
#define GL_FOG_FUNC_POINTS_SGIS           0x812B
#define GL_MAX_FOG_FUNC_POINTS_SGIS       0x812C
#endif

#if GL_SGIX_fog_offset
#define GL_FOG_OFFSET_SGIX                0x8198
#define GL_FOG_OFFSET_VALUE_SGIX          0x8199
#endif

#if GL_HP_image_transform
#define GL_IMAGE_SCALE_X_HP               0x8155
#define GL_IMAGE_SCALE_Y_HP               0x8156
#define GL_IMAGE_TRANSLATE_X_HP           0x8157
#define GL_IMAGE_TRANSLATE_Y_HP           0x8158
#define GL_IMAGE_ROTATE_ANGLE_HP          0x8159
#define GL_IMAGE_ROTATE_ORIGIN_X_HP       0x815A
#define GL_IMAGE_ROTATE_ORIGIN_Y_HP       0x815B
#define GL_IMAGE_MAG_FILTER_HP            0x815C
#define GL_IMAGE_MIN_FILTER_HP            0x815D
#define GL_IMAGE_CUBIC_WEIGHT_HP          0x815E
#define GL_CUBIC_HP                       0x815F
#define GL_AVERAGE_HP                     0x8160
#define GL_IMAGE_TRANSFORM_2D_HP          0x8161
#define GL_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP 0x8162
#define GL_PROXY_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP 0x8163
#endif

#if GL_HP_convolution_border_modes
#define GL_IGNORE_BORDER_HP               0x8150
#define GL_CONSTANT_BORDER_HP             0x8151
#define GL_REPLICATE_BORDER_HP            0x8153
#define GL_CONVOLUTION_BORDER_COLOR_HP    0x8154
#endif

#if GL_SGIX_texture_add_env
#define GL_TEXTURE_ENV_BIAS_SGIX          0x80BE
#endif

#if GL_PGI_vertex_hints
#define GL_VERTEX_DATA_HINT_PGI           0x1A22A
#define GL_VERTEX_CONSISTENT_HINT_PGI     0x1A22B
#define GL_MATERIAL_SIDE_HINT_PGI         0x1A22C
#define GL_MAX_VERTEX_HINT_PGI            0x1A22D
#define GL_COLOR3_BIT_PGI                 0x00010000
#define GL_COLOR4_BIT_PGI                 0x00020000
#define GL_EDGEFLAG_BIT_PGI               0x00040000
#define GL_INDEX_BIT_PGI                  0x00080000
#define GL_MAT_AMBIENT_BIT_PGI            0x00100000
#define GL_MAT_AMBIENT_AND_DIFFUSE_BIT_PGI 0x00200000
#define GL_MAT_DIFFUSE_BIT_PGI            0x00400000
#define GL_MAT_EMISSION_BIT_PGI           0x00800000
#define GL_MAT_COLOR_INDEXES_BIT_PGI      0x01000000
#define GL_MAT_SHININESS_BIT_PGI          0x02000000
#define GL_MAT_SPECULAR_BIT_PGI           0x04000000
#define GL_NORMAL_BIT_PGI                 0x08000000
#define GL_TEXCOORD1_BIT_PGI              0x10000000
#define GL_TEXCOORD2_BIT_PGI              0x20000000
#define GL_TEXCOORD3_BIT_PGI              0x40000000
#define GL_TEXCOORD4_BIT_PGI              0x80000000
#define GL_VERTEX23_BIT_PGI               0x00000004
#define GL_VERTEX4_BIT_PGI                0x00000008
#endif

#if GL_PGI_misc_hints
#define GL_PREFER_DOUBLEBUFFER_HINT_PGI   0x1A1F8
#define GL_CONSERVE_MEMORY_HINT_PGI       0x1A1FD
#define GL_RECLAIM_MEMORY_HINT_PGI        0x1A1FE
#define GL_NATIVE_GRAPHICS_HANDLE_PGI     0x1A202
#define GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI 0x1A203
#define GL_NATIVE_GRAPHICS_END_HINT_PGI   0x1A204
#define GL_ALWAYS_FAST_HINT_PGI           0x1A20C
#define GL_ALWAYS_SOFT_HINT_PGI           0x1A20D
#define GL_ALLOW_DRAW_OBJ_HINT_PGI        0x1A20E
#define GL_ALLOW_DRAW_WIN_HINT_PGI        0x1A20F
#define GL_ALLOW_DRAW_FRG_HINT_PGI        0x1A210
#define GL_ALLOW_DRAW_MEM_HINT_PGI        0x1A211
#define GL_STRICT_DEPTHFUNC_HINT_PGI      0x1A216
#define GL_STRICT_LIGHTING_HINT_PGI       0x1A217
#define GL_STRICT_SCISSOR_HINT_PGI        0x1A218
#define GL_FULL_STIPPLE_HINT_PGI          0x1A219
#define GL_CLIP_NEAR_HINT_PGI             0x1A220
#define GL_CLIP_FAR_HINT_PGI              0x1A221
#define GL_WIDE_LINE_HINT_PGI             0x1A222
#define GL_BACK_NORMALS_HINT_PGI          0x1A223
#endif

#if GL_EXT_paletted_texture
#define GL_COLOR_INDEX1_EXT               0x80E2
#define GL_COLOR_INDEX2_EXT               0x80E3
#define GL_COLOR_INDEX4_EXT               0x80E4
#define GL_COLOR_INDEX8_EXT               0x80E5
#define GL_COLOR_INDEX12_EXT              0x80E6
#define GL_COLOR_INDEX16_EXT              0x80E7
#define GL_TEXTURE_INDEX_SIZE_EXT         0x80ED
#endif

#if GL_EXT_clip_volume_hint
#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT  0x80F0
#endif

#if GL_SGIX_list_priority
#define GL_LIST_PRIORITY_SGIX             0x8182
#endif

#if GL_SGIX_ir_instrument1
#define GL_IR_INSTRUMENT1_SGIX            0x817F
#endif

#if GL_SGIX_calligraphic_fragment
#define GL_CALLIGRAPHIC_FRAGMENT_SGIX     0x8183
#endif

#if GL_SGIX_texture_lod_bias
#define GL_TEXTURE_LOD_BIAS_S_SGIX        0x818E
#define GL_TEXTURE_LOD_BIAS_T_SGIX        0x818F
#define GL_TEXTURE_LOD_BIAS_R_SGIX        0x8190
#endif

#if GL_SGIX_shadow_ambient
#define GL_SHADOW_AMBIENT_SGIX            0x80BF
#endif

#if GL_EXT_index_material
#define GL_INDEX_MATERIAL_EXT             0x81B8
#define GL_INDEX_MATERIAL_PARAMETER_EXT   0x81B9
#define GL_INDEX_MATERIAL_FACE_EXT        0x81BA
#endif

#if GL_EXT_index_func
#define GL_INDEX_TEST_EXT                 0x81B5
#define GL_INDEX_TEST_FUNC_EXT            0x81B6
#define GL_INDEX_TEST_REF_EXT             0x81B7
#endif

#if GL_EXT_index_array_formats
#define GL_IUI_V2F_EXT                    0x81AD
#define GL_IUI_V3F_EXT                    0x81AE
#define GL_IUI_N3F_V2F_EXT                0x81AF
#define GL_IUI_N3F_V3F_EXT                0x81B0
#define GL_T2F_IUI_V2F_EXT                0x81B1
#define GL_T2F_IUI_V3F_EXT                0x81B2
#define GL_T2F_IUI_N3F_V2F_EXT            0x81B3
#define GL_T2F_IUI_N3F_V3F_EXT            0x81B4
#endif

#if GL_EXT_compiled_vertex_array
#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT   0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT   0x81A9
#endif

#if GL_EXT_cull_vertex
#define GL_CULL_VERTEX_EXT                0x81AA
#define GL_CULL_VERTEX_EYE_POSITION_EXT   0x81AB
#define GL_CULL_VERTEX_OBJECT_POSITION_EXT 0x81AC
#endif

#if GL_SGIX_ycrcb
#define GL_YCRCB_422_SGIX                 0x81BB
#define GL_YCRCB_444_SGIX                 0x81BC
#endif

#if GL_SGIX_fragment_lighting
#define GL_FRAGMENT_LIGHTING_SGIX         0x8400
#define GL_FRAGMENT_COLOR_MATERIAL_SGIX   0x8401
#define GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX 0x8402
#define GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX 0x8403
#define GL_MAX_FRAGMENT_LIGHTS_SGIX       0x8404
#define GL_MAX_ACTIVE_LIGHTS_SGIX         0x8405
#define GL_CURRENT_RASTER_NORMAL_SGIX     0x8406
#define GL_LIGHT_ENV_MODE_SGIX            0x8407
#define GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX 0x8408
#define GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX 0x8409
#define GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX 0x840A
#define GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX 0x840B
#define GL_FRAGMENT_LIGHT0_SGIX           0x840C
#define GL_FRAGMENT_LIGHT1_SGIX           0x840D
#define GL_FRAGMENT_LIGHT2_SGIX           0x840E
#define GL_FRAGMENT_LIGHT3_SGIX           0x840F
#define GL_FRAGMENT_LIGHT4_SGIX           0x8410
#define GL_FRAGMENT_LIGHT5_SGIX           0x8411
#define GL_FRAGMENT_LIGHT6_SGIX           0x8412
#define GL_FRAGMENT_LIGHT7_SGIX           0x8413
#endif

#if GL_IBM_rasterpos_clip
#define GL_RASTER_POSITION_UNCLIPPED_IBM  0x19262
#endif

#if GL_HP_texture_lighting
#define GL_TEXTURE_LIGHTING_MODE_HP       0x8167
#define GL_TEXTURE_POST_SPECULAR_HP       0x8168
#define GL_TEXTURE_PRE_SPECULAR_HP        0x8169
#endif

#if GL_EXT_draw_range_elements
#define GL_MAX_ELEMENTS_VERTICES_EXT      0x80E8
#define GL_MAX_ELEMENTS_INDICES_EXT       0x80E9
#endif

#if GL_WIN_phong_shading
#define GL_PHONG_WIN                      0x80EA
#define GL_PHONG_HINT_WIN                 0x80EB
#endif

#if GL_WIN_specular_fog
#define GL_FOG_SPECULAR_TEXTURE_WIN       0x80EC
#endif

#if GL_EXT_light_texture
#define GL_FRAGMENT_MATERIAL_EXT          0x8349
#define GL_FRAGMENT_NORMAL_EXT            0x834A
#define GL_FRAGMENT_COLOR_EXT             0x834C
#define GL_ATTENUATION_EXT                0x834D
#define GL_SHADOW_ATTENUATION_EXT         0x834E
#define GL_TEXTURE_APPLICATION_MODE_EXT   0x834F
#define GL_TEXTURE_LIGHT_EXT              0x8350
#define GL_TEXTURE_MATERIAL_FACE_EXT      0x8351
#define GL_TEXTURE_MATERIAL_PARAMETER_EXT 0x8352
/* reuse GL_FRAGMENT_DEPTH_EXT */
#endif

#if GL_SGIX_blend_alpha_minmax
#define GL_ALPHA_MIN_SGIX                 0x8320
#define GL_ALPHA_MAX_SGIX                 0x8321
#endif

#if GL_EXT_bgra
#define GL_BGR_EXT                        0x80E0
#define GL_BGRA_EXT                       0x80E1
#endif

#if GL_SGIX_async
#define GL_ASYNC_MARKER_SGIX              0x8329
#endif

#if GL_SGIX_async_pixel
#define GL_ASYNC_TEX_IMAGE_SGIX           0x835C
#define GL_ASYNC_DRAW_PIXELS_SGIX         0x835D
#define GL_ASYNC_READ_PIXELS_SGIX         0x835E
#define GL_MAX_ASYNC_TEX_IMAGE_SGIX       0x835F
#define GL_MAX_ASYNC_DRAW_PIXELS_SGIX     0x8360
#define GL_MAX_ASYNC_READ_PIXELS_SGIX     0x8361
#endif

#if GL_SGIX_async_histogram
#define GL_ASYNC_HISTOGRAM_SGIX           0x832C
#define GL_MAX_ASYNC_HISTOGRAM_SGIX       0x832D
#endif

#if GL_INTEL_parallel_arrays
#define GL_PARALLEL_ARRAYS_INTEL          0x83F4
#define GL_VERTEX_ARRAY_PARALLEL_POINTERS_INTEL 0x83F5
#define GL_NORMAL_ARRAY_PARALLEL_POINTERS_INTEL 0x83F6
#define GL_COLOR_ARRAY_PARALLEL_POINTERS_INTEL 0x83F7
#define GL_TEXTURE_COORD_ARRAY_PARALLEL_POINTERS_INTEL 0x83F8
#endif

#if GL_HP_occlusion_test
#define GL_OCCLUSION_TEST_HP              0x8165
#define GL_OCCLUSION_TEST_RESULT_HP       0x8166
#endif

#if GL_EXT_pixel_transform
#define GL_PIXEL_TRANSFORM_2D_EXT         0x8330
#define GL_PIXEL_MAG_FILTER_EXT           0x8331
#define GL_PIXEL_MIN_FILTER_EXT           0x8332
#define GL_PIXEL_CUBIC_WEIGHT_EXT         0x8333
#define GL_CUBIC_EXT                      0x8334
#define GL_AVERAGE_EXT                    0x8335
#define GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT 0x8336
#define GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT 0x8337
#define GL_PIXEL_TRANSFORM_2D_MATRIX_EXT  0x8338
#endif

#if GL_EXT_shared_texture_palette
#define GL_SHARED_TEXTURE_PALETTE_EXT     0x81FB
#endif

#if GL_EXT_separate_specular_color
#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT  0x81F8
#define GL_SINGLE_COLOR_EXT               0x81F9
#define GL_SEPARATE_SPECULAR_COLOR_EXT    0x81FA
#endif

#if GL_EXT_secondary_color
#define GL_COLOR_SUM_EXT                  0x8458
#define GL_CURRENT_SECONDARY_COLOR_EXT    0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT 0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT 0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT 0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT 0x845D
#define GL_SECONDARY_COLOR_ARRAY_EXT      0x845E
#endif

#if GL_EXT_texture_perturb_normal
#define GL_PERTURB_EXT                    0x85AE
#define GL_TEXTURE_NORMAL_EXT             0x85AF
#endif

#if GL_EXT_fog_coord
#define GL_FOG_COORDINATE_SOURCE_EXT      0x8450
#define GL_FOG_COORDINATE_EXT             0x8451
#define GL_FRAGMENT_DEPTH_EXT             0x8452
#define GL_CURRENT_FOG_COORDINATE_EXT     0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT  0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT 0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT 0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT       0x8457
#endif

#if GL_APPLE_vertex_array_range
#define GL_VERTEX_ARRAY_RANGE_APPLE             0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_APPLE      0x851E
#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_APPLE 0x8520
#define GL_VERTEX_ARRAY_RANGE_POINTER_APPLE     0x8521
#define GL_VERTEX_ARRAY_STORAGE_HINT_APPLE      0x851F
/* GL_STORAGE_PRIVATE_APPLE           0x85BD */
/* GL_STORAGE_CACHED_APPLE            0x85BE */
/* GL_STORAGE_SHARED_APPLE            0x85BF */
#endif

#if GL_APPLE_vertex_array_object
#define GL_VERTEX_ARRAY_BINDING_APPLE      0x85B5
#endif

#if GL_APPLE_element_array
#define GL_ELEMENT_ARRAY_APPLE             0x8A0C
#define GL_ELEMENT_ARRAY_TYPE_APPLE        0x8A0D
#define GL_ELEMENT_ARRAY_POINTER_APPLE     0x8A0E
#endif

#if GL_APPLE_fence
#define GL_DRAW_PIXELS_APPLE              0x8A0A
#define GL_FENCE_APPLE                    0x8A0B
#endif

#if GL_REND_screen_coordinates
#define GL_SCREEN_COORDINATES_REND        0x8490
#define GL_INVERTED_SCREEN_W_REND         0x8491
#endif

#if GL_EXT_coordinate_frame
#define GL_TANGENT_ARRAY_EXT              0x8439
#define GL_BINORMAL_ARRAY_EXT             0x843A
#define GL_CURRENT_TANGENT_EXT            0x843B
#define GL_CURRENT_BINORMAL_EXT           0x843C
#define GL_TANGENT_ARRAY_TYPE_EXT         0x843E
#define GL_TANGENT_ARRAY_STRIDE_EXT       0x843F
#define GL_BINORMAL_ARRAY_TYPE_EXT        0x8440
#define GL_BINORMAL_ARRAY_STRIDE_EXT      0x8441
#define GL_TANGENT_ARRAY_POINTER_EXT      0x8442
#define GL_BINORMAL_ARRAY_POINTER_EXT     0x8443
#define GL_MAP1_TANGENT_EXT               0x8444
#define GL_MAP2_TANGENT_EXT               0x8445
#define GL_MAP1_BINORMAL_EXT              0x8446
#define GL_MAP2_BINORMAL_EXT              0x8447
#endif

#if GL_EXT_texture_env_combine
#define GL_COMBINE_EXT                    0x8570
#define GL_COMBINE_RGB_EXT                0x8571
#define GL_COMBINE_ALPHA_EXT              0x8572
#define GL_RGB_SCALE_EXT                  0x8573
#define GL_ADD_SIGNED_EXT                 0x8574
#define GL_INTERPOLATE_EXT                0x8575
#define GL_CONSTANT_EXT                   0x8576
#define GL_PRIMARY_COLOR_EXT              0x8577
#define GL_PREVIOUS_EXT                   0x8578
#define GL_SOURCE0_RGB_EXT                0x8580
#define GL_SOURCE1_RGB_EXT                0x8581
#define GL_SOURCE2_RGB_EXT                0x8582
#define GL_SOURCE3_RGB_EXT                0x8583
#define GL_SOURCE4_RGB_EXT                0x8584
#define GL_SOURCE5_RGB_EXT                0x8585
#define GL_SOURCE6_RGB_EXT                0x8586
#define GL_SOURCE7_RGB_EXT                0x8587
#define GL_SOURCE0_ALPHA_EXT              0x8588
#define GL_SOURCE1_ALPHA_EXT              0x8589
#define GL_SOURCE2_ALPHA_EXT              0x858A
#define GL_SOURCE3_ALPHA_EXT              0x858B
#define GL_SOURCE4_ALPHA_EXT              0x858C
#define GL_SOURCE5_ALPHA_EXT              0x858D
#define GL_SOURCE6_ALPHA_EXT              0x858E
#define GL_SOURCE7_ALPHA_EXT              0x858F
#define GL_OPERAND0_RGB_EXT               0x8590
#define GL_OPERAND1_RGB_EXT               0x8591
#define GL_OPERAND2_RGB_EXT               0x8592
#define GL_OPERAND3_RGB_EXT               0x8593
#define GL_OPERAND4_RGB_EXT               0x8594
#define GL_OPERAND5_RGB_EXT               0x8595
#define GL_OPERAND6_RGB_EXT               0x8596
#define GL_OPERAND7_RGB_EXT               0x8597
#define GL_OPERAND0_ALPHA_EXT             0x8598
#define GL_OPERAND1_ALPHA_EXT             0x8599
#define GL_OPERAND2_ALPHA_EXT             0x859A
#define GL_OPERAND3_ALPHA_EXT             0x859B
#define GL_OPERAND4_ALPHA_EXT             0x859C
#define GL_OPERAND5_ALPHA_EXT             0x859D
#define GL_OPERAND6_ALPHA_EXT             0x859E
#define GL_OPERAND7_ALPHA_EXT             0x859F
#endif

#if GL_ARB_texture_env_combine
#define GL_COMBINE_ARB                    0x8570
#define GL_COMBINE_RGB_ARB                0x8571
#define GL_COMBINE_ALPHA_ARB              0x8572
#define GL_RGB_SCALE_ARB                  0x8573
#define GL_ADD_SIGNED_ARB                 0x8574
#define GL_INTERPOLATE_ARB                0x8575
#define GL_CONSTANT_ARB                   0x8576
#define GL_PRIMARY_COLOR_ARB              0x8577
#define GL_PREVIOUS_ARB                   0x8578
#define GL_SUBTRACT_ARB                   0x84E7
#define GL_SOURCE0_RGB_ARB                0x8580
#define GL_SOURCE1_RGB_ARB                0x8581
#define GL_SOURCE2_RGB_ARB                0x8582
#define GL_SOURCE3_RGB_ARB                0x8583
#define GL_SOURCE4_RGB_ARB                0x8584
#define GL_SOURCE5_RGB_ARB                0x8585
#define GL_SOURCE6_RGB_ARB                0x8586
#define GL_SOURCE7_RGB_ARB                0x8587
#define GL_SOURCE0_ALPHA_ARB              0x8588
#define GL_SOURCE1_ALPHA_ARB              0x8589
#define GL_SOURCE2_ALPHA_ARB              0x858A
#define GL_SOURCE3_ALPHA_ARB              0x858B
#define GL_SOURCE4_ALPHA_ARB              0x858C
#define GL_SOURCE5_ALPHA_ARB              0x858D
#define GL_SOURCE6_ALPHA_ARB              0x858E
#define GL_SOURCE7_ALPHA_ARB              0x858F
#define GL_OPERAND0_RGB_ARB               0x8590
#define GL_OPERAND1_RGB_ARB               0x8591
#define GL_OPERAND2_RGB_ARB               0x8592
#define GL_OPERAND3_RGB_ARB               0x8593
#define GL_OPERAND4_RGB_ARB               0x8594
#define GL_OPERAND5_RGB_ARB               0x8595
#define GL_OPERAND6_RGB_ARB               0x8596
#define GL_OPERAND7_RGB_ARB               0x8597
#define GL_OPERAND0_ALPHA_ARB             0x8598
#define GL_OPERAND1_ALPHA_ARB             0x8599
#define GL_OPERAND2_ALPHA_ARB             0x859A
#define GL_OPERAND3_ALPHA_ARB             0x859B
#define GL_OPERAND4_ALPHA_ARB             0x859C
#define GL_OPERAND5_ALPHA_ARB             0x859D
#define GL_OPERAND6_ALPHA_ARB             0x859E
#define GL_OPERAND7_ALPHA_ARB             0x859F
#endif


#if GL_APPLE_specular_vector
#define GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE 0x85B0
#endif

#if GL_APPLE_transform_hint
#define GL_TRANSFORM_HINT_APPLE           0x85B1
#endif

#if GL_APPLE_client_storage
#define GL_UNPACK_CLIENT_STORAGE_APPLE    0x85B2
#endif

#if GL_APPLE_ycbcr_422
#define GL_YCBCR_422_APPLE                 0x85B9
#define GL_UNSIGNED_SHORT_8_8_APPLE        0x85BA
#define GL_UNSIGNED_SHORT_8_8_REV_APPLE    0x85BB
#endif

#if GL_APPLE_texture_range
#define GL_TEXTURE_RANGE_LENGTH_APPLE      0x85B7
#define GL_TEXTURE_RANGE_POINTER_APPLE     0x85B8
#define GL_TEXTURE_STORAGE_HINT_APPLE      0x85BC
#endif
#if GL_APPLE_vertex_array_range || GL_APPLE_texture_range
#define GL_STORAGE_PRIVATE_APPLE           0x85BD
#define GL_STORAGE_CACHED_APPLE            0x85BE
#define GL_STORAGE_SHARED_APPLE            0x85BF
#endif

#if GL_SGIX_fog_scale
#define GL_FOG_SCALE_SGIX                 0x81FC
#define GL_FOG_SCALE_VALUE_SGIX           0x81FD
#endif

#if GL_SUNX_constant_data
#define GL_UNPACK_CONSTANT_DATA_SUNX      0x81D5
#define GL_TEXTURE_CONSTANT_DATA_SUNX     0x81D6
#endif

#if GL_SUN_global_alpha
#define GL_GLOBAL_ALPHA_SUN               0x81D9
#define GL_GLOBAL_ALPHA_FACTOR_SUN        0x81DA
#endif

#if GL_SUN_triangle_list
#define GL_RESTART_SUN                    0x01
#define GL_REPLACE_MIDDLE_SUN             0x02
#define GL_REPLACE_OLDEST_SUN             0x03
#define GL_TRIANGLE_LIST_SUN              0x81D7
#define GL_REPLACEMENT_CODE_SUN           0x81D8
#define GL_REPLACEMENT_CODE_ARRAY_SUN     0x85C0
#define GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN 0x85C1
#define GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN 0x85C2
#define GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN 0x85C3
#define GL_R1UI_V3F_SUN                   0x85C4
#define GL_R1UI_C4UB_V3F_SUN              0x85C5
#define GL_R1UI_C3F_V3F_SUN               0x85C6
#define GL_R1UI_N3F_V3F_SUN               0x85C7
#define GL_R1UI_C4F_N3F_V3F_SUN           0x85C8
#define GL_R1UI_T2F_V3F_SUN               0x85C9
#define GL_R1UI_T2F_N3F_V3F_SUN           0x85CA
#define GL_R1UI_T2F_C4F_N3F_V3F_SUN       0x85CB
#endif

#if GL_EXT_blend_func_separate
#define GL_BLEND_DST_RGB_EXT              0x80C8
#define GL_BLEND_SRC_RGB_EXT              0x80C9
#define GL_BLEND_DST_ALPHA_EXT            0x80CA
#define GL_BLEND_SRC_ALPHA_EXT            0x80CB
#endif

#if GL_INGR_color_clamp
#define GL_RED_MIN_CLAMP_INGR             0x8560
#define GL_GREEN_MIN_CLAMP_INGR           0x8561
#define GL_BLUE_MIN_CLAMP_INGR            0x8562
#define GL_ALPHA_MIN_CLAMP_INGR           0x8563
#define GL_RED_MAX_CLAMP_INGR             0x8564
#define GL_GREEN_MAX_CLAMP_INGR           0x8565
#define GL_BLUE_MAX_CLAMP_INGR            0x8566
#define GL_ALPHA_MAX_CLAMP_INGR           0x8567
#endif

#if GL_INGR_interlace_read
#define GL_INTERLACE_READ_INGR            0x8568
#endif

#if GL_EXT_stencil_wrap
#define GL_INCR_WRAP_EXT                  0x8507
#define GL_DECR_WRAP_EXT                  0x8508
#endif

#if GL_EXT_422_pixels
#define GL_422_EXT                        0x80CC
#define GL_422_REV_EXT                    0x80CD
#define GL_422_AVERAGE_EXT                0x80CE
#define GL_422_REV_AVERAGE_EXT            0x80CF
#endif

#if GL_NV_texgen_reflection
#define GL_NORMAL_MAP_NV                  0x8511
#define GL_REFLECTION_MAP_NV              0x8512
#endif

#if GL_EXT_texture_cube_map
#define GL_NORMAL_MAP_EXT                 0x8511
#define GL_REFLECTION_MAP_EXT             0x8512
#define GL_TEXTURE_CUBE_MAP_EXT           0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT   0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT     0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT  0x851C
#endif

#if GL_SUN_convolution_border_modes
#define GL_WRAP_BORDER_SUN                0x81D4
#endif

#if GL_EXT_texture_lod_bias
#define GL_MAX_TEXTURE_LOD_BIAS_EXT       0x84FD
#define GL_TEXTURE_FILTER_CONTROL_EXT     0x8500
#define GL_TEXTURE_LOD_BIAS_EXT           0x8501
#endif

#if GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#if GL_EXT_vertex_weighting
#define GL_MODELVIEW0_STACK_DEPTH_EXT     GL_MODELVIEW_STACK_DEPTH
#define GL_MODELVIEW1_STACK_DEPTH_EXT     0x8502
#define GL_MODELVIEW0_MATRIX_EXT          GL_MODELVIEW_MATRIX
#define GL_MODELVIEW_MATRIX1_EXT          0x8506
#define GL_VERTEX_WEIGHTING_EXT           0x8509
#define GL_MODELVIEW0_EXT                 GL_MODELVIEW
#define GL_MODELVIEW1_EXT                 0x850A
#define GL_CURRENT_VERTEX_WEIGHT_EXT      0x850B
#define GL_VERTEX_WEIGHT_ARRAY_EXT        0x850C
#define GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT   0x850D
#define GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT   0x850E
#define GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT 0x850F
#define GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT 0x8510
#endif

#if GL_NV_light_max_exponent
#define GL_MAX_SHININESS_NV               0x8504
#define GL_MAX_SPOT_EXPONENT_NV           0x8505
#endif

#if GL_NV_vertex_array_range
#define GL_VERTEX_ARRAY_RANGE_NV          0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV   0x851E
#define GL_VERTEX_ARRAY_RANGE_VALID_NV    0x851F
#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
#define GL_VERTEX_ARRAY_RANGE_POINTER_NV  0x8521
#endif

#if GL_NV_vertex_array_range2
#define GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV 0x8533
#endif

#if GL_NV_register_combiners
#define GL_REGISTER_COMBINERS_NV          0x8522
#define GL_VARIABLE_A_NV                  0x8523
#define GL_VARIABLE_B_NV                  0x8524
#define GL_VARIABLE_C_NV                  0x8525
#define GL_VARIABLE_D_NV                  0x8526
#define GL_VARIABLE_E_NV                  0x8527
#define GL_VARIABLE_F_NV                  0x8528
#define GL_VARIABLE_G_NV                  0x8529
#define GL_CONSTANT_COLOR0_NV             0x852A
#define GL_CONSTANT_COLOR1_NV             0x852B
#define GL_PRIMARY_COLOR_NV               0x852C
#define GL_SECONDARY_COLOR_NV             0x852D
#define GL_SPARE0_NV                      0x852E
#define GL_SPARE1_NV                      0x852F
#define GL_DISCARD_NV                     0x8530
#define GL_E_TIMES_F_NV                   0x8531
#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV 0x8532
#define GL_UNSIGNED_IDENTITY_NV           0x8536
#define GL_UNSIGNED_INVERT_NV             0x8537
#define GL_EXPAND_NORMAL_NV               0x8538
#define GL_EXPAND_NEGATE_NV               0x8539
#define GL_HALF_BIAS_NORMAL_NV            0x853A
#define GL_HALF_BIAS_NEGATE_NV            0x853B
#define GL_SIGNED_IDENTITY_NV             0x853C
#define GL_SIGNED_NEGATE_NV               0x853D
#define GL_SCALE_BY_TWO_NV                0x853E
#define GL_SCALE_BY_FOUR_NV               0x853F
#define GL_SCALE_BY_ONE_HALF_NV           0x8540
#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV   0x8541
#define GL_COMBINER_INPUT_NV              0x8542
#define GL_COMBINER_MAPPING_NV            0x8543
#define GL_COMBINER_COMPONENT_USAGE_NV    0x8544
#define GL_COMBINER_AB_DOT_PRODUCT_NV     0x8545
#define GL_COMBINER_CD_DOT_PRODUCT_NV     0x8546
#define GL_COMBINER_MUX_SUM_NV            0x8547
#define GL_COMBINER_SCALE_NV              0x8548
#define GL_COMBINER_BIAS_NV               0x8549
#define GL_COMBINER_AB_OUTPUT_NV          0x854A
#define GL_COMBINER_CD_OUTPUT_NV          0x854B
#define GL_COMBINER_SUM_OUTPUT_NV         0x854C
#define GL_MAX_GENERAL_COMBINERS_NV       0x854D
#define GL_NUM_GENERAL_COMBINERS_NV       0x854E
#define GL_COLOR_SUM_CLAMP_NV             0x854F
#define GL_COMBINER0_NV                   0x8550
#define GL_COMBINER1_NV                   0x8551
#define GL_COMBINER2_NV                   0x8552
#define GL_COMBINER3_NV                   0x8553
#define GL_COMBINER4_NV                   0x8554
#define GL_COMBINER5_NV                   0x8555
#define GL_COMBINER6_NV                   0x8556
#define GL_COMBINER7_NV                   0x8557
/* reuse GL_TEXTURE0_ARB */
/* reuse GL_TEXTURE1_ARB */
/* reuse GL_ZERO */
/* reuse GL_NONE */
/* reuse GL_FOG */
#endif

#if GL_ARB_texture_mirrored_repeat
#define GL_MIRRORED_REPEAT_ARB            0x8370
#endif

#if GL_NV_register_combiners2
#define GL_PER_STAGE_CONSTANTS_NV         0x8535
#endif

#if GL_NV_fog_distance
#define GL_FOG_DISTANCE_MODE_NV           0x855A
#define GL_EYE_RADIAL_NV                  0x855B
#define GL_EYE_PLANE_ABSOLUTE_NV          0x855C
/* reuse GL_EYE_PLANE */
#endif

#if GL_NV_texgen_emboss
#define GL_EMBOSS_LIGHT_NV                0x855D
#define GL_EMBOSS_CONSTANT_NV             0x855E
#define GL_EMBOSS_MAP_NV                  0x855F
#endif

#if GL_NV_texture_env_combine4
#define GL_COMBINE4_NV                    0x8503
#define GL_SOURCE3_RGB_NV                 0x8583
#define GL_SOURCE3_ALPHA_NV               0x858B
#define GL_OPERAND3_RGB_NV                0x8593
#define GL_OPERAND3_ALPHA_NV              0x859B
#endif

#if GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

#if GL_EXT_texture_rectangle
#define GL_TEXTURE_RECTANGLE_EXT          0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_EXT  0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_EXT    0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT 0x84F8
#endif

#if GL_EXT_vertex_shader
#define GL_VERTEX_SHADER_EXT              0x8780
#define GL_VARIANT_VALUE_EXT              0x87E4
#define GL_VARIANT_DATATYPE_EXT           0x87E5
#define GL_VARIANT_ARRAY_STRIDE_EXT       0x87E6
#define GL_VARIANT_ARRAY_TYPE_EXT         0x87E7
#define GL_VARIANT_ARRAY_EXT              0x87E8
#define GL_VARIANT_ARRAY_POINTER_EXT      0x87E9
#define GL_INVARIANT_VALUE_EXT            0x87EA
#define GL_INVARIANT_DATATYPE_EXT         0x87EB
#define GL_LOCAL_CONSTANT_VALUE_EXT       0x87EC
#define GL_LOCAL_CONSTANT_DATATYPE_EXT    0x87Ed
#define GL_OP_INDEX_EXT                   0x8782
#define GL_OP_NEGATE_EXT                  0x8783
#define GL_OP_DOT3_EXT                    0x8784
#define GL_OP_DOT4_EXT                    0x8785
#define GL_OP_MUL_EXT                     0x8786
#define GL_OP_ADD_EXT                     0x8787
#define GL_OP_MADD_EXT                    0x8788
#define GL_OP_FRAC_EXT                    0x8789
#define GL_OP_MAX_EXT                     0x878A
#define GL_OP_MIN_EXT                     0x878B
#define GL_OP_SET_GE_EXT                  0x878C
#define GL_OP_SET_LT_EXT                  0x878D
#define GL_OP_CLAMP_EXT                   0x878E
#define GL_OP_FLOOR_EXT                   0x878F
#define GL_OP_ROUND_EXT                   0x8790
#define GL_OP_EXP_BASE_2_EXT              0x8791
#define GL_OP_LOG_BASE_2_EXT              0x8792
#define GL_OP_POWER_EXT                   0x8793
#define GL_OP_RECIP_EXT                   0x8794
#define GL_OP_RECIP_SQRT_EXT              0x8795
#define GL_OP_SUB_EXT                     0x8796
#define GL_OP_CROSS_PRODUCT_EXT           0x8797
#define GL_OP_MULTIPLY_MATRIX_EXT         0x8798
#define GL_OP_MOV_EXT                     0x8799
#define GL_OUTPUT_VERTEX_EXT              0x879A
#define GL_OUTPUT_COLOR0_EXT              0x879B
#define GL_OUTPUT_COLOR1_EXT              0x879C
#define GL_OUTPUT_TEXTURE_COORD0_EXT      0x879D
#define GL_OUTPUT_TEXTURE_COORD1_EXT      0x879E
#define GL_OUTPUT_TEXTURE_COORD2_EXT      0x879F
#define GL_OUTPUT_TEXTURE_COORD3_EXT      0x87A0
#define GL_OUTPUT_TEXTURE_COORD4_EXT      0x87A1
#define GL_OUTPUT_TEXTURE_COORD5_EXT      0x87A2
#define GL_OUTPUT_TEXTURE_COORD6_EXT      0x87A3
#define GL_OUTPUT_TEXTURE_COORD7_EXT      0x87A4
#define GL_OUTPUT_TEXTURE_COORD8_EXT      0x87A5
#define GL_OUTPUT_TEXTURE_COORD9_EXT      0x87A6
#define GL_OUTPUT_TEXTURE_COORD10_EXT     0x87A7
#define GL_OUTPUT_TEXTURE_COORD11_EXT     0x87A8
#define GL_OUTPUT_TEXTURE_COORD12_EXT     0x87A9
#define GL_OUTPUT_TEXTURE_COORD13_EXT     0x87AA
#define GL_OUTPUT_TEXTURE_COORD14_EXT     0x87AB
#define GL_OUTPUT_TEXTURE_COORD15_EXT     0x87AC
#define GL_OUTPUT_TEXTURE_COORD16_EXT     0x87AD
#define GL_OUTPUT_TEXTURE_COORD17_EXT     0x87AE
#define GL_OUTPUT_TEXTURE_COORD18_EXT     0x87AF
#define GL_OUTPUT_TEXTURE_COORD19_EXT     0x87B0
#define GL_OUTPUT_TEXTURE_COORD20_EXT     0x87B1
#define GL_OUTPUT_TEXTURE_COORD21_EXT     0x87B2
#define GL_OUTPUT_TEXTURE_COORD22_EXT     0x87B3
#define GL_OUTPUT_TEXTURE_COORD23_EXT     0x87B4
#define GL_OUTPUT_TEXTURE_COORD24_EXT     0x87B5
#define GL_OUTPUT_TEXTURE_COORD25_EXT     0x87B6
#define GL_OUTPUT_TEXTURE_COORD26_EXT     0x87B7
#define GL_OUTPUT_TEXTURE_COORD27_EXT     0x87B8
#define GL_OUTPUT_TEXTURE_COORD28_EXT     0x87B9
#define GL_OUTPUT_TEXTURE_COORD29_EXT     0x87BA
#define GL_OUTPUT_TEXTURE_COORD30_EXT     0x87BB
#define GL_OUTPUT_TEXTURE_COORD31_EXT     0x87BC
#define GL_OUTPUT_FOG_EXT                 0x87BD
#define GL_SCALAR_EXT                     0x87BE
#define GL_VECTOR_EXT                     0x87BF
#define GL_MATRIX_EXT                     0x87C0
#define GL_VARIANT_EXT                    0x87C1
#define GL_INVARIANT_EXT                  0x87C2
#define GL_LOCAL_CONSTANT_EXT             0x87C3
#define GL_LOCAL_EXT                      0x87C4
#define GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT              0x87C5
#define GL_MAX_VERTEX_SHADER_VARIANTS_EXT                  0x87C6
#define GL_MAX_VERTEX_SHADER_INVARIANTS_EXT                0x87C7
#define GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT           0x87C8
#define GL_MAX_VERTEX_SHADER_LOCALS_EXT                    0x87C9
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT    0x87CA
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT        0x87CB
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT 0x87CC
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT      0x87CD
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT          0x87CE
#define GL_VERTEX_SHADER_INSTRUCTIONS_EXT                  0x87CF
#define GL_VERTEX_SHADER_VARIANTS_EXT                      0x87D0
#define GL_VERTEX_SHADER_INVARIANTS_EXT                    0x87D1
#define GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT               0x87D2
#define GL_VERTEX_SHADER_LOCALS_EXT                        0x87D3
#define GL_VERTEX_SHADER_BINDING_EXT                       0x8781
#define GL_VERTEX_SHADER_OPTIMIZED_EXT                     0x87D4
#define GL_X_EXT                          0x87D5
#define GL_Y_EXT                          0x87D6
#define GL_Z_EXT                          0x87D7
#define GL_W_EXT                          0x87D8
#define GL_NEGATIVE_X_EXT                 0x87D9
#define GL_NEGATIVE_Y_EXT                 0x87DA
#define GL_NEGATIVE_Z_EXT                 0x87DB
#define GL_NEGATIVE_W_EXT                 0x87DC
#define GL_NEGATIVE_ONE_EXT               0x87DF
#define GL_NORMALIZED_RANGE_EXT           0x87E0
#define GL_FULL_RANGE_EXT                 0x87E1
#define GL_CURRENT_VERTEX_EXT             0x87E2
#define GL_MVP_MATRIX_EXT                 0x87E3
#endif

#if GL_EXT_fragment_shader
#define GL_FRAGMENT_SHADER_EXT            0x8920
#define GL_REG_0_EXT                      0x8921
#define GL_REG_1_EXT                      0x8922
#define GL_REG_2_EXT                      0x8923
#define GL_REG_3_EXT                      0x8924
#define GL_REG_4_EXT                      0x8925
#define GL_REG_5_EXT                      0x8926
#define GL_REG_6_EXT                      0x8927
#define GL_REG_7_EXT                      0x8928
#define GL_REG_8_EXT                      0x8929
#define GL_REG_9_EXT                      0x892A
#define GL_REG_10_EXT                     0x892B
#define GL_REG_11_EXT                     0x892C
#define GL_REG_12_EXT                     0x892D
#define GL_REG_13_EXT                     0x892E
#define GL_REG_14_EXT                     0x892F
#define GL_REG_15_EXT                     0x8930
#define GL_REG_16_EXT                     0x8931
#define GL_REG_17_EXT                     0x8932
#define GL_REG_18_EXT                     0x8933
#define GL_REG_19_EXT                     0x8934
#define GL_REG_20_EXT                     0x8935
#define GL_REG_21_EXT                     0x8936
#define GL_REG_22_EXT                     0x8937
#define GL_REG_23_EXT                     0x8938
#define GL_REG_24_EXT                     0x8939
#define GL_REG_25_EXT                     0x893A
#define GL_REG_26_EXT                     0x893B
#define GL_REG_27_EXT                     0x893C
#define GL_REG_28_EXT                     0x893D
#define GL_REG_29_EXT                     0x893E
#define GL_REG_30_EXT                     0x893F
#define GL_REG_31_EXT                     0x8940
#define GL_CON_0_EXT                      0x8941
#define GL_CON_1_EXT                      0x8942
#define GL_CON_2_EXT                      0x8943
#define GL_CON_3_EXT                      0x8944
#define GL_CON_4_EXT                      0x8945
#define GL_CON_5_EXT                      0x8946
#define GL_CON_6_EXT                      0x8947
#define GL_CON_7_EXT                      0x8948
#define GL_CON_8_EXT                      0x8949
#define GL_CON_9_EXT                      0x894A
#define GL_CON_10_EXT                     0x894B
#define GL_CON_11_EXT                     0x894C
#define GL_CON_12_EXT                     0x894D
#define GL_CON_13_EXT                     0x894E
#define GL_CON_14_EXT                     0x894F
#define GL_CON_15_EXT                     0x8950
#define GL_CON_16_EXT                     0x8951
#define GL_CON_17_EXT                     0x8952
#define GL_CON_18_EXT                     0x8953
#define GL_CON_19_EXT                     0x8954
#define GL_CON_20_EXT                     0x8955
#define GL_CON_21_EXT                     0x8956
#define GL_CON_22_EXT                     0x8957
#define GL_CON_23_EXT                     0x8958
#define GL_CON_24_EXT                     0x8959
#define GL_CON_25_EXT                     0x895A
#define GL_CON_26_EXT                     0x895B
#define GL_CON_27_EXT                     0x895C
#define GL_CON_28_EXT                     0x895D
#define GL_CON_29_EXT                     0x895E
#define GL_CON_30_EXT                     0x895F
#define GL_CON_31_EXT                     0x8960
#define GL_MOV_EXT                        0x8961
#define GL_ADD_EXT                        0x8963
#define GL_MUL_EXT                        0x8964
#define GL_SUB_EXT                        0x8965
#define GL_DOT3_EXT                       0x8966
#define GL_DOT4_EXT                       0x8967
#define GL_MAD_EXT                        0x8968
#define GL_LERP_EXT                       0x8969
#define GL_CND_EXT                        0x896A
#define GL_CND0_EXT                       0x896B
#define GL_DOT2_ADD_EXT                   0x896C
#define GL_SECONDARY_INTERPOLATOR_EXT     0x896D
#define GL_NUM_FRAGMENT_REGISTERS_EXT     0x896E
#define GL_NUM_FRAGMENT_CONSTANTS_EXT     0x896F
#define GL_NUM_PASSES_EXT                 0x8970
#define GL_NUM_INSTRUCTIONS_PER_PASS_EXT  0x8971
#define GL_NUM_INSTRUCTIONS_TOTAL_EXT     0x8972
#define GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_EXT 0x8973
#define GL_NUM_LOOPBACK_COMPONENTS_EXT    0x8974
#define GL_COLOR_ALPHA_PAIRING_EXT        0x8975
#define GL_SWIZZLE_STR_EXT                0x8976
#define GL_SWIZZLE_STQ_EXT                0x8977
#define GL_SWIZZLE_STR_DR_EXT             0x8978
#define GL_SWIZZLE_STQ_DQ_EXT             0x8979
#define GL_SWIZZLE_STRQ_EXT               0x897A
#define GL_SWIZZLE_STRQ_DQ_EXT            0x897B
#define GL_RED_BIT_EXT                    0x00000001
#define GL_GREEN_BIT_EXT                  0x00000002
#define GL_BLUE_BIT_EXT                   0x00000004
#define GL_2X_BIT_EXT                     0x00000001
#define GL_4X_BIT_EXT                     0x00000002
#define GL_8X_BIT_EXT                     0x00000004
#define GL_HALF_BIT_EXT                   0x00000008
#define GL_QUARTER_BIT_EXT                0x00000010
#define GL_EIGHTH_BIT_EXT                 0x00000020
#define GL_SATURATE_BIT_EXT               0x00000040
#define GL_COMP_BIT_EXT                   0x00000002
#define GL_NEGATE_BIT_EXT                 0x00000004
#define GL_BIAS_BIT_EXT                   0x00000008
#endif

#if GL_IBM_cull_vertex
#define GL_CULL_VERTEX_IBM                103050
#endif

#if GL_IBM_vertex_array_lists
#define GL_VERTEX_ARRAY_LIST_IBM          103070
#define GL_NORMAL_ARRAY_LIST_IBM          103071
#define GL_COLOR_ARRAY_LIST_IBM           103072
#define GL_INDEX_ARRAY_LIST_IBM           103073
#define GL_TEXTURE_COORD_ARRAY_LIST_IBM   103074
#define GL_EDGE_FLAG_ARRAY_LIST_IBM       103075
#define GL_FOG_COORDINATE_ARRAY_LIST_IBM  103076
#define GL_SECONDARY_COLOR_ARRAY_LIST_IBM 103077
#define GL_VERTEX_ARRAY_LIST_STRIDE_IBM   103080
#define GL_NORMAL_ARRAY_LIST_STRIDE_IBM   103081
#define GL_COLOR_ARRAY_LIST_STRIDE_IBM    103082
#define GL_INDEX_ARRAY_LIST_STRIDE_IBM    103083
#define GL_TEXTURE_COORD_ARRAY_LIST_STRIDE_IBM 103084
#define GL_EDGE_FLAG_ARRAY_LIST_STRIDE_IBM 103085
#define GL_FOG_COORDINATE_ARRAY_LIST_STRIDE_IBM 103086
#define GL_SECONDARY_COLOR_ARRAY_LIST_STRIDE_IBM 103087
#endif

#if GL_SGIX_subsample
#define GL_PACK_SUBSAMPLE_RATE_SGIX       0x85A0
#define GL_UNPACK_SUBSAMPLE_RATE_SGIX     0x85A1
#define GL_PIXEL_SUBSAMPLE_4444_SGIX      0x85A2
#define GL_PIXEL_SUBSAMPLE_2424_SGIX      0x85A3
#define GL_PIXEL_SUBSAMPLE_4242_SGIX      0x85A4
#endif

#if GL_SGIX_ycrcba
#define GL_YCRCB_SGIX                     0x8318
#define GL_YCRCBA_SGIX                    0x8319
#endif

#if GL_SGI_depth_pass_instrument
#define GL_DEPTH_PASS_INSTRUMENT_SGIX     0x8310
#define GL_DEPTH_PASS_INSTRUMENT_COUNTERS_SGIX 0x8311
#define GL_DEPTH_PASS_INSTRUMENT_MAX_SGIX 0x8312
#endif

#if GL_3DFX_texture_compression_FXT1
#define GL_COMPRESSED_RGB_FXT1_3DFX       0x86B0
#define GL_COMPRESSED_RGBA_FXT1_3DFX      0x86B1
#endif

#if GL_3DFX_multisample
#define GL_MULTISAMPLE_3DFX               0x86B2
#define GL_SAMPLE_BUFFERS_3DFX            0x86B3
#define GL_SAMPLES_3DFX                   0x86B4
#define GL_MULTISAMPLE_BIT_3DFX           0x20000000
#endif

#if GL_EXT_multisample
#define GL_MULTISAMPLE_EXT                0x809D
#define GL_SAMPLE_ALPHA_TO_MASK_EXT       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_EXT        0x809F
#define GL_SAMPLE_MASK_EXT                0x80A0
#define GL_1PASS_EXT                      0x80A1
#define GL_2PASS_0_EXT                    0x80A2
#define GL_2PASS_1_EXT                    0x80A3
#define GL_4PASS_0_EXT                    0x80A4
#define GL_4PASS_1_EXT                    0x80A5
#define GL_4PASS_2_EXT                    0x80A6
#define GL_4PASS_3_EXT                    0x80A7
#define GL_SAMPLE_BUFFERS_EXT             0x80A8
#define GL_SAMPLES_EXT                    0x80A9
#define GL_SAMPLE_MASK_VALUE_EXT          0x80AA
#define GL_SAMPLE_MASK_INVERT_EXT         0x80AB
#define GL_SAMPLE_PATTERN_EXT             0x80AC
#endif

#if GL_SGIX_vertex_preclip
#define GL_VERTEX_PRECLIP_SGIX            0x83EE
#define GL_VERTEX_PRECLIP_HINT_SGIX       0x83EF
#endif

#if GL_SGIX_convolution_accuracy
#define GL_CONVOLUTION_HINT_SGIX          0x8316
#endif

#if GL_SGIX_resample
#define GL_PACK_RESAMPLE_SGIX             0x842C
#define GL_UNPACK_RESAMPLE_SGIX           0x842D
#define GL_RESAMPLE_REPLICATE_SGIX        0x842E
#define GL_RESAMPLE_ZERO_FILL_SGIX        0x842F
#define GL_RESAMPLE_DECIMATE_SGIX         0x8430
#endif

#if GL_SGIS_point_line_texgen
#define GL_EYE_DISTANCE_TO_POINT_SGIS     0x81F0
#define GL_OBJECT_DISTANCE_TO_POINT_SGIS  0x81F1
#define GL_EYE_DISTANCE_TO_LINE_SGIS      0x81F2
#define GL_OBJECT_DISTANCE_TO_LINE_SGIS   0x81F3
#define GL_EYE_POINT_SGIS                 0x81F4
#define GL_OBJECT_POINT_SGIS              0x81F5
#define GL_EYE_LINE_SGIS                  0x81F6
#define GL_OBJECT_LINE_SGIS               0x81F7
#endif

#if GL_SGIS_texture_color_mask
#define GL_TEXTURE_COLOR_WRITEMASK_SGIS   0x81EF
#endif

#if GL_NV_vertex_program
#define GL_VERTEX_PROGRAM_NV              0x8620
#define GL_VERTEX_STATE_PROGRAM_NV        0x8621
#define GL_ATTRIB_ARRAY_SIZE_NV           0x8623
#define GL_ATTRIB_ARRAY_STRIDE_NV         0x8624
#define GL_ATTRIB_ARRAY_TYPE_NV           0x8625
#define GL_CURRENT_ATTRIB_NV              0x8626
#define GL_PROGRAM_LENGTH_NV              0x8627
#define GL_PROGRAM_STRING_NV              0x8628
#define GL_MODELVIEW_PROJECTION_NV        0x8629
#define GL_IDENTITY_NV                    0x862A
#define GL_INVERSE_NV                     0x862B
#define GL_TRANSPOSE_NV                   0x862C
#define GL_INVERSE_TRANSPOSE_NV           0x862D
#define GL_MAX_TRACK_MATRIX_STACK_DEPTH_NV 0x862E
#define GL_MAX_TRACK_MATRICES_NV          0x862F
#define GL_MATRIX0_NV                     0x8630
#define GL_MATRIX1_NV                     0x8631
#define GL_MATRIX2_NV                     0x8632
#define GL_MATRIX3_NV                     0x8633
#define GL_MATRIX4_NV                     0x8634
#define GL_MATRIX5_NV                     0x8635
#define GL_MATRIX6_NV                     0x8636
#define GL_MATRIX7_NV                     0x8637
#define GL_CURRENT_MATRIX_STACK_DEPTH_NV  0x8640
#define GL_CURRENT_MATRIX_NV              0x8641
#define GL_VERTEX_PROGRAM_POINT_SIZE_NV   0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_NV     0x8643
#define GL_PROGRAM_PARAMETER_NV           0x8644
#define GL_ATTRIBUTE_ARRAY_POINTER_NV     0x8645
#define GL_PROGRAM_TARGET_NV              0x8646
#define GL_PROGRAM_RESIDENT_NV            0x8647
#define GL_TRACK_MATRIX_NV                0x8648
#define GL_TRACK_MATRIX_TRANSFORM_NV      0x8649
#define GL_VERTEX_PROGRAM_BINDING_NV      0x864A
#define GL_PROGRAM_ERROR_POSITION_NV      0x864B
#define GL_VERTEX_ATTRIB_ARRAY0_NV        0x8650
#define GL_VERTEX_ATTRIB_ARRAY1_NV        0x8651
#define GL_VERTEX_ATTRIB_ARRAY2_NV        0x8652
#define GL_VERTEX_ATTRIB_ARRAY3_NV        0x8653
#define GL_VERTEX_ATTRIB_ARRAY4_NV        0x8654
#define GL_VERTEX_ATTRIB_ARRAY5_NV        0x8655
#define GL_VERTEX_ATTRIB_ARRAY6_NV        0x8656
#define GL_VERTEX_ATTRIB_ARRAY7_NV        0x8657
#define GL_VERTEX_ATTRIB_ARRAY8_NV        0x8658
#define GL_VERTEX_ATTRIB_ARRAY9_NV        0x8659
#define GL_VERTEX_ATTRIB_ARRAY10_NV       0x865A
#define GL_VERTEX_ATTRIB_ARRAY11_NV       0x865B
#define GL_VERTEX_ATTRIB_ARRAY12_NV       0x865C
#define GL_VERTEX_ATTRIB_ARRAY13_NV       0x865D
#define GL_VERTEX_ATTRIB_ARRAY14_NV       0x865E
#define GL_VERTEX_ATTRIB_ARRAY15_NV       0x865F
#define GL_MAP1_VERTEX_ATTRIB0_4_NV       0x8660
#define GL_MAP1_VERTEX_ATTRIB1_4_NV       0x8661
#define GL_MAP1_VERTEX_ATTRIB2_4_NV       0x8662
#define GL_MAP1_VERTEX_ATTRIB3_4_NV       0x8663
#define GL_MAP1_VERTEX_ATTRIB4_4_NV       0x8664
#define GL_MAP1_VERTEX_ATTRIB5_4_NV       0x8665
#define GL_MAP1_VERTEX_ATTRIB6_4_NV       0x8666
#define GL_MAP1_VERTEX_ATTRIB7_4_NV       0x8667
#define GL_MAP1_VERTEX_ATTRIB8_4_NV       0x8668
#define GL_MAP1_VERTEX_ATTRIB9_4_NV       0x8669
#define GL_MAP1_VERTEX_ATTRIB10_4_NV      0x866A
#define GL_MAP1_VERTEX_ATTRIB11_4_NV      0x866B
#define GL_MAP1_VERTEX_ATTRIB12_4_NV      0x866C
#define GL_MAP1_VERTEX_ATTRIB13_4_NV      0x866D
#define GL_MAP1_VERTEX_ATTRIB14_4_NV      0x866E
#define GL_MAP1_VERTEX_ATTRIB15_4_NV      0x866F
#define GL_MAP2_VERTEX_ATTRIB0_4_NV       0x8670
#define GL_MAP2_VERTEX_ATTRIB1_4_NV       0x8671
#define GL_MAP2_VERTEX_ATTRIB2_4_NV       0x8672
#define GL_MAP2_VERTEX_ATTRIB3_4_NV       0x8673
#define GL_MAP2_VERTEX_ATTRIB4_4_NV       0x8674
#define GL_MAP2_VERTEX_ATTRIB5_4_NV       0x8675
#define GL_MAP2_VERTEX_ATTRIB6_4_NV       0x8676
#define GL_MAP2_VERTEX_ATTRIB7_4_NV       0x8677
#define GL_MAP2_VERTEX_ATTRIB8_4_NV       0x8678
#define GL_MAP2_VERTEX_ATTRIB9_4_NV       0x8679
#define GL_MAP2_VERTEX_ATTRIB10_4_NV      0x867A
#define GL_MAP2_VERTEX_ATTRIB11_4_NV      0x867B
#define GL_MAP2_VERTEX_ATTRIB12_4_NV      0x867C
#define GL_MAP2_VERTEX_ATTRIB13_4_NV      0x867D
#define GL_MAP2_VERTEX_ATTRIB14_4_NV      0x867E
#define GL_MAP2_VERTEX_ATTRIB15_4_NV      0x867F
#endif

#if GL_ARB_texture_env_dot3
#define GL_DOT3_RGB_ARB                   0x86AE
#define GL_DOT3_RGBA_ARB                  0x86AF
#endif

#if GL_ARB_point_parameters
#define GL_POINT_SIZE_MIN_ARB                            0x8126
#define GL_POINT_SIZE_MAX_ARB                            0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_ARB                 0x8128
#define GL_POINT_DISTANCE_ATTENUATION_ARB                0x8129
#endif

#if GL_ATI_texture_mirror_once
#define GL_MIRROR_CLAMP_ATI                             0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_ATI                     0x8743
#endif

#if GL_ATI_pn_triangles
#define GL_PN_TRIANGLES_ATI                             0x6090
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI       0x6091
#define GL_PN_TRIANGLES_POINT_MODE_ATI                  0x6092
#define GL_PN_TRIANGLES_NORMAL_MODE_ATI                 0x6093
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI           0x6094
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI           0x6095
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI            0x6096
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI          0x6097
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI       0x6098
#endif

#if GL_ATIX_pn_triangles
#define GL_PN_TRIANGLES_ATIX                            0x6090
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATIX      0x6091
#define GL_PN_TRIANGLES_POINT_MODE_ATIX                 0x6092
#define GL_PN_TRIANGLES_NORMAL_MODE_ATIX                0x6093
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATIX          0x6094
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATIX          0x6095
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATIX           0x6096
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATIX         0x6097
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATIX      0x6098
#endif

#if GL_ATI_text_fragment_shader
#define GL_TEXT_FRAGMENT_SHADER_ATI                     0x8200
#endif

#if GL_ATI_blend_equation_separate
#define GL_ALPHA_BLEND_EQUATION_ATI                     0x883D
#endif

#if GL_ARB_vertex_program
#define GL_VERTEX_PROGRAM_ARB                            0x8620
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB                 0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB                   0x8643
#define GL_PROGRAM_FORMAT_ASCII_ARB                      0x8875
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB               0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB                  0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB                0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB                  0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB            0x886A
#define GL_CURRENT_VERTEX_ATTRIB_ARB                     0x8626
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB               0x8645
#define GL_PROGRAM_LENGTH_ARB                            0x8627
#define GL_PROGRAM_FORMAT_ARB                            0x8876
#define GL_PROGRAM_NAME_ARB                              0x8677
#define GL_PROGRAM_INSTRUCTIONS_ARB                      0x88A0
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB                  0x88A1
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB               0x88A2
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB           0x88A3
#define GL_PROGRAM_TEMPORARIES_ARB                       0x88A4
#define GL_MAX_PROGRAM_TEMPORARIES_ARB                   0x88A5
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB                0x88A6
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB            0x88A7
#define GL_PROGRAM_PARAMETERS_ARB                        0x88A8
#define GL_MAX_PROGRAM_PARAMETERS_ARB                    0x88A9
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB                 0x88AA
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB             0x88AB
#define GL_PROGRAM_ATTRIBS_ARB                           0x88AC
#define GL_MAX_PROGRAM_ATTRIBS_ARB                       0x88AD
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB                    0x88AE
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB                0x88AF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB                 0x88B0
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB             0x88B1
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB          0x88B2
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB      0x88B3
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB              0x88B4
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB                0x88B5
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB               0x88B6
#define GL_PROGRAM_STRING_ARB                            0x8628
#define GL_PROGRAM_ERROR_POSITION_ARB                    0x864B
#define GL_CURRENT_MATRIX_ARB                            0x8641
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB                  0x88B7
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB                0x8640
#define GL_MAX_VERTEX_ATTRIBS_ARB                        0x8869
#define GL_MAX_PROGRAM_MATRICES_ARB                      0x862F
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB            0x862E
#define GL_PROGRAM_ERROR_STRING_ARB                      0x8874

#define GL_MATRIX0_ARB                                   0x88C0
#define GL_MATRIX1_ARB                                   0x88C1
#define GL_MATRIX2_ARB                                   0x88C2
#define GL_MATRIX3_ARB                                   0x88C3
#define GL_MATRIX4_ARB                                   0x88C4
#define GL_MATRIX5_ARB                                   0x88C5
#define GL_MATRIX6_ARB                                   0x88C6
#define GL_MATRIX7_ARB                                   0x88C7
#define GL_MATRIX8_ARB                                   0x88C8
#define GL_MATRIX9_ARB                                   0x88C9
#define GL_MATRIX10_ARB                                  0x88CA
#define GL_MATRIX11_ARB                                  0x88CB
#define GL_MATRIX12_ARB                                  0x88CC
#define GL_MATRIX13_ARB                                  0x88CD
#define GL_MATRIX14_ARB                                  0x88CE
#define GL_MATRIX15_ARB                                  0x88CF
#define GL_MATRIX16_ARB                                  0x88D0
#define GL_MATRIX17_ARB                                  0x88D1
#define GL_MATRIX18_ARB                                  0x88D2
#define GL_MATRIX19_ARB                                  0x88D3
#define GL_MATRIX20_ARB                                  0x88D4
#define GL_MATRIX21_ARB                                  0x88D5
#define GL_MATRIX22_ARB                                  0x88D6
#define GL_MATRIX23_ARB                                  0x88D7
#define GL_MATRIX24_ARB                                  0x88D8
#define GL_MATRIX25_ARB                                  0x88D9
#define GL_MATRIX26_ARB                                  0x88DA
#define GL_MATRIX27_ARB                                  0x88DB
#define GL_MATRIX28_ARB                                  0x88DC
#define GL_MATRIX29_ARB                                  0x88DD
#define GL_MATRIX30_ARB                                  0x88DE
#define GL_MATRIX31_ARB                                  0x88DF
#endif

#if GL_APPLE_vertex_program_evaluators
#define GL_VERTEX_ATTRIB_MAP1_ARB                        0x8A00
#define GL_VERTEX_ATTRIB_MAP2_ARB                        0x8A01
#define GL_VERTEX_ATTRIB_MAP1_SIZE_ARB                   0x8A02
#define GL_VERTEX_ATTRIB_MAP1_COEFF_ARB                  0x8A03
#define GL_VERTEX_ATTRIB_MAP1_ORDER_ARB                  0x8A04
#define GL_VERTEX_ATTRIB_MAP1_DOMAIN_ARB                 0x8A05
#define GL_VERTEX_ATTRIB_MAP2_SIZE_ARB                   0x8A06
#define GL_VERTEX_ATTRIB_MAP2_COEFF_ARB                  0x8A07
#define GL_VERTEX_ATTRIB_MAP2_ORDER_ARB                  0x8A08
#define GL_VERTEX_ATTRIB_MAP2_DOMAIN_ARB                 0x8A09
#endif

#if GL_NV_texture_shader
#define GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV          0x86D9
#define GL_UNSIGNED_INT_S8_S8_8_8_NV                     0x86DA
#define GL_UNSIGNED_INT_8_8_S8_S8_REV_NV                 0x86DB
#define GL_DSDT_MAG_INTENSITY_NV                         0x86DC
#define GL_TEXTURE_SHADER_NV                             0x86DE
#define GL_SHADER_OPERATION_NV                           0x86DF

#define GL_CULL_MODES_NV                                 0x86E0
#define GL_OFFSET_TEXTURE_MATRIX_NV                      0x86E1
#define GL_OFFSET_TEXTURE_SCALE_NV                       0x86E2
#define GL_OFFSET_TEXTURE_BIAS_NV                        0x86E3
#define GL_OFFSET_TEXTURE_2D_MATRIX_NV                   GL_OFFSET_TEXTURE_MATRIX_NV
#define GL_OFFSET_TEXTURE_2D_SCALE_NV                    GL_OFFSET_TEXTURE_SCALE_NV
#define GL_OFFSET_TEXTURE_2D_BIAS_NV                     GL_OFFSET_TEXTURE_BIAS_NV
#define GL_PREVIOUS_TEXTURE_INPUT_NV                     0x86E4
#define GL_CONST_EYE_NV                                  0x86E5
#define GL_SHADER_CONSISTENT_NV                          0x86DD
#define GL_PASS_THROUGH_NV                               0x86E6
#define GL_CULL_FRAGMENT_NV                              0x86E7
#define GL_OFFSET_TEXTURE_2D_NV                          0x86E8
#define GL_OFFSET_TEXTURE_RECTANGLE_NV                   0x864C
#define GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV             0x864D
#define GL_DEPENDENT_AR_TEXTURE_2D_NV                    0x86E9
#define GL_DEPENDENT_GB_TEXTURE_2D_NV                    0x86EA
#define GL_DOT_PRODUCT_NV                                0x86EC
#define GL_DOT_PRODUCT_DEPTH_REPLACE_NV                  0x86ED
#define GL_DOT_PRODUCT_TEXTURE_2D_NV                     0x86EE
#define GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV              0x864E
#define GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV               0x86F0
#define GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV               0x86F1
#define GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV               0x86F2
#define GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV     0x86F3
#define GL_HILO_NV                                       0x86F4
#define GL_DSDT_NV                                       0x86F5
#define GL_DSDT_MAG_NV                                   0x86F6
#define GL_DSDT_MAG_VIB_NV                               0x86F7
#define GL_HILO16_NV                                     0x86F8
#define GL_SIGNED_HILO_NV                                0x86F9
#define GL_SIGNED_HILO16_NV                              0x86FA
#define GL_SIGNED_RGBA_NV                                0x86FB
#define GL_SIGNED_RGBA8_NV                               0x86FC
#define GL_SIGNED_RGB_NV                                 0x86FE
#define GL_SIGNED_RGB8_NV                                0x86FF
#define GL_SIGNED_LUMINANCE_NV                           0x8701
#define GL_SIGNED_LUMINANCE8_NV                          0x8702
#define GL_SIGNED_LUMINANCE_ALPHA_NV                     0x8703
#define GL_SIGNED_LUMINANCE8_ALPHA8_NV                   0x8704
#define GL_SIGNED_ALPHA_NV                               0x8705
#define GL_SIGNED_ALPHA8_NV                              0x8706
#define GL_SIGNED_INTENSITY_NV                           0x8707
#define GL_SIGNED_INTENSITY8_NV                          0x8708
#define GL_DSDT8_NV                                      0x8709
#define GL_DSDT8_MAG8_NV                                 0x870A
#define GL_DSDT8_MAG8_INTENSITY8_NV                      0x870B
#define GL_SIGNED_RGB_UNSIGNED_ALPHA_NV                  0x870C
#define GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV                0x870D
#define GL_HI_SCALE_NV                                   0x870E
#define GL_LO_SCALE_NV                                   0x870F
#define GL_DS_SCALE_NV                                   0x8710
#define GL_DT_SCALE_NV                                   0x8711
#define GL_MAGNITUDE_SCALE_NV                            0x8712
#define GL_VIBRANCE_SCALE_NV                             0x8713
#define GL_HI_BIAS_NV                                    0x8714
#define GL_LO_BIAS_NV                                    0x8715
#define GL_DS_BIAS_NV                                    0x8716
#define GL_DT_BIAS_NV                                    0x8717
#define GL_MAGNITUDE_BIAS_NV                             0x8718
#define GL_VIBRANCE_BIAS_NV                              0x8719
#define GL_TEXTURE_BORDER_VALUES_NV                      0x871A
#define GL_TEXTURE_HI_SIZE_NV                            0x871B
#define GL_TEXTURE_LO_SIZE_NV                            0x871C
#define GL_TEXTURE_DS_SIZE_NV                            0x871D
#define GL_TEXTURE_DT_SIZE_NV                            0x871E
#define GL_TEXTURE_MAG_SIZE_NV                           0x871F
#endif

#if GL_NV_texture_shader2
#define GL_DOT_PRODUCT_TEXTURE_3D_NV                     0x86EF
#endif

#if GL_NV_texture_shader3
#define GL_OFFSET_PROJECTIVE_TEXTURE_2D_NV               0x8850
#define GL_OFFSET_PROJECTIVE_TEXTURE_2D_SCALE_NV         0x8851
#define GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_NV        0x8852
#define GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_SCALE_NV  0x8853
#define GL_OFFSET_HILO_TEXTURE_2D_NV                     0x8854
#define GL_OFFSET_HILO_TEXTURE_RECTANGLE_NV              0x8855
#define GL_OFFSET_HILO_PROJECTIVE_TEXTURE_2D_NV          0x8856
#define GL_OFFSET_HILO_PROJECTIVE_TEXTURE_RECTANGLE_NV   0x8857
#define GL_DEPENDENT_HILO_TEXTURE_2D_NV                  0x8858
#define GL_DEPENDENT_RGB_TEXTURE_3D_NV                   0x8859
#define GL_DEPENDENT_RGB_TEXTURE_CUBE_MAP_NV             0x885A
#define GL_DOT_PRODUCT_PASS_THROUGH_NV                   0x885B
#define GL_DOT_PRODUCT_TEXTURE_1D_NV                     0x885C
#define GL_DOT_PRODUCT_AFFINE_DEPTH_REPLACE_NV           0x885D
#define GL_HILO8_NV                                      0x885E
#define GL_SIGNED_HILO8_NV                               0x885F
#define GL_FORCE_BLUE_TO_ONE_NV                          0x8860
#endif

#if GL_ATI_point_cull_mode
#define GL_POINT_CULL_MODE_ATI                           0x60B3
#define GL_POINT_CULL_CENTER_ATI                         0x60B4
#define GL_POINT_CULL_CLIP_ATI                           0x60B5
#endif

#if GL_NV_point_sprite
#define GL_POINT_SPRITE_NV                               0x8861
#define GL_COORD_REPLACE_NV                              0x8862
#define GL_POINT_SPRITE_R_MODE_NV                        0x8863
#endif

#if GL_NV_depth_clamp
#define GL_DEPTH_CLAMP_NV                                0x864F
#endif

#if GL_NV_multisample_filter_hint
#define GL_MULTISAMPLE_FILTER_HINT_NV                    0x8534
#endif

#if GL_EXT_stencil_two_side
#define GL_STENCIL_TEST_TWO_SIDE_EXT                     0x8910
#define GL_ACTIVE_STENCIL_FACE_EXT                       0x8911
#endif

/*************************************************************/

#if GL_VERSION_1_2
extern void glBlendColor( GLclampf, GLclampf, GLclampf, GLclampf );
extern void glBlendEquation( GLenum );
extern void glDrawRangeElements( GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid * );
extern void glColorTable( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glColorTableParameterfv( GLenum, GLenum, const GLfloat * );
extern void glColorTableParameteriv( GLenum, GLenum, const GLint * );
extern void glCopyColorTable( GLenum, GLenum, GLint, GLint, GLsizei );
extern void glGetColorTable( GLenum, GLenum, GLenum, GLvoid * );
extern void glGetColorTableParameterfv( GLenum, GLenum, GLfloat * );
extern void glGetColorTableParameteriv( GLenum, GLenum, GLint * );
extern void glColorSubTable( GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glCopyColorSubTable( GLenum, GLsizei, GLint, GLint, GLsizei );
extern void glConvolutionFilter1D( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glConvolutionFilter2D( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glConvolutionParameterf( GLenum, GLenum, GLfloat );
extern void glConvolutionParameterfv( GLenum, GLenum, const GLfloat * );
extern void glConvolutionParameteri( GLenum, GLenum, GLint );
extern void glConvolutionParameteriv( GLenum, GLenum, const GLint * );
extern void glCopyConvolutionFilter1D( GLenum, GLenum, GLint, GLint, GLsizei );
extern void glCopyConvolutionFilter2D( GLenum, GLenum, GLint, GLint, GLsizei, GLsizei );
extern void glGetConvolutionFilter( GLenum, GLenum, GLenum, GLvoid * );
extern void glGetConvolutionParameterfv( GLenum, GLenum, GLfloat * );
extern void glGetConvolutionParameteriv( GLenum, GLenum, GLint * );
extern void glGetSeparableFilter( GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid * );
extern void glSeparableFilter2D( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid * );
extern void glGetHistogram( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
extern void glGetHistogramParameterfv( GLenum, GLenum, GLfloat * );
extern void glGetHistogramParameteriv( GLenum, GLenum, GLint * );
extern void glGetMinmax( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
extern void glGetMinmaxParameterfv( GLenum, GLenum, GLfloat * );
extern void glGetMinmaxParameteriv( GLenum, GLenum, GLint * );
extern void glHistogram( GLenum, GLsizei, GLenum, GLboolean );
extern void glMinmax( GLenum, GLenum, GLboolean );
extern void glResetHistogram( GLenum );
extern void glResetMinmax( GLenum );
extern void glTexImage3D( GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid * );
extern void glTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glCopyTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei );
#endif

#if GL_APPLE_texture_range
extern void glTextureRangeAPPLE( GLenum target, GLsizei length, const GLvoid *pointer );
extern void glGetTexParameterPointervAPPLE( GLenum target, GLenum pname, GLvoid **params );
#endif

#if GL_ARB_multitexture
extern void glActiveTextureARB( GLenum );
extern void glClientActiveTextureARB( GLenum );
extern void glMultiTexCoord1dARB( GLenum, GLdouble );
extern void glMultiTexCoord1dvARB( GLenum, const GLdouble * );
extern void glMultiTexCoord1fARB( GLenum, GLfloat );
extern void glMultiTexCoord1fvARB( GLenum, const GLfloat * );
extern void glMultiTexCoord1iARB( GLenum, GLint );
extern void glMultiTexCoord1ivARB( GLenum, const GLint * );
extern void glMultiTexCoord1sARB( GLenum, GLshort );
extern void glMultiTexCoord1svARB( GLenum, const GLshort * );
extern void glMultiTexCoord2dARB( GLenum, GLdouble, GLdouble );
extern void glMultiTexCoord2dvARB( GLenum, const GLdouble * );
extern void glMultiTexCoord2fARB( GLenum, GLfloat, GLfloat );
extern void glMultiTexCoord2fvARB( GLenum, const GLfloat * );
extern void glMultiTexCoord2iARB( GLenum, GLint, GLint );
extern void glMultiTexCoord2ivARB( GLenum, const GLint * );
extern void glMultiTexCoord2sARB( GLenum, GLshort, GLshort );
extern void glMultiTexCoord2svARB( GLenum, const GLshort * );
extern void glMultiTexCoord3dARB( GLenum, GLdouble, GLdouble, GLdouble );
extern void glMultiTexCoord3dvARB( GLenum, const GLdouble * );
extern void glMultiTexCoord3fARB( GLenum, GLfloat, GLfloat, GLfloat );
extern void glMultiTexCoord3fvARB( GLenum, const GLfloat * );
extern void glMultiTexCoord3iARB( GLenum, GLint, GLint, GLint );
extern void glMultiTexCoord3ivARB( GLenum, const GLint * );
extern void glMultiTexCoord3sARB( GLenum, GLshort, GLshort, GLshort );
extern void glMultiTexCoord3svARB( GLenum, const GLshort * );
extern void glMultiTexCoord4dARB( GLenum, GLdouble, GLdouble, GLdouble, GLdouble );
extern void glMultiTexCoord4dvARB( GLenum, const GLdouble * );
extern void glMultiTexCoord4fARB( GLenum, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glMultiTexCoord4fvARB( GLenum, const GLfloat * );
extern void glMultiTexCoord4iARB( GLenum, GLint, GLint, GLint, GLint );
extern void glMultiTexCoord4ivARB( GLenum, const GLint * );
extern void glMultiTexCoord4sARB( GLenum, GLshort, GLshort, GLshort, GLshort );
extern void glMultiTexCoord4svARB( GLenum, const GLshort * );
#endif

#if GL_ARB_transpose_matrix
extern void glLoadTransposeMatrixfARB( const GLfloat * );
extern void glLoadTransposeMatrixdARB( const GLdouble * );
extern void glMultTransposeMatrixfARB( const GLfloat * );
extern void glMultTransposeMatrixdARB( const GLdouble * );
#endif

#if GL_ARB_multisample
extern void glSampleCoverageARB( GLclampf, GLboolean );
extern void glSamplePassARB( GLenum );
#endif

#if GL_ARB_texture_compression
extern void glCompressedTexImage3DARB( GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid * );
extern void glCompressedTexImage2DARB( GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid * );
extern void glCompressedTexImage1DARB( GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid * );
extern void glCompressedTexSubImage3DARB( GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid * );
extern void glCompressedTexSubImage2DARB( GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid * );
extern void glCompressedTexSubImage1DARB( GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid * );
extern void glGetCompressedTexImageARB( GLenum, GLint, GLvoid * );
#endif

#if GL_ARB_vertex_blend
extern void glWeightbvARB( GLint, const GLbyte * );
extern void glWeightsvARB( GLint, const GLshort * );
extern void glWeightivARB( GLint, const GLint * );
extern void glWeightfvARB( GLint, const GLfloat * );
extern void glWeightdvARB( GLint, const GLdouble * );
extern void glWeightubvARB( GLint, const GLubyte * );
extern void glWeightusvARB( GLint, const GLushort * );
extern void glWeightuivARB( GLint, const GLuint * );
extern void glWeightPointerARB( GLint, GLenum, GLsizei, const GLvoid * );
extern void glVertexBlendARB( GLint );
#endif

#if GL_EXT_blend_color
extern void glBlendColorEXT( GLclampf, GLclampf, GLclampf, GLclampf );
#endif

#if GL_EXT_polygon_offset
extern void glPolygonOffsetEXT( GLfloat, GLfloat );
#endif

#if GL_EXT_texture3D
extern void glTexImage3DEXT( GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid * );
extern void glTexSubImage3DEXT( GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
#endif

#if GL_SGIS_texture_filter4
extern void glGetTexFilterFuncSGIS( GLenum, GLenum, GLfloat * );
extern void glTexFilterFuncSGIS( GLenum, GLenum, GLsizei, const GLfloat * );
#endif

#if GL_EXT_subtexture
extern void glTexSubImage1DEXT( GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glTexSubImage2DEXT( GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
#endif

#if GL_EXT_copy_texture
extern void glCopyTexImage1DEXT( GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint );
extern void glCopyTexImage2DEXT( GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint );
extern void glCopyTexSubImage1DEXT( GLenum, GLint, GLint, GLint, GLint, GLsizei );
extern void glCopyTexSubImage2DEXT( GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei );
extern void glCopyTexSubImage3DEXT( GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei );
#endif

#if GL_EXT_histogram
extern void glGetHistogramEXT( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
extern void glGetHistogramParameterfvEXT( GLenum, GLenum, GLfloat * );
extern void glGetHistogramParameterivEXT( GLenum, GLenum, GLint * );
extern void glGetMinmaxEXT( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
extern void glGetMinmaxParameterfvEXT( GLenum, GLenum, GLfloat * );
extern void glGetMinmaxParameterivEXT( GLenum, GLenum, GLint * );
extern void glHistogramEXT( GLenum, GLsizei, GLenum, GLboolean );
extern void glMinmaxEXT( GLenum, GLenum, GLboolean );
extern void glResetHistogramEXT( GLenum );
extern void glResetMinmaxEXT( GLenum );
#endif

#if GL_EXT_convolution
extern void glConvolutionFilter1DEXT( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glConvolutionFilter2DEXT( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glConvolutionParameterfEXT( GLenum, GLenum, GLfloat );
extern void glConvolutionParameterfvEXT( GLenum, GLenum, const GLfloat * );
extern void glConvolutionParameteriEXT( GLenum, GLenum, GLint );
extern void glConvolutionParameterivEXT( GLenum, GLenum, const GLint * );
extern void glCopyConvolutionFilter1DEXT( GLenum, GLenum, GLint, GLint, GLsizei );
extern void glCopyConvolutionFilter2DEXT( GLenum, GLenum, GLint, GLint, GLsizei, GLsizei );
extern void glGetConvolutionFilterEXT( GLenum, GLenum, GLenum, GLvoid * );
extern void glGetConvolutionParameterfvEXT( GLenum, GLenum, GLfloat * );
extern void glGetConvolutionParameterivEXT( GLenum, GLenum, GLint * );
extern void glGetSeparableFilterEXT( GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid * );
extern void glSeparableFilter2DEXT( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid * );
#endif

#if GL_SGI_color_table
extern void glColorTableSGI( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glColorTableParameterfvSGI( GLenum, GLenum, const GLfloat * );
extern void glColorTableParameterivSGI( GLenum, GLenum, const GLint * );
extern void glCopyColorTableSGI( GLenum, GLenum, GLint, GLint, GLsizei );
extern void glGetColorTableSGI( GLenum, GLenum, GLenum, GLvoid * );
extern void glGetColorTableParameterfvSGI( GLenum, GLenum, GLfloat * );
extern void glGetColorTableParameterivSGI( GLenum, GLenum, GLint * );
#endif

#if GL_SGIX_pixel_texture
extern void glPixelTexGenSGIX( GLenum );
#endif

#if GL_SGIS_pixel_texture
extern void glPixelTexGenParameteriSGIS( GLenum, GLint );
extern void glPixelTexGenParameterivSGIS( GLenum, const GLint * );
extern void glPixelTexGenParameterfSGIS( GLenum, GLfloat );
extern void glPixelTexGenParameterfvSGIS( GLenum, const GLfloat * );
extern void glGetPixelTexGenParameterivSGIS( GLenum, GLint * );
extern void glGetPixelTexGenParameterfvSGIS( GLenum, GLfloat * );
#endif

#if GL_SGIS_texture4D
extern void glTexImage4DSGIS( GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid * );
extern void glTexSubImage4DSGIS( GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
#endif

#if GL_EXT_texture_object
extern GLboolean glAreTexturesResidentEXT( GLsizei, const GLuint *, GLboolean * );
extern void glBindTextureEXT( GLenum, GLuint );
extern void glDeleteTexturesEXT( GLsizei, const GLuint * );
extern void glGenTexturesEXT( GLsizei, GLuint * );
extern GLboolean glIsTextureEXT( GLuint );
extern void glPrioritizeTexturesEXT( GLsizei, const GLuint *, const GLclampf * );
#endif

#if GL_SGIS_detail_texture
extern void glDetailTexFuncSGIS( GLenum, GLsizei, const GLfloat * );
extern void glGetDetailTexFuncSGIS( GLenum, GLfloat * );
#endif

#if GL_SGIS_sharpen_texture
extern void glSharpenTexFuncSGIS( GLenum, GLsizei, const GLfloat * );
extern void glGetSharpenTexFuncSGIS( GLenum, GLfloat * );
#endif

#if GL_SGIS_multisample
extern void glSampleMaskSGIS( GLclampf, GLboolean );
extern void glSamplePatternSGIS( GLenum );
#endif

#if GL_EXT_vertex_array
extern void glArrayElementEXT( GLint );
extern void glColorPointerEXT( GLint, GLenum, GLsizei, GLsizei, const GLvoid * );
extern void glDrawArraysEXT( GLenum, GLint, GLsizei );
extern void glEdgeFlagPointerEXT( GLsizei, GLsizei, const GLvoid * );
extern void glGetPointervEXT( GLenum, GLvoid * * );
extern void glIndexPointerEXT( GLenum, GLsizei, GLsizei, const GLvoid * );
extern void glNormalPointerEXT( GLenum, GLsizei, GLsizei, const GLvoid * );
extern void glTexCoordPointerEXT( GLint, GLenum, GLsizei, GLsizei, const GLvoid * );
extern void glVertexPointerEXT( GLint, GLenum, GLsizei, GLsizei, const GLvoid * );
#endif

#if GL_EXT_blend_minmax
extern void glBlendEquationEXT( GLenum );
#endif

#if GL_SGIX_sprite
extern void glSpriteParameterfSGIX( GLenum, GLfloat );
extern void glSpriteParameterfvSGIX( GLenum, const GLfloat * );
extern void glSpriteParameteriSGIX( GLenum, GLint );
extern void glSpriteParameterivSGIX( GLenum, const GLint * );
#endif

#if GL_SGIX_instruments
extern GLint glGetInstrumentsSGIX( void );
extern void glInstrumentsBufferSGIX( GLsizei, GLint * );
extern GLint glPollInstrumentsSGIX( GLint * );
extern void glReadInstrumentsSGIX( GLint );
extern void glStartInstrumentsSGIX( void );
extern void glStopInstrumentsSGIX( GLint );
#endif

#if GL_SGIX_framezoom
extern void glFrameZoomSGIX( GLint );
#endif

#if GL_SGIX_tag_sample_buffer
extern void glTagSampleBufferSGIX( void );
#endif

#if GL_SGIX_polynomial_ffd
extern void glDeformationMap3dSGIX( GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble * );
extern void glDeformationMap3fSGIX( GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat * );
extern void glDeformSGIX( GLbitfield );
extern void glLoadIdentityDeformationMapSGIX( GLbitfield );
#endif

#if GL_SGIX_reference_plane
extern void glReferencePlaneSGIX( const GLdouble * );
#endif

#if GL_SGIX_flush_raster
extern void glFlushRasterSGIX( void );
#endif

#if GL_SGIS_fog_function
extern void glFogFuncSGIS( GLsizei, const GLfloat * );
extern void glGetFogFuncSGIS( const GLfloat * );
#endif

#if GL_HP_image_transform
extern void glImageTransformParameteriHP( GLenum, GLenum, GLint );
extern void glImageTransformParameterfHP( GLenum, GLenum, GLfloat );
extern void glImageTransformParameterivHP( GLenum, GLenum, const GLint * );
extern void glImageTransformParameterfvHP( GLenum, GLenum, const GLfloat * );
extern void glGetImageTransformParameterivHP( GLenum, GLenum, GLint * );
extern void glGetImageTransformParameterfvHP( GLenum, GLenum, GLfloat * );
#endif

#if GL_EXT_color_subtable
extern void glColorSubTableEXT( GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glCopyColorSubTableEXT( GLenum, GLsizei, GLint, GLint, GLsizei );
#endif

#if GL_PGI_misc_hints
extern void glHintPGI( GLenum, GLint );
#endif

#if GL_EXT_paletted_texture
extern void glColorTableEXT( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glColorSubTableEXT( GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void glGetColorTableEXT( GLenum, GLenum, GLenum, GLvoid * );
extern void glGetColorTableParameterivEXT( GLenum, GLenum, GLint * );
extern void glGetColorTableParameterfvEXT( GLenum, GLenum, GLfloat * );
#endif

#if GL_SGIX_list_priority
extern void glGetListParameterfvSGIX( GLuint, GLenum, GLfloat * );
extern void glGetListParameterivSGIX( GLuint, GLenum, GLint * );
extern void glListParameterfSGIX( GLuint, GLenum, GLfloat );
extern void glListParameterfvSGIX( GLuint, GLenum, const GLfloat * );
extern void glListParameteriSGIX( GLuint, GLenum, GLint );
extern void glListParameterivSGIX( GLuint, GLenum, const GLint * );
#endif

#if GL_EXT_index_material
extern void glIndexMaterialEXT( GLenum, GLenum );
#endif

#if GL_EXT_index_func
extern void glIndexFuncEXT( GLenum, GLclampf );
#endif

#if GL_EXT_compiled_vertex_array
extern void glLockArraysEXT( GLint, GLsizei );
extern void glUnlockArraysEXT( void );
#endif

#if GL_EXT_cull_vertex
extern void glCullParameterdvEXT( GLenum, GLdouble * );
extern void glCullParameterfvEXT( GLenum, GLfloat * );
#endif

#if GL_SGIX_fragment_lighting
extern void glFragmentColorMaterialSGIX( GLenum, GLenum );
extern void glFragmentLightfSGIX( GLenum, GLenum, GLfloat );
extern void glFragmentLightfvSGIX( GLenum, GLenum, const GLfloat * );
extern void glFragmentLightiSGIX( GLenum, GLenum, GLint );
extern void glFragmentLightivSGIX( GLenum, GLenum, const GLint * );
extern void glFragmentLightModelfSGIX( GLenum, GLfloat );
extern void glFragmentLightModelfvSGIX( GLenum, const GLfloat * );
extern void glFragmentLightModeliSGIX( GLenum, GLint );
extern void glFragmentLightModelivSGIX( GLenum, const GLint * );
extern void glFragmentMaterialfSGIX( GLenum, GLenum, GLfloat );
extern void glFragmentMaterialfvSGIX( GLenum, GLenum, const GLfloat * );
extern void glFragmentMaterialiSGIX( GLenum, GLenum, GLint );
extern void glFragmentMaterialivSGIX( GLenum, GLenum, const GLint * );
extern void glGetFragmentLightfvSGIX( GLenum, GLenum, GLfloat * );
extern void glGetFragmentLightivSGIX( GLenum, GLenum, GLint * );
extern void glGetFragmentMaterialfvSGIX( GLenum, GLenum, GLfloat * );
extern void glGetFragmentMaterialivSGIX( GLenum, GLenum, GLint * );
extern void glLightEnviSGIX( GLenum, GLint );
#endif

#if GL_EXT_draw_range_elements
extern void glDrawRangeElementsEXT( GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid * );
#endif

#if GL_EXT_light_texture
extern void glApplyTextureEXT( GLenum );
extern void glTextureLightEXT( GLenum );
extern void glTextureMaterialEXT( GLenum, GLenum );
#endif

#if GL_SGIX_async
extern void glAsyncMarkerSGIX( GLuint );
extern GLint glFinishAsyncSGIX( GLuint * );
extern GLint glPollAsyncSGIX( GLuint * );
extern GLuint glGenAsyncMarkersSGIX( GLsizei );
extern void glDeleteAsyncMarkersSGIX( GLuint, GLsizei );
extern GLboolean glIsAsyncMarkerSGIX( GLuint );
#endif

#if GL_INTEL_parallel_arrays
extern void glVertexPointervINTEL( GLint, GLenum, const GLvoid * * );
extern void glNormalPointervINTEL( GLenum, const GLvoid * * );
extern void glColorPointervINTEL( GLint, GLenum, const GLvoid * * );
extern void glTexCoordPointervINTEL( GLint, GLenum, const GLvoid * * );
#endif

#if GL_EXT_pixel_transform
extern void glPixelTransformParameteriEXT( GLenum, GLenum, GLint );
extern void glPixelTransformParameterfEXT( GLenum, GLenum, GLfloat );
extern void glPixelTransformParameterivEXT( GLenum, GLenum, const GLint * );
extern void glPixelTransformParameterfvEXT( GLenum, GLenum, const GLfloat * );
#endif

#if GL_EXT_secondary_color
extern void glSecondaryColor3bEXT( GLbyte, GLbyte, GLbyte );
extern void glSecondaryColor3bvEXT( const GLbyte * );
extern void glSecondaryColor3dEXT( GLdouble, GLdouble, GLdouble );
extern void glSecondaryColor3dvEXT( const GLdouble * );
extern void glSecondaryColor3fEXT( GLfloat, GLfloat, GLfloat );
extern void glSecondaryColor3fvEXT( const GLfloat * );
extern void glSecondaryColor3iEXT( GLint, GLint, GLint );
extern void glSecondaryColor3ivEXT( const GLint * );
extern void glSecondaryColor3sEXT( GLshort, GLshort, GLshort );
extern void glSecondaryColor3svEXT( const GLshort * );
extern void glSecondaryColor3ubEXT( GLubyte, GLubyte, GLubyte );
extern void glSecondaryColor3ubvEXT( const GLubyte * );
extern void glSecondaryColor3uiEXT( GLuint, GLuint, GLuint );
extern void glSecondaryColor3uivEXT( const GLuint * );
extern void glSecondaryColor3usEXT( GLushort, GLushort, GLushort );
extern void glSecondaryColor3usvEXT( const GLushort * );
extern void glSecondaryColorPointerEXT( GLint, GLenum, GLsizei, const GLvoid * );
#endif

#if GL_EXT_texture_perturb_normal
extern void glTextureNormalEXT( GLenum );
#endif

#if GL_EXT_multi_draw_arrays
extern void glMultiDrawArraysEXT( GLenum, const GLint *, const GLsizei *, GLsizei );
extern void glMultiDrawElementsEXT( GLenum, const GLsizei *, GLenum, const GLvoid * *, GLsizei );
#endif

#if GL_EXT_fog_coord
extern void glFogCoordfEXT( GLfloat );
extern void glFogCoordfvEXT( const GLfloat * );
extern void glFogCoorddEXT( GLdouble );
extern void glFogCoorddvEXT( const GLdouble * );
extern void glFogCoordPointerEXT( GLenum, GLsizei, const GLvoid * );
#endif

#if GL_APPLE_vertex_array_range
extern void glVertexArrayRangeAPPLE( GLsizei length, const GLvoid *pointer );
extern void glFlushVertexArrayRangeAPPLE( GLsizei length, const GLvoid *pointer );
extern void glVertexArrayParameteriAPPLE( GLenum pname, GLint param );
#endif

#if GL_APPLE_vertex_array_object
extern void glBindVertexArrayAPPLE( GLuint id );
extern void glDeleteVertexArraysAPPLE( GLsizei n, const GLuint *ids );
extern void glGenVertexArraysAPPLE( GLsizei n, GLuint *ids );
extern GLboolean glIsVertexArrayAPPLE( GLuint id );
#endif

#if GL_APPLE_fence
extern void glGenFencesAPPLE( GLsizei n, GLuint *fences );
extern void glDeleteFencesAPPLE( GLsizei n, const GLuint *fences );
extern void glSetFenceAPPLE( GLuint fence );
extern GLboolean glIsFenceAPPLE( GLuint fence );
extern GLboolean glTestFenceAPPLE( GLuint fence );
extern void glFinishFenceAPPLE( GLuint fence );
extern GLboolean glTestObjectAPPLE( GLenum object, GLuint name );
extern void glFinishObjectAPPLE( GLenum object, GLuint name );
#endif

#if GL_APPLE_element_array
extern void glElementPointerAPPLE( GLenum type, const GLvoid *pointer );
extern void glDrawElementArrayAPPLE( GLenum mode, GLint first, GLsizei count );
extern void glDrawRangeElementArrayAPPLE( GLenum mode, GLuint start, GLuint end, GLint first, GLsizei count );
#endif

#if GL_EXT_coordinate_frame
extern void glTangent3bEXT( GLbyte, GLbyte, GLbyte );
extern void glTangent3bvEXT( const GLbyte * );
extern void glTangent3dEXT( GLdouble, GLdouble, GLdouble );
extern void glTangent3dvEXT( const GLdouble * );
extern void glTangent3fEXT( GLfloat, GLfloat, GLfloat );
extern void glTangent3fvEXT( const GLfloat * );
extern void glTangent3iEXT( GLint, GLint, GLint );
extern void glTangent3ivEXT( const GLint * );
extern void glTangent3sEXT( GLshort, GLshort, GLshort );
extern void glTangent3svEXT( const GLshort * );
extern void glBinormal3bEXT( GLbyte, GLbyte, GLbyte );
extern void glBinormal3bvEXT( const GLbyte * );
extern void glBinormal3dEXT( GLdouble, GLdouble, GLdouble );
extern void glBinormal3dvEXT( const GLdouble * );
extern void glBinormal3fEXT( GLfloat, GLfloat, GLfloat );
extern void glBinormal3fvEXT( const GLfloat * );
extern void glBinormal3iEXT( GLint, GLint, GLint );
extern void glBinormal3ivEXT( const GLint * );
extern void glBinormal3sEXT( GLshort, GLshort, GLshort );
extern void glBinormal3svEXT( const GLshort * );
extern void glTangentPointerEXT( GLenum, GLsizei, const GLvoid * );
extern void glBinormalPointerEXT( GLenum, GLsizei, const GLvoid * );
#endif

#if GL_SUNX_constant_data
extern void glFinishTextureSUNX( void );
#endif

#if GL_SUN_global_alpha
extern void glGlobalAlphaFactorbSUN( GLbyte );
extern void glGlobalAlphaFactorsSUN( GLshort );
extern void glGlobalAlphaFactoriSUN( GLint );
extern void glGlobalAlphaFactorfSUN( GLfloat );
extern void glGlobalAlphaFactordSUN( GLdouble );
extern void glGlobalAlphaFactorubSUN( GLubyte );
extern void glGlobalAlphaFactorusSUN( GLushort );
extern void glGlobalAlphaFactoruiSUN( GLuint );
#endif

#if GL_SUN_triangle_list
extern void glReplacementCodeuiSUN( GLuint );
extern void glReplacementCodeusSUN( GLushort );
extern void glReplacementCodeubSUN( GLubyte );
extern void glReplacementCodeuivSUN( const GLuint * );
extern void glReplacementCodeusvSUN( const GLushort * );
extern void glReplacementCodeubvSUN( const GLubyte * );
extern void glReplacementCodePointerSUN( GLenum, GLsizei, const GLvoid * * );
#endif

#if GL_SUN_vertex
extern void glColor4ubVertex2fSUN( GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat );
extern void glColor4ubVertex2fvSUN( const GLubyte *, const GLfloat * );
extern void glColor4ubVertex3fSUN( GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat, GLfloat );
extern void glColor4ubVertex3fvSUN( const GLubyte *, const GLfloat * );
extern void glColor3fVertex3fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glColor3fVertex3fvSUN( const GLfloat *, const GLfloat * );
extern void glNormal3fVertex3fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glNormal3fVertex3fvSUN( const GLfloat *, const GLfloat * );
extern void glColor4fNormal3fVertex3fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glColor4fNormal3fVertex3fvSUN( const GLfloat *, const GLfloat *, const GLfloat * );
extern void glTexCoord2fVertex3fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glTexCoord2fVertex3fvSUN( const GLfloat *, const GLfloat * );
extern void glTexCoord4fVertex4fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glTexCoord4fVertex4fvSUN( const GLfloat *, const GLfloat * );
extern void glTexCoord2fColor4ubVertex3fSUN( GLfloat, GLfloat, GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat, GLfloat );
extern void glTexCoord2fColor4ubVertex3fvSUN( const GLfloat *, const GLubyte *, const GLfloat * );
extern void glTexCoord2fColor3fVertex3fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glTexCoord2fColor3fVertex3fvSUN( const GLfloat *, const GLfloat *, const GLfloat * );
extern void glTexCoord2fNormal3fVertex3fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glTexCoord2fNormal3fVertex3fvSUN( const GLfloat *, const GLfloat *, const GLfloat * );
extern void glTexCoord2fColor4fNormal3fVertex3fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glTexCoord2fColor4fNormal3fVertex3fvSUN( const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat * );
extern void glTexCoord4fColor4fNormal3fVertex4fSUN( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glTexCoord4fColor4fNormal3fVertex4fvSUN( const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat * );
extern void glReplacementCodeuiVertex3fSUN( GLenum, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiVertex3fvSUN( const GLenum *, const GLfloat * );
extern void glReplacementCodeuiColor4ubVertex3fSUN( GLenum, GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiColor4ubVertex3fvSUN( const GLenum *, const GLubyte *, const GLfloat * );
extern void glReplacementCodeuiColor3fVertex3fSUN( GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiColor3fVertex3fvSUN( const GLenum *, const GLfloat *, const GLfloat * );
extern void glReplacementCodeuiNormal3fVertex3fSUN( GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiNormal3fVertex3fvSUN( const GLenum *, const GLfloat *, const GLfloat * );
extern void glReplacementCodeuiColor4fNormal3fVertex3fSUN( GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiColor4fNormal3fVertex3fvSUN( const GLenum *, const GLfloat *, const GLfloat *, const GLfloat * );
extern void glReplacementCodeuiTexCoord2fVertex3fSUN( GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiTexCoord2fVertex3fvSUN( const GLenum *, const GLfloat *, const GLfloat * );
extern void glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN( GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN( const GLenum *, const GLfloat *, const GLfloat *, const GLfloat * );
extern void glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN( GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
extern void glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN( const GLenum *, const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat * );
#endif

#if GL_EXT_blend_func_separate
extern void glBlendFuncSeparateEXT( GLenum, GLenum, GLenum, GLenum );
#endif

#if GL_EXT_vertex_weighting
extern void glVertexWeightfEXT( GLfloat );
extern void glVertexWeightfvEXT( const GLfloat * );
extern void glVertexWeightPointerEXT( GLsizei, GLenum, GLsizei, const GLvoid * );
#endif

#if GL_NV_vertex_array_range
extern void glFlushVertexArrayRangeNV( void );
extern void glVertexArrayRangeNV( GLsizei, const GLvoid * );
#endif

#if GL_NV_register_combiners
extern void glCombinerParameterfvNV( GLenum, const GLfloat * );
extern void glCombinerParameterfNV( GLenum, GLfloat );
extern void glCombinerParameterivNV( GLenum, const GLint * );
extern void glCombinerParameteriNV( GLenum, GLint );
extern void glCombinerInputNV( GLenum, GLenum, GLenum, GLenum, GLenum, GLenum );
extern void glCombinerOutputNV( GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean );
extern void glFinalCombinerInputNV( GLenum, GLenum, GLenum, GLenum );
extern void glGetCombinerInputParameterfvNV( GLenum, GLenum, GLenum, GLenum, GLfloat * );
extern void glGetCombinerInputParameterivNV( GLenum, GLenum, GLenum, GLenum, GLint * );
extern void glGetCombinerOutputParameterfvNV( GLenum, GLenum, GLenum, GLfloat * );
extern void glGetCombinerOutputParameterivNV( GLenum, GLenum, GLenum, GLint * );
extern void glGetFinalCombinerInputParameterfvNV( GLenum, GLenum, GLfloat * );
extern void glGetFinalCombinerInputParameterivNV( GLenum, GLenum, GLint * );
#endif

#if GL_NV_register_combiners2
extern void glCombinerStageParameterfvNV( GLenum, GLenum, const GLfloat * );
extern void glGetCombinerStageParameterfvNV( GLenum, GLenum, GLfloat * );
#endif

#if GL_EXT_vertex_shader
extern void glBeginVertexShaderEXT( void );
extern void glEndVertexShaderEXT( void );
extern void glBindVertexShaderEXT( GLuint id );
extern GLuint glGenVertexShadersEXT( GLuint range );
extern void glDeleteVertexShaderEXT( GLuint id );
extern void glShaderOp1EXT( GLenum op, GLuint res, GLuint arg1 );
extern void glShaderOp2EXT( GLenum op, GLuint res, GLuint arg1, GLuint arg2 );
extern void glShaderOp3EXT( GLenum op, GLuint res, GLuint arg1, GLuint arg2, GLuint arg3 );
extern void glSwizzleEXT( GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, GLenum outW );
extern void glWriteMaskEXT( GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, GLenum outW );
extern void glInsertComponentEXT( GLuint res, GLuint src, GLuint num );
extern void glExtractComponentEXT( GLuint res, GLuint src, GLuint num );
extern GLuint glGenSymbolsEXT( GLenum datatype, GLenum storagetype, GLenum range, GLuint components );
extern void glSetInvariantEXT( GLuint id, GLenum type, GLvoid *addr );
extern void glSetLocalConstantEXT( GLuint id, GLenum type, GLvoid *addr );
extern void glVariantbvEXT( GLuint id, GLbyte *addr );
extern void glVariantdvEXT( GLuint id, GLdouble *addr );
extern void glVariantfvEXT( GLuint id, GLfloat *addr );
extern void glVariantivEXT( GLuint id, GLint *addr );
extern void glVariantsvEXT( GLuint id, GLshort *addr );
extern void glVariantubvEXT( GLuint id, GLubyte *addr );
extern void glVariantuivEXT( GLuint id, GLuint *addr );
extern void glVariantusvEXT( GLuint id, GLushort *addr );
extern void glVariantPointerEXT( GLuint id, GLenum type, GLuint stride, GLvoid *addr );
extern void glEnableVariantClientStateEXT( GLuint id );
extern void glDisableVariantClientStateEXT( GLuint id );
extern GLuint glBindLightParameterEXT( GLenum light, GLenum value );
extern GLuint glBindMaterialParameterEXT( GLenum face, GLenum value );
extern GLuint glBindTexGenParameterEXT( GLenum unit, GLenum coord, GLenum value );
extern GLuint glBindTextureUnitParameterEXT( GLenum unit, GLenum value );
extern GLuint glBindParameterEXT( GLenum value );
extern GLboolean glIsVariantEnabledEXT( GLuint id, GLenum cap );
extern void glGetVariantBooleanvEXT( GLuint id, GLenum value, GLboolean *data );
extern void glGetVariantIntegervEXT( GLuint id, GLenum value, GLint *data );
extern void glGetVariantFloatvEXT( GLuint id, GLenum value, GLfloat *data );
extern void glGetVariantPointervEXT( GLuint id, GLenum value, GLvoid **data );
extern void glGetInvariantBooleanvEXT( GLuint id, GLenum value, GLboolean *data );
extern void glGetInvariantIntegervEXT( GLuint id, GLenum value, GLint *data );
extern void glGetInvariantFloatvEXT( GLuint id, GLenum value, GLfloat *data );
extern void glGetLocalConstantBooleanvEXT( GLuint id, GLenum value, GLboolean *data );
extern void glGetLocalConstantIntegervEXT( GLuint id, GLenum value, GLint *data );
extern void glGetLocalConstantFloatvEXT( GLuint id, GLenum value, GLfloat *data );
#endif

#if GL_EXT_fragment_shader
extern GLuint glGenFragmentShadersEXT( GLuint range );
extern void glBindFragmentShaderEXT( GLuint id );
extern void glDeleteFragmentShaderEXT( GLuint id );
extern void glBeginFragmentShaderEXT( void );
extern void glEndFragmentShaderEXT( void );
extern void glPassTexCoordEXT( GLuint dst, GLuint coord, GLenum swizzle );
extern void glSampleMapEXT( GLuint dst, GLuint interp, GLenum swizzle );
extern void glColorFragmentOp1EXT( GLenum op, GLuint dst, GLuint dstMask,
								   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
								   GLuint arg1Mod );
extern void glColorFragmentOp2EXT( GLenum op, GLuint dst, GLuint dstMask,
								   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
								   GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
								   GLuint arg2Mod );
extern void glColorFragmentOp3EXT( GLenum op, GLuint dst, GLuint dstMask,
								   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
								   GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
								   GLuint arg2Mod, GLuint arg3, GLuint arg3Rep,
								   GLuint arg3Mod );
extern void glAlphaFragmentOp1EXT( GLenum op, GLuint dst, GLuint dstMod,
								   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod );
extern void glAlphaFragmentOp2EXT( GLenum op, GLuint dst, GLuint dstMod,
								   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
								   GLuint arg2, GLuint arg2Rep, GLuint arg2Mod );
extern void glAlphaFragmentOp3EXT( GLenum op, GLuint dst, GLuint dstMod,
								   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
								   GLuint arg2, GLuint arg2Rep, GLuint arg2Mod,
								   GLuint arg3, GLuint arg3Rep, GLuint arg3Mod );
extern void glSetFragmentShaderConstantEXT( GLuint dst, const GLfloat *value );
#endif

#if GL_MESA_resize_buffers
extern void glResizeBuffersMESA( void );
#endif

#if GL_ARB_window_pos
extern void glWindowPos2dARB( GLdouble, GLdouble );
extern void glWindowPos2dvARB( const GLdouble * );
extern void glWindowPos2fARB( GLfloat, GLfloat );
extern void glWindowPos2fvARB( const GLfloat * );
extern void glWindowPos2iARB( GLint, GLint );
extern void glWindowPos2ivARB( const GLint * );
extern void glWindowPos2sARB( GLshort, GLshort );
extern void glWindowPos2svARB( const GLshort * );
extern void glWindowPos3dARB( GLdouble, GLdouble, GLdouble );
extern void glWindowPos3dvARB( const GLdouble * );
extern void glWindowPos3fARB( GLfloat, GLfloat, GLfloat );
extern void glWindowPos3fvARB( const GLfloat * );
extern void glWindowPos3iARB( GLint, GLint, GLint );
extern void glWindowPos3ivARB( const GLint * );
extern void glWindowPos3sARB( GLshort, GLshort, GLshort );
extern void glWindowPos3svARB( const GLshort * );
#endif

#if GL_IBM_multimode_draw_arrays
extern void glMultiModeDrawArraysIBM( GLenum, const GLint *, const GLsizei *, GLsizei, GLint );
extern void glMultiModeDrawElementsIBM( const GLenum *, const GLsizei *, GLenum, const GLvoid * *, GLsizei, GLint );
#endif

#if GL_IBM_vertex_array_lists
extern void glColorPointerListIBM( GLint, GLenum, GLint, const GLvoid * *, GLint );
extern void glSecondaryColorPointerListIBM( GLint, GLenum, GLint, const GLvoid * *, GLint );
extern void glEdgeFlagPointerListIBM( GLint, const GLboolean * *, GLint );
extern void glFogCoordPointerListIBM( GLenum, GLint, const GLvoid * *, GLint );
extern void glIndexPointerListIBM( GLenum, GLint, const GLvoid * *, GLint );
extern void glNormalPointerListIBM( GLenum, GLint, const GLvoid * *, GLint );
extern void glTexCoordPointerListIBM( GLint, GLenum, GLint, const GLvoid * *, GLint );
extern void glVertexPointerListIBM( GLint, GLenum, GLint, const GLvoid * *, GLint );
#endif

#if GL_3DFX_tbuffer
extern void glTbufferMask3DFX( GLuint );
#endif

#if GL_EXT_multisample
extern void glSampleMaskEXT( GLclampf, GLboolean );
extern void glSamplePatternEXT( GLenum );
#endif

#if GL_SGIS_texture_color_mask
extern void glTextureColorMaskSGIS( GLboolean, GLboolean, GLboolean, GLboolean );
#endif

#if GL_SGIX_igloo_interface
extern void glIglooInterfaceSGIX( GLenum, const GLvoid * );
#endif

#if GL_NV_vertex_program
extern void glBindProgramNV( GLenum target, GLuint id );
extern void glDeleteProgramsNV( GLsizei n, const GLuint *ids );
extern void glExecuteProgramNV( GLenum target, GLuint id, const GLfloat *params );
extern void glGenProgramsNV( GLsizei n, GLuint *ids );
extern GLboolean glAreProgramsResidentNV( GLsizei n, const GLuint *ids, GLboolean *residences );
extern void glRequestResidentProgramsNV( GLsizei n, GLuint *ids );
extern void glGetProgramParameterfvNV( GLenum target, GLuint index, GLenum pname, GLfloat *params );
extern void glGetProgramParameterdvNV( GLenum target, GLuint index, GLenum pname, GLdouble *params );
extern void glGetProgramivNV( GLuint id, GLenum pname, GLint *params );
extern void glGetProgramStringNV( GLuint id, GLenum pname, GLubyte *program );
extern void glGetTrackMatrixivNV( GLenum target, GLuint address, GLenum pname, GLint *params );
extern void glGetVertexAttribdvNV( GLuint index, GLenum pname, GLdouble *params );
extern void glGetVertexAttribfvNV( GLuint index, GLenum pname, GLfloat *params );
extern void glGetVertexAttribivNV( GLuint index, GLenum pname, GLint *params );
extern void glGetVertexAttribPointervNV( GLuint index, GLenum pname, GLvoid **pointer );
extern GLboolean glIsProgramNV( GLuint id );
extern void glLoadProgramNV( GLenum target, GLuint id, GLsizei len, const GLubyte *program );
extern void glProgramParameter4fNV( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void glProgramParameter4dNV( GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void glProgramParameter4dvNV( GLenum target, GLuint index, const GLdouble *params );
extern void glProgramParameter4fvNV( GLenum target, GLuint index, const GLfloat *params );
extern void glProgramParameters4dvNV( GLenum target, GLuint index, GLuint num, const GLdouble *params );
extern void glProgramParameters4fvNV( GLenum target, GLuint index, GLuint num, const GLfloat *params );
extern void glTrackMatrixNV( GLenum target, GLuint address, GLenum matrix, GLenum transform );
extern void glVertexAttribPointerNV( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
extern void glVertexAttrib1sNV( GLuint index, GLshort x );
extern void glVertexAttrib1fNV( GLuint index, GLfloat x );
extern void glVertexAttrib1dNV( GLuint index, GLdouble x );
extern void glVertexAttrib2sNV( GLuint index, GLshort x, GLshort y );
extern void glVertexAttrib2fNV( GLuint index, GLfloat x, GLfloat y );
extern void glVertexAttrib2dNV( GLuint index, GLdouble x, GLdouble y );
extern void glVertexAttrib3sNV( GLuint index, GLshort x, GLshort y, GLshort z );
extern void glVertexAttrib3fNV( GLuint index, GLfloat x, GLfloat y, GLfloat z );
extern void glVertexAttrib3dNV( GLuint index, GLdouble x, GLdouble y, GLdouble z );
extern void glVertexAttrib4sNV( GLuint index, GLshort x, GLshort y, GLshort z, GLshort w );
extern void glVertexAttrib4fNV( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void glVertexAttrib4dNV( GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void glVertexAttrib4ubNV( GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w );
extern void glVertexAttrib1svNV( GLuint index, GLshort *v );
extern void glVertexAttrib1fvNV( GLuint index, GLfloat *v );
extern void glVertexAttrib1dvNV( GLuint index, GLdouble *v );
extern void glVertexAttrib2svNV( GLuint index, GLshort *v );
extern void glVertexAttrib2fvNV( GLuint index, GLfloat *v );
extern void glVertexAttrib2dvNV( GLuint index, GLdouble *v );
extern void glVertexAttrib3svNV( GLuint index, GLshort *v );
extern void glVertexAttrib3fvNV( GLuint index, GLfloat *v );
extern void glVertexAttrib3dvNV( GLuint index, GLdouble *v );
extern void glVertexAttrib4svNV( GLuint index, GLshort *v );
extern void glVertexAttrib4fvNV( GLuint index, GLfloat *v );
extern void glVertexAttrib4dvNV( GLuint index, GLdouble *v );
extern void glVertexAttrib4ubvNV( GLuint index, GLubyte *v );
extern void glVertexAttribs1svNV( GLuint index, GLsizei n, GLshort *v );
extern void glVertexAttribs1fvNV( GLuint index, GLsizei n, GLfloat *v );
extern void glVertexAttribs1dvNV( GLuint index, GLsizei n, GLdouble *v );
extern void glVertexAttribs2svNV( GLuint index, GLsizei n, GLshort *v );
extern void glVertexAttribs2fvNV( GLuint index, GLsizei n, GLfloat *v );
extern void glVertexAttribs2dvNV( GLuint index, GLsizei n, GLdouble *v );
extern void glVertexAttribs3svNV( GLuint index, GLsizei n, GLshort *v );
extern void glVertexAttribs3fvNV( GLuint index, GLsizei n, GLfloat *v );
extern void glVertexAttribs3dvNV( GLuint index, GLsizei n, GLdouble *v );
extern void glVertexAttribs4svNV( GLuint index, GLsizei n, GLshort *v );
extern void glVertexAttribs4fvNV( GLuint index, GLsizei n, GLfloat *v );
extern void glVertexAttribs4dvNV( GLuint index, GLsizei n, GLdouble *v );
extern void glVertexAttribs4ubvNV( GLuint index, GLsizei n, GLubyte *v );
#endif

#if GL_NV_point_sprite
extern void glPointParameteriNV( GLenum pname, GLint param );
extern void glPointParameterivNV( GLenum pname, const GLint *params );
#endif

#if GL_ATI_pn_triangles
extern void glPNTrianglesiATI( GLenum pname, GLint param );
extern void glPNTrianglesfATI( GLenum pname, GLfloat param );
#endif

#if GL_ATIX_pn_triangles
extern void glPNTrianglesiATIX( GLenum pname, GLint param );
extern void glPNTrianglesfATIX( GLenum pname, GLfloat param );
#endif

#if GL_ATI_blend_equation_separate
extern void glBlendEquationSeparateATI( GLenum equationRGB, GLenum equationAlpha );
#endif

#if GL_ARB_point_parameters
extern void glPointParameterfARB( GLenum pname, GLfloat param );
extern void glPointParameterfvARB( GLenum pname, const GLfloat *params );
#endif

#if GL_ARB_vertex_program
extern void glBindProgramARB( GLenum target, GLuint program );
extern void glDeleteProgramsARB( GLsizei n, const GLuint *programs );
extern void glGenProgramsARB( GLsizei n, GLuint *programs );
extern GLboolean glIsProgramARB( GLuint program );

extern void glVertexAttrib1sARB( GLuint index, GLshort x );
extern void glVertexAttrib1fARB( GLuint index, GLfloat x );
extern void glVertexAttrib1dARB( GLuint index, GLdouble x );
extern void glVertexAttrib2sARB( GLuint index, GLshort x, GLshort y );
extern void glVertexAttrib2fARB( GLuint index, GLfloat x, GLfloat y );
extern void glVertexAttrib2dARB( GLuint index, GLdouble x, GLdouble y );
extern void glVertexAttrib3sARB( GLuint index, GLshort x, GLshort y, GLshort z );
extern void glVertexAttrib3fARB( GLuint index, GLfloat x, GLfloat y, GLfloat z );
extern void glVertexAttrib3dARB( GLuint index, GLdouble x, GLdouble y, GLdouble z );
extern void glVertexAttrib4sARB( GLuint index, GLshort x, GLshort y, GLshort z, GLshort w );
extern void glVertexAttrib4fARB( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void glVertexAttrib4dARB( GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void glVertexAttrib4NubARB( GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w );
extern void glVertexAttrib1svARB( GLuint index, const GLshort *v );
extern void glVertexAttrib1fvARB( GLuint index, const GLfloat *v );
extern void glVertexAttrib1dvARB( GLuint index, const GLdouble *v );
extern void glVertexAttrib2svARB( GLuint index, const GLshort *v );
extern void glVertexAttrib2fvARB( GLuint index, const GLfloat *v );
extern void glVertexAttrib2dvARB( GLuint index, const GLdouble *v );
extern void glVertexAttrib3svARB( GLuint index, const GLshort *v );
extern void glVertexAttrib3fvARB( GLuint index, const GLfloat *v );
extern void glVertexAttrib3dvARB( GLuint index, const GLdouble *v );
extern void glVertexAttrib4bvARB( GLuint index, const GLbyte *v );
extern void glVertexAttrib4svARB( GLuint index, const GLshort *v );
extern void glVertexAttrib4ivARB( GLuint index, const GLint *v );
extern void glVertexAttrib4ubvARB( GLuint index, const GLubyte *v );
extern void glVertexAttrib4usvARB( GLuint index, const GLushort *v );
extern void glVertexAttrib4uivARB( GLuint index, const GLuint *v );
extern void glVertexAttrib4fvARB( GLuint index, const GLfloat *v );
extern void glVertexAttrib4dvARB( GLuint index, const GLdouble *v );
extern void glVertexAttrib4NbvARB( GLuint index, const GLbyte *v );
extern void glVertexAttrib4NsvARB( GLuint index, const GLshort *v );
extern void glVertexAttrib4NivARB( GLuint index, const GLint *v );
extern void glVertexAttrib4NubvARB( GLuint index, const GLubyte *v );
extern void glVertexAttrib4NusvARB( GLuint index, const GLushort *v );
extern void glVertexAttrib4NuivARB( GLuint index, const GLuint *v );

extern void glVertexAttribPointerARB( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer );

extern void glEnableVertexAttribArrayARB( GLuint index );
extern void glDisableVertexAttribArrayARB( GLuint index );

extern void glGetVertexAttribdvARB( GLuint index, GLenum pname, GLdouble *params );
extern void glGetVertexAttribfvARB( GLuint index, GLenum pname, GLfloat *params );
extern void glGetVertexAttribivARB( GLuint index, GLenum pname, GLint *params );
extern void glGetVertexAttribPointervARB( GLuint index, GLenum pname, GLvoid **pointer );

extern void glProgramEnvParameter4dARB( GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void glProgramEnvParameter4dvARB( GLenum target, GLuint index, const GLdouble *params );
extern void glProgramEnvParameter4fARB( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void glProgramEnvParameter4fvARB( GLenum target, GLuint index, const GLfloat *params );
extern void glProgramLocalParameter4dARB( GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void glProgramLocalParameter4dvARB( GLenum target, GLuint index, const GLdouble *params );
extern void glProgramLocalParameter4fARB( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void glProgramLocalParameter4fvARB( GLenum target, GLuint index, const GLfloat *params );

extern void glGetProgramEnvParameterdvARB( GLenum target, GLuint index, GLdouble *params );
extern void glGetProgramEnvParameterfvARB( GLenum target, GLuint index, GLfloat *params );
extern void glGetProgramLocalParameterdvARB( GLenum target, GLuint index, GLdouble *params );
extern void glGetProgramLocalParameterfvARB( GLenum target, GLuint index, GLfloat *params );

extern void glProgramStringARB( GLenum target, GLenum format, GLsizei len, const GLvoid *string );
extern void glGetProgramStringARB( GLenum target, GLenum pname, GLvoid *string );

extern void glGetProgramivARB( GLenum target, GLenum pname, GLint *params );
#endif

#if GL_APPLE_vertex_program_evaluators
extern void glEnableVertexAttribAPPLE( GLuint index, GLenum pname );
extern void glDisableVertexAttribAPPLE( GLuint index, GLenum pname );
extern GLboolean glIsVertexAttribEnabledAPPLE( GLuint index, GLenum pname );
extern void glMapVertexAttrib1dAPPLE( GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points );
extern void glMapVertexAttrib1fAPPLE( GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points );
extern void glMapVertexAttrib2dAPPLE( GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points );
extern void glMapVertexAttrib2fAPPLE( GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points );
#endif

#if GL_EXT_stencil_two_side
extern void glActiveStencilFaceEXT( GLenum face );
#endif

#ifdef __cplusplus
}
#endif

#endif

