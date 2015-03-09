// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TEXTURE2ADDEFFECT_H
#define WM5TEXTURE2ADDEFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VisualEffectInstance.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Texture2AddEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Texture2AddEffect);

public:
    // Construction and destruction.
    Texture2AddEffect ();
    virtual ~Texture2AddEffect ();

    // Any change in sampler state is made via the pixel shader.
    PixelShader* GetPixelShader () const;

    // Create an instance of the effect with unique parameters.  If a
    // sampler filter mode is set to a value corresponding to mipmapping,
    // then the mipmaps will be generated if necessary.
    VisualEffectInstance* CreateInstance (Texture2D* texture0,
        Texture2D* texture1) const;

    // Convenience for creating an instance.  The application does not have to
    // create the effect explicitly in order to create an instance from it.
    static VisualEffectInstance* CreateUniqueInstance (
        Texture2D* texture0,
        Shader::SamplerFilter filter0,
        Shader::SamplerCoordinate coordinate00,
        Shader::SamplerCoordinate coordinate01,
        Texture2D* texture1,
        Shader::SamplerFilter filter1,
        Shader::SamplerCoordinate coordinate10,
        Shader::SamplerCoordinate coordinate11);

private:
    static int msDx9VRegisters[1];
    static int msOglVRegisters[1];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
    static int msAllPTextureUnits[2];
    static int* msPTextureUnits[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_STREAM(Texture2AddEffect);
typedef Pointer0<Texture2AddEffect> Texture2AddEffectPtr;

}

#endif
