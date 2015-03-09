// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5LIGHTNODE_H
#define WM5LIGHTNODE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Light.h"
#include "Wm5Node.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM LightNode : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(LightNode);

public:
    // Construction and destruction.  The node's world translation is used
    // as the light's location.  The node's world rotation matrix is used
    // for the light's coordinate axes.  Column 0 of the world rotation
    // matrix is the light's direction vector, column 1 of the world rotation
    // matrix is the light's up vector, and column 2 of the world rotation
    // matrix is the light's right vector.
    //
    // On construction, the node's local transformation is set to the
    // light's current coordinate system.
    //   local translation       = light location
    //   local rotation column 0 = light direction
    //   local rotation column 1 = light up
    //   local rotation column 2 = light right
    LightNode (Light* light = 0);
    virtual ~LightNode ();

    // When you set the light, the node's local transformation is set to the
    // light's current current coordinate system.  The node's world
    // transformation is computed, and the light's coordinate system is set
    // to use the node's light transformation.
    void SetLight (Light* light);

    // Member access.
    inline Light* GetLight ();
    inline const Light* GetLight () const;

protected:
    // Geometric updates.
    virtual void UpdateWorldData (double applicationTime);

    LightPtr mLight;
};

WM5_REGISTER_STREAM(LightNode);
typedef Pointer0<LightNode> LightNodePtr;
#include "Wm5LightNode.inl"

}

#endif
