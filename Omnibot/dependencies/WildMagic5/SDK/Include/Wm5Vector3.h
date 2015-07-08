// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5VECTOR3_H
#define WM5VECTOR3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"
#include "Wm5Math.h"

namespace Wm5
{
	template <typename Real>
	class Vector3 : public Tuple<3,Real>
	{
	public:
		// Construction.
		Vector3 ();  // uninitialized
		Vector3 (const Vector3& vec);
		Vector3 (const Tuple<3,Real>& tuple);
		Vector3 (Real x, Real y, Real z);
		Vector3 (const Real coords[3]);

		// Assignment.
		Vector3& operator= (const Vector3& vec);
		Vector3& operator= (const Tuple<3,Real>& tuple);

		// Coordinate access.
		inline Real X () const;
		inline Real& X ();
		inline Real Y () const;
		inline Real& Y ();
		inline Real Z () const;
		inline Real& Z ();

		inline Real* Ptr();
		inline const Real* Ptr() const;

		// Arithmetic operations.
		inline Vector3 operator+ (const Vector3& vec) const;
		inline Vector3 operator- (const Vector3& vec) const;
		inline Vector3 operator* (Real scalar) const;
		inline Vector3 operator/ (Real scalar) const;
		inline Vector3 operator- () const;

		// Arithmetic updates.
		inline Vector3& operator+= (const Vector3& vec);
		inline Vector3& operator-= (const Vector3& vec);
		inline Vector3& operator*= (Real scalar);
		inline Vector3& operator/= (Real scalar);

		// Vector3 operations.
		inline Real Length () const;
		inline Real Length2d () const;
		inline Real SquaredLength () const;
		inline Real SquaredLength2d () const;
		inline Real Dot (const Vector3& vec) const;
		inline Real Normalize (const Real epsilon = Math<Real>::ZERO_TOLERANCE);

		// Compute the axis-aligned bounding box of the points.
		static void ComputeExtremes (int numVectors, const Vector3* vectors,
			Vector3& vmin, Vector3& vmax);

		// The cross products are computed using the right-handed rule.  Be aware
		// that some graphics APIs use a left-handed rule.  If you have to compute
		// a cross product with these functions and send the result to the API
		// that expects left-handed, you will need to change sign on the vector
		// (replace each component value c by -c).
		Vector3 Cross (const Vector3& vec) const;
		Vector3 UnitCross (const Vector3& vec) const;

		// Gram-Schmidt orthonormalization.  Take linearly independent vectors
		// U, V, and W and compute an orthonormal set (unit length, mutually
		// perpendicular).
		static void Orthonormalize (Vector3& u, Vector3& v, Vector3& w);
		static void Orthonormalize (Vector3* v);

		// Input W must be a nonzero vector. The output is an orthonormal basis
		// {U,V,W}.  The input W is normalized by this function.  If you know
		// W is already unit length, use GenerateComplementBasis to compute U
		// and V.
		static void GenerateOrthonormalBasis (Vector3& u, Vector3& v, Vector3& w);

		// Input W must be a unit-length vector.  The output vectors {U,V} are
		// unit length and mutually perpendicular, and {U,V,W} is an orthonormal
		// basis.
		static void GenerateComplementBasis (Vector3& u, Vector3& v,
			const Vector3& w);

		// Compute the barycentric coordinates of the point V with respect to the
		// tetrahedron <V0,V1,V2,V3>, V = b0*V0 + b1*V1 + b2*V2 + b3*V3, where
		// b0 + b1 + b2 + b3 = 1.  The return value is 'true' iff {V0,V1,V2,V3} is
		// a linearly independent set.  Numerically, this is measured by
		// |det[V0 V1 V2 V3]| <= epsilon.  The values bary[...] are valid only
		// when the return value is 'true' but set to zero when the return value
		// is 'false'.
		bool GetBarycentrics (const Vector3& v0, const Vector3& v1,
			const Vector3& v2, const Vector3& v3, Real bary[4],
			const Real epsilon = (Real)0) const;

		struct Information
		{
			// The intrinsic dimension of the input set.  The parameter 'epsilon'
			// to the GetInformation function is used to provide a tolerance when
			// determining the dimension.
			int mDimension;

			// Axis-aligned bounding box of the input set.  The maximum range is
			// the larger of max[0]-min[0], max[1]-min[1], and max[2]-min[2].
			Real mMin[3], mMax[3];
			Real mMaxRange;

			// Coordinate system.  The origin is valid for any dimension d.  The
			// unit-length direction vector is valid only for 0 <= i < d.  The
			// extreme index is relative to the array of input points, and is also
			// valid only for 0 <= i < d.  If d = 0, all points are effectively
			// the same, but the use of an epsilon may lead to an extreme index
			// that is not zero.  If d = 1, all points effectively lie on a line
			// segment.  If d = 2, all points effectively line on a plane.  If
			// d = 3, the points are not coplanar.
			Vector3 mOrigin;
			Vector3 mDirection[3];

			// The indices that define the maximum dimensional extents.  The
			// values mExtreme[0] and mExtreme[1] are the indices for the points
			// that define the largest extent in one of the coordinate axis
			// directions.  If the dimension is 2, then mExtreme[2] is the index
			// for the point that generates the largest extent in the direction
			// perpendicular to the line through the points corresponding to
			// mExtreme[0] and mExtreme[1].  Furthermore, if the dimension is 3,
			// then mExtreme[3] is the index for the point that generates the
			// largest extent in the direction perpendicular to the triangle
			// defined by the other extreme points.  The tetrahedron formed by the
			// points V[extreme0], V[extreme1], V[extreme2], V[extreme3]> is
			// clockwise or counterclockwise, the condition stored in mExtremeCCW.
			int mExtreme[4];
			bool mExtremeCCW;
		};

		// The value of epsilon is used as a relative error when computing the
		// dimension of the point set.
		static void GetInformation (int numPoints, const Vector3* points,
			Real epsilon, Information& info);

		// Special vectors.
		WM5_MATHEMATICS_ITEM static const Vector3 ZERO;    // (0,0,0)
		WM5_MATHEMATICS_ITEM static const Vector3 UNIT_X;  // (1,0,0)
		WM5_MATHEMATICS_ITEM static const Vector3 UNIT_Y;  // (0,1,0)
		WM5_MATHEMATICS_ITEM static const Vector3 UNIT_Z;  // (0,0,1)
		WM5_MATHEMATICS_ITEM static const Vector3 ONE;     // (1,1,1)

		///////////////////////////////////////////////////////////////////////////
		// Custom functions
		bool IsZero () const;

		Vector3 Reflect(const Vector3& normal) const;
		Vector3 MidPoint(const Vector3& vec) const;
		void Truncate(Real length);
		Real ProjectOntoVector(const Vector3& rU);

		Vector3 Perpendicular() const;

		void RandomVector(Real magnitude);
		void FromSpherical(Real heading, Real pitch, Real radius);
		void ToSpherical(Real &heading, Real &pitch, Real &radius);

		Real GetPitch() const;
		Real XYHeading() const;
		void FromHeading(Real fHeading);

		Vector3<Real> Flatten(Real _z = 0.f) const;

		bool IsValid() const { return !isnan( X() ) && !isnan( Y() ) && !isnan( Z() ); }
		///////////////////////////////////////////////////////////////////////////
	protected:
		using Tuple<3,Real>::mTuple;
	};

	// Arithmetic operations.
	template <typename Real>
	inline Vector3<Real> operator* (Real scalar, const Vector3<Real>& vec);

	// Debugging output.
	template <typename Real>
	std::ostream& operator<< (std::ostream& outFile, const Vector3<Real>& vec);

	template <class Real>
	std::istream& operator>> (std::istream& rkOStr, Vector3<Real>& rkV);
	///////////////////////////////////////////////////////////////////////////
	// Custom functions
	template <class Real>
	Real Length(const Vector3<Real>& v1, const Vector3<Real>& v2);

	template <class Real>
	Real Length2d(const Vector3<Real>& v1, const Vector3<Real>& v2);

	template <class Real>
	Real SquaredLength(const Vector3<Real>& v1, const Vector3<Real>& v2);

	template <class Real>
	Real SquaredLength2d(const Vector3<Real>& v1, const Vector3<Real>& v2);

	template <class Real>
	Vector3<Real> Normalize(const Vector3<Real>& v1);

	template <class Real>
	Vector3<Real> Interpolate(const Vector3<Real> &_1, const Vector3<Real> &_2, Real _t);

	template <class Real>
	Real PointToSegmentDistance(const Vector3<Real> &aPt, const Vector3<Real> &aSeg1,
		const Vector3<Real> &aSeg2, Vector3<Real> &aOutClosest, Real &aOutTime);

	///////////////////////////////////////////////////////////////////////////
#include "Wm5Vector3.inl"

	typedef Vector3<float> Vector3f;
	typedef Vector3<double> Vector3d;

	typedef std::vector<Vector3f> Vector3List;
}

#endif
