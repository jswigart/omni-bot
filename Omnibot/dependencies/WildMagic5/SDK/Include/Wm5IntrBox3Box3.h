// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRBOX3BOX3_H
#define WM5INTRBOX3BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Box3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrBox3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrBox3Box3 (const Box3<Real>& box0, const Box3<Real>& box1);

    // Object access.
    const Box3<Real>& GetBox0 () const;
    const Box3<Real>& GetBox1 () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Dynamic test-intersection query.  The first time of contact (if any)
    // is computed, but not any information about the contact set.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Dynamic find-intersection query.  The contact set is computed.
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // The intersection set for dynamic find-intersection.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

    // Dynamic test-intersection query where the boxes have constant linear
    // velocities *and* constant angular velocities.  The length of the
    // rotation axes are the angular speeds.  A differential equation solver
    // is used to predict the intersection.  The input numSteps is the
    // number of iterations for the numerical ODE solver.
    bool Test (Real tmax, int numSteps, const Vector3<Real>& velocity0,
        const Vector3<Real>& rotCenter0, const Vector3<Real>& rotAxis0,
        const Vector3<Real>& velocity1, const Vector3<Real>& rotCenter1,
        const Vector3<Real>& rotAxis1);

private:
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // Support for dynamic queries.  The inputs are the projection intervals
    // for the boxes onto a potential separating axis, the relative speed of
    // the intervals, and the maximum time for the query.  The outputs are
    // the first time when separating fails and the last time when separation
    // begins again along that axis.  The outputs are *updates* in the sense
    // that this function is called repeatedly for the potential separating
    // axes.  The output first time is updated only if it is larger than
    // the input first time.  The output last time is updated only if it is
    // smaller than the input last time.
    //
    // NOTE:  The BoxBoxAxisTest function could be used, but the box-box
    // code optimizes the projections of the boxes onto the various axes.
    // This function is effectively BoxBoxAxisTest but without the dot product
    // of axis-direction and velocity to obtain speed.  The optimizations are
    // to compute the speed with fewer operations.
    bool IsSeparated (Real min0, Real max0, Real min1, Real max1, Real speed,
        Real tmax, Real& tlast);

    // The objects to intersect.
    const Box3<Real>* mBox0;
    const Box3<Real>* mBox1;

    // The intersection set for dynamic find-intersection.  The worst case
    // is a polygon with 8 vertices.
    int mQuantity;
    Vector3<Real> mPoint[8];
};

typedef IntrBox3Box3<float> IntrBox3Box3f;
typedef IntrBox3Box3<double> IntrBox3Box3d;

}

#endif
