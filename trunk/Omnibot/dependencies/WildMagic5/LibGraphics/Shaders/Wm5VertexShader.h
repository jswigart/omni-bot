// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VERTEXSHADER_H
#define WM5VERTEXSHADER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Shader.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VertexShader : public Shader
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VertexShader);

public:
    // Construction and destruction.  See comments in Wm5Shader.h about
    // constructing shader programs.
    VertexShader (const std::string& programName, int numInputs,
        int numOutputs, int numConstants, int numSamplers, bool profileOwner);

    virtual ~VertexShader ();

    // Vertex shader profile information.
    enum WM5_GRAPHICS_ITEM Profile
    {
        VP_NONE,
        VP_VS_1_1,
        VP_VS_2_0,
        VP_VS_3_0,
        VP_ARBVP1
    };

    static void SetProfile (Profile profile);
    static Profile GetProfile ();

protected:
    static Profile msProfile;
};

WM5_REGISTER_STREAM(VertexShader);
typedef Pointer0<VertexShader> VertexShaderPtr;

}

#endif
