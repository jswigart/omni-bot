// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#include "Wm5GraphicsPCH.h"
#include "Wm5BlendTransformController.h"
#include "Wm5HQuaternion.h"
#include "Wm5Spatial.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TransformController, BlendTransformController);
WM5_IMPLEMENT_STREAM(BlendTransformController);
WM5_IMPLEMENT_FACTORY(BlendTransformController);

//----------------------------------------------------------------------------
BlendTransformController::BlendTransformController (
    TransformController* controller0, TransformController* controller1,
    bool rsMatrices, bool geometricRotation, bool geometricScale)
    :
    TransformController(Transform::IDENTITY),
    mController0(controller0),
    mController1(controller1),
    mWeight(0.0f),
    mRSMatrices(rsMatrices),
    mGeometricRotation(geometricRotation),
    mGeometricScale(geometricScale)
{
}
//----------------------------------------------------------------------------
BlendTransformController::~BlendTransformController ()
{
}
//----------------------------------------------------------------------------
bool BlendTransformController::Update (double applicationTime)
{
    if (!Controller::Update(applicationTime))
    {
        return false;
    }

    mController0->Update(applicationTime);
    mController1->Update(applicationTime);

    const Transform& xfrm0 = mController0->GetTransform();
    const Transform& xfrm1 = mController1->GetTransform();
    float oneMinusWeight = 1.0f - mWeight;

    // Arithmetic blend of translations.
    const APoint& trn0 = xfrm0.GetTranslate();
    const APoint& trn1 = xfrm1.GetTranslate();
    APoint blendTrn = oneMinusWeight*trn0 + mWeight*trn1;
    mLocalTransform.SetTranslate(blendTrn);

    if (mRSMatrices)
    {
        const HMatrix& rot0 = xfrm0.GetRotate();
        const HMatrix& rot1 = xfrm1.GetRotate();

        HQuaternion quat0(rot0), quat1(rot1);
        if (quat0.Dot(quat1) < 0.0f)
        {
            quat1 = -quat1;
        }

        APoint sca0 = xfrm0.GetScale();
        APoint sca1 = xfrm1.GetScale();
        HMatrix blendRot;
        HQuaternion blendQuat;
        APoint blendSca;

        if (mGeometricRotation)
        {
            blendQuat.Slerp(mWeight, quat0, quat1);
        }
        else
        {
            blendQuat = oneMinusWeight*quat0 + mWeight*quat1;
            blendQuat.Normalize();
        }
        blendQuat.ToRotationMatrix(blendRot);
        mLocalTransform.SetRotate(blendRot);

        if (mGeometricScale)
        {
            for (int i = 0; i < 3; ++i)
            {
                float s0 = sca0[i];
                float s1 = sca1[i];
                if (s0 != 0.0f && s1 != 0.0f)
                {
                    float sign0 = Mathf::Sign(s0);
                    float sign1 = Mathf::Sign(s1);
                    s0 = Mathf::FAbs(s0);
                    s1 = Mathf::FAbs(s1);
                    float pow0 = Mathf::Pow(s0, oneMinusWeight);
                    float pow1 = Mathf::Pow(s1, mWeight);
                    blendSca[i] = sign0*sign1*pow0*pow1;
                }
                else
                {
                    blendSca[i] = 0.0f;
                }
            }
        }
        else
        {
            blendSca = oneMinusWeight*sca0 + mWeight*sca1;
        }
        mLocalTransform.SetScale(blendSca);
    }
    else
    {
        // Arithemtic blend of matrices.
        const HMatrix& mat0 = xfrm0.GetMatrix();
        const HMatrix& mat1 = xfrm1.GetMatrix();
        HMatrix blendMat = oneMinusWeight*mat0 + mWeight*mat1;
        mLocalTransform.SetMatrix(blendMat);
    }

    Spatial* spatial = StaticCast<Spatial>(mObject);
    spatial->LocalTransform = mLocalTransform;
    return true;
}
//----------------------------------------------------------------------------
void BlendTransformController::SetObject (ControlledObject* object)
{
    TransformController::SetObject(object);
    mController0->SetObject(object);
    mController1->SetObject(object);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* BlendTransformController::GetObjectByName (const std::string& name)
{
    Object* found = TransformController::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mController0, name, found);
    WM5_GET_OBJECT_BY_NAME(mController1, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void BlendTransformController::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    TransformController::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mController0, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mController1, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
BlendTransformController::BlendTransformController (LoadConstructor value)
    :
    TransformController(value),
    mWeight(0.0f),
    mRSMatrices(false),
    mGeometricRotation(false),
    mGeometricScale(false)
{
}
//----------------------------------------------------------------------------
void BlendTransformController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TransformController::Load(source);

    source.ReadPointer(mController0);
    source.ReadPointer(mController1);
    source.Read(mWeight);
    source.ReadBool(mRSMatrices);
    source.ReadBool(mGeometricRotation);
    source.ReadBool(mGeometricScale);

    WM5_END_DEBUG_STREAM_LOAD(BlendTransformController, source);
}
//----------------------------------------------------------------------------
void BlendTransformController::Link (InStream& source)
{
    TransformController::Link(source);

    source.ResolveLink(mController0);
    source.ResolveLink(mController1);
}
//----------------------------------------------------------------------------
void BlendTransformController::PostLink ()
{
    TransformController::PostLink();
}
//----------------------------------------------------------------------------
bool BlendTransformController::Register (OutStream& target) const
{
    if (TransformController::Register(target))
    {
        target.Register(mController0);
        target.Register(mController1);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void BlendTransformController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TransformController::Save(target);

    target.WritePointer(mController0);
    target.WritePointer(mController1);
    target.Write(mWeight);
    target.WriteBool(mRSMatrices);
    target.WriteBool(mGeometricRotation);
    target.WriteBool(mGeometricScale);

    WM5_END_DEBUG_STREAM_SAVE(BlendTransformController, target);
}
//----------------------------------------------------------------------------
int BlendTransformController::GetStreamingSize () const
{
    int size = TransformController::GetStreamingSize();
    size += WM5_POINTERSIZE(mController0);
    size += WM5_POINTERSIZE(mController1);
    size += sizeof(mWeight);
    size += WM5_BOOLSIZE(mRSMatrices);
    size += WM5_BOOLSIZE(mGeometricRotation);
    size += WM5_BOOLSIZE(mGeometricScale);
    return size;
}
//----------------------------------------------------------------------------
