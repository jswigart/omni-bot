// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int ControlledObject::GetNumControllers () const
{
    return mNumControllers;
}
//----------------------------------------------------------------------------
inline Controller* ControlledObject::GetController (int i) const
{
    if (0 <= i && i < mNumControllers)
    {
        return mControllers[i];
    }

    assertion(false, "Invalid index in GetController.\n");
    return 0;
}
//----------------------------------------------------------------------------
