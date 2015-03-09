// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query2Filtered<Real>::Query2Filtered (int numVertices,
    const Vector2<Real>* vertices, Real uncertainty)
    :
    Query2<Real>(numVertices, vertices),
    mRQuery(numVertices, vertices),
    mUncertainty(uncertainty)
{
    assertion((Real)0 <= mUncertainty && mUncertainty <= (Real)1,
        "Invalid uncertainty\n");
}
//----------------------------------------------------------------------------
template <typename Real>
Query2Filtered<Real>::~Query2Filtered ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query2Filtered<Real>::GetType () const
{
    return Query::QT_FILTERED;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Filtered<Real>::ToLine (const Vector2<Real>& test, int v0, int v1)
    const
{
    const Vector2<Real>& vec0 = mVertices[v0];
    const Vector2<Real>& vec1 = mVertices[v1];

    Real x0 = test[0] - vec0[0];
    Real y0 = test[1] - vec0[1];
    Real x1 = vec1[0] - vec0[0];
    Real y1 = vec1[1] - vec0[1];

    Real len0 = Math<Real>::Sqrt(x0*x0 + y0*y0);
    Real len1 = Math<Real>::Sqrt(x1*x1 + y1*y1);
    Real scaledUncertainty = mUncertainty*len0*len1;

    Real det = Det2(x0, y0, x1, y1);
    if (Math<Real>::FAbs(det) >= scaledUncertainty)
    {
        return (det > (Real)0 ? +1 : (det < (Real)0 ? -1 : 0));
    }

    return mRQuery.ToLine(test, v0, v1);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query2Filtered<Real>::ToCircumcircle (const Vector2<Real>& test, int v0,
    int v1, int v2) const
{
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

    Real len0 = Math<Real>::Sqrt(d0x*d0x + d0y*d0y + z0*z0);
    Real len1 = Math<Real>::Sqrt(d1x*d1x + d1y*d1y + z1*z1);
    Real len2 = Math<Real>::Sqrt(d2x*d2x + d2y*d2y + z2*z2);
    Real scaledUncertainty = mUncertainty*len0*len1*len2;

    Real det = Det3(d0x, d0y, z0, d1x, d1y, z1, d2x, d2y, z2);
    if (Math<Real>::FAbs(det) >= scaledUncertainty)
    {
        return (det < (Real)0 ? 1 : (det > (Real)0 ? -1 : 0));
    }

    return mRQuery.ToCircumcircle(test, v0, v1, v2);
}
//----------------------------------------------------------------------------
