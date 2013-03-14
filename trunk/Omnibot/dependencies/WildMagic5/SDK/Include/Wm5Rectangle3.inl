// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Rectangle3<Real>::Rectangle3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Rectangle3<Real>::~Rectangle3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Rectangle3<Real>::Rectangle3 (const Vector3<Real>& center,
    const Vector3<Real> axis[2], const Real extent[2])
    :
    Center(center)
{
    Axis[0] = axis[0];
    Axis[1] = axis[1];
    Extent[0] = extent[0];
    Extent[1] = extent[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Rectangle3<Real>::Rectangle3 (const Vector3<Real>& center,
    const Vector3<Real>& axis0, const Vector3<Real>& axis1, Real extent0,
    Real extent1)
    :
    Center(center)
{
    Axis[0] = axis0;
    Axis[1] = axis1;
    Extent[0] = extent0;
    Extent[1] = extent1;
}
//----------------------------------------------------------------------------
template <typename Real>
void Rectangle3<Real>::ComputeVertices (Vector3<Real> vertex[4]) const
{
    Vector3<Real> extAxis0 = Axis[0]*Extent[0];
    Vector3<Real> extAxis1 = Axis[1]*Extent[1];

    vertex[0] = Center - extAxis0 - extAxis1;
    vertex[1] = Center + extAxis0 - extAxis1;
    vertex[2] = Center + extAxis0 + extAxis1;
    vertex[3] = Center - extAxis0 + extAxis1;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Rectangle3<Real>::GetPPCorner () const
{
    return Center + Extent[0]*Axis[0] + Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Rectangle3<Real>::GetPMCorner () const
{
    return Center + Extent[0]*Axis[0] - Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Rectangle3<Real>::GetMPCorner () const
{
    return Center - Extent[0]*Axis[0] + Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Rectangle3<Real>::GetMMCorner () const
{
    return Center - Extent[0]*Axis[0] - Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
