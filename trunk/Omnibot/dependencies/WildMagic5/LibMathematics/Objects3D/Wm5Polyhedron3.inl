// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Polyhedron3<Real>::Polyhedron3 (int numVertices, Vector3<Real>* vertices,
    int numTriangles, int* indices)
    :
    mNumVertices(numVertices),
    mNumTriangles(numTriangles),
    mNumIndices(3*numTriangles)
{
    // The polyhedron must be at least a tetrahedron.
    assertion(numVertices >= 4 && vertices && numTriangles >= 4 && indices,
        "Invalid input to Polyhedron3 constructor\n");

    mVertices = vertices;
    mIndices = indices;
}
//----------------------------------------------------------------------------
template <typename Real>
Polyhedron3<Real>::Polyhedron3 (const Polyhedron3& polyhedron)
{
    mVertices = 0;
    mIndices = 0;
    *this = polyhedron;
}
//----------------------------------------------------------------------------
template <typename Real>
Polyhedron3<Real>::~Polyhedron3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Polyhedron3<Real>& Polyhedron3<Real>::operator= (
    const Polyhedron3& polyhedron)
{
    mNumVertices = polyhedron.mNumVertices;
    mNumTriangles = polyhedron.mNumTriangles;
    mNumIndices = polyhedron.mNumIndices;
    mVertices = polyhedron.mVertices;
    mIndices = polyhedron.mIndices;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
int Polyhedron3<Real>::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* Polyhedron3<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& Polyhedron3<Real>::GetVertex (int i) const
{
    assertion(0 <= i && i < mNumVertices, "Invalid index in GetVertex\n");
    return mVertices[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int Polyhedron3<Real>::GetNumTriangles () const
{
    return mNumTriangles;
}
//----------------------------------------------------------------------------
template <typename Real>
int Polyhedron3<Real>::GetNumIndices () const
{
    return mNumIndices;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* Polyhedron3<Real>::GetIndices () const
{
    return mIndices;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* Polyhedron3<Real>::GetTriangle (int i) const
{
    assertion(0 <= i && i < mNumTriangles, "Invalid index in GetTriangle\n");
    return &mIndices[3*i];
}
//----------------------------------------------------------------------------
template <typename Real>
void Polyhedron3<Real>::SetVertex (int i, const Vector3<Real>& vertex)
{
    assertion(0 <= i && i < mNumVertices, "Invalid index in SetVertex\n");
    mVertices[i] = vertex;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Polyhedron3<Real>::ComputeVertexAverage () const
{
    Vector3<Real> average = mVertices[0];
    for (int i = 1; i < mNumVertices; ++i)
    {
        average += mVertices[i];
    }
    average /= (Real)mNumVertices;
    return average;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Polyhedron3<Real>::ComputeSurfaceArea () const
{
    Real surfaceArea = (Real)0;
    const int* indices = mIndices;
    for (int i = 0; i < mNumTriangles; ++i)
    {
        int v0 = *indices++;
        int v1 = *indices++;
        int v2 = *indices++;
        Vector3<Real> edge0 = mVertices[v1] - mVertices[v0];
        Vector3<Real> edge1 = mVertices[v2] - mVertices[v0];
        Vector3<Real> cross = edge0.Cross(edge1);
        surfaceArea += cross.Length();
    }
    surfaceArea *= (Real)0.5;
    return surfaceArea;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Polyhedron3<Real>::ComputeVolume () const
{
    Real volume = (Real)0;
    const int* indices = mIndices;
    for (int i = 0; i < mNumTriangles; ++i)
    {
        int v0 = *indices++;
        int v1 = *indices++;
        int v2 = *indices++;
        volume += mVertices[v0].Dot(mVertices[v1].Cross(mVertices[v2]));
    }
    volume /= (Real)6.0;
    return volume;
}
//----------------------------------------------------------------------------
