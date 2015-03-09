// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline void RectangleSurface::SetSurface (ParametricSurfacef* surface)
{
    mSurface = surface;
}
//----------------------------------------------------------------------------
inline const ParametricSurfacef* RectangleSurface::GetSurface () const
{
    return mSurface;
}
//----------------------------------------------------------------------------
inline int RectangleSurface::GetNumUSamples () const
{
    return mNumUSamples;
}
//----------------------------------------------------------------------------
inline int RectangleSurface::GetNumVSamples () const
{
    return mNumVSamples;
}
//----------------------------------------------------------------------------
