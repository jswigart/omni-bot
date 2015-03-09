// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/11/02)

#ifndef WM5INTRTRIANGLE3CYLINDER3_H
#define WM5INTRTRIANGLE3CYLINDER3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Triangle3.h"
#include "Wm5Cylinder3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrTriangle3Cylinder3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Cylinder3 (const Triangle3<Real>& triangle,
        const Cylinder3<Real>& cylinder);

    // Object access.
    const Triangle3<Real>& GetTriangle () const;
    const Cylinder3<Real>& GetCylinder () const;

    // Static query.
    virtual bool Test ();

private:
    // Support for the static test query.
    bool DiskOverlapsPoint (const Vector2<Real>& Q) const;
    bool DiskOverlapsSegment (const Vector2<Real>& Q0,
        const Vector2<Real>& Q1) const;
    bool DiskOverlapsPolygon (int numVertices, const Vector2<Real>* Q) const;

    // The objects to intersect.
    const Triangle3<Real>* mTriangle;
    const Cylinder3<Real>* mCylinder;
};

typedef IntrTriangle3Cylinder3<float> IntrTriangle3Cylinder3f;
typedef IntrTriangle3Cylinder3<double> IntrTriangle3Cylinder3d;

}

#endif
