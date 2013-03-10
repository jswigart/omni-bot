// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

//----------------------------------------------------------------------------
inline void TransformController::SetTransform (
    const Transform& localTransform)
{
    mLocalTransform = localTransform;
}
//----------------------------------------------------------------------------
inline const Transform& TransformController::GetTransform () const
{
    return mLocalTransform;
}
//----------------------------------------------------------------------------
