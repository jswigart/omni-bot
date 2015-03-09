// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE3BOX3_H
#define WM5INTRLINE3BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line3.h"
#include "Wm5Box3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Box3 (const Line3<Real>& line, const Box3<Real>& box);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Box3<Real>& GetBox () const;

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

    static bool Clip (Real denom, Real numer, Real& t0, Real& t1);

    // The objects to intersect.
    const Line3<Real>* mLine;
    const Box3<Real>* mBox;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];

public_internal:
    // Shared by IntrRay3Box3 and IntrSegment3Box3.
    static bool DoClipping (Real t0, Real t1, const Vector3<Real>& origin,
        const Vector3<Real>& direction, const Box3<Real>& box, bool solid,
        int& quantity, Vector3<Real> point[2], int& intrType);
};

typedef IntrLine3Box3<float> IntrLine3Box3f;
typedef IntrLine3Box3<double> IntrLine3Box3d;

}

#endif
