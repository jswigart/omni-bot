// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Projector.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Camera, Projector);
WM5_IMPLEMENT_STREAM(Projector);
WM5_IMPLEMENT_FACTORY(Projector);
WM5_IMPLEMENT_DEFAULT_NAMES(Camera, Projector);

const HMatrix Projector::BiasScaleMatrix[2] =
{
    HMatrix(
        0.5f,  0.0f, 0.0f, 0.5f,
        0.0f, -0.5f, 0.0f, 0.5f,
        0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f),

    HMatrix(
        0.5f,  0.0f, 0.0f, 0.5f,
        0.0f, +0.5f, 0.0f, 0.5f,
        0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f)
};

//----------------------------------------------------------------------------
Projector::Projector (DepthType depthType, bool isPerspective)
    :
    Camera(isPerspective)
{
    // Override the depth type set by the Camera constructor.
    mDepthType = depthType;
}
//----------------------------------------------------------------------------
Projector::~Projector ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Projector::Projector (LoadConstructor value)
    :
    Camera(value)
{
}
//----------------------------------------------------------------------------
void Projector::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Camera::Load(source);

    source.ReadEnum(mDepthType);

    // The Camera::Load sets mDepthType and updates the projection matrix,
    // which depends on the depth type.  Because we have now changed the depth
    // type, we need to update the projection matrix again.
    OnFrustumChange();

    WM5_END_DEBUG_STREAM_LOAD(Projector, source);
}
//----------------------------------------------------------------------------
void Projector::Link (InStream& source)
{
    Camera::Link(source);
}
//----------------------------------------------------------------------------
void Projector::PostLink ()
{
    Camera::PostLink();
}
//----------------------------------------------------------------------------
bool Projector::Register (OutStream& target) const
{
    return Camera::Register(target);
}
//----------------------------------------------------------------------------
void Projector::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);
    
    Camera::Save(target);

    target.WriteEnum(mDepthType);

    WM5_END_DEBUG_STREAM_SAVE(Projector, target);
}
//----------------------------------------------------------------------------
int Projector::GetStreamingSize () const
{
    int size = Camera::GetStreamingSize();
    size += WM5_ENUMSIZE(mDepthType);
    return size;
}
//----------------------------------------------------------------------------
