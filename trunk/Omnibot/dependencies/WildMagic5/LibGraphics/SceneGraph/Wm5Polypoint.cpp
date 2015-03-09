// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Polypoint.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Visual, Polypoint);
WM5_IMPLEMENT_STREAM(Polypoint);
WM5_IMPLEMENT_FACTORY(Polypoint);
WM5_IMPLEMENT_DEFAULT_NAMES(Visual, Polypoint);

//----------------------------------------------------------------------------
Polypoint::Polypoint (VertexFormat* vformat, VertexBuffer* vbuffer)
    :
    Visual(PT_POLYPOINT, vformat, vbuffer, 0)
{
    mNumPoints = mVBuffer->GetNumElements();
}
//----------------------------------------------------------------------------
Polypoint::~Polypoint ()
{
}
//----------------------------------------------------------------------------
int Polypoint::GetMaxNumPoints () const
{
    return mVBuffer->GetNumElements();
}
//----------------------------------------------------------------------------
void Polypoint::SetNumPoints (int numPoints)
{
    int numVertices = mVBuffer->GetNumElements();
    if (0 <= numPoints && numPoints <= numVertices)
    {
        mNumPoints = numPoints;
    }
    else
    {
        mNumPoints = numVertices;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Polypoint::Polypoint (LoadConstructor value)
    :
    Visual(value),
    mNumPoints(0)
{
}
//----------------------------------------------------------------------------
void Polypoint::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Visual::Load(source);

    source.Read(mNumPoints);

    WM5_END_DEBUG_STREAM_LOAD(Polypoint, source);
}
//----------------------------------------------------------------------------
void Polypoint::Link (InStream& source)
{
    Visual::Link(source);
}
//----------------------------------------------------------------------------
void Polypoint::PostLink ()
{
    Visual::PostLink();
}
//----------------------------------------------------------------------------
bool Polypoint::Register (OutStream& target) const
{
    return Visual::Register(target);
}
//----------------------------------------------------------------------------
void Polypoint::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Visual::Save(target);

    target.Write(mNumPoints);

    WM5_END_DEBUG_STREAM_SAVE(Polypoint, target);
}
//----------------------------------------------------------------------------
int Polypoint::GetStreamingSize () const
{
    int size = Visual::GetStreamingSize();
    size += sizeof(mNumPoints);
    return size;
}
//----------------------------------------------------------------------------
