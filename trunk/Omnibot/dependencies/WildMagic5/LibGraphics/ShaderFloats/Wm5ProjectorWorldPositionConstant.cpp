// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ProjectorWorldPositionConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, ProjectorWorldPositionConstant);
WM5_IMPLEMENT_STREAM(ProjectorWorldPositionConstant);
WM5_IMPLEMENT_FACTORY(ProjectorWorldPositionConstant);

//----------------------------------------------------------------------------
ProjectorWorldPositionConstant::ProjectorWorldPositionConstant (
    Projector* projector)
    :
    ShaderFloat(1),
    mProjector(projector)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
ProjectorWorldPositionConstant::~ProjectorWorldPositionConstant ()
{
}
//----------------------------------------------------------------------------
Projector* ProjectorWorldPositionConstant::GetProjector ()
{
    return mProjector;
}
//----------------------------------------------------------------------------
void ProjectorWorldPositionConstant::Update (const Visual*, const Camera*)
{
    const APoint& worldPosition = mProjector->GetPosition();

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
Object* ProjectorWorldPositionConstant::GetObjectByName (
    const std::string& name)
{
    Object* found = ShaderFloat::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mProjector, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void ProjectorWorldPositionConstant::GetAllObjectsByName (
    const std::string& name, std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mProjector, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
ProjectorWorldPositionConstant::ProjectorWorldPositionConstant (
    LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void ProjectorWorldPositionConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mProjector);

    WM5_END_DEBUG_STREAM_LOAD(ProjectorWorldPositionConstant, source);
}
//----------------------------------------------------------------------------
void ProjectorWorldPositionConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mProjector);
}
//----------------------------------------------------------------------------
void ProjectorWorldPositionConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool ProjectorWorldPositionConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mProjector);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void ProjectorWorldPositionConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mProjector);

    WM5_END_DEBUG_STREAM_SAVE(ProjectorWorldPositionConstant, target);
}
//----------------------------------------------------------------------------
int ProjectorWorldPositionConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mProjector);
    return size;
}
//----------------------------------------------------------------------------
