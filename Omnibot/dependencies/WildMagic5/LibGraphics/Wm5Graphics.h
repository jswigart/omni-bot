// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/01)

#ifndef WM5GRAPHICS_H
#define WM5GRAPHICS_H

// Controllers
#include "Wm5BlendTransformController.h"
#include "Wm5ControlledObject.h"
#include "Wm5Controller.h"
#include "Wm5IKController.h"
#include "Wm5IKGoal.h"
#include "Wm5IKJoint.h"
#include "Wm5KeyframeController.h"
#include "Wm5MorphController.h"
#include "Wm5ParticleController.h"
#include "Wm5PointController.h"
#include "Wm5SkinController.h"
#include "Wm5TransformController.h"

// CurvesSurfaces
#include "Wm5BoxSurface.h"
#include "Wm5BSplineSurfacePatch.h"
#include "Wm5CurveMesh.h"
#include "Wm5CurveSegment.h"
#include "Wm5FloatArray.h"
#include "Wm5Float2Array.h"
#include "Wm5RectangleSurface.h"
#include "Wm5RevolutionSurface.h"
#include "Wm5SurfacePatch.h"
#include "Wm5SurfaceMesh.h"
#include "Wm5TubeSurface.h"

// DataTypes
#include "Wm5Bound.h"
#include "Wm5Color.h"
#include "Wm5HalfFloat.h"
#include "Wm5SpecializedIO.h"
#include "Wm5Transform.h"
#include "Wm5Utility.h"

// Detail
#include "Wm5BillboardNode.h"
#include "Wm5ClodMesh.h"
#include "Wm5CollapseRecord.h"
#include "Wm5CollapseRecordArray.h"
#include "Wm5CreateClodMesh.h"
#include "Wm5DlodNode.h"
#include "Wm5SwitchNode.h"

// GlobalEffects
#include "Wm5GlobalEffect.h"
#include "Wm5PlanarReflectionEffect.h"
#include "Wm5PlanarShadowEffect.h"

// Image Processing
#include "Wm5ImageProcessing.h"
#include "Wm5ImageProcessing2.h"
#include "Wm5ImageProcessing3.h"

// LocalEffects
#include "Wm5DefaultEffect.h"
#include "Wm5LightAmbEffect.h"
#include "Wm5LightDirPerPixEffect.h"
#include "Wm5LightDirPerVerEffect.h"
#include "Wm5LightPntPerPixEffect.h"
#include "Wm5LightPntPerVerEffect.h"
#include "Wm5LightSptPerPixEffect.h"
#include "Wm5LightSptPerVerEffect.h"
#include "Wm5MaterialEffect.h"
#include "Wm5MaterialTextureEffect.h"
#include "Wm5Texture1DEffect.h"
#include "Wm5Texture2AddEffect.h"
#include "Wm5Texture2ColorBlendEffect.h"
#include "Wm5Texture2DEffect.h"
#include "Wm5Texture2MulEffect.h"
#include "Wm5Texture3DEffect.h"
#include "Wm5VertexColor3Effect.h"
#include "Wm5VertexColor4Effect.h"
#include "Wm5VertexColor4TextureEffect.h"

// Renderers
#include "Wm5Renderer.h"

#ifdef WM5_USE_DX9
#include "Wm5Dx9IndexBuffer.h"
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9PixelShader.h"
#include "Wm5Dx9RendererData.h"
#include "Wm5Dx9RendererInput.h"
#include "Wm5Dx9RenderTarget.h"
#include "Wm5Dx9Shader.h"
#include "Wm5Dx9Texture1D.h"
#include "Wm5Dx9Texture3D.h"
#include "Wm5Dx9Texture3D.h"
#include "Wm5Dx9TextureCube.h"
#include "Wm5Dx9VertexBuffer.h"
#include "Wm5Dx9VertexFormat.h"
#include "Wm5Dx9VertexShader.h"
#endif

#ifdef WM5_USE_OPENGL
#include "Wm5OpenGLBitmapFont.h"
#include "Wm5OpenGLIndexBuffer.h"
#include "Wm5OpenGLMapping.h"
#include "Wm5OpenGLPixelShader.h"
#include "Wm5OpenGLRendererData.h"
#include "Wm5OpenGLRenderTarget.h"
#include "Wm5OpenGLShader.h"
#include "Wm5OpenGLTexture1D.h"
#include "Wm5OpenGLTexture2D.h"
#include "Wm5OpenGLTexture3D.h"
#include "Wm5OpenGLTextureCube.h"
#include "Wm5OpenGLVertexBuffer.h"
#include "Wm5OpenGLVertexFormat.h"
#include "Wm5OpenGLVertexShader.h"
#endif

// Resources
#include "Wm5Buffer.h"
#include "Wm5IndexBuffer.h"
#include "Wm5RenderTarget.h"
#include "Wm5Texture.h"
#include "Wm5Texture1D.h"
#include "Wm5Texture2D.h"
#include "Wm5Texture3D.h"
#include "Wm5TextureCube.h"
#include "Wm5VertexBuffer.h"
#include "Wm5VertexBufferAccessor.h"
#include "Wm5VertexFormat.h"

// SceneGraph
#include "Wm5Camera.h"
#include "Wm5CameraNode.h"
#include "Wm5Culler.h"
#include "Wm5Light.h"
#include "Wm5LightNode.h"
#include "Wm5Material.h"
#include "Wm5Node.h"
#include "Wm5Particles.h"
#include "Wm5Picker.h"
#include "Wm5PickRecord.h"
#include "Wm5Polypoint.h"
#include "Wm5Polysegment.h"
#include "Wm5Projector.h"
#include "Wm5ScreenTarget.h"
#include "Wm5Spatial.h"
#include "Wm5StandardMesh.h"
#include "Wm5Triangles.h"
#include "Wm5TriFan.h"
#include "Wm5TriMesh.h"
#include "Wm5TriStrip.h"
#include "Wm5VisibleSet.h"
#include "Wm5Visual.h"

// Shaders
#include "Wm5AlphaState.h"
#include "Wm5CullState.h"
#include "Wm5DepthState.h"
#include "Wm5OffsetState.h"
#include "Wm5PixelShader.h"
#include "Wm5Shader.h"
#include "Wm5ShaderParameters.h"
#include "Wm5StencilState.h"
#include "Wm5VertexShader.h"
#include "Wm5VisualEffect.h"
#include "Wm5VisualEffectInstance.h"
#include "Wm5VisualPass.h"
#include "Wm5VisualTechnique.h"
#include "Wm5WireState.h"

// ShaderFloats
#include "Wm5CameraModelDVectorConstant.h"
#include "Wm5CameraModelPositionConstant.h"
#include "Wm5CameraWorldDVectorConstant.h"
#include "Wm5CameraWorldPositionConstant.h"
#include "Wm5LightAmbientConstant.h"
#include "Wm5LightAttenuationConstant.h"
#include "Wm5LightDiffuseConstant.h"
#include "Wm5LightModelDVectorConstant.h"
#include "Wm5LightModelPositionConstant.h"
#include "Wm5LightSpecularConstant.h"
#include "Wm5LightSpotConstant.h"
#include "Wm5LightWorldDVectorConstant.h"
#include "Wm5LightWorldPositionConstant.h"
#include "Wm5MaterialAmbientConstant.h"
#include "Wm5MaterialDiffuseConstant.h"
#include "Wm5MaterialEmissiveConstant.h"
#include "Wm5MaterialSpecularConstant.h"
#include "Wm5PMatrixConstant.h"
#include "Wm5PVMatrixConstant.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5ProjectorMatrixConstant.h"
#include "Wm5ProjectorWorldPositionConstant.h"
#include "Wm5ShaderFloat.h"
#include "Wm5VMatrixConstant.h"
#include "Wm5VWMatrixConstant.h"
#include "Wm5WMatrixConstant.h"

// Sorting
#include "Wm5BspNode.h"
#include "Wm5ConvexRegion.h"
#include "Wm5ConvexRegionManager.h"
#include "Wm5CRMCuller.h"
#include "Wm5Portal.h"

// Terrain
#include "Wm5TerrainPage.h"
#include "Wm5Terrain.h"

#endif
