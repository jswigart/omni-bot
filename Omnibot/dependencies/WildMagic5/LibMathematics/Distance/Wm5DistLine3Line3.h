// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTLINE3LINE3_H
#define WM5DISTLINE3LINE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Line3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistLine3Line3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Line3 (const Line3<Real>& line0, const Line3<Real>& line1);

    // Object access.
    const Line3<Real>& GetLine0 () const;
    const Line3<Real>& GetLine1 () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest points.
    Real GetLine0Parameter () const;
    Real GetLine1Parameter () const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Line3<Real>* mLine0;
    const Line3<Real>* mLine1;

    // Information about the closest points.
    Real mLine0Parameter;  // closest0 = line0.origin+param*line0.direction
    Real mLine1Parameter;  // closest1 = line1.origin+param*line1.direction
};

typedef DistLine3Line3<float> DistLine3Line3f;
typedef DistLine3Line3<double> DistLine3Line3d;

}

#endif
