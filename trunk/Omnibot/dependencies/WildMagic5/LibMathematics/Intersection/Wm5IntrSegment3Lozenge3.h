// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT3LOZENGE3_H
#define WM5INTRSEGMENT3LOZENGE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment3.h"
#include "Wm5Lozenge3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment3Lozenge3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Lozenge3 (const Segment3<Real>& segment,
        const Lozenge3<Real>& lozenge);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Lozenge3<Real>& GetLozenge () const;

    // Static intersection query.
    virtual bool Test ();

private:
    // The objects to intersect.
    const Segment3<Real>* mSegment;
    const Lozenge3<Real>* mLozenge;
};

typedef IntrSegment3Lozenge3<float> IntrSegment3Lozenge3f;
typedef IntrSegment3Lozenge3<double> IntrSegment3Lozenge3d;

}

#endif
