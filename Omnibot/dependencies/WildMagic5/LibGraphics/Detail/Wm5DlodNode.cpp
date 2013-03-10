// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5DlodNode.h"
#include "Wm5Camera.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, SwitchNode, DlodNode);
WM5_IMPLEMENT_STREAM(DlodNode);
WM5_IMPLEMENT_FACTORY(DlodNode);
WM5_IMPLEMENT_DEFAULT_NAMES(SwitchNode, DlodNode);

//----------------------------------------------------------------------------
DlodNode::DlodNode (int numLevelsOfDetail)
    :
    mNumLevelsOfDetail(numLevelsOfDetail)
{
    mModelMinDist = new1<float>(mNumLevelsOfDetail);
    mModelMaxDist = new1<float>(mNumLevelsOfDetail);
    mWorldMinDist = new1<float>(mNumLevelsOfDetail);
    mWorldMaxDist = new1<float>(mNumLevelsOfDetail);
}
//----------------------------------------------------------------------------
DlodNode::~DlodNode ()
{
    delete1(mModelMinDist);
    delete1(mModelMaxDist);
    delete1(mWorldMinDist);
    delete1(mWorldMaxDist);
}
//----------------------------------------------------------------------------
void DlodNode::SetModelDistance (int i, float minDist, float maxDist)
{
    if (0 <= i && i < mNumLevelsOfDetail)
    {
        mModelMinDist[i] = minDist;
        mModelMaxDist[i] = maxDist;
        mWorldMinDist[i] = minDist;
        mWorldMaxDist[i] = maxDist;
        return;
    }

    assertion(false, "Invalid index in SetModelDistance.\n");
}
//----------------------------------------------------------------------------
void DlodNode::SelectLevelOfDetail (const Camera* camera)
{
    // ASSERT:  The child array of an DlodNode is compacted--there are no
    // empty slots in the array and the number of children is mChild.size().
    // Moreover, it is assumed that all model distance values were set for
    // these children.

    // Compute the world LOD center.
    mWorldLodCenter = WorldTransform*mModelLodCenter;

    // Compute the world squared-distance intervals.
    int i;
    for (i = 0; i < mNumLevelsOfDetail; ++i)
    {
        mWorldMinDist[i] = WorldTransform.GetUniformScale()*mModelMinDist[i];
        mWorldMaxDist[i] = WorldTransform.GetUniformScale()*mModelMaxDist[i];
    }

    // Select the LOD child.
    SetActiveChild(SN_INVALID_CHILD);
    AVector diff = mWorldLodCenter - camera->GetPosition();
    float dist = diff.Length();
    for (i = 0; i < mNumLevelsOfDetail; ++i) 
    {
        if (mWorldMinDist[i] <= dist && dist < mWorldMaxDist[i])
        {
            SetActiveChild(i);
            break;
        }
    }
}
//----------------------------------------------------------------------------
void DlodNode::GetVisibleSet (Culler& culler, bool noCull)
{
    SelectLevelOfDetail(culler.GetCamera());
    SwitchNode::GetVisibleSet(culler, noCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
DlodNode::DlodNode (LoadConstructor value)
    :
    SwitchNode(value),
    mNumLevelsOfDetail(0),
    mModelMinDist(0),
    mModelMaxDist(0),
    mWorldMinDist(0),
    mWorldMaxDist(0)
{
}
//----------------------------------------------------------------------------
void DlodNode::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    SwitchNode::Load(source);

    source.ReadAggregate(mModelLodCenter);
    source.ReadRR(mNumLevelsOfDetail, mModelMinDist);
    source.ReadVR(mNumLevelsOfDetail, mModelMaxDist);
    source.ReadVR(mNumLevelsOfDetail, mWorldMinDist);
    source.ReadVR(mNumLevelsOfDetail, mWorldMaxDist);

    WM5_END_DEBUG_STREAM_LOAD(DlodNode, source);
}
//----------------------------------------------------------------------------
void DlodNode::Link (InStream& source)
{
    SwitchNode::Link(source);
}
//----------------------------------------------------------------------------
void DlodNode::PostLink ()
{
    SwitchNode::PostLink();
}
//----------------------------------------------------------------------------
bool DlodNode::Register (OutStream& target) const
{
    return SwitchNode::Register(target);
}
//----------------------------------------------------------------------------
void DlodNode::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    SwitchNode::Save(target);

    target.WriteAggregate(mModelLodCenter);
    target.WriteW(mNumLevelsOfDetail, mModelMinDist);
    target.WriteN(mNumLevelsOfDetail, mModelMaxDist);
    target.WriteN(mNumLevelsOfDetail, mWorldMinDist);
    target.WriteN(mNumLevelsOfDetail, mWorldMaxDist);

    // The world LOD center is computed from the model LOD center in
    // SelectLevelOfDetail.  The world distance extremes are also computed
    // from the model distance extremes in SelectLevelOfDetail.  These
    // world quantities do not need to be saved.

    WM5_END_DEBUG_STREAM_SAVE(DlodNode, target);
}
//----------------------------------------------------------------------------
int DlodNode::GetStreamingSize () const
{
    int size = SwitchNode::GetStreamingSize();
    size += sizeof(mModelLodCenter);
    size += sizeof(mNumLevelsOfDetail);
    size += mNumLevelsOfDetail*sizeof(float);
    size += mNumLevelsOfDetail*sizeof(float);
    size += mNumLevelsOfDetail*sizeof(float);
    size +=mNumLevelsOfDetail*sizeof(float);
    return size;
}
//----------------------------------------------------------------------------
