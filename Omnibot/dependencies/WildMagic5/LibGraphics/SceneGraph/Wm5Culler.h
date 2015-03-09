// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CULLER_H
#define WM5CULLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Camera.h"
#include "Wm5Spatial.h"
#include "Wm5VisibleSet.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Culler
{
public:
    // Construction and destruction.  Culling requires a camera model.  If the
    // camera is not passed to the constructor, you should set it using
    // SetCamera before calling ComputeVisibleSet.
    Culler (const Camera* camera = 0);
    virtual ~Culler ();

    // Access to the camera, frustum copy, and potentially visible set.
    inline void SetCamera (const Camera* camera);
    inline const Camera* GetCamera () const;
    void SetFrustum (const float* frustum);
    inline const float* GetFrustum () const;
    inline VisibleSet& GetVisibleSet ();

    // The base class behavior is to append the visible object to the end of
    // the visible set (stored as an array).  Derived classes may override
    // this behavior; for example, the array might be maintained as a sorted
    // array for minimizing render state changes or it might be/ maintained
    // as a unique list of objects for a portal system.
    virtual void Insert (Spatial* visible);

    // Access to the stack of world culling planes.  You may push and pop
    // planes to be used in addition to the view frustum planes.  PushPlane
    // requires the input plane to be in world coordinates.  See the comments
    // before data member mPlaneState about the bit system for enabling and
    // disabling planes during culling.
    enum { MAX_PLANE_QUANTITY = 32 };
    inline int GetPlaneQuantity () const;
    inline const HPlane* GetPlanes () const;
    inline void SetPlaneState (unsigned int planeState);
    inline unsigned int GetPlaneState () const;
    inline void PushPlane (const HPlane& plane);
    inline void PopPlane ();

    // Compare the object's world bound against the culling planes.  Only
    // Spatial calls this function.
    bool IsVisible (const Bound& bound);

    // Support for Portal::GetVisibleSet.
    bool IsVisible (int numVertices, const APoint* vertices,
        bool ignoreNearPlane);

    // Support for BspNode::GetVisibleSet.  Determine whether the view frustum
    // is fully on one side of a plane.  The "positive side" of the plane is
    // the half space to which the plane normal points.  The "negative side"
    // is the other half space.  The function returns +1 if the view frustum
    // is fully on the positive side of the plane, -1 if the view frustum is
    // fully on the negative side of the plane, or 0 if the view frustum
    // straddles the plane.  The input plane is in world coordinates and the
    // world camera coordinate system is used for the test.
    int WhichSide (const HPlane& plane) const;

    // This is the main function you should use for culling within a scene
    // graph.  Traverse the scene graph and construct the potentially visible
    // set relative to the world planes.
    void ComputeVisibleSet (Spatial* scene);

protected:
    // The input camera has information that might be needed during the
    // culling pass over the scene.
    const Camera* mCamera;

    // A copy of the view frustum for the input camera.  This allows various
    // subsystems to change the frustum parameters during culling (for
    // example, the portal system) without affecting the camera, whose initial
    // state is needed by the renderer.
    float mFrustum[Camera::VF_QUANTITY];

    // The world culling planes corresponding to the view frustum plus any
    // additional user-defined culling planes.  The member m_uiPlaneState
    // represents bit flags to store whether or not a plane is active in the
    // culling system.  A bit of 1 means the plane is active, otherwise the
    // plane is inactive.  An active plane is compared to bounding volumes,
    // whereas an inactive plane is not.  This supports an efficient culling
    // of a hierarchy.  For example, if a node's bounding volume is inside
    // the left plane of the view frustum, then the left plane is set to
    // inactive because the children of the node are automatically all inside
    // the left plane.
    int mPlaneQuantity;
    HPlane mPlane[MAX_PLANE_QUANTITY];
    unsigned int mPlaneState;

    // The potentially visible set for a call to GetVisibleSet.
    VisibleSet mVisibleSet;
};

#include "Wm5Culler.inl"

}

#endif
