// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PIXELSHADER_H
#define WM5PIXELSHADER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Shader.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM PixelShader : public Shader
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(PixelShader);

public:
    // Construction and destruction.  See comments in Wm5Shader.h about
    // constructing shader programs.
    PixelShader (const std::string& programName, int numInputs,
        int numOutputs, int numConstants, int numSamplers, bool profileOwner);

    virtual ~PixelShader ();

    // Pixel shader profile information.
    enum WM5_GRAPHICS_ITEM Profile
    {
        PP_NONE,
        PP_PS_1_1,
        PP_PS_2_0,
        PP_PS_3_0,
        PP_ARBFP1
    };

    static void SetProfile (Profile profile);
    static Profile GetProfile ();

protected:
    static Profile msProfile;
};

WM5_REGISTER_STREAM(PixelShader);
typedef Pointer0<PixelShader> PixelShaderPtr;

}

#endif
