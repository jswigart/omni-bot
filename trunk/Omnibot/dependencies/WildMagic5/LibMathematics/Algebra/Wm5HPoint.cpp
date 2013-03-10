// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5HPoint.h"
using namespace Wm5;

//----------------------------------------------------------------------------
HPoint::HPoint ()
{
}
//----------------------------------------------------------------------------
HPoint::HPoint (const HPoint& pnt)
{
    mTuple[0] = pnt.mTuple[0];
    mTuple[1] = pnt.mTuple[1];
    mTuple[2] = pnt.mTuple[2];
    mTuple[3] = pnt.mTuple[3];
}
//----------------------------------------------------------------------------
HPoint::HPoint (float x, float y, float z, float w)
{
    mTuple[0] = x;
    mTuple[1] = y;
    mTuple[2] = z;
    mTuple[3] = w;
}
//----------------------------------------------------------------------------
HPoint::~HPoint ()
{
}
//----------------------------------------------------------------------------
HPoint& HPoint::operator= (const HPoint& pnt)
{
    mTuple[0] = pnt.mTuple[0];
    mTuple[1] = pnt.mTuple[1];
    mTuple[2] = pnt.mTuple[2];
    mTuple[3] = pnt.mTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
bool HPoint::operator== (const HPoint& pnt) const
{
    return memcmp(mTuple, pnt.mTuple, 4*sizeof(float)) == 0;
}
//----------------------------------------------------------------------------
bool HPoint::operator!= (const HPoint& pnt) const
{
    return memcmp(mTuple, pnt.mTuple, 4*sizeof(float)) != 0;
}
//----------------------------------------------------------------------------
bool HPoint::operator< (const HPoint& pnt) const
{
    return memcmp(mTuple, pnt.mTuple, 4*sizeof(float)) < 0;
}
//----------------------------------------------------------------------------
bool HPoint::operator<= (const HPoint& pnt) const
{
    return memcmp(mTuple, pnt.mTuple, 4*sizeof(float)) <= 0;
}
//----------------------------------------------------------------------------
bool HPoint::operator> (const HPoint& pnt) const
{
    return memcmp(mTuple, pnt.mTuple, 4*sizeof(float)) > 0;
}
//----------------------------------------------------------------------------
bool HPoint::operator>= (const HPoint& pnt) const
{
    return memcmp(mTuple, pnt.mTuple, 4*sizeof(float)) >= 0;
}
//----------------------------------------------------------------------------
