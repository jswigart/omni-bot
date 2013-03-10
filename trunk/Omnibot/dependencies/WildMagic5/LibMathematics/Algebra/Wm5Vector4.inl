// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real>::Vector4 ()
{
    // Uninitialized for performance in array construction.
}
//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real>::Vector4 (const Vector4& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    mTuple[2] = vec.mTuple[2];
    mTuple[3] = vec.mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real>::Vector4 (const Tuple<4,Real>& tuple)
{
    mTuple[0] = tuple[0];
    mTuple[1] = tuple[1];
    mTuple[2] = tuple[2];
    mTuple[3] = tuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real>::Vector4 (Real x, Real y, Real z, Real w)
{
    mTuple[0] = x;
    mTuple[1] = y;
    mTuple[2] = z;
    mTuple[3] = w;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real>& Vector4<Real>::operator= (const Vector4& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    mTuple[2] = vec.mTuple[2];
    mTuple[3] = vec.mTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real>& Vector4<Real>::operator= (const Tuple<4,Real>& tuple)
{
    mTuple[0] = tuple[0];
    mTuple[1] = tuple[1];
    mTuple[2] = tuple[2];
    mTuple[3] = tuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::X () const
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector4<Real>::X ()
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::Y () const
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector4<Real>::Y ()
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::Z () const
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector4<Real>::Z ()
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::W () const
{
    return mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector4<Real>::W ()
{
    return mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real> Vector4<Real>::operator+ (const Vector4& vec) const
{
    return Vector4
    (
        mTuple[0] + vec.mTuple[0],
        mTuple[1] + vec.mTuple[1],
        mTuple[2] + vec.mTuple[2],
        mTuple[3] + vec.mTuple[3]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real> Vector4<Real>::operator- (const Vector4& vec) const
{
    return Vector4
    (
        mTuple[0] - vec.mTuple[0],
        mTuple[1] - vec.mTuple[1],
        mTuple[2] - vec.mTuple[2],
        mTuple[3] - vec.mTuple[3]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real> Vector4<Real>::operator* (Real scalar) const
{
    return Vector4
    (
        scalar*mTuple[0],
        scalar*mTuple[1],
        scalar*mTuple[2],
        scalar*mTuple[3]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real> Vector4<Real>::operator/ (Real scalar) const
{
    Vector4 result;

    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        result.mTuple[0] = invScalar*mTuple[0];
        result.mTuple[1] = invScalar*mTuple[1];
        result.mTuple[2] = invScalar*mTuple[2];
        result.mTuple[3] = invScalar*mTuple[3];
    }
    else
    {
        result.mTuple[0] = Math<Real>::MAX_REAL;
        result.mTuple[1] = Math<Real>::MAX_REAL;
        result.mTuple[2] = Math<Real>::MAX_REAL;
        result.mTuple[3] = Math<Real>::MAX_REAL;
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real> Vector4<Real>::operator- () const
{
    return Vector4
    (
        -mTuple[0],
        -mTuple[1],
        -mTuple[2],
        -mTuple[3]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real>& Vector4<Real>::operator+= (const Vector4& vec)
{
    mTuple[0] += vec.mTuple[0];
    mTuple[1] += vec.mTuple[1];
    mTuple[2] += vec.mTuple[2];
    mTuple[3] += vec.mTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real>& Vector4<Real>::operator-= (const Vector4& vec)
{
    mTuple[0] -= vec.mTuple[0];
    mTuple[1] -= vec.mTuple[1];
    mTuple[2] -= vec.mTuple[2];
    mTuple[3] -= vec.mTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real>& Vector4<Real>::operator*= (Real scalar)
{
    mTuple[0] *= scalar;
    mTuple[1] *= scalar;
    mTuple[2] *= scalar;
    mTuple[3] *= scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real>& Vector4<Real>::operator/= (Real scalar)
{
    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        mTuple[0] *= invScalar;
        mTuple[1] *= invScalar;
        mTuple[2] *= invScalar;
        mTuple[3] *= invScalar;
    }
    else
    {
        mTuple[0] *= Math<Real>::MAX_REAL;
        mTuple[1] *= Math<Real>::MAX_REAL;
        mTuple[2] *= Math<Real>::MAX_REAL;
        mTuple[3] *= Math<Real>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::Length () const
{
    return Math<Real>::Sqrt
    (
        mTuple[0]*mTuple[0] +
        mTuple[1]*mTuple[1] +
        mTuple[2]*mTuple[2] +
        mTuple[3]*mTuple[3]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::SquaredLength () const
{
    return
        mTuple[0]*mTuple[0] +
        mTuple[1]*mTuple[1] +
        mTuple[2]*mTuple[2] +
        mTuple[3]*mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::Dot (const Vector4& vec) const
{
    return
        mTuple[0]*vec.mTuple[0] +
        mTuple[1]*vec.mTuple[1] +
        mTuple[2]*vec.mTuple[2] +
        mTuple[3]*vec.mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector4<Real>::Normalize (const Real epsilon)
{
    Real length = Length();

    if (length > epsilon)
    {
        Real invLength = ((Real)1)/length;
        mTuple[0] *= invLength;
        mTuple[1] *= invLength;
        mTuple[2] *= invLength;
        mTuple[3] *= invLength;
    }
    else
    {
        length = (Real)0;
        mTuple[0] = (Real)0;
        mTuple[1] = (Real)0;
        mTuple[2] = (Real)0;
        mTuple[3] = (Real)0;
    }

    return length;
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector4<Real>::ComputeExtremes (int numVectors, const Vector4* vectors,
    Vector4& vmin, Vector4& vmax)
{
    assertion(numVectors > 0 && vectors,
        "Invalid inputs to ComputeExtremes\n");

    vmin = vectors[0];
    vmax = vmin;
    for (int j = 1; j < numVectors; ++j)
    {
        const Vector4& vec = vectors[j];
        for (int i = 0; i < 4; ++i)
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
inline Vector4<Real> operator* (Real scalar, const Vector4<Real>& vec)
{
    return Vector4<Real>
    (
        scalar*vec[0],
        scalar*vec[1],
        scalar*vec[2],
        scalar*vec[3]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
std::ostream& operator<< (std::ostream& outFile, const Vector4<Real>& vec)
{
     return outFile << vec.X() << ' ' << vec.Y() << ' ' << vec.Z()
         << ' ' << vec.W();
}
//----------------------------------------------------------------------------
