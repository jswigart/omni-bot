// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Plane3<Real>::Plane3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Plane3<Real>::~Plane3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Plane3<Real>::Plane3 (const Vector3<Real>& normal, Real constant)
    :
    Normal(normal),
    Constant(constant)
{
}
//----------------------------------------------------------------------------
template <typename Real>
Plane3<Real>::Plane3 (const Vector3<Real>& normal, const Vector3<Real>& p)
    :
    Normal(normal)
{
    Constant = normal.Dot(p);
}
//----------------------------------------------------------------------------
template <typename Real>
Plane3<Real>::Plane3 (const Vector3<Real>& p0, const Vector3<Real>& p1,
    const Vector3<Real>& p2)
{
    Vector3<Real> edge1 = p1 - p0;
    Vector3<Real> edge2 = p2 - p0;
    Normal = edge1.UnitCross(edge2);
    Constant = Normal.Dot(p0);
}
//----------------------------------------------------------------------------
template <typename Real>
Real Plane3<Real>::DistanceTo (const Vector3<Real>& p) const
{
    return Normal.Dot(p) - Constant;
}
//----------------------------------------------------------------------------
template <typename Real>
int Plane3<Real>::WhichSide (const Vector3<Real>& p) const
{
    Real distance = DistanceTo(p);

    if (distance < (Real)0)
    {
        return -1;
    }
    else if (distance > (Real)0)
    {
        return +1;
    }
    else
    {
        return 0;
    }
}
//----------------------------------------------------------------------------
