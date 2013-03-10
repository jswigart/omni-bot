// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int Buffer::GetNumElements () const
{
    return mNumElements;
}
//----------------------------------------------------------------------------
inline int Buffer::GetElementSize () const
{
    return mElementSize;
}
//----------------------------------------------------------------------------
inline Buffer::Usage Buffer::GetUsage () const
{
    return mUsage;
}
//----------------------------------------------------------------------------
inline void Buffer::SetNumElements (int numElements)
{
    mNumElements = numElements;
}
//----------------------------------------------------------------------------
inline int Buffer::GetNumBytes () const
{
    return mNumBytes;
}
//----------------------------------------------------------------------------
inline const char* Buffer::GetData () const
{
    return mData;
}
//----------------------------------------------------------------------------
