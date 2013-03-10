// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightAmbientConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, LightAmbientConstant);
WM5_IMPLEMENT_STREAM(LightAmbientConstant);
WM5_IMPLEMENT_FACTORY(LightAmbientConstant);

//----------------------------------------------------------------------------
LightAmbientConstant::LightAmbientConstant (Light* light)
    :
    ShaderFloat(1),
    mLight(light)
{
    EnableUpdater();
}
//----------------------------------------------------------------------------
LightAmbientConstant::~LightAmbientConstant ()
{
}
//----------------------------------------------------------------------------
Light* LightAmbientConstant::GetLight ()
{
    return mLight;
}
//----------------------------------------------------------------------------
void LightAmbientConstant::Update (const Visual*, const Camera*)
{
    const float* source = (const float*)mLight->Ambient;
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
Object* LightAmbientConstant::GetObjectByName (const std::string& name)
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
void LightAmbientConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mLight, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightAmbientConstant::LightAmbientConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void LightAmbientConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mLight);

    WM5_END_DEBUG_STREAM_LOAD(LightAmbientConstant, source);
}
//----------------------------------------------------------------------------
void LightAmbientConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mLight);
}
//----------------------------------------------------------------------------
void LightAmbientConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool LightAmbientConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mLight);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void LightAmbientConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mLight);

    WM5_END_DEBUG_STREAM_SAVE(LightAmbientConstant, target);
}
//----------------------------------------------------------------------------
int LightAmbientConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mLight);
    return size;
}
//----------------------------------------------------------------------------
