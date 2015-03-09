// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline char* VertexBufferAccessor::GetData () const
{
    return mData;
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetNumVertices () const
{
    return mVBuffer->GetNumElements();
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetStride () const
{
    return mStride;
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::Position (int i)
{
    return *(T*)(mPosition + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasPosition () const
{
    return mPosition != 0;
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetPositionChannels () const
{
    return mPositionChannels;
}
//----------------------------------------------------------------------------
inline float* VertexBufferAccessor::PositionTuple (int i)
{
    return (float*)(mPosition + i*mStride);
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::Normal (int i)
{
    return *(T*)(mNormal + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasNormal () const
{
    return mNormal != 0;
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetNormalChannels () const
{
    return mNormalChannels;
}
//----------------------------------------------------------------------------
inline float* VertexBufferAccessor::NormalTuple (int i)
{
    return (float*)(mNormal + i*mStride);
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::Tangent (int i)
{
    return *(T*)(mTangent + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasTangent () const
{
    return mTangent != 0;
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetTangentChannels () const
{
    return mTangentChannels;
}
//----------------------------------------------------------------------------
inline float* VertexBufferAccessor::TangentTuple (int i)
{
    return (float*)(mTangent + i*mStride);
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::Binormal (int i)
{
    return *(T*)(mBinormal + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasBinormal () const
{
    return mBinormal != 0;
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetBinormalChannels () const
{
    return mBinormalChannels;
}
//----------------------------------------------------------------------------
inline float* VertexBufferAccessor::BinormalTuple (int i)
{
    return (float*)(mBinormal + i*mStride);
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::TCoord (int unit, int i)
{
    return *(T*)(mTCoord[unit] + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasTCoord (int unit) const
{
    return mTCoord[unit] != 0;
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetTCoordChannels (int unit) const
{
    return mTCoordChannels[unit];
}
//----------------------------------------------------------------------------
inline float* VertexBufferAccessor::TCoordTuple (int unit, int i)
{
    return (float*)(mTCoord[unit] + i*mStride);
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::Color (int unit, int i)
{
    return *(T*)(mColor[unit] + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasColor (int unit) const
{
    return mColor[unit] != 0;
}
//----------------------------------------------------------------------------
inline int VertexBufferAccessor::GetColorChannels (int unit) const
{
    return mColorChannels[unit];
}
//----------------------------------------------------------------------------
inline float* VertexBufferAccessor::ColorTuple (int unit, int i)
{
    return (float*)(mColor[unit] + i*mStride);
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::BlendIndices (int i)
{
    return *(T*)(mBlendIndices + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasBlendIndices () const
{
    return mBlendIndices != 0;
}
//----------------------------------------------------------------------------
template <typename T>
inline T& VertexBufferAccessor::BlendWeight (int i)
{
    return *(T*)(mBlendWeight + i*mStride);
}
//----------------------------------------------------------------------------
inline bool VertexBufferAccessor::HasBlendWeight () const
{
    return mBlendWeight != 0;
}
//----------------------------------------------------------------------------
