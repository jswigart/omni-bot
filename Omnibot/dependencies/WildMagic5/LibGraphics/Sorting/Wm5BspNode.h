// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPNODE_H
#define WM5BSPNODE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Node.h"
#include "Wm5HPlane.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM BspNode : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(BspNode);

public:
    // Construction and destruction.  The base class Node has *three* children
    // and is not allowed to grow.  The first and last children (indices 0 and
    // 2) are the "positive" and "negative" children of the binary tree.  The
    // positive child corresponds to the positive side of the separating
    // plane.  The negative child corresponds to the negative side of the
    // separating plane.  The middle child slot is where additional geometry
    // may be attached such as the triangles that are coplanar with the
    // separating plane.
    BspNode ();
    BspNode (const HPlane& modelPlane);
    virtual ~BspNode ();

    // These methods should be used instead of the attach/detach methods in
    // the Node base class.
    inline SpatialPtr AttachPositiveChild (Spatial* child);
    inline SpatialPtr AttachCoplanarChild (Spatial* child);
    inline SpatialPtr AttachNegativeChild (Spatial* child);
    inline SpatialPtr DetachPositiveChild ();
    inline SpatialPtr DetachCoplanarChild ();
    inline SpatialPtr DetachNegativeChild ();
    inline SpatialPtr GetPositiveChild ();
    inline SpatialPtr GetCoplanarChild ();
    inline SpatialPtr GetNegativeChild ();

    // Member access.
    HPlane ModelPlane;
    inline const HPlane& GetWorldPlane () const;

    // Determine the portion of the scene that contains the point.
    Spatial* GetContainingNode (const APoint& point);

protected:
    // Support for the geometric update.
    virtual void UpdateWorldData (double applicationTime);

    // Support for hierarchical culling.
    virtual void GetVisibleSet (Culler& culler, bool noCull);

    HPlane mWorldPlane;
};

WM5_REGISTER_STREAM(BspNode);
typedef Pointer0<BspNode> BspNodePtr;
#include "Wm5BspNode.inl"

}

#endif
