// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE3TORUS3_H
#define WM5INTRLINE3TORUS3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line3.h"
#include "Wm5Torus3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine3Torus3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Torus3 (const Line3<Real>& line, const Torus3<Real>& torus);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Torus3<Real>& GetTorus () const;

    // Static intersection query.
    virtual bool Find ();

    // The intersection set (quantity is at most 4).
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line3<Real>* mLine;
    const Torus3<Real>* mTorus;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[4];
};

typedef IntrLine3Torus3<float> IntrLine3Torus3f;
typedef IntrLine3Torus3<double> IntrLine3Torus3d;

}

#endif
