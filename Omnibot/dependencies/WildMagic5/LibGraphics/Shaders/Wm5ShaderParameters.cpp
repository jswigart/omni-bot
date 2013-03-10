// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ShaderParameters.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, ShaderParameters);
WM5_IMPLEMENT_STREAM(ShaderParameters);
WM5_IMPLEMENT_FACTORY(ShaderParameters);

//----------------------------------------------------------------------------
ShaderParameters::ShaderParameters (Shader* shader)
    :
    mShader(shader)
{
    assertion(shader != 0, "Shader must be specified.\n");

    mNumConstants = mShader->GetNumConstants();
    if (mNumConstants > 0)
    {
        mConstants = new1<ShaderFloatPtr>(mNumConstants);
    }
    else
    {
        mConstants = 0;
    }

    mNumTextures = mShader->GetNumSamplers();
    if (mNumTextures > 0)
    {
        mTextures = new1<TexturePtr>(mNumTextures);
    }
    else
    {
        mTextures = 0;
    }
}
//----------------------------------------------------------------------------
ShaderParameters::~ShaderParameters ()
{
    delete1(mConstants);
    delete1(mTextures);
}
//----------------------------------------------------------------------------
int ShaderParameters::SetConstant (const std::string& name,
    ShaderFloat* sfloat)
{
    for (int i = 0; i < mNumConstants; ++i)
    {
        if (mShader->GetConstantName(i) == name)
        {
            mConstants[i] = sfloat;
            return i;
        }
    }

    assertion(false, "Cannot find constant.\n");
    return -1;
}
//----------------------------------------------------------------------------
int ShaderParameters::SetTexture (const std::string& name, Texture* texture)
{
    for (int i = 0; i < mNumTextures; ++i)
    {
        if (mShader->GetSamplerName(i) == name)
        {
            mTextures[i] = texture;
            return i;
        }
    }

    assertion(false, "Cannot find texture.\n");
    return -1;
}
//----------------------------------------------------------------------------
void ShaderParameters::SetConstant (int handle, ShaderFloat* sfloat)
{
    if (0 <= handle && handle < mNumConstants)
    {
        mConstants[handle] = sfloat;
        return;
    }

    assertion(false, "Invalid constant handle.\n");
}
//----------------------------------------------------------------------------
void ShaderParameters::SetTexture (int handle, Texture* texture)
{
    if (0 <= handle && handle < mNumTextures)
    {
        mTextures[handle] = texture;
        return;
    }

    assertion(false, "Invalid texture handle.\n");
}
//----------------------------------------------------------------------------
ShaderFloat* ShaderParameters::GetConstant (const std::string& name) const
{
    for (int i = 0; i < mNumConstants; ++i)
    {
        if (mShader->GetConstantName(i) == name)
        {
            return mConstants[i];
        }
    }

    assertion(false, "Cannot find constant.\n");
    return 0;
}
//----------------------------------------------------------------------------
Texture* ShaderParameters::GetTexture (const std::string& name) const
{
    for (int i = 0; i < mNumTextures; ++i)
    {
        if (mShader->GetSamplerName(i) == name)
        {
            return mTextures[i];
        }
    }

    assertion(false, "Cannot find texture.\n");
    return 0;
}
//----------------------------------------------------------------------------
ShaderFloat* ShaderParameters::GetConstant (int handle) const
{
    if (0 <= handle && handle < mNumConstants)
    {
        return mConstants[handle];
    }

    assertion(false, "Invalid constant handle.\n");
    return 0;
}
//----------------------------------------------------------------------------
Texture* ShaderParameters::GetTexture (int handle) const
{
    if (0 <= handle && handle < mNumTextures)
    {
        return mTextures[handle];
    }

    assertion(false, "Invalid texture handle.\n");
    return 0;
}
//----------------------------------------------------------------------------
void ShaderParameters::UpdateConstants (const Visual* visual,
    const Camera* camera)
{
    ShaderFloatPtr* constants = mConstants;
    for (int i = 0; i < mNumConstants; ++i, ++constants)
    {
        ShaderFloat* constant = *constants;
        if (constant->AllowUpdater())
        {
            constant->Update(visual, camera);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* ShaderParameters::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mShader, name, found);

    int i;
    for (i = 0; i < mNumConstants; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mConstants[i], name, found);
    }

    for (i = 0; i < mNumTextures; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mTextures[i], name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void ShaderParameters::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mShader, name, objects);

    int i;
    for (i = 0; i < mNumConstants; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mConstants[i], name, objects);
    }

    for (i = 0; i < mNumTextures; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mTextures[i], name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
ShaderParameters::ShaderParameters (LoadConstructor value)
    :
    Object(value),
    mNumConstants(0),
    mConstants(0),
    mNumTextures(0),
    mTextures(0)
{
}
//----------------------------------------------------------------------------
void ShaderParameters::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadPointer(mShader);
    source.ReadPointerRR(mNumConstants, mConstants);
    source.ReadPointerRR(mNumTextures, mTextures);

    WM5_END_DEBUG_STREAM_LOAD(ShaderParameters, source);
}
//----------------------------------------------------------------------------
void ShaderParameters::Link (InStream& source)
{
    Object::Link(source);

    source.ResolveLink(mShader);
    source.ResolveLink(mNumConstants, mConstants);
    source.ResolveLink(mNumTextures, mTextures);
}
//----------------------------------------------------------------------------
void ShaderParameters::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool ShaderParameters::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        target.Register(mShader);
        target.Register(mNumConstants, mConstants);
        target.Register(mNumTextures, mTextures);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void ShaderParameters::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WritePointer(mShader);
    target.WritePointerW(mNumConstants, mConstants);
    target.WritePointerW(mNumTextures, mTextures);

    WM5_END_DEBUG_STREAM_SAVE(ShaderParameters, target);
}
//----------------------------------------------------------------------------
int ShaderParameters::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_POINTERSIZE(mShader);
    size += sizeof(mNumConstants);
    size += mNumConstants*WM5_POINTERSIZE(mConstants[0]);
    size += sizeof(mNumTextures);
    size += mNumTextures*WM5_POINTERSIZE(mTextures[0]);
    return size;
}
//----------------------------------------------------------------------------
