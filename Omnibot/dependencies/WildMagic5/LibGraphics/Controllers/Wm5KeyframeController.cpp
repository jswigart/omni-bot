// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

#include "Wm5GraphicsPCH.h"
#include "Wm5KeyframeController.h"
#include "Wm5Spatial.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, TransformController, KeyframeController);
WM5_IMPLEMENT_STREAM(KeyframeController);
WM5_IMPLEMENT_FACTORY(KeyframeController);
WM5_IMPLEMENT_DEFAULT_NAMES(TransformController, KeyframeController);

//----------------------------------------------------------------------------
KeyframeController::KeyframeController (int numCommonTimes,
    int numTranslations, int numRotations, int numScales,
    const Transform& localTransform)
    :
    TransformController(localTransform),
    mTLastIndex(0),
    mRLastIndex(0),
    mSLastIndex(0),
    mCLastIndex(0)
{
    if (numCommonTimes > 0)
    {
        mNumCommonTimes = numCommonTimes;
        mCommonTimes = new1<float>(mNumCommonTimes);

        if (numTranslations > 0)
        {
            mNumTranslations = numTranslations;
            mTranslationTimes = mCommonTimes;
            mTranslations = new1<APoint>(mNumTranslations);
        }
        else
        {
            mNumTranslations = 0;
            mTranslationTimes = 0;
            mTranslations = 0;
        }

        if (numRotations > 0)
        {
            mNumRotations = numRotations;
            mRotationTimes = mCommonTimes;
            mRotations = new1<HQuaternion>(mNumRotations);
        }
        else
        {
            mNumRotations = 0;
            mRotationTimes = 0;
            mRotations = 0;
        }

        if (numScales > 0)
        {
            mNumScales = numScales;
            mScaleTimes = mCommonTimes;
            mScales = new1<float>(mNumScales);
        }
        else
        {
            mNumScales = 0;
            mScaleTimes = 0;
            mScales = 0;
        }
    }
    else
    {
        mNumCommonTimes = 0;
        mCommonTimes = 0;

        if (numTranslations > 0)
        {
            mNumTranslations = numTranslations;
            mTranslationTimes = new1<float>(mNumTranslations);
            mTranslations = new1<APoint>(mNumTranslations);
        }
        else
        {
            mNumTranslations = 0;
            mTranslationTimes = 0;
            mTranslations = 0;
        }

        if (numRotations > 0)
        {
            mNumRotations = numRotations;
            mRotationTimes = new1<float>(mNumRotations);
            mRotations = new1<HQuaternion>(mNumRotations);
        }
        else
        {
            mNumRotations = 0;
            mRotationTimes = 0;
            mRotations = 0;
        }

        if (numScales > 0)
        {
            mNumScales = numScales;
            mScaleTimes = new1<float>(mNumScales);
            mScales = new1<float>(mNumScales);
        }
        else
        {
            mNumScales = 0;
            mScaleTimes = 0;
            mScales = 0;
        }
    }
}
//----------------------------------------------------------------------------
KeyframeController::~KeyframeController ()
{
    if (mNumCommonTimes > 0)
    {
        delete1(mCommonTimes);
    }
    else
    {
        delete1(mTranslationTimes);
        delete1(mRotationTimes);
        delete1(mScaleTimes);
    }

    delete1(mTranslations);
    delete1(mRotations);
    delete1(mScales);
}
//----------------------------------------------------------------------------
void KeyframeController::GetKeyInfo (float ctrlTime, int numTimes,
    float* times, int& lastIndex, float& normTime, int& i0, int& i1)
{
    if (ctrlTime <= times[0])
    {
        normTime = 0.0f;
        lastIndex = 0;
        i0 = 0;
        i1 = 0;
        return;
    }

    if (ctrlTime >= times[numTimes-1])
    {
        normTime = 0.0f;
        lastIndex = numTimes - 1;
        i0 = lastIndex;
        i1 = lastIndex;
        return;
    }

    int nextIndex;
    if (ctrlTime > times[lastIndex])
    {
        nextIndex = lastIndex + 1;
        while (ctrlTime >= times[nextIndex])
        {
            lastIndex = nextIndex;
            ++nextIndex;
        }

        i0 = lastIndex;
        i1 = nextIndex;
        normTime = (ctrlTime - times[i0])/(times[i1] - times[i0]);
    }
    else if (ctrlTime < times[lastIndex])
    {
        nextIndex = lastIndex - 1;
        while (ctrlTime <= times[nextIndex])
        {
            lastIndex = nextIndex;
            --nextIndex;
        }

        i0 = nextIndex;
        i1 = lastIndex;
        normTime = (ctrlTime - times[i0])/(times[i1] - times[i0]);
    }
    else
    {
        normTime = 0.0f;
        i0 = lastIndex;
        i1 = lastIndex;
    }
}
//----------------------------------------------------------------------------
APoint KeyframeController::GetTranslate (float normTime, int i0, int i1)
{
    APoint trn = mTranslations[i0] + normTime*(mTranslations[i1] -
        mTranslations[i0]);
    return trn;
}
//----------------------------------------------------------------------------
HMatrix KeyframeController::GetRotate (float normTime, int i0, int i1)
{
    HQuaternion q;
    q.Slerp(normTime, mRotations[i0], mRotations[i1]);

    HMatrix rot;
    q.ToRotationMatrix(rot);
    return rot;
}
//----------------------------------------------------------------------------
float KeyframeController::GetScale (float normTime, int i0, int i1)
{
    return mScales[i0] + normTime*(mScales[i1] - mScales[i0]);
}
//----------------------------------------------------------------------------
bool KeyframeController::Update (double applicationTime)
{
    if (!Controller::Update(applicationTime))
    {
        return false;
    }

    float ctrlTime = (float)GetControlTime(applicationTime);
    float normTime = 0.0f;
    int i0 = 0, i1 = 0;
    APoint trn;
    HMatrix rot;
    float scale;

    // The logic here checks for equal-time arrays to minimize the number of
    // times GetKeyInfo is called.
    if (mNumCommonTimes > 0)
    {
        GetKeyInfo(ctrlTime, mNumCommonTimes, mCommonTimes, mCLastIndex,
            normTime, i0, i1);

        if (mNumTranslations > 0)
        {
            trn = GetTranslate(normTime, i0, i1);
            mLocalTransform.SetTranslate(trn);
        }

        if (mNumRotations > 0)
        {
            rot = GetRotate(normTime, i0, i1);
            mLocalTransform.SetRotate(rot);
        }

        if (mNumScales > 0)
        {
            scale = GetScale(normTime, i0, i1);
            mLocalTransform.SetUniformScale(scale);
        }
    }
    else
    {
        if (mNumTranslations > 0)
        {
            GetKeyInfo(ctrlTime, mNumTranslations, mTranslationTimes,
                mTLastIndex, normTime, i0, i1);
            trn = GetTranslate(normTime, i0, i1);
            mLocalTransform.SetTranslate(trn);
        }

        if (mNumRotations > 0)
        {
            GetKeyInfo(ctrlTime, mNumRotations, mRotationTimes, mRLastIndex,
                normTime, i0, i1);
            rot = GetRotate(normTime, i0, i1);
            mLocalTransform.SetRotate(rot);
        }

        if (mNumScales > 0)
        {
            GetKeyInfo(ctrlTime, mNumScales, mScaleTimes, mSLastIndex,
                normTime, i0, i1);
            scale = GetScale(normTime, i0, i1);
            mLocalTransform.SetUniformScale(scale);
        }
    }

    Spatial* spatial = StaticCast<Spatial>(mObject);
    spatial->LocalTransform = mLocalTransform;
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
KeyframeController::KeyframeController (LoadConstructor value)
    :
    TransformController(value),
    mNumCommonTimes(0),
    mCommonTimes(0),
    mNumTranslations(0),
    mTranslationTimes(0),
    mTranslations(0),
    mNumRotations(0),
    mRotationTimes(0),
    mRotations(0),
    mNumScales(0),
    mScaleTimes(0),
    mScales(0),
    mTLastIndex(0),
    mRLastIndex(0),
    mSLastIndex(0),
    mCLastIndex(0)
{
}
//----------------------------------------------------------------------------
void KeyframeController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    TransformController::Load(source);

    source.Read(mNumCommonTimes);
    if (mNumCommonTimes > 0)
    {
        source.ReadVR(mNumCommonTimes, mCommonTimes);
        source.ReadAggregateRR(mNumTranslations, mTranslations);
        source.ReadAggregateRR(mNumRotations, mRotations);
        source.ReadRR(mNumScales, mScales);
    }
    else
    {
        source.ReadRR(mNumTranslations, mTranslationTimes);
        source.ReadAggregateVR(mNumTranslations, mTranslations);
        source.ReadRR(mNumRotations, mRotationTimes);
        source.ReadAggregateVR(mNumRotations, mRotations);
        source.ReadRR(mNumScales, mScaleTimes);
        source.ReadVR(mNumScales, mScales);
    }

    WM5_END_DEBUG_STREAM_LOAD(KeyframeController, source);
}
//----------------------------------------------------------------------------
void KeyframeController::Link (InStream& source)
{
    TransformController::Link(source);
}
//----------------------------------------------------------------------------
void KeyframeController::PostLink ()
{
    TransformController::PostLink();
}
//----------------------------------------------------------------------------
bool KeyframeController::Register (OutStream& target) const
{
    return TransformController::Register(target);
}
//----------------------------------------------------------------------------
void KeyframeController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    TransformController::Save(target);

    target.Write(mNumCommonTimes);
    if (mNumCommonTimes > 0)
    {
        target.WriteN(mNumCommonTimes, mCommonTimes);
        target.WriteAggregateW(mNumTranslations, mTranslations);
        target.WriteAggregateW(mNumRotations, mRotations);
        target.WriteW(mNumScales, mScales);
    }
    else
    {
        target.WriteW(mNumTranslations, mTranslationTimes);
        target.WriteAggregateN(mNumTranslations, mTranslations);
        target.WriteW(mNumRotations, mRotationTimes);
        target.WriteAggregateN(mNumRotations, mRotations);
        target.WriteW(mNumScales, mScaleTimes);
        target.WriteN(mNumScales, mScales);
    }

    WM5_END_DEBUG_STREAM_SAVE(KeyframeController, target);
}
//----------------------------------------------------------------------------
int KeyframeController::GetStreamingSize () const
{
    int size = TransformController::GetStreamingSize();

    size += sizeof(mNumCommonTimes);

    if (mNumCommonTimes > 0)
    {
        size += mNumCommonTimes*sizeof(mCommonTimes[0]);
        size += sizeof(mNumTranslations);
        size += mNumTranslations*sizeof(mTranslations[0]);
        size += sizeof(mNumRotations);
        size += mNumRotations*sizeof(mRotations[0]);
        size += sizeof(mNumScales);
        size += mNumScales*sizeof(mScales[0]);
    }
    else
    {
        size += sizeof(mNumTranslations);
        size += mNumTranslations*sizeof(mTranslationTimes[0]);
        size += mNumTranslations*sizeof(mTranslations[0]);
        size += sizeof(mNumRotations);
        size += mNumRotations*sizeof(mRotationTimes[0]);
        size += mNumRotations*sizeof(mRotations[0]);
        size += sizeof(mNumScales);
        size += mNumScales*sizeof(mScaleTimes[0]);
        size += mNumScales*sizeof(mScales[0]);
    }

    return size;
}
//----------------------------------------------------------------------------
