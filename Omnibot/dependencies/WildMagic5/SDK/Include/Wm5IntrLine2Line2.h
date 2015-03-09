// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE2LINE2_H
#define WM5INTRLINE2LINE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine2Line2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Line2 (const Line2<Real>& line0, const Line2<Real>& line1);

    // Object access.
    const Line2<Real>& GetLine0 () const;
    const Line2<Real>& GetLine1 () const;

    // Static intersection query.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the lines do not intersect, GetQuantity()
    // returns 0.  If the lines intersect in a single point, GetQuantity()
    // returns 1, in which case GetPoint() returns the point of intersection
    // and Intersector::GetIntersectionType() returns IT_POINT.  If the lines
    // are the same geometric entity, GetQuantity() returns INT_MAX and
    // Intersector::GetIntersectionType() returns IT_LINE.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_LINE;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // Determine the relationship between the two lines.
    int Classify (Real* s, Vector2<Real>* diff, Vector2<Real>* diffN);

    // The objects to intersect.
    const Line2<Real>* mLine0;
    const Line2<Real>* mLine1;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint;
};

typedef IntrLine2Line2<float> IntrLine2Line2f;
typedef IntrLine2Line2<double> IntrLine2Line2d;

}

#endif
