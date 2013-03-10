// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5SwitchNode.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, SwitchNode);
WM5_IMPLEMENT_STREAM(SwitchNode);
WM5_IMPLEMENT_FACTORY(SwitchNode);
WM5_IMPLEMENT_DEFAULT_NAMES(Node, SwitchNode);

//----------------------------------------------------------------------------
SwitchNode::SwitchNode ()
    :
    mActiveChild(SN_INVALID_CHILD)
{
}
//----------------------------------------------------------------------------
SwitchNode::~SwitchNode ()
{
}
//----------------------------------------------------------------------------
void SwitchNode::GetVisibleSet (Culler& culler, bool noCull)
{
    if (mActiveChild == SN_INVALID_CHILD)
    {
        return;
    }

    // All Visual objects in the active subtree are added to the visible set.
    Spatial* child = mChild[mActiveChild];
    if (child)
    {
        child->OnGetVisibleSet(culler, noCull);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SwitchNode::SwitchNode (LoadConstructor value)
    :
    Node(value),
    mActiveChild(SN_INVALID_CHILD)
{
}
//----------------------------------------------------------------------------
void SwitchNode::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    source.Read(mActiveChild);

    WM5_END_DEBUG_STREAM_LOAD(SwitchNode, source);
}
//----------------------------------------------------------------------------
void SwitchNode::Link (InStream& source)
{
    Node::Link(source);
}
//----------------------------------------------------------------------------
void SwitchNode::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool SwitchNode::Register (OutStream& target) const
{
    return Node::Register(target);
}
//----------------------------------------------------------------------------
void SwitchNode::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    target.Write(mActiveChild);

    WM5_END_DEBUG_STREAM_SAVE(SwitchNode, target);
}
//----------------------------------------------------------------------------
int SwitchNode::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();
    size += sizeof(mActiveChild);
    return size;
}
//----------------------------------------------------------------------------
