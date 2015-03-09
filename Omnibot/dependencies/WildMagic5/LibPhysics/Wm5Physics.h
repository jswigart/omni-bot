// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5PHYSICS_H
#define WM5PHYSICS_H

// CollisionDetection
#include "Wm5BoundTree.h"
#include "Wm5CollisionGroup.h"
#include "Wm5CollisionRecord.h"

// Fluid
#include "Wm5Fluid2Da.h"
#include "Wm5Fluid3Da.h"
#include "Wm5Fluid2Db.h"
#include "Wm5Fluid3Db.h"

// Intersection
#include "Wm5BoxManager.h"
#include "Wm5ExtremalQuery3.h"
#include "Wm5ExtremalQuery3BSP.h"
#include "Wm5ExtremalQuery3PRJ.h"
#include "Wm5IntervalManager.h"
#include "Wm5RectangleManager.h"

// LCPSolver
#include "Wm5LCPPolyDist.h"
#include "Wm5LCPSolver.h"

// ParticleSystem
#include "Wm5MassSpringArbitrary.h"
#include "Wm5MassSpringCurve.h"
#include "Wm5MassSpringSurface.h"
#include "Wm5MassSpringVolume.h"
#include "Wm5ParticleSystem.h"

// RigidBody
#include "Wm5PolyhedralMassProperties.h"
#include "Wm5RigidBody.h"

#endif
