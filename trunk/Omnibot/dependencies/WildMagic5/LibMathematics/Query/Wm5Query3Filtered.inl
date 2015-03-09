// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Query3Filtered<Real>::Query3Filtered (int numVertices,
    const Vector3<Real>* vertices, Real uncertainty)
    :
    Query3<Real>(numVertices, vertices),
    mRQuery(numVertices, vertices),
    mUncertainty(uncertainty)
{
    assertion((Real)0 <= mUncertainty && mUncertainty <= (Real)1,
        "Invalid uncertainty\n");
}
//----------------------------------------------------------------------------
template <typename Real>
Query3Filtered<Real>::~Query3Filtered ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Query::Type Query3Filtered<Real>::GetType () const
{
    return Query::QT_FILTERED;
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Filtered<Real>::ToPlane (const Vector3<Real>& test, int v0, int v1,
    int v2) const
{
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

    Real len0 = Math<Real>::Sqrt(x0*x0 + y0*y0 + z0*z0);
    Real len1 = Math<Real>::Sqrt(x1*x1 + y1*y1 + z1*z1);
    Real len2 = Math<Real>::Sqrt(x2*x2 + y2*y2 + z2*z2);
    Real scaledUncertainty = mUncertainty*len0*len1*len2;

    Real det = Det3(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    if (Math<Real>::FAbs(det) >= scaledUncertainty)
    {
        return (det > (Real)0 ? +1 : (det < (Real)0 ? -1 : 0));
    }

    return mRQuery.ToPlane(test, v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int Query3Filtered<Real>::ToCircumsphere (const Vector3<Real>& test, int v0,
    int v1, int v2, int v3) const
{
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

    Real len0 = Math<Real>::Sqrt(d0x*d0x+d0y*d0y+d0z*d0z+w0*w0);
    Real len1 = Math<Real>::Sqrt(d1x*d1x+d1y*d1y+d1z*d1z+w1*w1);
    Real len2 = Math<Real>::Sqrt(d2x*d2x+d2y*d2y+d2z*d2z+w2*w2);
    Real len3 = Math<Real>::Sqrt(d3x*d3x+d3y*d3y+d3z*d3z+w3*w3);
    Real scaledUncertainty = mUncertainty*len0*len1*len2*len3;

    Real det = Det4(d0x, d0y, d0z, w0, d1x, d1y, d1z, w1, d2x, d2y, d2z,
        w2, d3x, d3y, d3z, w3);

    if (Math<Real>::FAbs(det) >= scaledUncertainty)
    {
        return (det > (Real)0 ? 1 : (det < (Real)0 ? -1 : 0));
    }

    return mRQuery.ToCircumsphere(test, v0, v1, v2, v3);
}
//----------------------------------------------------------------------------
