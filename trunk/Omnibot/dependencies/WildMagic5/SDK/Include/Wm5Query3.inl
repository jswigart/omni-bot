// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query3<Real>::Query3 (int numVertices, const Vector3<Real>* vertices)
    :
    mNumVertices(numVertices),
    mVertices(vertices)
{
    assertion(mNumVertices > 0 && mVertices, "Invalid inputs\n");
}
//----------------------------------------------------------------------------
template <typename Real>
Query3<Real>::~Query3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query3<Real>::GetType () const
{
    return Query::QT_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Query3<Real>::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Vector3<Real>* Query3<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3<Real>::ToPlane (int i, int v0, int v1, int v2) const
{
    return ToPlane(mVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3<Real>::ToPlane (const Vector3<Real>& test, int v0, int v1, int v2)
    const
{
    bool positive = Sort(v0, v1, v2);

    const Vector3<Real>& vec0 = mVertices[v0];
    const Vector3<Real>& vec1 = mVertices[v1];
    const Vector3<Real>& vec2 = mVertices[v2];

    Real x0 = test[0] - vec0[0];
    Real y0 = test[1] - vec0[1];
    Real z0 = test[2] - vec0[2];
    Real x1 = vec1[0] - vec0[0];
    Real y1 = vec1[1] - vec0[1];
    Real z1 = vec1[2] - vec0[2];
    Real x2 = vec2[0] - vec0[0];
    Real y2 = vec2[1] - vec0[1];
    Real z2 = vec2[2] - vec0[2];

    Real det = Det3(x0,y0,z0,x1,y1,z1,x2,y2,z2);
    if (!positive)
    {
        det = -det;
    }

    return (det > (Real)0 ? +1 : (det < (Real)0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3<Real>::ToTetrahedron (int i, int v0, int v1, int v2,
    int v3) const
{
    return ToTetrahedron(mVertices[i], v0, v1, v2, v3);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3<Real>::ToTetrahedron (const Vector3<Real>& test, int v0,
    int v1, int v2, int v3) const
{
    int sign0 = ToPlane(test, v1, v2, v3);
    if (sign0 > 0)
    {
        return +1;
    }

    int sign1 = ToPlane(test, v0, v2, v3);
    if (sign1 < 0)
    {
        return +1;
    }

    int sign2 = ToPlane(test, v0, v1, v3);
    if (sign2 > 0)
    {
        return +1;
    }

    int sign3 = ToPlane(test, v0, v1, v2);
    if (sign3 < 0)
    {
        return +1;
    }

    return ((sign0 && sign1 && sign2 && sign3) ? -1 : 0);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3<Real>::ToCircumsphere (int i, int v0, int v1, int v2,
    int v3) const
{
    return ToCircumsphere(mVertices[i], v0, v1, v2, v3);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3<Real>::ToCircumsphere (const Vector3<Real>& test, int v0,
    int v1, int v2, int v3) const
{
    bool positive = Sort(v0, v1, v2, v3);

    const Vector3<Real>& vec0 = mVertices[v0];
    const Vector3<Real>& vec1 = mVertices[v1];
    const Vector3<Real>& vec2 = mVertices[v2];
    const Vector3<Real>& vec3 = mVertices[v3];

    Real s0x = vec0[0] + test[0];
    Real d0x = vec0[0] - test[0];
    Real s0y = vec0[1] + test[1];
    Real d0y = vec0[1] - test[1];
    Real s0z = vec0[2] + test[2];
    Real d0z = vec0[2] - test[2];
    Real s1x = vec1[0] + test[0];
    Real d1x = vec1[0] - test[0];
    Real s1y = vec1[1] + test[1];
    Real d1y = vec1[1] - test[1];
    Real s1z = vec1[2] + test[2];
    Real d1z = vec1[2] - test[2];
    Real s2x = vec2[0] + test[0];
    Real d2x = vec2[0] - test[0];
    Real s2y = vec2[1] + test[1];
    Real d2y = vec2[1] - test[1];
    Real s2z = vec2[2] + test[2];
    Real d2z = vec2[2] - test[2];
    Real s3x = vec3[0] + test[0];
    Real d3x = vec3[0] - test[0];
    Real s3y = vec3[1] + test[1];
    Real d3y = vec3[1] - test[1];
    Real s3z = vec3[2] + test[2];
    Real d3z = vec3[2] - test[2];
    Real w0 = s0x*d0x + s0y*d0y + s0z*d0z;
    Real w1 = s1x*d1x + s1y*d1y + s1z*d1z;
    Real w2 = s2x*d2x + s2y*d2y + s2z*d2z;
    Real w3 = s3x*d3x + s3y*d3y + s3z*d3z;

    Real det = Det4(d0x, d0y, d0z, w0, d1x, d1y, d1z, w1, d2x, d2y, d2z, w2,
        d3x,d3y,d3z,w3);
    if (!positive)
    {
        det = -det;
    }

    return (det > (Real)0 ? 1 : (det < (Real)0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
Real Query3<Real>::Dot (Real x0, Real y0, Real z0, Real x1, Real y1,
    Real z1)
{
    return x0*x1 + y0*y1 + z0*z1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Query3<Real>::Det3 (Real x0, Real y0, Real z0, Real x1, Real y1,
    Real z1, Real x2, Real y2, Real z2)
{
    Real c00 = y1*z2 - y2*z1;
    Real c01 = y2*z0 - y0*z2;
    Real c02 = y0*z1 - y1*z0;
    return x0*c00 + x1*c01 + x2*c02;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Query3<Real>::Det4 (Real x0, Real y0, Real z0, Real w0, Real x1,
    Real y1, Real z1, Real w1, Real x2, Real y2, Real z2, Real w2,
    Real x3, Real y3, Real z3, Real w3)
{
    Real a0 = x0*y1 - x1*y0;
    Real a1 = x0*y2 - x2*y0;
    Real a2 = x0*y3 - x3*y0;
    Real a3 = x1*y2 - x2*y1;
    Real a4 = x1*y3 - x3*y1;
    Real a5 = x2*y3 - x3*y2;
    Real b0 = z0*w1 - z1*w0;
    Real b1 = z0*w2 - z2*w0;
    Real b2 = z0*w3 - z3*w0;
    Real b3 = z1*w2 - z2*w1;
    Real b4 = z1*w3 - z3*w1;
    Real b5 = z2*w3 - z3*w2;
    return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
}
//----------------------------------------------------------------------------
