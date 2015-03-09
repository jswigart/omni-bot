// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline void TubeSurface::SetMedial (Curve3f* medial)
{
    mMedial = medial;
}
//----------------------------------------------------------------------------
inline const Curve3f* TubeSurface::GetMedial () const
{
    return mMedial;
}
//----------------------------------------------------------------------------
inline void TubeSurface::SetRadial (RadialFunction radial)
{
    mRadial = radial;
}
//----------------------------------------------------------------------------
inline TubeSurface::RadialFunction TubeSurface::GetRadial () const
{
    return mRadial;
}
//----------------------------------------------------------------------------
inline void TubeSurface::SetUpVector (const Vector3f& upVector)
{
    mUpVector = upVector;
}
//----------------------------------------------------------------------------
inline const Vector3f& TubeSurface::GetUpVector () const
{
    return mUpVector;
}
//----------------------------------------------------------------------------
inline int TubeSurface::GetNumMedialSamples () const
{
    return mNumMedialSamples;
}
//----------------------------------------------------------------------------
inline int TubeSurface::GetNumSliceSamples () const
{
    return mNumSliceSamples;
}
//----------------------------------------------------------------------------
inline int TubeSurface::Index (int s, int m)
{
    return s + (mNumSliceSamples + 1)*m;
}
//----------------------------------------------------------------------------
