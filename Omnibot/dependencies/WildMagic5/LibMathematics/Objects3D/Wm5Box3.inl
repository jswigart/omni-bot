// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Box3<Real>::Box3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real>::~Box3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real>::Box3 (const Vector3<Real>& center, const Vector3<Real> axis[3],
				  const Real extent[3])
				  :
Center(center)
{
	Axis[0] = axis[0];
	Axis[1] = axis[1];
	Axis[2] = axis[2];
	Extent[0] = extent[0];
	Extent[1] = extent[1];
	Extent[2] = extent[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real>::Box3 (const Vector3<Real>& center, const Vector3<Real>& axis0,
				  const Vector3<Real>& axis1, const Vector3<Real>& axis2,
				  const Real extent0, const Real extent1, const Real extent2)
				  :
Center(center)
{
	Axis[0] = axis0;
	Axis[1] = axis1;
	Axis[2] = axis2;
	Extent[0] = extent0;
	Extent[1] = extent1;
	Extent[2] = extent2;
}
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real>::Box3( const Vector3<Real>& mins, const Vector3<Real>& maxs )
	:
	Center( ( mins + maxs ) * 0.5 )
{
	Axis[ 0 ] = Vector3f::UNIT_X;
	Axis[ 1 ] = Vector3f::UNIT_Y;
	Axis[ 2 ] = Vector3f::UNIT_Z;
	Extent[ 0 ] = maxs[ 0 ] - mins[ 0 ];
	Extent[ 1 ] = maxs[ 1 ] - mins[ 1 ];
	Extent[ 2 ] = maxs[ 2 ] - mins[ 2 ];
}
//----------------------------------------------------------------------------
template <typename Real>
void Box3<Real>::ComputeVertices (Vector3<Real> vertex[8]) const
{
	Vector3<Real> extAxis0 = Extent[0]*Axis[0];
	Vector3<Real> extAxis1 = Extent[1]*Axis[1];
	Vector3<Real> extAxis2 = Extent[2]*Axis[2];

	vertex[0] = Center - extAxis0 - extAxis1 - extAxis2;
	vertex[1] = Center + extAxis0 - extAxis1 - extAxis2;
	vertex[2] = Center + extAxis0 + extAxis1 - extAxis2;
	vertex[3] = Center - extAxis0 + extAxis1 - extAxis2;
	vertex[4] = Center - extAxis0 - extAxis1 + extAxis2;
	vertex[5] = Center + extAxis0 - extAxis1 + extAxis2;
	vertex[6] = Center + extAxis0 + extAxis1 + extAxis2;
	vertex[7] = Center - extAxis0 + extAxis1 + extAxis2;
}
//----------------------------------------------------------------------------
template <class Real>
void Box3<Real>::Identity( float defaultSize )
{
	Center = Vector3<Real>::ZERO;
	Axis[0] = Vector3<Real>( 1.f, 0.f, 0.f );
	Axis[1] = Vector3<Real>( 0.f, 1.f, 0.f );
	Axis[2] = Vector3<Real>( 0.f, 0.f, 1.f );
	Extent[0] = defaultSize;
	Extent[1] = defaultSize;
	Extent[2] = defaultSize;
}

//----------------------------------------------------------------------------