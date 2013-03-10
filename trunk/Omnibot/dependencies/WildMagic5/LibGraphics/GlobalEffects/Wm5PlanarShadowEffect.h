// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PLANARSHADOWEFFECT_H
#define WM5PLANARSHADOWEFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5GlobalEffect.h"
#include "Wm5HMatrix.h"
#include "Wm5Light.h"
#include "Wm5MaterialEffect.h"
#include "Wm5Node.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM PlanarShadowEffect : public GlobalEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(PlanarShadowEffect);

public:
    // Construction and destruction.  The number of planes supported depends
    // on the number of stencil bits (256 for an 8-bit stencil buffer).
    PlanarShadowEffect (int numPlanes, Node* shadowCaster);
    virtual ~PlanarShadowEffect ();

    virtual void Draw (Renderer* renderer, const VisibleSet& visibleSet);

    // Member access.
    inline int GetNumPlanes () const;
    inline void SetPlane (int i, TriMesh* plane);
    inline TriMesh* GetPlane (int i) const;
    inline void SetProjector (int i, Light* projector);
    inline Light* GetProjector (int i) const;
    inline void SetShadowColor (int i, const Float4& shadowColor);
    inline const Float4& GetShadowColor (int i) const;

protected:
    bool GetProjectionMatrix (int i, HMatrix& projection);

    int mNumPlanes;
    TriMeshPtr* mPlanes;
    LightPtr* mProjectors;
    Float4* mShadowColors;
    NodePtr mShadowCaster;

    // Global render state for drawing.
    AlphaStatePtr mAlphaState;
    DepthStatePtr mDepthState;
    StencilStatePtr mStencilState;

    // Override effect for drawing the shadows.
    MaterialPtr mMaterial;
    MaterialEffectPtr mMaterialEffect;
    VisualEffectInstancePtr mMaterialEffectInstance;
};

WM5_REGISTER_STREAM(PlanarShadowEffect);
typedef Pointer0<PlanarShadowEffect> PlanarShadowEffectPtr;
#include "Wm5PlanarShadowEffect.inl"

}

#endif
