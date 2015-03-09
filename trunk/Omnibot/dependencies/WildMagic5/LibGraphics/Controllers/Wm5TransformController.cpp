// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#include "Wm5GraphicsPCH.h"
#include "Wm5TransformController.h"
#include "Wm5Spatial.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Controller, TransformController);
WM5_IMPLEMENT_STREAM(TransformController);
WM5_IMPLEMENT_FACTORY(TransformController);
WM5_IMPLEMENT_DEFAULT_NAMES(Controller, TransformController);

//----------------------------------------------------------------------------
TransformController::TransformController (const Transform& localTransform)
    :
    mLocalTransform(localTransform)
{
}
//----------------------------------------------------------------------------
TransformController::~TransformController ()
{
}
//----------------------------------------------------------------------------
bool TransformController::Update (double applicationTime)
{
    if (!Controller::Update(applicationTime))
    {
        return false;
    }

    Spatial* spatial = StaticCast<Spatial>(mObject);
    spatial->LocalTransform = mLocalTransform;
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
TransformController::TransformController (LoadConstructor value)
    :
    Controller(value)
{
}
//----------------------------------------------------------------------------
void TransformController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Controller::Load(source);

    source.ReadAggregate(mLocalTransform);

    WM5_END_DEBUG_STREAM_LOAD(TransformController, source);
}
//----------------------------------------------------------------------------
void TransformController::Link (InStream& source)
{
    Controller::Link(source);
}
//----------------------------------------------------------------------------
void TransformController::PostLink ()
{
    Controller::PostLink();
}
//----------------------------------------------------------------------------
bool TransformController::Register (OutStream& target) const
{
    return Controller::Register(target);
}
//----------------------------------------------------------------------------
void TransformController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Controller::Save(target);

    target.WriteAggregate(mLocalTransform);

    WM5_END_DEBUG_STREAM_SAVE(TransformController, target);
}
//----------------------------------------------------------------------------
int TransformController::GetStreamingSize () const
{
    int size = Controller::GetStreamingSize();
    size += mLocalTransform.GetStreamingSize();
    return size;
}
//----------------------------------------------------------------------------
