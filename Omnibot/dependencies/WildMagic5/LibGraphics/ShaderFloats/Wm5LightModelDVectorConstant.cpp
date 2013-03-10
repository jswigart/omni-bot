// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightModelDVectorConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, LightModelDVectorConstant);
WM5_IMPLEMENT_STREAM(LightModelDVectorConstant);
WM5_IMPLEMENT_FACTORY(LightModelDVectorConstant);

//----------------------------------------------------------------------------
LightModelDVectorConstant::LightModelDVectorConstant (Light* light)
    :
    ShaderFloat(1),
    mLight(light)
{
    EnableUpdater();
}
//----------------------------------------------------------------------------
LightModelDVectorConstant::~LightModelDVectorConstant ()
{
}
//----------------------------------------------------------------------------
void LightModelDVectorConstant::Update (const Visual* visual, const Camera*)
{
    const HMatrix& worldInvMatrix = visual->WorldTransform.Inverse();
    AVector modelDVector = worldInvMatrix*mLight->DVector;

    const float* source = (const float*)modelDVector;
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
Object* LightModelDVectorConstant::GetObjectByName (const std::string& name)
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
void LightModelDVectorConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mLight, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightModelDVectorConstant::LightModelDVectorConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void LightModelDVectorConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mLight);

    WM5_END_DEBUG_STREAM_LOAD(LightModelDVectorConstant, source);
}
//----------------------------------------------------------------------------
void LightModelDVectorConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mLight);
}
//----------------------------------------------------------------------------
void LightModelDVectorConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool LightModelDVectorConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mLight);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void LightModelDVectorConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mLight);

    WM5_END_DEBUG_STREAM_SAVE(LightModelDVectorConstant, target);
}
//----------------------------------------------------------------------------
int LightModelDVectorConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mLight);
    return size;
}
//----------------------------------------------------------------------------
