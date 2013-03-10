// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IKJOINT_H
#define WM5IKJOINT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5IKGoal.h"

namespace Wm5
{

class Spatial;

class WM5_GRAPHICS_ITEM IKJoint : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(IKJoint);

public:
    // Construction and destruction.  IKJoint assumes responsibility for the
    // input array of pointers to goals and will delete it.  The array should
    // therefore be dynamically allocated.  The goals themselves are managed
    // by the IKController and will be deleted by it.
    IKJoint (Spatial* object, int numGoals, IKGoalPtr* goals);
    virtual ~IKJoint ();

    // Member access.  Index i is for the joint's parent's world axis[i].
    bool AllowTranslation[3];  // default = false
    float MinTranslation[3];   // default = -infinity
    float MaxTranslation[3];   // default = +infinity
    bool AllowRotation[3];     // default = false
    float MinRotation[3];      // default = -infinity
    float MaxRotation[3];      // default = +infinity

protected:
    // Support for the IK update.
    friend class IKController;

    // Joint update.
    AVector GetAxis (int i);
    void UpdateWorldSRT ();
    void UpdateWorldRT ();
    bool UpdateLocalT (int i);
    bool UpdateLocalR (int i);

    // The managed object.
    Spatial* mObject;

    // The goals that affect this joint.
    int mNumGoals;
    IKGoalPtr* mGoals;
};

WM5_REGISTER_STREAM(IKJoint);
typedef Pointer0<IKJoint> IKJointPtr;

}

#endif
