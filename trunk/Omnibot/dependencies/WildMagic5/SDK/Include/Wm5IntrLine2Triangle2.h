// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE2TRIANGLE2_H
#define WM5INTRLINE2TRIANGLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line2.h"
#include "Wm5Triangle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine2Triangle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Triangle2 (const Line2<Real>& line,
        const Triangle2<Real>& triangle);

    // Object access.
    const Line2<Real>& GetLine () const;
    const Triangle2<Real>& GetTriangle () const;

    // Static intersection query.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the line and triangle do not intersect,
    // GetQuantity() returns 0, in which case the intersection type is
    // IT_EMPTY.  If the line and triangle intersect in a single point,
    // GetQuantity() returns 1, in which case the intersection type is
    // IT_POINT and GetPoint() returns the intersection point.  If the line
    // and triangle intersect in a segment, GetQuantity() returns 2, in which
    // case the intersection type is IT_SEGMENT and GetPoint() returns the
    // segment endpoints.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Line2<Real>* mLine;
    const Triangle2<Real>* mTriangle;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];

public_internal:
    // Shared by IntrRay2Triangle2 and IntrSegment2Triangle2.

    // Determine how the triangle and line intersect (if at all).
    static void TriangleLineRelations (const Vector2<Real>& origin,
        const Vector2<Real>& direction, const Triangle2<Real>& triangle,
        Real dist[3], int sign[3], int& positive, int& negative,
        int& zero);

    // Compute the parameter interval for the segment of intersection when
    // the triangle transversely intersects the line.
    static void GetInterval (const Vector2<Real>& origin,
        const Vector2<Real>& direction, const Triangle2<Real>& triangle,
        const Real dist[3], const int sign[3], Real param[2]);
};

typedef IntrLine2Triangle2<float> IntrLine2Triangle2f;
typedef IntrLine2Triangle2<double> IntrLine2Triangle2d;

}

#endif
