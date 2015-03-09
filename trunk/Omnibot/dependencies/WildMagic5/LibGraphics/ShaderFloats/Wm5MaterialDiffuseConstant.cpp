// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5MaterialDiffuseConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, MaterialDiffuseConstant);
WM5_IMPLEMENT_STREAM(MaterialDiffuseConstant);
WM5_IMPLEMENT_FACTORY(MaterialDiffuseConstant);

//----------------------------------------------------------------------------
MaterialDiffuseConstant::MaterialDiffuseConstant (Material* material)
    :
    ShaderFloat(1),
    mMaterial(material)
{
    EnableUpdater();
}
//----------------------------------------------------------------------------
MaterialDiffuseConstant::~MaterialDiffuseConstant ()
{
}
//----------------------------------------------------------------------------
Material* MaterialDiffuseConstant::GetMaterial ()
{
    return mMaterial;
}
//----------------------------------------------------------------------------
void MaterialDiffuseConstant::Update (const Visual*, const Camera*)
{
    const float* source = (const float*)mMaterial->Diffuse;
    float* target = mData;
    for (int i = 0; i < 4; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* MaterialDiffuseConstant::GetObjectByName (const std::string& name)
{
    Object* found = ShaderFloat::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mMaterial, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void MaterialDiffuseConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mMaterial, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
MaterialDiffuseConstant::MaterialDiffuseConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void MaterialDiffuseConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mMaterial);

    WM5_END_DEBUG_STREAM_LOAD(MaterialDiffuseConstant, source);
}
//----------------------------------------------------------------------------
void MaterialDiffuseConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mMaterial);
}
//----------------------------------------------------------------------------
void MaterialDiffuseConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool MaterialDiffuseConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mMaterial);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void MaterialDiffuseConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mMaterial);

    WM5_END_DEBUG_STREAM_SAVE(MaterialDiffuseConstant, target);
}
//----------------------------------------------------------------------------
int MaterialDiffuseConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mMaterial);
    return size;
}
//----------------------------------------------------------------------------
