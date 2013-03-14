// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRARC2ARC2_H
#define WM5INTRARC2ARC2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Arc2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrArc2Arc2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrArc2Arc2 (const Arc2<Real>& arc0, const Arc2<Real>& arc1);

    // Object access.
    const Arc2<Real>& GetArc0 () const;
    const Arc2<Real>& GetArc1 () const;

    // Static find-intersection query.
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q is
    // 0, 1, or 2.  When Q > 0, the interpretation depends on the intersection
    // type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The arcs overlap by more than a point.  The arc of
    //              intersection is accessed by GetIntersectionArc().  Q is
    //              invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Arc2<Real>& GetIntersectionArc () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Arc2<Real>* mArc0;
    const Arc2<Real>* mArc1;

    // Set of intersection.
    int mQuantity;
    Vector2<Real> mPoint[2];
    Arc2<Real> mIntersectionArc;
};

typedef IntrArc2Arc2<float> IntrArc2Arc2f;
typedef IntrArc2Arc2<double> IntrArc2Arc2d;

}

#endif
