// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5RectangleManager.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
RectangleManager<Real>::RectangleManager (
    std::vector<AxisAlignedBox2<Real> >& rectangles)
    :
    mRectangles(&rectangles)
{
    Initialize();
}
//----------------------------------------------------------------------------
template <typename Real>
RectangleManager<Real>::~RectangleManager ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void RectangleManager<Real>::Initialize ()
{
    // Get the rectangle endpoints.
    int intrSize = (int)mRectangles->size(), endpSize = 2*intrSize;
    mXEndpoints.resize(endpSize);
    mYEndpoints.resize(endpSize);
    int i, j;
    for (i = 0, j = 0; i < intrSize; ++i)
    {
        mXEndpoints[j].Type = 0;
        mXEndpoints[j].Value = (*mRectangles)[i].Min[0];
        mXEndpoints[j].Index = i;
        mYEndpoints[j].Type = 0;
        mYEndpoints[j].Value = (*mRectangles)[i].Min[1];
        mYEndpoints[j].Index = i;
        j++;

        mXEndpoints[j].Type = 1;
        mXEndpoints[j].Value = (*mRectangles)[i].Max[0];
        mXEndpoints[j].Index = i;
        mYEndpoints[j].Type = 1;
        mYEndpoints[j].Value = (*mRectangles)[i].Max[1];
        mYEndpoints[j].Index = i;
        j++;
    }

    // Sort the rectangle endpoints.
    std::sort(mXEndpoints.begin(), mXEndpoints.end());
    std::sort(mYEndpoints.begin(), mYEndpoints.end());

    // Create the interval-to-endpoint lookup tables.
    mXLookup.resize(endpSize);
    mYLookup.resize(endpSize);
    for (j = 0; j < endpSize; ++j)
    {
        mXLookup[2*mXEndpoints[j].Index + mXEndpoints[j].Type] = j;
        mYLookup[2*mYEndpoints[j].Index + mYEndpoints[j].Type] = j;
    }

    // Active set of rectangles (stored by index in array).
    std::set<int> active;

    // Set of overlapping rectangles (stored by pairs of indices in array).
    mOverlap.clear();

    // Sweep through the endpoints to determine overlapping x-intervals.
    for (i = 0; i < endpSize; ++i)
    {
        Endpoint& endpoint = mXEndpoints[i];
        int index = endpoint.Index;
        if (endpoint.Type == 0)  // an interval 'begin' value
        {
            // In the 1D problem, the current interval overlaps with all the
            // active intervals.  In 2D this we also need to check for
            // y-overlap.
            std::set<int>::iterator iter = active.begin();
            std::set<int>::iterator end = active.end();
            for (/**/; iter != end; ++iter)
            {
                // Rectangles activeIndex and index overlap in the
                // x-dimension.  Test for overlap in the y-dimension.
                int activeIndex = *iter;
                const AxisAlignedBox2<Real>& r0 = (*mRectangles)[activeIndex];
                const AxisAlignedBox2<Real>& r1 = (*mRectangles)[index];
                if (r0.HasYOverlap(r1))
                {
                    if (activeIndex < index)
                    {
                        mOverlap.insert(EdgeKey(activeIndex, index));
                    }
                    else
                    {
                        mOverlap.insert(EdgeKey(index, activeIndex));
                    }
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
void RectangleManager<Real>::SetRectangle (int i,
    const AxisAlignedBox2<Real>& rectangle)
{
    assertion(0 <= i && i < (int)mRectangles->size(), "Invalid index\n");
    (*mRectangles)[i] = rectangle;
    mXEndpoints[mXLookup[2*i]].Value = rectangle.Min[0];
    mXEndpoints[mXLookup[2*i+1]].Value = rectangle.Max[0];
    mYEndpoints[mYLookup[2*i]].Value = rectangle.Min[1];
    mYEndpoints[mYLookup[2*i+1]].Value = rectangle.Max[1];
}
//----------------------------------------------------------------------------
template <typename Real>
void RectangleManager<Real>::GetRectangle (int i,
    AxisAlignedBox2<Real>& rectangle) const
{
    assertion(0 <= i && i < (int)mRectangles->size(), "Invalid index\n");
    rectangle = (*mRectangles)[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void RectangleManager<Real>::InsertionSort (
    std::vector<Endpoint>& endpoint, std::vector<int>& lookup)
{
    // Apply an insertion sort.  Under the assumption that the rectangles
    // have not changed much since the last call, the endpoints are nearly
    // sorted.  The insertion sort should be very fast in this case.

    int endpSize = (int)endpoint.size();
    for (int j = 1; j < endpSize; ++j)
    {
        Endpoint key = endpoint[j];
        int i = j - 1;
        while (i >= 0 && key < endpoint[i])
        {
            Endpoint e0 = endpoint[i];
            Endpoint e1 = endpoint[i+1];

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
                    // real time savings in testing for existence first, then
                    // deleting if it does.
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
                    const AxisAlignedBox2<Real>& r0 =
                        (*mRectangles)[e0.Index];

                    const AxisAlignedBox2<Real>& r1 =
                        (*mRectangles)[e1.Index];

                    if (r0.TestIntersection(r1))
                    {
                        mOverlap.insert(EdgeKey(e0.Index, e1.Index));
                    }
                }
            }

            // Reorder the items to maintain the sorted list.
            endpoint[i] = e1;
            endpoint[i+1] = e0;
            lookup[2*e1.Index + e1.Type] = i;
            lookup[2*e0.Index + e0.Type] = i+1;
            i--;
        }
        endpoint[i+1] = key;
        lookup[2*key.Index + key.Type] = i+1;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void RectangleManager<Real>::Update ()
{
    InsertionSort(mXEndpoints, mXLookup);
    InsertionSort(mYEndpoints, mYLookup);
}
//----------------------------------------------------------------------------
template <typename Real>
const std::set<EdgeKey>& RectangleManager<Real>::GetOverlap () const
{
    return mOverlap;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// RectangleManager::Endpoint
//----------------------------------------------------------------------------
template <typename Real>
bool RectangleManager<Real>::Endpoint::operator< (const Endpoint& endpoint)
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
class RectangleManager<float>;

template WM5_PHYSICS_ITEM
class RectangleManager<double>;
//----------------------------------------------------------------------------
}
