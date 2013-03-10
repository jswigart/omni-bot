// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE3TRIANGLE3_H
#define WM5INTRLINE3TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line3.h"
#include "Wm5Triangle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine3Triangle3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Triangle3 (const Line3<Real>& line,
        const Triangle3<Real>& triangle);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Triangle3<Real>& GetTriangle () const;

    // Test-intersection query.
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    //   P = origin + t*direction = b0*V0 + b1*V1 + b2*V2
    virtual bool Find ();
    Real GetLineParameter () const;
    Real GetTriBary0 () const;
    Real GetTriBary1 () const;
    Real GetTriBary2 () const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line3<Real>* mLine;
    const Triangle3<Real>* mTriangle;

    // Information about the intersection set.
    Real mLineParameter, mTriBary0, mTriBary1, mTriBary2;
};

typedef IntrLine3Triangle3<float> IntrLine3Triangle3f;
typedef IntrLine3Triangle3<double> IntrLine3Triangle3d;

}

#endif
