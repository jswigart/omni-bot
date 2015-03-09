// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VisualEffect.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, VisualEffect);
WM5_IMPLEMENT_STREAM(VisualEffect);
WM5_IMPLEMENT_FACTORY(VisualEffect);

//----------------------------------------------------------------------------
VisualEffect::VisualEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffect::~VisualEffect ()
{
}
//----------------------------------------------------------------------------
void VisualEffect::InsertTechnique (VisualTechnique* technique)
{
    if (technique)
    {
        mTechniques.push_back(technique);
    }
    else
    {
        assertion(false, "Input to InsertTechnique must be nonnull.\n");
    }
}
//----------------------------------------------------------------------------
VisualTechnique* VisualEffect::GetTechnique (int techniqueIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex];
    }

    assertion(false, "Invalid index in GetTechnique.\n");
    return 0;
}
//----------------------------------------------------------------------------
int VisualEffect::GetNumPasses (int techniqueIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetNumPasses();
    }

    assertion(false, "Invalid index in GetNumPasses.\n");
    return 0;
}
//----------------------------------------------------------------------------
VisualPass* VisualEffect::GetPass (int techniqueIndex, int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetPass(passIndex);
    }

    assertion(false, "Invalid index in GetPass.\n");
    return 0;
}
//----------------------------------------------------------------------------
VertexShader* VisualEffect::GetVertexShader (int techniqueIndex,
    int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetVertexShader(passIndex);
    }

    assertion(false, "Invalid index in GetVertexShader.\n");
    return 0;
}
//----------------------------------------------------------------------------
PixelShader* VisualEffect::GetPixelShader (int techniqueIndex,
    int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetPixelShader(passIndex);
    }

    assertion(false, "Invalid index in GetPixelShader.\n");
    return 0;
}
//----------------------------------------------------------------------------
AlphaState* VisualEffect::GetAlphaState (int techniqueIndex,
    int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetAlphaState(passIndex);
    }

    assertion(false, "Invalid index in GetAlphaState.\n");
    return 0;
}
//----------------------------------------------------------------------------
CullState* VisualEffect::GetCullState (int techniqueIndex,
    int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetCullState(passIndex);
    }

    assertion(false, "Invalid index in GetCullState.\n");
    return 0;
}
//----------------------------------------------------------------------------
DepthState* VisualEffect::GetDepthState (int techniqueIndex,
    int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetDepthState(passIndex);
    }

    assertion(false, "Invalid index in GetDepthState.\n");
    return 0;
}
//----------------------------------------------------------------------------
OffsetState* VisualEffect::GetOffsetState (int techniqueIndex,
    int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetOffsetState(passIndex);
    }

    assertion(false, "Invalid index in GetOffsetState.\n");
    return 0;
}
//----------------------------------------------------------------------------
StencilState* VisualEffect::GetStencilState (int techniqueIndex,
    int passIndex) const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetStencilState(passIndex);
    }

    assertion(false, "Invalid index in GetStencilState.\n");
    return 0;
}
//----------------------------------------------------------------------------
WireState* VisualEffect::GetWireState (int techniqueIndex, int passIndex)
    const
{
    if (0 <= techniqueIndex && techniqueIndex < (int)mTechniques.size())
    {
        return mTechniques[techniqueIndex]->GetWireState(passIndex);
    }

    assertion(false, "Invalid index in GetWireState.\n");
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for load/save of raw data.
//----------------------------------------------------------------------------
VisualEffect* VisualEffect::LoadWMFX (const std::string& name, int mode)
{
    return new0 VisualEffect(name, mode);
}
//----------------------------------------------------------------------------
void VisualEffect::SaveWMFX (const std::string& name, int mode)
{
    FileIO outFile(name, mode);
    if (!outFile)
    {
        assertion(false, "Failed to open file %s\n", name.c_str());
        return;
    }

    int numTechniques = (int)mTechniques.size();
    outFile.Write(sizeof(int), &numTechniques);
    for (int i = 0; i < numTechniques; ++i)
    {
        SaveVisualTechnique(outFile, mTechniques[i]);
    }

    outFile.Close();
}
//----------------------------------------------------------------------------
VisualEffect::VisualEffect (const std::string& name, int mode)
{
    FileIO inFile(name, mode);
    if (!inFile)
    {
        assertion(false, "Failed to open file %s\n", name.c_str());
        return;
    }

    int numTechniques;
    inFile.Read(sizeof(int), &numTechniques);
    for (int i = 0; i < numTechniques; ++i)
    {
        InsertTechnique(LoadVisualTechnique(inFile));
    }

    inFile.Close();
}
//----------------------------------------------------------------------------
VisualTechnique* VisualEffect::LoadVisualTechnique (FileIO& inFile)
{
    VisualTechnique* technique = new0 VisualTechnique();

    int numPasses;
    inFile.Read(sizeof(int), &numPasses);
    for (int i = 0; i < numPasses; ++i)
    {
        technique->InsertPass(LoadVisualPass(inFile));
    }
    return technique;
}
//----------------------------------------------------------------------------
VisualPass* VisualEffect::LoadVisualPass (FileIO& inFile)
{
    VisualPass* pass = new0 VisualPass();

    pass->SetVertexShader((VertexShader*)LoadShader(inFile, true));
    pass->SetPixelShader((PixelShader*)LoadShader(inFile, false));
    pass->SetAlphaState(LoadAlphaState(inFile));
    pass->SetCullState(LoadCullState(inFile));
    pass->SetDepthState(LoadDepthState(inFile));
    pass->SetOffsetState(LoadOffsetState(inFile));
    pass->SetStencilState(LoadStencilState(inFile));
    pass->SetWireState(LoadWireState(inFile));

    return pass;
}
//----------------------------------------------------------------------------
Shader* VisualEffect::LoadShader (FileIO& inFile, bool isVertexShader)
{
    std::string programName = LoadStdString(inFile);

    int numInputs, numOutputs, numConstants, numSamplers, numProfiles;
    inFile.Read(sizeof(int), &numInputs);
    inFile.Read(sizeof(int), &numOutputs);
    inFile.Read(sizeof(int), &numConstants);
    inFile.Read(sizeof(int), &numSamplers);
    inFile.Read(sizeof(int), &numProfiles);

    Shader* shader;
    if (isVertexShader)
    {
        shader = new0 VertexShader(programName, numInputs, numOutputs,
            numConstants, numSamplers, true);
    }
    else
    {
        shader = new0 PixelShader(programName, numInputs, numOutputs,
            numConstants, numSamplers, true);
    }

    std::string name;
    int i, type, semantic, numRegistersUsed, filter, coord[3];
    float lodBias, anisotropy;
    Float4 borderColor;

    for (i = 0; i < numInputs; ++i)
    {
        name = LoadStdString(inFile);
        inFile.Read(sizeof(int), &type);
        inFile.Read(sizeof(int), &semantic);
        shader->SetInput(i, name, (Shader::VariableType)type,
            (Shader::VariableSemantic)semantic);
    }

    for (i = 0; i < numOutputs; ++i)
    {
        name = LoadStdString(inFile);
        inFile.Read(sizeof(int), &type);
        inFile.Read(sizeof(int), &semantic);
        shader->SetOutput(i, name, (Shader::VariableType)type,
            (Shader::VariableSemantic)semantic);
    }

    for (i = 0; i < numConstants; ++i)
    {
        name = LoadStdString(inFile);
        inFile.Read(sizeof(int), &numRegistersUsed);
        shader->SetConstant(i, name, numRegistersUsed);
    }

    for (i = 0; i < numSamplers; ++i)
    {
        name = LoadStdString(inFile);
        inFile.Read(sizeof(int), &type);
        inFile.Read(sizeof(int), &filter);
        inFile.Read(sizeof(int), 3, coord);
        inFile.Read(sizeof(float), &lodBias);
        inFile.Read(sizeof(float), &anisotropy);
        inFile.Read(sizeof(float), 4, &borderColor[0]);
        shader->SetSampler(i, name, (Shader::SamplerType)type);
        shader->SetFilter(i, (Shader::SamplerFilter)filter);
        shader->SetCoordinate(i, 0, (Shader::SamplerCoordinate)coord[0]);
        shader->SetCoordinate(i, 1, (Shader::SamplerCoordinate)coord[1]);
        shader->SetCoordinate(i, 2, (Shader::SamplerCoordinate)coord[2]);
        shader->SetLodBias(i, lodBias);
        shader->SetAnisotropy(i, anisotropy);
        shader->SetBorderColor(i, borderColor);
    }

    for (i = 0; i < numProfiles; ++i)
    {
        inFile.Read(sizeof(int), &type);
        if (0 <= type && type < Shader::MAX_PROFILES)
        {
            std::string programName = LoadStdString(inFile);
            shader->SetProgram(i, programName);

            if (programName != "")
            {
                int j;
                for (j = 0; j < numConstants; ++j)
                {
                    int baseRegister;
                    inFile.Read(sizeof(int), &baseRegister);
                    shader->SetBaseRegister(i, j, baseRegister);
                }

                for (j = 0; j < numSamplers; ++j)
                {
                    int textureUnit;
                    inFile.Read(sizeof(int), &textureUnit);
                    shader->SetTextureUnit(i, j, textureUnit);
                }
            }
        }
        else
        {
            assertion(false, "Invalid profile.\n");
        }
    }

    return shader;
}
//----------------------------------------------------------------------------
AlphaState* VisualEffect::LoadAlphaState (FileIO& inFile)
{
    int blendEnabled, srcBlend, dstBlend, compareEnabled, compare;
    float reference;
    Float4 constantColor;
    inFile.Read(sizeof(int), &blendEnabled);
    inFile.Read(sizeof(int), &srcBlend);
    inFile.Read(sizeof(int), &dstBlend);
    inFile.Read(sizeof(int), &compareEnabled);
    inFile.Read(sizeof(int), &compare);
    inFile.Read(sizeof(float), &reference);
    inFile.Read(sizeof(float), 4, &constantColor[0]);

    AlphaState* astate = new0 AlphaState();
    astate->BlendEnabled = (blendEnabled != 0);
    astate->SrcBlend = (AlphaState::SrcBlendMode)srcBlend;
    astate->DstBlend = (AlphaState::DstBlendMode)dstBlend;
    astate->CompareEnabled = (compareEnabled != 0);
    astate->Compare = (AlphaState::CompareMode)compare;
    astate->Reference = reference;
    astate->ConstantColor = constantColor;

    return astate;
}
//----------------------------------------------------------------------------
CullState* VisualEffect::LoadCullState (FileIO& inFile)
{
    int enabled, ccwOrder;
    inFile.Read(sizeof(int), &enabled);
    inFile.Read(sizeof(int), &ccwOrder);

    CullState* cstate = new0 CullState();
    cstate->Enabled = (enabled != 0);
    cstate->CCWOrder = (ccwOrder != 0);

    return cstate;
}
//----------------------------------------------------------------------------
DepthState* VisualEffect::LoadDepthState (FileIO& inFile)
{
    int enabled, writable, compare;
    inFile.Read(sizeof(int), &enabled);
    inFile.Read(sizeof(int), &writable);
    inFile.Read(sizeof(int), &compare);

    DepthState* dstate = new0 DepthState();
    dstate->Enabled = (enabled != 0);
    dstate->Writable = (writable != 0);
    dstate->Compare = (DepthState::CompareMode)compare;

    return dstate;
}
//----------------------------------------------------------------------------
OffsetState* VisualEffect::LoadOffsetState (FileIO& inFile)
{
    int fillEnabled, lineEnabled, pointEnabled;
    float scale, bias;
    inFile.Read(sizeof(int), &fillEnabled);
    inFile.Read(sizeof(int), &lineEnabled);
    inFile.Read(sizeof(int), &pointEnabled);
    inFile.Read(sizeof(float), &scale);
    inFile.Read(sizeof(float), &bias);

    OffsetState* ostate = new0 OffsetState();
    ostate->FillEnabled = (fillEnabled != 0);
    ostate->LineEnabled = (lineEnabled != 0);
    ostate->PointEnabled = (pointEnabled != 0);
    ostate->Scale = scale;
    ostate->Bias = bias;

    return ostate;
}
//----------------------------------------------------------------------------
StencilState* VisualEffect::LoadStencilState (FileIO& inFile)
{
    int enabled, compare, onFail, onZFail, onZPass;
    unsigned int reference, mask, writeMask;
    inFile.Read(sizeof(int), &enabled);
    inFile.Read(sizeof(int), &compare);
    inFile.Read(sizeof(unsigned int), &reference);
    inFile.Read(sizeof(unsigned int), &mask);
    inFile.Read(sizeof(unsigned int), &writeMask);
    inFile.Read(sizeof(int), &onFail);
    inFile.Read(sizeof(int), &onZFail);
    inFile.Read(sizeof(int), &onZPass);

    StencilState* sstate = new0 StencilState();
    sstate->Enabled = (enabled != 0);
    sstate->Compare = (StencilState::CompareMode)compare;
    sstate->Reference = reference;
    sstate->Mask = mask;
    sstate->WriteMask = writeMask;
    sstate->OnFail = (StencilState::OperationType)onFail;
    sstate->OnZFail = (StencilState::OperationType)onZFail;
    sstate->OnZFail = (StencilState::OperationType)onZPass;

    return sstate;
}
//----------------------------------------------------------------------------
WireState* VisualEffect::LoadWireState (FileIO& inFile)
{
    int enabled;
    inFile.Read(sizeof(int), &enabled);

    WireState* wstate = new0 WireState();
    wstate->Enabled = (enabled != 0);

    return wstate;
}
//----------------------------------------------------------------------------
std::string VisualEffect::LoadStdString (FileIO& inFile)
{
    int length;
    inFile.Read(sizeof(int), &length);
    if (length <= 0)
    {
        return std::string("");
    }

    // Strings are written in multiples of four bytes.
    int padding = (length % 4);
    if (padding > 0)
    {
        padding = 4 - padding;
    }

    int numBytes = length + padding;
    char* text = new1<char>(numBytes);
    inFile.Read(sizeof(char), numBytes, text);

    std::string name;
    name.assign(text, length);
    delete1(text);

    return name;
}
//----------------------------------------------------------------------------
void VisualEffect::SaveVisualTechnique (FileIO& outFile,
    VisualTechnique* technique)
{
    int numPasses = technique->GetNumPasses();
    outFile.Write(sizeof(int), &numPasses);
    for (int i = 0; i < numPasses; ++i)
    {
        SaveVisualPass(outFile, technique->GetPass(i));
    }
}
//----------------------------------------------------------------------------
void VisualEffect::SaveVisualPass (FileIO& outFile, VisualPass* pass)
{
    SaveShader(outFile, pass->GetVertexShader());
    SaveShader(outFile, pass->GetPixelShader());
    SaveAlphaState(outFile, pass->GetAlphaState());
    SaveCullState(outFile, pass->GetCullState());
    SaveDepthState(outFile, pass->GetDepthState());
    SaveOffsetState(outFile, pass->GetOffsetState());
    SaveStencilState(outFile, pass->GetStencilState());
    SaveWireState(outFile, pass->GetWireState());
}
//----------------------------------------------------------------------------
void VisualEffect::SaveShader (FileIO& outFile, Shader* shader)
{
    std::string programName = shader->GetName();
    SaveStdString(outFile, programName);

    int numInputs = shader->GetNumInputs();
    int numOutputs = shader->GetNumOutputs();
    int numConstants = shader->GetNumConstants();
    int numSamplers = shader->GetNumSamplers();
    int numProfiles = Shader::MAX_PROFILES;
    outFile.Write(sizeof(int), &numInputs);
    outFile.Write(sizeof(int), &numOutputs);
    outFile.Write(sizeof(int), &numConstants);
    outFile.Write(sizeof(int), &numSamplers);
    outFile.Write(sizeof(int), &numProfiles);

    std::string name;
    int i, type, semantic, numRegistersUsed, filter, coord[3];
    float lodBias, anisotropy;
    Float4 borderColor;

    for (i = 0; i < numInputs; ++i)
    {
        name = shader->GetInputName(i);
        type = (int)shader->GetInputType(i);
        semantic = (int)shader->GetInputSemantic(i);
        SaveStdString(outFile, name);
        outFile.Write(sizeof(int), &type);
        outFile.Write(sizeof(int), &semantic);
    }

    for (i = 0; i < numOutputs; ++i)
    {
        name = shader->GetOutputName(i);
        type = (int)shader->GetOutputType(i);
        semantic = (int)shader->GetOutputSemantic(i);
        SaveStdString(outFile, name);
        outFile.Write(sizeof(int), &type);
        outFile.Write(sizeof(int), &semantic);
    }

    for (i = 0; i < numConstants; ++i)
    {
        name = shader->GetConstantName(i);
        numRegistersUsed = shader->GetNumRegistersUsed(i);
        SaveStdString(outFile, name);
        outFile.Write(sizeof(int), &numRegistersUsed);
    }

    for (i = 0; i < numSamplers; ++i)
    {
        name = shader->GetSamplerName(i);
        type = shader->GetSamplerType(i);
        filter = shader->GetFilter(i);
        coord[0] = shader->GetCoordinate(i, 0);
        coord[1] = shader->GetCoordinate(i, 1);
        coord[2] = shader->GetCoordinate(i, 2);
        lodBias = shader->GetLodBias(i);
        anisotropy = shader->GetAnisotropy(i);
        borderColor = shader->GetBorderColor(i);

        SaveStdString(outFile, name);
        outFile.Write(sizeof(int), &type);
        outFile.Write(sizeof(int), &filter);
        outFile.Write(sizeof(int), 3, coord);
        outFile.Write(sizeof(float), &lodBias);
        outFile.Write(sizeof(float), &anisotropy);
        outFile.Write(sizeof(float), 4, &borderColor[0]);
    }

    for (i = 0; i < numProfiles; ++i)
    {
        outFile.Write(sizeof(int), &i);
        std::string programString = *shader->GetProgram(i);
        SaveStdString(outFile, programString);

        if (programString != "")
        {
            int j;
            for (j = 0; j < numConstants; ++j)
            {
                // TEST program for not "" before "get"
                int baseRegister = shader->GetBaseRegister(i, j);
                outFile.Write(sizeof(int), &baseRegister);
            }
            for (j = 0; j < numSamplers; ++j)
            {
                int textureUnit = shader->GetTextureUnit(i, j);
                outFile.Write(sizeof(int), &textureUnit);
            }
        }
    }
}
//----------------------------------------------------------------------------
void VisualEffect::SaveAlphaState (FileIO& outFile, AlphaState* astate)
{
    int blendEnabled = (astate->BlendEnabled ? 1 : 0);
    int srcBlend = (int)astate->SrcBlend;
    int dstBlend = (int)astate->DstBlend;
    int compareEnabled = (astate->CompareEnabled ? 1 : 0);
    int compare = (int)astate->Compare;
    float reference = astate->Reference;
    Float4 constantColor = astate->ConstantColor;

    outFile.Write(sizeof(int), &blendEnabled);
    outFile.Write(sizeof(int), &srcBlend);
    outFile.Write(sizeof(int), &dstBlend);
    outFile.Write(sizeof(int), &compareEnabled);
    outFile.Write(sizeof(int), &compare);
    outFile.Write(sizeof(float), &reference);
    outFile.Write(sizeof(float), 4, &constantColor[0]);
}
//----------------------------------------------------------------------------
void VisualEffect::SaveCullState (FileIO& outFile, CullState* cstate)
{
    int enabled = (cstate->Enabled ? 1 : 0);
    int ccwOrder = (cstate->CCWOrder ? 1 : 0);

    outFile.Write(sizeof(int), &enabled);
    outFile.Write(sizeof(int), &ccwOrder);
}
//----------------------------------------------------------------------------
void VisualEffect::SaveDepthState (FileIO& outFile, DepthState* dstate)
{
    int enabled = (dstate->Enabled ? 1 : 0);
    int writable = (dstate->Writable ? 1 : 0);
    int compare = (int)dstate->Compare;

    outFile.Write(sizeof(int), &enabled);
    outFile.Write(sizeof(int), &writable);
    outFile.Write(sizeof(int), &compare);
}
//----------------------------------------------------------------------------
void VisualEffect::SaveOffsetState (FileIO& outFile, OffsetState* ostate)
{
    int fillEnabled = (ostate->FillEnabled ? 1 : 0);
    int lineEnabled = (ostate->LineEnabled ? 1 : 0);
    int pointEnabled = (ostate->PointEnabled ? 1 : 0);
    float scale = ostate->Scale;
    float bias = ostate->Bias;

    outFile.Write(sizeof(int), &fillEnabled);
    outFile.Write(sizeof(int), &lineEnabled);
    outFile.Write(sizeof(int), &pointEnabled);
    outFile.Write(sizeof(float), &scale);
    outFile.Write(sizeof(float), &bias);
}
//----------------------------------------------------------------------------
void VisualEffect::SaveStencilState (FileIO& outFile, StencilState* sstate)
{
    int enabled = (sstate->Enabled ? 1 : 0);
    int compare = (int)sstate->Compare;
    unsigned int reference = sstate->Reference;
    unsigned int mask = sstate->Mask;
    unsigned int writeMask = sstate->WriteMask;
    int onFail = (int)sstate->OnFail;
    int onZFail = (int)sstate->OnZFail;
    int onZPass = (int)sstate->OnZPass;

    outFile.Write(sizeof(int), &enabled);
    outFile.Write(sizeof(int), &compare);
    outFile.Write(sizeof(unsigned int), &reference);
    outFile.Write(sizeof(unsigned int), &mask);
    outFile.Write(sizeof(unsigned int), &writeMask);
    outFile.Write(sizeof(int), &onFail);
    outFile.Write(sizeof(int), &onZFail);
    outFile.Write(sizeof(int), &onZPass);
}
//----------------------------------------------------------------------------
void VisualEffect::SaveWireState (FileIO& outFile, WireState* wstate)
{
    int enabled = (wstate->Enabled ? 1 : 0);

    outFile.Write(sizeof(int), &enabled);
}
//----------------------------------------------------------------------------
void VisualEffect::SaveStdString (FileIO& outFile, std::string& name)
{
    int length = (int)name.length();
    outFile.Write(sizeof(int), &length);
    if (length > 0)
    {
        outFile.Write(sizeof(char), length, name.c_str());

        // Strings are written in multiples of four bytes.
        char zero[4] = { 0, 0, 0, 0 };
        int padding = (length % 4);
        if (padding > 0)
        {
            padding = 4 - padding;
            outFile.Write(sizeof(char), padding, zero);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* VisualEffect::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    std::vector<VisualTechniquePtr>::iterator iter = mTechniques.begin();
    std::vector<VisualTechniquePtr>::iterator end = mTechniques.end();
    for (/**/; iter != end; ++iter)
    {
        WM5_GET_OBJECT_BY_NAME((*iter), name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void VisualEffect::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    std::vector<VisualTechniquePtr>::iterator iter = mTechniques.begin();
    std::vector<VisualTechniquePtr>::iterator end = mTechniques.end();
    for (/**/; iter != end; ++iter)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME((*iter), name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
VisualEffect::VisualEffect (LoadConstructor value)
    :
    Object(value)
{
}
//----------------------------------------------------------------------------
void VisualEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    int numTechniques;
    source.Read(numTechniques);
    mTechniques.resize(numTechniques);
    source.ReadPointerVV(numTechniques, &mTechniques[0]);

    WM5_END_DEBUG_STREAM_LOAD(VisualEffect, source);
}
//----------------------------------------------------------------------------
void VisualEffect::Link (InStream& source)
{
    Object::Link(source);

    std::vector<VisualTechniquePtr>::iterator iter = mTechniques.begin();
    std::vector<VisualTechniquePtr>::iterator end = mTechniques.end();
    for (/**/; iter != end; ++iter)
    {
        source.ResolveLink(*iter);
    }
}
//----------------------------------------------------------------------------
void VisualEffect::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool VisualEffect::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        std::vector<VisualTechniquePtr>::const_iterator iter =
            mTechniques.begin();
        std::vector<VisualTechniquePtr>::const_iterator end = mTechniques.end();
        for (/**/; iter != end; ++iter)
        {
            target.Register(*iter);
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void VisualEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WritePointerW((int)mTechniques.size(), &mTechniques[0]);

    WM5_END_DEBUG_STREAM_SAVE(VisualEffect, target);
}
//----------------------------------------------------------------------------
int VisualEffect::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    int numTechniques = (int)mTechniques.size();
    size += sizeof(numTechniques);
    size += numTechniques*WM5_POINTERSIZE(mTechniques[0]);
    return size;
}
//----------------------------------------------------------------------------
