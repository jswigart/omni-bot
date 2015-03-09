// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE2BOX2_H
#define WM5INTRLINE2BOX2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line2.h"
#include "Wm5Box2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine2Box2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Box2 (const Line2<Real>& line, const Box2<Real>& box);

    // Object access.
    const Line2<Real>& GetLine () const;
    const Box2<Real>& GetBox () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    static bool Clip (Real denom, Real numer, Real& t0, Real& t1);

    // The objects to intersect.
    const Line2<Real>* mLine;
    const Box2<Real>* mBox;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];

public_internal:
    // Shared by IntrRay2Box2 and IntrSegment2Box2.
    static bool DoClipping (Real t0, Real t1, const Vector2<Real>& origin,
        const Vector2<Real>& direction, const Box2<Real>& box, bool solid,
        int& quantity, Vector2<Real> point[2], int& intrType);
};

typedef IntrLine2Box2<float> IntrLine2Box2f;
typedef IntrLine2Box2<double> IntrLine2Box2d;

}

#endif
