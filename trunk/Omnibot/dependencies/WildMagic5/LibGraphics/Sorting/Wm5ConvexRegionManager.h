// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONVEXREGIONMANAGER_H
#define WM5CONVEXREGIONMANAGER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5BspNode.h"
#include "Wm5ConvexRegion.h"
#include "Wm5CRMCuller.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ConvexRegionManager : public BspNode
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ConvexRegionManager);

public:
    // Construction and destruction.  The BSP tree should be built so that
    // the leaf nodes are where the ConvexRegion objects are located.
    ConvexRegionManager ();
    ConvexRegionManager (const HPlane& modelPlane);
    virtual ~ConvexRegionManager ();

    // The middle child of ConvexRegionManager is where the representation
    // of the outside of the set of regions is stored.  This can be an
    // arbitrary subgraph, not just drawable geometry.
    SpatialPtr AttachOutside (Spatial* outside);
    SpatialPtr DetachOutside ();
    SpatialPtr GetOutside ();

    // Determine region that contains the point.  If the point is outside
    // the set of regions, the return values is null.
    ConvexRegion* GetContainingRegion (const APoint& point);

protected:
    // Support for culling.
    virtual void GetVisibleSet (Culler& culler, bool noCull);

    CRMCuller mCuller;
};

WM5_REGISTER_STREAM(ConvexRegionManager);
typedef Pointer0<ConvexRegionManager> ConvexRegionManagerPtr;

}

#endif
