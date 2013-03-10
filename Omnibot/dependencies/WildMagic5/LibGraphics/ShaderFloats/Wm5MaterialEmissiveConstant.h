// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MATERIALEMISSIVECONSTANT_H
#define WM5MATERIALEMISSIVECONSTANT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ShaderFloat.h"
#include "Wm5Material.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM MaterialEmissiveConstant : public ShaderFloat
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(MaterialEmissiveConstant);

public:
    // Construction and destruction.
    MaterialEmissiveConstant (Material* material);
    virtual ~MaterialEmissiveConstant ();

    // Member access.
    Material* GetMaterial ();

    virtual void Update (const Visual* visual, const Camera* camera);

protected:
    MaterialPtr mMaterial;
};

WM5_REGISTER_STREAM(MaterialEmissiveConstant);
typedef Pointer0<MaterialEmissiveConstant> MaterialEmissiveConstantPtr;

}

#endif
