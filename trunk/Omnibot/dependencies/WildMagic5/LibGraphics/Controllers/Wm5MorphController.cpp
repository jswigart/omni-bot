// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5MorphController.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Controller, MorphController);
WM5_IMPLEMENT_STREAM(MorphController);
WM5_IMPLEMENT_FACTORY(MorphController);
WM5_IMPLEMENT_DEFAULT_NAMES(Controller, MorphController);

//----------------------------------------------------------------------------
MorphController::MorphController (int numVertices, int numTargets,
    int numKeys)
    :
    mNumVertices(numVertices),
    mNumTargets(numTargets),
    mNumKeys(numKeys),
    mLastIndex(0)
{
    mVertices = new2<APoint>(mNumVertices, mNumTargets);
    mTimes = new1<float>(mNumKeys);
    mWeights = new2<float>(mNumTargets - 1, mNumKeys);
}
//----------------------------------------------------------------------------
MorphController::~MorphController ()
{
    delete2(mVertices);
    delete1(mTimes);
    delete2(mWeights);
}
//----------------------------------------------------------------------------
void MorphController::GetKeyInfo (float ctrlTime, float& normTime, int& i0,
    int& i1)
{
    if (ctrlTime <= mTimes[0])
    {
        normTime = 0.0f;
        mLastIndex = 0;
        i0 = 0;
        i1 = 0;
        return;
    }

    if (ctrlTime >= mTimes[mNumKeys - 1])
    {
        normTime = 0.0f;
        mLastIndex = mNumKeys - 1;
        i0 = mLastIndex;
        i1 = mLastIndex;
        return;
    }

    int nextIndex;
    if (ctrlTime > mTimes[mLastIndex])
    {
        nextIndex = mLastIndex + 1;
        while (ctrlTime >= mTimes[nextIndex])
        {
            mLastIndex = nextIndex;
            ++nextIndex;
        }

        i0 = mLastIndex;
        i1 = nextIndex;
        normTime = (ctrlTime - mTimes[i0])/(mTimes[i1] - mTimes[i0]);
    }
    else if (ctrlTime < mTimes[mLastIndex])
    {
        nextIndex = mLastIndex - 1;
        while (ctrlTime <= mTimes[nextIndex])
        {
            mLastIndex = nextIndex;
            --nextIndex;
        }

        i0 = nextIndex;
        i1 = mLastIndex;
        normTime = (ctrlTime - mTimes[i0])/(mTimes[i1] - mTimes[i0]);
    }
    else
    {
        normTime = 0.0f;
        i0 = mLastIndex;
        i1 = mLastIndex;
    }
}
//----------------------------------------------------------------------------
bool MorphController::Update (double applicationTime)
{
    // The key interpolation uses linear interpolation.  To get higher-order
    // interpolation, you need to provide a more sophisticated key (Bezier
    // cubic or TCB spline, for example).

    if (!Controller::Update(applicationTime))
    {
        return false;
    }

    // Get access to the vertex buffer to store the blended targets.
    Visual* visual = StaticCast<Visual>(mObject);
    assertion(visual->GetVertexBuffer()->GetNumElements() == mNumVertices,
        "Mismatch in number of vertices.\n");

    VertexBufferAccessor vba(visual);

    // Set vertices to target[0].
    APoint* baseTarget = mVertices[0];
    int i;
    for (i = 0; i < mNumVertices; ++i)
    {
        vba.Position<Float3>(i) = baseTarget[i];
    }

    // Look up the bounding keys.
    float ctrlTime = (float)GetControlTime(applicationTime);
    float normTime;
    int i0, i1;
    GetKeyInfo(ctrlTime, normTime, i0, i1);

    // Add the remaining components in the convex composition.
    float* weights0 = mWeights[i0];
    float* weights1 = mWeights[i1];
    for (i = 1; i < mNumTargets; ++i)
    {
        // Add in the delta-vertices of target[i].
        float coeff = (1.0f-normTime)*weights0[i-1] + normTime*weights1[i-1];
        AVector* target = (AVector*)mVertices[i];
        for (int j = 0; j < mNumVertices; ++j)
        {
            APoint position = vba.Position<Float3>(j);
            position += coeff*target[j];
            vba.Position<Float3>(j) = position;
        }
    }

    visual->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(visual->GetVertexBuffer());
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
MorphController::MorphController (LoadConstructor value)
    :
    Controller(value),
    mNumVertices(0),
    mNumTargets(0),
    mVertices(0),
    mNumKeys(0),
    mTimes(0),
    mWeights(0),
    mLastIndex(0)
{
}
//----------------------------------------------------------------------------
void MorphController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Controller::Load(source);

    source.Read(mNumVertices);
    source.Read(mNumTargets);
    source.Read(mNumKeys);

    int numTotalVertices = mNumVertices*mNumTargets;
    mVertices = new2<APoint>(mNumVertices, mNumTargets);
    source.ReadAggregateVV(numTotalVertices, mVertices[0]);

    source.ReadVR(mNumKeys, mTimes);

    int numTotalWeights = mNumKeys*(mNumTargets - 1);
    mWeights = new2<float>(mNumTargets - 1, mNumKeys);
    source.ReadVV(numTotalWeights, mWeights[0]);

    WM5_END_DEBUG_STREAM_LOAD(MorphController, source);
}
//----------------------------------------------------------------------------
void MorphController::Link (InStream& source)
{
    Controller::Link(source);
}
//----------------------------------------------------------------------------
void MorphController::PostLink ()
{
    Controller::PostLink();
}
//----------------------------------------------------------------------------
bool MorphController::Register (OutStream& target) const
{
    return Controller::Register(target);
}
//----------------------------------------------------------------------------
void MorphController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Controller::Save(target);

    target.Write(mNumVertices);
    target.Write(mNumTargets);
    target.Write(mNumKeys);

    int numTotalVertices = mNumVertices*mNumTargets;
    target.WriteAggregateN(numTotalVertices, mVertices[0]);

    target.WriteN(mNumKeys, mTimes);

    int numTotalWeights = mNumKeys*(mNumTargets - 1);
    target.WriteN(numTotalWeights, mWeights[0]);

    WM5_END_DEBUG_STREAM_SAVE(MorphController, target);
}
//----------------------------------------------------------------------------
int MorphController::GetStreamingSize () const
{
    int size = Controller::GetStreamingSize();
    size += sizeof(mNumVertices);
    size += sizeof(mNumTargets);
    size += sizeof(mNumKeys);

    int numTotalVertices = mNumVertices*mNumTargets;
    size += numTotalVertices*sizeof(mVertices[0][0]);

    size += mNumKeys*sizeof(mTimes[0]);

    int numTotalWeights = mNumKeys*(mNumTargets - 1);
    size += numTotalWeights*sizeof(mWeights[0][0]);

    return size;
}
//----------------------------------------------------------------------------
