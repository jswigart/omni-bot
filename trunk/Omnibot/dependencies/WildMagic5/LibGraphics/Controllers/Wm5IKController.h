// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IKCONTROLLER_H
#define WM5IKCONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Controller.h"
#include "Wm5IKGoal.h"
#include "Wm5IKJoint.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM IKController : public Controller
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(IKController);

public:
    // Construction and destruction.  IKController assumes responsibility for
    // the input arrays and will delete them.  They should be dynamically
    // allocated.
    IKController (int numJoints, IKJointPtr* joints, int numGoals,
        IKGoalPtr* goals);

    virtual ~IKController ();

    // Member access.
    int Iterations;       // default = 128
    bool OrderEndToRoot;  // default = true

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

protected:
    int mNumJoints;
    IKJointPtr* mJoints;
    int mNumGoals;
    IKGoalPtr* mGoals;
};

WM5_REGISTER_STREAM(IKController);
typedef Pointer0<IKController> IKControllerPtr;

}

#endif
