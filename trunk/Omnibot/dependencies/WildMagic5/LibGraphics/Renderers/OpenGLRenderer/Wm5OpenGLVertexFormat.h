// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5OPENGLVERTEXFORMAT_H
#define WM5OPENGLVERTEXFORMAT_H

#include "Wm5OpenGLRendererLIB.h"
#include "Wm5VertexFormat.h"

namespace Wm5
{

class Renderer;

class PdrVertexFormat
{
public:
    // Construction and destruction.
    PdrVertexFormat (Renderer* renderer, const VertexFormat* vformat);
    ~PdrVertexFormat ();

    // Vertex format operations.
    void Enable (Renderer* renderer);
    void Disable (Renderer* renderer);

private:
    int mStride;

    GLuint mHasPosition;
    GLuint mPositionChannels;
    GLuint mPositionType;
    GLuint mPositionOffset;

    GLuint mHasNormal;
    GLuint mNormalChannels;
    GLuint mNormalType;
    GLuint mNormalOffset;

    GLuint mHasTangent;
    GLuint mTangentChannels;
    GLuint mTangentType;
    GLuint mTangentOffset;

    GLuint mHasBinormal;
    GLuint mBinormalChannels;
    GLuint mBinormalType;
    GLuint mBinormalOffset;

    GLuint mHasTCoord[VertexFormat::AM_MAX_TCOORD_UNITS];
    GLuint mTCoordChannels[VertexFormat::AM_MAX_TCOORD_UNITS];
    GLuint mTCoordType[VertexFormat::AM_MAX_TCOORD_UNITS];
    GLuint mTCoordOffset[VertexFormat::AM_MAX_TCOORD_UNITS];

    GLuint mHasColor[VertexFormat::AM_MAX_COLOR_UNITS];
    GLuint mColorChannels[VertexFormat::AM_MAX_COLOR_UNITS];
    GLuint mColorType[VertexFormat::AM_MAX_COLOR_UNITS];
    GLuint mColorOffset[VertexFormat::AM_MAX_COLOR_UNITS];

    GLuint mHasBlendIndices;
    GLuint mBlendIndicesChannels;
    GLuint mBlendIndicesType;
    GLuint mBlendIndicesOffset;

    GLuint mHasBlendWeight;
    GLuint mBlendWeightChannels;
    GLuint mBlendWeightType;
    GLuint mBlendWeightOffset;

    GLuint mHasFogCoord;
    GLuint mFogCoordChannels;
    GLuint mFogCoordType;
    GLuint mFogCoordOffset;

    GLuint mHasPSize;
    GLuint mPSizeChannels;
    GLuint mPSizeType;
    GLuint mPSizeOffset;
};

}

#endif
