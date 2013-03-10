// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5EXTREMALQUERY3_H
#define WM5EXTREMALQUERY3_H

#include "Wm5PhysicsLIB.h"
#include "Wm5ConvexPolyhedron3.h"

namespace Wm5
{

template <typename Real>
class WM5_PHYSICS_ITEM ExtremalQuery3
{
public:
    // Abstract base class.
    virtual ~ExtremalQuery3 ();

    // Member access.
    const ConvexPolyhedron3<Real>* GetPolytope () const;
    const Vector3<Real>* GetFaceNormals () const;

    // Compute the extreme vertices in the specified direction and return the
    // indices of the vertices in the polyhedron vertex array.
    virtual void GetExtremeVertices (const Vector3<Real>& direction,
        int& positiveDirection, int& negativeDirection) = 0;

protected:
    ExtremalQuery3 (const ConvexPolyhedron3<Real>* polytope);

    const ConvexPolyhedron3<Real>* mPolytope;
    Vector3<Real>* mFaceNormals;
};

typedef ExtremalQuery3<float> ExtremalQuery3f;
typedef ExtremalQuery3<double> ExtremalQuery3d;

}

#endif
