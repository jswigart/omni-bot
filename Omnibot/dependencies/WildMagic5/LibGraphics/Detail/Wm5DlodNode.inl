// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline APoint& DlodNode::ModelCenter ()
{
    return mModelLodCenter;
}
//----------------------------------------------------------------------------
inline const APoint& DlodNode::GetModelCenter () const
{
    return mModelLodCenter;
}
//----------------------------------------------------------------------------
inline const APoint& DlodNode::GetWorldCenter () const
{
    return mWorldLodCenter;
}
//----------------------------------------------------------------------------
inline int DlodNode::GetNumLevelsOfDetail () const
{
    return mNumLevelsOfDetail;
}
//----------------------------------------------------------------------------
inline float DlodNode::GetModelMinDistance (int i) const
{
    assertion(0 <= i && i < mNumLevelsOfDetail, 
        "Invalid index in GetModelMinDistance.\n");

    return mModelMinDist[i];
}
//----------------------------------------------------------------------------
inline float DlodNode::GetModelMaxDistance (int i) const
{
    assertion(0 <= i && i < mNumLevelsOfDetail, 
        "Invalid index in GetModelMaxDistance.\n");

    return mModelMaxDist[i];
}
//----------------------------------------------------------------------------
inline float DlodNode::GetWorldMinDistance (int i) const
{
    assertion(0 <= i && i < mNumLevelsOfDetail, 
        "Invalid index in GetWorldMinDistance.\n");

    return mWorldMinDist[i];
}
//----------------------------------------------------------------------------
inline float DlodNode::GetWorldMaxDistance (int i) const
{
    assertion(0 <= i && i < mNumLevelsOfDetail, 
        "Invalid index in GetWorldMaxDistance.\n");

    return mWorldMaxDist[i];
}
//----------------------------------------------------------------------------
