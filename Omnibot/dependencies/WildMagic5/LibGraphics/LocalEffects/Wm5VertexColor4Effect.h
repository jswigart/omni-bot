// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VERTEXCOLOR4EFFECT_H
#define WM5VERTEXCOLOR4EFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VertexColor4Effect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VertexColor4Effect);

public:
    // Construction and destruction.
    VertexColor4Effect ();
    virtual ~VertexColor4Effect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance () const;

    // Convenience for creating an instance.  The application does not have to
    // create the effect explicitly in order to create an instance from it.
    static VisualEffectInstance* CreateUniqueInstance ();

private:
    static int msDx9VRegisters[1];
    static int msOglVRegisters[1];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_STREAM(VertexColor4Effect);
typedef Pointer0<VertexColor4Effect> VertexColor4EffectPtr;

}

#endif
