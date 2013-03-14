// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRLINE2ARC2_H
#define WM5INTRLINE2ARC2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Line2.h"
#include "Wm5Arc2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrLine2Arc2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Arc2 (const Line2<Real>& line, const Arc2<Real>& arc);

    // Object access.
    const Line2<Real>& GetLine () const;
    const Arc2<Real>& GetArc () const;

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
    const Arc2<Real>* mArc;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];
};

typedef IntrLine2Arc2<float> IntrLine2Arc2f;
typedef IntrLine2Arc2<double> IntrLine2Arc2d;

}

#endif
