// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightWorldPositionConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, LightWorldPositionConstant);
WM5_IMPLEMENT_STREAM(LightWorldPositionConstant);
WM5_IMPLEMENT_FACTORY(LightWorldPositionConstant);

//----------------------------------------------------------------------------
LightWorldPositionConstant::LightWorldPositionConstant (Light* light)
    :
    ShaderFloat(1),
    mLight(light)
{
    EnableUpdater();
}
//----------------------------------------------------------------------------
LightWorldPositionConstant::~LightWorldPositionConstant ()
{
}
//----------------------------------------------------------------------------
Light* LightWorldPositionConstant::GetLight ()
{
    return mLight;
}
//----------------------------------------------------------------------------
void LightWorldPositionConstant::Update (const Visual*, const Camera*)
{
    const APoint& worldPosition = mLight->Position;

    const float* source = (const float*)worldPosition;
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
Object* LightWorldPositionConstant::GetObjectByName (const std::string& name)
{
    return ShaderFloat::GetObjectByName(name);
}
//----------------------------------------------------------------------------
void LightWorldPositionConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightWorldPositionConstant::LightWorldPositionConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void LightWorldPositionConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mLight);

    WM5_END_DEBUG_STREAM_LOAD(LightWorldPositionConstant, source);
}
//----------------------------------------------------------------------------
void LightWorldPositionConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mLight);
}
//----------------------------------------------------------------------------
void LightWorldPositionConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool LightWorldPositionConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mLight);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void LightWorldPositionConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mLight);

    WM5_END_DEBUG_STREAM_SAVE(LightWorldPositionConstant, target);
}
//----------------------------------------------------------------------------
int LightWorldPositionConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mLight);
    return size;
}
//----------------------------------------------------------------------------
