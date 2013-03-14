// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query2<Real>::Query2 (int numVertices, const Vector2<Real>* vertices)
    :
    mNumVertices(numVertices),
    mVertices(vertices)
{
    assertion(mNumVertices > 0 && mVertices, "Invalid inputs\n");
}
//----------------------------------------------------------------------------
template <typename Real>
Query2<Real>::~Query2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query2<Real>::GetType () const
{
    return Query::QT_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Query2<Real>::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Vector2<Real>* Query2<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2<Real>::ToLine (int i, int v0, int v1) const
{
    return ToLine(mVertices[i], v0, v1);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2<Real>::ToLine (const Vector2<Real>& test, int v0, int v1) const
{
    bool positive = Sort(v0, v1);

    const Vector2<Real>& vec0 = mVertices[v0];
    const Vector2<Real>& vec1 = mVertices[v1];

    Real x0 = test[0] - vec0[0];
    Real y0 = test[1] - vec0[1];
    Real x1 = vec1[0] - vec0[0];
    Real y1 = vec1[1] - vec0[1];

    Real det = Det2(x0, y0, x1, y1);
    if (!positive)
    {
        det = -det;
    }

    return (det > (Real)0 ? +1 : (det < (Real)0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2<Real>::ToTriangle (int i, int v0, int v1, int v2) const
{
    return ToTriangle(mVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2<Real>::ToTriangle (const Vector2<Real>& test, int v0, int v1,
    int v2) const
{
    int sign0 = ToLine(test, v1, v2);
    if (sign0 > 0)
    {
        return +1;
    }

    int sign1 = ToLine(test, v0, v2);
    if (sign1 < 0)
    {
        return +1;
    }

    int sign2 = ToLine(test, v0, v1);
    if (sign2 > 0)
    {
        return +1;
    }

    return ((sign0 && sign1 && sign2) ? -1 : 0);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2<Real>::ToCircumcircle (int i, int v0, int v1, int v2) const
{
    return ToCircumcircle(mVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2<Real>::ToCircumcircle (const Vector2<Real>& test, int v0, int v1,
    int v2) const
{
    bool positive = Sort(v0, v1, v2);

    const Vector2<Real>& vec0 = mVertices[v0];
    const Vector2<Real>& vec1 = mVertices[v1];
    const Vector2<Real>& vec2 = mVertices[v2];

    Real s0x = vec0[0] + test[0];
    Real d0x = vec0[0] - test[0];
    Real s0y = vec0[1] + test[1];
    Real d0y = vec0[1] - test[1];
    Real s1x = vec1[0] + test[0];
    Real d1x = vec1[0] - test[0];
    Real s1y = vec1[1] + test[1];
    Real d1y = vec1[1] - test[1];
    Real s2x = vec2[0] + test[0];
    Real d2x = vec2[0] - test[0];
    Real s2y = vec2[1] + test[1];
    Real d2y = vec2[1] - test[1];
    Real z0 = s0x*d0x + s0y*d0y;
    Real z1 = s1x*d1x + s1y*d1y;
    Real z2 = s2x*d2x + s2y*d2y;

    Real det = Det3(d0x, d0y, z0, d1x, d1y, z1, d2x, d2y, z2);
    if (!positive)
    {
        det = -det;
    }

    return (det < (Real)0 ? 1 : (det > (Real)0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
Real Query2<Real>::Dot (Real x0, Real y0, Real x1, Real y1)
{
    return x0*x1 + y0*y1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Query2<Real>::Det2 (Real x0, Real y0, Real x1, Real y1)
{
    return x0*y1 - x1*y0;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Query2<Real>::Det3 (Real x0, Real y0, Real z0, Real x1, Real y1,
    Real z1, Real x2, Real y2, Real z2)
{
    Real c00 = y1*z2 - y2*z1;
    Real c01 = y2*z0 - y0*z2;
    Real c02 = y0*z1 - y1*z0;
    return x0*c00 + x1*c01 + x2*c02;
}
//----------------------------------------------------------------------------
