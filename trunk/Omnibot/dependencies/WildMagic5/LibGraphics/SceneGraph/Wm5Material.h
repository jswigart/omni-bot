// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MATERIAL_H
#define WM5MATERIAL_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5Float4.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Material : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Material);

public:
    // Construction and destruction.
    Material ();
    virtual ~Material ();

    Float4 Emissive;  // default: (0,0,0,1)
    Float4 Ambient;   // default: (0,0,0,1)

    // The material alpha is the alpha channel of mDiffuse.
    Float4 Diffuse;   // default: (0,0,0,1)

    // The material specular exponent is in the alpha channel of mSpecular.
    Float4 Specular;  // default: (0,0,0,0)
};

WM5_REGISTER_STREAM(Material);
typedef Pointer0<Material> MaterialPtr;

}

#endif
