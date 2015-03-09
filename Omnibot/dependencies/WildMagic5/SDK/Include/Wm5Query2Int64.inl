// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query2Int64<Real>::Query2Int64 (int numVertices,
    const Vector2<Real>* vertices)
    :
    Query2<Real>(numVertices, vertices)
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query2Int64<Real>::GetType () const
{
    return Query::QT_INT64;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Int64<Real>::ToLine (const Vector2<Real>& test, int v0, int v1)
    const
{
    const Vector2<Real>& vec0 = mVertices[v0];
    const Vector2<Real>& vec1 = mVertices[v1];

    int64_t x0 = (int64_t)test[0] - (int64_t)vec0[0];
    int64_t y0 = (int64_t)test[1] - (int64_t)vec0[1];
    int64_t x1 = (int64_t)vec1[0] - (int64_t)vec0[0];
    int64_t y1 = (int64_t)vec1[1] - (int64_t)vec0[1];

    int64_t det = Det2(x0, y0, x1, y1);
    return (det > 0 ? +1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Int64<Real>::ToCircumcircle (const Vector2<Real>& test, int v0,
    int v1, int v2) const
{
    const Vector2<Real>& vec0 = mVertices[v0];
    const Vector2<Real>& vec1 = mVertices[v1];
    const Vector2<Real>& vec2 = mVertices[v2];

    int64_t iTest[2] = { (int64_t)test[0], (int64_t)test[1] };
    int64_t iV0[2] = { (int64_t)vec0[0], (int64_t)vec0[1] };
    int64_t iV1[2] = { (int64_t)vec1[0], (int64_t)vec1[1] };
    int64_t iV2[2] = { (int64_t)vec2[0], (int64_t)vec2[1] };

    int64_t s0x = iV0[0] + iTest[0];
    int64_t d0x = iV0[0] - iTest[0];
    int64_t s0y = iV0[1] + iTest[1];
    int64_t d0y = iV0[1] - iTest[1];
    int64_t s1x = iV1[0] + iTest[0];
    int64_t d1x = iV1[0] - iTest[0];
    int64_t s1y = iV1[1] + iTest[1];
    int64_t d1y = iV1[1] - iTest[1];
    int64_t s2x = iV2[0] + iTest[0];
    int64_t d2x = iV2[0] - iTest[0];
    int64_t s2y = iV2[1] + iTest[1];
    int64_t d2y = iV2[1] - iTest[1];
    int64_t z0 = s0x*d0x + s0y*d0y;
    int64_t z1 = s1x*d1x + s1y*d1y;
    int64_t z2 = s2x*d2x + s2y*d2y;
    int64_t det = Det3(d0x, d0y, z0, d1x, d1y, z1, d2x, d2y, z2);
    return (det < 0 ? 1 : (det > 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int64_t Query2Int64<Real>::Dot (int64_t x0, int64_t y0, int64_t x1,
    int64_t y1)
{
    return x0*x1 + y0*y1;
}
//----------------------------------------------------------------------------
template <typename Real>
int64_t Query2Int64<Real>::Det2 (int64_t x0, int64_t y0, int64_t x1,
    int64_t y1)
{
    return x0*y1 - x1*y0;
}
//----------------------------------------------------------------------------
template <typename Real>
int64_t Query2Int64<Real>::Det3 (int64_t x0, int64_t y0, int64_t z0,
    int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2)
{
    int64_t c00 = y1*z2 - y2*z1;
    int64_t c01 = y2*z0 - y0*z2;
    int64_t c02 = y0*z1 - y1*z0;
    return x0*c00 + x1*c01 + x2*c02;
}
//----------------------------------------------------------------------------
