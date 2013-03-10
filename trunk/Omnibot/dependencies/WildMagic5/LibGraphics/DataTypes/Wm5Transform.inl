// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

//----------------------------------------------------------------------------
inline bool Transform::IsIdentity () const
{
    return mIsIdentity;
}
//----------------------------------------------------------------------------
inline bool Transform::IsRSMatrix () const
{
    return mIsRSMatrix;
}
//----------------------------------------------------------------------------
inline bool Transform::IsUniformScale () const
{
    return mIsRSMatrix && mIsUniformScale;
}
//----------------------------------------------------------------------------
inline const HMatrix& Transform::GetRotate () const
{
    assertion(mIsRSMatrix, "Matrix is not a rotation\n");
    return mMatrix;
}
//----------------------------------------------------------------------------
inline const HMatrix& Transform::GetMatrix () const
{
    return mMatrix;
}
//----------------------------------------------------------------------------
inline const APoint& Transform::GetTranslate () const
{
    return mTranslate;
}
//----------------------------------------------------------------------------
inline const APoint& Transform::GetScale () const
{
    assertion(mIsRSMatrix, "Matrix is not a rotation-scale\n");
    return mScale;
}
//----------------------------------------------------------------------------
inline float Transform::GetUniformScale () const
{
    assertion(mIsRSMatrix, "Matrix is not a rotation-scale\n");
    assertion(mIsUniformScale, "Matrix is not uniform scale\n");
    return mScale[0];
}
//----------------------------------------------------------------------------
inline APoint Transform::operator* (const APoint& pnt) const
{
    return mHMatrix*pnt;
}
//----------------------------------------------------------------------------
inline AVector Transform::operator* (const AVector& vec) const
{
    return mHMatrix*vec;
}
//----------------------------------------------------------------------------
inline const HMatrix& Transform::Matrix () const
{
    return mHMatrix;
}
//----------------------------------------------------------------------------
inline int Transform::GetStreamingSize ()
{
    return (int)(3*sizeof(HMatrix) + 2*sizeof(APoint) +
        4*sizeof(unsigned int));
}
//----------------------------------------------------------------------------
