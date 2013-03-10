// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5PointController.h"
#include "Wm5Polypoint.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Controller, PointController);
WM5_IMPLEMENT_STREAM(PointController);
WM5_IMPLEMENT_ABSTRACT_FACTORY(PointController);
WM5_IMPLEMENT_DEFAULT_NAMES(Controller, PointController);

//----------------------------------------------------------------------------
PointController::PointController ()
    :
    SystemLinearSpeed(0.0f),
    SystemAngularSpeed(0.0f),
    SystemLinearAxis(AVector::UNIT_Z),
    SystemAngularAxis(AVector::UNIT_Z),
    mNumPoints(0),
    mPointLinearSpeed(0),
    mPointAngularSpeed(0),
    mPointLinearAxis(0),
    mPointAngularAxis(0)
{
}
//----------------------------------------------------------------------------
PointController::~PointController ()
{
    delete1(mPointLinearSpeed);
    delete1(mPointAngularSpeed);
    delete1(mPointLinearAxis);
    delete1(mPointAngularAxis);
}
//----------------------------------------------------------------------------
bool PointController::Update (double applicationTime)
{
    if (!Controller::Update(applicationTime))
    {
        return false;
    }

    float ctrlTime = (float)GetControlTime(applicationTime);

    UpdateSystemMotion(ctrlTime);
    UpdatePointMotion(ctrlTime);
    return true;
}
//----------------------------------------------------------------------------
void PointController::Reallocate (int numPoints)
{
    delete1(mPointLinearSpeed);
    delete1(mPointAngularSpeed);
    delete1(mPointLinearAxis);
    delete1(mPointAngularAxis);

    mNumPoints = numPoints;
    if (mNumPoints > 0)
    {
        mPointLinearSpeed = new1<float>(mNumPoints);
        mPointAngularSpeed = new1<float>(mNumPoints);
        mPointLinearAxis = new1<AVector>(mNumPoints);
        mPointAngularAxis = new1<AVector>(mNumPoints);
        for (int i = 0; i < mNumPoints; ++i)
        {
            mPointLinearSpeed[i] = 0.0f;
            mPointAngularSpeed[i] = 0.0f;
            mPointLinearAxis[i] = AVector::UNIT_Z;
            mPointAngularAxis[i] = AVector::UNIT_Z;
        }
    }
}
//----------------------------------------------------------------------------
void PointController::SetObject (ControlledObject* object)
{
    Controller::SetObject(object);

    if (object)
    {
        assertion(object->IsDerived(Polypoint::TYPE), "Invalid class\n");
        Polypoint* points = StaticCast<Polypoint>(object);
        Reallocate(points->GetVertexBuffer()->GetNumElements());
    }
    else
    {
        Reallocate(0);
    }
}
//----------------------------------------------------------------------------
void PointController::UpdateSystemMotion (float ctrlTime)
{
    Polypoint* points = StaticCast<Polypoint>(mObject);

    float distance = ctrlTime*SystemLinearSpeed;
    AVector deltaTrn = distance*SystemLinearAxis;
    points->LocalTransform.SetTranslate(
        points->LocalTransform.GetTranslate() + deltaTrn);

    float angle = ctrlTime*SystemAngularSpeed;
    HMatrix deltaRot(SystemAngularAxis, angle);
    points->LocalTransform.SetRotate(
        deltaRot*points->LocalTransform.GetRotate());
}
//----------------------------------------------------------------------------
void PointController::UpdatePointMotion (float ctrlTime)
{
    Polypoint* points = StaticCast<Polypoint>(mObject);

    VertexBufferAccessor vba(points);

    const int numPoints = points->GetNumPoints();
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        float distance = ctrlTime*mPointLinearSpeed[i];
        APoint position = vba.Position<Float3>(i);
        AVector deltaTrn = distance*mPointLinearAxis[i];
        vba.Position<Float3>(i) = position + deltaTrn;
    }

    if (vba.HasNormal())
    {
        for (i = 0; i < numPoints; ++i)
        {
            float angle = ctrlTime*mPointAngularSpeed[i];
            AVector normal = vba.Normal<Float3>(i);
            normal.Normalize();
            HMatrix deltaRot(mPointAngularAxis[i], angle);
            vba.Normal<Float3>(i) = deltaRot*normal;
        }
    }

    Renderer::UpdateAll(points->GetVertexBuffer());
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
PointController::PointController (LoadConstructor value)
    :
    Controller(value),
    SystemLinearSpeed(0.0f),
    SystemAngularSpeed(0.0f),
    SystemLinearAxis(AVector::ZERO),
    SystemAngularAxis(AVector::ZERO),
    mNumPoints(0),
    mPointLinearSpeed(0),
    mPointAngularSpeed(0),
    mPointLinearAxis(0),
    mPointAngularAxis(0)
{
}
//----------------------------------------------------------------------------
void PointController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Controller::Load(source);

    source.Read(SystemLinearSpeed);
    source.Read(SystemAngularSpeed);
    source.Read(SystemLinearAxis);
    source.Read(SystemAngularAxis);
    source.ReadRR(mNumPoints, mPointLinearSpeed);
    source.ReadVR(mNumPoints, mPointAngularSpeed);
    source.ReadAggregateVR(mNumPoints, mPointLinearAxis);
    source.ReadAggregateVR(mNumPoints, mPointAngularAxis);

    WM5_END_DEBUG_STREAM_LOAD(PointController, source);
}
//----------------------------------------------------------------------------
void PointController::Link (InStream& source)
{
    Controller::Link(source);
}
//----------------------------------------------------------------------------
void PointController::PostLink ()
{
    Controller::PostLink();
}
//----------------------------------------------------------------------------
bool PointController::Register (OutStream& target) const
{
    return Controller::Register(target);
}
//----------------------------------------------------------------------------
void PointController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Controller::Save(target);

    target.Write(SystemLinearSpeed);
    target.Write(SystemAngularSpeed);
    target.Write(SystemLinearAxis);
    target.Write(SystemAngularAxis);
    target.WriteW(mNumPoints, mPointLinearSpeed);
    target.WriteN(mNumPoints, mPointAngularSpeed);
    target.WriteAggregateN(mNumPoints, mPointLinearAxis);
    target.WriteAggregateN(mNumPoints, mPointAngularAxis);

    WM5_END_DEBUG_STREAM_SAVE(PointController, target);
}
//----------------------------------------------------------------------------
int PointController::GetStreamingSize () const
{
    int size = Controller::GetStreamingSize();
    size += sizeof(SystemLinearSpeed);
    size += sizeof(SystemAngularSpeed);
    size += sizeof(SystemLinearAxis);
    size += sizeof(SystemAngularAxis);
    size += sizeof(mNumPoints);
    size += mNumPoints*sizeof(mPointLinearSpeed[0]);
    size += mNumPoints*sizeof(mPointAngularSpeed[0]);
    size += mNumPoints*sizeof(mPointLinearAxis[0]);
    size += mNumPoints*sizeof(mPointAngularAxis[0]);
    return size;
}
//----------------------------------------------------------------------------
