// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5GLOBALEFFECT_H
#define WM5GLOBALEFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5Renderer.h"
#include "Wm5VisibleSet.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM GlobalEffect : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(GlobalEffect);

protected:
    // Abstract base class.  Construction and destruction.
    GlobalEffect ();
public:
    virtual ~GlobalEffect ();

    // This function is called in Renderer::Draw(visibleSet,globalEffect)
    // when the globalEffect is nonnull.
    virtual void Draw (Renderer* renderer, const VisibleSet& visibleSet) = 0;
};

WM5_REGISTER_STREAM(GlobalEffect);
typedef Pointer0<GlobalEffect> GlobalEffectPtr;

}

#endif
