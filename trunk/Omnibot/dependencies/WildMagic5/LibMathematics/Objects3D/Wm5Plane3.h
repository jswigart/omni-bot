// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5PLANE3_H
#define WM5PLANE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{
	template <typename Real>
	class Plane3
	{
	public:
		// The plane is represented as Dot(N,X) = c where N is a unit-length
		// normal vector, c is the plane constant, and X is any point on the
		// plane.  The user must ensure that the normal vector is unit length.

		// Construction and destruction.
		Plane3 ();  // uninitialized
		~Plane3 ();

		// Specify N and c directly.
		Plane3 (const Vector3<Real>& normal, Real constant);

		// N is specified, c = Dot(N,P) where P is a point on the plane.
		Plane3 (const Vector3<Real>& normal, const Vector3<Real>& p);

		// N = Cross(P1-P0,P2-P0)/Length(Cross(P1-P0,P2-P0)), c = Dot(N,P0) where
		// P0, P1, P2 are points on the plane.
		Plane3 (const Vector3<Real>& p0, const Vector3<Real>& p1,
			const Vector3<Real>& p2);

		// Compute d = Dot(N,P)-c where N is the plane normal and c is the plane
		// constant.  This is a signed distance.  The sign of the return value is
		// positive if the point is on the positive side of the plane, negative if
		// the point is on the negative side, and zero if the point is on the
		// plane.
		Real DistanceTo (const Vector3<Real>& p) const;

		// The "positive side" of the plane is the half space to which the plane
		// normal points.  The "negative side" is the other half space.  The
		// function returns +1 when P is on the positive side, -1 when P is on the
		// the negative side, or 0 when P is on the plane.
		int WhichSide (const Vector3<Real>& p) const;

		Vector3<Real> Normal;
		Real Constant;

		///////////////////////////////////////////////////////////////////////////
		// Custom functions

		Plane3 operator-() const { return Plane3( -Normal, -Constant );  }

		///////////////////////////////////////////////////////////////////////////
	};

#include "Wm5Plane3.inl"

	typedef Plane3<float> Plane3f;
	typedef Plane3<double> Plane3d;
}

#endif
