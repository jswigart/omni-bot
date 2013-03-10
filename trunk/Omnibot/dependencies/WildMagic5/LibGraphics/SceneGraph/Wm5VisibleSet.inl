// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int VisibleSet::GetNumVisible () const
{
    return mNumVisible;
}
//----------------------------------------------------------------------------
inline const std::vector<Spatial*>& VisibleSet::GetAllVisible () const
{
    return mVisible;
}
//----------------------------------------------------------------------------
inline Spatial* VisibleSet::GetVisible (int i) const
{
    assertion(0 <= i && i < mNumVisible, "Invalid index to GetVisible\n");
    return mVisible[i];
}
//----------------------------------------------------------------------------
inline void VisibleSet::Clear ()
{
    mNumVisible = 0;
}
//----------------------------------------------------------------------------
