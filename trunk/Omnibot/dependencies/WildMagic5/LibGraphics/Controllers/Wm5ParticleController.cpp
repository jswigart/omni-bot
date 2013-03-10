// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ParticleController.h"
#include "Wm5Particles.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Controller, ParticleController);
WM5_IMPLEMENT_STREAM(ParticleController);
WM5_IMPLEMENT_ABSTRACT_FACTORY(ParticleController);
WM5_IMPLEMENT_DEFAULT_NAMES(Controller, ParticleController);

//----------------------------------------------------------------------------
ParticleController::ParticleController ()
    :
    SystemLinearSpeed(0.0f),
    SystemAngularSpeed(0.0f),
    SystemLinearAxis(AVector::UNIT_Z),
    SystemAngularAxis(AVector::UNIT_Z),
    SystemSizeChange(0.0f),
    mNumParticles(0),
    mParticleLinearSpeed(0),
    mParticleLinearAxis(0),
    mParticleSizeChange(0)
{
}
//----------------------------------------------------------------------------
ParticleController::~ParticleController ()
{
    delete1(mParticleLinearSpeed);
    delete1(mParticleLinearAxis);
    delete1(mParticleSizeChange);
}
//----------------------------------------------------------------------------
bool ParticleController::Update (double applicationTime)
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
void ParticleController::Reallocate (int numParticles)
{
    delete1(mParticleLinearSpeed);
    delete1(mParticleLinearAxis);
    delete1(mParticleSizeChange);

    mNumParticles = numParticles;
    if (mNumParticles > 0)
    {
        mParticleLinearSpeed = new1<float>(mNumParticles);
        mParticleLinearAxis = new1<AVector>(mNumParticles);
        mParticleSizeChange = new1<float>(mNumParticles);
        for (int i = 0; i < mNumParticles; ++i)
        {
            mParticleLinearSpeed[i] = 0.0f;
            mParticleLinearAxis[i] = AVector::UNIT_Z;
            mParticleSizeChange[i] = 0.0f;
        }
    }
}
//----------------------------------------------------------------------------
void ParticleController::SetObject (ControlledObject* object)
{
    Controller::SetObject(object);

    if (object)
    {
        assertion(object->IsDerived(Particles::TYPE), "Invalid class.\n");
        Particles* particles = StaticCast<Particles>(object);
        Reallocate(particles->GetNumParticles());
    }
    else
    {
        Reallocate(0);
    }
}
//----------------------------------------------------------------------------
void ParticleController::UpdateSystemMotion (float ctrlTime)
{
    Particles* particles = StaticCast<Particles>(mObject);

    float dSize = ctrlTime*SystemSizeChange;
    particles->SetSizeAdjust(particles->GetSizeAdjust() + dSize);
    if (particles->GetSizeAdjust() < 0.0f)
    {
        particles->SetSizeAdjust(0.0f);
    }

    float distance = ctrlTime*SystemLinearSpeed;
    AVector deltaTrn = distance*SystemLinearAxis;
    particles->LocalTransform.SetTranslate(
        particles->LocalTransform.GetTranslate() + deltaTrn);

    float angle = ctrlTime*SystemAngularSpeed;
    HMatrix deltaRot(SystemAngularAxis, angle);
    particles->LocalTransform.SetRotate(
        deltaRot*particles->LocalTransform.GetRotate());
}
//----------------------------------------------------------------------------
void ParticleController::UpdatePointMotion (float ctrlTime)
{
    Particles* particles = StaticCast<Particles>(mObject);
    Float4* posSizes = particles->GetPositionSizes();

    int numActive = particles->GetNumActive();
    for (int i = 0; i < numActive; ++i)
    {
        float dSize = ctrlTime*mParticleSizeChange[i];
        posSizes[i][3] += dSize;
        float distance = ctrlTime*mParticleLinearSpeed[i];
        AVector deltaTrn = distance*mParticleLinearAxis[i];
        posSizes[i][0] += deltaTrn[0];
        posSizes[i][1] += deltaTrn[1];
        posSizes[i][2] += deltaTrn[2];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
ParticleController::ParticleController (LoadConstructor value)
    :
    Controller(value),
    SystemLinearSpeed(0.0f),
    SystemAngularSpeed(0.0f),
    SystemLinearAxis(AVector::ZERO),
    SystemAngularAxis(AVector::ZERO),
    SystemSizeChange(0.0f),
    mNumParticles(0),
    mParticleLinearSpeed(0),
    mParticleLinearAxis(0),
    mParticleSizeChange(0)
{
}
//----------------------------------------------------------------------------
void ParticleController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Controller::Load(source);

    source.Read(SystemLinearSpeed);
    source.Read(SystemAngularSpeed);
    source.Read(SystemLinearAxis);
    source.Read(SystemAngularAxis);
    source.Read(SystemSizeChange);
    source.ReadRR(mNumParticles, mParticleLinearSpeed);
    source.ReadVR(mNumParticles, mParticleLinearAxis);
    source.ReadVR(mNumParticles, mParticleSizeChange);

    WM5_END_DEBUG_STREAM_LOAD(ParticleController, source);
}
//----------------------------------------------------------------------------
void ParticleController::Link (InStream& source)
{
    Controller::Link(source);
}
//----------------------------------------------------------------------------
void ParticleController::PostLink ()
{
    Controller::PostLink();
}
//----------------------------------------------------------------------------
bool ParticleController::Register (OutStream& target) const
{
    return Controller::Register(target);
}
//----------------------------------------------------------------------------
void ParticleController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Controller::Save(target);

    target.Write(SystemLinearSpeed);
    target.Write(SystemAngularSpeed);
    target.Write(SystemLinearAxis);
    target.Write(SystemAngularAxis);
    target.Write(SystemSizeChange);
    target.WriteW(mNumParticles, mParticleLinearSpeed);
    target.WriteAggregateN(mNumParticles, mParticleLinearAxis);
    target.WriteN(mNumParticles, mParticleSizeChange);

    WM5_END_DEBUG_STREAM_SAVE(ParticleController, target);
}
//----------------------------------------------------------------------------
int ParticleController::GetStreamingSize () const
{
    int size = Controller::GetStreamingSize();
    size += sizeof(SystemLinearSpeed);
    size += sizeof(SystemAngularSpeed);
    size += sizeof(SystemLinearAxis);
    size += sizeof(SystemAngularAxis);
    size += sizeof(SystemSizeChange);

    size += sizeof(mNumParticles);
    if (mNumParticles > 0)
    {
        size += mNumParticles*sizeof(mParticleLinearSpeed[0]);
        size += mNumParticles*sizeof(mParticleLinearAxis[0]);
        size += mNumParticles*sizeof(mParticleSizeChange[0]);
    }

    return size;
}
//----------------------------------------------------------------------------
