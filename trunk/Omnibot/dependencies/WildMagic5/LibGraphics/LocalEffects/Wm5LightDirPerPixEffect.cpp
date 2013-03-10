// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightDirPerPixEffect.h"
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

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, LightDirPerPixEffect);
WM5_IMPLEMENT_STREAM(LightDirPerPixEffect);
WM5_IMPLEMENT_FACTORY(LightDirPerPixEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, LightDirPerPixEffect);

//----------------------------------------------------------------------------
LightDirPerPixEffect::LightDirPerPixEffect ()
{
    VertexShader* vshader = new0 VertexShader("Wm5.LightDirPerPix",
        2, 3, 1, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelNormal", Shader::VT_FLOAT3,
        Shader::VS_NORMAL);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexPosition", Shader::VT_FLOAT3,
        Shader::VS_TEXCOORD0);
    vshader->SetOutput(2, "vertexNormal", Shader::VT_FLOAT3,
        Shader::VS_TEXCOORD1);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.LightDirPerPix",
        2, 1, 10, 0, false);
    pshader->SetInput(0, "vertexPosition", Shader::VT_FLOAT3,
        Shader::VS_TEXCOORD0);
    pshader->SetInput(1, "vertexNormal", Shader::VT_FLOAT3,
        Shader::VS_TEXCOORD1);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetConstant(0, "CameraModelPosition", 1);
    pshader->SetConstant(1, "MaterialEmissive", 1);
    pshader->SetConstant(2, "MaterialAmbient", 1);
    pshader->SetConstant(3, "MaterialDiffuse", 1);
    pshader->SetConstant(4, "MaterialSpecular", 1);
    pshader->SetConstant(5, "LightModelDirection", 1);
    pshader->SetConstant(6, "LightAmbient", 1);
    pshader->SetConstant(7, "LightDiffuse", 1);
    pshader->SetConstant(8, "LightSpecular", 1);
    pshader->SetConstant(9, "LightAttenuation", 1);
    pshader->SetBaseRegisters(msPRegisters);
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
LightDirPerPixEffect::~LightDirPerPixEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* LightDirPerPixEffect::CreateInstance (Light* light,
    Material* material) const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0,
        new0 PVWMatrixConstant());
    instance->SetPixelConstant(0, 0,
        new0 CameraModelPositionConstant());
    instance->SetPixelConstant(0, 1,
        new0 MaterialEmissiveConstant(material));
    instance->SetPixelConstant(0, 2,
        new0 MaterialAmbientConstant(material));
    instance->SetPixelConstant(0, 3,
        new0 MaterialDiffuseConstant(material));
    instance->SetPixelConstant(0, 4,
        new0 MaterialSpecularConstant(material));
    instance->SetPixelConstant(0, 5,
        new0 LightModelDVectorConstant(light));
    instance->SetPixelConstant(0, 6,
        new0 LightAmbientConstant(light));
    instance->SetPixelConstant(0, 7,
        new0 LightDiffuseConstant(light));
    instance->SetPixelConstant(0, 8,
        new0 LightSpecularConstant(light));
    instance->SetPixelConstant(0, 9,
        new0 LightAttenuationConstant(light));
    return instance;
}
//----------------------------------------------------------------------------
VisualEffectInstance* LightDirPerPixEffect::CreateUniqueInstance (
    Light* light, Material* material)
{
    LightDirPerPixEffect* effect = new0 LightDirPerPixEffect();
    return effect->CreateInstance(light, material);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightDirPerPixEffect::LightDirPerPixEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void LightDirPerPixEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(LightDirPerPixEffect, source);
}
//----------------------------------------------------------------------------
void LightDirPerPixEffect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void LightDirPerPixEffect::PostLink ()
{
    VisualEffect::PostLink();

    VisualPass* pass = mTechniques[0]->GetPass(0);
    VertexShader* vshader = pass->GetVertexShader();
    PixelShader* pshader = pass->GetPixelShader();
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);
    pshader->SetBaseRegisters(msPRegisters);
    pshader->SetPrograms(msPPrograms);
}
//----------------------------------------------------------------------------
bool LightDirPerPixEffect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void LightDirPerPixEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(LightDirPerPixEffect, target);
}
//----------------------------------------------------------------------------
int LightDirPerPixEffect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int LightDirPerPixEffect::msDx9VRegisters[1] = { 0 };
int LightDirPerPixEffect::msOglVRegisters[1] = { 1 };
int* LightDirPerPixEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string LightDirPerPixEffect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position v0\n"
    "dcl_normal v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oT0.xyz, v0\n"
    "mov oT1.xyz, v1\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_texcoord0 o1\n"
    "dcl_texcoord1 o2\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1.xyz, v0\n"
    "mov o2.xyz, v1\n",

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
    "MOV result.texcoord[0].xyz, vertex.position;\n"
    "MOV result.texcoord[1].xyz, vertex.normal;\n"
    "END\n"
};

int LightDirPerPixEffect::msAllPRegisters[10] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

int* LightDirPerPixEffect::msPRegisters[Shader::MAX_PROFILES] =
{
    0,
    msAllPRegisters,
    msAllPRegisters,
    msAllPRegisters,
    msAllPRegisters
};

std::string LightDirPerPixEffect::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "def c10, 0.00000000, 0, 0, 0\n"
    "dcl t1.xyz\n"
    "dcl t0.xyz\n"
    "add r1.xyz, -t0, c0\n"
    "dp3 r0.x, r1, r1\n"
    "rsq r0.x, r0.x\n"
    "mad r2.xyz, r0.x, r1, -c5\n"
    "dp3 r1.x, r2, r2\n"
    "dp3 r0.x, t1, t1\n"
    "rsq r1.x, r1.x\n"
    "mul r1.xyz, r1.x, r2\n"
    "rsq r0.x, r0.x\n"
    "mul r2.xyz, r0.x, t1\n"
    "dp3 r0.x, r2, r1\n"
    "max r0.x, r0, c10\n"
    "pow r1, r0.x, c4.w\n"
    "dp3 r0.x, r2, c5\n"
    "max r2.x, -r0, c10\n"
    "cmp r0.x, r0, c10, r1\n"
    "mul r2.xyz, r2.x, c3\n"
    "mul r2.xyz, r2, c7\n"
    "mov r1.xyz, c2\n"
    "mad r1.xyz, c6, r1, r2\n"
    "mul r0.xyz, r0.x, c4\n"
    "mad r0.xyz, r0, c8, r1\n"
    "mul r0.xyz, r0, c9.w\n"
    "add r0.xyz, r0, c1\n"
    "mov r0.w, c3\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "def c10, 0.00000000, 0, 0, 0\n"
    "dcl_texcoord1 v1.xyz\n"
    "dcl_texcoord0 v0.xyz\n"
    "add r0.xyz, -v0, c0\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mad r0.xyz, r0.w, r0, -c5\n"
    "dp3 r1.x, r0, r0\n"
    "rsq r1.x, r1.x\n"
    "dp3 r0.w, v1, v1\n"
    "mul r0.xyz, r1.x, r0\n"
    "rsq r0.w, r0.w\n"
    "mul r1.xyz, r0.w, v1\n"
    "dp3 r0.x, r1, r0\n"
    "max r1.w, r0.x, c10.x\n"
    "pow r0, r1.w, c4.w\n"
    "dp3 r0.w, r1, c5\n"
    "mov r1.x, r0\n"
    "max r0.y, -r0.w, c10.x\n"
    "mul r0.xyz, r0.y, c3\n"
    "cmp r0.w, r0, c10.x, r1.x\n"
    "mul r1.xyz, r0, c7\n"
    "mov r0.xyz, c2\n"
    "mad r0.xyz, c6, r0, r1\n"
    "mul r1.xyz, r0.w, c4\n"
    "mad r0.xyz, r1, c8, r0\n"
    "mul r0.xyz, r0, c9.w\n"
    "add oC0.xyz, r0, c1\n"
    "mov oC0.w, c3\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[10] = { program.local[0..9] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "ADD R1.xyz, -fragment.texcoord[0], c[0];\n"
    "DP3 R0.x, R1, R1;\n"
    "RSQ R0.x, R0.x;\n"
    "MAD R0.xyz, R0.x, R1, -c[5];\n"
    "DP3 R0.w, R0, R0;\n"
    "RSQ R0.w, R0.w;\n"
    "DP3 R1.x, fragment.texcoord[1], fragment.texcoord[1];\n"
    "RSQ R1.x, R1.x;\n"
    "MUL R0.xyz, R0.w, R0;\n"
    "MUL R1.xyz, R1.x, fragment.texcoord[1];\n"
    "DP3 R0.y, R1, R0;\n"
    "DP3 R0.x, R1, c[5];\n"
    "MOV R0.x, -R0;\n"
    "MOV R0.z, c[4].w;\n"
    "LIT R0.yz, R0.xyzz;\n"
    "MUL R1.xyz, R0.y, c[3];\n"
    "MOV R0.xyw, c[6].xyzz;\n"
    "MUL R1.xyz, R1, c[7];\n"
    "MAD R1.xyz, R0.xyww, c[2], R1;\n"
    "MUL R0.xyz, R0.z, c[4];\n"
    "MAD R0.xyz, R0, c[8], R1;\n"
    "MUL R0.xyz, R0, c[9].w;\n"
    "ADD result.color.xyz, R0, c[1];\n"
    "MOV result.color.w, c[3];\n"
    "END\n"
};
//----------------------------------------------------------------------------
