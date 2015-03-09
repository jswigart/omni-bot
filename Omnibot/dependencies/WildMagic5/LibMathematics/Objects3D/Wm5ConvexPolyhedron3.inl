// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron3<Real>::ConvexPolyhedron3 (int numVertices,
    Vector3<Real>* vertices, int numTriangles, int* indices,
    NCPlane* planes)
    :
    Polyhedron3<Real>(numVertices, vertices, numTriangles, indices)
{
    if (planes)
    {
        mPlanes = planes;
    }
    else
    {
        mPlanes = new1<NCPlane>(mNumTriangles);
        UpdatePlanes();
    }
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron3<Real>::ConvexPolyhedron3 (
    const ConvexPolyhedron3& polyhedron)
    :
    Polyhedron3<Real>(polyhedron)
{
    mPlanes = 0;
    *this = polyhedron;
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron3<Real>::~ConvexPolyhedron3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron3<Real>& ConvexPolyhedron3<Real>::operator= (
    const ConvexPolyhedron3& polyhedron)
{
    Polyhedron3<Real>::operator=(polyhedron);
    mPlanes = polyhedron.mPlanes;
    mSharingTriangles = polyhedron.mSharingTriangles;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename ConvexPolyhedron3<Real>::NCPlane*
ConvexPolyhedron3<Real>::GetPlanes () const
{
    return mPlanes;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename ConvexPolyhedron3<Real>::NCPlane&
ConvexPolyhedron3<Real>::GetPlane (int i) const
{
    assertion(0 <= i && i < mNumTriangles, "Invalid index in GetPlane\n");
    return mPlanes[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron3<Real>::SetVertex (int i, const Vector3<Real>& vertex)
{
    Polyhedron3<Real>::SetVertex(i, vertex);

    // Keep track of faces sharing this vertex.  Their planes need to be
    // updated later.
    const int* indices = mIndices;
    for (int j = 0; j < mNumTriangles; ++j)
    {
        int v0 = *indices++;
        int v1 = *indices++;
        int v2 = *indices++;
        if (i == v0 || i == v1 || i == v2)
        {
            mSharingTriangles.insert(j);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron3<Real>::UpdatePlanes ()
{
    if (mSharingTriangles.size() > 0)
    {
        Vector3<Real> average = this->ComputeVertexAverage();
        for (int i = 0; i < mNumTriangles; ++i)
        {
            UpdatePlane(i, average);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron3<Real>::IsConvex (Real threshold) const
{
    Real maxDistance = -Math<Real>::MAX_REAL;
    Real minDistance = Math<Real>::MAX_REAL;

    for (int j = 0; j < mNumTriangles; ++j)
    {
        const NCPlane& plane = mPlanes[j];
        for (int i = 0; i < mNumVertices; ++i)
        {
            Real distance = plane.first.Dot(mVertices[i]) - plane.second;
            if (distance < minDistance)
            {
                minDistance = distance;
            }
            if (distance > maxDistance)
            {
                maxDistance = distance;
            }
            if (distance < threshold)
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron3<Real>::Contains (const Vector3<Real>& p,
    Real threshold) const
{
    for (int i = 0; i < mNumTriangles; ++i)
    {
        const NCPlane& plane = mPlanes[i];
        Real distance = plane.first.Dot(p) - plane.second;
        if (distance < threshold)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron3<Real>::UpdatePlane (int i,
    const Vector3<Real>& average)
{
    int base = 3*i;
    int v0 = mIndices[base++];
    int v1 = mIndices[base++];
    int v2 = mIndices[base];

    Vector3<Real>& vertex0 = mVertices[v0];
    Vector3<Real>& vertex1 = mVertices[v1];
    Vector3<Real>& vertex2 = mVertices[v2];

    Vector3<Real> diff = average - vertex0;
    Vector3<Real> edge1 = vertex1 - vertex0;
    Vector3<Real> edge2 = vertex2 - vertex0;
    Vector3<Real> normal = edge2.Cross(edge1);
    Real length = normal.Length();
    if (length > Math<Real>::ZERO_TOLERANCE)
    {
        normal /= length;
        Real dot = normal.Dot(diff);
        assertion(dot >= (Real)0, "Dot product must be nonnegative\n");
        if (dot < (Real)0)
        {
            normal = -normal;
        }
    }
    else
    {
        // The triangle is degenerate.  Use a "normal" that points towards
        // the average.
        normal = diff;
        normal.Normalize();
    }

    // This plane has an inner-pointing normal.
    mPlanes[i].first = normal;
    mPlanes[i].second = normal.Dot(vertex0);
}
//----------------------------------------------------------------------------
