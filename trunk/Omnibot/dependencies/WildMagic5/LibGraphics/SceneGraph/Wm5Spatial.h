// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SPATIAL_H
#define WM5SPATIAL_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ControlledObject.h"
#include "Wm5Bound.h"
#include "Wm5Transform.h"

namespace Wm5
{

class Culler;

class WM5_GRAPHICS_ITEM Spatial : public ControlledObject
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Spatial);

protected:
    // Abstract base class.  Construction and destruction.
    Spatial ();
public:
    virtual ~Spatial ();

    // Local and world transforms.  In some situations you might need to set
    // the world transform directly and bypass the Spatial::Update()
    // mechanism.  If World is set directly, the mWorldIsCurrent flag should
    // be set to 'true'.
    Transform LocalTransform;
    Transform WorldTransform;
    bool WorldTransformIsCurrent;

    // World bound access.  In some situations you might want to set the
    // world bound directly and bypass the Spatial::Update() mechanism.  If
    // mWorldBound is set directly, the mWorldBoundIsCurrent flag should be
    // set to 'true'.
    Bound WorldBound;
    bool WorldBoundIsCurrent;

    // Culling parameters.
    enum WM5_GRAPHICS_ITEM CullingMode
    {
        // Determine visibility state by comparing the world bounding volume
        // to culling planes.
        CULL_DYNAMIC,

        // Force the object to be culled.  If a Node is culled, its entire
        // subtree is culled.
        CULL_ALWAYS,

        // Never cull the object.  If a Node is never culled, its entire
        // subtree is never culled.  To accomplish this, the first time such
        // a Node is encountered, the bNoCull parameter is set to 'true' in
        // the recursive chain GetVisibleSet/OnGetVisibleSet.
        CULL_NEVER
    };

    CullingMode Culling;

    // Update of geometric state and controllers.  The function computes world
    // transformations on the downward pass of the scene graph traversal and
    // world bounding volumes on the upward pass of the traversal.
    void Update (double applicationTime = -Mathd::MAX_REAL,
        bool initiator = true);

    // Access to the parent object.
    inline Spatial* GetParent ();

protected:
    // Support for the geometric update.
    virtual void UpdateWorldData (double applicationTime);
    virtual void UpdateWorldBound () = 0;
    void PropagateBoundToRoot ();

public_internal:
    // Support for hierarchical culling.
    void OnGetVisibleSet (Culler& culler, bool noCull);
    virtual void GetVisibleSet (Culler& culler, bool noCull) = 0;

    // Access to the parent object.  Node calls this during attach/detach of
    // children.
    inline void SetParent (Spatial* parent);

protected:
    // Support for a hierarchical scene graph.  Spatial provides the parent
    // pointer.  Node provides the child pointers.
    Spatial* mParent;
};

WM5_REGISTER_STREAM(Spatial);
typedef Pointer0<Spatial> SpatialPtr;
#include "Wm5Spatial.inl"

}

#endif
