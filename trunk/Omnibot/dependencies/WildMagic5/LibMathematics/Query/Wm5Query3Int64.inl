// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query3Int64<Real>::Query3Int64 (int iVQuantity, const Vector3<Real>* akVertex)
    :
    Query3<Real>(iVQuantity,akVertex)
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query3Int64<Real>::GetType () const
{
    return Query::QT_INT64;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Int64<Real>::ToPlane (const Vector3<Real>& test, int v0, int v1,
    int v2) const
{
    const Vector3<Real>& vec0 = mVertices[v0];
    const Vector3<Real>& vec1 = mVertices[v1];
    const Vector3<Real>& vec2 = mVertices[v2];

    int64_t x0 = (int64_t)test[0] - (int64_t)vec0[0];
    int64_t y0 = (int64_t)test[1] - (int64_t)vec0[1];
    int64_t z0 = (int64_t)test[2] - (int64_t)vec0[2];
    int64_t x1 = (int64_t)vec1[0] - (int64_t)vec0[0];
    int64_t y1 = (int64_t)vec1[1] - (int64_t)vec0[1];
    int64_t z1 = (int64_t)vec1[2] - (int64_t)vec0[2];
    int64_t x2 = (int64_t)vec2[0] - (int64_t)vec0[0];
    int64_t y2 = (int64_t)vec2[1] - (int64_t)vec0[1];
    int64_t z2 = (int64_t)vec2[2] - (int64_t)vec0[2];

    int64_t det = Det3(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    return (det > 0 ? +1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Int64<Real>::ToCircumsphere (const Vector3<Real>& test, int v0,
    int v1, int v2, int v3) const
{
    const Vector3<Real>& vec0 = mVertices[v0];
    const Vector3<Real>& vec1 = mVertices[v1];
    const Vector3<Real>& vec2 = mVertices[v2];
    const Vector3<Real>& vec3 = mVertices[v3];

    int64_t iTest[3] = { (int64_t)test[0], (int64_t)test[1],
        (int64_t)test[2] };
    int64_t iV0[3] = { (int64_t)vec0[0], (int64_t)vec0[1],
        (int64_t)vec0[2] };
    int64_t iV1[3] = { (int64_t)vec1[0], (int64_t)vec1[1],
        (int64_t)vec1[2] };
    int64_t iV2[3] = { (int64_t)vec2[0], (int64_t)vec2[1],
        (int64_t)vec2[2] };
    int64_t iV3[3] = { (int64_t)vec3[0], (int64_t)vec3[1],
        (int64_t)vec3[2] };

    int64_t s0x = iV0[0] + iTest[0];
    int64_t d0x = iV0[0] - iTest[0];
    int64_t s0y = iV0[1] + iTest[1];
    int64_t d0y = iV0[1] - iTest[1];
    int64_t s0z = iV0[2] + iTest[2];
    int64_t d0z = iV0[2] - iTest[2];
    int64_t s1x = iV1[0] + iTest[0];
    int64_t d1x = iV1[0] - iTest[0];
    int64_t s1y = iV1[1] + iTest[1];
    int64_t d1y = iV1[1] - iTest[1];
    int64_t s1z = iV1[2] + iTest[2];
    int64_t d1z = iV1[2] - iTest[2];
    int64_t s2x = iV2[0] + iTest[0];
    int64_t d2x = iV2[0] - iTest[0];
    int64_t s2y = iV2[1] + iTest[1];
    int64_t d2y = iV2[1] - iTest[1];
    int64_t s2z = iV2[2] + iTest[2];
    int64_t d2z = iV2[2] - iTest[2];
    int64_t s3x = iV3[0] + iTest[0];
    int64_t d3x = iV3[0] - iTest[0];
    int64_t s3y = iV3[1] + iTest[1];
    int64_t d3y = iV3[1] - iTest[1];
    int64_t s3z = iV3[2] + iTest[2];
    int64_t d3z = iV3[2] - iTest[2];
    int64_t w0 = s0x*d0x + s0y*d0y + s0z*d0z;
    int64_t w1 = s1x*d1x + s1y*d1y + s1z*d1z;
    int64_t w2 = s2x*d2x + s2y*d2y + s2z*d2z;
    int64_t w3 = s3x*d3x + s3y*d3y + s3z*d3z;
    int64_t det = Det4(d0x, d0y, d0z, w0, d1x, d1y, d1z, w1, d2x, d2y, d2z,
        w2, d3x, d3y, d3z, w3);

    return (det > 0 ? 1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int64_t Query3Int64<Real>::Dot (int64_t x0, int64_t y0, int64_t z0,
    int64_t x1, int64_t y1, int64_t z1)
{
    return x0*x1 + y0*y1 + z0*z1;
}
//----------------------------------------------------------------------------
template <typename Real>
int64_t Query3Int64<Real>::Det3 (int64_t x0, int64_t y0, int64_t z0,
    int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2)
{
    int64_t c00 = y1*z2 - y2*z1;
    int64_t c01 = y2*z0 - y0*z2;
    int64_t c02 = y0*z1 - y1*z0;
    return x0*c00 + x1*c01 + x2*c02;
}
//----------------------------------------------------------------------------
template <typename Real>
int64_t Query3Int64<Real>::Det4 (int64_t x0, int64_t y0,
    int64_t z0, int64_t w0, int64_t x1, int64_t y1, int64_t z1,
    int64_t w1, int64_t x2, int64_t y2, int64_t z2, int64_t w2,
    int64_t x3, int64_t y3, int64_t z3, int64_t w3)
{
    int64_t a0 = x0*y1 - x1*y0;
    int64_t a1 = x0*y2 - x2*y0;
    int64_t a2 = x0*y3 - x3*y0;
    int64_t a3 = x1*y2 - x2*y1;
    int64_t a4 = x1*y3 - x3*y1;
    int64_t a5 = x2*y3 - x3*y2;
    int64_t b0 = z0*w1 - z1*w0;
    int64_t b1 = z0*w2 - z2*w0;
    int64_t b2 = z0*w3 - z3*w0;
    int64_t b3 = z1*w2 - z2*w1;
    int64_t b4 = z1*w3 - z3*w1;
    int64_t b5 = z2*w3 - z3*w2;
    return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
}
//----------------------------------------------------------------------------
