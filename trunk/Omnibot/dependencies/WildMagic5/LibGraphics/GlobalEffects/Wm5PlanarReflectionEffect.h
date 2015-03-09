// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PLANARREFLECTIONEFFECT_H
#define WM5PLANARREFLECTIONEFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5GlobalEffect.h"
#include "Wm5HMatrix.h"
#include "Wm5HPlane.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM PlanarReflectionEffect : public GlobalEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(PlanarReflectionEffect);

public:
    // Construction and destruction.  The number of planes supported depends
    // on the number of stencil bits (256 for an 8-bit stencil buffer).
    PlanarReflectionEffect (int numPlanes);
    virtual ~PlanarReflectionEffect ();

    virtual void Draw (Renderer* renderer, const VisibleSet& visibleSet);

    // Member access.
    inline int GetNumPlanes () const;
    inline void SetPlane (int i, TriMesh* plane);
    inline TriMesh* GetPlane (int i) const;
    inline void SetReflectance (int i, float reflectance);
    inline float GetReflectance (int i) const;

protected:
    void GetReflectionMatrixAndModelPlane (int i, HMatrix& reflection,
        HPlane& modelPlane);

    int mNumPlanes;
    TriMeshPtr* mPlanes;
    float* mReflectances;

    // Global render state for drawing.
    AlphaStatePtr mAlphaState;
    DepthStatePtr mDepthState;
    StencilStatePtr mStencilState;
};

WM5_REGISTER_STREAM(PlanarReflectionEffect);
typedef Pointer0<PlanarReflectionEffect> PlanarReflectionEffectPtr;
#include "Wm5PlanarReflectionEffect.inl"

}

#endif
