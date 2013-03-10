// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IMAGEPROCESSING3_H
#define WM5IMAGEPROCESSING3_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ImageProcessing.h"

// This class supports processing of a 3D image that is stored as a tiled
// 2D image.  Let the image have b0 voxels in the x-direction, b1 voxels
// in the y-direction, and b2 voxels in the z-direction.  The value b2 must
// be factorable into b2 = f0*f1.  The 2D representation of the image is a
// f0-by-f1 array of tiles, each tile a z-slice of dimensions b0-by-b1.
//
// A 3D image is typically stored in lexicographical order in linear memory.
// If (x,y,z) is a voxel location in [0,b0)x[0,b1)x[0,b2), then the linear
// index is i = x + b0*(y + b1*z).  The 2D-tiled representation has a
// different mapping.  Let (u,v) be a pixel location in the 2D representation,
// in which case 0 <= u < b0*f0 and 0 <= v < b1*f1.  The corresponding
// (x,y,z) voxel location is
//    x = u mod b0
//    y = v mod b1
//    z = (u div b0) + (v div b1)*f0
// Now let (x,y,z) be a voxel location.  The corresponding (u,v) pixel
// location is
//    u = x + (z mod f0)*b0
//    v = y + (z div f0)*b1

namespace Wm5
{

class WM5_GRAPHICS_ITEM ImageProcessing3 : public ImageProcessing
{
public:
    // Construction and destruction.  The constructors are for a rectangular
    // cube with bound2 = factor0*factor1.  Objects of this class should not
    // be instantiated until the shader profile is known.  Thus, do not create
    // such an object in an application constructor, but do so in OnInitialize
    // or later.

    // Use this constructor for the standard image processing pipeline.
    ImageProcessing3 (int bound0, int bound1, int factor0, int factor1,
        Float4* imageData, PixelShader* mainPShader,
        const Float4& boundaryColor, bool useDirichlet);

    // Use this constructor when you want to set up your own pipeline for
    // processing the image.
    ImageProcessing3 (int bound0, int bound1, int factor0, int factor1,
        int numTargets);

    virtual ~ImageProcessing3 ();

    // Member access.
    inline int GetBound0 () const;
    inline int GetBound1 () const;
    inline int GetBound2 () const;
    inline int GetFactor0 () const;
    inline int GetFactor1 () const;
    inline float GetDx () const;
    inline float GetDy () const;
    inline float GetDz () const;

    // The mapping from 2D pixel to 3D voxel.
    void Map2Dto3D (int u, int v, int& x, int& y, int& z) const;

    // The mapping from 3D voxel to 2D pixel.
    void Map3Dto2D (int x, int y, int z, int& u, int& v) const;

    // The mapping from 3D voxel to 1D memory.
    int Map3Dto1D (int x, int y, int z);

    // Create a tiled image corresponding to the 3D image data.
    Texture2D* CreateTiledImage (Float4* imageData);

    // Set boundary pixels to (0,0,0,0).
    void CreateBoundaryDirichletEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    // Set boundary pixels so that boundary derivatives are zero.
    void CreateBoundaryNeumannEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    // Draw the tiled image using the boundary color for boundary voxels.
    void CreateDrawEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance, const Float4& boundaryColor);

private:
    int mBound0, mBound1, mBound2, mBound0M1, mBound1M1, mBound2M1;
    int mFactor0, mFactor1;
    float mDx, mDy, mDz;

    // Profile information for BoundaryDirichlet.fx.
    static int msAllDirichletPTextureUnits[2];
    static int* msDirichletPTextureUnits[Shader::MAX_PROFILES];
    static std::string msDirichletPPrograms[Shader::MAX_PROFILES];

    // Profile information for BoundaryNeumann.fx.
    static int msAllNeumannPTextureUnits[2];
    static int* msNeumannPTextureUnits[Shader::MAX_PROFILES];
    static std::string msNeumannPPrograms[Shader::MAX_PROFILES];

    // Profile information for ScreenShader.fx, function v_ScreenShader and
    // p_ScreenShader3.
    static int msAllDrawPRegisters[1];
    static int* msDrawPRegisters[Shader::MAX_PROFILES];
    static int msAllDrawPTextureUnits[2];
    static int* msDrawPTextureUnits[Shader::MAX_PROFILES];
    static std::string msDrawPPrograms[Shader::MAX_PROFILES];
};

#include "Wm5ImageProcessing3.inl"

}

#endif
