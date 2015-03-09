// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int Float2Array::GetNumElements () const
{
    return mNumElements;
}
//----------------------------------------------------------------------------
inline Float2* Float2Array::GetData () const
{
    return mElements;
}
//----------------------------------------------------------------------------
inline Float2Array::operator const Float2* () const
{
    return mElements;
}
//----------------------------------------------------------------------------
inline Float2Array::operator Float2* ()
{
    return mElements;
}
//----------------------------------------------------------------------------
inline const Float2& Float2Array::operator[] (int i) const
{
    return mElements[i];
}
//----------------------------------------------------------------------------
inline Float2& Float2Array::operator[] (int i)
{
    return mElements[i];
}
//----------------------------------------------------------------------------
