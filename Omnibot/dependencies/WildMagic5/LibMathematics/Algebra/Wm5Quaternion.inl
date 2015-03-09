// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Quaternion ()
{
    // Uninitialized for performance in array construction.
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Quaternion (Real w, Real x, Real y, Real z)
{
    mTuple[0] = w;
    mTuple[1] = x;
    mTuple[2] = y;
    mTuple[3] = z;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Quaternion (const Quaternion& q)
{
    mTuple[0] = q.mTuple[0];
    mTuple[1] = q.mTuple[1];
    mTuple[2] = q.mTuple[2];
    mTuple[3] = q.mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Quaternion (const Matrix3<Real>& rot)
{
    FromRotationMatrix(rot);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Quaternion (const Vector3<Real>& axis, Real angle)
{
    FromAxisAngle(axis, angle);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Quaternion (const Vector3<Real> rotColumn[3])
{
    FromRotationMatrix(rotColumn);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real>::operator const Real* () const
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real>::operator Real* ()
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real& Quaternion<Real>::operator[] (int i) const
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Quaternion<Real>::operator[] (int i)
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::W () const
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Quaternion<Real>::W ()
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::X () const
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Quaternion<Real>::X ()
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::Y () const
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Quaternion<Real>::Y ()
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::Z () const
{
    return mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Quaternion<Real>::Z ()
{
    return mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real>& Quaternion<Real>::operator= (const Quaternion& q)
{
    mTuple[0] = q.mTuple[0];
    mTuple[1] = q.mTuple[1];
    mTuple[2] = q.mTuple[2];
    mTuple[3] = q.mTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline bool Quaternion<Real>::operator== (const Quaternion& q) const
{
    return memcmp(mTuple, q.mTuple, 4*sizeof(Real)) == 0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline bool Quaternion<Real>::operator!= (const Quaternion& q) const
{
    return memcmp(mTuple, q.mTuple, 4*sizeof(Real)) != 0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline bool Quaternion<Real>::operator< (const Quaternion& q) const
{
    return memcmp(mTuple, q.mTuple, 4*sizeof(Real)) < 0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline bool Quaternion<Real>::operator<= (const Quaternion& q) const
{
    return memcmp(mTuple, q.mTuple, 4*sizeof(Real)) <= 0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline bool Quaternion<Real>::operator> (const Quaternion& q) const
{
    return memcmp(mTuple, q.mTuple, 4*sizeof(Real)) > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline bool Quaternion<Real>::operator>= (const Quaternion& q) const
{
    return memcmp(mTuple, q.mTuple, 4*sizeof(Real)) >= 0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real> Quaternion<Real>::operator+ (const Quaternion& q)
    const
{
    Quaternion result;
    for (int i = 0; i < 4; ++i)
    {
        result.mTuple[i] = mTuple[i] + q.mTuple[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real> Quaternion<Real>::operator- (const Quaternion& q)
    const
{
    Quaternion result;
    for (int i = 0; i < 4; ++i)
    {
        result.mTuple[i] = mTuple[i] - q.mTuple[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real> Quaternion<Real>::operator* (const Quaternion& q)
    const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    Quaternion result;

    result.mTuple[0] =
        mTuple[0]*q.mTuple[0] -
        mTuple[1]*q.mTuple[1] -
        mTuple[2]*q.mTuple[2] -
        mTuple[3]*q.mTuple[3];

    result.mTuple[1] =
        mTuple[0]*q.mTuple[1] +
        mTuple[1]*q.mTuple[0] +
        mTuple[2]*q.mTuple[3] -
        mTuple[3]*q.mTuple[2];

    result.mTuple[2] =
        mTuple[0]*q.mTuple[2] +
        mTuple[2]*q.mTuple[0] +
        mTuple[3]*q.mTuple[1] -
        mTuple[1]*q.mTuple[3];

    result.mTuple[3] =
        mTuple[0]*q.mTuple[3] +
        mTuple[3]*q.mTuple[0] +
        mTuple[1]*q.mTuple[2] -
        mTuple[2]*q.mTuple[1];

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real> Quaternion<Real>::operator* (Real scalar) const
{
    Quaternion result;
    for (int i = 0; i < 4; ++i)
    {
        result.mTuple[i] = scalar*mTuple[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real> Quaternion<Real>::operator/ (Real scalar) const
{
    Quaternion result;
    int i;

    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        for (i = 0; i < 4; ++i)
        {
            result.mTuple[i] = invScalar*mTuple[i];
        }
    }
    else
    {
        for (i = 0; i < 4; ++i)
        {
            result.mTuple[i] = Math<Real>::MAX_REAL;
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real> Quaternion<Real>::operator- () const
{
    Quaternion result;
    for (int i = 0; i < 4; ++i)
    {
        result.mTuple[i] = -mTuple[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real>& Quaternion<Real>::operator+= (const Quaternion& q)
{
    for (int i = 0; i < 4; ++i)
    {
        mTuple[i] += q.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real>& Quaternion<Real>::operator-= (const Quaternion& q)
{
    for (int i = 0; i < 4; ++i)
    {
        mTuple[i] -= q.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real>& Quaternion<Real>::operator*= (Real scalar)
{
    for (int i = 0; i < 4; ++i)
    {
        mTuple[i] *= scalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real>& Quaternion<Real>::operator/= (Real scalar)
{
    int i;

    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        for (i = 0; i < 4; ++i)
        {
            mTuple[i] *= invScalar;
        }
    }
    else
    {
        for (i = 0; i < 4; ++i)
        {
            mTuple[i] = Math<Real>::MAX_REAL;
        }
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FromRotationMatrix (const Matrix3<Real>& rot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    const int next[3] = { 1, 2, 0 };

    Real trace = rot(0,0) + rot(1,1) + rot(2,2);
    Real root;

    if (trace > (Real)0)
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = Math<Real>::Sqrt(trace + (Real)1);  // 2w
        mTuple[0] = ((Real)0.5)*root;
        root = ((Real)0.5)/root;  // 1/(4w)
        mTuple[1] = (rot(2,1) - rot(1,2))*root;
        mTuple[2] = (rot(0,2) - rot(2,0))*root;
        mTuple[3] = (rot(1,0) - rot(0,1))*root;
    }
    else
    {
        // |w| <= 1/2
        int i = 0;
        if (rot(1,1) > rot(0,0))
        {
            i = 1;
        }
        if (rot(2,2) > rot(i,i))
        {
            i = 2;
        }
        int j = next[i];
        int k = next[j];

        root = Math<Real>::Sqrt(rot(i,i) - rot(j,j) - rot(k,k) + (Real)1);
        Real* quat[3] = { &mTuple[1], &mTuple[2], &mTuple[3] };
        *quat[i] = ((Real)0.5)*root;
        root = ((Real)0.5)/root;
        mTuple[0] = (rot(k,j) - rot(j,k))*root;
        *quat[j] = (rot(j,i) + rot(i,j))*root;
        *quat[k] = (rot(k,i) + rot(i,k))*root;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::ToRotationMatrix (Matrix3<Real>& rot) const
{
    Real twoX  = ((Real)2)*mTuple[1];
    Real twoY  = ((Real)2)*mTuple[2];
    Real twoZ  = ((Real)2)*mTuple[3];
    Real twoWX = twoX*mTuple[0];
    Real twoWY = twoY*mTuple[0];
    Real twoWZ = twoZ*mTuple[0];
    Real twoXX = twoX*mTuple[1];
    Real twoXY = twoY*mTuple[1];
    Real twoXZ = twoZ*mTuple[1];
    Real twoYY = twoY*mTuple[2];
    Real twoYZ = twoZ*mTuple[2];
    Real twoZZ = twoZ*mTuple[3];

    rot(0,0) = (Real)1 - (twoYY + twoZZ);
    rot(0,1) = twoXY - twoWZ;
    rot(0,2) = twoXZ + twoWY;
    rot(1,0) = twoXY + twoWZ;
    rot(1,1) = (Real)1 - (twoXX + twoZZ);
    rot(1,2) = twoYZ - twoWX;
    rot(2,0) = twoXZ - twoWY;
    rot(2,1) = twoYZ + twoWX;
    rot(2,2) = (Real)1 - (twoXX + twoYY);
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FromRotationMatrix (const Vector3<Real> rotColumn[3])
{
    Matrix3<Real> kRot;
    for (int col = 0; col < 3; ++col)
    {
        kRot(0,col) = rotColumn[col][0];
        kRot(1,col) = rotColumn[col][1];
        kRot(2,col) = rotColumn[col][2];
    }
    FromRotationMatrix(kRot);
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::ToRotationMatrix (Vector3<Real> rotColumn[3]) const
{
    Matrix3<Real> rot;
    ToRotationMatrix(rot);
    for (int col = 0; col < 3; ++col)
    {
        rotColumn[col][0] = rot(0,col);
        rotColumn[col][1] = rot(1,col);
        rotColumn[col][2] = rot(2,col);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FromAxisAngle (const Vector3<Real>& axis, Real angle)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real halfAngle = ((Real)0.5)*angle;
    Real sn = Math<Real>::Sin(halfAngle);
    mTuple[0] = Math<Real>::Cos(halfAngle);
    mTuple[1] = sn*axis[0];
    mTuple[2] = sn*axis[1];
    mTuple[3] = sn*axis[2];
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::ToAxisAngle (Vector3<Real>& axis, Real& angle) const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real sqrLength = mTuple[1]*mTuple[1] + mTuple[2]*mTuple[2]
        + mTuple[3]*mTuple[3];

    if (sqrLength > Math<Real>::ZERO_TOLERANCE)
    {
        angle = ((Real)2)*Math<Real>::ACos(mTuple[0]);
        Real invLength = Math<Real>::InvSqrt(sqrLength);
        axis[0] = mTuple[1]*invLength;
        axis[1] = mTuple[2]*invLength;
        axis[2] = mTuple[3]*invLength;
    }
    else
    {
        // Angle is 0 (mod 2*pi), so any axis will do.
        angle = (Real)0;
        axis[0] = (Real)1;
        axis[1] = (Real)0;
        axis[2] = (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::Length () const
{
    return Math<Real>::Sqrt(mTuple[0]*mTuple[0] + mTuple[1]*mTuple[1] +
        mTuple[2]*mTuple[2] + mTuple[3]*mTuple[3]);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::SquaredLength () const
{
    return mTuple[0]*mTuple[0] + mTuple[1]*mTuple[1] +
        mTuple[2]*mTuple[2] + mTuple[3]*mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::Dot (const Quaternion& q) const
{
    return mTuple[0]*q.mTuple[0] + mTuple[1]*q.mTuple[1] +
        mTuple[2]*q.mTuple[2] + mTuple[3]*q.mTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Quaternion<Real>::Normalize (Real epsilon)
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
Quaternion<Real> Quaternion<Real>::Inverse () const
{
    Quaternion inverse;

    Real norm = SquaredLength();
    if (norm > (Real)0)
    {
        Real invNorm = ((Real)1)/norm;
        inverse.mTuple[0] = mTuple[0]*invNorm;
        inverse.mTuple[1] = -mTuple[1]*invNorm;
        inverse.mTuple[2] = -mTuple[2]*invNorm;
        inverse.mTuple[3] = -mTuple[3]*invNorm;
    }
    else
    {
        // Return an invalid result to flag the error.
        for (int i = 0; i < 4; ++i)
        {
            inverse.mTuple[i] = (Real)0;
        }
    }

    return inverse;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::Conjugate () const
{
    return Quaternion(mTuple[0], -mTuple[1], -mTuple[2], -mTuple[3]);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::Exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    Quaternion result;

    Real angle = Math<Real>::Sqrt(mTuple[1]*mTuple[1] +
        mTuple[2]*mTuple[2] + mTuple[3]*mTuple[3]);

    Real sn = Math<Real>::Sin(angle);
    result.mTuple[0] = Math<Real>::Cos(angle);

    int i;

    if (Math<Real>::FAbs(sn) >= Math<Real>::ZERO_TOLERANCE)
    {
        Real coeff = sn/angle;
        for (i = 1; i < 4; ++i)
        {
            result.mTuple[i] = coeff*mTuple[i];
        }
    }
    else
    {
        for (i = 1; i < 4; ++i)
        {
            result.mTuple[i] = mTuple[i];
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::Log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    Quaternion result;
    result.mTuple[0] = (Real)0;

    int i;

    if (Math<Real>::FAbs(mTuple[0]) < (Real)1)
    {
        Real angle = Math<Real>::ACos(mTuple[0]);
        Real sn = Math<Real>::Sin(angle);
        if (Math<Real>::FAbs(sn) >= Math<Real>::ZERO_TOLERANCE)
        {
            Real coeff = angle/sn;
            for (i = 1; i < 4; ++i)
            {
                result.mTuple[i] = coeff*mTuple[i];
            }
            return result;
        }
    }

    for (i = 1; i < 4; ++i)
    {
        result.mTuple[i] = mTuple[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Quaternion<Real>::Rotate (const Vector3<Real>& vec) const
{
    // Given a vector u = (x0,y0,z0) and a unit length quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // rotation of u by q is v = q*u*q^{-1} where * indicates quaternion
    // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no real work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the rotation matrix computed in Quaternion<Real>::ToRotationMatrix.
    // The vector v is obtained as the product of that rotation matrix with
    // vector u.  As such, the quaternion representation of a rotation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

    Matrix3<Real> rot;
    ToRotationMatrix(rot);
    return rot*vec;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>& Quaternion<Real>::Slerp (Real t, const Quaternion& p,
    const Quaternion& q)
{
    Real cs = p.Dot(q);
    Real angle = Math<Real>::ACos(cs);

    if (Math<Real>::FAbs(angle) >= Math<Real>::ZERO_TOLERANCE)
    {
        Real sn = Math<Real>::Sin(angle);
        Real invSn = ((Real)1)/sn;
        Real tAngle = t*angle;
        Real coeff0 = Math<Real>::Sin(angle - tAngle)*invSn;
        Real coeff1 = Math<Real>::Sin(tAngle)*invSn;

        mTuple[0] = coeff0*p.mTuple[0] + coeff1*q.mTuple[0];
        mTuple[1] = coeff0*p.mTuple[1] + coeff1*q.mTuple[1];
        mTuple[2] = coeff0*p.mTuple[2] + coeff1*q.mTuple[2];
        mTuple[3] = coeff0*p.mTuple[3] + coeff1*q.mTuple[3];
    }
    else
    {
        mTuple[0] = p.mTuple[0];
        mTuple[1] = p.mTuple[1];
        mTuple[2] = p.mTuple[2];
        mTuple[3] = p.mTuple[3];
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>& Quaternion<Real>::SlerpExtraSpins (Real t,
    const Quaternion& p, const Quaternion& q, int extraSpins)
{
    Real cs = p.Dot(q);
    Real angle = Math<Real>::ACos(cs);

    if (Math<Real>::FAbs(angle) >= Math<Real>::ZERO_TOLERANCE)
    {
        Real sn = Math<Real>::Sin(angle);
        Real phase = Math<Real>::PI*extraSpins*t;
        Real invSin = ((Real)1)/sn;
        Real coeff0 = Math<Real>::Sin(((Real)1 - t)*angle - phase)*invSin;
        Real coeff1 = Math<Real>::Sin(t*angle + phase)*invSin;

        mTuple[0] = coeff0*p.mTuple[0] + coeff1*q.mTuple[0];
        mTuple[1] = coeff0*p.mTuple[1] + coeff1*q.mTuple[1];
        mTuple[2] = coeff0*p.mTuple[2] + coeff1*q.mTuple[2];
        mTuple[3] = coeff0*p.mTuple[3] + coeff1*q.mTuple[3];
    }
    else
    {
        mTuple[0] = p.mTuple[0];
        mTuple[1] = p.mTuple[1];
        mTuple[2] = p.mTuple[2];
        mTuple[3] = p.mTuple[3];
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>& Quaternion<Real>::Intermediate (const Quaternion& q0,
    const Quaternion& q1, const Quaternion& q2)
{
    // assert:  Q0, Q1, Q2 all unit-length
    Quaternion q1Inv = q1.Conjugate();
    Quaternion p0 = q1Inv*q0;
    Quaternion p2 = q1Inv*q2;
    Quaternion arg = -((Real)0.25)*(p0.Log() + p2.Log());
    Quaternion a = q1*arg.Exp();
    *this = a;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>& Quaternion<Real>::Squad (Real t, const Quaternion& q0,
    const Quaternion& a0, const Quaternion& a1, const Quaternion& q1)
{
    Real slerpT = ((Real)2)*t*((Real)1 - t);
    Quaternion slerpP = Slerp(t, q0, q1);
    Quaternion slerpQ = Slerp(t, a0, a1);
    return Slerp(slerpT, slerpP, slerpQ);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>& Quaternion<Real>::Align (const Vector3<Real>& v1,
    const Vector3<Real>& v2)
{
    // If V1 and V2 are not parallel, the axis of rotation is the unit-length
    // vector U = Cross(V1,V2)/Length(Cross(V1,V2)).  The angle of rotation,
    // A, is the angle between V1 and V2.  The quaternion for the rotation is
    // q = cos(A/2) + sin(A/2)*(ux*i+uy*j+uz*k) where U = (ux,uy,uz).
    //
    // (1) Rather than extract A = acos(Dot(V1,V2)), multiply by 1/2, then
    //     compute sin(A/2) and cos(A/2), we reduce the computational costs by
    //     computing the bisector B = (V1+V2)/Length(V1+V2), so cos(A/2) =
    //     Dot(V1,B).
    //
    // (2) The rotation axis is U = Cross(V1,B)/Length(Cross(V1,B)), but
    //     Length(Cross(V1,B)) = Length(V1)*Length(B)*sin(A/2) = sin(A/2), in
    //     which case sin(A/2)*(ux*i+uy*j+uz*k) = (cx*i+cy*j+cz*k) where
    //     C = Cross(V1,B).
    //
    // If V1 = V2, then B = V1, cos(A/2) = 1, and U = (0,0,0).  If V1 = -V2,
    // then B = 0.  This can happen even if V1 is approximately -V2 using
    // floating point arithmetic, since Vector3::Normalize checks for
    // closeness to zero and returns the zero vector accordingly.  The test
    // for exactly zero is usually not recommend for floating point
    // arithmetic, but the implementation of Vector3::Normalize guarantees
    // the comparison is robust.  In this case, the A = pi and any axis
    // perpendicular to V1 may be used as the rotation axis.

    Vector3<Real> bisector = v1 + v2;
    bisector.Normalize();

    Real cosHalfAngle = v1.Dot(bisector);
    Vector3<Real> cross;

    mTuple[0] = cosHalfAngle;

    if (cosHalfAngle != (Real)0)
    {
        cross = v1.Cross(bisector);
        mTuple[1] = cross.X();
        mTuple[2] = cross.Y();
        mTuple[3] = cross.Z();
    }
    else
    {
        Real invLength;
        if (Math<Real>::FAbs(v1[0]) >= Math<Real>::FAbs(v1[1]))
        {
            // V1.X() or V1.Z() is the largest magnitude component.
            invLength = Math<Real>::InvSqrt(v1[0]*v1[0] + v1[2]*v1[2]);
            mTuple[1] = -v1[2]*invLength;
            mTuple[2] = (Real)0;
            mTuple[3] = +v1[0]*invLength;
        }
        else
        {
            // V1.Y() or V1.Z() is the largest magnitude component.
            invLength = Math<Real>::InvSqrt(v1[1]*v1[1] + v1[2]*v1[2]);
            mTuple[1] = (Real)0;
            mTuple[2] = +v1[2]*invLength;
            mTuple[3] = -v1[1]*invLength;
        }
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::DecomposeTwistTimesSwing (const Vector3<Real>& v1,
    Quaternion& twist, Quaternion& swing)
{
    Vector3<Real> v2 = Rotate(v1);
    swing = Align(v1, v2);
    twist = (*this)*swing.Conjugate();
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::DecomposeSwingTimesTwist (const Vector3<Real>& v1,
    Quaternion& swing, Quaternion& twist)
{
    Vector3<Real> v2 = Rotate(v1);
    swing = Align(v1, v2);
    twist = swing.Conjugate()*(*this);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestX () const
{
    return GetClosest(1);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestY () const
{
    return GetClosest(2);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestZ () const
{
    return GetClosest(3);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestXY () const
{
    Quaternion q;

    Real det = mTuple[0]*mTuple[3] - mTuple[1]*mTuple[2];
    if(Math<Real>::FAbs(det) < (Real)0.5 - Math<Real>::ZERO_TOLERANCE)
    {
        Real discr = (Real)1 - ((Real)4)*det*det;
        discr = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
        Real a = mTuple[0]*mTuple[1] + mTuple[2]*mTuple[3];
        Real b = mTuple[0]*mTuple[0] - mTuple[1]*mTuple[1] +
            mTuple[2]*mTuple[2] - mTuple[3]*mTuple[3];

        Real c0, s0, c1, s1, invLength;

        if (b >= (Real)0)
        {
            c0 = ((Real)0.5)*(discr + b);
            s0 = a;
        }
        else
        {
            c0 = a;
            s0 = ((Real)0.5)*(discr - b);
        }
        invLength = Math<Real>::InvSqrt(c0*c0 + s0*s0);
        c0 *= invLength;
        s0 *= invLength;

        c1 = mTuple[0]*c0 + mTuple[1]*s0;
        s1 = mTuple[2]*c0 + mTuple[3]*s0;
        invLength = Math<Real>::InvSqrt(c1*c1 + s1*s1);
        c1 *= invLength;
        s1 *= invLength;

        q[0] = c0*c1;
        q[1] = s0*c1;
        q[2] = c0*s1;
        q[3] = s0*s1;
    }
    else
    {
        Real invLength = Math<Real>::InvSqrt(Math<Real>::FAbs(det));
        q[0] = mTuple[0]*invLength;
        q[1] = mTuple[1]*invLength;
        q[2] = (Real)0;
        q[3] = (Real)0;
    }

    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestYX () const
{
    Quaternion alt(mTuple[0], mTuple[1], mTuple[2], -mTuple[3]);
    Quaternion q = alt.GetClosestXY();
    q[3] = -q[3];
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestZX () const
{
    Quaternion alt(mTuple[0], mTuple[1], mTuple[3], mTuple[2]);
    Quaternion q = alt.GetClosestXY();
    Real save = q[2];
    q[2] = q[3];
    q[3] = save;
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestXZ () const
{
    Quaternion alt(mTuple[0], mTuple[1], -mTuple[3], mTuple[2]);
    Quaternion q = alt.GetClosestXY();
    Real save = q[2];
    q[2] = q[3];
    q[3] = -save;
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestYZ () const
{
    Quaternion alt(mTuple[0], mTuple[2], mTuple[3], mTuple[1]);
    Quaternion q = alt.GetClosestXY();
    Real save = q[3];
    q[3] = q[2];
    q[2] = q[1];
    q[1] = save;
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestZY () const
{
    Quaternion alt(mTuple[0], mTuple[2], mTuple[3], -mTuple[1]);
    Quaternion q = alt.GetClosestXY();
    Real save = q[3];
    q[3] = q[2];
    q[2] = q[1];
    q[1] = -save;
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FactorXYZ (Real& cx, Real& sx, Real& cy,
    Real& sy, Real& cz, Real& sz)
{
    Real a = mTuple[0]*mTuple[1] - mTuple[2]*mTuple[3];
    Real b = ((Real)0.5)*(
          mTuple[0]*mTuple[0]
        - mTuple[1]*mTuple[1]
        - mTuple[2]*mTuple[2]
        + mTuple[3]*mTuple[3]);

    Real fLength = Math<Real>::Sqrt(a*a + b*b);
    if (fLength > Math<Real>::ZERO_TOLERANCE)
    {
        Real invLength = ((Real)1)/fLength;
        Real sigma0 = a * invLength;
        Real gamma0 = b * invLength;
        if (gamma0 >= (Real)0)
        {
            cx = Math<Real>::Sqrt(((Real)0.5)*((Real)1 + gamma0));
            sx = ((Real)0.5)*sigma0/cx;
        }
        else
        {
            sx = Math<Real>::Sqrt(((Real)0.5)*((Real)1 - gamma0));
            cx = ((Real)0.5)*sigma0/sx;
        }

        Real tmp0 = cx*mTuple[0] + sx*mTuple[1];
        Real tmp1 = cx*mTuple[3] - sx*mTuple[2];
        invLength = Math<Real>::InvSqrt(tmp0*tmp0 + tmp1*tmp1);
        cz = tmp0 * invLength;
        sz = tmp1 * invLength;

        if(Math<Real>::FAbs(cz) >= Math<Real>::FAbs(sz))
        {
            invLength = ((Real)1)/cz;
            cy = tmp0 * invLength;
            sy = (cx*mTuple[2] + sx*mTuple[3]) * invLength;
        }
        else
        {
            invLength = ((Real)1)/sz;
            cy = tmp1 * invLength;
            sy = (cx*mTuple[1] - sx*mTuple[0]) * invLength;
        }
    }
    else
    {
        // Infinitely many solutions.  Choose one of them.
        if(mTuple[0]*mTuple[2] + mTuple[1]*mTuple[3] > (Real)0)
        {
            // p = (p0,p1,p0,p1)
            cx = (Real)1;
            sx = (Real)0;
            cy = Math<Real>::INV_SQRT_2;
            sy = Math<Real>::INV_SQRT_2;
            cz = Math<Real>::SQRT_2 * mTuple[0];
            sz = Math<Real>::SQRT_2 * mTuple[1];
        }
        else
        {
            // p = (p0,p1,-p0,-p1)
            cx = (Real)1;
            sx = (Real)0;
            cy = Math<Real>::INV_SQRT_2;
            sy = -Math<Real>::INV_SQRT_2;
            cz = Math<Real>::SQRT_2 * mTuple[0];
            sz = -Math<Real>::SQRT_2 * mTuple[1];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FactorXZY (Real& cx, Real& sx, Real& cz,
    Real& sz, Real& cy, Real& sy)
{
    Quaternion alt(mTuple[0], mTuple[1], mTuple[3], -mTuple[2]);
    alt.FactorXYZ(cx, sx, cz, sz, cy, sy);
    sy = -sy;
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FactorYZX (Real& cy, Real& sy, Real& cz,
    Real& sz, Real& cx, Real& sx)
{
    Quaternion alt(mTuple[0], -mTuple[2], mTuple[3], -mTuple[1]);
    alt.FactorXYZ(cy, sy, cz, sz, cx, sx);
    sx = -sx;
    sy = -sy;
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FactorYXZ (Real& cy, Real& sy, Real& cx,
    Real& sx, Real& cz, Real& sz)
{
    Quaternion alt(mTuple[0], -mTuple[2], mTuple[1], mTuple[3]);
    alt.FactorXYZ(cy, sy, cx, sx, cz, sz);
    sy = -sy;
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FactorZXY (Real& cz, Real& sz, Real& cx,
    Real& sx, Real& cy, Real& sy)
{
    Quaternion alt(mTuple[0], -mTuple[3], mTuple[1], -mTuple[2]);
    alt.FactorXYZ(cz, sz, cx, sx, cy, sy);
    sy = -sy;
    sz = -sz;
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::FactorZYX (Real& cz, Real& sz, Real& cy,
    Real& sy, Real& cx, Real& sx)
{
    Quaternion alt(mTuple[0], mTuple[3], -mTuple[2], mTuple[1]);
    alt.FactorXYZ(cz, sz, cy, sy, cx, sx);
    sy = -sy;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestX (const Constraints& xCon) const
{
    return GetClosest(1, xCon);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestY (const Constraints& yCon) const
{
    return GetClosest(2, yCon);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestZ (const Constraints& zCon) const
{
    return GetClosest(3, zCon);
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestXY (const Constraints& xCon,
    const Constraints& yCon) const
{
    Quaternion q, tmp;
    Real c0, s0, c1, s1, invLength;

    Real det = mTuple[0]*mTuple[3] - mTuple[1]*mTuple[2];
    if (Math<Real>::FAbs(det) < (Real)0.5 - Math<Real>::ZERO_TOLERANCE)
    {
        Real discr = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 -
            ((Real)4)*det*det));
        Real a = mTuple[0]*mTuple[1] + mTuple[2]*mTuple[3];
        Real b = mTuple[0]*mTuple[0] - mTuple[1]*mTuple[1]
            + mTuple[2]*mTuple[2] - mTuple[3]*mTuple[3];

        if (b >= (Real)0)
        {
            c0 = ((Real)0.5)*(discr + b);
            s0 = a;
        }
        else
        {
            c0 = a;
            s0 = ((Real)0.5)*(discr - b);
        }
        invLength = Math<Real>::InvSqrt(c0*c0 + s0*s0);
        c0 *= invLength;
        s0 *= invLength;

        c1 = mTuple[0]*c0 + mTuple[1]*s0;
        s1 = mTuple[2]*c0 + mTuple[3]*s0;
        invLength = Math<Real>::InvSqrt(c1*c1 + s1*s1);
        c1 *= invLength;
        s1 *= invLength;

        if (xCon.IsValid(c0, s0) && yCon.IsValid(c1, s1))
        {
            // The maximum occurs at an interior point.
            q[0] = c0*c1;
            q[1] = s0*c1;
            q[2] = c0*s1;
            q[3] = s0*s1;
        }
        else
        {
            // The maximum occurs at a boundary point.
            Quaternion r(xCon.CosMinAngle, xCon.SinMinAngle,
                (Real)0, (Real)0);
            Quaternion rInv(xCon.CosMinAngle, -xCon.SinMinAngle,
                (Real)0, (Real)0);
            Quaternion prod = rInv*(*this);
            tmp = prod.GetClosest(2, yCon);
            Real dotOptAngle = prod.Dot(tmp);
            q = r*tmp;

            r = Quaternion(xCon.CosMaxAngle, xCon.SinMaxAngle,
                (Real)0, (Real)0);
            rInv = Quaternion(xCon.CosMaxAngle, -xCon.SinMaxAngle,
                (Real)0, (Real)0);
            prod = rInv*(*this);
            tmp = prod.GetClosest(2, yCon);
            Real dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = r*tmp;
                dotOptAngle = dotAngle;
            }

            r = Quaternion(yCon.CosMinAngle, (Real)0,
                yCon.SinMinAngle, (Real)0);
            rInv = Quaternion(yCon.CosMinAngle, (Real)0,
                -yCon.SinMinAngle, (Real)0);
            prod = (*this)*rInv;
            tmp = prod.GetClosest(1, xCon);
            dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = tmp*r;
                dotOptAngle = dotAngle;
            }

            r = Quaternion(yCon.CosMaxAngle, (Real)0,
                yCon.SinMaxAngle, (Real)0);
            rInv = Quaternion(yCon.CosMaxAngle, (Real)0,
                -yCon.SinMaxAngle, (Real)0);
            prod = (*this)*rInv;
            tmp = prod.GetClosest(1, xCon);
            dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = tmp*r;
                dotOptAngle = dotAngle;
            }
        }
    }
    else
    {
        // Infinitely many solutions, choose one that satisfies the angle
        // constraints.
        Real minAngle, maxAngle, angle;
        Constraints con;

        if (det > (Real)0)
        {
            minAngle = xCon.MinAngle - yCon.MaxAngle;
            maxAngle = xCon.MaxAngle - yCon.MinAngle;
            con.SetAngles(minAngle, maxAngle);

            tmp = GetClosest(1, con);

            angle = Math<Real>::ATan2(tmp[1], tmp[0]);
            if (angle < minAngle || angle > maxAngle)
            {
                angle -= (tmp[1] >= (Real)0 ? Math<Real>::PI :
                    -Math<Real>::PI);
                // assert(minAngle <= angle && angle <= maxAngle);
            }

            if (angle <= xCon.MaxAngle - yCon.MaxAngle)
            {
                c1 = yCon.CosMaxAngle;
                s1 = yCon.SinMaxAngle;
                angle = yCon.MaxAngle + angle;
                c0 = Math<Real>::Cos(angle);
                s0 = Math<Real>::Sin(angle);
            }
            else
            {
                c0 = xCon.CosMaxAngle;
                s0 = xCon.SinMaxAngle;
                angle = xCon.MaxAngle - angle;
                c1 = Math<Real>::Cos(angle);
                s1 = Math<Real>::Sin(angle);
            }
        }
        else
        {
            minAngle = xCon.MinAngle + yCon.MinAngle;
            maxAngle = xCon.MaxAngle + yCon.MaxAngle;
            con.SetAngles(minAngle, maxAngle);

            tmp = GetClosest(1, con);

            angle = Math<Real>::ATan2(tmp[1], tmp[0]);
            if (angle < minAngle || angle > maxAngle)
            {
                angle -= (tmp[1] >= (Real)0 ? Math<Real>::PI :
                    -Math<Real>::PI);
                // assert(minAngle <= angle && angle <= maxAngle);
            }

            if (angle >= xCon.MinAngle + yCon.MaxAngle)
            {
                c1 = yCon.CosMaxAngle;
                s1 = yCon.SinMaxAngle;
                angle = angle - yCon.MaxAngle;
                c0 = Math<Real>::Cos(angle);
                s0 = Math<Real>::Sin(angle);
            }
            else
            {
                c0 = xCon.CosMaxAngle;
                s0 = xCon.SinMaxAngle;
                angle = angle - xCon.MaxAngle;
                c1 = Math<Real>::Cos(angle);
                s1 = Math<Real>::Sin(angle);
            }
        }

        q[0] = c0*c1;
        q[1] = s0*c1;
        q[2] = c0*s1;
        q[3] = s0*s1;
        if (Dot(q) < (Real)0)
        {
            q = -q;
        }
    }

    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestYX (const Constraints& yCon,
    const Constraints& xCon) const
{
    Quaternion alt(mTuple[0], mTuple[1], mTuple[2], -mTuple[3]);
    Quaternion q = alt.GetClosestXY(xCon, yCon);
    q[3] = -q[3];
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestZX (const Constraints& zCon,
    const Constraints& xCon) const
{
    Quaternion q, tmp;
    Real c2, s2, c0, s0, invLength;

    Real det = mTuple[0]*mTuple[2] - mTuple[1]*mTuple[3];
    if (Math<Real>::FAbs(det) < (Real)0.5 - Math<Real>::ZERO_TOLERANCE)
    {
        Real discr = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 -
            ((Real)4)*det*det));
        Real a = mTuple[0]*mTuple[3] + mTuple[1]*mTuple[2];
        Real b = mTuple[0]*mTuple[0] + mTuple[1]*mTuple[1]
            - mTuple[2]*mTuple[2] - mTuple[3]*mTuple[3];

        if (b >= (Real)0)
        {
            c2 = ((Real)0.5)*(discr + b);
            s2 = a;
        }
        else
        {
            c2 = a;
            s2 = ((Real)0.5)*(discr - b);
        }
        invLength = Math<Real>::InvSqrt(c2*c2 + s2*s2);
        c2 *= invLength;
        s2 *= invLength;

        c0 = mTuple[0]*c2 + mTuple[3]*s2;
        s0 = mTuple[1]*c2 + mTuple[2]*s2;
        invLength = Math<Real>::InvSqrt(c0*c0 + s0*s0);
        c0 *= invLength;
        s0 *= invLength;

        if (zCon.IsValid(c2, s2) && xCon.IsValid(c0, s0))
        {
            // The maximum occurs at an interior point.
            q[0] = c2*c0;
            q[1] = c2*s0;
            q[2] = s2*s0;
            q[3] = s2*c0;
        }
        else
        {
            // The maximum occurs at a boundary point.
            Quaternion r(zCon.CosMinAngle, (Real)0, (Real)0,
                zCon.SinMinAngle);
            Quaternion rInv(zCon.CosMinAngle, (Real)0, (Real)0,
                -zCon.SinMinAngle);
            Quaternion prod = rInv*(*this);
            tmp = prod.GetClosest(1, xCon);
            Real dotOptAngle = prod.Dot(tmp);
            q = r*tmp;

            r = Quaternion(zCon.CosMaxAngle, (Real)0, (Real)0,
                zCon.SinMaxAngle);
            rInv = Quaternion(zCon.CosMaxAngle, (Real)0, (Real)0,
                -zCon.SinMaxAngle);
            prod = rInv*(*this);
            tmp = prod.GetClosest(1, xCon);
            Real dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = r*tmp;
                dotOptAngle = dotAngle;
            }

            r = Quaternion(xCon.CosMinAngle, xCon.SinMinAngle,
                (Real)0, (Real)0);
            rInv = Quaternion(xCon.CosMinAngle, -xCon.SinMinAngle,
                (Real)0, (Real)0);
            prod = (*this)*rInv;
            tmp = prod.GetClosest(3, zCon);
            dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = tmp*r;
                dotOptAngle = dotAngle;
            }

            r = Quaternion(xCon.CosMaxAngle, xCon.SinMaxAngle,
                (Real)0, (Real)0);
            rInv = Quaternion(xCon.CosMaxAngle, -xCon.SinMaxAngle,
                (Real)0, (Real)0);
            prod = (*this)*rInv;
            tmp = prod.GetClosest(3, zCon);
            dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = tmp*r;
                dotOptAngle = dotAngle;
            }
        }
    }
    else
    {
        // Infinitely many solutions, choose one that satisfies the angle
        // constraints.
        Real minAngle, maxAngle, angle;
        Constraints con;

        if (det > (Real)0)
        {
            minAngle = xCon.MinAngle - zCon.MaxAngle;
            maxAngle = xCon.MaxAngle - zCon.MinAngle;
            con.SetAngles(minAngle, maxAngle);

            tmp = GetClosest(1, con);

            angle = Math<Real>::ATan2(tmp[1], tmp[0]);
            if (angle < minAngle || angle > maxAngle)
            {
                angle -= (tmp[1] >= (Real)0 ? Math<Real>::PI :
                    -Math<Real>::PI);
                // assert(minAngle <= angle && angle <= maxAngle);
            }

            if (angle <= xCon.MaxAngle - zCon.MaxAngle)
            {
                c2 = zCon.CosMaxAngle;
                s2 = zCon.SinMaxAngle;
                angle = zCon.MaxAngle + angle;
                c0 = Math<Real>::Cos(angle);
                s0 = Math<Real>::Sin(angle);
            }
            else
            {
                c0 = xCon.CosMaxAngle;
                s0 = xCon.SinMaxAngle;
                angle = xCon.MaxAngle - angle;
                c2 = Math<Real>::Cos(angle);
                s2 = Math<Real>::Sin(angle);
            }
        }
        else
        {
            minAngle = xCon.MinAngle + zCon.MinAngle;
            maxAngle = xCon.MaxAngle + zCon.MaxAngle;
            con.SetAngles(minAngle, maxAngle);

            tmp = GetClosest(1, con);

            angle = Math<Real>::ATan2(tmp[1], tmp[0]);
            if (angle < minAngle || angle > maxAngle)
            {
                angle -= (tmp[1] >= (Real)0 ? Math<Real>::PI :
                    -Math<Real>::PI);
                // assert(minAngle <= angle && angle <= maxAngle);
            }

            if (angle >= xCon.MinAngle + zCon.MaxAngle)
            {
                c2 = zCon.CosMaxAngle;
                s2 = zCon.SinMaxAngle;
                angle = angle - zCon.MaxAngle;
                c0 = Math<Real>::Cos(angle);
                s0 = Math<Real>::Sin(angle);
            }
            else
            {
                c0 = xCon.CosMaxAngle;
                s0 = xCon.SinMaxAngle;
                angle = angle - xCon.MaxAngle;
                c2 = Math<Real>::Cos(angle);
                s2 = Math<Real>::Sin(angle);
            }
        }

        q[0] = c2*c0;
        q[1] = c2*s0;
        q[2] = s2*s0;
        q[3] = s2*c0;
        if (Dot(q) < (Real)0)
        {
            q = -q;
        }
    }

    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestXZ (const Constraints& xCon,
    const Constraints& zCon) const
{
    Quaternion alt(mTuple[0], mTuple[1], -mTuple[2], mTuple[3]);
    Quaternion q = alt.GetClosestZX(zCon,xCon);
    q[2] = -q[2];
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestZY (const Constraints& zCon,
    const Constraints& yCon) const
{
    Quaternion q, tmp;
    Real c2, s2, c1, s1, invLength;

    Real det = mTuple[0]*mTuple[1] + mTuple[2]*mTuple[3];
    if (Math<Real>::FAbs(det) < (Real)0.5 - Math<Real>::ZERO_TOLERANCE)
    {
        Real discr = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 -
            ((Real)4)*det*det));
        Real a = mTuple[0]*mTuple[3] - mTuple[1]*mTuple[2];
        Real b = mTuple[0]*mTuple[0] - mTuple[1]*mTuple[1]
            + mTuple[2]*mTuple[2] - mTuple[3]*mTuple[3];

        if (b >= (Real)0)
        {
            c2 = ((Real)0.5)*(discr + b);
            s2 = a;
        }
        else
        {
            c2 = a;
            s2 = ((Real)0.5)*(discr - b);
        }
        invLength = Math<Real>::InvSqrt(c2*c2 + s2*s2);
        c2 *= invLength;
        s2 *= invLength;

        c1 = mTuple[0]*c2 + mTuple[3]*s2;
        s1 = mTuple[2]*c2 - mTuple[1]*s2;
        invLength = Math<Real>::InvSqrt(c1*c1 + s1*s1);
        c1 *= invLength;
        s1 *= invLength;

        if (zCon.IsValid(c2, s2) && yCon.IsValid(c1, s1))
        {
            // The maximum occurs at an interior point.
            q[0] = c2*c1;
            q[1] = -s2*s1;
            q[2] = c2*s1;
            q[3] = s2*c1;
        }
        else
        {
            // The maximum occurs at a boundary point.
            Quaternion r(zCon.CosMinAngle, (Real)0, (Real)0,
                zCon.SinMinAngle);
            Quaternion rInv(zCon.CosMinAngle, (Real)0, (Real)0,
                -zCon.SinMinAngle);
            Quaternion prod = rInv*(*this);
            tmp = prod.GetClosest(2, yCon);
            Real dotOptAngle = prod.Dot(tmp);
            q = r*tmp;

            r = Quaternion(zCon.CosMaxAngle, (Real)0, (Real)0,
                zCon.SinMaxAngle);
            rInv = Quaternion(zCon.CosMaxAngle, (Real)0, (Real)0,
                -zCon.SinMaxAngle);
            prod = rInv*(*this);
            tmp = prod.GetClosest(2, yCon);
            Real dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = r*tmp;
                dotOptAngle = dotAngle;
            }

            r = Quaternion(yCon.CosMinAngle, (Real)0,
                yCon.SinMinAngle, (Real)0);
            rInv = Quaternion(yCon.CosMinAngle, (Real)0,
                -yCon.SinMinAngle, (Real)0);
            prod = (*this)*rInv;
            tmp = prod.GetClosest(3, zCon);
            dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = tmp*r;
                dotOptAngle = dotAngle;
            }

            r = Quaternion(yCon.CosMaxAngle, (Real)0,
                yCon.SinMaxAngle, (Real)0);
            rInv = Quaternion(yCon.CosMaxAngle, (Real)0,
                -yCon.SinMaxAngle, (Real)0);
            prod = (*this)*rInv;
            tmp = prod.GetClosest(3, zCon);
            dotAngle = prod.Dot(tmp);
            if (dotAngle > dotOptAngle)
            {
                q = tmp*r;
                dotOptAngle = dotAngle;
            }
        }
    }
    else
    {
        // Infinitely many solutions, choose one that satisfies the angle
        // constraints.
        Real minAngle, maxAngle, angle;
        Constraints con;

        if (det < (Real)0)
        {
            minAngle = yCon.MinAngle - zCon.MaxAngle;
            maxAngle = yCon.MaxAngle - zCon.MinAngle;
            con.SetAngles(minAngle, maxAngle);

            tmp = GetClosest(2, con);

            angle = Math<Real>::ATan2(tmp[2], tmp[0]);
            if (angle < minAngle || angle > maxAngle)
            {
                angle -= (tmp[2] >= (Real)0 ? Math<Real>::PI :
                    -Math<Real>::PI);
                // assert(minAngle <= angle && angle <= maxAngle);
            }

            if (angle <= yCon.MaxAngle - zCon.MaxAngle)
            {
                c2 = zCon.CosMaxAngle;
                s2 = zCon.SinMaxAngle;
                angle = zCon.MaxAngle + angle;
                c1 = Math<Real>::Cos(angle);
                s1 = Math<Real>::Sin(angle);
            }
            else
            {
                c1 = yCon.CosMaxAngle;
                s1 = yCon.SinMaxAngle;
                angle = yCon.MaxAngle - angle;
                c2 = Math<Real>::Cos(angle);
                s2 = Math<Real>::Sin(angle);
            }
        }
        else
        {
            minAngle = yCon.MinAngle + zCon.MinAngle;
            maxAngle = yCon.MaxAngle + zCon.MaxAngle;
            con.SetAngles(minAngle, maxAngle);

            tmp = GetClosest(2, con);

            angle = Math<Real>::ATan2(tmp[2], tmp[0]);
            if (angle < minAngle || angle > maxAngle)
            {
                angle -= (tmp[2] >= (Real)0 ? Math<Real>::PI :
                    -Math<Real>::PI);
                // assert(minAngle <= angle && angle <= maxAngle);
            }

            if (angle >= yCon.MinAngle + zCon.MaxAngle)
            {
                c2 = zCon.CosMaxAngle;
                s2 = zCon.SinMaxAngle;
                angle = angle - zCon.MaxAngle;
                c1 = Math<Real>::Cos(angle);
                s1 = Math<Real>::Sin(angle);
            }
            else
            {
                c1 = yCon.CosMaxAngle;
                s1 = yCon.SinMaxAngle;
                angle = angle - yCon.MaxAngle;
                c2 = Math<Real>::Cos(angle);
                s2 = Math<Real>::Sin(angle);
            }
        }

        q[0] = c2*c1;
        q[1] = -s2*s1;
        q[2] = c2*s1;
        q[3] = s2*c1;
        if (Dot(q) < (Real)0)
        {
            q = -q;
        }
    }

    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosestYZ (const Constraints& yCon,
    const Constraints& zCon) const
{
    Quaternion alt(mTuple[0], -mTuple[1], mTuple[2], mTuple[3]);
    Quaternion q = alt.GetClosestZY(zCon, yCon);
    q[1] = -q[1];
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosest (int axis) const
{
    // The appropriate nonzero components will be set later.
    Quaternion q((Real)0, (Real)0, (Real)0, (Real)0);
    Real p0 = mTuple[0];
    Real p1 = mTuple[axis];
    Real sqrLength = p0*p0 + p1*p1;
    if (sqrLength > Math<Real>::ZERO_TOLERANCE)
    {
        // A unique closest point.
        Real invLength = Math<Real>::InvSqrt(sqrLength);
        q[0] = p0*invLength;
        q[axis] = p1*invLength;
    }
    else
    {
        // Infinitely many solutions, choose the one for theta = 0.
        q[0] = (Real)1;
        q[axis] = (Real)0;
    }
    return q;
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::GetClosest (int axis,
    const Constraints& con) const
{
    Quaternion q((Real)0, (Real)0, (Real)0, (Real)0);

    Real p0 = mTuple[0];
    Real p1 = mTuple[axis];
    Real sqrLength = p0*p0 + p1*p1;
    if (sqrLength > Math<Real>::ZERO_TOLERANCE)
    {
        Real invLength = Math<Real>::InvSqrt(sqrLength);
        p0 *= invLength;
        p1 *= invLength;
        if (con.IsValid(p0, p1))
        {
            // The maximum occurs at an interior point.
            q[0] = p0;
            q[axis] = p1;
        }
        else
        {
            // The maximum occurs at a boundary point.
            Real csMin = con.CosMinAngle;
            Real snMin = con.SinMinAngle;
            Real dotMinAngle = p0*csMin + p1*snMin;
            if (dotMinAngle < (Real)0)
            {
                csMin = -csMin;
                snMin = -snMin;
                dotMinAngle = -dotMinAngle;
            }

            Real csMax = con.CosMaxAngle;
            Real snMax = con.SinMaxAngle;
            Real dotMaxAngle = p0*csMax + p1*snMax;
            if (dotMaxAngle < (Real)0)
            {
                csMax = -csMax;
                snMax = -snMax;
                dotMaxAngle = -dotMaxAngle;
            }

            if (dotMaxAngle >= dotMinAngle)
            {
                q[0] = csMax;
                q[axis] = snMax;
            }
            else
            {
                q[0] = csMin;
                q[axis] = snMin;
            }
        }
    }
    else
    {
        // Infinitely many solutions, choose one that satisfies the angle
        // constraints.
        q[0] = con.CosAvrAngle;
        q[axis] = con.SinAvrAngle;
    }

    return q;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Quaternion::Constraints
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Constraints::Constraints ()
{
    // Members are uninitialized.
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Constraints::Constraints (Real minAngle, Real maxAngle)
{
    SetAngles(minAngle, maxAngle);
}
//----------------------------------------------------------------------------
template <typename Real>
void Quaternion<Real>::Constraints::SetAngles (Real minAngle, Real maxAngle)
{
    MinAngle = minAngle;
    MaxAngle = maxAngle;
    CosMinAngle = Math<Real>::Cos(MinAngle);
    SinMinAngle = Math<Real>::Sin(MinAngle);
    CosMaxAngle = Math<Real>::Cos(MaxAngle);
    SinMaxAngle = Math<Real>::Sin(MaxAngle);
    DiffCosMaxMin = CosMaxAngle - CosMinAngle;
    DiffSinMaxMin = SinMaxAngle - SinMinAngle;
    Real avrAngle = ((Real)0.5)*(MinAngle + MaxAngle);
    CosAvrAngle = Math<Real>::Cos(avrAngle);
    SinAvrAngle = Math<Real>::Sin(avrAngle);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Quaternion<Real>::Constraints::IsValid (Real x, Real y) const
{
    // (x,y) must be unit-length.

    // Test whether (x,y) satisfies the constraints.
    Real xm = x - CosMinAngle;
    Real ym = y - SinMinAngle;
    if (xm*DiffSinMaxMin >= ym*DiffCosMaxMin)
    {
        return true;
    }

    // Test whether (-x,-y) satisfies the constraints.
    Real xp = x + CosMinAngle;
    Real yp = y + SinMinAngle;
    if (xp*DiffSinMaxMin <= yp*DiffCosMaxMin)
    {
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
