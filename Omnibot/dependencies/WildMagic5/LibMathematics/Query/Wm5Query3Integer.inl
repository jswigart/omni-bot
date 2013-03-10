// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query3Integer<Real>::Query3Integer (int numVertices,
    const Vector3<Real>* vertices)
    :
    Query3<Real>(numVertices, vertices)
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query3Integer<Real>::GetType () const
{
    return Query::QT_INTEGER;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Integer<Real>::ToPlane (const Vector3<Real>& test, int v0, int v1,
    int v2) const
{
    const Vector3<Real>& vec0 = mVertices[v0];
    const Vector3<Real>& vec1 = mVertices[v1];
    const Vector3<Real>& vec2 = mVertices[v2];

    Integer<4> x0((int)test[0] - (int)vec0[0]);
    Integer<4> y0((int)test[1] - (int)vec0[1]);
    Integer<4> z0((int)test[2] - (int)vec0[2]);
    Integer<4> x1((int)vec1[0] - (int)vec0[0]);
    Integer<4> y1((int)vec1[1] - (int)vec0[1]);
    Integer<4> z1((int)vec1[2] - (int)vec0[2]);
    Integer<4> x2((int)vec2[0] - (int)vec0[0]);
    Integer<4> y2((int)vec2[1] - (int)vec0[1]);
    Integer<4> z2((int)vec2[2] - (int)vec0[2]);

    Integer<4> det = Det3(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    return (det > 0 ? +1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Integer<Real>::ToCircumsphere (const Vector3<Real>& test, int v0,
    int v1, int v2, int v3) const
{
    const Vector3<Real>& vec0 = mVertices[v0];
    const Vector3<Real>& vec1 = mVertices[v1];
    const Vector3<Real>& vec2 = mVertices[v2];
    const Vector3<Real>& vec3 = mVertices[v3];

    int iTest[3] = { (int)test[0], (int)test[1], (int)test[2] };
    int iV0[3] = { (int)vec0[0], (int)vec0[1], (int)vec0[2] };
    int iV1[3] = { (int)vec1[0], (int)vec1[1], (int)vec1[2] };
    int iV2[3] = { (int)vec2[0], (int)vec2[1], (int)vec2[2] };
    int iV3[3] = { (int)vec3[0], (int)vec3[1], (int)vec3[2] };

    Integer<6> s0x(iV0[0] + iTest[0]);
    Integer<6> d0x(iV0[0] - iTest[0]);
    Integer<6> s0y(iV0[1] + iTest[1]);
    Integer<6> d0y(iV0[1] - iTest[1]);
    Integer<6> s0z(iV0[2] + iTest[2]);
    Integer<6> d0z(iV0[2] - iTest[2]);
    Integer<6> s1x(iV1[0] + iTest[0]);
    Integer<6> d1x(iV1[0] - iTest[0]);
    Integer<6> s1y(iV1[1] + iTest[1]);
    Integer<6> d1y(iV1[1] - iTest[1]);
    Integer<6> s1z(iV1[2] + iTest[2]);
    Integer<6> d1z(iV1[2] - iTest[2]);
    Integer<6> s2x(iV2[0] + iTest[0]);
    Integer<6> d2x(iV2[0] - iTest[0]);
    Integer<6> s2y(iV2[1] + iTest[1]);
    Integer<6> d2y(iV2[1] - iTest[1]);
    Integer<6> s2z(iV2[2] + iTest[2]);
    Integer<6> d2z(iV2[2] - iTest[2]);
    Integer<6> s3x(iV3[0] + iTest[0]);
    Integer<6> d3x(iV3[0] - iTest[0]);
    Integer<6> s3y(iV3[1] + iTest[1]);
    Integer<6> d3y(iV3[1] - iTest[1]);
    Integer<6> s3z(iV3[2] + iTest[2]);
    Integer<6> d3z(iV3[2] - iTest[2]);
    Integer<6> w0 = s0x*d0x + s0y*d0y + s0z*d0z;
    Integer<6> w1 = s1x*d1x + s1y*d1y + s1z*d1z;
    Integer<6> w2 = s2x*d2x + s2y*d2y + s2z*d2z;
    Integer<6> w3 = s3x*d3x + s3y*d3y + s3z*d3z;
    Integer<6> det = Det4(d0x, d0y, d0z, w0, d1x, d1y, d1z, w1, d2x, d2y,
        d2z, w2, d3x, d3y, d3z, w3);

    return (det > 0 ? 1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
Integer<3> Query3Integer<Real>::Dot (Integer<3>& x0, Integer<3>& y0,
    Integer<3>& z0, Integer<3>& x1, Integer<3>& y1, Integer<3>& z1)
{
    return x0*x1 + y0*y1 + z0*z1;
}
//----------------------------------------------------------------------------
template <typename Real>
Integer<4> Query3Integer<Real>::Det3 (Integer<4>& x0, Integer<4>& y0,
    Integer<4>& z0, Integer<4>& x1, Integer<4>& y1, Integer<4>& z1,
    Integer<4>& x2, Integer<4>& y2, Integer<4>& z2)
{
    Integer<4> c00 = y1*z2 - y2*z1;
    Integer<4> c01 = y2*z0 - y0*z2;
    Integer<4> c02 = y0*z1 - y1*z0;
    return x0*c00 + x1*c01 + x2*c02;
}
//----------------------------------------------------------------------------
template <typename Real>
Integer<6> Query3Integer<Real>::Det4 (Integer<6>& x0, Integer<6>& y0,
    Integer<6>& z0, Integer<6>& w0, Integer<6>& x1, Integer<6>& y1,
    Integer<6>& z1, Integer<6>& w1, Integer<6>& x2, Integer<6>& y2,
    Integer<6>& z2, Integer<6>& w2, Integer<6>& x3, Integer<6>& y3,
    Integer<6>& z3, Integer<6>& w3)
{
    Integer<6> a0 = x0*y1 - x1*y0;
    Integer<6> a1 = x0*y2 - x2*y0;
    Integer<6> a2 = x0*y3 - x3*y0;
    Integer<6> a3 = x1*y2 - x2*y1;
    Integer<6> a4 = x1*y3 - x3*y1;
    Integer<6> a5 = x2*y3 - x3*y2;
    Integer<6> b0 = z0*w1 - z1*w0;
    Integer<6> b1 = z0*w2 - z2*w0;
    Integer<6> b2 = z0*w3 - z3*w0;
    Integer<6> b3 = z1*w2 - z2*w1;
    Integer<6> b4 = z1*w3 - z3*w1;
    Integer<6> b5 = z2*w3 - z3*w2;
    return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
}
//----------------------------------------------------------------------------
