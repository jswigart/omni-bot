// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTANCE_H
#define WM5DISTANCE_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real, typename TVector>
class WM5_MATHEMATICS_ITEM Distance
{
public:
    // Abstract base class.
    virtual ~Distance ();

    // Static distance queries.
    virtual Real Get () = 0;     // distance
    virtual Real GetSquared () = 0;  // squared distance

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const TVector& velocity0,
        const TVector& velocity1) = 0;
    virtual Real GetSquared (Real fT, const TVector& velocity0,
        const TVector& velocity1) = 0;

    // Derivative calculations for dynamic distance queries.  The defaults
    // use finite difference estimates
    //   f'(t) = (f(t+h)-f(t-h))/(2*h)
    // where h = DifferenceStep.  A derived class may override these and
    // provide implementations of exact formulas that do not require h.
    virtual Real GetDerivative (Real t, const TVector& velocity0,
        const TVector& velocity1);
    virtual Real GetDerivativeSquared (Real t, const TVector& velocity0,
        const TVector& velocity1);

    // Dynamic distance queries.  The function computes the smallest distance
    // between the two objects over the time interval [tmin,tmax].
    virtual Real Get (Real tmin, Real tmax, const TVector& velocity0,
        const TVector& relocity1);
    virtual Real GetSquared (Real fTMin, Real fTMax,
        const TVector& velocity0, const TVector& velocity1);

    // For Newton's method and inverse parabolic interpolation.
    int MaximumIterations;  // default = 8
    Real ZeroThreshold;     // default = Math<Real>::ZERO_TOLERANCE

    // For derivative approximations.
    void SetDifferenceStep (Real differenceStep);  // default = 1e-03
    Real GetDifferenceStep () const;

    // The time at which minimum distance occurs for the dynamic queries.
    Real GetContactTime () const;

    // Closest points on the two objects.  These are valid for static or
    // dynamic queries.  The set of closest points on a single object need
    // not be a single point.  In this case, the Boolean member functions
    // return 'true'.  A derived class may support querying for the full
    // contact set.
    const TVector& GetClosestPoint0 () const;
    const TVector& GetClosestPoint1 () const;
    bool HasMultipleClosestPoints0 () const;
    bool HasMultipleClosestPoints1 () const;

protected:
    Distance ();

    Real mContactTime;
    TVector mClosestPoint0;
    TVector mClosestPoint1;
    bool mHasMultipleClosestPoints0;
    bool mHasMultipleClosestPoints1;
    Real mDifferenceStep, mInvTwoDifferenceStep;
};

typedef Distance<float,Vector2f> Distance2f;
typedef Distance<float,Vector3f> Distance3f;
typedef Distance<double,Vector2d> Distance2d;
typedef Distance<double,Vector3d> Distance3d;

}

#endif
