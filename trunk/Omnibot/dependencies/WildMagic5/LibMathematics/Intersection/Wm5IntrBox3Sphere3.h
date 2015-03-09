// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRBOX3SPHERE3_H
#define WM5INTRBOX3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Box3.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrBox3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrBox3Sphere3 (const Box3<Real>& box, const Sphere3<Real>& sphere);

    // Object access.
    const Box3<Real>& GetBox () const;
    const Sphere3<Real>& GetSphere () const;

    // Test-intersection query.
    virtual bool Test ();

    // Find-intersection query.
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Intersection set for dynamic find-intersection query.
    const Vector3<Real>& GetContactPoint () const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_OTHER;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // Supporting functions for dynamic Find function.
    static Real GetVertexIntersection (Real dx, Real dy, Real dz, Real vx,
        Real vy, Real vz, Real rsqr);

    static Real GetEdgeIntersection (Real dx, Real dz, Real vx, Real vz,
        Real vsqr, Real rsqr);

    int FindFaceRegionIntersection (Real ex, Real ey, Real ez, Real cx,
        Real cy, Real cz, Real vx, Real vy, Real vz, Real& ix, Real& iy,
        Real& iz, bool aboveFace);

    int FindJustEdgeIntersection (Real cy, Real ex, Real ey, Real ez, Real dx,
        Real dz, Real vx, Real vy, Real vz, Real& ix, Real& iy, Real& iz);

    int FindEdgeRegionIntersection (Real ex, Real ey, Real ez, Real cx,
        Real cy, Real cz, Real vx, Real vy, Real vz, Real& ix, Real& iy,
        Real& iz, bool aboveEdge);

    int FindVertexRegionIntersection (Real ex, Real ey, Real ez, Real cx,
        Real cy, Real cz, Real vx, Real vy, Real vz, Real& ix, Real& iy,
        Real& iz);

    // The objects to intersect.
    const Box3<Real>* mBox;
    const Sphere3<Real>* mSphere;

    // Point of intersection.
    Vector3<Real> mContactPoint;
};

typedef IntrBox3Sphere3<float> IntrBox3Sphere3f;
typedef IntrBox3Sphere3<double> IntrBox3Sphere3d;

}

#endif
