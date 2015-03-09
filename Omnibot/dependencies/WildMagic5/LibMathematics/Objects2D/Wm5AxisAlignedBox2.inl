// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
AxisAlignedBox2<Real>::AxisAlignedBox2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
AxisAlignedBox2<Real>::~AxisAlignedBox2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
AxisAlignedBox2<Real>::AxisAlignedBox2 (Real xmin, Real xmax, Real ymin,
    Real ymax)
{
    Min[0] = xmin;
    Max[0] = xmax;
    Min[1] = ymin;
    Max[1] = ymax;
}
//----------------------------------------------------------------------------
template <typename Real>
void AxisAlignedBox2<Real>::GetCenterExtents (Vector2<Real>& center,
    Real extent[2])
{
    center[0] = ((Real)0.5)*(Max[0] + Min[0]);
    center[1] = ((Real)0.5)*(Max[1] + Min[1]);
    extent[0] = ((Real)0.5)*(Max[0] - Min[0]);
    extent[1] = ((Real)0.5)*(Max[1] - Min[1]);
}
//----------------------------------------------------------------------------
template <typename Real>
bool AxisAlignedBox2<Real>::HasXOverlap (const AxisAlignedBox2& box) const
{
    return (Max[0] >= box.Min[0] && Min[0] <= box.Max[0]);
}
//----------------------------------------------------------------------------
template <typename Real>
bool AxisAlignedBox2<Real>::HasYOverlap (const AxisAlignedBox2& box) const
{
    return (Max[1] >= box.Min[1] && Min[1] <= box.Max[1]);
}
//----------------------------------------------------------------------------
template <typename Real>
bool AxisAlignedBox2<Real>::TestIntersection (const AxisAlignedBox2& box)
    const
{
    for (int i = 0; i < 2; ++i)
    {
        if (Max[i] < box.Min[i] || Min[i] > box.Max[i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool AxisAlignedBox2<Real>::FindIntersection (const AxisAlignedBox2& box,
    AxisAlignedBox2& intersection) const
{
    int i;
    for (i = 0; i < 2; ++i)
    {
        if (Max[i] < box.Min[i] || Min[i] > box.Max[i])
        {
            return false;
        }
    }

    for (i = 0; i < 2; ++i)
    {
        if (Max[i] <= box.Max[i])
        {
            intersection.Max[i] = Max[i];
        }
        else
        {
            intersection.Max[i] = box.Max[i];
        }

        if (Min[i] <= box.Min[i])
        {
            intersection.Min[i] = box.Min[i];
        }
        else
        {
            intersection.Min[i] = Min[i];
        }
    }
    return true;
}
//----------------------------------------------------------------------------
