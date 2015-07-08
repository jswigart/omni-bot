// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/09/19)

//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>::Vector3 ()
{
	// Uninitialized for performance in array construction.
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>::Vector3 (const Vector3& vec)
{
	mTuple[0] = vec.mTuple[0];
	mTuple[1] = vec.mTuple[1];
	mTuple[2] = vec.mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>::Vector3 (const Tuple<3,Real>& tuple)
{
	mTuple[0] = tuple[0];
	mTuple[1] = tuple[1];
	mTuple[2] = tuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>::Vector3 (Real x, Real y, Real z)
{
	mTuple[0] = x;
	mTuple[1] = y;
	mTuple[2] = z;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>::Vector3 (const Real coords[3])
{
	mTuple[0] = coords[0];
	mTuple[1] = coords[1];
	mTuple[2] = coords[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>& Vector3<Real>::operator= (const Vector3& vec)
{
	mTuple[0] = vec.mTuple[0];
	mTuple[1] = vec.mTuple[1];
	mTuple[2] = vec.mTuple[2];
	return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>& Vector3<Real>::operator= (const Tuple<3,Real>& tuple)
{
	mTuple[0] = tuple[0];
	mTuple[1] = tuple[1];
	mTuple[2] = tuple[2];
	return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::X () const
{
	return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector3<Real>::X ()
{
	return mTuple[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::Y () const
{
	return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector3<Real>::Y ()
{
	return mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::Z () const
{
	return mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Vector3<Real>::Z ()
{
	return mTuple[2];
}

template <typename Real>
inline Real* Vector3<Real>::Ptr()
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <typename Real>
inline const Real* Vector3<Real>::Ptr() const
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> Vector3<Real>::operator+ (const Vector3& vec) const
{
	return Vector3
		(
		mTuple[0] + vec.mTuple[0],
		mTuple[1] + vec.mTuple[1],
		mTuple[2] + vec.mTuple[2]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> Vector3<Real>::operator- (const Vector3& vec) const
{
	return Vector3
		(
		mTuple[0] - vec.mTuple[0],
		mTuple[1] - vec.mTuple[1],
		mTuple[2] - vec.mTuple[2]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> Vector3<Real>::operator* (Real scalar) const
{
	return Vector3
		(
		scalar*mTuple[0],
		scalar*mTuple[1],
		scalar*mTuple[2]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> Vector3<Real>::operator/ (Real scalar) const
{
	Vector3 result;

	if (scalar != (Real)0)
	{
		Real invScalar = ((Real)1)/scalar;
		result.mTuple[0] = invScalar*mTuple[0];
		result.mTuple[1] = invScalar*mTuple[1];
		result.mTuple[2] = invScalar*mTuple[2];
	}
	else
	{
		result.mTuple[0] = Math<Real>::MAX_REAL;
		result.mTuple[1] = Math<Real>::MAX_REAL;
		result.mTuple[2] = Math<Real>::MAX_REAL;
	}

	return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> Vector3<Real>::operator- () const
{
	return Vector3
		(
		-mTuple[0],
		-mTuple[1],
		-mTuple[2]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real>& Vector3<Real>::operator+= (const Vector3& vec)
{
	mTuple[0] += vec.mTuple[0];
	mTuple[1] += vec.mTuple[1];
	mTuple[2] += vec.mTuple[2];
	return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real>& Vector3<Real>::operator-= (const Vector3& vec)
{
	mTuple[0] -= vec.mTuple[0];
	mTuple[1] -= vec.mTuple[1];
	mTuple[2] -= vec.mTuple[2];
	return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real>& Vector3<Real>::operator*= (Real scalar)
{
	mTuple[0] *= scalar;
	mTuple[1] *= scalar;
	mTuple[2] *= scalar;
	return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real>& Vector3<Real>::operator/= (Real scalar)
{
	if (scalar != (Real)0)
	{
		Real invScalar = ((Real)1)/scalar;
		mTuple[0] *= invScalar;
		mTuple[1] *= invScalar;
		mTuple[2] *= invScalar;
	}
	else
	{
		mTuple[0] *= Math<Real>::MAX_REAL;
		mTuple[1] *= Math<Real>::MAX_REAL;
		mTuple[2] *= Math<Real>::MAX_REAL;
	}

	return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::Length () const
{
	return Math<Real>::Sqrt
		(
		mTuple[0]*mTuple[0] +
		mTuple[1]*mTuple[1] +
		mTuple[2]*mTuple[2]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::Length2d () const
{
	return Math<Real>::Sqrt
		(
		mTuple[0]*mTuple[0] +
		mTuple[1]*mTuple[1]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::SquaredLength () const
{
	return
		mTuple[0]*mTuple[0] +
		mTuple[1]*mTuple[1] +
		mTuple[2]*mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::SquaredLength2d () const
{
	return
		mTuple[0]*mTuple[0] +
		mTuple[1]*mTuple[1];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::Dot (const Vector3& vec) const
{
	return
		mTuple[0]*vec.mTuple[0] +
		mTuple[1]*vec.mTuple[1] +
		mTuple[2]*vec.mTuple[2];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Vector3<Real>::Normalize (const Real epsilon)
{
	Real length = Length();

	if (length > epsilon)
	{
		Real invLength = ((Real)1)/length;
		mTuple[0] *= invLength;
		mTuple[1] *= invLength;
		mTuple[2] *= invLength;
	}
	else
	{
		length = (Real)0;
		mTuple[0] = (Real)0;
		mTuple[1] = (Real)0;
		mTuple[2] = (Real)0;
	}

	return length;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Vector3<Real>::Cross (const Vector3& vec) const
{
	return Vector3
		(
		mTuple[1]*vec.mTuple[2] - mTuple[2]*vec.mTuple[1],
		mTuple[2]*vec.mTuple[0] - mTuple[0]*vec.mTuple[2],
		mTuple[0]*vec.mTuple[1] - mTuple[1]*vec.mTuple[0]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Vector3<Real>::UnitCross (const Vector3& vec) const
{
	Vector3 cross
		(
		mTuple[1]*vec.mTuple[2] - mTuple[2]*vec.mTuple[1],
		mTuple[2]*vec.mTuple[0] - mTuple[0]*vec.mTuple[2],
		mTuple[0]*vec.mTuple[1] - mTuple[1]*vec.mTuple[0]
	);
	cross.Normalize();
	return cross;
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector3<Real>::ComputeExtremes (int numVectors, const Vector3* vectors,
									 Vector3& vmin, Vector3& vmax)
{
	assertion(numVectors > 0 && vectors,
		"Invalid inputs to ComputeExtremes\n");

	vmin = vectors[0];
	vmax = vmin;
	for (int j = 1; j < numVectors; ++j)
	{
		const Vector3& vec = vectors[j];
		for (int i = 0; i < 3; ++i)
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
void Vector3<Real>::Orthonormalize (Vector3& u, Vector3& v, Vector3& w)
{
	// If the input vectors are v0, v1, and v2, then the Gram-Schmidt
	// orthonormalization produces vectors u0, u1, and u2 as follows,
	//
	//   u0 = v0/|v0|
	//   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
	//   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
	//
	// where |A| indicates length of vector A and A*B indicates dot
	// product of vectors A and B.

	// compute u0
	u.Normalize();

	// compute u1
	Real dot0 = u.Dot(v);
	v -= dot0*u;
	v.Normalize();

	// compute u2
	Real dot1 = v.Dot(w);
	dot0 = u.Dot(w);
	w -= dot0*u + dot1*v;
	w.Normalize();
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector3<Real>::Orthonormalize (Vector3* v)
{
	Orthonormalize(v[0], v[1], v[2]);
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector3<Real>::GenerateOrthonormalBasis (Vector3& u, Vector3& v,
											  Vector3& w)
{
	w.Normalize();
	GenerateComplementBasis(u, v, w);
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector3<Real>::GenerateComplementBasis (Vector3& u, Vector3& v,
											 const Vector3& w)
{
	Real invLength;

	if (Math<Real>::FAbs(w.mTuple[0]) >= Math<Real>::FAbs(w.mTuple[1]))
	{
		// W.X() or W.Z() is the largest magnitude component, swap them
		invLength = Math<Real>::InvSqrt(w.mTuple[0]*w.mTuple[0] +
			w.mTuple[2]*w.mTuple[2]);
		u.mTuple[0] = -w.mTuple[2]*invLength;
		u.mTuple[1] = (Real)0;
		u.mTuple[2] = +w.mTuple[0]*invLength;
		v.mTuple[0] = w.mTuple[1]*u.mTuple[2];
		v.mTuple[1] = w.mTuple[2]*u.mTuple[0] - w.mTuple[0]*u.mTuple[2];
		v.mTuple[2] = -w.mTuple[1]*u.mTuple[0];
	}
	else
	{
		// W.Y() or W.Z() is the largest magnitude component, swap them
		invLength = Math<Real>::InvSqrt(w.mTuple[1]*w.mTuple[1] +
			w.mTuple[2]*w.mTuple[2]);
		u.mTuple[0] = (Real)0;
		u.mTuple[1] = +w.mTuple[2]*invLength;
		u.mTuple[2] = -w.mTuple[1]*invLength;
		v.mTuple[0] = w.mTuple[1]*u.mTuple[2] - w.mTuple[2]*u.mTuple[1];
		v.mTuple[1] = -w.mTuple[0]*u.mTuple[2];
		v.mTuple[2] = w.mTuple[0]*u.mTuple[1];
	}
}
//----------------------------------------------------------------------------
template <typename Real>
bool Vector3<Real>::GetBarycentrics (const Vector3& v0, const Vector3& v1,
									 const Vector3& v2, const Vector3& v3, Real bary[4], const Real epsilon)
									 const
{
	// Compute the vectors relative to V3 of the tetrahedron.
	Vector3<Real> diff[4] = { v0 - v3, v1 - v3, v2 - v3, *this - v3 };

	Real det = diff[0].Dot(diff[1].Cross(diff[2]));
	Vector3<Real> e1ce2 = diff[1].Cross(diff[2]);
	Vector3<Real> e2ce0 = diff[2].Cross(diff[0]);
	Vector3<Real> e0ce1 = diff[0].Cross(diff[1]);
	if (Math<Real>::FAbs(det) > epsilon)
	{
		Real invDet = ((Real)1)/det;
		bary[0] = diff[3].Dot(e1ce2)*invDet;
		bary[1] = diff[3].Dot(e2ce0)*invDet;
		bary[2] = diff[3].Dot(e0ce1)*invDet;
		bary[3] = (Real)1 - bary[0] - bary[1] - bary[2];
		return true;
	}

	for (int i = 0; i < 4; ++i)
	{
		bary[i] = (Real)0;
	}

#ifdef WM5_ASSERT_ON_BARYCENTRIC3_DEGENERATE
	assertion(false, "Input tetrahedron is degenerate.\n");
#endif
	return false;
}
//----------------------------------------------------------------------------
template <typename Real>
void Vector3<Real>::GetInformation (int numPoints, const Vector3* points,
									Real epsilon, Information& info)
{
	assertion(numPoints > 0 && points && epsilon >= (Real)0,
		"Invalid inputs to GetInformation\n");

	info.mExtremeCCW = false;

	// Compute the axis-aligned bounding box for the input points.  Keep track
	// of the indices into 'points' for the current min and max.
	int j, indexMin[3], indexMax[3];
	for (j = 0; j < 3; ++j)
	{
		info.mMin[j] = points[0][j];
		info.mMax[j] = info.mMin[j];
		indexMin[j] = 0;
		indexMax[j] = 0;
	}

	int i;
	for (i = 1; i < numPoints; ++i)
	{
		for (j = 0; j < 3; ++j)
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
	range = info.mMax[2] - info.mMin[2];
	if (range > info.mMaxRange)
	{
		info.mMaxRange = range;
		info.mExtreme[0] = indexMin[2];
		info.mExtreme[1] = indexMax[2];
	}

	// The origin is either the point of minimum x-value, point of
	// minimum y-value, or point of minimum z-value.
	info.mOrigin = points[info.mExtreme[0]];

	// Test whether the point set is (nearly) a point.
	if (info.mMaxRange < epsilon)
	{
		info.mDimension = 0;
		for (j = 0; j < 3; ++j)
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
	Real maxDistance = (Real)0;
	Real distance, dot;
	info.mExtreme[2] = info.mExtreme[0];
	for (i = 0; i < numPoints; ++i)
	{
		Vector3<Real> diff = points[i] - info.mOrigin;
		dot = info.mDirection[0].Dot(diff);
		Vector3<Real> proj = diff - dot*info.mDirection[0];
		distance = proj.Length();
		if (distance > maxDistance)
		{
			maxDistance = distance;
			info.mExtreme[2] = i;
		}
	}

	if (maxDistance < epsilon*info.mMaxRange)
	{
		info.mDimension = 1;
		info.mExtreme[2] = info.mExtreme[1];
		info.mExtreme[3] = info.mExtreme[1];
		return;
	}

	// Test whether the point set is (nearly) a planar polygon.
	info.mDirection[1] = points[info.mExtreme[2]] - info.mOrigin;
	dot = info.mDirection[0].Dot(info.mDirection[1]);
	info.mDirection[1] -= dot*info.mDirection[0];
	info.mDirection[1].Normalize();
	info.mDirection[2] = info.mDirection[0].Cross(info.mDirection[1]);
	maxDistance = (Real)0;
	Real maxSign = (Real)0;
	info.mExtreme[3] = info.mExtreme[0];
	for (i = 0; i < numPoints; ++i)
	{
		Vector3<Real> diff = points[i] - info.mOrigin;
		distance = info.mDirection[2].Dot(diff);
		Real sign = Math<Real>::Sign(distance);
		distance = Math<Real>::FAbs(distance);
		if (distance > maxDistance)
		{
			maxDistance = distance;
			maxSign = sign;
			info.mExtreme[3] = i;
		}
	}

	if (maxDistance < epsilon*info.mMaxRange)
	{
		info.mDimension = 2;
		info.mExtreme[3] = info.mExtreme[2];
		return;
	}

	info.mDimension = 3;
	info.mExtremeCCW = (maxSign > (Real)0);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Vector3<Real>::Reflect(const Vector3& normal) const
{
	return Vector3( *this - ( 2 * this->Dot(normal) * normal ) );
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Vector3<Real>::MidPoint(const Vector3& vec) const
{
	return Vector3(
		( x + vec.X() ) * (Real)0.5,
		( y + vec.Y() ) * (Real)0.5,
		( z + vec.Z() ) * (Real)0.5 );
}
//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::Truncate(Real length)
{
	if(length == 0.0f)
		x = y = z = 0.0f;
	else
	{
		Real length2 = SquaredLength();
		if ( length2 > length * length )
		{
			Real ilength = length * Math<Real>::InvSqrt( length2 );
			x *= ilength;
			y *= ilength;
			z *= ilength;
		}
	}
}
//----------------------------------------------------------------------------
template <class Real>
Real Vector3<Real>::ProjectOntoVector(const Vector3& rU)
{
	Real Scale = this->Dot(rU) / rU.SquaredLength();	// Find the scale of this vector on U
	(*this)=rU;											// Copy U onto this vector
	(*this)*=Scale;										// Use the previously calculated scale to get the right length.
	return Scale;
}
//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::RandomVector(Real magnitude)
{
	x = Math<Real>::SymmetricRandom();
	y = Math<Real>::SymmetricRandom();
	z = Math<Real>::SymmetricRandom();

	Normalize();

	*this *= magnitude;
}
//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::FromSpherical( Real heading, Real pitch, Real radius )
{
	float fST, fCT, fSP, fCP;
	Math<Real>::SinCos(heading, fST, fCT);
	Math<Real>::SinCos(pitch, fSP, fCP);
	*this = radius * Vector3<Real>(fCP*fST, fCP*fCT, fSP);
}
//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::ToSpherical( Real &heading, Real &pitch, Real &radius )
{
	// reference vector is 0,0,1
	radius = Length();
	pitch = radius > 0.0f ? Math<Real>::ASin(Z()/radius) : 0.0f;
	heading = Math<Real>::ATan2(X(), Y());
}
//----------------------------------------------------------------------------
template <class Real>
Real Vector3<Real>::GetPitch() const
{
	Vector3<Real> vTmp = *this;
	vTmp.Normalize();
	return Math<Real>::ASin(vTmp.Z());
}
//----------------------------------------------------------------------------
template <class Real>
Real Vector3<Real>::XYHeading() const
{
	return -Math<Real>::ATan2(-X(), Y());
}
//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::FromHeading( Real fHeading )
{
	Math<Real>::SinCos(fHeading, X(), Y());
	Z() = (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Vector3<Real>::Perpendicular() const
{
	const Real fSquareZero = (Real)1e-06 * (Real)1e-06;

	Vector3 perp = UnitCross(Vector3::UNIT_X);

	// Check length
	if(perp.SquaredLength() < fSquareZero)
	{
		/* This vector is the Y axis multiplied by a scalar, so we have
		to use another axis. */
		perp = UnitCross( Vector3::UNIT_Y );
	}
	return perp;
}
//----------------------------------------------------------------------------
template <class Real>
bool Vector3<Real>::IsZero () const
{
	return *this == Vector3<Real>::ZERO;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Vector3<Real>::Flatten(Real _z) const
{
	return Vector3<Real>(X(),Y(),Z());
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> operator* (Real scalar, const Vector3<Real>& vec)
{
	return Vector3<Real>
		(
		scalar*vec[0],
		scalar*vec[1],
		scalar*vec[2]
	);
}
//----------------------------------------------------------------------------
template <typename Real>
std::ostream& operator<< (std::ostream& outFile, const Vector3<Real>& vec)
{
	return outFile << vec.X() << ' ' << vec.Y() << ' ' << vec.Z();
}
//----------------------------------------------------------------------------
template <class Real>
std::istream& operator>> (std::istream& rkOStr, Vector3<Real>& rkV)
{
	return rkOStr >> rkV.x >> rkV.y >> rkV.z;
}
//----------------------------------------------------------------------------
template <class Real>
Real Length(const Vector3<Real>& v1, const Vector3<Real>& v2)
{
	return (v1-v2).Length();
}
//----------------------------------------------------------------------------
template <class Real>
Real Length2d(const Vector3<Real>& v1, const Vector3<Real>& v2)
{
	return (v1-v2).Length2d();
}
//----------------------------------------------------------------------------
template <class Real>
Real SquaredLength(const Vector3<Real>& v1, const Vector3<Real>& v2)
{
	return (v1-v2).SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real SquaredLength2d(const Vector3<Real>& v1, const Vector3<Real>& v2)
{
	return (v1-v2).SquaredLength2d();
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Normalize(const Vector3<Real>& v1)
{
	Vector3<Real> v = v1;
	v.Normalize();
	return v;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Interpolate(const Vector3<Real> &_v1, const Vector3<Real> &_v2, Real _t)
{
	return _v1 + (_v2 - _v1) * _t;
}
//----------------------------------------------------------------------------
template <class Real>
Real PointToSegmentDistance(const Vector3<Real> &aPt, const Vector3<Real> &aSeg1,
							const Vector3<Real> &aSeg2, Vector3<Real> &aOutClosest, Real &aOutTime)
{
	Vector3<Real> vSegmentNormal = aSeg2 - aSeg1;
	vSegmentNormal.Normalize();
	Vector3<Real> vDiff = aPt - aSeg1;
	Real fSegmentLength = Length(aSeg2, aSeg1);

	// Edge cases if the point isn't on the segment, distance must be
	// to the nearest end point.
	float fProjection = vSegmentNormal.Dot(vDiff);
	if(fProjection < 0)
	{
		aOutClosest = aSeg1;
		aOutTime = 0.0;
	}
	else if(fProjection > fSegmentLength)
	{
		aOutClosest = aSeg2;
		aOutTime = 1.0;
	}
	else
	{
		aOutClosest = aSeg1 + vSegmentNormal * fProjection;
		aOutTime = fProjection / fSegmentLength;
	}
	return Length(aPt, aOutClosest);
}