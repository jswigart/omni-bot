// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5BoxManager.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BoxManager<Real>::BoxManager (std::vector<AxisAlignedBox3<Real> >& boxes)
    :
    mBoxes(&boxes)
{
    Initialize();
}
//----------------------------------------------------------------------------
template <typename Real>
BoxManager<Real>::~BoxManager ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void BoxManager<Real>::Initialize ()
{
    // Get the box endpoints.
    int intrSize = (int)mBoxes->size(), endpSize = 2*intrSize;
    mXEndpoints.resize(endpSize);
    mYEndpoints.resize(endpSize);
    mZEndpoints.resize(endpSize);
    int i, j;
    for (i = 0, j = 0; i < intrSize; ++i)
    {
        mXEndpoints[j].Type = 0;
        mXEndpoints[j].Value = (*mBoxes)[i].Min[0];
        mXEndpoints[j].Index = i;
        mYEndpoints[j].Type = 0;
        mYEndpoints[j].Value = (*mBoxes)[i].Min[1];
        mYEndpoints[j].Index = i;
        mZEndpoints[j].Type = 0;
        mZEndpoints[j].Value = (*mBoxes)[i].Min[2];
        mZEndpoints[j].Index = i;
        j++;

        mXEndpoints[j].Type = 1;
        mXEndpoints[j].Value = (*mBoxes)[i].Max[0];
        mXEndpoints[j].Index = i;
        mYEndpoints[j].Type = 1;
        mYEndpoints[j].Value = (*mBoxes)[i].Max[1];
        mYEndpoints[j].Index = i;
        mZEndpoints[j].Type = 1;
        mZEndpoints[j].Value = (*mBoxes)[i].Max[2];
        mZEndpoints[j].Index = i;
        j++;
    }

    // Sort the rectangle endpoints.
    std::sort(mXEndpoints.begin(), mXEndpoints.end());
    std::sort(mYEndpoints.begin(), mYEndpoints.end());
    std::sort(mZEndpoints.begin(), mZEndpoints.end());

    // Create the interval-to-endpoint lookup tables.
    mXLookup.resize(endpSize);
    mYLookup.resize(endpSize);
    mZLookup.resize(endpSize);
    for (j = 0; j < endpSize; ++j)
    {
        mXLookup[2*mXEndpoints[j].Index + mXEndpoints[j].Type] = j;
        mYLookup[2*mYEndpoints[j].Index + mYEndpoints[j].Type] = j;
        mZLookup[2*mZEndpoints[j].Index + mZEndpoints[j].Type] = j;
    }

    // Active set of rectangles (stored by index in array).
    std::set<int> active;

    // set of overlapping rectangles (stored by pairs of indices in array)
    mOverlap.clear();

    // Sweep through the endpoints to determine overlapping x-intervals.
    for (i = 0; i < endpSize; ++i)
    {
        Endpoint& endpoint = mXEndpoints[i];
        int index = endpoint.Index;
        if (endpoint.Type == 0)  // an interval 'begin' value
        {
            // In the 1D problem, the current interval overlaps with all the
            // active intervals.  In 3D this we also need to check for
            // y-overlap and z-overlap.
            std::set<int>::iterator iter = active.begin();
            std::set<int>::iterator end = active.end();
            for (/**/; iter != end; ++iter)
            {
                // Rectangles activeIndex and index overlap in the
                // x-dimension.  Test for overlap in the y-dimension and
                // z-dimension.
                int activeIndex = *iter;
                const AxisAlignedBox3<Real>& b0 = (*mBoxes)[activeIndex];
                const AxisAlignedBox3<Real>& b1 = (*mBoxes)[index];
                if (b0.HasYOverlap(b1) && b0.HasZOverlap(b1))
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
void BoxManager<Real>::SetBox (int i, const AxisAlignedBox3<Real>& box)
{
    assertion(0 <= i && i < (int)mBoxes->size(), "Invalid index\n");
    (*mBoxes)[i] = box;
    mXEndpoints[mXLookup[2*i]].Value = box.Min[0];
    mXEndpoints[mXLookup[2*i+1]].Value = box.Max[0];
    mYEndpoints[mYLookup[2*i]].Value = box.Min[1];
    mYEndpoints[mYLookup[2*i+1]].Value = box.Max[1];
    mZEndpoints[mZLookup[2*i]].Value = box.Min[2];
    mZEndpoints[mZLookup[2*i+1]].Value = box.Max[2];
}
//----------------------------------------------------------------------------
template <typename Real>
void BoxManager<Real>::GetBox (int i, AxisAlignedBox3<Real>& box) const
{
    assertion(0 <= i && i < (int)mBoxes->size(), "Invalid index\n");
    box = (*mBoxes)[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void BoxManager<Real>::InsertionSort (std::vector<Endpoint>& endpoint,
    std::vector<int>& lookup)
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
                    const AxisAlignedBox3<Real>& b0 = (*mBoxes)[e0.Index];
                    const AxisAlignedBox3<Real>& b1 = (*mBoxes)[e1.Index];
                    if (b0.TestIntersection(b1))
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
void BoxManager<Real>::Update ()
{
    InsertionSort(mXEndpoints, mXLookup);
    InsertionSort(mYEndpoints, mYLookup);
    InsertionSort(mZEndpoints, mZLookup);
}
//----------------------------------------------------------------------------
template <typename Real>
const std::set<EdgeKey>& BoxManager<Real>::GetOverlap () const
{
    return mOverlap;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BoxManager::Endpoint
//----------------------------------------------------------------------------
template <typename Real>
bool BoxManager<Real>::Endpoint::operator< (const Endpoint& endpoint) const
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
class BoxManager<float>;

template WM5_PHYSICS_ITEM
class BoxManager<double>;
//----------------------------------------------------------------------------
}
