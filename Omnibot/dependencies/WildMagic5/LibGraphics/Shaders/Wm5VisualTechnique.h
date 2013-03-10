// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VISUALTECHNIQUE_H
#define WM5VISUALTECHNIQUE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VisualPass.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VisualTechnique : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VisualTechnique);

public:
    // Construction and destruction.
    VisualTechnique ();
    virtual ~VisualTechnique ();

    // Support for deferred creation.  The function appends the new pass to
    // the end of the array.
    void InsertPass (VisualPass* pass);

    // Member access.
    inline int GetNumPasses () const;
    VisualPass* GetPass (int passIndex) const;

    // Access to components of the pass.
    VertexShader* GetVertexShader (int passIndex) const;
    PixelShader* GetPixelShader (int passIndex) const;
    AlphaState* GetAlphaState (int passIndex) const;
    CullState* GetCullState (int passIndex) const;
    DepthState* GetDepthState (int passIndex) const;
    OffsetState* GetOffsetState (int passIndex) const;
    StencilState* GetStencilState (int passIndex) const;
    WireState* GetWireState (int passIndex) const;

protected:
    std::vector<VisualPassPtr> mPasses;
};

WM5_REGISTER_STREAM(VisualTechnique);
typedef Pointer0<VisualTechnique> VisualTechniquePtr;
#include "Wm5VisualTechnique.inl"

}

#endif
