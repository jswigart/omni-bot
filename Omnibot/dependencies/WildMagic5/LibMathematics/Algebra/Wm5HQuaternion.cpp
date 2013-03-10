// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5HQuaternion.h"
using namespace Wm5;

const HQuaternion HQuaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const HQuaternion HQuaternion::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f);

//----------------------------------------------------------------------------
HQuaternion::HQuaternion ()
{
}
//----------------------------------------------------------------------------
HQuaternion::HQuaternion (float w, float x, float y, float z)
{
	mTuple[0] = w;
	mTuple[1] = x;
	mTuple[2] = y;
	mTuple[3] = z;
}
//----------------------------------------------------------------------------
HQuaternion::HQuaternion (const HQuaternion& q)
{
	mTuple[0] = q.mTuple[0];
	mTuple[1] = q.mTuple[1];
	mTuple[2] = q.mTuple[2];
	mTuple[3] = q.mTuple[3];
}
//----------------------------------------------------------------------------
HQuaternion::HQuaternion (const HMatrix& rot)
{
	FromRotationMatrix(rot);
}
//----------------------------------------------------------------------------
HQuaternion::HQuaternion (const AVector& axis, float angle)
{
	FromAxisAngle(axis, angle);
}
//----------------------------------------------------------------------------
HQuaternion& HQuaternion::operator= (const HQuaternion& q)
{
	mTuple[0] = q.mTuple[0];
	mTuple[1] = q.mTuple[1];
	mTuple[2] = q.mTuple[2];
	mTuple[3] = q.mTuple[3];
	return *this;
}
//----------------------------------------------------------------------------
bool HQuaternion::operator== (const HQuaternion& q) const
{
	return memcmp(mTuple, q.mTuple, 4*sizeof(float)) == 0;
}
//----------------------------------------------------------------------------
bool HQuaternion::operator!= (const HQuaternion& q) const
{
	return memcmp(mTuple, q.mTuple, 4*sizeof(float)) != 0;
}
//----------------------------------------------------------------------------
bool HQuaternion::operator< (const HQuaternion& q) const
{
	return memcmp(mTuple, q.mTuple, 4*sizeof(float)) < 0;
}
//----------------------------------------------------------------------------
bool HQuaternion::operator<= (const HQuaternion& q) const
{
	return memcmp(mTuple, q.mTuple, 4*sizeof(float)) <= 0;
}
//----------------------------------------------------------------------------
bool HQuaternion::operator> (const HQuaternion& q) const
{
	return memcmp(mTuple, q.mTuple, 4*sizeof(float)) > 0;
}
//----------------------------------------------------------------------------
bool HQuaternion::operator>= (const HQuaternion& q) const
{
	return memcmp(mTuple, q.mTuple, 4*sizeof(float)) >= 0;
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::operator+ (const HQuaternion& q) const
{
	HQuaternion result;
	for (int i = 0; i < 4; ++i)
	{
		result.mTuple[i] = mTuple[i] + q.mTuple[i];
	}
	return result;
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::operator- (const HQuaternion& q) const
{
	HQuaternion result;
	for (int i = 0; i < 4; ++i)
	{
		result.mTuple[i] = mTuple[i] - q.mTuple[i];
	}
	return result;
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::operator* (const HQuaternion& q) const
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.

	HQuaternion result;

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
HQuaternion HQuaternion::operator* (float scalar) const
{
	HQuaternion result;
	for (int i = 0; i < 4; ++i)
	{
		result.mTuple[i] = scalar*mTuple[i];
	}
	return result;
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::operator/ (float scalar) const
{
	HQuaternion result;
	int i;

	if (scalar != 0.0f)
	{
		float invScalar = 1.0f/scalar;
		for (i = 0; i < 4; ++i)
		{
			result.mTuple[i] = invScalar*mTuple[i];
		}
	}
	else
	{
		for (i = 0; i < 4; ++i)
		{
			result.mTuple[i] = FLT_MAX;
		}
	}

	return result;
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::operator- () const
{
	HQuaternion result;
	for (int i = 0; i < 4; ++i)
	{
		result.mTuple[i] = -mTuple[i];
	}
	return result;
}
//----------------------------------------------------------------------------
HQuaternion Wm5::operator* (float scalar, const HQuaternion& q)
{
	return q*scalar;
}
//----------------------------------------------------------------------------
HQuaternion& HQuaternion::operator+= (const HQuaternion& q)
{
	for (int i = 0; i < 4; ++i)
	{
		mTuple[i] += q.mTuple[i];
	}
	return *this;
}
//----------------------------------------------------------------------------
HQuaternion& HQuaternion::operator-= (const HQuaternion& q)
{
	for (int i = 0; i < 4; ++i)
	{
		mTuple[i] -= q.mTuple[i];
	}
	return *this;
}
//----------------------------------------------------------------------------
HQuaternion& HQuaternion::operator*= (float scalar)
{
	for (int i = 0; i < 4; ++i)
	{
		mTuple[i] *= scalar;
	}
	return *this;
}
//----------------------------------------------------------------------------
HQuaternion& HQuaternion::operator/= (float scalar)
{
	int i;

	if (scalar != 0.0f)
	{
		float invScalar = 1.0f/scalar;
		for (i = 0; i < 4; ++i)
		{
			mTuple[i] *= invScalar;
		}
	}
	else
	{
		for (i = 0; i < 4; ++i)
		{
			mTuple[i] = FLT_MAX;
		}
	}

	return *this;
}
//----------------------------------------------------------------------------
void HQuaternion::FromRotationMatrix (const HMatrix& rot)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "HQuaternion Calculus and Fast Animation".

	const int next[3] = { 1, 2, 0 };

	float trace = rot(0,0) + rot(1,1) + rot(2,2);
	float root;

	if (trace > (float)0)
	{
		// |w| > 1/2, may as well choose w > 1/2
		root = sqrtf(trace + 1.0f);  // 2w
		mTuple[0] = 0.5f*root;
		root = 0.5f/root;  // 1/(4w)
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

		root = sqrtf(rot(i,i) - rot(j,j) - rot(k,k) + (float)1);
		float* quat[3] = { &mTuple[1], &mTuple[2], &mTuple[3] };
		*quat[i] = 0.5f*root;
		root = 0.5f/root;
		mTuple[0] = (rot(k,j) - rot(j,k))*root;
		*quat[j] = (rot(j,i) + rot(i,j))*root;
		*quat[k] = (rot(k,i) + rot(i,k))*root;
	}
}
//----------------------------------------------------------------------------
void HQuaternion::ToRotationMatrix (HMatrix& rot) const
{
	float twoX  = 2.0f*mTuple[1];
	float twoY  = 2.0f*mTuple[2];
	float twoZ  = 2.0f*mTuple[3];
	float twoWX = twoX*mTuple[0];
	float twoWY = twoY*mTuple[0];
	float twoWZ = twoZ*mTuple[0];
	float twoXX = twoX*mTuple[1];
	float twoXY = twoY*mTuple[1];
	float twoXZ = twoZ*mTuple[1];
	float twoYY = twoY*mTuple[2];
	float twoYZ = twoZ*mTuple[2];
	float twoZZ = twoZ*mTuple[3];

	rot(0,0) = 1.0f - (twoYY + twoZZ);
	rot(0,1) = twoXY - twoWZ;
	rot(0,2) = twoXZ + twoWY;
	rot(0,3) = 0.0f;
	rot(1,0) = twoXY + twoWZ;
	rot(1,1) = 1.0f - (twoXX + twoZZ);
	rot(1,2) = twoYZ - twoWX;
	rot(1,3) = 0.0f;
	rot(2,0) = twoXZ - twoWY;
	rot(2,1) = twoYZ + twoWX;
	rot(2,2) = 1.0f - (twoXX + twoYY);
	rot(2,3) = 0.0f;
	rot(3,0) = 0.0f;
	rot(3,1) = 0.0f;
	rot(3,2) = 0.0f;
	rot(3,3) = 1.0f;
}
//----------------------------------------------------------------------------
void HQuaternion::FromAxisAngle (const AVector& axis, float angle)
{
	// assert:  axis[] is unit length
	//
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	float halfAngle = 0.5f*angle;
	float sn = sinf(halfAngle);
	mTuple[0] = cosf(halfAngle);
	mTuple[1] = sn*axis[0];
	mTuple[2] = sn*axis[1];
	mTuple[3] = sn*axis[2];
}
//----------------------------------------------------------------------------
void HQuaternion::ToAxisAngle (AVector& axis, float& angle) const
{
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	float sqrLength = mTuple[1]*mTuple[1] + mTuple[2]*mTuple[2]
	+ mTuple[3]*mTuple[3];

	if (sqrLength > 0.0f)
	{
		angle = 2.0f*acosf(mTuple[0]);
		float invLength = 1.0f/sqrtf(sqrLength);
		axis[0] = mTuple[1]*invLength;
		axis[1] = mTuple[2]*invLength;
		axis[2] = mTuple[3]*invLength;
	}
	else
	{
		// Angle is 0 (mod 2*pi), so any axis will do.
		angle = 0.0f;
		axis[0] = 1.0f;
		axis[1] = 0.0f;
		axis[2] = 0.0f;
	}
}
//----------------------------------------------------------------------------
float HQuaternion::Length () const
{
	return sqrtf(mTuple[0]*mTuple[0] + mTuple[1]*mTuple[1] +
		mTuple[2]*mTuple[2] + mTuple[3]*mTuple[3]);
}
//----------------------------------------------------------------------------
float HQuaternion::SquaredLength () const
{
	return mTuple[0]*mTuple[0] + mTuple[1]*mTuple[1] +
		mTuple[2]*mTuple[2] + mTuple[3]*mTuple[3];
}
//----------------------------------------------------------------------------
float HQuaternion::Dot (const HQuaternion& q) const
{
	return mTuple[0]*q.mTuple[0] + mTuple[1]*q.mTuple[1] +
		mTuple[2]*q.mTuple[2] + mTuple[3]*q.mTuple[3];
}
//----------------------------------------------------------------------------
float HQuaternion::Normalize (const float epsilon)
{
	float length = Length();

	if (length > epsilon)
	{
		float invLength = 1.0f/length;
		mTuple[0] *= invLength;
		mTuple[1] *= invLength;
		mTuple[2] *= invLength;
		mTuple[3] *= invLength;
	}
	else
	{
		length = 0.0f;
		mTuple[0] = 0.0f;
		mTuple[1] = 0.0f;
		mTuple[2] = 0.0f;
		mTuple[3] = 0.0f;
	}

	return length;
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::Inverse () const
{
	HQuaternion inverse;

	float norm = SquaredLength();
	if (norm > 0.0f)
	{
		float invNorm = 1.0f/norm;
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
			inverse.mTuple[i] = 0.0f;
		}
	}

	return inverse;
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::Conjugate () const
{
	return HQuaternion(mTuple[0], -mTuple[1], -mTuple[2], -mTuple[3]);
}
//----------------------------------------------------------------------------
HQuaternion HQuaternion::Exp () const
{
	// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
	// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

	HQuaternion result;

	float angle = sqrtf(mTuple[1]*mTuple[1] +
		mTuple[2]*mTuple[2] + mTuple[3]*mTuple[3]);

	float sn = sinf(angle);
	result.mTuple[0] = cosf(angle);

	int i;

	if (fabsf(sn) > 0.0f)
	{
		float coeff = sn/angle;
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
HQuaternion HQuaternion::Log () const
{
	// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
	// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

	HQuaternion result;
	result.mTuple[0] = 0.0f;

	int i;

	if (fabsf(mTuple[0]) < 1.0f)
	{
		float angle = acosf(mTuple[0]);
		float sn = sinf(angle);
		if (fabsf(sn) > 0.0f)
		{
			float coeff = angle/sn;
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
AVector HQuaternion::Rotate (const AVector& vec) const
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
	// of the rotation matrix computed in HQuaternion::ToRotationMatrix.
	// The vector v is obtained as the product of that rotation matrix with
	// vector u.  As such, the quaternion representation of a rotation
	// matrix requires less space than the matrix and more time to compute
	// the rotated vector.  Typical space-time tradeoff...

	HMatrix rot;
	ToRotationMatrix(rot);
	return rot*vec;
}
//----------------------------------------------------------------------------
HQuaternion& HQuaternion::Slerp (float t, const HQuaternion& p,
								 const HQuaternion& q)
{
	float cs = p.Dot(q);
	float angle = acosf(cs);

	if (fabsf(angle) > 0.0f)
	{
		float sn = sinf(angle);
		float invSn = 1.0f/sn;
		float tAngle = t*angle;
		float coeff0 = sinf(angle - tAngle)*invSn;
		float coeff1 = sinf(tAngle)*invSn;

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
HQuaternion& HQuaternion::Intermediate (const HQuaternion& q0,
										const HQuaternion& q1, const HQuaternion& q2)
{
	// assert:  Q0, Q1, Q2 all unit-length
	HQuaternion q1Inv = q1.Conjugate();
	HQuaternion p0 = q1Inv*q0;
	HQuaternion p2 = q1Inv*q2;
	HQuaternion arg = -0.25f*(p0.Log() + p2.Log());
	HQuaternion a = q1*arg.Exp();
	*this = a;
	return *this;
}
//----------------------------------------------------------------------------
HQuaternion& HQuaternion::Squad (float t, const HQuaternion& q0,
								 const HQuaternion& a0, const HQuaternion& a1, const HQuaternion& q1)
{
	float slerpT = 2.0f*t*(1.0f - t);
	HQuaternion slerpP = Slerp(t, q0, q1);
	HQuaternion slerpQ = Slerp(t, a0, a1);
	return Slerp(slerpT, slerpP, slerpQ);
}
//----------------------------------------------------------------------------