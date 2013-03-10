// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightSpotConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, LightSpotConstant);
WM5_IMPLEMENT_STREAM(LightSpotConstant);
WM5_IMPLEMENT_FACTORY(LightSpotConstant);

//----------------------------------------------------------------------------
LightSpotConstant::LightSpotConstant (Light* light)
    :
    ShaderFloat(1),
    mLight(light)
{
    EnableUpdater();
}
//----------------------------------------------------------------------------
LightSpotConstant::~LightSpotConstant ()
{
}
//----------------------------------------------------------------------------
Light* LightSpotConstant::GetLight ()
{
    return mLight;
}
//----------------------------------------------------------------------------
void LightSpotConstant::Update (const Visual*, const Camera*)
{
    mData[0] = mLight->Angle;
    mData[1] = mLight->CosAngle;
    mData[2] = mLight->SinAngle;
    mData[3] = mLight->Exponent;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* LightSpotConstant::GetObjectByName (const std::string& name)
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
void LightSpotConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mLight, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightSpotConstant::LightSpotConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void LightSpotConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mLight);

    WM5_END_DEBUG_STREAM_LOAD(LightSpotConstant, source);
}
//----------------------------------------------------------------------------
void LightSpotConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mLight);
}
//----------------------------------------------------------------------------
void LightSpotConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool LightSpotConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mLight);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void LightSpotConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mLight);

    WM5_END_DEBUG_STREAM_SAVE(LightSpotConstant, target);
}
//----------------------------------------------------------------------------
int LightSpotConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mLight);
    return size;
}
//----------------------------------------------------------------------------
