// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5BOX3_H
#define WM5BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{
	template <typename Real>
	class Box3
	{
	public:
		// A box has center C, axis directions U[0], U[1], and U[2] (mutually
		// perpendicular unit-length vectors), and extents e[0], e[1], and e[2]
		// (all nonnegative numbers).  A point X = C+y[0]*U[0]+y[1]*U[1]+y[2]*U[2]
		// is inside or on the box whenever |y[i]| <= e[i] for all i.

		// Construction and destruction.
		Box3 ();  // uninitialized
		~Box3 ();

		Box3 (const Vector3<Real>& center, const Vector3<Real> axis[3],
			const Real extent[3]);

		Box3 (const Vector3<Real>& center, const Vector3<Real>& axis0,
			const Vector3<Real>& axis1, const Vector3<Real>& axis2,
			const Real extent0, const Real extent1, const Real extent2);

		// axis aligned
		Box3( const Vector3<Real>& mins, const Vector3<Real>& maxs );

		bool operator==( const Box3<Real> & other )
		{
			return Center == other.Center &&
				Axis[ 0 ] == other.Axis[ 0 ] && 
				Axis[ 1 ] == other.Axis[ 1 ] && 
				Axis[ 2 ] == other.Axis[ 2 ] && 
				Extent[ 0 ] == other.Extent[ 0 ] && 
				Extent[ 1 ] == other.Extent[ 1 ] && 
				Extent[ 2 ] == other.Extent[ 2 ];
		}

		bool operator!=( const Box3<Real> & other )
		{
			return Center != other.Center ||
				Axis[ 0 ] != other.Axis[ 0 ] ||
				Axis[ 1 ] != other.Axis[ 1 ] ||
				Axis[ 2 ] != other.Axis[ 2 ] ||
				Extent[ 0 ] != other.Extent[ 0 ] ||
				Extent[ 1 ] != other.Extent[ 1 ] ||
				Extent[ 2 ] != other.Extent[ 2 ];
		}

		void ComputeVertices (Vector3<Real> vertex[8]) const;

		Vector3<Real> Center;
		Vector3<Real> Axis[3];
		Real Extent[3];

		void Identity( float defaultSize );

		Vector3f GetCenterTop() const
		{
			return Center + Axis[ 2 ] * Extent[ 2 ];
		}
		Vector3f GetCenterBottom() const
		{
			return Center - Axis[ 2 ] * Extent[ 2 ];
		}

		Real GetArea() const
		{
			return Extent[ 0 ] * Extent[ 1 ] * Extent[ 2 ];
		}

		void Expand( Real amount )
		{
			Extent[ 0 ] += amount;
			Extent[ 1 ] += amount;
			Extent[ 2 ] += amount;
		}
	};

#include "Wm5Box3.inl"

	typedef Box3<float> Box3f;
	typedef Box3<double> Box3d;
}

#endif
