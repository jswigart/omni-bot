// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline void SwitchNode::SetActiveChild (int activeChild)
{
    assertion(
        activeChild == SN_INVALID_CHILD || activeChild < (int)mChild.size(),
        "Invalid active child specified\n");

    mActiveChild = activeChild;
}
//----------------------------------------------------------------------------
inline int SwitchNode::GetActiveChild () const
{
    return mActiveChild;
}
//----------------------------------------------------------------------------
inline void SwitchNode::DisableAllChildren ()
{
    mActiveChild = SN_INVALID_CHILD;
}
//----------------------------------------------------------------------------
