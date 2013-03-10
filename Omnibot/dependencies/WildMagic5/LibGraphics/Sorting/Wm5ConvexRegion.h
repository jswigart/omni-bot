// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONVEXREGION_H
#define WM5CONVEXREGION_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Node.h"

namespace Wm5
{

class Portal;

class WM5_GRAPHICS_ITEM ConvexRegion : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ConvexRegion);

public:
    // Construction and destruction.  ConvexRegion accepts responsibility
    // for deleting the input array of Portal*.
    ConvexRegion (int numPortals, Portal** portals);
    virtual ~ConvexRegion ();

    // Portal access.
    inline int GetNumPortals () const;
    inline Portal* GetPortal (int i) const;

protected:
    // Support for the geometric update.
    virtual void UpdateWorldData (double applicationTime);

    // Portals of the region (these are not set up to be shared).
    int mNumPortals;
    Portal** mPortals;

    // For region graph traversal.
    bool mVisited;

public_internal:
    // Culling.  ConvexRegionManager starts the region graph traversal
    // with the region containing the camera position.  Portal continues the
    // traversal.  Should you decide not to use a convex region manager and
    // track which region the eyepoint is in using your own system, you will
    // need to make sure the convex region that contains the camera position
    // is the first one visited during a depth-first traversal of the scene.
    virtual void GetVisibleSet (Culler& culler, bool noCull);
};

WM5_REGISTER_STREAM(ConvexRegion);
typedef Pointer0<ConvexRegion> ConvexRegionPtr;
#include "Wm5ConvexRegion.inl"

}

#endif
