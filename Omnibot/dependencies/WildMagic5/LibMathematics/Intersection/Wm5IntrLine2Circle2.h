// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE2CIRCLE2_H
#define WM5INTRLINE2CIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line2.h"
#include "Wm5Circle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Circle2 (const Line2<Real>& line,
        const Circle2<Real>& circle);

    // Object access.
    const Line2<Real>& GetLine () const;
    const Circle2<Real>& GetCircle () const;

    // Static intersection query.
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line2<Real>* mLine;
    const Circle2<Real>* mCircle;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];

public_internal:
    // Shared by IntrRay2Circle2, IntrSegment2Circle2, IntrLine2Arc2,
    // IntrRay2Arc2, and IntrSegment2Arc2.
    static bool Find (const Vector2<Real>& origin,
        const Vector2<Real>& direction, const Vector2<Real>& center,
        Real radius, int& rootCount, Real t[2]);
};

typedef IntrLine2Circle2<float> IntrLine2Circle2f;
typedef IntrLine2Circle2<double> IntrLine2Circle2d;

}

#endif
