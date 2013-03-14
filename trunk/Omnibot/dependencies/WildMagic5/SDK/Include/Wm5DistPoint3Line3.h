// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTPOINT3LINE3_H
#define WM5DISTPOINT3LINE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Line3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint3Line3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistPoint3Line3 (const Vector3<Real>& point, const Line3<Real>& line);

    // Object access.
    const Vector3<Real>& GetPoint () const;
    const Line3<Real>& GetLine () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest line point.
    Real GetLineParameter () const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Vector3<Real>* mPoint;
    const Line3<Real>* mLine;

    // Information about the closest line point.
    Real mLineParameter;  // closest1 = line.origin+param*line.direction
};

typedef DistPoint3Line3<float> DistPoint3Line3f;
typedef DistPoint3Line3<double> DistPoint3Line3d;

}

#endif
