// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Circle3<Real>::Circle3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Circle3<Real>::~Circle3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Circle3<Real>::Circle3 (const Vector3<Real>& center,
    const Vector3<Real>& direction0, const Vector3<Real>& direction1,
    const Vector3<Real>& normal, const Real radius)
    :
    Center(center),
    Direction0(direction0),
    Direction1(direction1),
    Normal(normal),
    Radius(radius)
{
}
//----------------------------------------------------------------------------
