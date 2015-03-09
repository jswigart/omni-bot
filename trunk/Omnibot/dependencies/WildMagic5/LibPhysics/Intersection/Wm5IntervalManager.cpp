// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5IntervalManager.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntervalManager<Real>::IntervalManager (
    std::vector<Vector2<Real> >& interval)
    :
    mInterval(&interval)
{
    Initialize();
}
//----------------------------------------------------------------------------
template <typename Real>
IntervalManager<Real>::~IntervalManager ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void IntervalManager<Real>::Initialize ()
{
    // Get the interval endpoints.
    int intrSize = (int)mInterval->size(), endpSize = 2*intrSize;
    mEndpoint.resize(endpSize);
    int i, j;
    for (i = 0, j = 0; i < intrSize; ++i)
    {
        Endpoint& emin = mEndpoint[j++];
        emin.Type = 0;
        emin.Value = (*mInterval)[i][0];
        emin.Index = i;

        Endpoint& emax = mEndpoint[j++];
        emax.Type = 1;
        emax.Value = (*mInterval)[i][1];
        emax.Index = i;
    }

    // Sort the interval endpoints.
    std::sort(mEndpoint.begin(), mEndpoint.end());

    // Create the interval-to-endpoint lookup table.
    mLookup.resize(endpSize);
    for (j = 0; j < endpSize; ++j)
    {
        Endpoint& endpoint = mEndpoint[j];
        mLookup[2*endpoint.Index + endpoint.Type] = j;
    }

    // Active set of intervals (stored by index in array).
    std::set<int> active;

    // Set of overlapping intervals (stored by pairs of indices in array).
    mOverlap.clear();

    // Sweep through the endpoints to determine overlapping intervals.
    for (i = 0; i < endpSize; ++i)
    {
        Endpoint& endpoint = mEndpoint[i];
        int index = endpoint.Index;
        if (endpoint.Type == 0)  // an interval 'begin' value
        {
            std::set<int>::iterator iter = active.begin();
            std::set<int>::iterator end = active.end();
            for (/**/; iter != end; ++iter)
            {
                int activeIndex = *iter;
                if (activeIndex < index)
                {
                    mOverlap.insert(EdgeKey(activeIndex, index));
                }
                else
                {
                    mOverlap.insert(EdgeKey(index, activeIndex));
                }
            }
            active.insert(index);
        }
        else  // an interval 'end' value
        {
            active.erase(index);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntervalManager<Real>::SetInterval (int i, Real minValue,
    Real maxValue)
{
    assertion(0 <= i && i < (int)mInterval->size(), "Invalid index\n");
    (*mInterval)[i][0] = minValue;
    (*mInterval)[i][1] = maxValue;
    mEndpoint[mLookup[2*i]].Value = minValue;
    mEndpoint[mLookup[2*i+1]].Value = maxValue;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntervalManager<Real>::GetInterval (int i, Real& minValue,
    Real& maxValue) const
{
    assertion(0 <= i && i < (int)mInterval->size(), "Invalid index\n");
    minValue = (*mInterval)[i][0];
    maxValue = (*mInterval)[i][1];
}
//----------------------------------------------------------------------------
template <typename Real>
void IntervalManager<Real>::Update ()
{
    // Apply an insertion sort.  Under the assumption that the intervals
    // have not changed much since the last call, the end points are nearly
    // sorted.  The insertion sort should be very fast in this case.
    int endpSize = (int)mEndpoint.size();
    for (int j = 1; j < endpSize; ++j)
    {
        Endpoint key = mEndpoint[j];
        int i = j - 1;
        while (i >= 0 && key < mEndpoint[i])
        {
            Endpoint e0 = mEndpoint[i];
            Endpoint e1 = mEndpoint[i+1];

            // Update the overlap status.
            if (e0.Type == 0)
            {
                if (e1.Type == 1)
                {
                    // The 'b' of interval E0.mIndex was smaller than the 'e'
                    // of interval E1.mIndex, and the intervals *might have
                    // been* overlapping.  Now 'b' and 'e' are swapped, and
                    // the intervals cannot overlap.  Remove the pair from
                    // the overlap set.  The removal operation needs to find
                    // the pair and erase it if it exists.  Finding the pair
                    // is the expensive part of the operation, so there is no
                    // time savings in testing for existence first and then
                    // deleting.
                    mOverlap.erase(EdgeKey(e0.Index, e1.Index));
                }
            }
            else
            {
                if (e1.Type == 0)
                {
                    // The 'b' of interval E1.index was larger than the 'e'
                    // of interval E0.index, and the intervals were not
                    // overlapping.  Now 'b' and 'e' are swapped, and the
                    // intervals *might be* overlapping.  Determine if they
                    // are overlapping and then insert.
                    const Vector2<Real>& i0 = (*mInterval)[e0.Index];
                    const Vector2<Real>& i1 = (*mInterval)[e1.Index];
                    if (i0[0] <= i1[1])
                    {
                        mOverlap.insert(EdgeKey(e0.Index, e1.Index));
                    }
                }
            }

            // Reorder the items to maintain the sorted list.
            mEndpoint[i] = e1;
            mEndpoint[i+1] = e0;
            mLookup[2*e1.Index + e1.Type] = i;
            mLookup[2*e0.Index + e0.Type] = i+1;
            i--;
        }
        mEndpoint[i+1] = key;
        mLookup[2*key.Index + key.Type] = i+1;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
const std::set<EdgeKey>& IntervalManager<Real>::GetOverlap () const
{
    return mOverlap;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IntervalManager::Endpoint
//----------------------------------------------------------------------------
template <typename Real>
bool IntervalManager<Real>::Endpoint::operator< (const Endpoint& endpoint)
    const
{
    if (Value < endpoint.Value)
    {
        return true;
    }
    if (Value > endpoint.Value)
    {
        return false;
    }
    return Type < endpoint.Type;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class IntervalManager<float>;

template WM5_PHYSICS_ITEM
class IntervalManager<double>;
//----------------------------------------------------------------------------
}
