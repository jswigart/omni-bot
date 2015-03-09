// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Spatial.h"
#include "Wm5Culler.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ControlledObject, Spatial);
WM5_IMPLEMENT_STREAM(Spatial);
WM5_IMPLEMENT_ABSTRACT_FACTORY(Spatial);

//----------------------------------------------------------------------------
Spatial::Spatial ()
    :
    WorldTransformIsCurrent(false),
    WorldBoundIsCurrent(false),
    Culling(CULL_DYNAMIC),
    mParent(0)
{
}
//----------------------------------------------------------------------------
Spatial::~Spatial ()
{
    // The mParent member is not reference counted by Spatial, so do not
    // release it here.
}
//----------------------------------------------------------------------------
void Spatial::Update (double applicationTime, bool initiator)
{
    UpdateWorldData(applicationTime);
    UpdateWorldBound();
    if (initiator)
    {
        PropagateBoundToRoot();
    }
}
//----------------------------------------------------------------------------
void Spatial::UpdateWorldData (double applicationTime)
{
    // Update any controllers associated with this object.
    UpdateControllers(applicationTime);

    // Update world transforms.
    if (!WorldTransformIsCurrent)
    {
        if (mParent)
        {
            WorldTransform = mParent->WorldTransform*LocalTransform;
        }
        else
        {
            WorldTransform = LocalTransform;
        }
    }
}
//----------------------------------------------------------------------------
void Spatial::PropagateBoundToRoot ()
{
    if (mParent)
    {
        mParent->UpdateWorldBound();
        mParent->PropagateBoundToRoot();
    }
}
//----------------------------------------------------------------------------
void Spatial::OnGetVisibleSet (Culler& culler, bool noCull)
{
    if (Culling == CULL_ALWAYS)
    {
        return;
    }

    if (Culling == CULL_NEVER)
    {
        noCull = true;
    }

    unsigned int savePlaneState = culler.GetPlaneState();
    if (noCull || culler.IsVisible(WorldBound))
    {
        GetVisibleSet(culler, noCull);
    }
    culler.SetPlaneState(savePlaneState);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* Spatial::GetObjectByName (const std::string& name)
{
    // mParent is not searched to avoid a cycle in the Object graph.
    return ControlledObject::GetObjectByName(name);
}
//----------------------------------------------------------------------------
void Spatial::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    // mParent is not searched to avoid a cycle in the Object graph.
    ControlledObject::GetAllObjectsByName(name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Spatial::Spatial (LoadConstructor value)
    :
    ControlledObject(value),
    WorldTransformIsCurrent(false),
    WorldBoundIsCurrent(false),
    Culling(CULL_DYNAMIC),
    mParent(0)
{
}
//----------------------------------------------------------------------------
void Spatial::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ControlledObject::Load(source);

    source.ReadAggregate(LocalTransform);
    source.ReadAggregate(WorldTransform);
    source.ReadBool(WorldTransformIsCurrent);
    source.ReadAggregate(WorldBound);
    source.ReadBool(WorldBoundIsCurrent);
    source.ReadEnum(Culling);

    // mParent was not saved.  It will be set in Node::Link when the child
    // pointers of the Node are resolved by Node::SetChild.

    WM5_END_DEBUG_STREAM_LOAD(Spatial, source);
}
//----------------------------------------------------------------------------
void Spatial::Link (InStream& source)
{
    // mParent was not saved.  It will be set in Node::Link when the child
    // pointers of the Node are resolved by Node::SetChild.

    ControlledObject::Link(source);
}
//----------------------------------------------------------------------------
void Spatial::PostLink ()
{
    ControlledObject::PostLink();
}
//----------------------------------------------------------------------------
bool Spatial::Register (OutStream& target) const
{
    // mParent need not be registered since the parent itself must have
    // initiated the Register call to its children, 'this' being one of them.

    return ControlledObject::Register(target);
}
//----------------------------------------------------------------------------
void Spatial::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ControlledObject::Save(target);

    target.WriteAggregate(LocalTransform);
    target.WriteAggregate(WorldTransform);
    target.WriteBool(WorldTransformIsCurrent);
    target.WriteAggregate(WorldBound);
    target.WriteBool(WorldBoundIsCurrent);
    target.WriteEnum(Culling);

    // mParent is not saved.  On load, it will be set in Node::Link when the
    // child pointers of the Node are resolved by Node::SetChild.

    WM5_END_DEBUG_STREAM_SAVE(Spatial, target);
}
//----------------------------------------------------------------------------
int Spatial::GetStreamingSize () const
{
    int size = ControlledObject::GetStreamingSize();
    size += LocalTransform.GetStreamingSize();;
    size += WorldTransform.GetStreamingSize();
    size += WM5_BOOLSIZE(WorldTransformIsCurrent);
    size += WorldBound.GetStreamingSize();
    size += WM5_BOOLSIZE(WorldBoundIsCurrent);
    size += WM5_ENUMSIZE(Culling);

    // mParent is not streamed.

    return size;
}
//----------------------------------------------------------------------------
