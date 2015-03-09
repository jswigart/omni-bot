// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5APoint.h"
using namespace Wm5;

const APoint APoint::ORIGIN(0.0f, 0.0f, 0.0f);

//----------------------------------------------------------------------------
APoint::APoint ()
{
    mTuple[0] = 0.0f;
    mTuple[1] = 0.0f;
    mTuple[2] = 0.0f;
    mTuple[3] = 1.0f;
}
//----------------------------------------------------------------------------
APoint::APoint (const APoint& pnt)
{
    mTuple[0] = pnt.mTuple[0];
    mTuple[1] = pnt.mTuple[1];
    mTuple[2] = pnt.mTuple[2];
    mTuple[3] = 1.0f;
}
//----------------------------------------------------------------------------
APoint::APoint (float x, float y, float z)
{
    mTuple[0] = x;
    mTuple[1] = y;
    mTuple[2] = z;
    mTuple[3] = 1.0f;
}
//----------------------------------------------------------------------------
APoint::APoint (const Float3& tuple)
{
    mTuple[0] = tuple[0];
    mTuple[1] = tuple[1];
    mTuple[2] = tuple[2];
    mTuple[3] = 1.0f;
}
//----------------------------------------------------------------------------
APoint::APoint (const Vector3f& pnt)
{
    mTuple[0] = pnt[0];
    mTuple[1] = pnt[1];
    mTuple[2] = pnt[2];
    mTuple[3] = 1.0f;
}
//----------------------------------------------------------------------------
APoint::~APoint ()
{
}
//----------------------------------------------------------------------------
APoint& APoint::operator= (const APoint& pnt)
{
    mTuple[0] = pnt.mTuple[0];
    mTuple[1] = pnt.mTuple[1];
    mTuple[2] = pnt.mTuple[2];
    mTuple[3] = 1.0f;
    return *this;
}
//----------------------------------------------------------------------------
AVector APoint::operator- (const APoint& pnt) const
{
    return AVector
    (
        mTuple[0] - pnt.mTuple[0],
        mTuple[1] - pnt.mTuple[1],
        mTuple[2] - pnt.mTuple[2]
    );
}
//----------------------------------------------------------------------------
APoint APoint::operator+ (const AVector& vec) const
{
    return APoint
    (
        mTuple[0] + vec[0],
        mTuple[1] + vec[1],
        mTuple[2] + vec[2]
    );
}
//----------------------------------------------------------------------------
APoint APoint::operator- (const AVector& vec) const
{
    return APoint
    (
        mTuple[0] - vec[0],
        mTuple[1] - vec[1],
        mTuple[2] - vec[2]
    );
}
//----------------------------------------------------------------------------
APoint& APoint::operator+= (const AVector& vec)
{
    mTuple[0] += vec[0];
    mTuple[1] += vec[1];
    mTuple[2] += vec[2];
    return *this;
}
//----------------------------------------------------------------------------
APoint& APoint::operator-= (const AVector& vec)
{
    mTuple[0] -= vec[0];
    mTuple[1] -= vec[1];
    mTuple[2] -= vec[2];
    return *this;
}
//----------------------------------------------------------------------------
APoint APoint::operator+ (const APoint& pnt) const
{
    return APoint
    (
        mTuple[0] + pnt.mTuple[0],
        mTuple[1] + pnt.mTuple[1],
        mTuple[2] + pnt.mTuple[2]
    );
}
//----------------------------------------------------------------------------
APoint APoint::operator* (float scalar) const
{
    return APoint
    (
        scalar*mTuple[0],
        scalar*mTuple[1],
        scalar*mTuple[2]
    );
}
//----------------------------------------------------------------------------
APoint APoint::operator/ (float scalar) const
{
    if (scalar != 0.0f)
    {
        float invScalar = 1.0f/scalar;
        return APoint
        (
            invScalar*mTuple[0],
            invScalar*mTuple[1],
            invScalar*mTuple[2]
        );
    }

    return APoint(FLT_MAX, FLT_MAX, FLT_MAX);
}
//----------------------------------------------------------------------------
APoint Wm5::operator* (float scalar, const APoint& pnt)
{
    return pnt*scalar;
}
//----------------------------------------------------------------------------
APoint& APoint::operator+= (const APoint& pnt)
{
    mTuple[0] += pnt[0];
    mTuple[1] += pnt[1];
    mTuple[2] += pnt[2];
    return *this;
}
//----------------------------------------------------------------------------
APoint& APoint::operator-= (const APoint& pnt)
{
    mTuple[0] -= pnt[0];
    mTuple[1] -= pnt[1];
    mTuple[2] -= pnt[2];
    return *this;
}
//----------------------------------------------------------------------------
APoint& APoint::operator*= (float scalar)
{
    mTuple[0] *= scalar;
    mTuple[1] *= scalar;
    mTuple[2] *= scalar;
    return *this;
}
//----------------------------------------------------------------------------
APoint& APoint::operator/= (float scalar)
{
    if (scalar != 0.0f)
    {
        float invScalar = 1.0f/scalar;
        mTuple[0] *= invScalar;
        mTuple[1] *= invScalar;
        mTuple[2] *= invScalar;
    }
    else
    {
        mTuple[0] = FLT_MAX;
        mTuple[1] = FLT_MAX;
        mTuple[2] = FLT_MAX;
    }

    return *this;
}
//----------------------------------------------------------------------------
APoint APoint::operator- () const
{
    return APoint(-mTuple[0], -mTuple[1], -mTuple[2]);
}
//----------------------------------------------------------------------------
float APoint::Dot (const AVector& vec) const
{
    return mTuple[0]*vec[0] + mTuple[1]*vec[1] + mTuple[2]*vec[2];
}
//----------------------------------------------------------------------------
