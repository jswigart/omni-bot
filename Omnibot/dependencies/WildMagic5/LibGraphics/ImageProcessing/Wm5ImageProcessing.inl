// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int ImageProcessing::GetNumCols () const
{
    return mNumCols;
}
//----------------------------------------------------------------------------
inline int ImageProcessing::GetNumRows () const
{
    return mNumRows;
}
//----------------------------------------------------------------------------
inline int ImageProcessing::GetNumTargets () const
{
    return mNumTargets;
}
//----------------------------------------------------------------------------
inline float ImageProcessing::GetColSpacing () const
{
    return mColSpacing;
}
//----------------------------------------------------------------------------
inline float ImageProcessing::GetRowSpacing () const
{
    return mRowSpacing;
}
//----------------------------------------------------------------------------
inline Camera* ImageProcessing::GetCamera () const
{
    return mCamera;
}
//----------------------------------------------------------------------------
inline TriMesh* ImageProcessing::GetRectangle () const
{
    return mRectangle;
}
//----------------------------------------------------------------------------
inline RenderTarget* ImageProcessing::GetTarget (int i) const
{
    if (0 <= i && i < mNumTargets)
    {
        return mTargets[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
inline int ImageProcessing::Index (int col, int row) const
{
    return col + mNumCols*row;
}
//----------------------------------------------------------------------------
inline Texture2D* ImageProcessing::GetMainTexture () const
{
    return mMainTexture;
}
//----------------------------------------------------------------------------
inline VisualEffectInstance* ImageProcessing::GetMainEffectInstance () const
{
    return mMainEffectInstance;
}
//----------------------------------------------------------------------------
