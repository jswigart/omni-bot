// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightDirPerVerEffect.h"
#include "Wm5CameraModelPositionConstant.h"
#include "Wm5MaterialEmissiveConstant.h"
#include "Wm5MaterialAmbientConstant.h"
#include "Wm5MaterialDiffuseConstant.h"
#include "Wm5MaterialSpecularConstant.h"
#include "Wm5LightAmbientConstant.h"
#include "Wm5LightDiffuseConstant.h"
#include "Wm5LightSpecularConstant.h"
#include "Wm5LightAttenuationConstant.h"
#include "Wm5LightModelDVectorConstant.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, LightDirPerVerEffect);
WM5_IMPLEMENT_STREAM(LightDirPerVerEffect);
WM5_IMPLEMENT_FACTORY(LightDirPerVerEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, LightDirPerVerEffect);

//----------------------------------------------------------------------------
LightDirPerVerEffect::LightDirPerVerEffect ()
{
    VertexShader* vshader = new0 VertexShader("Wm5.LightDirPerVer",
        2, 2, 11, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelNormal", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetConstant(1, "CameraModelPosition", 1);
    vshader->SetConstant(2, "MaterialEmissive", 1);
    vshader->SetConstant(3, "MaterialAmbient", 1);
    vshader->SetConstant(4, "MaterialDiffuse", 1);
    vshader->SetConstant(5, "MaterialSpecular", 1);
    vshader->SetConstant(6, "LightModelDirection", 1);
    vshader->SetConstant(7, "LightAmbient", 1);
    vshader->SetConstant(8, "LightDiffuse", 1);
    vshader->SetConstant(9, "LightSpecular", 1);
    vshader->SetConstant(10, "LightAttenuation", 1);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.LightDirPerVer",
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
LightDirPerVerEffect::~LightDirPerVerEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* LightDirPerVerEffect::CreateInstance (
    Light* light, Material* material) const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0,
        new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1,
        new0 CameraModelPositionConstant());
    instance->SetVertexConstant(0, 2,
        new0 MaterialEmissiveConstant(material));
    instance->SetVertexConstant(0, 3,
        new0 MaterialAmbientConstant(material));
    instance->SetVertexConstant(0, 4,
        new0 MaterialDiffuseConstant(material));
    instance->SetVertexConstant(0, 5,
        new0 MaterialSpecularConstant(material));
    instance->SetVertexConstant(0, 6,
        new0 LightModelDVectorConstant(light));
    instance->SetVertexConstant(0, 7,
        new0 LightAmbientConstant(light));
    instance->SetVertexConstant(0, 8,
        new0 LightDiffuseConstant(light));
    instance->SetVertexConstant(0, 9,
        new0 LightSpecularConstant(light));
    instance->SetVertexConstant(0, 10,
        new0 LightAttenuationConstant(light));
    return instance;
}
//----------------------------------------------------------------------------
VisualEffectInstance* LightDirPerVerEffect::CreateUniqueInstance (
    Light* light, Material* material)
{
    LightDirPerVerEffect* effect = new0 LightDirPerVerEffect();
    return effect->CreateInstance(light, material);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightDirPerVerEffect::LightDirPerVerEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void LightDirPerVerEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(LightDirPerVerEffect, source);
}
//----------------------------------------------------------------------------
void LightDirPerVerEffect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void LightDirPerVerEffect::PostLink ()
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
bool LightDirPerVerEffect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void LightDirPerVerEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(LightDirPerVerEffect, target);
}
//----------------------------------------------------------------------------
int LightDirPerVerEffect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int LightDirPerVerEffect::msDx9VRegisters[11] =
    { 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

int LightDirPerVerEffect::msOglVRegisters[11] =
    { 1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

int* LightDirPerVerEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string LightDirPerVerEffect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "vs_1_1\n"
    "def c14, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "add r1.xyz, -v0, c4\n"
    "dp3 r0.x, r1, r1\n"
    "rsq r0.x, r0.x\n"
    "mad r1.xyz, r0.x, r1, -c9\n"
    "dp3 r0.x, r1, r1\n"
    "rsq r0.x, r0.x\n"
    "mul r0.xyz, r0.x, r1\n"
    "dp3 r0.y, v1, r0\n"
    "dp3 r0.x, v1, c9\n"
    "mov r0.x, -r0\n"
    "mov r0.z, c8.w\n"
    "lit r0.yz, r0.xyzz\n"
    "mul r1.xyz, r0.y, c7\n"
    "mov r0.xyw, c6.xyzz\n"
    "mul r1.xyz, r1, c11\n"
    "mad r1.xyz, c10, r0.xyww, r1\n"
    "mul r0.xyz, r0.z, c8\n"
    "mad r0.xyz, r0, c12, r1\n"
    "mul r0.xyz, r0, c13.w\n"
    "add oD0.xyz, r0, c5\n"
    "mov r0.w, c14.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0.w, c7\n",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c14, 1.00000000, 0, 0, 0\n"
    "dcl_position v0\n"
    "dcl_normal v1\n"
    "add r1.xyz, -v0, c4\n"
    "dp3 r0.x, r1, r1\n"
    "rsq r0.x, r0.x\n"
    "mad r1.xyz, r0.x, r1, -c9\n"
    "dp3 r0.x, r1, r1\n"
    "rsq r0.x, r0.x\n"
    "mul r0.xyz, r0.x, r1\n"
    "dp3 r0.y, v1, r0\n"
    "dp3 r0.x, v1, c9\n"
    "mov r0.x, -r0\n"
    "mov r0.z, c8.w\n"
    "lit r0.yz, r0.xyzz\n"
    "mul r1.xyz, r0.y, c7\n"
    "mov r0.xyw, c6.xyzz\n"
    "mul r1.xyz, r1, c11\n"
    "mad r1.xyz, c10, r0.xyww, r1\n"
    "mul r0.xyz, r0.z, c8\n"
    "mad r0.xyz, r0, c12, r1\n"
    "mul r0.xyz, r0, c13.w\n"
    "add oD0.xyz, r0, c5\n"
    "mov r0.w, c14.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0.w, c7\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_color0 o1\n"
    "def c14, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "add r1.xyz, -v0, c4\n"
    "dp3 r0.x, r1, r1\n"
    "rsq r0.x, r0.x\n"
    "mad r1.xyz, r0.x, r1, -c9\n"
    "dp3 r0.x, r1, r1\n"
    "rsq r0.x, r0.x\n"
    "mul r0.xyz, r0.x, r1\n"
    "dp3 r0.y, v1, r0\n"
    "dp3 r0.x, v1, c9\n"
    "mov r0.x, -r0\n"
    "mov r0.z, c8.w\n"
    "lit r0.yz, r0.xyzz\n"
    "mul r1.xyz, r0.y, c7\n"
    "mov r0.xyw, c6.xyzz\n"
    "mul r1.xyz, r1, c11\n"
    "mad r1.xyz, c10, r0.xyww, r1\n"
    "mul r0.xyz, r0.z, c8\n"
    "mad r0.xyz, r0, c12, r1\n"
    "mul r0.xyz, r0, c13.w\n"
    "add o1.xyz, r0, c5\n"
    "mov r0.w, c14.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1.w, c7\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "PARAM c[15] = { { 1 }, program.local[1..14] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "ADD R1.xyz, -vertex.position, c[5];\n"
    "DP3 R0.x, R1, R1;\n"
    "RSQ R0.x, R0.x;\n"
    "MAD R1.xyz, R0.x, R1, -c[10];\n"
    "DP3 R0.x, R1, R1;\n"
    "RSQ R0.x, R0.x;\n"
    "MUL R0.xyz, R0.x, R1;\n"
    "DP3 R0.y, vertex.normal, R0;\n"
    "DP3 R0.x, vertex.normal, c[10];\n"
    "MOV R0.x, -R0;\n"
    "MOV R0.z, c[9].w;\n"
    "LIT R0.yz, R0.xyzz;\n"
    "MUL R1.xyz, R0.y, c[8];\n"
    "MOV R0.xyw, c[11].xyzz;\n"
    "MUL R1.xyz, R1, c[12];\n"
    "MAD R1.xyz, R0.xyww, c[7], R1;\n"
    "MUL R0.xyz, R0.z, c[9];\n"
    "MAD R0.xyz, R0, c[13], R1;\n"
    "MUL R0.xyz, R0, c[14].w;\n"
    "ADD result.color.xyz, R0, c[6];\n"
    "MOV R0.w, c[0].x;\n"
    "MOV R0.xyz, vertex.position;\n"
    "DP4 result.position.w, R0, c[4];\n"
    "DP4 result.position.z, R0, c[3];\n"
    "DP4 result.position.y, R0, c[2];\n"
    "DP4 result.position.x, R0, c[1];\n"
    "MOV result.color.w, c[8];\n"
    "END\n"
};

std::string LightDirPerVerEffect::msPPrograms[Shader::MAX_PROFILES] =
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
