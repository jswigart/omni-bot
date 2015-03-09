// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline bool BasicMesh::IsValid () const
{
    return mIsValid;
}
//----------------------------------------------------------------------------
inline int BasicMesh::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
inline int BasicMesh::GetNumEdges () const
{
    return mNumEdges;
}
//----------------------------------------------------------------------------
inline int BasicMesh::GetNumTriangles () const
{
    return mNumTriangles;
}
//----------------------------------------------------------------------------
inline const void* BasicMesh::GetPoints () const
{
    return mPoints;
}
//----------------------------------------------------------------------------
inline const int* BasicMesh::GetIndices () const
{
    return mIndices;
}
//----------------------------------------------------------------------------
inline const BasicMesh::Vertex* BasicMesh::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
inline const BasicMesh::Edge* BasicMesh::GetEdges () const
{
    return mEdges;
}
//----------------------------------------------------------------------------
inline const BasicMesh::Triangle* BasicMesh::GetTriangles () const
{
    return mTriangles;
}
//----------------------------------------------------------------------------
