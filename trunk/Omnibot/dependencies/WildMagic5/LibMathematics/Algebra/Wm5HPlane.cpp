// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/08/30)

#include "Wm5MathematicsPCH.h"
#include "Wm5HPlane.h"
using namespace Wm5;

//----------------------------------------------------------------------------
HPlane::HPlane ()
{
}
//----------------------------------------------------------------------------
HPlane::HPlane (const HPlane& plane)
    :
    mTuple(plane.mTuple)
{
}
//----------------------------------------------------------------------------
HPlane::~HPlane ()
{
}
//----------------------------------------------------------------------------
HPlane::HPlane (float normal0, float normal1, float normal2, float constant)
{
    mTuple[0] = normal0;
    mTuple[1] = normal1;
    mTuple[2] = normal2;
    mTuple[3] = -constant;
}
//----------------------------------------------------------------------------
HPlane::HPlane (const AVector& normal, float constant)
{
    mTuple[0] = normal[0];
    mTuple[1] = normal[1];
    mTuple[2] = normal[2];
    mTuple[3] = -constant;
}
//----------------------------------------------------------------------------
HPlane::HPlane (const AVector& normal, const APoint& p)
{
    mTuple[0] = normal[0];
    mTuple[1] = normal[1];
    mTuple[2] = normal[2];
    mTuple[3] = -p.Dot(normal);
}
//----------------------------------------------------------------------------
HPlane::HPlane (const APoint& p0, const APoint& p1, const APoint& p2)
{
    AVector edge1 = p1 - p0;
    AVector edge2 = p2 - p0;
    AVector normal = edge1.UnitCross(edge2);
    mTuple[0] = normal[0];
    mTuple[1] = normal[1];
    mTuple[2] = normal[2];
    mTuple[3] = -p0.Dot(normal);
}
//----------------------------------------------------------------------------
HPlane::HPlane (const HPoint& tuple)
    :
    mTuple(tuple)
{
}
//----------------------------------------------------------------------------
HPlane& HPlane::operator= (const HPlane& plane)
{
    mTuple = plane.mTuple;
    return *this;
}
//----------------------------------------------------------------------------
bool HPlane::operator== (const HPlane& plane) const
{
    return memcmp(mTuple, plane.mTuple, 4*sizeof(float)) == 0;
}
//----------------------------------------------------------------------------
bool HPlane::operator!= (const HPlane& plane) const
{
    return memcmp(mTuple, plane.mTuple, 4*sizeof(float)) != 0;
}
//----------------------------------------------------------------------------
bool HPlane::operator< (const HPlane& plane) const
{
    return memcmp(mTuple, plane.mTuple, 4*sizeof(float)) < 0;
}
//----------------------------------------------------------------------------
bool HPlane::operator<= (const HPlane& plane) const
{
    return memcmp(mTuple, plane.mTuple, 4*sizeof(float)) <= 0;
}
//----------------------------------------------------------------------------
bool HPlane::operator> (const HPlane& plane) const
{
    return memcmp(mTuple, plane.mTuple, 4*sizeof(float)) > 0;
}
//----------------------------------------------------------------------------
bool HPlane::operator>= (const HPlane& plane) const
{
    return memcmp(mTuple, plane.mTuple, 4*sizeof(float)) >= 0;
}
//----------------------------------------------------------------------------
float HPlane::Normalize (const float epsilon)
{
    float length = sqrtf(mTuple[0]*mTuple[0] + mTuple[1]*mTuple[1] +
        mTuple[2]*mTuple[2]);

    if (length > epsilon)
    {
        float invLength = 1.0f/length;
        mTuple[0] *= invLength;
        mTuple[1] *= invLength;
        mTuple[2] *= invLength;
        mTuple[3] *= invLength;
    }

    return length;
}
//----------------------------------------------------------------------------
float HPlane::DistanceTo (const APoint& p) const
{
    return mTuple[0]*p[0] + mTuple[1]*p[1] + mTuple[2]*p[2] + mTuple[3];
}
//----------------------------------------------------------------------------
int HPlane::WhichSide (const APoint& p) const
{
    float distance = DistanceTo(p);

    if (distance < 0.0f)
    {
        return -1;
    }
    else if (distance > 0.0f)
    {
        return +1;
    }
    else
    {
        return 0;
    }
}
//----------------------------------------------------------------------------
