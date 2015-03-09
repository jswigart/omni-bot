// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MATERIALAMBIENTCONSTANT_H
#define WM5MATERIALAMBIENTCONSTANT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ShaderFloat.h"
#include "Wm5Material.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM MaterialAmbientConstant : public ShaderFloat
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(MaterialAmbientConstant);

public:
    // Construction and destruction.
    MaterialAmbientConstant (Material* material);
    virtual ~MaterialAmbientConstant ();

    // Member access.
    Material* GetMaterial ();

    virtual void Update (const Visual* visual, const Camera* camera);

protected:
    MaterialPtr mMaterial;
};

WM5_REGISTER_STREAM(MaterialAmbientConstant);
typedef Pointer0<MaterialAmbientConstant> MaterialAmbientConstantPtr;

}

#endif
