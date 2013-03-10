// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5KEYFRAMECONTROLLER_H
#define WM5KEYFRAMECONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5TransformController.h"
#include "Wm5HMatrix.h"
#include "Wm5HQuaternion.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM KeyframeController : public TransformController
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(KeyframeController);

public:
    // Construction and destruction.  If the translations, rotations, and
    // scales all share the same keyframe times, then numCommonTimes is
    // set to a positive number.  Each remaining number is numCommonTimes
    // when the channel exists or zero when it does not.  If the keyframe
    // times are not shared, then numCommonTimes must be set to zero and
    // the remaining numbers set to the appropriate values--positive when
    // the channel exists or zero otherwise.
    //
    // The Transform input initializes the controlled object's local
    // transform.  The previous behavior of this class was to fill in only
    // those transformation channels represented by the key frames, which
    // relied implicitly on the Spatial mObject to have its other channels
    // set appropriately by the application.  Now KeyframeController sets
    // *all* the channels.
    KeyframeController (int numCommonTimes, int numTranslations,
        int numRotations, int numScales, const Transform& localTransform);

    virtual ~KeyframeController ();

    // Member access.  After calling the constructor, you must set the data
    // using these functions.
    inline int GetNumCommonTimes () const;
    inline float* GetCommonTimes () const;

    inline int GetNumTranslations () const;
    inline float* GetTranslationTimes () const;
    inline APoint* GetTranslations () const;

    inline int GetNumRotations () const;
    inline float* GetRotationTimes () const;
    inline HQuaternion* GetRotations () const;

    inline int GetNumScales () const;
    inline float* GetScaleTimes () const;
    inline float* GetScales () const;

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

protected:
    // Support for looking up keyframes given the specified time.
    static void GetKeyInfo (float ctrlTime, int numTimes, float* times,
        int& lastIndex, float& normTime, int& i0, int& i1);

    APoint GetTranslate (float normTime, int i0, int i1);
    HMatrix GetRotate (float normTime, int i0, int i1);
    float GetScale (float normTime, int i0, int i1);

    // This array is used only when times are shared by translations,
    // rotations, and scales.
    int mNumCommonTimes;
    float* mCommonTimes;

    int mNumTranslations;
    float* mTranslationTimes;
    APoint* mTranslations;

    int mNumRotations;
    float* mRotationTimes;
    HQuaternion* mRotations;

    int mNumScales;
    float* mScaleTimes;
    float* mScales;

    // Cached indices for the last found pair of keys used for interpolation.
    // For a sequence of times, this guarantees an O(1) lookup.
    int mTLastIndex, mRLastIndex, mSLastIndex, mCLastIndex;
};

WM5_REGISTER_STREAM(KeyframeController);
typedef Pointer0<KeyframeController> KeyframeControllerPtr;
#include "Wm5KeyframeController.inl"

}

#endif
