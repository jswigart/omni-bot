// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
inline HMatrix::operator const float* () const
{
    return mEntry;
}
//----------------------------------------------------------------------------
inline HMatrix::operator float* ()
{
    return mEntry;
}
//----------------------------------------------------------------------------
inline const float* HMatrix::operator[] (int row) const
{
    return &mEntry[4*row];
}
//----------------------------------------------------------------------------
inline float* HMatrix::operator[] (int row)
{
    return &mEntry[4*row];
}
//----------------------------------------------------------------------------
inline const float& HMatrix::operator() (int row, int column) const
{
    return mEntry[column + 4*row];
}
//----------------------------------------------------------------------------
inline float& HMatrix::operator() (int row, int column)
{
    return mEntry[column + 4*row];
}
//----------------------------------------------------------------------------
