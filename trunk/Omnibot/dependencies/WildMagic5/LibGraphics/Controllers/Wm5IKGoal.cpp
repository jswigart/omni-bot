// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5IKGoal.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, IKGoal);
WM5_IMPLEMENT_STREAM(IKGoal);
WM5_IMPLEMENT_FACTORY(IKGoal);

//----------------------------------------------------------------------------
IKGoal::IKGoal (Spatial* target, Spatial* effector, float weight)
    :
    Weight(weight),
    mTarget(target),
    mEffector(effector)
{
}
//----------------------------------------------------------------------------
IKGoal::~IKGoal ()
{
}
//----------------------------------------------------------------------------
APoint IKGoal::GetTargetPosition () const
{
    return mTarget->WorldTransform.GetTranslate();
}
//----------------------------------------------------------------------------
APoint IKGoal::GetEffectorPosition () const
{
    return mEffector->WorldTransform.GetTranslate();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* IKGoal::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mTarget, name, found);
    WM5_GET_OBJECT_BY_NAME(mEffector, name, found);

    return 0;
}
//----------------------------------------------------------------------------
void IKGoal::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mTarget, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mEffector, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
IKGoal::IKGoal (LoadConstructor value)
    :
    Object(value),
    Weight(0.0f)
{
}
//----------------------------------------------------------------------------
void IKGoal::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.Read(Weight);
    source.ReadPointer(mTarget);
    source.ReadPointer(mEffector);

    WM5_END_DEBUG_STREAM_LOAD(IKGoal, source);
}
//----------------------------------------------------------------------------
void IKGoal::Link (InStream& source)
{
    Object::Link(source);

    source.ResolveLink(mTarget);
    source.ResolveLink(mEffector);
}
//----------------------------------------------------------------------------
void IKGoal::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool IKGoal::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        target.Register(mTarget);
        target.Register(mEffector);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void IKGoal::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.Write(Weight);
    target.WritePointer(mTarget);
    target.WritePointer(mEffector);

    WM5_END_DEBUG_STREAM_SAVE(IKGoal, target);
}
//----------------------------------------------------------------------------
int IKGoal::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(Weight);
    size += WM5_POINTERSIZE(mTarget);
    size += WM5_POINTERSIZE(mEffector);
    return size;
}
//----------------------------------------------------------------------------
