// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5SkinController.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Controller, SkinController);
WM5_IMPLEMENT_STREAM(SkinController);
WM5_IMPLEMENT_FACTORY(SkinController);
WM5_IMPLEMENT_DEFAULT_NAMES(Controller, SkinController);

//----------------------------------------------------------------------------
SkinController::SkinController (int numVertices, int numBones)
    :
    mNumVertices(numVertices),
    mNumBones(numBones)
{
    mBones = new1<Node*>(mNumBones);
    mWeights = new2<float>(mNumBones, mNumVertices);
    mOffsets = new2<APoint>(mNumBones, mNumVertices);
}
//----------------------------------------------------------------------------
SkinController::~SkinController ()
{
    delete1(mBones);
    delete2(mWeights);
    delete2(mOffsets);
}
//----------------------------------------------------------------------------
bool SkinController::Update (double applicationTime)
{
    if (!Controller::Update(applicationTime))
    {
        return false;
    }

    // Get access to the vertex buffer to store the blended targets.
    Visual* visual = StaticCast<Visual>(mObject);
    assertion(mNumVertices == visual->GetVertexBuffer()->GetNumElements(),
        "Controller must have the same number of vertices as the buffer\n");
    VertexBufferAccessor vba(visual);

    // The skin vertices are calculated in the bone world coordinate system,
    // so the visual's world transform must be the identity.
    visual->WorldTransform = Transform::IDENTITY;
    visual->WorldTransformIsCurrent = true;

    // Compute the skin vertex locations.
    for (int vertex = 0; vertex < mNumVertices; ++vertex)
    {
        APoint position = APoint::ORIGIN;
        for (int bone = 0; bone < mNumBones; ++bone)
        {
            float weight = mWeights[vertex][bone];
            if (weight != 0.0f)
            {
                APoint offset = mOffsets[vertex][bone];
                APoint worldOffset = mBones[bone]->WorldTransform*offset;
                position += weight*worldOffset;
            }
        }
        vba.Position<Float3>(vertex) = position;
    }

    visual->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(visual->GetVertexBuffer());
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SkinController::SkinController (LoadConstructor value)
    :
    Controller(value),
    mNumVertices(0),
    mNumBones(0),
    mBones(0),
    mWeights(0),
    mOffsets(0)
{
}
//----------------------------------------------------------------------------
void SkinController::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Controller::Load(source);

    source.Read(mNumVertices);
    source.Read(mNumBones);

    int numWeightsOffsets = mNumVertices*mNumBones;
    mWeights = new2<float>(mNumBones, mNumVertices);
    mOffsets = new2<APoint>(mNumBones, mNumVertices);
    source.ReadVV(numWeightsOffsets, mWeights[0]);
    source.ReadAggregateVV(numWeightsOffsets, mOffsets[0]);

    source.ReadPointerVR(mNumBones, mBones);

    WM5_END_DEBUG_STREAM_LOAD(SkinController, source);
}
//----------------------------------------------------------------------------
void SkinController::Link (InStream& source)
{
    Controller::Link(source);

    source.ResolveLink(mNumBones, mBones);
}
//----------------------------------------------------------------------------
void SkinController::PostLink ()
{
    Controller::PostLink();
}
//----------------------------------------------------------------------------
bool SkinController::Register (OutStream& target) const
{
    if (Controller::Register(target))
    {
        target.Register(mNumBones, mBones);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void SkinController::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Controller::Save(target);

    target.Write(mNumVertices);
    target.Write(mNumBones);

    int numWeightsOffsets = mNumVertices*mNumBones;
    target.WriteN(numWeightsOffsets, mWeights[0]);
    target.WriteAggregateN(numWeightsOffsets, mOffsets[0]);

    target.WritePointerN(mNumBones, mBones);

    WM5_END_DEBUG_STREAM_SAVE(SkinController, target);
}
//----------------------------------------------------------------------------
int SkinController::GetStreamingSize () const
{
    int size = Controller::GetStreamingSize();
    size += sizeof(mNumVertices);
    size += sizeof(mNumBones);

    int numWeightsOffsets = mNumVertices*mNumBones;
    size += numWeightsOffsets*sizeof(mWeights[0][0]);
    size += numWeightsOffsets*sizeof(mOffsets[0][0]);

    size += mNumBones*WM5_POINTERSIZE(mBones[0]);

    return size;
}
//----------------------------------------------------------------------------
