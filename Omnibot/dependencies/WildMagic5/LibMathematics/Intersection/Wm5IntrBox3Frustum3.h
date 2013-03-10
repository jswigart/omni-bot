// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRBOX3FRUSTUM3_H
#define WM5INTRBOX3FRUSTUM3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Box3.h"
#include "Wm5Frustum3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrBox3Frustum3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrBox3Frustum3 (const Box3<Real>& box, const Frustum3<Real>& frustum);

    // Object access.
    const Box3<Real>& GetBox () const;
    const Frustum3<Real>& GetFrustum () const;

    // Test-intersection query.
    virtual bool Test ();

private:
    // The objects to intersect.
    const Box3<Real>* mBox;
    const Frustum3<Real>* mFrustum;
};

typedef IntrBox3Frustum3<float> IntrBox3Frustum3f;
typedef IntrBox3Frustum3<double> IntrBox3Frustum3d;

}

#endif
