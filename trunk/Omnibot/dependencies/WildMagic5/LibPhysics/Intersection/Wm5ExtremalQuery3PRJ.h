// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5EXTREMALQUERY3PRJ_H
#define WM5EXTREMALQUERY3PRJ_H

#include "Wm5PhysicsLIB.h"
#include "Wm5ExtremalQuery3.h"

namespace Wm5
{

template <typename Real>
class WM5_PHYSICS_ITEM ExtremalQuery3PRJ : public ExtremalQuery3<Real>
{
public:
    ExtremalQuery3PRJ (const ConvexPolyhedron3<Real>* polytope);
    virtual ~ExtremalQuery3PRJ ();

    // Compute the extreme vertices in the specified direction and return the
    // indices of the vertices in the polyhedron vertex array.
    virtual void GetExtremeVertices (const Vector3<Real>& direction,
        int& positiveDirection, int& negativeDirection);

private:
    using ExtremalQuery3<Real>::mPolytope;

    Vector3<Real> mCentroid;
};

typedef ExtremalQuery3PRJ<float> ExtremalQuery3PRJf;
typedef ExtremalQuery3PRJ<double> ExtremalQuery3PRJd;

}

#endif
