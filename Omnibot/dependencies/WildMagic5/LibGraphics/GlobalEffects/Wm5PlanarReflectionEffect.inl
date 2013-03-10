// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int PlanarReflectionEffect::GetNumPlanes () const
{
    return mNumPlanes;
}
//----------------------------------------------------------------------------
inline void PlanarReflectionEffect::SetPlane (int i, TriMesh* plane)
{
    // The culling flag is set to "always" because this effect is responsible
    // for drawing the TriMesh.  This prevents drawing attempts by another
    // scene graph for which 'plane' is a leaf node.
    mPlanes[i] = plane;
    mPlanes[i]->Culling = Spatial::CULL_ALWAYS;
}
//----------------------------------------------------------------------------
inline TriMesh* PlanarReflectionEffect::GetPlane (int i) const
{
    return mPlanes[i];
}
//----------------------------------------------------------------------------
inline void PlanarReflectionEffect::SetReflectance (int i, float reflectance)
{
    mReflectances[i] = reflectance;
}
//----------------------------------------------------------------------------
inline float PlanarReflectionEffect::GetReflectance (int i) const
{
    return mReflectances[i];
}
//----------------------------------------------------------------------------
