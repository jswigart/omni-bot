// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query2Rational<Real>::Query2Rational (int numVertices,
    const Vector2<Real>* vertices)
    :
    Query2<Real>(numVertices, vertices)
{
    mRVertices = new1<QRVector>(mNumVertices);
    mEvaluated = new1<bool>(mNumVertices);
    memset(mEvaluated, 0, mNumVertices*sizeof(bool));
}
//----------------------------------------------------------------------------
template <typename Real>
Query2Rational<Real>::~Query2Rational ()
{
    delete1(mRVertices);
    delete1(mEvaluated);
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query2Rational<Real>::GetType () const
{
    return Query::QT_RATIONAL;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToLine (int i, int v0, int v1) const
{
    int indices[3] = { i, v0, v1 };
    Convert(3, indices);
    return ToLine(mRVertices[i], v0, v1);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToLine (const Vector2<Real>& test, int v0, int v1)
    const
{
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    int indices[2] = { v0, v1 };
    Convert(2, indices);
    return ToLine(ratTest, v0, v1);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToTriangle (int i, int v0, int v1, int v2)
    const
{
    int indices[4] = { i, v0, v1, v2 };
    Convert(4, indices);
    return ToTriangle(mRVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToTriangle (const Vector2<Real>& test, int v0,
    int v1, int v2) const
{
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    int indices[3] = { v0, v1, v2 };
    Convert(3, indices);
    return ToTriangle(ratTest, v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToCircumcircle (int i, int v0, int v1, int v2)
    const
{
    int indices[4] = { i, v0, v1, v2 };
    Convert(4, indices);
    return ToCircumcircle(mRVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToCircumcircle (const Vector2<Real>& test,
    int v0, int v1, int v2) const
{
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    int indices[3] = { v0, v1, v2 };
    Convert(3, indices);
    return ToCircumcircle(ratTest, v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
void Query2Rational<Real>::Convert (int numIndices, int* indices) const
{
    for (int i = 0; i < numIndices; ++i)
    {
        int j = indices[i];
        if (!mEvaluated[j])
        {
            mEvaluated[j] = true;
            mRVertices[j][0] = QRational(mVertices[j][0]);
            mRVertices[j][1] = QRational(mVertices[j][1]);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToLine (const QRVector& ratTest, int v0,
    int v1) const
{
    QRational x0 = ratTest[0] - mRVertices[v0][0];
    QRational y0 = ratTest[1] - mRVertices[v0][1];
    QRational x1 = mRVertices[v1][0] - mRVertices[v0][0];
    QRational y1 = mRVertices[v1][1] - mRVertices[v0][1];

    QRational det = Det2(x0, y0, x1, y1);
    return (det > 0 ? +1 : (det < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToTriangle (const QRVector& ratTest, int v0, int v1,
    int v2) const
{
    int sign0 = ToLine(ratTest, v1, v2);
    if (sign0 > 0)
    {
        return +1;
    }

    int sign1 = ToLine(ratTest, v0, v2);
    if (sign1 < 0)
    {
        return +1;
    }

    int sign2 = ToLine(ratTest, v0, v1);
    if (sign2 > 0)
    {
        return +1;
    }

    return ((sign0 && sign1 && sign2) ? -1 : 0);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Rational<Real>::ToCircumcircle (const QRVector& ratTest, int v0,
    int v1, int v2) const
{
    QRVector& ratVec0 = mRVertices[v0];
    QRVector& ratVec1 = mRVertices[v1];
    QRVector& ratVec2 = mRVertices[v2];

    QRational s0x = ratVec0[0] + ratTest[0];
    QRational d0x = ratVec0[0] - ratTest[0];
    QRational s0y = ratVec0[1] + ratTest[1];
    QRational d0y = ratVec0[1] - ratTest[1];
    QRational s1x = ratVec1[0] + ratTest[0];
    QRational d1x = ratVec1[0] - ratTest[0];
    QRational s1y = ratVec1[1] + ratTest[1];
    QRational d1y = ratVec1[1] - ratTest[1];
    QRational s2x = ratVec2[0] + ratTest[0];
    QRational d2x = ratVec2[0] - ratTest[0];
    QRational s2y = ratVec2[1] + ratTest[1];
    QRational d2y = ratVec2[1] - ratTest[1];
    QRational z0 = s0x*d0x + s0y*d0y;
    QRational z1 = s1x*d1x + s1y*d1y;
    QRational z2 = s2x*d2x + s2y*d2y;
    QRational det = Det3(d0x, d0y, z0, d1x, d1y, z1, d2x, d2y, z2);
    return (det < 0 ? 1 : (det > 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
typename Query2Rational<Real>::QRational Query2Rational<Real>::Dot (
    QRational& x0, QRational& y0, QRational& x1, QRational& y1)
{
    return x0*x1 + y0*y1;
}
//----------------------------------------------------------------------------
template <typename Real>
typename Query2Rational<Real>::QRational Query2Rational<Real>::Det2 (
    QRational& x0, QRational& y0, QRational& x1, QRational& y1)
{
    return x0*y1 - x1*y0;
}
//----------------------------------------------------------------------------
template <typename Real>
typename Query2Rational<Real>::QRational Query2Rational<Real>::Det3 (
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
