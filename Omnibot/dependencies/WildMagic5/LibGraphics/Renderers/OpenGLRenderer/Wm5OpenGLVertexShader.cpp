// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5OpenGLVertexShader.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrVertexShader::PdrVertexShader (Renderer*, const VertexShader* vshader)
{
    const char* programText =
        vshader->GetProgram(VertexShader::GetProfile())->c_str();
    int programLength = (int)strlen(programText);

    glEnable(GL_VERTEX_PROGRAM_ARB);
    glGenProgramsARB(1, &mShader);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mShader);
    glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
        programLength, programText);
    glDisable(GL_VERTEX_PROGRAM_ARB);
}
//----------------------------------------------------------------------------
PdrVertexShader::~PdrVertexShader ()
{
    glDeleteProgramsARB(1, &mShader);
}
//----------------------------------------------------------------------------
void PdrVertexShader::Enable (Renderer* renderer,
    const VertexShader* vshader, const ShaderParameters* parameters)
{
    // Enable the buffer by setting the state.
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mShader);

    // Set the shader constants.
    int profile = VertexShader::GetProfile();
    const int numConstants = vshader->GetNumConstants();
    for (int i = 0; i < numConstants; ++i)
    {
        const int numRegisters = vshader->GetNumRegistersUsed(i);
        const float* data = parameters->GetConstant(i)->GetData();
        int baseRegister = vshader->GetBaseRegister(profile, i);
        for (int j = 0; j < numRegisters; ++j)
        {
            glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB,
                (GLuint)baseRegister, data);
            baseRegister++;
            data += 4;
        }
    }

    SetSamplerState(renderer, vshader, profile, parameters,
        renderer->mData->mMaxVShaderImages, renderer->mData->mCurrentSS);
}
//----------------------------------------------------------------------------
void PdrVertexShader::Disable (Renderer* renderer,
    const VertexShader* vshader, const ShaderParameters* parameters)
{
    glDisable(GL_VERTEX_PROGRAM_ARB);
    int profile = VertexShader::GetProfile();
    DisableTextures(renderer, vshader, profile, parameters,
        renderer->mData->mMaxVShaderImages);
}
//----------------------------------------------------------------------------
