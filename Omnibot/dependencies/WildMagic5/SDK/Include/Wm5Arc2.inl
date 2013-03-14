// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Arc2<Real>::Arc2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Arc2<Real>::~Arc2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Arc2<Real>::Arc2 (const Vector2<Real>& center, Real radius,
    const Vector2<Real>& end0, const Vector2<Real>& end1)
    :
    Center(center),
    Radius(radius),
    End0(end0),
    End1(end1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool Arc2<Real>::Contains (const Vector2<Real>& p) const
{
    // Assert: |P-C| = R where P is the input point, C is the circle center,
    // and R is the circle radius.  For P to be on the arc from A to B, it
    // must be on the side of the plane containing A with normal N = Perp(B-A)
    // where Perp(u,v) = (v,-u).

    Vector2<Real> diffPE0 = p - End0;
    Vector2<Real> diffE1E0 = End1 - End0;
    Real dotPerp = diffPE0.DotPerp(diffE1E0);
    return dotPerp >= (Real)0;
}
//----------------------------------------------------------------------------
