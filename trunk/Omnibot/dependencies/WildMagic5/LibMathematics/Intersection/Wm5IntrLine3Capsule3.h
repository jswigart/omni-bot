// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE3CAPSULE3_H
#define WM5INTRLINE3CAPSULE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line3.h"
#include "Wm5Capsule3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Capsule3 (const Line3<Real>& line,
        const Capsule3<Real>& capsule);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Capsule3<Real>& GetCapsule () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line3<Real>* mLine;
    const Capsule3<Real>* mCapsule;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];

public_internal:
    // Shared by IntrRay3Capsule3 and IntrSegment3Capsule3.
    static int Find (const Vector3<Real>& origin, const Vector3<Real>& dir,
        const Capsule3<Real>& capsule, Real t[2]);
};

typedef IntrLine3Capsule3<float> IntrLine3Capsule3f;
typedef IntrLine3Capsule3<double> IntrLine3Capsule3d;

}

#endif
