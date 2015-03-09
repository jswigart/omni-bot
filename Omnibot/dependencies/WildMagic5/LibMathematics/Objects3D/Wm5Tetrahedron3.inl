// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Tetrahedron3<Real>::Tetrahedron3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Tetrahedron3<Real>::~Tetrahedron3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Tetrahedron3<Real>::Tetrahedron3 (const Vector3<Real>& v0,
    const Vector3<Real>& v1, const Vector3<Real>& v2,
    const Vector3<Real>& v3)
{
    V[0] = v0;
    V[1] = v1;
    V[2] = v2;
    V[3] = v3;
}
//----------------------------------------------------------------------------
template <typename Real>
Tetrahedron3<Real>::Tetrahedron3 (const Vector3<Real> vertex[4])
{
    V[0] = vertex[0];
    V[1] = vertex[1];
    V[2] = vertex[2];
    V[3] = vertex[3];
}
//----------------------------------------------------------------------------
template <typename Real>
void Tetrahedron3<Real>::GetFaceIndices (int face, int index[3]) const
{
    // assert:  0 <= face <= 3
    if (face == 0)
    {
        index[0] = 0;
        index[1] = 2;
        index[2] = 1;
    }
    else if (face == 1)
    {
        index[0] = 0;
        index[1] = 1;
        index[2] = 3;
    }
    else if (face == 2)
    {
        index[0] = 0;
        index[1] = 3;
        index[2] = 2;
    }
    else  // face == 3 (no index validation is performed)
    {
        index[0] = 1;
        index[1] = 2;
        index[2] = 3;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Tetrahedron3<Real>::GetPlanes (Plane3<Real> plane[4]) const
{
    Vector3<Real> edge10 = V[1] - V[0];
    Vector3<Real> edge20 = V[2] - V[0];
    Vector3<Real> edge30 = V[3] - V[0];
    Vector3<Real> edge21 = V[2] - V[1];
    Vector3<Real> edge31 = V[3] - V[1];

    plane[0].Normal = edge20.UnitCross(edge10);  // <v0,v2,v1>
    plane[1].Normal = edge10.UnitCross(edge30);  // <v0,v1,v3>
    plane[2].Normal = edge30.UnitCross(edge20);  // <v0,v3,v2>
    plane[3].Normal = edge21.UnitCross(edge31);  // <v1,v2,v3>

    Real det = edge10.Dot(plane[3].Normal);
    if (det < (Real)0)
    {
        // The normals are inner pointing, reverse their directions.
        for (int i = 0; i < 4; ++i)
        {
            plane[i].Normal = -plane[i].Normal;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        plane[i].Constant = V[i].Dot(plane[i].Normal);
    }
}
//----------------------------------------------------------------------------
