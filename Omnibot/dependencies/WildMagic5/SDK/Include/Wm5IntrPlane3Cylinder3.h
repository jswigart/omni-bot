// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRPLANE3CYLINDER3_H
#define WM5INTRPLANE3CYLINDER3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Cylinder3.h"
#include "Wm5Circle3.h"
#include "Wm5Ellipse3.h"
#include "Wm5Line3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrPlane3Cylinder3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Cylinder3 (const Plane3<Real>& plane,
        const Cylinder3<Real>& cylinder);

    // Object access.
    const Plane3<Real>& GetPlane () const;
    const Cylinder3<Real>& GetCylinder () const;

    // Static intersection query for a *finite* cylinder.
    virtual bool Test ();

    // Static intersection query for an *infinite* cylinder.
    virtual bool Find ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The cylinder is culled if it is on the negative
    // side of the plane.
    bool CylinderIsCulled () const;

    // The intersection set for an *infinite* cylinder and the plane.
    enum
    {
        PC_EMPTY_SET,
        PC_ONE_LINE,
        PC_TWO_LINES,
        PC_CIRCLE,
        PC_ELLIPSE
    };

    int GetType () const;

    // Valid when GetType() returns PC_ONE_LINE.
    void GetOneLine (Line3<Real>& line) const;

    // Valid when GetType() returns PC_TWO_LINES.
    void GetTwoLines (Line3<Real>& line0, Line3<Real>& line1) const;

    // Valid when GetType() returns PC_CIRCLE.
    void GetCircle (Circle3<Real>& circle) const;

    // Valid when GetType() returns PC_ELLIPSE.
    void GetEllipse (Ellipse3<Real>& ellipse) const;

protected:
    // The objects to intersect.
    const Plane3<Real>* mPlane;
    const Cylinder3<Real>* mCylinder;

    // The intersection set when the cylinder is infinite.
    int mType;
    Line3<Real> mLine0, mLine1;
    Circle3<Real> mCircle;
    Ellipse3<Real> mEllipse;

};

typedef IntrPlane3Cylinder3<float> IntrPlane3Cylinder3f;
typedef IntrPlane3Cylinder3<double> IntrPlane3Cylinder3d;

}

#endif
