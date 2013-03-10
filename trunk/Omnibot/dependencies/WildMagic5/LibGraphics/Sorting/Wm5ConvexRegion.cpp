// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ConvexRegion.h"
#include "Wm5Portal.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, ConvexRegion);
WM5_IMPLEMENT_STREAM(ConvexRegion);
WM5_IMPLEMENT_FACTORY(ConvexRegion);

//----------------------------------------------------------------------------
ConvexRegion::ConvexRegion (int numPortals, Portal** portals)
    :
    mNumPortals(numPortals),
    mPortals(portals),
    mVisited(false)
{
}
//----------------------------------------------------------------------------
ConvexRegion::~ConvexRegion ()
{
    for (int i = 0; i < mNumPortals; ++i)
    {
        delete0(mPortals[i]);
    }
    delete1(mPortals);
}
//----------------------------------------------------------------------------
void ConvexRegion::UpdateWorldData (double applicationTime)
{
    // Update the region walls and contained objects.
    Node::UpdateWorldData(applicationTime);

    // Update the portal geometry.
    for (int i = 0; i < mNumPortals; ++i)
    {
        mPortals[i]->UpdateWorldData(WorldTransform);
    }
}
//----------------------------------------------------------------------------
void ConvexRegion::GetVisibleSet (Culler& culler, bool noCull)
{
    if (!mVisited)
    {
        mVisited = true;

        // Add anything visible through open portals.
        for (int i = 0; i < mNumPortals; ++i)
        {
            mPortals[i]->GetVisibleSet(culler, noCull);
        }

        // Add the region walls and contained objects.
        Node::GetVisibleSet(culler, noCull);

        mVisited = false;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* ConvexRegion::GetObjectByName (const std::string& name)
{
    Object* found = Node::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    for (int i = 0; i < mNumPortals; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mPortals[i], name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void ConvexRegion::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Node::GetAllObjectsByName(name, objects);

    for (int i = 0; i < mNumPortals; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mPortals[i], name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
ConvexRegion::ConvexRegion (LoadConstructor value)
    :
    Node(value),
    mNumPortals(0),
    mPortals(0),
    mVisited(false)
{
}
//----------------------------------------------------------------------------
void ConvexRegion::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    source.ReadPointerRR(mNumPortals, mPortals);

    WM5_END_DEBUG_STREAM_LOAD(ConvexRegion, source);
}
//----------------------------------------------------------------------------
void ConvexRegion::Link (InStream& source)
{
    Node::Link(source);

    source.ResolveLink(mNumPortals, mPortals);
}
//----------------------------------------------------------------------------
void ConvexRegion::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool ConvexRegion::Register (OutStream& target) const
{
    if (Node::Register(target))
    {
        target.Register(mNumPortals, mPortals);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void ConvexRegion::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    target.WritePointerW(mNumPortals, mPortals);

    WM5_END_DEBUG_STREAM_SAVE(ConvexRegion, target);
}
//----------------------------------------------------------------------------
int ConvexRegion::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();
    size += sizeof(mNumPortals);
    size += mNumPortals*WM5_POINTERSIZE(mPortals[0]);
    return size;
}
//----------------------------------------------------------------------------
