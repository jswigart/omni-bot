// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/09)

#include "Wm5GraphicsPCH.h"
#include "Wm5Bound.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Bound::Bound ()
    :
    mCenter(APoint::ORIGIN),
    mRadius(0.0f)
{
    // The radius must be nonnegative.  A radius of zero denotes an invalid
    // bound.
}
//----------------------------------------------------------------------------
Bound::~Bound ()
{
}
//----------------------------------------------------------------------------
Bound& Bound::operator= (const Bound& bound)
{
    mCenter = bound.mCenter;
    mRadius = bound.mRadius;
    return *this;
}
//----------------------------------------------------------------------------
int Bound::WhichSide (const HPlane& plane) const
{
    float signedDistance = plane.DistanceTo(mCenter);

    if (signedDistance <= -mRadius)
    {
        return -1;
    }

    if (signedDistance >= mRadius)
    {
        return +1;
    }

    return 0;
}
//----------------------------------------------------------------------------
void Bound::GrowToContain (const Bound& bound)
{
    if (bound.mRadius == 0.0f)
    {
        // The incoming bound is invalid and cannot affect growth.
        return;
    }

    if (mRadius == 0.0f)
    {
        // The current bound is invalid, so just assign the incoming bound.
        *this = bound;
        return;
    }

    AVector centerDiff = bound.mCenter - mCenter;
    float lengthSqr = centerDiff.SquaredLength();
    float radiusDiff = bound.mRadius - mRadius;
    float radiusDiffSqr = radiusDiff*radiusDiff;

    if (radiusDiffSqr >= lengthSqr)
    {
        if (radiusDiff >= 0.0f)
        {
            mCenter = bound.mCenter;
            mRadius = bound.mRadius;
        }
        return;
    }

    float length = Mathf::Sqrt(lengthSqr);
    if (length > Mathf::ZERO_TOLERANCE)
    {
        float coeff = (length + radiusDiff)/(2.0f*length);
        mCenter += coeff*centerDiff;
    }

    mRadius = 0.5f*(length + mRadius + bound.mRadius);
}
//----------------------------------------------------------------------------
void Bound::TransformBy (const Transform& transform, Bound& bound)
{
    bound.mCenter = transform*mCenter;
    bound.mRadius = transform.GetNorm()*mRadius;
}
//----------------------------------------------------------------------------
void Bound::ComputeFromData (int numElements, int stride, const char* data)
{
    // The center is the average of the positions.
    float sum[3] = { 0.0f, 0.0f, 0.0f };
    int i;
    for (i = 0; i < numElements; ++i)
    {
        // This assumes the positions are at offset zero, which they should be
        // for vertex buffer data.
        const float* position = (const float*)(data + i*stride);
        sum[0] += position[0];
        sum[1] += position[1];
        sum[2] += position[2];
    }
    float invNumElements = 1.0f/(float)numElements;
    mCenter[0] = sum[0]*invNumElements;
    mCenter[1] = sum[1]*invNumElements;
    mCenter[2] = sum[2]*invNumElements;
    mCenter[3] = 1.0f;

    // The radius is the largest distance from the center to the positions.
    mRadius = 0.0f;
    for (i = 0; i < numElements; ++i)
    {
        // This assumes the positions are at offset zero, which they should be
        // for vertex buffer data.
        const float* position = (const float*)(data + i*stride);
        float diff[3] =
        {
            position[0] - mCenter[0],
            position[1] - mCenter[1],
            position[2] - mCenter[2]
        };
        float radiusSqr = diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2];
        if (radiusSqr > mRadius)
        {
            mRadius = radiusSqr;
        }
    }

    mRadius = Mathf::Sqrt(mRadius);
}
//----------------------------------------------------------------------------
bool Bound::TestIntersection (const APoint& origin, const AVector& direction,
    float tmin, float tmax) const
{
    if (mRadius == 0.0f)
    {
        // The bound is invalid and cannot be intersected.
        return false;
    }

    AVector diff;
    float a0, a1, discr;

    if (tmin == -Mathf::MAX_REAL)
    {
        assertion(tmax == Mathf::MAX_REAL,
            "tmax must be infinity for a line.\n");

        // Test for sphere-line intersection.
        diff = origin - mCenter;
        a0 = diff.Dot(diff) - mRadius*mRadius;
        a1 = direction.Dot(diff);
        discr = a1*a1 - a0;
        return discr >= 0.0f;
    }

    if (tmax == Mathf::MAX_REAL)
    {
        assertion(tmin == 0.0f, "tmin must be zero for a ray.\n");

        // Test for sphere-ray intersection.
        AVector diff = origin - mCenter;
        a0 = diff.Dot(diff) - mRadius*mRadius;
        if (a0 <= 0.0f)
        {
            // The ray origin is inside the sphere.
            return true;
        }
        // else: The ray origin is outside the sphere.

        a1 = direction.Dot(diff);
        if (a1 >= 0.0f)
        {
            // The ray forms an acute angle with diff, and so the ray is
            // directed from the sphere.  Thus, the ray origin is outside
            // the sphere, and points P+t*D for t >= 0 are even farther
            // away from the sphere.
            return false;
        }

        discr = a1*a1 - a0;
        return discr >= 0.0f;
    }

    assertion(tmax > tmin, "tmin < tmax is required for a segment.\n");

    // Test for sphere-segment intersection.
    float segExtent = 0.5f*(tmin + tmax);
    APoint segOrigin = origin + segExtent*direction;

    diff = segOrigin - mCenter;
    a0 = diff.Dot(diff) - mRadius*mRadius;
    a1 = direction.Dot(diff);
    discr = a1*a1 - a0;
    if (discr < 0.0f)
    {
        return false;
    }

    float tmp0 = segExtent*segExtent + a0;
    float tmp1 = 2.0f*a1*segExtent;
    float qm = tmp0 - tmp1;
    float qp = tmp0 + tmp1;
    if (qm*qp <= 0.0f)
    {
        return true;
    }

    return qm > 0.0f && Mathf::FAbs(a1) < segExtent;
}
//----------------------------------------------------------------------------
bool Bound::TestIntersection (const Bound& bound) const
{
    if (bound.mRadius == 0.0f || mRadius == 0.0f)
    {
        // One of the bounds is invalid and cannot be intersected.
        return false;
    }

    // Test for staticSphere-staticSphere intersection.
    AVector diff = mCenter - bound.mCenter;
    float rSum = mRadius + bound.mRadius;
    return diff.SquaredLength() <= rSum*rSum;
}
//----------------------------------------------------------------------------
bool Bound::TestIntersection (const Bound& bound, float tmax,
    const AVector& velocity0, const AVector& velocity1) const
{
    if (bound.mRadius == 0.0f || mRadius == 0.0f)
    {
        // One of the bounds is invalid and cannot be intersected.
        return false;
    }

    // Test for movingSphere-movingSphere intersection.
    AVector relVelocity = velocity1 - velocity0;
    AVector cenDiff = bound.mCenter - mCenter;
    float a = relVelocity.SquaredLength();
    float c = cenDiff.SquaredLength();
    float rSum = bound.mRadius + mRadius;
    float rSumSqr = rSum*rSum;

    if (a > 0.0f)
    {
        float b = cenDiff.Dot(relVelocity);
        if (b <= 0.0f)
        {
            if (-tmax*a <= b)
            {
                return a*c - b*b <= a*rSumSqr;
            }
            else
            {
                return tmax*(tmax*a + 2.0f*b) + c <= rSumSqr;
            }
        }
    }

    return c <= rSumSqr;
}
//----------------------------------------------------------------------------
