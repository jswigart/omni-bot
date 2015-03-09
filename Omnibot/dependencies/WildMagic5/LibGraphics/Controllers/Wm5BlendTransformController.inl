// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

//----------------------------------------------------------------------------
inline TransformController* BlendTransformController::GetController0 () const
{
    return mController0;
}
//----------------------------------------------------------------------------
inline TransformController* BlendTransformController::GetController1 () const
{
    return mController1;
}
//----------------------------------------------------------------------------
inline bool BlendTransformController::GetRSMatrices () const
{
    return mRSMatrices;
}
//----------------------------------------------------------------------------
inline void BlendTransformController::SetWeight (float weight)
{
    mWeight = weight;
}
//----------------------------------------------------------------------------
inline float BlendTransformController::GetWeight () const
{
    return mWeight;
}
//----------------------------------------------------------------------------
