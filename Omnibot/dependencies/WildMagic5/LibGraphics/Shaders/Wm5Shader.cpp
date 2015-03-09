// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Shader.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Shader);
WM5_IMPLEMENT_STREAM(Shader);
WM5_IMPLEMENT_ABSTRACT_FACTORY(Shader);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, Shader);

std::string Shader::msNullString = "";

//----------------------------------------------------------------------------
Shader::Shader ()
    :
    mNumInputs(0),
    mInputName(0),
    mInputType(0),
    mInputSemantic(0),
    mNumOutputs(0),
    mOutputName(0),
    mOutputType(0),
    mOutputSemantic(0),
    mNumConstants(0),
    mConstantName(0),
    mNumRegistersUsed(0),
    mNumSamplers(0),
    mSamplerName(0),
    mSamplerType(0),
    mFilter(0),
    mLodBias(0),
    mAnisotropy(0),
    mBorderColor(0),
    mProfileOwner(false)
{
    SetName("");

    mCoordinate[0] = 0;
    mCoordinate[1] = 0;
    mCoordinate[2] = 0;

    for (int i = 0; i < MAX_PROFILES; ++i)
    {
        mBaseRegister[i] = 0;
        mTextureUnit[i] = 0;
        mProgram[i] = 0;
    }
}
//----------------------------------------------------------------------------
Shader::Shader (const std::string& programName, int numInputs, int numOutputs,
    int numConstants, int numSamplers, bool profileOwner)
    :
    mNumInputs(numInputs),
    mNumOutputs(numOutputs),
    mNumConstants(numConstants),
    mNumSamplers(numSamplers),
    mProfileOwner(profileOwner)
{
    assertion(numOutputs > 0, "Shader must have at least one output.\n");
    int i, dim;

    SetName(programName);

    if (mNumInputs > 0)
    {
        mInputName = new1<std::string>(mNumInputs);
        mInputType = new1<VariableType>(mNumInputs);
        mInputSemantic = new1<VariableSemantic>(mNumInputs);
    }
    else
    {
        mInputName = 0;
        mInputType = 0;
        mInputSemantic = 0;
    }

    mOutputName = new1<std::string>(mNumOutputs);
    mOutputType = new1<VariableType>(mNumOutputs);
    mOutputSemantic = new1<VariableSemantic>(mNumOutputs);

    if (mNumConstants > 0)
    {
        mConstantName = new1<std::string>(mNumConstants);
        mNumRegistersUsed = new1<int>(mNumConstants);
        if (mProfileOwner)
        {
            for (i = 0; i < MAX_PROFILES; ++i)
            {
                mBaseRegister[i] = new1<int>(mNumConstants);
                memset(mBaseRegister[i], 0, mNumConstants*sizeof(int));
            }
        }
        else
        {
            for (i = 0; i < MAX_PROFILES; ++i)
            {
                mBaseRegister[i] = 0;
            }
        }
    }
    else
    {
        mConstantName = 0;
        mNumRegistersUsed = 0;
        for (i = 0; i < MAX_PROFILES; ++i)
        {
            mBaseRegister[i] = 0;
        }
    }

    if (mNumSamplers > 0)
    {
        mSamplerName = new1<std::string>(mNumSamplers);
        mSamplerType = new1<SamplerType>(mNumSamplers);

        mFilter = new1<SamplerFilter>(mNumSamplers);
        mCoordinate[0] = new1<SamplerCoordinate>(mNumSamplers);
        mCoordinate[1] = new1<SamplerCoordinate>(mNumSamplers);
        mCoordinate[2] = new1<SamplerCoordinate>(mNumSamplers);
        mLodBias = new1<float>(mNumSamplers);
        mAnisotropy = new1<float>(mNumSamplers);
        mBorderColor = new1<Float4>(mNumSamplers);
        for (i = 0; i < mNumSamplers; ++i)
        {
            mFilter[i] = SF_NEAREST;
            mCoordinate[0][i] = SC_CLAMP_EDGE;
            mCoordinate[1][i] = SC_CLAMP_EDGE;
            mCoordinate[2][i] = SC_CLAMP_EDGE;
            mLodBias[i] = 0.0f;
            mAnisotropy[i] = 1.0f;
            mBorderColor[i] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        if (mProfileOwner)
        {
            for (i = 0; i < MAX_PROFILES; ++i)
            {
                mTextureUnit[i] = new1<int>(mNumSamplers);
                memset(mTextureUnit[i], 0, mNumSamplers*sizeof(int));
            }
        }
        else
        {
            for (i = 0; i < MAX_PROFILES; ++i)
            {
                mTextureUnit[i] = 0;
            }
        }
    }
    else
    {
        mSamplerName = 0;
        mSamplerType = 0;
        mFilter = 0;
        for (dim = 0; dim < 3; ++dim)
        {
            mCoordinate[dim] = 0;
        }
        mLodBias = 0;
        mAnisotropy = 0;
        mBorderColor = 0;
        for (i = 0; i < MAX_PROFILES; ++i)
        {
            mTextureUnit[i] = 0;
        }
    }

    if (mProfileOwner)
    {
        for (i = 0; i < MAX_PROFILES; ++i)
        {
            mProgram[i] = new0 std::string();
        }
    }
    else
    {
        for (i = 0; i < MAX_PROFILES; ++i)
        {
            mProgram[i] = 0;
        }
    }
}
//----------------------------------------------------------------------------
Shader::~Shader ()
{
    delete1(mInputName);
    delete1(mInputType);
    delete1(mInputSemantic);
    delete1(mOutputName);
    delete1(mOutputType);
    delete1(mOutputSemantic);
    delete1(mConstantName);
    delete1(mNumRegistersUsed);
    delete1(mSamplerName);
    delete1(mSamplerType);
    delete1(mFilter);
    delete1(mCoordinate[0]);
    delete1(mCoordinate[1]);
    delete1(mCoordinate[2]);
    delete1(mLodBias);
    delete1(mAnisotropy);
    delete1(mBorderColor);

    if (mProfileOwner)
    {
        for (int i = 0; i < MAX_PROFILES; ++i)
        {
            delete1(mBaseRegister[i]);
            delete1(mTextureUnit[i]);
            delete0(mProgram[i]);
        }
    }
}
//----------------------------------------------------------------------------
void Shader::SetInput (int i, const std::string& name, VariableType type,
    VariableSemantic semantic)
{
    if (0 <= i && i < mNumInputs)
    {
        mInputName[i] = name;
        mInputType[i] = type;
        mInputSemantic[i] = semantic;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetOutput (int i, const std::string& name, VariableType type,
    VariableSemantic semantic)
{
    if (0 <= i && i < mNumOutputs)
    {
        mOutputName[i] = name;
        mOutputType[i] = type;
        mOutputSemantic[i] = semantic;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetConstant (int i, const std::string& name,
    int numRegistersUsed)
{
    if (0 <= i && i < mNumConstants)
    {
        mConstantName[i] = name;
        mNumRegistersUsed[i] = numRegistersUsed;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetSampler (int i, const std::string& name, SamplerType type)
{
    if (0 <= i && i < mNumSamplers)
    {
        mSamplerName[i] = name;
        mSamplerType[i] = type;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetFilter (int i, SamplerFilter filter)
{
    if (0 <= i && i < mNumSamplers)
    {
        mFilter[i] = filter;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetCoordinate (int i, int dim, SamplerCoordinate coordinate)
{
    if (0 <= i && i < mNumSamplers)
    {
        if (0 <= dim && dim < 3)
        {
            mCoordinate[dim][i] = coordinate;
            return;
        }
        assertion(false, "Invalid dimension.\n");
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetLodBias (int i, float lodBias)
{
    if (0 <= i && i < mNumSamplers)
    {
        mLodBias[i] = lodBias;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetAnisotropy (int i, float anisotropy)
{
    if (0 <= i && i < mNumSamplers)
    {
        mAnisotropy[i] = anisotropy;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetBorderColor (int i, const Float4& borderColor)
{
    if (0 <= i && i < mNumSamplers)
    {
        mBorderColor[i] = borderColor;
        return;
    }
    assertion(false, "Invalid index.\n");
}
//----------------------------------------------------------------------------
void Shader::SetBaseRegister (int profile, int i, int baseRegister)
{
    if (mProfileOwner)
    {
        if (0 <= profile && profile < MAX_PROFILES)
        {
            if (0 <= i && i < mNumConstants)
            {
                mBaseRegister[profile][i] = baseRegister;
                return;
            }
            assertion(false, "Invalid index.\n");
        }
        assertion(false, "Invalid profile.\n");
    }
    assertion(false, "You may not set profile data you do not own.\n");
}
//----------------------------------------------------------------------------
void Shader::SetTextureUnit (int profile, int i, int textureUnit)
{
    if (mProfileOwner)
    {
        if (0 <= profile && profile < MAX_PROFILES)
        {
            if (0 <= i && i < mNumSamplers)
            {
                mTextureUnit[profile][i] = textureUnit;
                return;
            }
            assertion(false, "Invalid index.\n");
        }
        assertion(false, "Invalid profile.\n");
    }
    assertion(false, "You may not set profile data you do not own.\n");
}
//----------------------------------------------------------------------------
void Shader::SetProgram (int profile, const std::string& program)
{
    if (mProfileOwner)
    {
        if (0 <= profile && profile < MAX_PROFILES)
        {
            *mProgram[profile] = program;
            return;
        }
        assertion(false, "Invalid profile.\n");
    }
    assertion(false, "You may not set profile data you do not own.\n");
}
//----------------------------------------------------------------------------
void Shader::SetBaseRegisters (int* baseRegisters[MAX_PROFILES])
{
    if (!mProfileOwner)
    {
        for (int i = 0; i < MAX_PROFILES; ++i)
        {
            mBaseRegister[i] = baseRegisters[i];
        }
        return;
    }
    assertion(false, "You already own the profile data.\n");
}
//----------------------------------------------------------------------------
void Shader::SetTextureUnits (int* textureUnits[MAX_PROFILES])
{
    if (!mProfileOwner)
    {
        for (int i = 0; i < MAX_PROFILES; ++i)
        {
            mTextureUnit[i] = textureUnits[i];
        }
        return;
    }
    assertion(false, "You already own the profile data.\n");
}
//----------------------------------------------------------------------------
void Shader::SetPrograms (std::string programs[MAX_PROFILES])
{
    if (!mProfileOwner)
    {
        for (int i = 0; i < MAX_PROFILES; ++i)
        {
            mProgram[i] = &programs[i];
        }
        return;
    }
    assertion(false, "You already own the profile data.\n");
}
//----------------------------------------------------------------------------
const std::string& Shader::GetInputName (int i) const
{
    if (0 <= i && i < mNumInputs)
    {
        return mInputName[i];
    }

    assertion(false, "Invalid index.\n");
    return msNullString;
}
//----------------------------------------------------------------------------
Shader::VariableType Shader::GetInputType (int i) const
{
    if (0 <= i && i < mNumInputs)
    {
        return mInputType[i];
    }

    assertion(false, "Invalid index.\n");
    return VT_NONE;
}
//----------------------------------------------------------------------------
Shader::VariableSemantic Shader::GetInputSemantic (int i) const
{
    if (0 <= i && i < mNumInputs)
    {
        return mInputSemantic[i];
    }

    assertion(false, "Invalid index.\n");
    return VS_NONE;
}
//----------------------------------------------------------------------------
const std::string& Shader::GetOutputName (int i) const
{
    if (0 <= i && i < mNumOutputs)
    {
        return mOutputName[i];
    }

    assertion(false, "Invalid index.\n");
    return msNullString;
}
//----------------------------------------------------------------------------
Shader::VariableType Shader::GetOutputType (int i) const
{
    if (0 <= i && i < mNumOutputs)
    {
        return mOutputType[i];
    }

    assertion(false, "Invalid index.\n");
    return VT_NONE;
}
//----------------------------------------------------------------------------
Shader::VariableSemantic Shader::GetOutputSemantic (int i) const
{
    if (0 <= i && i < mNumOutputs)
    {
        return mOutputSemantic[i];
    }

    assertion(false, "Invalid index.\n");
    return VS_NONE;
}
//----------------------------------------------------------------------------
const std::string& Shader::GetConstantName (int i) const
{
    if (0 <= i && i < mNumConstants)
    {
        return mConstantName[i];
    }

    assertion(false, "Invalid index.\n");
    return msNullString;
}
//----------------------------------------------------------------------------
int Shader::GetNumRegistersUsed (int i) const
{
    if (0 <= i && i < mNumConstants)
    {
        return mNumRegistersUsed[i];
    }

    assertion(false, "Invalid index.\n");
    return 0;
}
//----------------------------------------------------------------------------
const std::string& Shader::GetSamplerName (int i) const
{
    if (0 <= i && i < mNumSamplers)
    {
        return mSamplerName[i];
    }

    assertion(false, "Invalid index.\n");
    return msNullString;
}
//----------------------------------------------------------------------------
Shader::SamplerType Shader::GetSamplerType (int i) const
{
    if (0 <= i && i < mNumSamplers)
    {
        return mSamplerType[i];
    }

    assertion(false, "Invalid index.\n");
    return ST_NONE;
}
//----------------------------------------------------------------------------
Shader::SamplerFilter Shader::GetFilter (int i) const
{
    if (0 <= i && i < mNumSamplers)
    {
        return mFilter[i];
    }

    assertion(false, "Invalid index.\n");
    return SF_NONE;
}
//----------------------------------------------------------------------------
Shader::SamplerCoordinate Shader::GetCoordinate (int i, int dim) const
{
    if (0 <= i && i < mNumSamplers)
    {
        if (0 <= dim && dim < 3)
        {
            return mCoordinate[dim][i];
        }
        assertion(false, "Invalid dimension.\n");
        return SC_NONE;
    }

    assertion(false, "Invalid index.\n");
    return SC_NONE;
}
//----------------------------------------------------------------------------
float Shader::GetLodBias (int i) const
{
    if (0 <= i && i < mNumSamplers)
    {
        return mLodBias[i];
    }

    assertion(false, "Invalid index.\n");
    return 0.0f;
}
//----------------------------------------------------------------------------
float Shader::GetAnisotropy (int i) const
{
    if (0 <= i && i < mNumSamplers)
    {
        return mAnisotropy[i];
    }

    assertion(false, "Invalid index.\n");
    return 1.0f;
}
//----------------------------------------------------------------------------
Float4 Shader::GetBorderColor (int i) const
{
    if (0 <= i && i < mNumSamplers)
    {
        return mBorderColor[i];
    }

    assertion(false, "Invalid index.\n");
    return Float4(0.0f, 0.0f, 0.0f, 0.0f);
}
//----------------------------------------------------------------------------
int Shader::GetBaseRegister (int profile, int i) const
{
    if (0 <= profile && profile < MAX_PROFILES)
    {
        if (0 <= i && i < mNumConstants)
        {
            return mBaseRegister[profile][i];
        }
        assertion(false, "Invalid index.\n");
        return 0;
    }

    assertion(false, "Invalid profile.\n");
    return 0;
}
//----------------------------------------------------------------------------
int Shader::GetTextureUnit (int profile, int i) const
{
    if (0 <= profile && profile < MAX_PROFILES)
    {
        if (0 <= i && i < mNumSamplers)
        {
            return mTextureUnit[profile][i];
        }
        assertion(false, "Invalid index.\n");
        return 0;
    }

    assertion(false, "Invalid profile.\n");
    return 0;
}
//----------------------------------------------------------------------------
const std::string* Shader::GetProgram (int profile) const
{
    if (0 <= profile && profile < MAX_PROFILES)
    {
        return mProgram[profile];
    }

    assertion(false, "Invalid profile.\n");
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Shader::Shader (LoadConstructor value)
    :
    Object(value),
    mNumInputs(0),
    mInputName(0),
    mInputType(0),
    mInputSemantic(0),
    mNumOutputs(0),
    mOutputName(0),
    mOutputType(0),
    mOutputSemantic(0),
    mNumConstants(0),
    mConstantName(0),
    mNumRegistersUsed(0),
    mNumSamplers(0),
    mSamplerName(0),
    mSamplerType(0),
    mFilter(0),
    mLodBias(0),
    mAnisotropy(0),
    mBorderColor(0),
    mProfileOwner(false)
{
    SetName("");

    mCoordinate[0] = 0;
    mCoordinate[1] = 0;
    mCoordinate[2] = 0;

    for (int i = 0; i < MAX_PROFILES; ++i)
    {
        mBaseRegister[i] = 0;
        mTextureUnit[i] = 0;
        mProgram[i] = 0;
    }
}
//----------------------------------------------------------------------------
void Shader::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadStringRR(mNumInputs, mInputName);
    source.ReadEnumVR(mNumInputs, mInputType);
    source.ReadEnumVR(mNumInputs, mInputSemantic);
    source.ReadStringRR(mNumOutputs, mOutputName);
    source.ReadEnumVR(mNumOutputs, mOutputType);
    source.ReadEnumVR(mNumOutputs, mOutputSemantic);
    source.ReadStringRR(mNumConstants, mConstantName);
    source.ReadVR(mNumConstants, mNumRegistersUsed);
    source.ReadStringRR(mNumSamplers, mSamplerName);
    source.ReadEnumVR(mNumSamplers, mSamplerType);
    source.ReadEnumVR(mNumSamplers, mFilter);
    source.ReadEnumVR(mNumSamplers, mCoordinate[0]);
    source.ReadEnumVR(mNumSamplers, mCoordinate[1]);
    source.ReadEnumVR(mNumSamplers, mCoordinate[2]);
    source.ReadVR(mNumSamplers, mLodBias);
    source.ReadVR(mNumSamplers, mAnisotropy);
    source.ReadAggregateVR(mNumSamplers, mBorderColor);

    // Test for modified MAX_PROFILES.
    int maxProfiles;
    source.Read(maxProfiles);

#ifdef WM5_ASSERT_ON_CHANGED_MAX_PROFILES
    assertion(maxProfiles == MAX_PROFILES,
        "You changed MAX_PROFILES and are loading an old data set.\n");
#endif

    source.ReadBool(mProfileOwner);
    if (mProfileOwner)
    {
        int i;
        for (i = 0; i < maxProfiles; ++i)
        {
            source.ReadVR(mNumConstants, mBaseRegister[i]);
            source.ReadVR(mNumSamplers, mTextureUnit[i]);
            mProgram[i] = new0 std::string();
            source.ReadString(*mProgram[i]);
        }
        for (i = maxProfiles; i < MAX_PROFILES; ++i)
        {
            mBaseRegister[i] = 0;
            mTextureUnit[i] = 0;
            mProgram[i] = 0;
        }
    }

    WM5_END_DEBUG_STREAM_LOAD(Shader, source);
}
//----------------------------------------------------------------------------
void Shader::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void Shader::PostLink ()
{
    Object::PostLink();

    // NOTE.  If this shader is not the profile owner, then the post-link
    // semantics of setting the base registers, texture units, and programs
    // from static global data is the responsibility of the class that defines
    // such shaders and provides such data.
}
//----------------------------------------------------------------------------
bool Shader::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void Shader::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteStringW(mNumInputs, mInputName);
    target.WriteEnumN(mNumInputs, mInputType);
    target.WriteEnumN(mNumInputs, mInputSemantic);
    target.WriteStringW(mNumOutputs, mOutputName);
    target.WriteEnumN(mNumOutputs, mOutputType);
    target.WriteEnumN(mNumOutputs, mOutputSemantic);
    target.WriteStringW(mNumConstants, mConstantName);
    target.WriteN(mNumConstants, mNumRegistersUsed);
    target.WriteStringW(mNumSamplers, mSamplerName);
    target.WriteEnumN(mNumSamplers, mSamplerType);
    target.WriteEnumN(mNumSamplers, mFilter);
    target.WriteEnumN(mNumSamplers, mCoordinate[0]);
    target.WriteEnumN(mNumSamplers, mCoordinate[1]);
    target.WriteEnumN(mNumSamplers, mCoordinate[2]);
    target.WriteN(mNumSamplers, mLodBias);
    target.WriteN(mNumSamplers, mAnisotropy);
    target.WriteAggregateN(mNumSamplers, mBorderColor);

    // On load, test this against MAX_PROFILES just in case a user increases
    // MAX_PROFILES and tries to load old data sets.
    target.Write((int)MAX_PROFILES);
    target.WriteBool(mProfileOwner);
    if (mProfileOwner)
    {
        for (int i = 0; i < MAX_PROFILES; ++i)
        {
            target.WriteN(mNumConstants, mBaseRegister[i]);
            target.WriteN(mNumSamplers, mTextureUnit[i]);
            target.WriteString(*mProgram[i]);
        }
    }

    WM5_END_DEBUG_STREAM_SAVE(Shader, target);
}
//----------------------------------------------------------------------------
int Shader::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    int i;

    size += sizeof(mNumInputs);
    for (i = 0; i < mNumInputs; ++i)
    {
        size += WM5_STRINGSIZE(mInputName[i]);
        size += WM5_ENUMSIZE(mInputType[i]);
        size += WM5_ENUMSIZE(mInputSemantic[i]);
    }

    size += sizeof(mNumOutputs);
    for (i = 0; i < mNumOutputs; ++i)
    {
        size += WM5_STRINGSIZE(mOutputName[i]);
        size += WM5_ENUMSIZE(mOutputType[i]);
        size += WM5_ENUMSIZE(mOutputSemantic[i]);
    }

    size += sizeof(mNumConstants);
    for (i = 0; i < mNumConstants; ++i)
    {
        size += WM5_STRINGSIZE(mConstantName[i]);
        size += sizeof(mNumRegistersUsed[i]);
    }

    size += sizeof(mNumSamplers);
    for (i = 0; i < mNumSamplers; ++i)
    {
        size += WM5_STRINGSIZE(mSamplerName[i]);
        size += WM5_ENUMSIZE(mSamplerType[i]);
        size += WM5_ENUMSIZE(mFilter[i]);
        size += WM5_ENUMSIZE(mCoordinate[0][i]);
        size += WM5_ENUMSIZE(mCoordinate[1][i]);
        size += WM5_ENUMSIZE(mCoordinate[2][i]);
        size += sizeof(mLodBias[i]);
        size += sizeof(mAnisotropy[i]);
        size += sizeof(mBorderColor[i]);
    }

    size += sizeof(int);  // MAX_PROFILES
    size += WM5_BOOLSIZE(mProfileOwner);
    if (mProfileOwner)
    {
        for (i = 0; i < MAX_PROFILES; ++i)
        {
            size += mNumConstants*sizeof(mBaseRegister[i]);
            size += mNumSamplers*sizeof(mTextureUnit[i]);
            size += WM5_STRINGSIZE(*mProgram[i]);
        }
    }

    return size;
}
//----------------------------------------------------------------------------
