// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IMAGEPROCESSING2_H
#define WM5IMAGEPROCESSING2_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ImageProcessing.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ImageProcessing2 : public ImageProcessing
{
public:
    // Construction and destruction.  Objects of this class should not be
    // instantiated until the shader profile is known.  Thus, do not create
    // such an object in an application constructor, but do so in
    // OnInitialize or later.

    // Use this constructor for the standard image processing pipeline.
    ImageProcessing2 (int bound0, int bound1, Float4* imageData,
        PixelShader* mainPShader, bool useDirichlet);

    // Use this constructor when you want to set up your own pipeline for
    // processing the image.
    ImageProcessing2 (int bound0, int bound1, int numTargets);

    virtual ~ImageProcessing2 ();

    // Member access.
    inline int GetBound0 () const;
    inline int GetBound1 () const;
    inline float GetDx () const;
    inline float GetDy () const;

    // Create a texture corresponding to the 2D image data.
    Texture2D* CreateImage (Float4* imageData);

    // Set boundary pixels to (0,0,0,0).
    void CreateBoundaryDirichletEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    // Set boundary pixels so that boundary derivatives are zero.
    void CreateBoundaryNeumannEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    // Draw the image.
    void CreateDrawEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

private:
    void CreateVertexShader ();

    int mBound0, mBound1, mBound0M1, mBound1M1;
    float mDx, mDy;

    // Profile information for BoundaryDirichlet.fx.
    static int msAllDirichletPTextureUnits[2];
    static int* msDirichletPTextureUnits[Shader::MAX_PROFILES];
    static std::string msDirichletPPrograms[Shader::MAX_PROFILES];

    // Profile information for BoundaryNeumann.fx.
    static int msAllNeumannPTextureUnits[2];
    static int* msNeumannPTextureUnits[Shader::MAX_PROFILES];
    static std::string msNeumannPPrograms[Shader::MAX_PROFILES];

    // Profile information for ScreenShader.fx, function v_ScreenShader and
    // p_ScreenShader2.
    static int msAllDrawPTextureUnits[2];
    static int* msDrawPTextureUnits[Shader::MAX_PROFILES];
    static std::string msDrawPPrograms[Shader::MAX_PROFILES];
};

#include "Wm5ImageProcessing2.inl"

}

#endif
