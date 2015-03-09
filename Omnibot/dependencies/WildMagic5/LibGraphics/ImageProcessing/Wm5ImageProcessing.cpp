// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ImageProcessing.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

//----------------------------------------------------------------------------
ImageProcessing::ImageProcessing (int numCols, int numRows, int numTargets)
    :
    mNumCols(numCols),
    mNumRows(numRows),
    mNumTargets(numTargets)
{
    assertion(mNumCols > 1 && mNumRows > 0, "Invalid bound.\n");
    assertion(mNumTargets > 0, "Invalid number of targets.\n");

    mColSpacing = 1.0f/(float)(mNumCols - 1);
    mRowSpacing = 1.0f/(float)(mNumRows - 1);

    // The screen camera maps (x,y,z) in [0,1]^3 to (x',y,'z') in
    // [-1,1]^2 x [0,1].
    mCamera = new0 Camera(false);
    mCamera->SetFrustum(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    mCamera->SetFrame(APoint::ORIGIN, AVector::UNIT_Z, AVector::UNIT_Y,
        AVector::UNIT_X);

    // Create the vertex format for the square.
    VertexFormat* vformat = new0 VertexFormat(2);
    vformat->SetAttribute(0, 0, 0, VertexFormat::AT_FLOAT3,
        VertexFormat::AU_POSITION, 0);
    vformat->SetAttribute(1, 0, 3*sizeof(float), VertexFormat::AT_FLOAT2,
        VertexFormat::AU_TEXCOORD, 0);
    vformat->SetStride(5*sizeof(float));

    // Create the vertex buffer for the squares.
    int vstride = vformat->GetStride();
    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    float xmin, xmax, ymin, ymax;
    Float2 tc0, tc1, tc2, tc3;
    if (VertexShader::GetProfile() == VertexShader::VP_ARBVP1)
    {
        xmin = 0.0f;
        xmax = 1.0f;
        ymin = 0.0f;
        ymax = 1.0f;
        tc0 = Float2(0.0f, 0.0f);
        tc1 = Float2(1.0f, 0.0f);
        tc2 = Float2(1.0f, 1.0f);
        tc3 = Float2(0.0f, 1.0f);
    }
    else
    {
        xmin = -0.5f*mColSpacing;
        xmax = 1.0f - 0.5f*mColSpacing;
        ymin = +0.5f*mRowSpacing;
        ymax = 1.0f + 0.5f*mRowSpacing;
        tc0 = Float2(0.0f, 1.0f);
        tc1 = Float2(1.0f, 1.0f);
        tc2 = Float2(1.0f, 0.0f);
        tc3 = Float2(0.0f, 0.0f);
    }

    vba.Position<Float3>(0) = Float3(xmin, ymin, 0.0f);
    vba.Position<Float3>(1) = Float3(xmax, ymin, 0.0f);
    vba.Position<Float3>(2) = Float3(xmax, ymax, 0.0f);
    vba.Position<Float3>(3) = Float3(xmin, ymax, 0.0f);
    vba.TCoord<Float2>(0, 0) = tc0;
    vba.TCoord<Float2>(0, 1) = tc1;
    vba.TCoord<Float2>(0, 2) = tc2;
    vba.TCoord<Float2>(0, 3) = tc3;

    // Create the index buffer for the square.
    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    // Create the square.
    mRectangle = new0 TriMesh(vformat, vbuffer, ibuffer);

    CreateVertexShader();
    mTargets = new1<RenderTargetPtr>(mNumTargets);
    for (int i = 0; i < mNumTargets; ++i)
    {
        mTargets[i] = new0 RenderTarget(1, Texture::TF_A32B32G32R32F,
            mNumCols, mNumRows, false, false);
    }
}
//----------------------------------------------------------------------------
ImageProcessing::~ImageProcessing ()
{
    delete1(mTargets);
}
//----------------------------------------------------------------------------
void ImageProcessing::CreateEffect (PixelShader* pshader,
    VisualEffect*& effect, VisualEffectInstance*& instance)
{
    // Create the pass.
    VisualPass* pass = new0 VisualPass();
    pass->SetPixelShader(pshader);

    // All effects share the vertex shader for the square trimesh.
    pass->SetVertexShader(mVertexShader);

    // Create global state.
    pass->SetAlphaState(new0 AlphaState());
    pass->SetOffsetState(new0 OffsetState());
    pass->SetStencilState(new0 StencilState());
    pass->SetWireState(new0 WireState());

    // Culling is not needed for image processing.
    CullState* cstate = new0 CullState();
    cstate->Enabled = false;
    pass->SetCullState(cstate);

    // Depth buffering is not needed for image processing.
    DepthState* dstate = new0 DepthState();
    dstate->Enabled = false;
    dstate->Writable = false;
    pass->SetDepthState(dstate);

    // Create the effect.
    VisualTechnique* technique = new0 VisualTechnique();
    technique->InsertPass(pass);
    effect = new0 VisualEffect();
    effect->InsertTechnique(technique);

    // Create the effect instance and set the vertex shader constants.
    instance = new0 VisualEffectInstance(effect, 0);

    // Standard transform for vertex shader.
    instance->SetVertexConstant(0, "PVWMatrix", mPVWMatrixConstant);
}
//----------------------------------------------------------------------------
void ImageProcessing::PreDraw ()
{
    if (VertexShader::GetProfile() == VertexShader::VP_ARBVP1)
    {
        // Reflect the image in y to account for OpenGL textures having
        // origin in the upper-left corner.
        mCamera->SetPostProjectionMatrix(HMatrix(1.0f, -1.0f, 1.0f));
    }
}
//----------------------------------------------------------------------------
void ImageProcessing::PostDraw ()
{
    if (VertexShader::GetProfile() == VertexShader::VP_ARBVP1)
    {
        // Remove the y-reflection.
        mCamera->SetPostProjectionMatrix(HMatrix::IDENTITY);
    }
}
//----------------------------------------------------------------------------
void ImageProcessing::Initialize (Renderer* renderer, bool openglHack)
{
    // Initialize the image processing by copying the image to a render
    // target.
    renderer->Bind(mTargets[0]);
    renderer->Bind(mTargets[1]);
    renderer->Bind(mMainTexture);
    if (renderer->PreDraw())
    {
        renderer->SetCamera(mCamera);

        // Copy the image to a render target.
        if (openglHack)
        {
            // On Windows Vista with dual NVIDIA GeForce 9800 GT cards running
            // drivers 191.07 and the GpuGaussianBlur3 sample application.
            // If I use mDrawInput and read render target 0 after the
            // Disable(mTargets[0]) below, the target has only black and green
            // pixels.  The green pixels are what p_ScreenShader3 draw for
            // boundary pixels.  If I call ClearColorBuffer with a clear color
            // of blue and without the draw call, the target has all blue
            // pixels, as expected.  If I clear the target and use the draw
            // call, the pixels are all black and green; moreover, the
            // pixels-drawn query in Renderer::DrawPrimitive reports that all
            // pixels in the square have been drawn, as expected.  If I modify
            // p_ScreenShader to test the mMainTexture samples (StateSampler)
            // and return blue whenever the samples are black, the render
            // target has blue and green pixels.  I have checked the
            // mMainTexture system memory and video memory (and mask memory
            // in the p_ScreenShader's other texture)--all appear to be
            // correct.  Everything in the drawing pipeline appear to be set
            // up correctly, so this has the feel of a driver bug.  (The
            // GpuGaussianBlur2 sample application works fine without this
            // hack.)  Switching to using mMainInput instead, everything draws
            // correctly.
            mMainEffectInstance->SetPixelTexture(0, "StateSampler",
                mMainTexture);
            mRectangle->SetEffectInstance(mMainEffectInstance);
        }
        else
        {
            mDrawEffectInstance->SetPixelTexture(0, "StateSampler",
                mMainTexture);
            mRectangle->SetEffectInstance(mDrawEffectInstance);
        }

        renderer->Enable(mTargets[0]);
        renderer->ClearColorBuffer();
        renderer->Draw(mRectangle);
        renderer->Disable(mTargets[0]);
        if (openglHack)
        {
            // See the coments above for OpenGL.
            mMainEffectInstance->SetPixelTexture(0, "StateSampler",
                mTargets[1]->GetColorTexture(0));
        }
        else
        {
            mDrawEffectInstance->SetPixelTexture(0, "StateSampler",
                mTargets[1]->GetColorTexture(0));
        }

        // Set the boundary conditions.
        mRectangle->SetEffectInstance(mBoundaryEffectInstance);
        renderer->Enable(mTargets[1]);
        renderer->Draw(mRectangle);
        renderer->Disable(mTargets[1]);

        renderer->PostDraw();
    }
}
//----------------------------------------------------------------------------
void ImageProcessing::ExecuteStep (Renderer* renderer, bool draw)
{
    // Draw the image.
    if (draw)
    {
        PreDraw();
        mRectangle->SetEffectInstance(mDrawEffectInstance);
        renderer->Draw(mRectangle);
        PostDraw();
    }

    // Take a step using the main effect.
    mRectangle->SetEffectInstance(mMainEffectInstance);
    renderer->Enable(mTargets[0]);
    renderer->Draw(mRectangle);
    renderer->Disable(mTargets[0]);

    // Set the boundary conditions.
    mRectangle->SetEffectInstance(mBoundaryEffectInstance);
    renderer->Enable(mTargets[1]);
    renderer->Draw(mRectangle);
    renderer->Disable(mTargets[1]);
}
//----------------------------------------------------------------------------
void ImageProcessing::CreateVertexShader ()
{
    // void v_ScreenShader
    // (
    //     in float3 modelPosition : POSITION,
    //     in float2 modelTCoord : TEXCOORD0,
    //     out float4 clipPosition : POSITION,
    //     out float2 vertexTCoord : TEXCOORD0,
    //     uniform float4x4 PVWMatrix
    // )
    // {
    //     // Transform the position from model space to clip space.
    //     clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));
    //
    //     // Pass through the texture coordinates.
    //     vertexTCoord = modelTCoord;
    // }

    mVertexShader = new0 VertexShader("Wm5.ScreenShader",
        1, 2, 1, 0, false);
    mVertexShader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    mVertexShader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    mVertexShader->SetOutput(1, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    mVertexShader->SetConstant(0, "PVWMatrix", 4);
    mVertexShader->SetBaseRegisters(msVRegisters);
    mVertexShader->SetPrograms(msVPrograms);

    mPVWMatrixConstant = new0 PVWMatrixConstant();
}
//----------------------------------------------------------------------------
int ImageProcessing::msDx9VRegisters[1] = { 0 };
int ImageProcessing::msOglVRegisters[1] = { 1 };
int* ImageProcessing::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string ImageProcessing::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oT0.xy, v1\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_texcoord0 o1\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1.xy, v1\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "PARAM c[5] = { { 1 },\n"
    "		program.local[1..4] };\n"
    "TEMP R0;\n"
    "MOV R0.w, c[0].x;\n"
    "MOV R0.xyz, vertex.position;\n"
    "DP4 result.position.w, R0, c[4];\n"
    "DP4 result.position.z, R0, c[3];\n"
    "DP4 result.position.y, R0, c[2];\n"
    "DP4 result.position.x, R0, c[1];\n"
    "MOV result.texcoord[0].xy, vertex.texcoord[0];\n"
    "END\n"
};
//----------------------------------------------------------------------------
