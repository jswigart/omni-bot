// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POLYHEDRALMASSPROPERTIES_H
#define WM5POLYHEDRALMASSPROPERTIES_H

#include "Wm5PhysicsLIB.h"
#include "Wm5Matrix3.h"

namespace Wm5
{

// The input triangle mesh must represent a polyhedron.  The triangles are
// represented as triples of indices <V0,V1,V2> into the vertex array.
// The index array has numTriangles such triples.  The Boolean value
// bodyCoords is 'true' if you want the inertia tensor to be relative to
// body coordinates, 'false' if you want it in world coordinates.
//
// NOTE:  The code assumes the rigid body has a constant density of 1.  If
// your application assigns a constant density of 'd', then you must multiply
// the output rfMass by 'd' and the output rkInertia by 'd'.

template <typename Real> WM5_PHYSICS_ITEM
void ComputeMassProperties (const Vector3<Real>* vertices, int numTriangles,
    const int* indices, bool bodyCoords, Real& mass,
    Vector3<Real>& center, Matrix3<Real>& inertia);

}

#endif
