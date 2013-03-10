// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRCAPSULE3CAPSULE3_H
#define WM5INTRCAPSULE3CAPSULE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Capsule3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrCapsule3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrCapsule3Capsule3 (const Capsule3<Real>& capsule0,
        const Capsule3<Real>& capsule1);

    // Object access.
    const Capsule3<Real>& GetCapsule0 () const;
    const Capsule3<Real>& GetCapsule1 () const;

    // Static test-intersection query.
    virtual bool Test ();

private:
    // The objects to intersect.
    const Capsule3<Real>* mCapsule0;
    const Capsule3<Real>* mCapsule1;
};

typedef IntrCapsule3Capsule3<float> IntrCapsule3Capsule3f;
typedef IntrCapsule3Capsule3<double> IntrCapsule3Capsule3d;

}

#endif
