// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VWMATRIXCONSTANT_H
#define WM5VWMATRIXCONSTANT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ShaderFloat.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VWMatrixConstant : public ShaderFloat
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VWMatrixConstant);

public:
    // Construction and destruction.
    VWMatrixConstant ();
    virtual ~VWMatrixConstant ();

    virtual void Update (const Visual* visual, const Camera* camera);
};

WM5_REGISTER_STREAM(VWMatrixConstant);
typedef Pointer0<VWMatrixConstant> VWMatrixConstantPtr;

}

#endif
