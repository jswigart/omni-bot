// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5MaterialEffect.h"
#include "Wm5MaterialDiffuseConstant.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, MaterialEffect);
WM5_IMPLEMENT_STREAM(MaterialEffect);
WM5_IMPLEMENT_FACTORY(MaterialEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, MaterialEffect);

//----------------------------------------------------------------------------
MaterialEffect::MaterialEffect ()
{
    VertexShader* vshader = new0 VertexShader("Wm5.Material",
        1, 2, 2, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetConstant(1, "MaterialDiffuse", 1);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.Material",
        1, 1, 0, 0, false);
    pshader->SetInput(0, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
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
MaterialEffect::~MaterialEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* MaterialEffect::CreateInstance (Material* material)
    const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, "PVWMatrix", new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, "MaterialDiffuse",
        new0 MaterialDiffuseConstant(material));
    return instance;
}
//----------------------------------------------------------------------------
VisualEffectInstance* MaterialEffect::CreateUniqueInstance (
    Material* material)
{
    MaterialEffect* effect = new0 MaterialEffect();
    return effect->CreateInstance(material);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
MaterialEffect::MaterialEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void MaterialEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(MaterialEffect, source);
}
//----------------------------------------------------------------------------
void MaterialEffect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void MaterialEffect::PostLink ()
{
    VisualEffect::PostLink();

    VisualPass* pass = mTechniques[0]->GetPass(0);
    VertexShader* vshader = pass->GetVertexShader();
    PixelShader* pshader = pass->GetPixelShader();
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);
    pshader->SetPrograms(msPPrograms);
}
//----------------------------------------------------------------------------
bool MaterialEffect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void MaterialEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(MaterialEffect, target);
}
//----------------------------------------------------------------------------
int MaterialEffect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int MaterialEffect::msDx9VRegisters[2] = { 0, 4 };
int MaterialEffect::msOglVRegisters[2] = { 1, 5 };
int* MaterialEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string MaterialEffect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "vs_1_1\n"
    "def c5, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "mov r0.w, c5.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0, c4\n",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c5, 1.00000000, 0, 0, 0\n"
    "dcl_position v0\n"
    "mov r0.w, c5.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0, c4\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_color0 o1\n"
    "def c5, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "mov r0.w, c5.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1, c4\n",

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
    "END\n"
};

std::string MaterialEffect::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "ps.1.1\n"
    "mov r0, v0\n",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl v0\n"
    "mov oC0, v0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_color0 v0\n"
    "mov oC0, v0\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "MOV result.color, fragment.color.primary;\n"
    "END\n"
};
//----------------------------------------------------------------------------
