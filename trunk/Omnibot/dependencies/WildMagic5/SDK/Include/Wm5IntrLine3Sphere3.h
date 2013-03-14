// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE3SPHERE3_H
#define WM5INTRLINE3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line3.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Sphere3 (const Line3<Real>& line, const Sphere3<Real>& sphere);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Sphere3<Real>& GetSphere () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetLineParameter (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line3<Real>* mLine;
    const Sphere3<Real>* mSphere;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
    Real mLineParameter[2];
};

typedef IntrLine3Sphere3<float> IntrLine3Sphere3f;
typedef IntrLine3Sphere3<double> IntrLine3Sphere3d;

}

#endif
