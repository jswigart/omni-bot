// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IKGOAL_H
#define WM5IKGOAL_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Spatial.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM IKGoal : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(IKGoal);

public:
    // Construction and destruction.
    IKGoal (Spatial* target, Spatial* effector, float weight);
    virtual ~IKGoal ();

    // Member access.
    float Weight;  // default = 1
    inline Spatial* GetTarget ();
    inline Spatial* GetEffector ();
    APoint GetTargetPosition () const;
    APoint GetEffectorPosition () const;

protected:
    SpatialPtr mTarget;
    SpatialPtr mEffector;
};

WM5_REGISTER_STREAM(IKGoal);
typedef Pointer0<IKGoal> IKGoalPtr;
#include "Wm5IKGoal.inl"

}

#endif
