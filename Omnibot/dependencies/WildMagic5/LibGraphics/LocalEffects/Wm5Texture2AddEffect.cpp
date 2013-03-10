// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Texture2AddEffect.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, Texture2AddEffect);
WM5_IMPLEMENT_STREAM(Texture2AddEffect);
WM5_IMPLEMENT_FACTORY(Texture2AddEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, Texture2AddEffect);

//----------------------------------------------------------------------------
Texture2AddEffect::Texture2AddEffect ()
{
    VertexShader* vshader = new0 VertexShader("Wm5.Texture2Add",
        3, 3, 1, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelTCoord0", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetInput(2, "modelTCoord1", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD1);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexTCoord0", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetOutput(2, "vertexTCoord1", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD1);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.Texture2Add",
        2, 1, 0, 2, false);
    pshader->SetInput(0, "vertexTCoord0", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetInput(1, "vertexTCoord1", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD1);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "Sampler0", Shader::ST_2D);
    pshader->SetSampler(1, "Sampler1", Shader::ST_2D);
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
Texture2AddEffect::~Texture2AddEffect ()
{
}
//----------------------------------------------------------------------------
PixelShader* Texture2AddEffect::GetPixelShader () const
{
    return mTechniques[0]->GetPass(0)->GetPixelShader();
}
//----------------------------------------------------------------------------
VisualEffectInstance* Texture2AddEffect::CreateInstance (Texture2D* texture0,
    Texture2D* texture1) const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetPixelTexture(0, 0, texture0);
    instance->SetPixelTexture(0, 1, texture1);

    PixelShader* pshader = GetPixelShader();

    Shader::SamplerFilter filter0 = pshader->GetFilter(0);
    if (filter0 != Shader::SF_NEAREST && filter0 != Shader::SF_LINEAR
    &&  !texture0->HasMipmaps())
    {
        texture0->GenerateMipmaps();
    }

    Shader::SamplerFilter filter1 = pshader->GetFilter(1);
    if (filter1 != Shader::SF_NEAREST && filter1 != Shader::SF_LINEAR
    &&  !texture1->HasMipmaps())
    {
        texture1->GenerateMipmaps();
    }

    return instance;
}
//----------------------------------------------------------------------------
VisualEffectInstance* Texture2AddEffect::CreateUniqueInstance (
    Texture2D* texture0, Shader::SamplerFilter filter0,
    Shader::SamplerCoordinate coordinate00,
    Shader::SamplerCoordinate coordinate01, Texture2D* texture1,
    Shader::SamplerFilter filter1, Shader::SamplerCoordinate coordinate10,
    Shader::SamplerCoordinate coordinate11)
{
    Texture2AddEffect* effect = new0 Texture2AddEffect();
    PixelShader* pshader = effect->GetPixelShader();
    pshader->SetFilter(0, filter0);
    pshader->SetCoordinate(0, 0, coordinate00);
    pshader->SetCoordinate(0, 1, coordinate01);
    pshader->SetFilter(1, filter1);
    pshader->SetCoordinate(1, 0, coordinate10);
    pshader->SetCoordinate(1, 1, coordinate11);
    return effect->CreateInstance(texture0, texture1);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Texture2AddEffect::Texture2AddEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void Texture2AddEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(Texture2AddEffect, source);
}
//----------------------------------------------------------------------------
void Texture2AddEffect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void Texture2AddEffect::PostLink ()
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
bool Texture2AddEffect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void Texture2AddEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(Texture2AddEffect, target);
}
//----------------------------------------------------------------------------
int Texture2AddEffect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int Texture2AddEffect::msDx9VRegisters[1] = { 0 };
int Texture2AddEffect::msOglVRegisters[1] = { 1 };
int* Texture2AddEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string Texture2AddEffect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "vs_1_1\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "dcl_texcoord1 v2\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oT0.xy, v1\n"
    "mov oT1.xy, v2\n",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position v0\n"
    "dcl_texcoord0 v1\n"
    "dcl_texcoord1 v2\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oT0.xy, v1\n"
    "mov oT1.xy, v2\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_texcoord0 o1\n"
    "dcl_texcoord1 o2\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "dcl_texcoord1 v2\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1.xy, v1\n"
    "mov o2.xy, v2\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "PARAM c[5] = { { 1 }, program.local[1..4] };\n"
    "TEMP R0;\n"
    "MOV R0.w, c[0].x;\n"
    "MOV R0.xyz, vertex.position;\n"
    "DP4 result.position.w, R0, c[4];\n"
    "DP4 result.position.z, R0, c[3];\n"
    "DP4 result.position.y, R0, c[2];\n"
    "DP4 result.position.x, R0, c[1];\n"
    "MOV result.texcoord[0].xy, vertex.texcoord[0];\n"
    "MOV result.texcoord[1].xy, vertex.texcoord[1];\n"
    "END\n"
};

int Texture2AddEffect::msAllPTextureUnits[2] = { 0, 1 };
int* Texture2AddEffect::msPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits
};

std::string Texture2AddEffect::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "ps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "add_sat r0, t0, t1\n",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl t0.xy\n"
    "dcl t1.xy\n"
    "texld r0, t1, s1\n"
    "texld r1, t0, s0\n"
    "add_sat r0, r1, r0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_texcoord0 v0.xy\n"
    "dcl_texcoord1 v1.xy\n"
    "texld r1, v1, s1\n"
    "texld r0, v0, s0\n"
    "add_sat oC0, r0, r1\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEX R1, fragment.texcoord[1], texture[1], 2D;\n"
    "TEX R0, fragment.texcoord[0], texture[0], 2D;\n"
    "ADD_SAT result.color, R0, R1;\n"
    "END\n"
};
//----------------------------------------------------------------------------
