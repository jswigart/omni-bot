// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PORTAL_H
#define WM5PORTAL_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Spatial.h"

namespace Wm5
{

class ConvexRegion;
class Culler;

class WM5_GRAPHICS_ITEM Portal : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Portal);

public:
    // The portal is a unidirectional connector between two regions.  The
    // vertices of the portal must satisfy the following constraints:
    // 1. They must form a planar simple polygon (numVertices >= 3 is
    //    implied).  The polygon does not have to be convex.
    // 2. They must be counterclockwise ordered when looking through the
    //    portal to the adjacent region.
    // 3. They must be in the model-space coordinates for the region that
    //    contains the portal.

    // Construction and destruction.  Portal accepts responsibility for
    // deleting the input array.  The model vertices must be counterclockwise
    // oriented in the model plane when viewed from the side of the plane to
    // which the plane normal points.
    Portal (int numVertices, APoint* modelVertices, const HPlane& modelPlane,
        ConvexRegion* adjacentRegion, bool open);

    virtual ~Portal ();

    // Member access.  The region to which the portal leads.  Portals can be
    // open or closed.
    ConvexRegion* AdjacentRegion;
    bool Open;

protected:
    // Support for the graph update.
    friend class ConvexRegion;
    void UpdateWorldData (const Transform& worldTransform);

    // Support for culling.
    bool ReducedFrustum (const Culler& culler, float reducedFrustum[6]);
    void GetVisibleSet (Culler& culler, bool noCull);

    // Portal vertices and their planes.
    int mNumVertices;
    APoint* mModelVertices;
    APoint* mWorldVertices;
    HPlane mModelPlane;
    HPlane mWorldPlane;
};

WM5_REGISTER_STREAM(Portal);
typedef Pointer0<Portal> PortalPtr;

}

#endif
