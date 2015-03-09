// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query2Integer<Real>::Query2Integer (int numVertices,
    const Vector2<Real>* vertices)
    :
    Query2<Real>(numVertices, vertices)
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query2Integer<Real>::GetType () const
{
    return Query::QT_INTEGER;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Integer<Real>::ToLine (const Vector2<Real>& test, int v0, int v1)
    const
{
    const Vector2<Real>& vec0 = mVertices[v0];
    const Vector2<Real>& vec1 = mVertices[v1];

    Integer<2> x0((int)test[0] - (int)vec0[0]);
    Integer<2> y0((int)test[1] - (int)vec0[1]);
    Integer<2> x1((int)vec1[0] - (int)vec0[0]);
    Integer<2> y1((int)vec1[1] - (int)vec0[1]);

    Integer<2> det = Det2(x0, y0, x1, y1);
    return (det > 0 ? +1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Integer<Real>::ToCircumcircle (const Vector2<Real>& test, int v0,
    int v1, int v2) const
{
    const Vector2<Real>& vec0 = mVertices[v0];
    const Vector2<Real>& vec1 = mVertices[v1];
    const Vector2<Real>& vec2 = mVertices[v2];

    int iTest[2] = { (int)test[0], (int)test[1] };
    int iV0[2] = { (int)vec0[0], (int)vec0[1] };
    int iV1[2] = { (int)vec1[0], (int)vec1[1] };
    int iV2[2] = { (int)vec2[0], (int)vec2[1] };

    Integer<4> s0x(iV0[0] + iTest[0]);
    Integer<4> d0x(iV0[0] - iTest[0]);
    Integer<4> s0y(iV0[1] + iTest[1]);
    Integer<4> d0y(iV0[1] - iTest[1]);
    Integer<4> s1x(iV1[0] + iTest[0]);
    Integer<4> d1x(iV1[0] - iTest[0]);
    Integer<4> s1y(iV1[1] + iTest[1]);
    Integer<4> d1y(iV1[1] - iTest[1]);
    Integer<4> s2x(iV2[0] + iTest[0]);
    Integer<4> d2x(iV2[0] - iTest[0]);
    Integer<4> s2y(iV2[1] + iTest[1]);
    Integer<4> d2y(iV2[1] - iTest[1]);
    Integer<4> z0 = s0x*d0x + s0y*d0y;
    Integer<4> z1 = s1x*d1x + s1y*d1y;
    Integer<4> z2 = s2x*d2x + s2y*d2y;
    Integer<4> det = Det3(d0x, d0y, z0, d1x, d1y, z1, d2x, d2y, z2);
    return (det < 0 ? 1 : (det > 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
Integer<2> Query2Integer<Real>::Dot (Integer<2>& x0, Integer<2>& y0,
    Integer<2>& x1, Integer<2>& y1)
{
    return x0*x1 + y0*y1;
}
//----------------------------------------------------------------------------
template <typename Real>
Integer<2> Query2Integer<Real>::Det2 (Integer<2>& x0, Integer<2>& y0,
    Integer<2>& x1, Integer<2>& y1)
{
    return x0*y1 - x1*y0;
}
//----------------------------------------------------------------------------
template <typename Real>
Integer<4> Query2Integer<Real>::Det3 (Integer<4>& x0, Integer<4>& y0,
    Integer<4>& z0, Integer<4>& x1, Integer<4>& y1, Integer<4>& z1,
    Integer<4>& x2, Integer<4>& y2, Integer<4>& z2)
{
    Integer<4> c00 = y1*z2 - y2*z1;
    Integer<4> c01 = y2*z0 - y0*z2;
    Integer<4> c02 = y0*z1 - y1*z0;
    return x0*c00 + x1*c01 + x2*c02;
}
//----------------------------------------------------------------------------
