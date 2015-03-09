// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VertexColor4Effect.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, VertexColor4Effect);
WM5_IMPLEMENT_STREAM(VertexColor4Effect);
WM5_IMPLEMENT_FACTORY(VertexColor4Effect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, VertexColor4Effect);

//----------------------------------------------------------------------------
VertexColor4Effect::VertexColor4Effect ()
{
    VertexShader* vshader = new0 VertexShader("Wm5.VertexColor4",
        2, 2, 1, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelColor", Shader::VT_FLOAT3,
        Shader::VS_COLOR0);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexColor", Shader::VT_FLOAT3,
        Shader::VS_COLOR0);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.VertexColor4",
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
VertexColor4Effect::~VertexColor4Effect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* VertexColor4Effect::CreateInstance () const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    return instance;
}
//----------------------------------------------------------------------------
VisualEffectInstance* VertexColor4Effect::CreateUniqueInstance ()
{
    VertexColor4Effect* effect = new0 VertexColor4Effect();
    return effect->CreateInstance();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
VertexColor4Effect::VertexColor4Effect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void VertexColor4Effect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(VertexColor4Effect, source);
}
//----------------------------------------------------------------------------
void VertexColor4Effect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void VertexColor4Effect::PostLink ()
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
bool VertexColor4Effect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void VertexColor4Effect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(VertexColor4Effect, target);
}
//----------------------------------------------------------------------------
int VertexColor4Effect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int VertexColor4Effect::msDx9VRegisters[1] = { 0 };
int VertexColor4Effect::msOglVRegisters[1] = { 1 };
int* VertexColor4Effect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string VertexColor4Effect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "vs_1_1\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_color0 v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0, v1\n",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position v0\n"
    "dcl_color v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0, v1\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_color0 o1\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_color0 v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1, v1\n",

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
    "MOV result.color, vertex.color;\n"
    "END\n"
};

std::string VertexColor4Effect::msPPrograms[Shader::MAX_PROFILES] =
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
