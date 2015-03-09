// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5IKJoint.h"
#include "Wm5Matrix3.h"
#include "Wm5Spatial.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, IKJoint);
WM5_IMPLEMENT_STREAM(IKJoint);
WM5_IMPLEMENT_FACTORY(IKJoint);

//----------------------------------------------------------------------------
IKJoint::IKJoint (Spatial* object, int numGoals, IKGoalPtr* goals)
    :
    mObject(object),
    mNumGoals(numGoals),
    mGoals(goals)
{
    for (int i = 0; i < 3; ++i)
    {
        AllowTranslation[i] = false;
        MinTranslation[i] = -Mathf::MAX_REAL;
        MaxTranslation[i] = Mathf::MAX_REAL;
        AllowRotation[i] = false;
        MinRotation[i] = -Mathf::PI;
        MaxRotation[i] = Mathf::PI;
    }
}
//----------------------------------------------------------------------------
IKJoint::~IKJoint ()
{
    delete1(mGoals);
}
//----------------------------------------------------------------------------
void IKJoint::UpdateWorldSRT ()
{
    const Spatial* parent = mObject->GetParent();
    if (parent)
    {
        mObject->WorldTransform =
            parent->WorldTransform*mObject->LocalTransform;
    }
    else
    {
        mObject->WorldTransform = mObject->LocalTransform;
    }
}
//----------------------------------------------------------------------------
void IKJoint::UpdateWorldRT ()
{
    const Spatial* parent = mObject->GetParent();
    if (parent)
    {
        HMatrix rot = parent->WorldTransform.GetRotate() *
            mObject->LocalTransform.GetRotate();
        mObject->WorldTransform.SetRotate(rot);

        APoint trn = parent->WorldTransform *
            mObject->LocalTransform.GetTranslate();
        mObject->WorldTransform.SetTranslate(trn);
    }
    else
    {
        mObject->WorldTransform.SetRotate(
            mObject->LocalTransform.GetRotate());

        mObject->WorldTransform.SetTranslate(
            mObject->LocalTransform.GetTranslate());
    }
}
//----------------------------------------------------------------------------
AVector IKJoint::GetAxis (int i)
{
    const Spatial* parent = mObject->GetParent();
    if (parent)
    {
        AVector axis;
        parent->WorldTransform.GetRotate().GetColumn(i, axis);
        return axis;
    }
    else if (i == 0)
    {
        return AVector::UNIT_X;
    }
    else if (i == 1)
    {
        return AVector::UNIT_Y;
    }
    else  //  i == 2
    {
        return AVector::UNIT_Z;
    }
}
//----------------------------------------------------------------------------
bool IKJoint::UpdateLocalT (int i)
{
    AVector U = GetAxis(i);
    float numer = 0.0f;
    float denom = 0.0f;

    float oldNorm = 0.0f;
    IKGoal* goal;
    int g;
    for (g = 0; g < mNumGoals; ++g)
    {
        goal = mGoals[g];
        AVector GmE = goal->GetTargetPosition() - goal->GetEffectorPosition();
        oldNorm += GmE.SquaredLength();
        numer += goal->Weight*U.Dot(GmE);
        denom += goal->Weight;
    }

    if (Mathf::FAbs(denom) <= Mathf::ZERO_TOLERANCE)
    {
        // Weights were too small, no translation.
        return false;
    }

    // Desired distance to translate along axis(i).
    float t = numer/denom;

    // Clamp to range.
    APoint trn = mObject->LocalTransform.GetTranslate();
    float desired = trn[i] + t;
    if (desired > MinTranslation[i])
    {
        if (desired < MaxTranslation[i])
        {
            trn[i] = desired;
        }
        else
        {
            t = MaxTranslation[i] - trn[i];
            trn[i] = MaxTranslation[i];
        }
    }
    else
    {
        t = MinTranslation[i] - trn[i];
        trn[i] = MinTranslation[i];
    }

    // Test whether step should be taken.
    float newNorm = 0.0f;
    AVector step = t*U;
    for (g = 0; g < mNumGoals; ++g)
    {
        goal = mGoals[g];
        APoint newE = goal->GetEffectorPosition() + step;
        AVector diff = goal->GetTargetPosition() - newE;
        newNorm += diff.SquaredLength();
    }
    if (newNorm >= oldNorm)
    {
        // Translation does not get effector closer to goal.
        return false;
    }

    // Update the local translation.
    mObject->LocalTransform.SetTranslate(trn);
    return true;
}
//----------------------------------------------------------------------------
bool IKJoint::UpdateLocalR (int i)
{
    AVector U = GetAxis(i);
    float numer = 0.0f;
    float denom = 0.0f;

    float oldNorm = 0.0f;
    IKGoal* goal;
    int g;
    for (g = 0; g < mNumGoals; ++g)
    {
        goal = mGoals[g];
        AVector EmP = goal->GetEffectorPosition() -
            mObject->WorldTransform.GetTranslate();
        AVector GmP = goal->GetTargetPosition() -
            mObject->WorldTransform.GetTranslate();
        AVector GmE = goal->GetTargetPosition() - goal->GetEffectorPosition();
        oldNorm += GmE.SquaredLength();
        AVector UxEmP = U.Cross(EmP);
        AVector UxUxEmP = U.Cross(UxEmP);
        numer += goal->Weight*GmP.Dot(UxEmP);
        denom -= goal->Weight*GmP.Dot(UxUxEmP);
    }

    if (numer*numer + denom*denom <= Mathf::ZERO_TOLERANCE)
    {
        // Undefined atan2, no rotation.
        return false;
    }

    // Desired angle to rotate about axis(i).
    float theta = Mathf::ATan2(numer, denom);

    // Factor local rotation into Euler angles.
    float euler[3];
    HMatrix rotate = mObject->LocalTransform.GetRotate();

    Matrix3f rot(
        rotate[0][0], rotate[0][1], rotate[0][2],
        rotate[1][0], rotate[1][1], rotate[1][2],
        rotate[2][0], rotate[2][1], rotate[2][2]);

    rot.ExtractEulerZYX(euler[2], euler[1], euler[0]);

    // Clamp to range.
    float desired = euler[i] + theta;
    if (desired > MinRotation[i])
    {
        if (desired < MaxRotation[i])
        {
            euler[i] = desired;
        }
        else
        {
            theta = MaxRotation[i] - euler[i];
            euler[i] = MaxRotation[i];
        }
    }
    else
    {
        theta = MinRotation[i] - euler[i];
        euler[i] = MinRotation[i];
    }

    // Test whether step should be taken.
    float newNorm = 0.0f;
    rotate.MakeRotation(U, theta);
    for (g = 0; g < mNumGoals; ++g)
    {
        goal = mGoals[g];
        AVector EmP = goal->GetEffectorPosition() -
            mObject->WorldTransform.GetTranslate();
        APoint newE = mObject->WorldTransform.GetTranslate() + rotate*EmP;
        AVector GmE = goal->GetTargetPosition() - newE;
        newNorm += GmE.SquaredLength();
    }

    if (newNorm >= oldNorm)
    {
        // Rotation does not get effector closer to goal.
        return false;
    }

    // Update the local rotation.
    rot.MakeEulerZYX(euler[2], euler[1], euler[0]);

    rotate = HMatrix(
        rot[0][0], rot[0][1], rot[0][2], 0.0f,
        rot[1][0], rot[1][1], rot[1][2], 0.0f,
        rot[2][0], rot[2][1], rot[2][2], 0.0f,
        0.0f,      0.0f,      0.0f,      1.0f);

    mObject->LocalTransform.SetRotate(rotate);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* IKJoint::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mObject, name, found);
    for (int i = 0; i < mNumGoals; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mGoals[i], name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void IKJoint::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mObject, name, objects);
    for (int i = 0; i < mNumGoals; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mGoals[i], name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
IKJoint::IKJoint (LoadConstructor value)
    :
    Object(value),
    mObject(0),
    mNumGoals(0),
    mGoals(0)
{
    for (int i = 0; i < 3; ++i)
    {
        AllowTranslation[i] = false;
        MinTranslation[i] = -Mathf::MAX_REAL;
        MaxTranslation[i] = Mathf::MAX_REAL;
        AllowRotation[i] = false;
        MinRotation[i] = -Mathf::PI;
        MaxRotation[i] = Mathf::PI;
    }
}
//----------------------------------------------------------------------------
void IKJoint::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadBoolVV(3, AllowTranslation);
    source.ReadVV(3, MinTranslation);
    source.ReadVV(3, MaxTranslation);
    source.ReadBoolVV(3, AllowRotation);
    source.ReadVV(3, MinRotation);
    source.ReadVV(3, MaxRotation);
    source.ReadPointer(mObject);
    source.ReadPointerRR(mNumGoals, mGoals);

    WM5_END_DEBUG_STREAM_LOAD(IKJoint, source);
}
//----------------------------------------------------------------------------
void IKJoint::Link (InStream& source)
{
    Object::Link(source);

    source.ResolveLink(mObject);
    source.ResolveLink(mNumGoals, mGoals);
}
//----------------------------------------------------------------------------
void IKJoint::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool IKJoint::Register (OutStream& target) const
{
    if (Object::Register(target)) 
    {
        target.Register(mObject);
        target.Register(mNumGoals, mGoals);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void IKJoint::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteBoolN(3, AllowTranslation);
    target.WriteN(3, MinTranslation);
    target.WriteN(3, MaxTranslation);
    target.WriteBoolN(3, AllowRotation);
    target.WriteN(3, MinRotation);
    target.WriteN(3, MaxRotation);
    target.WritePointer(mObject);
    target.WritePointerW(mNumGoals, mGoals);

    WM5_END_DEBUG_STREAM_SAVE(IKJoint, target);
}
//----------------------------------------------------------------------------
int IKJoint::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += 3*WM5_BOOLSIZE(AllowTranslation[0]);
    size += 3*sizeof(MinTranslation[0]);
    size += 3*sizeof(MaxTranslation[0]);
    size += 3*WM5_BOOLSIZE(AllowRotation[0]);
    size += 3*sizeof(MinRotation[0]);
    size += 3*sizeof(MaxRotation[0]);
    size += WM5_POINTERSIZE(mObject);
    size += sizeof(mNumGoals);
    size += mNumGoals*WM5_POINTERSIZE(mGoals[0]);
    return size;
}
//----------------------------------------------------------------------------
