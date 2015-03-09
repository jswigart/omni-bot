// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5IKController.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Controller, IKController);
WM5_IMPLEMENT_STREAM(IKController);
WM5_IMPLEMENT_FACTORY(IKController);

//----------------------------------------------------------------------------
IKController::IKController (int numJoints, IKJointPtr* joints, int numGoals,
    IKGoalPtr* goals)
    :
    Iterations(128),
    OrderEndToRoot(true),
    mNumJoints(numJoints),
    mJoints(joints),
    mNumGoals(numGoals),
    mGoals(goals)
{
}
//----------------------------------------------------------------------------
IKController::~IKController ()
{
    delete1(mJoints);
    delete1(mGoals);
}
//----------------------------------------------------------------------------
bool IKController::Update (double applicationTime)
{
    if (!Controller::Update(applicationTime))
    {
        return false;
    }

    // Make sure effectors are all current in world space.  It is assumed
    // that the joints form a chain, so the world transforms of joint I
    // are the parent transforms for the joint I+1.
    int k;
    for (k = 0; k < mNumJoints; ++k)
    {
        mJoints[k]->UpdateWorldSRT();
    }

    // Update joints one-at-a-time to meet goals.  As each joint is updated,
    // the nodes occurring in the chain after that joint must be made current
    // in world space.
    int iter, i, j;
    IKJoint* joint;
    if (OrderEndToRoot)
    {
        for (iter = 0; iter < Iterations; ++iter)
        {
            for (k = 0; k < mNumJoints; ++k)
            {
                int r = mNumJoints - 1 - k;
                joint = mJoints[r];

                for (i = 0; i < 3; ++i)
                {
                    if (joint->AllowTranslation[i])
                    {
                        if (joint->UpdateLocalT(i))
                        {
                            for (j = r; j < mNumJoints; ++j)
                            {
                                mJoints[j]->UpdateWorldRT();
                            }
                        }
                    }
                }

                for (i = 0; i < 3; ++i)
                {
                    if (joint->AllowRotation[i])
                    {
                        if (joint->UpdateLocalR(i))
                        {
                            for (j = r; j < mNumJoints; ++j)
                            {
                                mJoints[j]->UpdateWorldRT();
                            }
                        }
                    }
                }
            }
        }
    }
    else  // order root to end
    {
        for (iter = 0; iter < Iterations; ++iter)
        {
            for (k = 0; k < mNumJoints; ++k)
            {
                joint = mJoints[k];

                for (i = 0; i < 3; ++i)
                {
                    if (joint->AllowTranslation[i])
                    {
                        if (joint->UpdateLocalT(i))
                        {
                            for (j = k; j < mNumJoints; ++j)
                            {
                                mJoints[j]->UpdateWorldRT();
                            }
                        }
                    }
                }

                for (i = 0; i < 3; ++i)
                {
                    if (joint->AllowRotation[i])
                    {
                        if (joint->UpdateLocalR(i))
                        {
                            for (j = k; j < mNumJoints; ++j)
                            {
                                mJoints[j]->UpdateWorldRT();
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* IKController::GetObjectByName (const std::string& name)
{
    Object* found = Controller::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    int i;
    for (i = 0; i < mNumJoints; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mJoints[i], name, found);
    }

    for (i = 0; i < mNumGoals; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mGoals[i], name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void IKController::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Controller::GetAllObjectsByName(name,objects);

    int i;
    for (i = 0; i < mNumJoints; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mJoints[i], name, objects);
    }

    for (i = 0; i < mNumGoals; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mGoals[i], name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
IKController::IKController (LoadConstructor value)
    :
    Controller(value),
    Iterations(0),
    OrderEndToRoot(false),
    mNumJoints(0),
    mNumGoals(0),
    mGoals(0)
{
}
//----------------------------------------------------------------------------
void IKController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Controller::Load(source);

    source.Read(Iterations);
    source.ReadBool(OrderEndToRoot);
    source.ReadPointerRR(mNumJoints, mJoints);
    source.ReadPointerRR(mNumGoals, mGoals);

    WM5_END_DEBUG_STREAM_LOAD(IKController, source);
}
//----------------------------------------------------------------------------
void IKController::Link (InStream& source)
{
    Controller::Link(source);

    source.ResolveLink(mNumJoints, mJoints);
    source.ResolveLink(mNumGoals, mGoals);
}
//----------------------------------------------------------------------------
void IKController::PostLink ()
{
    Controller::PostLink();
}
//----------------------------------------------------------------------------
bool IKController::Register (OutStream& target) const
{
    if (Controller::Register(target))
    {
        target.Register(mNumJoints, mJoints);
        target.Register(mNumGoals, mGoals);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void IKController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Controller::Save(target);

    target.Write(Iterations);
    target.WriteBool(OrderEndToRoot);
    target.WritePointerW(mNumJoints, mJoints);
    target.WritePointerW(mNumGoals, mGoals);

    WM5_END_DEBUG_STREAM_SAVE(IKController, target);
}
//----------------------------------------------------------------------------
int IKController::GetStreamingSize () const
{
    int size = Controller::GetStreamingSize();
    size += sizeof(Iterations);
    size += WM5_BOOLSIZE(OrderEndToRoot);
    size += sizeof(mNumJoints);
    size += mNumJoints*WM5_POINTERSIZE(mJoints[0]);
    size += sizeof(mNumGoals);
    size += mNumGoals*WM5_POINTERSIZE(mGoals[0]);
    return size;
}
//----------------------------------------------------------------------------
