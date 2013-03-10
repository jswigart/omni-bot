// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE3PLANE3_H
#define WM5INTRLINE3PLANE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line3.h"
#include "Wm5Plane3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine3Plane3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Plane3 (const Line3<Real>& line, const Plane3<Real>& plane);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Plane3<Real>& GetPlane () const;

    // Test-intersection query.
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    // P = origin + t*direction.
    virtual bool Find ();
    Real GetLineParameter () const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_LINE;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line3<Real>* mLine;
    const Plane3<Real>* mPlane;

    // Information about the intersection set.
    Real mLineParameter;
};

typedef IntrLine3Plane3<float> IntrLine3Plane3f;
typedef IntrLine3Plane3<double> IntrLine3Plane3d;

}

#endif
