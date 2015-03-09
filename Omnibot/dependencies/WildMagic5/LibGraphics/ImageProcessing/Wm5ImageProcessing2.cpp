// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ImageProcessing2.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

//----------------------------------------------------------------------------
ImageProcessing2::ImageProcessing2 (int bound0, int bound1, Float4* imageData,
    PixelShader* mainPShader, bool useDirichlet)
    :
    ImageProcessing(bound0, bound1, 2),
    mBound0(bound0),
    mBound1(bound1)
{
    mBound0M1 = mBound0 - 1;
    mBound1M1 = mBound1 - 1;
    mDx = 1.0f/(float)mBound0M1;
    mDy = 1.0f/(float)mBound1M1;

    VisualEffect* effect;
    VisualEffectInstance* instance;

    // Create the texture corresponding to the 2D image.
    mMainTexture = CreateImage(imageData);

    // Create the image processing effect.
    CreateEffect(mainPShader, effect, instance);
    mMainEffect = effect;
    mMainEffectInstance = instance;
    mMainEffectInstance->SetPixelTexture(0, "StateSampler",
        mTargets[1]->GetColorTexture(0));

    // Create the boundary effect.
    if (useDirichlet)
    {
        CreateBoundaryDirichletEffect(effect, instance);
    }
    else
    {
        CreateBoundaryNeumannEffect(effect, instance);
    }
    mBoundaryEffect = effect;
    mBoundaryEffectInstance = instance;
    mBoundaryEffectInstance->SetPixelTexture(0, "StateSampler",
        mTargets[0]->GetColorTexture(0));

    // Create the effect for drawing the results of the image processing.
    CreateDrawEffect(effect, instance);
    mDrawEffect = effect;
    mDrawEffectInstance = instance;
    mDrawEffectInstance->SetPixelTexture(0, "StateSampler",
        mTargets[1]->GetColorTexture(0));
}
//----------------------------------------------------------------------------
ImageProcessing2::ImageProcessing2 (int bound0, int bound1, int numTargets)
    :
    ImageProcessing(bound0, bound1, numTargets),
    mBound0(bound0),
    mBound1(bound1)
{
    mBound0M1 = mBound0 - 1;
    mBound1M1 = mBound1 - 1;
    mDx = 1.0f/(float)mBound0M1;
    mDy = 1.0f/(float)mBound1M1;
}
//----------------------------------------------------------------------------
ImageProcessing2::~ImageProcessing2 ()
{
}
//----------------------------------------------------------------------------
Texture2D* ImageProcessing2::CreateImage (Float4* imageData)
{
    Texture2D* reflected = new0 Texture2D(Texture::TF_A32B32G32R32F,
        mBound0, mBound1, 1);

    memcpy(reflected->GetData(0), imageData, mBound0*mBound1*sizeof(Float4));
    return reflected;
}
//----------------------------------------------------------------------------
void ImageProcessing2::CreateBoundaryDirichletEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    // sampler2D MaskSampler = sampler_state
    // {
    //    MinFilter = Nearest;
    //    MagFilter = Nearest;
    //    WrapS     = Clamp;
    //    WrapT     = Clamp;
    // };
    // sampler2D StateSampler = sampler_state
    // {
    //    MinFilter = Nearest;
    //    MagFilter = Nearest;
    //    WrapS     = Clamp;
    //    WrapT     = Clamp;
    // };
    // void p_BoundaryDirichlet
    // (
    //     in float2 vertexTCoord : TEXCOORD0,
    //     out float4 pixelColor : COLOR
    // )
    // {
    //     float4 mask = tex2D(MaskSampler, vertexTCoord);
    //     float4 state = tex2D(StateSampler, vertexTCoord);
    //     pixelColor = mask*state;
    // }

    PixelShader* pshader = new0 PixelShader("Wm5.BoundaryDirichlet2",
        1, 1, 0, 2, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "MaskSampler", Shader::ST_2D);
    pshader->SetSampler(1, "StateSampler", Shader::ST_2D);
    pshader->SetTextureUnits(msDirichletPTextureUnits);
    pshader->SetPrograms(msDirichletPPrograms);

    CreateEffect(pshader, effect, instance);

    // Create the mask texture.
    Texture2D* maskTexture = new0 Texture2D(Texture::TF_A32B32G32R32F,
        mBound0, mBound1, 1);

    Float4* mask = (Float4*)maskTexture->GetData(0);
    Float4 one(1.0f, 1.0f, 1.0f, 1.0f);
    Float4 zero(0.0f, 0.0f, 0.0f, 0.0f);
    int x, y;

    // Interior.
    for (y = 1; y < mBound1M1; ++y)
    {
        for (x = 1; x < mBound0M1; ++x)
        {
            mask[Index(x,y)] = one;
        }
    }

    // Edge-interior.
    for (x = 1; x < mBound0M1; ++x)
    {
        mask[Index(x,0)] = zero;
        mask[Index(x,mBound1M1)] = zero;
    }
    for (y = 1; y < mBound1M1; ++y)
    {
        mask[Index(0,y)] = zero;
        mask[Index(mBound0M1,y)] = zero;
    }

    // Corners.
    mask[Index(0,0)] = zero;
    mask[Index(mBound0M1,0)] = zero;
    mask[Index(0,mBound1M1)] = zero;
    mask[Index(mBound0M1,mBound1M1)] = zero;

    instance->SetPixelTexture(0, "MaskSampler", maskTexture);
}
//----------------------------------------------------------------------------
void ImageProcessing2::CreateBoundaryNeumannEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    // sampler2D OffsetSampler = sampler_state
    // {
    //    MinFilter = Nearest;
    //    MagFilter = Nearest;
    //    WrapS     = Clamp;
    //    WrapT     = Clamp;
    // };
    // sampler2D StateSampler = sampler_state
    // {
    //    MinFilter = Nearest;
    //    MagFilter = Nearest;
    //    WrapS     = Clamp;
    //    WrapT     = Clamp;
    // };
    // void p_BoundaryNeumann
    // (
    //     in float2 vertexTCoord : TEXCOORD0,
    //     out float4 pixelColor : COLOR
    // )
    // {
    //     float2 offset = tex2D(OffsetSampler, vertexTCoord).xy;
    //     float2 tc = float2(vertexTCoord.x + offset.x,
    //         vertexTCoord.y + offset.y);
    //     float4 state = tex2D(StateSampler, tc);
    //     pixelColor = state;
    // }

    PixelShader* pshader = new0 PixelShader("Wm5.BoundaryNeumann2",
        1, 1, 0, 2, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "OffsetSampler", Shader::ST_2D);
    pshader->SetSampler(1, "StateSampler", Shader::ST_2D);
    pshader->SetTextureUnits(msNeumannPTextureUnits);
    pshader->SetPrograms(msNeumannPPrograms);

    CreateEffect(pshader, effect, instance);

    // Create the offset texture.
    Texture2D* offsetTexture = new0 Texture2D(Texture::TF_A32B32G32R32F,
        mBound0, mBound1, 1);

    Float4* offset = (Float4*)offsetTexture->GetData(0);
    Float4 zero(0.0f, 0.0f, 0.0f, 0.0f);
    Float4 x0EdgeOffset(+mDx, 0.0f, 0.0f, 0.0f);
    Float4 x1EdgeOffset(-mDx, 0.0f, 0.0f, 0.0f);
    Float4 y0EdgeOffset(0.0f, +mDy, 0.0f, 0.0f);
    Float4 y1EdgeOffset(0.0f, -mDy, 0.0f, 0.0f);
    int x, y;

    // Interior.
    for (y = 1; y < mBound1M1; ++y)
    {
        for (x = 1; x < mBound0M1; ++x)
        {
            offset[Index(x,y)] = zero;
        }
    }

    // Edge-interior.
    for (x = 1; x < mBound0M1; ++x)
    {
        offset[Index(x,0)] = y0EdgeOffset;
        offset[Index(x,mBound1M1)] = y1EdgeOffset;
    }
    for (y = 1; y < mBound1M1; ++y)
    {
        offset[Index(0,y)] = x0EdgeOffset;
        offset[Index(mBound0M1,y)] = x1EdgeOffset;
    }

    // Corners.
    offset[Index(0,0)] = Float4(+mDx, +mDy, 0.0f, 0.0f);
    offset[Index(mBound0M1,0)] = Float4(-mDx, +mDy, 0.0f, 0.0f);
    offset[Index(0,mBound1M1)] = Float4(+mDx, -mDy, 0.0f, 0.0f);
    offset[Index(mBound0M1,mBound1M1)] = Float4(-mDx, -mDy, 0.0f, 0.0f);

    instance->SetPixelTexture(0, "OffsetSampler", offsetTexture);
}
//----------------------------------------------------------------------------
void ImageProcessing2::CreateDrawEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    // sampler2D StateSampler = sampler_state
    // {
    //    MinFilter = Nearest;
    //    MagFilter = Nearest;
    //    WrapS     = Clamp;
    //    WrapT     = Clamp;
    // };
    // void p_ScreenShader2
    // (
    //     in float2 vertexTCoord : TEXCOORD0,
    //     out float4 pixelColor : COLOR
    // )
    // {
    //     pixelColor = tex2D(StateSampler, vertexTCoord);
    // }

    PixelShader* pshader = new0 PixelShader("Wm5.DrawImage2",
        1, 1, 0, 1, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "StateSampler", Shader::ST_2D);
    pshader->SetTextureUnits(msDrawPTextureUnits);
    pshader->SetPrograms(msDrawPPrograms);

    CreateEffect(pshader, effect, instance);
}
//----------------------------------------------------------------------------
int ImageProcessing2::msAllDirichletPTextureUnits[2] = { 0, 1 };
int* ImageProcessing2::msDirichletPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllDirichletPTextureUnits,
    msAllDirichletPTextureUnits,
    msAllDirichletPTextureUnits,
    msAllDirichletPTextureUnits
};

std::string ImageProcessing2::msDirichletPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl t0.xy\n"
    "texld r0, t0, s1\n"
    "texld r1, t0, s0\n"
    "mul r0, r1, r0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_texcoord0 v0.xy\n"
    "texld r1, v0, s1\n"
    "texld r0, v0, s0\n"
    "mul oC0, r0, r1\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEX R1, fragment.texcoord[0], texture[1], 2D;\n"
    "TEX R0, fragment.texcoord[0], texture[0], 2D;\n"
    "MUL result.color, R0, R1;\n"
    "END\n"
};
//----------------------------------------------------------------------------
int ImageProcessing2::msAllNeumannPTextureUnits[2] = { 0, 1 };
int* ImageProcessing2::msNeumannPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllNeumannPTextureUnits,
    msAllNeumannPTextureUnits,
    msAllNeumannPTextureUnits,
    msAllNeumannPTextureUnits
};

std::string ImageProcessing2::msNeumannPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl t0.xy\n"
    "texld r0, t0, s0\n"
    "add r0.xy, t0, r0\n"
    "texld r0, r0, s1\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_texcoord0 v0.xy\n"
    "texld r0.xy, v0, s0\n"
    "add r0.xy, v0, r0\n"
    "texld r0, r0, s1\n"
    "mov oC0, r0\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEMP R0;\n"
    "TEX R0.xy, fragment.texcoord[0], texture[0], 2D;\n"
    "ADD R0.xy, fragment.texcoord[0], R0;\n"
    "TEX result.color, R0, texture[1], 2D;\n"
    "END\n"
};
//----------------------------------------------------------------------------
int ImageProcessing2::msAllDrawPTextureUnits[2] = { 0, 1 };
int* ImageProcessing2::msDrawPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllDrawPTextureUnits,
    msAllDrawPTextureUnits,
    msAllDrawPTextureUnits,
    msAllDrawPTextureUnits
};

std::string ImageProcessing2::msDrawPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl t0.xy\n"
    "texld r0, t0, s0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_texcoord0 v0.xy\n"
    "texld r0, v0, s0\n"
    "mov oC0, r0\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEX result.color, fragment.texcoord[0], texture[0], 2D;\n"
    "END\n"
};
//----------------------------------------------------------------------------
