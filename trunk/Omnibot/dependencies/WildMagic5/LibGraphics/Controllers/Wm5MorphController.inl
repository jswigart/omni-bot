// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int MorphController::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
inline int MorphController::GetNumTargets () const
{
    return mNumTargets;
}
//----------------------------------------------------------------------------
inline int MorphController::GetNumKeys () const
{
    return mNumKeys;
}
//----------------------------------------------------------------------------
inline APoint* MorphController::GetBaseVertices () const
{
    return mVertices[0];
}
//----------------------------------------------------------------------------
inline AVector* MorphController::GetDeltaVertices (int target) const
{
    return (AVector*)mVertices[target];
}
//----------------------------------------------------------------------------
inline float* MorphController::GetTimes () const
{
    return mTimes;
}
//----------------------------------------------------------------------------
inline float* MorphController::GetWeights (int key) const
{
    return mWeights[key];
}
//----------------------------------------------------------------------------
