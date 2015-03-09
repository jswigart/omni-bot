// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SWITCHNODE_H
#define WM5SWITCHNODE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Node.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM SwitchNode : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SwitchNode);

public:
    // Construction and destruction.
    SwitchNode ();
    virtual ~SwitchNode ();

    enum { SN_INVALID_CHILD = -1 };

    inline void SetActiveChild (int activeChild);
    inline int GetActiveChild () const;
    inline void DisableAllChildren ();

protected:
    // Support for hierarchical culling.
    virtual void GetVisibleSet (Culler& culler, bool noCull);

    int mActiveChild;
};

WM5_REGISTER_STREAM(SwitchNode);
typedef Pointer0<SwitchNode> SwitchNodePtr;
#include "Wm5SwitchNode.inl"

}

#endif
