// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ControlledObject.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, ControlledObject);
WM5_IMPLEMENT_STREAM(ControlledObject);
WM5_IMPLEMENT_ABSTRACT_FACTORY(ControlledObject);

//----------------------------------------------------------------------------
ControlledObject::ControlledObject ()
    :
    mNumControllers(0),
    mCapacity(0),
    mControllers(0)
{
}
//----------------------------------------------------------------------------
ControlledObject::~ControlledObject ()
{
    DetachAllControllers();
    delete1(mControllers);
}
//----------------------------------------------------------------------------
void ControlledObject::AttachController (Controller* controller)
{
    // By design, controllers may not be controlled.  This avoids arbitrarily
    // complex graphs of controllers.  TODO:  Consider allowing this?
    if (IsDerived(Controller::TYPE))
    {
        assertion(false, "Controllers may not be controlled\n");
        return;
    }

    // The controller must exist.
    if (!controller)
    {
        assertion(false, "Cannot attach a null controller\n");
        return;
    }

    // Test whether the controller is already in the array.
    int i;
    for (i = 0; i < mNumControllers; ++i)
    {
        if (controller == mControllers[i])
        {
            return;
        }
    }

    // Bind the controller to the object.
    controller->SetObject(this);

    // The controller is not in the current array, so add it.
    if (mNumControllers == mCapacity)
    {
        // The array is full, so increase its capacity.
        mCapacity += CO_GROW_BY;
        ControllerPtr* newControllers = new1<ControllerPtr>(mCapacity);
        for (i = 0; i < mNumControllers; ++i)
        {
            newControllers[i] = mControllers[i];
        }
        delete1(mControllers);
        mControllers = newControllers;
    }

    mControllers[mNumControllers++] = controller;
}
//----------------------------------------------------------------------------
void ControlledObject::DetachController (Controller* controller)
{
    for (int i = 0; i < mNumControllers; ++i)
    {
        if (controller == mControllers[i])
        {
            // Unbind the controller from the object.
            controller->SetObject(0);

            // Remove the controller from the array, keeping the array
            // compact.
            for (int j = i + 1; j < mNumControllers; ++j, ++i)
            {
                mControllers[i] = mControllers[j];
            }
            mControllers[--mNumControllers] = 0;
            return;
        }
    }
}
//----------------------------------------------------------------------------
void ControlledObject::DetachAllControllers ()
{
    for (int i = 0; i < mNumControllers; ++i)
    {
        // Unbind the controller from the object.
        mControllers[i]->SetObject(0);
        mControllers[i] = 0;
    }
    mNumControllers = 0;
}
//----------------------------------------------------------------------------
bool ControlledObject::UpdateControllers (double applicationTime)
{
    bool someoneUpdated = false;
    for (int i = 0; i < mNumControllers; ++i)
    {
        if (mControllers[i]->Update(applicationTime))
        {
            someoneUpdated = true;
        }
    }
    return someoneUpdated;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* ControlledObject::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    for (int i = 0; i < mNumControllers; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mControllers[i], name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void ControlledObject::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    for (int i = 0; i < mNumControllers; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mControllers[i], name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
ControlledObject::ControlledObject (LoadConstructor value)
    :
    Object(value),
    mNumControllers(0),
    mCapacity(0),
    mControllers(0)
{
}
//----------------------------------------------------------------------------
void ControlledObject::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadPointerRR(mNumControllers, mControllers);

    mCapacity = mNumControllers;

    WM5_END_DEBUG_STREAM_LOAD(ControlledObject, source);
}
//----------------------------------------------------------------------------
void ControlledObject::Link (InStream& source)
{
    Object::Link(source);

    source.ResolveLink(mNumControllers, mControllers);
}
//----------------------------------------------------------------------------
void ControlledObject::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool ControlledObject::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        target.Register(mNumControllers, mControllers);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void ControlledObject::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WritePointerW(mNumControllers, mControllers);

    WM5_END_DEBUG_STREAM_SAVE(ControlledObject, target);
}
//----------------------------------------------------------------------------
int ControlledObject::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mNumControllers);
    size += mNumControllers*WM5_POINTERSIZE(mControllers[0]);
    return size;
}
//----------------------------------------------------------------------------
