// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5ExtremalQuery3PRJ.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ExtremalQuery3PRJ<Real>::ExtremalQuery3PRJ (
    const ConvexPolyhedron3<Real>* polytope)
    :
    ExtremalQuery3<Real>(polytope)
{
    mCentroid = mPolytope->ComputeVertexAverage();
}
//----------------------------------------------------------------------------
template <typename Real>
ExtremalQuery3PRJ<Real>::~ExtremalQuery3PRJ ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void ExtremalQuery3PRJ<Real>::GetExtremeVertices (
    const Vector3<Real>& direction, int& positiveDirection,
    int& negativeDirection)
{
    Vector3<Real> diff = mPolytope->GetVertex(0) - mCentroid;
    Real minValue = direction.Dot(diff);
    Real maxValue = minValue;
    negativeDirection = 0;
    positiveDirection = 0;

    const int numVertices = mPolytope->GetNumVertices();
    for (int i = 1; i < numVertices; ++i)
    {
        diff = mPolytope->GetVertex(i) - mCentroid;
        Real dot = direction.Dot(diff);
        if (dot < minValue)
        {
            negativeDirection = i;
            minValue = dot;
        }
        else if (dot > maxValue)
        {
            positiveDirection = i;
            maxValue = dot;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class ExtremalQuery3PRJ<float>;

template WM5_PHYSICS_ITEM
class ExtremalQuery3PRJ<double>;
//----------------------------------------------------------------------------
}
