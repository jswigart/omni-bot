// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query3Rational<Real>::Query3Rational (int numVertices,
    const Vector3<Real>* vertices)
    :
    Query3<Real>(numVertices, vertices)
{
    mRVertices = new1<QRVector>(mNumVertices);
    mEvaluated = new1<bool>(mNumVertices);
    memset(mEvaluated, 0, mNumVertices*sizeof(bool));
}
//----------------------------------------------------------------------------
template <typename Real>
Query3Rational<Real>::~Query3Rational ()
{
    delete1(mRVertices);
    delete1(mEvaluated);
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query3Rational<Real>::GetType () const
{
    return Query::QT_RATIONAL;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToPlane (int i, int v0, int v1, int v2) const
{
    int indices[4] = { i, v0, v1, v2 };
    Convert(4, indices);
    return ToPlane(mRVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToPlane (const Vector3<Real>& test, int v0, int v1,
    int v2) const
{
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    ratTest[2] = QRational(test[2]);
    int indices[3] = { v0, v1, v2 };
    Convert(3, indices);
    return ToPlane(ratTest, v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToTetrahedron (int i, int v0, int v1, int v2,
    int v3) const
{
    int indices[5] = { i, v0, v1, v2, v3 };
    Convert(5, indices);
    return ToTetrahedron(mRVertices[i], v0, v1, v2, v3);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToTetrahedron (const Vector3<Real>& test, int v0,
    int v1, int v2, int v3) const
{
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    ratTest[2] = QRational(test[2]);
    int indices[4] = { v0, v1, v2, v3 };
    Convert(4, indices);
    return ToTetrahedron(ratTest, v0, v1, v2, v3);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToCircumsphere (int i, int v0, int v1, int v2,
    int v3) const
{
    int indices[5] = { i, v0, v1, v2, v3 };
    Convert(5, indices);
    return ToCircumsphere(mRVertices[i], v0, v1, v2, v3);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToCircumsphere (const Vector3<Real>& test, int v0,
    int v1, int v2, int v3) const
{
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    ratTest[2] = QRational(test[2]);
    int indices[4] = { v0, v1, v2, v3 };
    Convert(4, indices);
    return ToCircumsphere(ratTest, v0, v1, v2, v3);
}
//----------------------------------------------------------------------------
template <typename Real>
void Query3Rational<Real>::Convert (int numIndices, int* indices) const
{
    for (int i = 0; i < numIndices; ++i)
    {
        int j = indices[i];
        if (!mEvaluated[j])
        {
            mEvaluated[j] = true;
            mRVertices[j][0] = QRational(mVertices[j][0]);
            mRVertices[j][1] = QRational(mVertices[j][1]);
            mRVertices[j][2] = QRational(mVertices[j][2]);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToPlane (const QRVector& ratTest, int v0, int v1,
    int v2) const
{
    QRational x0 = ratTest[0] - mRVertices[v0][0];
    QRational y0 = ratTest[1] - mRVertices[v0][1];
    QRational z0 = ratTest[2] - mRVertices[v0][2];
    QRational x1 = mRVertices[v1][0] - mRVertices[v0][0];
    QRational y1 = mRVertices[v1][1] - mRVertices[v0][1];
    QRational z1 = mRVertices[v1][2] - mRVertices[v0][2];
    QRational x2 = mRVertices[v2][0] - mRVertices[v0][0];
    QRational y2 = mRVertices[v2][1] - mRVertices[v0][1];
    QRational z2 = mRVertices[v2][2] - mRVertices[v0][2];

    QRational det = Det3(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    return (det > 0 ? +1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToTetrahedron (const QRVector& ratTest, int v0,
    int v1, int v2, int v3) const
{
    int sign0 = ToPlane(ratTest, v1, v2, v3);
    if (sign0 > 0)
    {
        return +1;
    }

    int sign1 = ToPlane(ratTest, v0, v2, v3);
    if (sign1 < 0)
    {
        return +1;
    }

    int sign2 = ToPlane(ratTest, v0, v1, v3);
    if (sign2 > 0)
    {
        return +1;
    }

    int sign3 = ToPlane(ratTest, v0, v1, v2);
    if (sign3 < 0)
    {
        return +1;
    }

    return ((sign0 && sign1 && sign2 && sign3) ? -1 : 0);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Rational<Real>::ToCircumsphere (const QRVector& ratTest, int v0,
    int v1, int v2, int v3) const
{
    QRVector& ratVec0 = mRVertices[v0];
    QRVector& ratVec1 = mRVertices[v1];
    QRVector& ratVec2 = mRVertices[v2];
    QRVector& ratVec3 = mRVertices[v3];

    QRational s0x = ratVec0[0] + ratTest[0];
    QRational d0x = ratVec0[0] - ratTest[0];
    QRational s0y = ratVec0[1] + ratTest[1];
    QRational d0y = ratVec0[1] - ratTest[1];
    QRational s0z = ratVec0[2] + ratTest[2];
    QRational d0z = ratVec0[2] - ratTest[2];
    QRational s1x = ratVec1[0] + ratTest[0];
    QRational d1x = ratVec1[0] - ratTest[0];
    QRational s1y = ratVec1[1] + ratTest[1];
    QRational d1y = ratVec1[1] - ratTest[1];
    QRational s1z = ratVec1[2] + ratTest[2];
    QRational d1z = ratVec1[2] - ratTest[2];
    QRational s2x = ratVec2[0] + ratTest[0];
    QRational d2x = ratVec2[0] - ratTest[0];
    QRational s2y = ratVec2[1] + ratTest[1];
    QRational d2y = ratVec2[1] - ratTest[1];
    QRational s2z = ratVec2[2] + ratTest[2];
    QRational d2z = ratVec2[2] - ratTest[2];
    QRational s3x = ratVec3[0] + ratTest[0];
    QRational d3x = ratVec3[0] - ratTest[0];
    QRational s3y = ratVec3[1] + ratTest[1];
    QRational d3y = ratVec3[1] - ratTest[1];
    QRational s3z = ratVec3[2] + ratTest[2];
    QRational d3z = ratVec3[2] - ratTest[2];
    QRational w0 = s0x*d0x + s0y*d0y + s0z*d0z;
    QRational w1 = s1x*d1x + s1y*d1y + s1z*d1z;
    QRational w2 = s2x*d2x + s2y*d2y + s2z*d2z;
    QRational w3 = s3x*d3x + s3y*d3y + s3z*d3z;
    QRational det = Det4(d0x, d0y, d0z, w0, d1x, d1y, d1z, w1, d2x, d2y, d2z,
        w2, d3x, d3y, d3z, w3);

    return (det > 0 ? 1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
typename Query3Rational<Real>::QRational Query3Rational<Real>::Dot (
    QRational& x0, QRational& y0, QRational& z0, QRational& x1,
    QRational& y1, QRational& z1)
{
    return x0*x1 + y0*y1 + z0*z1;
}
//----------------------------------------------------------------------------
template <typename Real>
typename Query3Rational<Real>::QRational Query3Rational<Real>::Det3 (
    QRational& x0, QRational& y0, QRational& z0, QRational& x1,
    QRational& y1, QRational& z1, QRational& x2, QRational& y2,
    QRational& z2)
{
    QRational c00 = y1*z2 - y2*z1;
    QRational c01 = y2*z0 - y0*z2;
    QRational c02 = y0*z1 - y1*z0;
    return x0*c00 + x1*c01 + x2*c02;
}
//----------------------------------------------------------------------------
template <typename Real>
typename Query3Rational<Real>::QRational Query3Rational<Real>::Det4 (
    QRational& x0, QRational& y0, QRational& z0, QRational& w0,
    QRational& x1, QRational& y1, QRational& z1, QRational& w1,
    QRational& x2, QRational& y2, QRational& z2, QRational& w2,
    QRational& x3, QRational& y3, QRational& z3, QRational& w3)
{
    QRational a0 = x0*y1 - x1*y0;
    QRational a1 = x0*y2 - x2*y0;
    QRational a2 = x0*y3 - x3*y0;
    QRational a3 = x1*y2 - x2*y1;
    QRational a4 = x1*y3 - x3*y1;
    QRational a5 = x2*y3 - x3*y2;
    QRational b0 = z0*w1 - z1*w0;
    QRational b1 = z0*w2 - z2*w0;
    QRational b2 = z0*w3 - z3*w0;
    QRational b3 = z1*w2 - z2*w1;
    QRational b4 = z1*w3 - z3*w1;
    QRational b5 = z2*w3 - z3*w2;
    return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
}
//----------------------------------------------------------------------------
