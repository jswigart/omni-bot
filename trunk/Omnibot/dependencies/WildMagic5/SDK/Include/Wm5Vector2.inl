// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/09/19)

//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real>::Vector2 ()
{
    // Uninitialized for performance in array construction.
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real>::Vector2 (const Vector2& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real>::Vector2 (const Tuple<2,Real>& tuple)
{
    mTuple[0] = tuple[0];
    mTuple[1] = tuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real>::Vector2 (Real x, Real y)
{
    mTuple[0] = x;
    mTuple[1] = y;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real>& Vector2<Real>::operator= (const Vector2& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real>& Vector2<Real>::operator= (const Tuple<2,Real>& tuple)
{
    mTuple[0] = tuple[0];
    mTuple[1] = tuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector2<Real>::X () const
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector2<Real>::X ()
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector2<Real>::Y () const
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector2<Real>::Y ()
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Vector2<Real>::operator+ (const Vector2& vec) const
{
    return Vector2
    (
        mTuple[0] + vec.mTuple[0],
        mTuple[1] + vec.mTuple[1]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Vector2<Real>::operator- (const Vector2& vec) const
{
    return Vector2
    (
        mTuple[0] - vec.mTuple[0],
        mTuple[1] - vec.mTuple[1]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Vector2<Real>::operator* (Real scalar) const
{
    return Vector2
    (
        scalar*mTuple[0],
        scalar*mTuple[1]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Vector2<Real>::operator/ (Real scalar) const
{
    Vector2 result;

    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        result.mTuple[0] = invScalar*mTuple[0];
        result.mTuple[1] = invScalar*mTuple[1];
    }
    else
    {
        result.mTuple[0] = Math<Real>::MAX_REAL;
        result.mTuple[1] = Math<Real>::MAX_REAL;
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Vector2<Real>::operator- () const
{
    return Vector2
    (
        -mTuple[0],
        -mTuple[1]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real>& Vector2<Real>::operator+= (const Vector2& vec)
{
    mTuple[0] += vec.mTuple[0];
    mTuple[1] += vec.mTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real>& Vector2<Real>::operator-= (const Vector2& vec)
{
    mTuple[0] -= vec.mTuple[0];
    mTuple[1] -= vec.mTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real>& Vector2<Real>::operator*= (Real scalar)
{
    mTuple[0] *= scalar;
    mTuple[1] *= scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real>& Vector2<Real>::operator/= (Real scalar)
{
    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        mTuple[0] *= invScalar;
        mTuple[1] *= invScalar;
    }
    else
    {
        mTuple[0] *= Math<Real>::MAX_REAL;
        mTuple[1] *= Math<Real>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector2<Real>::Length () const
{
    return Math<Real>::Sqrt
    (
        mTuple[0]*mTuple[0] +
        mTuple[1]*mTuple[1]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector2<Real>::SquaredLength () const
{
    return
        mTuple[0]*mTuple[0] +
        mTuple[1]*mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector2<Real>::Dot (const Vector2& vec) const
{
    return
        mTuple[0]*vec.mTuple[0] +
        mTuple[1]*vec.mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector2<Real>::Normalize (const Real epsilon)
{
    Real length = Length();

    if (length > epsilon)
    {
        Real invLength = ((Real)1)/length;
        mTuple[0] *= invLength;
        mTuple[1] *= invLength;
    }
    else
    {
        length = (Real)0;
        mTuple[0] = (Real)0;
        mTuple[1] = (Real)0;
    }

    return length;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Vector2<Real>::Perp () const
{
    return Vector2
    (
        mTuple[1],
        -mTuple[0]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Vector2<Real>::UnitPerp () const
{
    Vector2 perp(mTuple[1], -mTuple[0]);
    perp.Normalize();
    return perp;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector2<Real>::DotPerp (const Vector2& vec) const
{
    return mTuple[0]*vec.mTuple[1] - mTuple[1]*vec.mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector2<Real>::ComputeExtremes (int numVectors, const Vector2* vectors,
    Vector2& vmin, Vector2& vmax)
{
    assertion(numVectors > 0 && vectors,
        "Invalid inputs to ComputeExtremes\n");

    vmin = vectors[0];
    vmax = vmin;
    for (int j = 1; j < numVectors; ++j)
    {
        const Vector2& vec = vectors[j];
        for (int i = 0; i < 2; ++i)
        {
            if (vec[i] < vmin[i])
            {
                vmin[i] = vec[i];
            }
            else if (vec[i] > vmax[i])
            {
                vmax[i] = vec[i];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector2<Real>::Orthonormalize (Vector2& u, Vector2& v)
{
    // If the input vectors are v0 and v1, then the Gram-Schmidt
    // orthonormalization produces vectors u0 and u1 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.

    // Compute u0.
    u.Normalize();

    // Compute u1.
    Real dot0 = u.Dot(v); 
    v -= u*dot0;
    v.Normalize();
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector2<Real>::GenerateOrthonormalBasis (Vector2& u, Vector2& v)
{
    v.Normalize();
    u = v.Perp();
}
//----------------------------------------------------------------------------
template <typename Real>
bool Vector2<Real>::GetBarycentrics (const Vector2& v0, const Vector2& v1,
    const Vector2& v2, Real bary[3], Real epsilon) const
{
    // Compute the vectors relative to V2 of the triangle.
    Vector2 diff[3] = { v0 - v2, v1 - v2, *this - v2 };

    Real det = diff[0].DotPerp(diff[1]);
    if (Math<Real>::FAbs(det) > epsilon)
    {
        Real invDet = ((Real)1)/det;
        bary[0] = diff[2].DotPerp(diff[1])*invDet;
        bary[1] = diff[0].DotPerp(diff[2])*invDet;
        bary[2] = (Real)1 - bary[0] - bary[1];
        return true;
    }

    for (int i = 0; i < 3; ++i)
    {
        bary[i] = (Real)0;
    }

#ifdef WM5_ASSERT_ON_BARYCENTRIC2_DEGENERATE
    assertion(false, "Input triangle is degenerate.\n");
#endif
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector2<Real>::GetInformation (int numPoints, const Vector2* points,
    Real epsilon, Information& info)
{
    assertion(numPoints > 0 && points && epsilon >= (Real)0,
        "Invalid inputs to GetInformation\n");

    info.mExtremeCCW = false;

    // Compute the axis-aligned bounding box for the input points.  Keep track
    // of the indices into 'points' for the current min and max.
    int j, indexMin[2], indexMax[2];
    for (j = 0; j < 2; ++j)
    {
        info.mMin[j] = points[0][j];
        info.mMax[j] = info.mMin[j];
        indexMin[j] = 0;
        indexMax[j] = 0;
    }

    int i;
    for (i = 1; i < numPoints; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            if (points[i][j] < info.mMin[j])
            {
                info.mMin[j] = points[i][j];
                indexMin[j] = i;
            }
            else if (points[i][j] > info.mMax[j])
            {
                info.mMax[j] = points[i][j];
                indexMax[j] = i;
            }
        }
    }

    // Determine the maximum range for the bounding box.
    info.mMaxRange = info.mMax[0] - info.mMin[0];
    info.mExtreme[0] = indexMin[0];
    info.mExtreme[1] = indexMax[0];
    Real range = info.mMax[1] - info.mMin[1];
    if (range > info.mMaxRange)
    {
        info.mMaxRange = range;
        info.mExtreme[0] = indexMin[1];
        info.mExtreme[1] = indexMax[1];
    }

    // The origin is either the point of minimum x-value or point of
    // minimum y-value.
    info.mOrigin = points[info.mExtreme[0]];

    // Test whether the point set is (nearly) a point.
    if (info.mMaxRange < epsilon)
    {
        info.mDimension = 0;
        for (j = 0; j < 2; ++j)
        {
            info.mExtreme[j + 1] = info.mExtreme[0];
            info.mDirection[j][0] = (Real)0;
            info.mDirection[j][1] = (Real)0;
        }
        return;
    }

    // Test whether the point set is (nearly) a line segment.
    info.mDirection[0] = points[info.mExtreme[1]] - info.mOrigin;
    info.mDirection[0].Normalize();
    info.mDirection[1] = -info.mDirection[0].Perp();
    Real maxDistance = (Real)0;
    Real maxSign = (Real)0;
    info.mExtreme[2] = info.mExtreme[0];
    for (i = 0; i < numPoints; ++i)
    {
        Vector2 diff = points[i] - info.mOrigin;
        Real distance = info.mDirection[1].Dot(diff);
        Real sign = Math<Real>::Sign(distance);
        distance = Math<Real>::FAbs(distance);
        if (distance > maxDistance)
        {
            maxDistance = distance;
            maxSign = sign;
            info.mExtreme[2] = i;
        }
    }

    if (maxDistance < epsilon*info.mMaxRange)
    {
        info.mDimension = 1;
        info.mExtreme[2] = info.mExtreme[1];
        return;
    }

    info.mDimension = 2;
    info.mExtremeCCW = (maxSign > (Real)0);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> operator* (Real scalar, const Vector2<Real>& vec)
{
    return Vector2<Real>
    (
        scalar*vec[0],
        scalar*vec[1]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
std::ostream& operator<< (std::ostream& outFile, const Vector2<Real>& vec)
{
     return outFile << vec.X() << ' ' << vec.Y();
}
//----------------------------------------------------------------------------
