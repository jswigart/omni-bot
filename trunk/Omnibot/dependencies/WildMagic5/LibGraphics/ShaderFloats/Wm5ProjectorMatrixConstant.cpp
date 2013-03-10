// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ProjectorMatrixConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, ProjectorMatrixConstant);
WM5_IMPLEMENT_STREAM(ProjectorMatrixConstant);
WM5_IMPLEMENT_FACTORY(ProjectorMatrixConstant);

//----------------------------------------------------------------------------
ProjectorMatrixConstant::ProjectorMatrixConstant (Projector* projector,
    bool biased, int bsMatrix)
    :
    ShaderFloat(4),
    mProjector(projector),
    mBiased(biased),
    mBSMatrix(bsMatrix)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
ProjectorMatrixConstant::~ProjectorMatrixConstant ()
{
}
//----------------------------------------------------------------------------
Projector* ProjectorMatrixConstant::GetProjector ()
{
    return mProjector;
}
//----------------------------------------------------------------------------
void ProjectorMatrixConstant::Update (const Visual* visual, const Camera*)
{
    const HMatrix& PVMatrix = mProjector->GetProjectionViewMatrix();
    const HMatrix& WMatrix = visual->WorldTransform.Matrix();
    HMatrix PVWMatrix = PVMatrix*WMatrix;
    if (mBiased)
    {
        PVWMatrix = Projector::BiasScaleMatrix[mBSMatrix]*PVWMatrix;
    }

    const float* source = (const float*)PVWMatrix;
    float* target = mData;
    for (int i = 0; i < 16; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* ProjectorMatrixConstant::GetObjectByName (const std::string& name)
{
    Object* found = ShaderFloat::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mProjector, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void ProjectorMatrixConstant::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    ShaderFloat::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mProjector, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
ProjectorMatrixConstant::ProjectorMatrixConstant (LoadConstructor value)
    :
    ShaderFloat(value)
{
}
//----------------------------------------------------------------------------
void ProjectorMatrixConstant::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    ShaderFloat::Load(source);

    source.ReadPointer(mProjector);
    source.ReadBool(mBiased);
    source.Read(mBSMatrix);

    WM5_END_DEBUG_STREAM_LOAD(ProjectorMatrixConstant, source);
}
//----------------------------------------------------------------------------
void ProjectorMatrixConstant::Link (InStream& source)
{
    ShaderFloat::Link(source);

    source.ResolveLink(mProjector);
}
//----------------------------------------------------------------------------
void ProjectorMatrixConstant::PostLink ()
{
    ShaderFloat::PostLink();
}
//----------------------------------------------------------------------------
bool ProjectorMatrixConstant::Register (OutStream& target) const
{
    if (ShaderFloat::Register(target))
    {
        target.Register(mProjector);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void ProjectorMatrixConstant::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    ShaderFloat::Save(target);

    target.WritePointer(mProjector);
    target.WriteBool(mBiased);
    target.Write(mBSMatrix);

    WM5_END_DEBUG_STREAM_SAVE(ProjectorMatrixConstant, target);
}
//----------------------------------------------------------------------------
int ProjectorMatrixConstant::GetStreamingSize () const
{
    int size = ShaderFloat::GetStreamingSize();
    size += WM5_POINTERSIZE(mProjector);
    size += WM5_BOOLSIZE(mBiased);
    size += sizeof(mBSMatrix);
    return size;
}
//----------------------------------------------------------------------------
