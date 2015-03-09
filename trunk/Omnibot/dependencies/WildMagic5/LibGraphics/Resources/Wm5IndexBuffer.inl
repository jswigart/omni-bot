// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline char* IndexBuffer::GetData () const
{
    return mData;
}
//----------------------------------------------------------------------------
inline void IndexBuffer::SetOffset (int offset)
{
    if (offset >= 0)
    {
        mOffset = offset;
    }
    else
    {
        assertion(false, "The offset must be nonnegative\n");
    }
}
//----------------------------------------------------------------------------
inline int IndexBuffer::GetOffset () const
{
    return mOffset;
}
//----------------------------------------------------------------------------
