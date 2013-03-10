// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightModelPositionConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, LightModelPositionConstant);
WM5_IMPLEMENT_STREAM(LightModelPositionConstant);
WM5_IMPLEMENT_FACTORY(LightModelPositionConstant);

//----------------------------------------------------------------------------
LightModelPositionConstant::LightModelPositionConstant (Light* light)
    :
    ShaderFloat(1),
    mLight(light)
{
    EnableUpdater();
}
//----------------------------------------------------------------------------
LightModelPositionConstant::~LightModelPositionConstant ()
{
}
//----------------------------------------------------------------------------
Light* LightModelPositionConstant::GetLight ()
{
    return mLight;
}
//----------------------------------------------------------------------------
void LightModelPositionConstant::Update (const Visual* visual,
    const Camera*)
{
    const APoint& worldPosition = mLight->Position;
    const HMatrix& worldInvMatrix = visual->WorldTransform.Inverse();
    APoint modelPosition = worldInvMatrix*worldPosition;

    const float* source = (const float*)modelPosition;
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
Object* LightModelPositionConstant::GetObjectByName (const std::string& name)
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
void LightModelPositionConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mLight, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightModelPositionConstant::LightModelPositionConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void LightModelPositionConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mLight);

    WM5_END_DEBUG_STREAM_LOAD(LightModelPositionConstant, source);
}
//----------------------------------------------------------------------------
void LightModelPositionConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mLight);
}
//----------------------------------------------------------------------------
void LightModelPositionConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool LightModelPositionConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mLight);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void LightModelPositionConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mLight);

    WM5_END_DEBUG_STREAM_SAVE(LightModelPositionConstant, target);
}
//----------------------------------------------------------------------------
int LightModelPositionConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mLight);
    return size;
}
//----------------------------------------------------------------------------
