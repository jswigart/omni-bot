// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int ETNonmanifoldMesh::GetNumEdges () const
{
    return (int)mEMap.size();
}
//----------------------------------------------------------------------------
inline const ETNonmanifoldMesh::EMap& ETNonmanifoldMesh::GetEdges () const
{
    return mEMap;
}
//----------------------------------------------------------------------------
inline int ETNonmanifoldMesh::GetNumTriangles () const
{
    return (int)mTMap.size();
}
//----------------------------------------------------------------------------
inline const ETNonmanifoldMesh::TMap& ETNonmanifoldMesh::GetTriangles () const
{
    return mTMap;
}
//----------------------------------------------------------------------------
