// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightAttenuationConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, LightAttenuationConstant);
WM5_IMPLEMENT_STREAM(LightAttenuationConstant);
WM5_IMPLEMENT_FACTORY(LightAttenuationConstant);

//----------------------------------------------------------------------------
LightAttenuationConstant::LightAttenuationConstant (Light* light)
    :
    ShaderFloat(1),
    mLight(light)
{
    EnableUpdater();
}
//----------------------------------------------------------------------------
LightAttenuationConstant::~LightAttenuationConstant ()
{
}
//----------------------------------------------------------------------------
Light* LightAttenuationConstant::GetLight ()
{
    return mLight;
}
//----------------------------------------------------------------------------
void LightAttenuationConstant::Update (const Visual*, const Camera*)
{
    mData[0] = mLight->Constant;
    mData[1] = mLight->Linear;
    mData[2] = mLight->Quadratic;
    mData[3] = mLight->Intensity;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* LightAttenuationConstant::GetObjectByName (const std::string& name)
{
    Object* found = ShaderFloat::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mLight, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void LightAttenuationConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mLight, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightAttenuationConstant::LightAttenuationConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void LightAttenuationConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mLight);

    WM5_END_DEBUG_STREAM_LOAD(LightAttenuationConstant, source);
}
//----------------------------------------------------------------------------
void LightAttenuationConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mLight);
}
//----------------------------------------------------------------------------
void LightAttenuationConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool LightAttenuationConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mLight);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void LightAttenuationConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mLight);

    WM5_END_DEBUG_STREAM_SAVE(LightAttenuationConstant, target);
}
//----------------------------------------------------------------------------
int LightAttenuationConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mLight);
    return size;
}
//----------------------------------------------------------------------------
