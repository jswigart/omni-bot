// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DEFAULTEEFFECT_H
#define WM5DEFAULTEEFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM DefaultEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(DefaultEffect);

public:
    // Construction and destruction.
    DefaultEffect ();
    virtual ~DefaultEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance () const;

private:
    static int msDx9VRegisters[1];
    static int msOglVRegisters[1];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_STREAM(DefaultEffect);
typedef Pointer0<DefaultEffect> DefaultEffectPtr;

}

#endif
