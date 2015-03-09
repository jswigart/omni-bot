// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VERTEXBUFFERACCESSOR_H
#define WM5VERTEXBUFFERACCESSOR_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VertexBuffer.h"
#include "Wm5VertexFormat.h"
#include "Wm5APoint.h"
#include "Wm5FileIO.h"

namespace Wm5
{

class Visual;

class WM5_GRAPHICS_ITEM VertexBufferAccessor
{
public:
    // Construction and destruction.
    VertexBufferAccessor ();
    VertexBufferAccessor (VertexFormat* vformat, VertexBuffer* vbuffer);
    VertexBufferAccessor (Visual* visual);
    ~VertexBufferAccessor ();

    // Use these functions after default construction of an accessor object
    // or when you want to create an accessor object once and use it to
    // process multiple vertex buffers.
    void ApplyTo (VertexFormat* vformat, VertexBuffer* vbuffer);
    void ApplyTo (Visual* visual);

    inline char* GetData () const;
    inline int GetNumVertices () const;
    inline int GetStride () const;

    // Generic accessors to the vertex buffer data.  You must know the type T
    // for the particular attributes.  The Get*Channels() functions are valid
    // only for FLOAT{1,2,3,4} data and return the number of floats for the
    // attributes.
    template <typename T>
    inline T& Position (int i);
    inline bool HasPosition () const;
    inline int GetPositionChannels () const;
    inline float* PositionTuple (int i);

    template <typename T>
    inline T& Normal (int i);
    inline bool HasNormal () const;
    inline int GetNormalChannels () const;
    inline float* NormalTuple (int i);
    void SetNormal3 (int i, const AVector& normal);
    AVector GetNormal3 (int i) const;

    template <typename T>
    inline T& Tangent (int i);
    inline bool HasTangent () const;
    inline int GetTangentChannels () const;
    inline float* TangentTuple (int i);

    template <typename T>
    inline T& Binormal (int i);
    inline bool HasBinormal () const;
    inline int GetBinormalChannels () const;
    inline float* BinormalTuple (int i);

    template <typename T>
    inline T& TCoord (int unit, int i);
    inline bool HasTCoord (int unit) const;
    inline int GetTCoordChannels (int unit) const;
    inline float* TCoordTuple (int unit, int i);

    template <typename T>
    inline T& Color (int unit, int i);
    inline bool HasColor (int unit) const;
    inline int GetColorChannels (int unit) const;
    inline float* ColorTuple (int unit, int i);

    template <typename T>
    inline T& BlendIndices (int i);
    inline bool HasBlendIndices () const;

    template <typename T>
    inline T& BlendWeight (int i);
    inline bool HasBlendWeight () const;

    // Support for file reads and writes (with endianness).
    void Read (FileIO& inFile);
    void Write (FileIO& outFile);

private:
    // Called by the constructors.
    void Initialize ();

    VertexFormat* mVFormat;
    VertexBuffer* mVBuffer;
    int mStride;
    char* mData;

    char* mPosition;
    char* mNormal;
    char* mTangent;
    char* mBinormal;
    char* mTCoord[VertexFormat::AM_MAX_TCOORD_UNITS];
    char* mColor[VertexFormat::AM_MAX_COLOR_UNITS];
    char* mBlendIndices;
    char* mBlendWeight;

    int mPositionChannels;
    int mNormalChannels;
    int mTangentChannels;
    int mBinormalChannels;
    int mTCoordChannels[VertexFormat::AM_MAX_TCOORD_UNITS];
    int mColorChannels[VertexFormat::AM_MAX_COLOR_UNITS];
};

#include "Wm5VertexBufferAccessor.inl"

}

#endif
