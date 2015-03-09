// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CAMERAMODELPOSITIONCONSTANT_H
#define WM5CAMERAMODELPOSITIONCONSTANT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ShaderFloat.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM CameraModelPositionConstant : public ShaderFloat
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(CameraModelPositionConstant);

public:
    // Construction and destruction.
    CameraModelPositionConstant ();
    virtual ~CameraModelPositionConstant ();

    virtual void Update (const Visual* visual, const Camera* camera);
};

WM5_REGISTER_STREAM(CameraModelPositionConstant);
typedef Pointer0<CameraModelPositionConstant> CameraModelPositionConstantPtr;

}

#endif
