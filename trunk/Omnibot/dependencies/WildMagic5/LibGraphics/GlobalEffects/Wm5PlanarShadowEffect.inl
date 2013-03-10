// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int PlanarShadowEffect::GetNumPlanes () const
{
    return mNumPlanes;
}
//----------------------------------------------------------------------------
inline void PlanarShadowEffect::SetPlane (int i, TriMesh* plane)
{
    // The culling flag is set to "always" because this effect is responsible
    // for drawing the TriMesh.  This prevents drawing attempts by another
    // scene graph for which 'plane' is a leaf node.
    mPlanes[i] = plane;
    mPlanes[i]->Culling = Spatial::CULL_ALWAYS;
}
//----------------------------------------------------------------------------
inline TriMesh* PlanarShadowEffect::GetPlane (int i) const
{
    return mPlanes[i];
}
//----------------------------------------------------------------------------
inline void PlanarShadowEffect::SetProjector (int i, Light* projector)
{
    mProjectors[i] = projector;
}
//----------------------------------------------------------------------------
inline Light* PlanarShadowEffect::GetProjector (int i) const
{
    return mProjectors[i];
}
//----------------------------------------------------------------------------
inline void PlanarShadowEffect::SetShadowColor (int i,
    const Float4& shadowColor)
{
    mShadowColors[i] = shadowColor;
}
//----------------------------------------------------------------------------
inline const Float4& PlanarShadowEffect::GetShadowColor (int i) const
{
    return mShadowColors[i];
}
//----------------------------------------------------------------------------
