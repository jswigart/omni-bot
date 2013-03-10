// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTERVALMANAGER_H
#define WM5INTERVALMANAGER_H

#include "Wm5PhysicsLIB.h"
#include "Wm5EdgeKey.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class WM5_PHYSICS_ITEM IntervalManager
{
public:
    // Intervals are of the form [b,e] where b is the beginning value and e
    // is the ending value with b < e.  The end points are stored as a
    // Vector2.

    // Construction and destruction.
    IntervalManager (std::vector<Vector2<Real> >& interval);
    ~IntervalManager ();

    // This function is called by the constructor and does the sort-and-sweep
    // to initialize the update system.  However, if you add or remove items
    // from the array of intervals after the constructor call, you will need
    // to call this function once before you start the multiple calls of the
    // update function.
    void Initialize ();

    // After the system is initialized, you can move the intervals using this
    // function.  It is not enough to modify the input array of intervals
    // since the end point values stored internally by this class must also
    // change.  You can also retrieve the current interval information.
    void SetInterval (int i, Real minValue, Real maxValue);
    void GetInterval (int i, Real& minValue, Real& maxValue) const;
    
    // When you are finished moving intervals, call this function to determine
    // the overlapping intervals.  An incremental update is applied to
    // determine the new set of overlapping intervals.
    void Update ();

    // If (i,j) is in the overlap set, then interval i and interval j are
    // overlapping.  The indices are those for the the input array.  The
    // set elements (i,j) are stored so that i < j.
    const std::set<EdgeKey>& GetOverlap () const;

private:
    class Endpoint
    {
    public:
        Real Value; // endpoint value
        int Type;   // '0' if interval min, '1' if interval max.
        int Index;  // index of interval containing this endpoint

        // Support for sorting of endpoints.
        bool operator< (const Endpoint& endpoint) const;
    };

    std::vector<Vector2<Real> >* mInterval;
    std::vector<Endpoint> mEndpoint;
    std::set<EdgeKey> mOverlap;

    // The intervals are indexed 0 <= i < n.  The endpoint array has 2*n
    // entries.  The original 2*n interval values are ordered as b[0], e[0],
    // b[1], e[1], ..., b[n-1], e[n-1].  When the endpoint array is sorted,
    // the mapping between interval values and endpoints is lost.  In order
    // to modify interval values that are stored in the endpoint array, we
    // need to maintain the mapping.  This is done by the following lookup
    // table of 2*n entries.  The value mLookup[2*i] is the index of b[i]
    // in the endpoint array.  The value mLookup[2*i+1] is the index of
    // e[i] in the endpoint array.
    std::vector<int> mLookup;
};

typedef IntervalManager<float> IntervalManagerf;
typedef IntervalManager<double> IntervalManagerd;

}

#endif
