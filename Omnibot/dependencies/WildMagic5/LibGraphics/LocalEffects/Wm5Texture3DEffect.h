// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TEXTURE3DEFFECT_H
#define WM5TEXTURE3DEFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VisualEffectInstance.h"
#include "Wm5Texture3D.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Texture3DEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Texture3DEffect);

public:
    // Construction and destruction.
    Texture3DEffect (
        Shader::SamplerFilter filter = Shader::SF_NEAREST,
        Shader::SamplerCoordinate coordinate0 = Shader::SC_CLAMP_EDGE,
        Shader::SamplerCoordinate coordinate1 = Shader::SC_CLAMP_EDGE,
        Shader::SamplerCoordinate coordinate2 = Shader::SC_CLAMP_EDGE);

    virtual ~Texture3DEffect ();

    // Any change in sampler state is made via the pixel shader.
    PixelShader* GetPixelShader () const;

    // Create an instance of the effect with unique parameters.  If the
    // sampler filter mode is set to a value corresponding to mipmapping,
    // then the mipmaps will be generated if necessary.
    VisualEffectInstance* CreateInstance (Texture3D* texture) const;

    // Convenience for creating an instance.  The application does not have to
    // create the effect explicitly in order to create an instance from it.
    static VisualEffectInstance* CreateUniqueInstance (Texture3D* texture,
        Shader::SamplerFilter filter, Shader::SamplerCoordinate coordinate0,
        Shader::SamplerCoordinate coordinate1,
        Shader::SamplerCoordinate coordinate2);

private:
    static int msDx9VRegisters[1];
    static int msOglVRegisters[1];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
    static int msAllPTextureUnits[1];
    static int* msPTextureUnits[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_STREAM(Texture3DEffect);
typedef Pointer0<Texture3DEffect> Texture3DEffectPtr;

}

#endif
