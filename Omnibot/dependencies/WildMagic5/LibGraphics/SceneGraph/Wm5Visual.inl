// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline Visual::PrimitiveType Visual::GetPrimitiveType () const
{
    return mType;
}
//----------------------------------------------------------------------------
inline void Visual::SetVertexFormat (VertexFormat* vformat)
{
    mVFormat = vformat;
}
//----------------------------------------------------------------------------
inline const VertexFormat* Visual::GetVertexFormat () const
{
    return mVFormat;
}
//----------------------------------------------------------------------------
inline VertexFormat* Visual::GetVertexFormat ()
{
    return mVFormat;
}
//----------------------------------------------------------------------------
inline void Visual::SetVertexBuffer (VertexBuffer* vbuffer)
{
    mVBuffer = vbuffer;
}
//----------------------------------------------------------------------------
inline const VertexBuffer* Visual::GetVertexBuffer () const
{
    return mVBuffer;
}
//----------------------------------------------------------------------------
inline VertexBuffer* Visual::GetVertexBuffer ()
{
    return mVBuffer;
}
//----------------------------------------------------------------------------
inline void Visual::SetIndexBuffer (IndexBuffer* ibuffer)
{
    mIBuffer = ibuffer;
}
//----------------------------------------------------------------------------
inline const IndexBuffer* Visual::GetIndexBuffer () const
{
    return mIBuffer;
}
//----------------------------------------------------------------------------
inline IndexBuffer* Visual::GetIndexBuffer ()
{
    return mIBuffer;
}
//----------------------------------------------------------------------------
inline const Bound& Visual::GetModelBound () const
{
    return mModelBound;
}
//----------------------------------------------------------------------------
inline Bound& Visual::GetModelBound ()
{
    return mModelBound;
}
//----------------------------------------------------------------------------
inline void Visual::SetEffectInstance (VisualEffectInstance* effect)
{
    mEffect = effect;
}
//----------------------------------------------------------------------------
inline VisualEffectInstance* Visual::GetEffectInstance () const
{
    return mEffect;
}
//----------------------------------------------------------------------------
