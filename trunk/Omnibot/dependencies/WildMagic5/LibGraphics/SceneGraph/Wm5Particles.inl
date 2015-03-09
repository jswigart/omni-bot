// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int Particles::GetNumParticles () const
{
    return mNumParticles;
}
//----------------------------------------------------------------------------
inline const Float4* Particles::GetPositionSizes () const
{
    return mPositionSizes;
}
//----------------------------------------------------------------------------
inline Float4* Particles::GetPositionSizes ()
{
    return mPositionSizes;
}
//----------------------------------------------------------------------------
inline float Particles::GetSizeAdjust () const
{
    return mSizeAdjust;
}
//----------------------------------------------------------------------------
inline int Particles::GetNumActive () const
{
    return mNumActive;
}
//----------------------------------------------------------------------------
