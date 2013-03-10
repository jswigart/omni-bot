// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int ShaderFloat::GetNumRegisters () const
{
    return mNumElements/4;
}
//----------------------------------------------------------------------------
inline const float* ShaderFloat::GetData () const
{
    return mData;
}
//----------------------------------------------------------------------------
inline float* ShaderFloat::GetData ()
{
    return mData;
}
//----------------------------------------------------------------------------
inline float ShaderFloat::operator[] (int i) const
{
    assertion(0 <= i && i < mNumElements, "Invalid index\n");
    return mData[i];
}
//----------------------------------------------------------------------------
inline float& ShaderFloat::operator[] (int i)
{
    assertion(0 <= i && i < mNumElements, "Invalid index\n");
    return mData[i];
}
//----------------------------------------------------------------------------
inline void ShaderFloat::EnableUpdater ()
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
inline void ShaderFloat::DisableUpdater ()
{
    mAllowUpdater = false;
}
//----------------------------------------------------------------------------
inline bool ShaderFloat::AllowUpdater () const
{
    return mAllowUpdater;
}
//----------------------------------------------------------------------------
