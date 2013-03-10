// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline const BSplineVolumef* BoxSurface::GetVolume () const
{
    return mVolume;
}
//----------------------------------------------------------------------------
inline int BoxSurface::GetNumUSamples () const
{
    return mNumUSamples;
}
//----------------------------------------------------------------------------
inline int BoxSurface::GetNumVSamples () const
{
    return mNumVSamples;
}
//----------------------------------------------------------------------------
inline int BoxSurface::GetNumWSamples () const
{
    return mNumWSamples;
}
//----------------------------------------------------------------------------
