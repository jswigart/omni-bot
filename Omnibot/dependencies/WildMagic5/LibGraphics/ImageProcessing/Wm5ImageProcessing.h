// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5IMAGEPROCESSING_H
#define WM5IMAGEPROCESSING_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Camera.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5Renderer.h"
#include "Wm5RenderTarget.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ImageProcessing
{
public:
    // Construction and destruction.
    ImageProcessing (int numCols, int numRows, int numTargets);
    virtual ~ImageProcessing ();

    // Member access.
    inline int GetNumCols () const;
    inline int GetNumRows () const;
    inline int GetNumTargets () const;
    inline float GetColSpacing () const;
    inline float GetRowSpacing () const;
    inline Camera* GetCamera () const;
    inline TriMesh* GetRectangle () const;
    inline RenderTarget* GetTarget (int i) const;

    // The lexicographical mapping into linear memory.
    inline int Index (int col, int row) const;

    // Allow the caller to create an effect that shares the vertex shader
    // managed by this class.
    void CreateEffect (PixelShader* pshader, VisualEffect*& effect,
        VisualEffectInstance*& instance);

    // These function calls bound the drawing of the processed image to the
    // backbuffer.
    void PreDraw ();
    void PostDraw ();

    // Support for the standard image processing pipeline.
    inline Texture2D* GetMainTexture () const;
    inline VisualEffectInstance* GetMainEffectInstance () const;
    void Initialize (Renderer* renderer, bool openglHack = false);
    void ExecuteStep (Renderer* renderer, bool draw);

protected:
    void CreateVertexShader ();

    int mNumCols, mNumRows, mNumColsM1, mNumRowsM1;
    int mNumTargets;
    float mColSpacing, mRowSpacing;
    CameraPtr mCamera;
    TriMeshPtr mRectangle;
    VertexShaderPtr mVertexShader;
    PVWMatrixConstantPtr mPVWMatrixConstant;
    RenderTargetPtr* mTargets;

    // Support for the standard image processing pipeline.
    Texture2DPtr mMainTexture;
    VisualEffectPtr mMainEffect;
    VisualEffectInstancePtr mMainEffectInstance;
    VisualEffectPtr mBoundaryEffect;
    VisualEffectInstancePtr mBoundaryEffectInstance;
    VisualEffectPtr mDrawEffect;
    VisualEffectInstancePtr mDrawEffectInstance;
    VisualEffectPtr mDrawInput;
    VisualEffectInstancePtr mDrawInputInstance;

    static int msDx9VRegisters[1];
    static int msOglVRegisters[1];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
};

#include "Wm5ImageProcessing.inl"

}

#endif
