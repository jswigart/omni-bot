// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BILLBOARDNODE_H
#define WM5BILLBOARDNODE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Node.h"
#include "Wm5Camera.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM BillboardNode : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(BillboardNode);

public:
    // The model space of the billboard has an up vector of (0,1,0) that is
    // chosen to be the billboard's axis of rotation.

    // Construction and destruction.
    BillboardNode (Camera* camera = 0);
    virtual ~BillboardNode ();

    // The camera to which the billboard is aligned.
    inline void AlignTo (Camera* camera);

protected:
    // Support for the geometric update.
    virtual void UpdateWorldData (double applicationTime);

    CameraPtr mCamera;
};

WM5_REGISTER_STREAM(BillboardNode);
typedef Pointer0<BillboardNode> BillboardNodePtr;
#include "Wm5BillboardNode.inl"

}

#endif
