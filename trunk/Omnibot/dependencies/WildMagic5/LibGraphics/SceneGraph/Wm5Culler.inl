// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline void Culler::SetCamera (const Camera* camera)
{
    mCamera = camera;
}
//----------------------------------------------------------------------------
inline const Camera* Culler::GetCamera () const
{
    return mCamera;
}
//----------------------------------------------------------------------------
inline const float* Culler::GetFrustum () const
{
    return mFrustum;
}
//----------------------------------------------------------------------------
inline VisibleSet& Culler::GetVisibleSet ()
{
    return mVisibleSet;
}
//----------------------------------------------------------------------------
inline int Culler::GetPlaneQuantity () const
{
    return mPlaneQuantity;
}
//----------------------------------------------------------------------------
inline const HPlane* Culler::GetPlanes () const
{
    return mPlane;
}
//----------------------------------------------------------------------------
inline void Culler::SetPlaneState (unsigned int planeState)
{
    mPlaneState = planeState;
}
//----------------------------------------------------------------------------
inline unsigned int Culler::GetPlaneState () const
{
    return mPlaneState;
}
//----------------------------------------------------------------------------
inline void Culler::PushPlane (const HPlane& plane)
{
    if (mPlaneQuantity < MAX_PLANE_QUANTITY)
    {
        // The number of user-defined planes is limited.
        mPlane[mPlaneQuantity] = plane;
        ++mPlaneQuantity;
    }
}
//----------------------------------------------------------------------------
inline void Culler::PopPlane ()
{
    if (mPlaneQuantity > Camera::VF_QUANTITY)
    {
        // Frustum planes may not be removed from the stack.
        --mPlaneQuantity;
    }
}
//----------------------------------------------------------------------------
