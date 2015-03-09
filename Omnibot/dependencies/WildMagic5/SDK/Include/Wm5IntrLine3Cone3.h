// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE3CONE3_H
#define WM5INTRLINE3CONE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line3.h"
#include "Wm5Cone3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine3Cone3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Cone3 (const Line3<Real>& line, const Cone3<Real>& cone);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Cone3<Real>& GetCone () const;

    // Static intersection query.
    virtual bool Find ();

    // The intersection set.  The possible intersection types are
    //   IT_EMTPY:   no intersection
    //   IT_POINT:   point[0] is the intersection point
    //   IT_SEGMENT: <point[0],point[1]> is the intersection segment
    //   IT_RAY:     point[0]+t*point[1] is the intersection ray
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::IT_RAY;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line3<Real>* mLine;
    const Cone3<Real>* mCone;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
};

typedef IntrLine3Cone3<float> IntrLine3Cone3f;
typedef IntrLine3Cone3<double> IntrLine3Cone3d;

}

#endif
