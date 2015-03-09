// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTERSECTOR1_H
#define WM5INTERSECTOR1_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Math.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Intersector1
{
public:
    // A class for intersection of intervals [u0,u1] and [v0,v1].  The end
    // points must be ordered:  u0 <= u1 and v0 <= v1.  Values of MAX_REAL
    // and -MAX_REAL are allowed, and degenerate intervals are allowed:
    // u0 = u1 or v0 = v1.
    Intersector1 (Real u0, Real u1, Real v0, Real v1);
    Intersector1 (Real u[2], Real v[2]);
    ~Intersector1 ();

    // Object access.
    Real GetU (int i) const;
    Real GetV (int i) const;

    // Static intersection queries.
    bool Test ();
    bool Find ();

    // Dynamic intersection queries.  The Find query produces a set of first
    // contact.
    bool Test (Real tmax, Real speedU, Real speedV);
    bool Find (Real tmax, Real speedU, Real speedV);

    // The time at which two intervals are in first/last contact for the
    // dynamic intersection queries.
    Real GetFirstTime () const;
    Real GetLastTime () const;

    // Information about the intersection set.  The number of intersections
    // is 0 (intervals do not overlap), 1 (intervals are just touching), or
    // 2 (intervals intersect in an inteval).
    int GetNumIntersections () const;
    Real GetIntersection (int i) const;

protected:
    // The intervals to intersect.
    Real mU[2], mV[2];

    // Information about the intersection set.
    Real mFirstTime, mLastTime;
    int mNumIntersections;
    Real mIntersections[2];
};

typedef Intersector1<float> Intersector1f;
typedef Intersector1<double> Intersector1d;

}

#endif
