// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

#include "Wm5GraphicsPCH.h"
#include "Wm5Controller.h"
#include "Wm5ControlledObject.h"
#include "Wm5Math.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Controller);
WM5_IMPLEMENT_STREAM(Controller);
WM5_IMPLEMENT_ABSTRACT_FACTORY(Controller);

//----------------------------------------------------------------------------
Controller::Controller ()
    :
    Repeat(RT_CLAMP),
    MinTime(0.0),
    MaxTime(0.0),
    Phase(0.0),
    Frequency(1.0),
    Active(true),
    mObject(0),
    mApplicationTime(-Mathd::MAX_REAL)
{
}
//----------------------------------------------------------------------------
Controller::~Controller ()
{
}
//----------------------------------------------------------------------------
bool Controller::Update (double applicationTime)
{
    if (Active)
    {
        mApplicationTime = applicationTime;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Controller::SetObject (ControlledObject* object)
{
    mObject = object;
}
//----------------------------------------------------------------------------
double Controller::GetControlTime (double applicationTime)
{
    double controlTime = Frequency*applicationTime + Phase;

    if (Repeat == RT_CLAMP)
    {
        // Clamp the time to the [min,max] interval.
        if (controlTime < MinTime)
        {
            return MinTime;
        }
        if (controlTime > MaxTime)
        {
            return MaxTime;
        }
        return controlTime;
    }

    double timeRange = MaxTime - MinTime;
    if (timeRange > 0.0)
    {
        double multiples = (controlTime - MinTime)/timeRange;
        double integerTime = Mathd::Floor(multiples);
        double fractionTime = multiples - integerTime;
        if (Repeat == RT_WRAP)
        {
            return MinTime + fractionTime*timeRange;
        }

        // Repeat == WM5_RT_CYCLE
        if (((int)integerTime) & 1)
        {
            // Go backward in time.
            return MaxTime - fractionTime*timeRange;
        }
        else
        {
            // Go forward in time.
            return MinTime + fractionTime*timeRange;
        }
    }

    // The minimum and maximum times are the same, so return the minimum.
    return MinTime;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* Controller::GetObjectByName (const std::string& name)
{
    // mObject is not searched to avoid a cycle in the Object graph.
    return Object::GetObjectByName(name);
}
//----------------------------------------------------------------------------
void Controller::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    // mObject is not searched to avoid a cycle in the Object graph.
    Object::GetAllObjectsByName(name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Controller::Controller (LoadConstructor value)
    :
    Object(value),
    Repeat(RT_CLAMP),
    MinTime(0.0),
    MaxTime(0.0),
    Phase(0.0),
    Frequency(1.0),
    Active(true),
    mObject(0),
    mApplicationTime(-Mathd::MAX_REAL)
{
}
//----------------------------------------------------------------------------
void Controller::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadEnum(Repeat);
    source.Read(MinTime);
    source.Read(MaxTime);
    source.Read(Phase);
    source.Read(Frequency);
    source.ReadBool(Active);
    source.ReadPointer(mObject);

    mApplicationTime = -Mathd::MAX_REAL;

    WM5_END_DEBUG_STREAM_LOAD(Controller, source);
}
//----------------------------------------------------------------------------
void Controller::Link (InStream& source)
{
    Object::Link(source);

    source.ResolveLink(mObject);
}
//----------------------------------------------------------------------------
void Controller::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool Controller::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        target.Register(mObject);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Controller::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteEnum(Repeat);
    target.Write(MinTime);
    target.Write(MaxTime);
    target.Write(Phase);
    target.Write(Frequency);
    target.WriteBool(Active);
    target.WritePointer(mObject);

    WM5_END_DEBUG_STREAM_SAVE(Controller, target);
}
//----------------------------------------------------------------------------
int Controller::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_ENUMSIZE(Repeat);
    size += sizeof(MinTime);
    size += sizeof(MaxTime);
    size += sizeof(Phase);
    size += sizeof(Frequency);
    size += WM5_BOOLSIZE(Active);
    size += WM5_POINTERSIZE(mObject);
    return size;
}
//----------------------------------------------------------------------------
