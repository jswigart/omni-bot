// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5MaterialTextureEffect.h"
#include "Wm5MaterialDiffuseConstant.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, MaterialTextureEffect);
WM5_IMPLEMENT_STREAM(MaterialTextureEffect);
WM5_IMPLEMENT_FACTORY(MaterialTextureEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, MaterialTextureEffect);

//----------------------------------------------------------------------------
MaterialTextureEffect::MaterialTextureEffect (Shader::SamplerFilter filter,
    Shader::SamplerCoordinate coordinate0,
    Shader::SamplerCoordinate coordinate1)
{
    VertexShader* vshader = new0 VertexShader("Wm5.MaterialTexture",
        2, 3, 2, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    vshader->SetOutput(2, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetConstant(1, "MaterialDiffuse", 1);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.MaterialTexture",
        2, 1, 0, 1, false);
    pshader->SetInput(0, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetInput(1, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "BaseSampler", Shader::ST_2D);
    pshader->SetFilter(0, filter);
    pshader->SetCoordinate(0, 0, coordinate0);
    pshader->SetCoordinate(0, 1, coordinate1);
    pshader->SetTextureUnits(msPTextureUnits);
    pshader->SetPrograms(msPPrograms);

    VisualPass* pass = new0 VisualPass();
    pass->SetVertexShader(vshader);
    pass->SetPixelShader(pshader);
    pass->SetAlphaState(new0 AlphaState());
    pass->SetCullState(new0 CullState());
    pass->SetDepthState(new0 DepthState());
    pass->SetOffsetState(new0 OffsetState());
    pass->SetStencilState(new0 StencilState());
    pass->SetWireState(new0 WireState());

    VisualTechnique* technique = new0 VisualTechnique();
    technique->InsertPass(pass);
    InsertTechnique(technique);
}
//----------------------------------------------------------------------------
MaterialTextureEffect::~MaterialTextureEffect ()
{
}
//----------------------------------------------------------------------------
PixelShader* MaterialTextureEffect::GetPixelShader () const
{
    return mTechniques[0]->GetPass(0)->GetPixelShader();
}
//----------------------------------------------------------------------------
VisualEffectInstance* MaterialTextureEffect::CreateInstance (
    Material* material, Texture2D* texture) const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1,
        new0 MaterialDiffuseConstant(material));
    instance->SetPixelTexture(0, 0, texture);

    Shader::SamplerFilter filter = GetPixelShader()->GetFilter(0);
    if (filter != Shader::SF_NEAREST && filter != Shader::SF_LINEAR
    &&  !texture->HasMipmaps())
    {
        texture->GenerateMipmaps();
    }

    return instance;
}
//----------------------------------------------------------------------------
VisualEffectInstance* MaterialTextureEffect::CreateUniqueInstance (
    Material* material, Texture2D* texture, Shader::SamplerFilter filter,
    Shader::SamplerCoordinate coordinate0,
    Shader::SamplerCoordinate coordinate1)
{
    MaterialTextureEffect* effect = new0 MaterialTextureEffect();
    PixelShader* pshader = effect->GetPixelShader();
    pshader->SetFilter(0, filter);
    pshader->SetCoordinate(0, 0, coordinate0);
    pshader->SetCoordinate(0, 1, coordinate1);
    return effect->CreateInstance(material, texture);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
MaterialTextureEffect::MaterialTextureEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void MaterialTextureEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(MaterialTextureEffect, source);
}
//----------------------------------------------------------------------------
void MaterialTextureEffect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void MaterialTextureEffect::PostLink ()
{
    VisualEffect::PostLink();

    VisualPass* pass = mTechniques[0]->GetPass(0);
    VertexShader* vshader = pass->GetVertexShader();
    PixelShader* pshader = pass->GetPixelShader();
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);
    pshader->SetTextureUnits(msPTextureUnits);
    pshader->SetPrograms(msPPrograms);
}
//----------------------------------------------------------------------------
bool MaterialTextureEffect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void MaterialTextureEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(MaterialTextureEffect, target);
}
//----------------------------------------------------------------------------
int MaterialTextureEffect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int MaterialTextureEffect::msDx9VRegisters[2] = { 0, 4 };
int MaterialTextureEffect::msOglVRegisters[2] = { 1, 5 };
int* MaterialTextureEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string MaterialTextureEffect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "vs_1_1\n"
    "def c5, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "mov r0.w, c5.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0, c4\n"
    "mov oT0.xy, v1\n",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c5, 1.00000000, 0, 0, 0\n"
    "dcl_position v0\n"
    "dcl_texcoord0 v1\n"
    "mov r0.w, c5.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0, c4\n"
    "mov oT0.xy, v1\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_color0 o1\n"
    "dcl_texcoord0 o2\n"
    "def c5, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "mov r0.w, c5.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1, c4\n"
    "mov o2.xy, v1\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "PARAM c[6] = { { 1 }, program.local[1..5] };\n"
    "TEMP R0;\n"
    "MOV R0.w, c[0].x;\n"
    "MOV R0.xyz, vertex.position;\n"
    "DP4 result.position.w, R0, c[4];\n"
    "DP4 result.position.z, R0, c[3];\n"
    "DP4 result.position.y, R0, c[2];\n"
    "DP4 result.position.x, R0, c[1];\n"
    "MOV result.color, c[5];\n"
    "MOV result.texcoord[0].xy, vertex.texcoord[0];\n"
    "END\n"
};

int MaterialTextureEffect::msAllPTextureUnits[1] = { 0 };
int* MaterialTextureEffect::msPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits
};

std::string MaterialTextureEffect::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "ps.1.1\n"
    "tex t0\n"
    "add_sat r0.rgb, t0, v0\n"
    "+ mul r0.a, t0.a, v0.a\n",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl t0.xy\n"
    "dcl v0\n"
    "texld r0, t0, s0\n"
    "add_sat r0.xyz, r0, v0\n"
    "mul r0.w, r0, v0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_texcoord0 v1.xy\n"
    "dcl_color0 v0\n"
    "texld r0, v1, s0\n"
    "mul oC0.w, r0, v0\n"
    "add_sat oC0.xyz, r0, v0\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEMP R0;\n"
    "TEX R0, fragment.texcoord[0], texture[0], 2D;\n"
    "MUL result.color.w, R0, fragment.color.primary;\n"
    "ADD_SAT result.color.xyz, R0, fragment.color.primary;\n"
    "END\n"
};
//----------------------------------------------------------------------------
