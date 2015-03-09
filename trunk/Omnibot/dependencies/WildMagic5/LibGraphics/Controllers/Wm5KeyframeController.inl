// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int KeyframeController::GetNumCommonTimes () const
{
    return mNumCommonTimes;
}
//----------------------------------------------------------------------------
inline float* KeyframeController::GetCommonTimes () const
{
    return mCommonTimes;
}
//----------------------------------------------------------------------------
inline int KeyframeController::GetNumTranslations () const
{
    return mNumTranslations;
}
//----------------------------------------------------------------------------
inline float* KeyframeController::GetTranslationTimes () const
{
    return mTranslationTimes;
}
//----------------------------------------------------------------------------
inline APoint* KeyframeController::GetTranslations () const
{
    return mTranslations;
}
//----------------------------------------------------------------------------
inline int KeyframeController::GetNumRotations () const
{
    return mNumRotations;
}
//----------------------------------------------------------------------------
inline float* KeyframeController::GetRotationTimes () const
{
    return mRotationTimes;
}
//----------------------------------------------------------------------------
inline HQuaternion* KeyframeController::GetRotations () const
{
    return mRotations;
}
//----------------------------------------------------------------------------
inline int KeyframeController::GetNumScales () const
{
    return mNumScales;
}
//----------------------------------------------------------------------------
inline float* KeyframeController::GetScaleTimes () const
{
    return mScaleTimes;
}
//----------------------------------------------------------------------------
inline float* KeyframeController::GetScales () const
{
    return mScales;
}
//----------------------------------------------------------------------------
