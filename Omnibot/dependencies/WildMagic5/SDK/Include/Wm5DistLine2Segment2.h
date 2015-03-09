// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTLINE2SEGMENT2_H
#define WM5DISTLINE2SEGMENT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Line2.h"
#include "Wm5Segment2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistLine2Segment2
    : public Distance<Real,Vector2<Real> >
{
public:
    DistLine2Segment2 (const Line2<Real>& line,
        const Segment2<Real>& segment);

    // Object access.
    const Line2<Real>& GetLine () const;
    const Segment2<Real>& GetSegment () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);

private:
    using Distance<Real,Vector2<Real> >::mClosestPoint0;
    using Distance<Real,Vector2<Real> >::mClosestPoint1;

    const Line2<Real>* mLine;
    const Segment2<Real>* mSegment;
};

typedef DistLine2Segment2<float> DistLine2Segment2f;
typedef DistLine2Segment2<double> DistLine2Segment2d;

}

#endif
