// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5AXISALIGNEDBOX3_H
#define WM5AXISALIGNEDBOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{
	template <typename Real>
	class AxisAlignedBox3
	{
	public:
		// Construction and destruction.
		AxisAlignedBox3 ();  // uninitialized
		~AxisAlignedBox3 ();

		// The caller must ensure that xmin <= xmax, ymin <= ymax, and
		// zmin <= zmax.
		AxisAlignedBox3 (Real xmin, Real xmax, Real ymin, Real ymax,
			Real zmin, Real zmax);

		// Compute the center of the box and the extents (half-lengths)
		// of the box edges.
		void GetCenterExtents (Vector3<Real>& center, Real extent[3]);

		// Overlap testing is in the strict sense.  If the two boxes are just
		// touching along a common edge or a common face, the boxes are reported
		// as overlapping.
		bool HasXOverlap (const AxisAlignedBox3& box) const;
		bool HasYOverlap (const AxisAlignedBox3& box) const;
		bool HasZOverlap (const AxisAlignedBox3& box) const;
		bool TestIntersection (const AxisAlignedBox3& box) const;

		// The return value is 'true' if there is overlap.  In this case the
		// intersection is stored in 'intersection'.  If the return value is
		// 'false', there is no overlap.  In this case 'intersection' is
		// undefined.
		bool FindIntersection (const AxisAlignedBox3& box,
			AxisAlignedBox3& intersection) const;

		Vector3<Real> Min;
		Vector3<Real> Max;

		bool Contains( const Vector3<Real> & pos )
		{
			return
				pos.X() >= Min.X() &&
				pos.X() <= Max.X() &&
				pos.Y() >= Min.Y() &&
				pos.Y() <= Max.Y() &&
				pos.Z() >= Min.Z() &&
				pos.Z() <= Max.Z();
		}
		Vector3<Real> GetSize() const
		{
			return Vector3<Real>(
				Max[ 0 ] - Min[ 0 ],
				Max[ 1 ] - Min[ 1 ],
				Max[ 2 ] - Min[ 2 ]);
		}

		void Expand( const Vector3<Real> & pos )
		{
			for ( int i = 0; i < 3; ++i )
			{
				if ( pos[ i ] < Min[ i ] )
					Min[ i ] = pos[ i ];
				if ( pos[ i ] > Max[ i ] )
					Max[ i ] = pos[ i ];
			}
		}

		void Extend( float _x, float _y, float _z )
		{
			Min.X() -= _x;
			Min.Y() -= _y;
			Min.Z() -= _z;

			Max.X() += _x;
			Max.Y() += _y;
			Max.Z() += _z;
		}

		void Clear()
		{
			Min.X() = std::numeric_limits<Real>::max();
			Min.Y() = std::numeric_limits<Real>::max();
			Min.Z() = std::numeric_limits<Real>::max();

			Max.X() = std::numeric_limits<Real>::lowest();
			Max.Y() = std::numeric_limits<Real>::lowest();
			Max.Z() = std::numeric_limits<Real>::lowest();
		}
	};

#include "Wm5AxisAlignedBox3.inl"

	typedef AxisAlignedBox3<float> AxisAlignedBox3f;
	typedef AxisAlignedBox3<double> AxisAlignedBox3d;
}

#endif
