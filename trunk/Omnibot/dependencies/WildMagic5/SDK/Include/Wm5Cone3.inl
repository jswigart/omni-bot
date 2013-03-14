// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Cone3<Real>::Cone3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Cone3<Real>::~Cone3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Cone3<Real>::Cone3 (const Vector3<Real>& vertex, const Vector3<Real>& axis,
    const Real angle, const Real height)
    :
    Vertex(vertex),
    Axis(axis),
    Height(height)
{
    CosAngle = Math<Real>::Cos(angle);
    SinAngle = Math<Real>::Sin(angle);
}
//----------------------------------------------------------------------------
template <typename Real>
Cone3<Real>::Cone3 (const Vector3<Real>& vertex, const Vector3<Real>& axis,
    const Real cosAngle, const Real sinAngle, const Real height)
    :
    Vertex(vertex),
    Axis(axis),
    CosAngle(cosAngle),
    SinAngle(sinAngle),
    Height(height)
{
}
//----------------------------------------------------------------------------
