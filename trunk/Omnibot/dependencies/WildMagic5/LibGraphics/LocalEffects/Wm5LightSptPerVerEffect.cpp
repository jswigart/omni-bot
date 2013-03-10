// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightSptPerVerEffect.h"
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
#include "Wm5LightModelPositionConstant.h"
#include "Wm5LightSpotConstant.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5WMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, LightSptPerVerEffect);
WM5_IMPLEMENT_STREAM(LightSptPerVerEffect);
WM5_IMPLEMENT_FACTORY(LightSptPerVerEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, LightSptPerVerEffect);

//----------------------------------------------------------------------------
LightSptPerVerEffect::LightSptPerVerEffect ()
{
    VertexShader* vshader = new0 VertexShader("Wm5.LightSptPerVer",
        2, 2, 14, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelNormal", Shader::VT_FLOAT3,
        Shader::VS_NORMAL);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetConstant(1, "WMatrix", 4);
    vshader->SetConstant(2, "CameraModelPosition", 1);
    vshader->SetConstant(3, "MaterialEmissive", 1);
    vshader->SetConstant(4, "MaterialAmbient", 1);
    vshader->SetConstant(5, "MaterialDiffuse", 1);
    vshader->SetConstant(6, "MaterialSpecular", 1);
    vshader->SetConstant(7, "LightModelPosition", 1);
    vshader->SetConstant(8, "LightModelDirection", 1);
    vshader->SetConstant(9, "LightAmbient", 1);
    vshader->SetConstant(10, "LightDiffuse", 1);
    vshader->SetConstant(11, "LightSpecular", 1);
    vshader->SetConstant(12, "LightSpotCutoff", 1);
    vshader->SetConstant(13, "LightAttenuation", 1);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.LightSptPerVer",
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
LightSptPerVerEffect::~LightSptPerVerEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* LightSptPerVerEffect::CreateInstance (Light* light,
    Material* material) const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0,
        new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1,
        new0 WMatrixConstant());
    instance->SetVertexConstant(0, 2,
        new0 CameraModelPositionConstant());
    instance->SetVertexConstant(0, 3,
        new0 MaterialEmissiveConstant(material));
    instance->SetVertexConstant(0, 4,
        new0 MaterialAmbientConstant(material));
    instance->SetVertexConstant(0, 5,
        new0 MaterialDiffuseConstant(material));
    instance->SetVertexConstant(0, 6,
        new0 MaterialSpecularConstant(material));
    instance->SetVertexConstant(0, 7,
        new0 LightModelPositionConstant(light));
    instance->SetVertexConstant(0, 8,
        new0 LightModelDVectorConstant(light));
    instance->SetVertexConstant(0, 9,
        new0 LightAmbientConstant(light));
    instance->SetVertexConstant(0, 10,
        new0 LightDiffuseConstant(light));
    instance->SetVertexConstant(0, 11,
        new0 LightSpecularConstant(light));
    instance->SetVertexConstant(0, 12,
        new0 LightSpotConstant(light));
    instance->SetVertexConstant(0, 13,
        new0 LightAttenuationConstant(light));
    return instance;
}
//----------------------------------------------------------------------------
VisualEffectInstance* LightSptPerVerEffect::CreateUniqueInstance (
    Light* light, Material* material)
{
    LightSptPerVerEffect* effect = new0 LightSptPerVerEffect();
    return effect->CreateInstance(light, material);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightSptPerVerEffect::LightSptPerVerEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void LightSptPerVerEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(LightSptPerVerEffect, source);
}
//----------------------------------------------------------------------------
void LightSptPerVerEffect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void LightSptPerVerEffect::PostLink ()
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
bool LightSptPerVerEffect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void LightSptPerVerEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(LightSptPerVerEffect, target);
}
//----------------------------------------------------------------------------
int LightSptPerVerEffect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int LightSptPerVerEffect::msDx9VRegisters[14] =
    { 0, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

int LightSptPerVerEffect::msOglVRegisters[14] =
    { 1, 5, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };

int* LightSptPerVerEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string LightSptPerVerEffect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "vs_1_1\n"
    "def c20, 1.00000000, 0.00000000, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "add r0.xyz, v0, -c13\n"
    "dp3 r0.w, r0, r0\n"
    "add r2.xyz, -v0, c8\n"
    "rsq r0.w, r0.w\n"
    "mul r1.xyz, r0.w, r0\n"
    "dp3 r1.w, r2, r2\n"
    "rsq r0.w, r1.w\n"
    "mad r2.xyz, r0.w, r2, -r1\n"
    "dp3 r3.xy, r1, c14\n"
    "dp3 r0.w, r2, r2\n"
    "rsq r0.w, r0.w\n"
    "mul r2.xyz, r0.w, r2\n"
    "dp3 r2.y, v1, r2\n"
    "dp3 r1.x, r1, v1\n"
    "sge r0.w, r3.x, c18.y\n"
    "sge r1.w, c20.y, r0\n"
    "sge r0.w, r0, c20.y\n"
    "mul r0.w, r0, r1\n"
    "max r0.w, -r0, r0\n"
    "slt r0.w, c20.y, r0\n"
    "mov r2.x, -r1\n"
    "mov r2.z, c12.w\n"
    "lit r1.yz, r2.xyzz\n"
    "mov r3.z, c18.w\n"
    "lit r2.z, r3.xyzz\n"
    "mov r1.w, r2.z\n"
    "add r0.w, -r0, c20.x\n"
    "mul r1.xyz, r0.w, r1.yzww\n"
    "dp3 r2.z, r0, c6\n"
    "dp3 r2.x, r0, c4\n"
    "dp3 r2.y, r0, c5\n"
    "dp3 r0.w, r2, r2\n"
    "mul r2.xyz, r1.y, c12\n"
    "rsq r0.w, r0.w\n"
    "mul r0.xyz, r1.x, c11\n"
    "rcp r0.w, r0.w\n"
    "mad r1.x, r0.w, c19.z, c19.y\n"
    "mad r0.w, r0, r1.x, c19.x\n"
    "mul r2.xyz, r2, c17\n"
    "mad r0.xyz, r0, c16, r2\n"
    "mul r0.xyz, r1.z, r0\n"
    "mov r1.xyz, c10\n"
    "mad r1.xyz, c15, r1, r0\n"
    "rcp r0.w, r0.w\n"
    "mul r0.x, r0.w, c19.w\n"
    "mad oD0.xyz, r0.x, r1, c9\n"
    "mov r0.w, c20.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0.w, c11\n",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c20, 1.00000000, 0.00000000, 0, 0\n"
    "dcl_position v0\n"
    "dcl_normal v1\n"
    "add r1.xyz, v0, -c13\n"
    "dp3 r0.x, r1, r1\n"
    "add r2.xyz, -v0, c8\n"
    "dp3 r0.w, r2, r2\n"
    "rsq r0.x, r0.x\n"
    "mul r0.xyz, r0.x, r1\n"
    "rsq r0.w, r0.w\n"
    "mad r2.xyz, r0.w, r2, -r0\n"
    "dp3 r1.w, r0, c14\n"
    "dp3 r0.w, r2, r2\n"
    "rsq r0.w, r0.w\n"
    "mul r2.xyz, r0.w, r2\n"
    "dp3 r2.y, v1, r2\n"
    "sge r0.w, r1, c18.y\n"
    "sge r2.x, c20.y, r0.w\n"
    "sge r0.w, r0, c20.y\n"
    "mul r0.w, r0, r2.x\n"
    "dp3 r0.x, r0, v1\n"
    "mov r2.x, -r0\n"
    "mov r2.z, c12.w\n"
    "lit r2.yz, r2.xyzz\n"
    "max r2.x, -r0.w, r0.w\n"
    "pow r0, r1.w, c18.w\n"
    "slt r0.x, c20.y, r2\n"
    "dp3 r2.x, r1, c4\n"
    "mov r2.w, r0\n"
    "add r0.x, -r0, c20\n"
    "mul r0.xyz, r0.x, r2.yzww\n"
    "dp3 r2.z, r1, c6\n"
    "dp3 r2.y, r1, c5\n"
    "dp3 r0.w, r2, r2\n"
    "mul r1.xyz, r0.x, c11\n"
    "rsq r0.x, r0.w\n"
    "mul r2.xyz, r0.y, c12\n"
    "rcp r0.x, r0.x\n"
    "mad r0.y, r0.x, c19.z, c19\n"
    "mad r0.w, r0.x, r0.y, c19.x\n"
    "mul r2.xyz, r2, c17\n"
    "mad r1.xyz, r1, c16, r2\n"
    "mul r0.xyz, r0.z, r1\n"
    "mov r1.xyz, c10\n"
    "mad r1.xyz, c15, r1, r0\n"
    "rcp r0.w, r0.w\n"
    "mul r0.x, r0.w, c19.w\n"
    "mad oD0.xyz, r0.x, r1, c9\n"
    "mov r0.w, c20.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oD0.w, c11\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_color0 o1\n"
    "def c20, 1.00000000, 0.00000000, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "add r1.xyz, v0, -c13\n"
    "dp3 r1.w, r1, r1\n"
    "rsq r1.w, r1.w\n"
    "mul r2.xyz, r1.w, r1\n"
    "mov r0.xyz, v0\n"
    "mov r0.w, c20.x\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp3 r1.w, r2, c14\n"
    "dp4 o0.x, r0, c0\n"
    "if_ge r1.w, c18.y\n"
    "add r0.xyz, -v0, c8\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mad r0.xyz, r0.w, r0, -r2\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mul r3.xyz, r0.w, r0\n"
    "pow r0, r1.w, c18.w\n"
    "dp3 r0.y, v1, r3\n"
    "dp3 r0.x, v1, r2\n"
    "mov r0.z, c12.w\n"
    "mov r0.x, -r0\n"
    "lit r0.yz, r0.xyzz\n"
    "else\n"
    "mov r0.yzw, c20.y\n"
    "endif\n"
    "dp3 r2.z, r1, c6\n"
    "dp3 r2.x, r1, c4\n"
    "dp3 r2.y, r1, c5\n"
    "dp3 r0.x, r2, r2\n"
    "rsq r0.x, r0.x\n"
    "rcp r1.w, r0.x\n"
    "mul r1.xyz, r0.z, c12\n"
    "mad r2.x, r1.w, c19.z, c19.y\n"
    "mul r0.xyz, r0.y, c11\n"
    "mul r1.xyz, r1, c17\n"
    "mad r1.xyz, r0, c16, r1\n"
    "mad r0.x, r1.w, r2, c19\n"
    "mul r2.xyz, r0.w, r1\n"
    "rcp r0.x, r0.x\n"
    "mov r1.xyz, c10\n"
    "mad r1.xyz, c15, r1, r2\n"
    "mul r0.x, r0, c19.w\n"
    "mad o1.xyz, r0.x, r1, c9\n"
    "mov o1.w, c11\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "PARAM c[21] = { { 1, 0 }, program.local[1..20] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "ADD R1.xyz, vertex.position, -c[14];\n"
    "DP3 R0.x, R1, R1;\n"
    "ADD R2.xyz, -vertex.position, c[9];\n"
    "DP3 R0.w, R2, R2;\n"
    "RSQ R0.x, R0.x;\n"
    "MUL R0.xyz, R0.x, R1;\n"
    "RSQ R0.w, R0.w;\n"
    "MAD R2.xyz, R0.w, R2, -R0;\n"
    "DP3 R0.w, R2, R2;\n"
    "RSQ R1.w, R0.w;\n"
    "MUL R2.xyz, R1.w, R2;\n"
    "DP3 R0.w, R0, c[15];\n"
    "SGE R1.w, R0, c[19].y;\n"
    "DP3 R2.y, vertex.normal, R2;\n"
    "DP3 R0.x, R0, vertex.normal;\n"
    "MOV R2.x, -R0;\n"
    "MOV R2.z, c[13].w;\n"
    "LIT R0.yz, R2.xyzz;\n"
    "ABS R1.w, R1;\n"
    "DP3 R2.z, R1, c[7];\n"
    "DP3 R2.x, R1, c[5];\n"
    "DP3 R2.y, R1, c[6];\n"
    "POW R0.w, R0.w, c[19].w;\n"
    "SGE R0.x, c[0].y, R1.w;\n"
    "MAD R0.xyz, -R0.yzww, R0.x, R0.yzww;\n"
    "DP3 R0.w, R2, R2;\n"
    "MUL R1.xyz, R0.x, c[12];\n"
    "RSQ R0.x, R0.w;\n"
    "MUL R2.xyz, R0.y, c[13];\n"
    "RCP R0.x, R0.x;\n"
    "MAD R0.y, R0.x, c[20].z, c[20];\n"
    "MAD R0.w, R0.x, R0.y, c[20].x;\n"
    "MUL R2.xyz, R2, c[18];\n"
    "MAD R1.xyz, R1, c[17], R2;\n"
    "MUL R0.xyz, R0.z, R1;\n"
    "MOV R1.xyz, c[16];\n"
    "MAD R1.xyz, R1, c[11], R0;\n"
    "RCP R0.w, R0.w;\n"
    "MUL R0.x, R0.w, c[20].w;\n"
    "MAD result.color.xyz, R0.x, R1, c[10];\n"
    "MOV R0.w, c[0].x;\n"
    "MOV R0.xyz, vertex.position;\n"
    "DP4 result.position.w, R0, c[4];\n"
    "DP4 result.position.z, R0, c[3];\n"
    "DP4 result.position.y, R0, c[2];\n"
    "DP4 result.position.x, R0, c[1];\n"
    "MOV result.color.w, c[12];\n"
    "END\n"
};

std::string LightSptPerVerEffect::msPPrograms[Shader::MAX_PROFILES] =
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
