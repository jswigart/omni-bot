// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int Terrain::GetRowQuantity () const
{
    return mNumRows;
}
//----------------------------------------------------------------------------
inline int Terrain::GetColQuantity () const
{
    return mNumCols;
}
//----------------------------------------------------------------------------
inline int Terrain::GetSize () const
{
    return mSize;
}
//----------------------------------------------------------------------------
inline float Terrain::GetMinElevation () const
{
    return mMinElevation;
}
//----------------------------------------------------------------------------
inline float Terrain::GetMaxElevation () const
{
    return mMaxElevation;
}
//----------------------------------------------------------------------------
inline float Terrain::GetSpacing () const
{
    return mSpacing;
}
//----------------------------------------------------------------------------
