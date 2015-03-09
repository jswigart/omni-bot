// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTLINE3BOX3_H
#define WM5DISTLINE3BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Line3.h"
#include "Wm5Box3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistLine3Box3 
    : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Box3 (const Line3<Real>& line, const Box3<Real>& box);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Box3<Real>& GetBox () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Access to the line parameter for the closest point.  This is used by
    // the ray-box and segment-box distance calculators.
    Real GetLineParameter () const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    void Face (int i0, int i1, int i2, Vector3<Real>& pnt,
        const Vector3<Real>& dir, const Vector3<Real>& PmE,
        Real& sqrDistance);

    void CaseNoZeros (Vector3<Real>& pnt, const Vector3<Real>& dir,
        Real& sqrDistance);

    void Case0 (int i0, int i1, int i2, Vector3<Real>& pnt,
        const Vector3<Real>& dir, Real& sqrDistance);

    void Case00 (int i0, int i1, int i2, Vector3<Real>& pnt,
        const Vector3<Real>& dir, Real& sqrDistance);

    void Case000 (Vector3<Real>& pnt, Real& sqrDistance);

    const Line3<Real>* mLine;
    const Box3<Real>* mBox;
    Real mLineParameter;
};

typedef DistLine3Box3<float> DistLine3Box3f;
typedef DistLine3Box3<double> DistLine3Box3d;

}

#endif
