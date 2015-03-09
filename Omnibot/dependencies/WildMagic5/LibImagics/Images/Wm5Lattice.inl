// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int Lattice::GetDimensions () const
{
    return mNumDimensions;
}
//----------------------------------------------------------------------------
inline const int* Lattice::GetBounds () const
{
    return mBounds;
}
//----------------------------------------------------------------------------
inline int Lattice::GetBound (int i) const
{
    return mBounds[i];
}
//----------------------------------------------------------------------------
inline int Lattice::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
inline const int* Lattice::GetOffsets () const
{
    return mOffsets;
}
//----------------------------------------------------------------------------
inline int Lattice::GetOffset (int i) const
{
    return mOffsets[i];
}
//----------------------------------------------------------------------------
