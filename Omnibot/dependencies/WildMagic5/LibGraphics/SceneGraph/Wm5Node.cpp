// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/09)

#include "Wm5GraphicsPCH.h"
#include "Wm5Node.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Spatial, Node);
WM5_IMPLEMENT_STREAM(Node);
WM5_IMPLEMENT_FACTORY(Node);

//----------------------------------------------------------------------------
Node::Node ()
{
}
//----------------------------------------------------------------------------
Node::~Node ()
{
    std::vector<SpatialPtr>::iterator iter = mChild.begin();
    std::vector<SpatialPtr>::iterator end = mChild.end();
    for (/**/; iter != end; ++iter)
    {
        if (*iter)
        {
            (*iter)->SetParent(0);
            *iter = 0;
        }
    }
}
//----------------------------------------------------------------------------
int Node::AttachChild (Spatial* child)
{
    if (!child)
    {
        assertion(false, "You cannot attach null children to a node.\n");
        return -1;
    }

    if (child->GetParent())
    {
        assertion(false, "The child already has a parent.\n");
        return -1;
    }

    child->SetParent(this);

    // Insert the child in the first available slot (if any).
    std::vector<SpatialPtr>::iterator iter = mChild.begin();
    std::vector<SpatialPtr>::iterator end = mChild.end();
    for (int i = 0; iter != end; ++iter, ++i)
    {
        if (*iter == 0)
        {
            *iter = child;
            return i;
        }
    }

    // All slots are used, so append the child to the array.
    const int numChildren = (int)mChild.size();
    mChild.push_back(child);
    return numChildren;
}
//----------------------------------------------------------------------------
int Node::DetachChild (Spatial* child)
{
    if (child)
    {
        std::vector<SpatialPtr>::iterator iter = mChild.begin();
        std::vector<SpatialPtr>::iterator end = mChild.end();
        for (int i = 0; iter != end; ++iter, ++i)
        {
            if (*iter == child)
            {
                (*iter)->SetParent(0);
                *iter = 0;
                return i;
            }
        }
    }
    return -1;
}
//----------------------------------------------------------------------------
SpatialPtr Node::DetachChildAt (int i)
{
    if (0 <= i && i < (int)mChild.size())
    {
        SpatialPtr child = mChild[i];
        if (child)
        {
            child->SetParent(0);
            mChild[i] = 0;
        }
        return child;
    }
    return 0;
}
//----------------------------------------------------------------------------
SpatialPtr Node::SetChild (int i, Spatial* child)
{
    if (child)
    {
        assertion(!child->GetParent(), "The child already has a parent.\n");
    }

    const int numChildren = (int)mChild.size();
    if (0 <= i && i < numChildren)
    {
        // Remove the child currently in the slot.
        SpatialPtr previousChild = mChild[i];
        if (previousChild)
        {
            previousChild->SetParent(0);
        }

        // Insert the new child in the slot.
        if (child)
        {
            child->SetParent(this);
        }

        mChild[i] = child;
        return previousChild;
    }

    // The index is out of range, so append the child to the array.
    if (child)
    {
        child->SetParent(this);
    }
    mChild.push_back(child);
    return 0;
}
//----------------------------------------------------------------------------
SpatialPtr Node::GetChild (int i)
{
    if (0 <= i && i < (int)mChild.size())
    {
        return mChild[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
void Node::UpdateWorldData (double applicationTime)
{
    Spatial::UpdateWorldData(applicationTime);

    std::vector<SpatialPtr>::iterator iter = mChild.begin();
    std::vector<SpatialPtr>::iterator end = mChild.end();
    for (/**/; iter != end; ++iter)
    {
        Spatial* child = *iter;
        if (child)
        {
            child->Update(applicationTime, false);
        }
    }
}
//----------------------------------------------------------------------------
void Node::UpdateWorldBound ()
{
    if (!WorldBoundIsCurrent)
    {
        // Start with an invalid bound.
        WorldBound.SetCenter(APoint::ORIGIN);
        WorldBound.SetRadius(0.0f);

        std::vector<SpatialPtr>::iterator iter = mChild.begin();
        std::vector<SpatialPtr>::iterator end = mChild.end();
        for (/**/; iter != end; ++iter)
        {
            Spatial* child = *iter;
            if (child)
            {
                // GrowToContain ignores invalid child bounds.  If the world
                // bound is invalid and a child bound is valid, the child
                // bound is copied to the world bound.  If the world bound and
                // child bound are valid, the smallest bound containing both
                // bounds is assigned to the world bound.
                WorldBound.GrowToContain(child->WorldBound);
            }
        }
    }
}
//----------------------------------------------------------------------------
void Node::GetVisibleSet (Culler& culler, bool noCull)
{
    std::vector<SpatialPtr>::iterator iter = mChild.begin();
    std::vector<SpatialPtr>::iterator end = mChild.end();
    for (/**/; iter != end; ++iter)
    {
        Spatial* child = *iter;
        if (child)
        {
            child->OnGetVisibleSet(culler, noCull);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* Node::GetObjectByName (const std::string& name)
{
    Object* found = Spatial::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    const int numChildren = (int)mChild.size();
    for (int i = 0; i < numChildren; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mChild[i], name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void Node::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Spatial::GetAllObjectsByName(name, objects);

    const int numChildren = (int)mChild.size();
    for (int i = 0; i < numChildren; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mChild[i], name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Node::Node (LoadConstructor value)
    :
    Spatial(value)
{
}
//----------------------------------------------------------------------------
void Node::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Spatial::Load(source);

    int numChildren;
    source.Read(numChildren);
    if (numChildren > 0)
    {
        mChild.resize(numChildren);
        source.ReadPointerVV(numChildren, &mChild[0]);
    }

    WM5_END_DEBUG_STREAM_LOAD(Node, source);
}
//----------------------------------------------------------------------------
void Node::Link (InStream& source)
{
    Spatial::Link(source);

    const int numChildren = (int)mChild.size();
    for (int i = 0; i < numChildren; ++i)
    {
        if (mChild[i])
        {
            source.ResolveLink(mChild[i]);
            SetChild(i, mChild[i]);
        }
    }
}
//----------------------------------------------------------------------------
void Node::PostLink ()
{
    Spatial::PostLink();
}
//----------------------------------------------------------------------------
bool Node::Register (OutStream& target) const
{
    if (Spatial::Register(target))
    {
        const int numChildren = (int)mChild.size();
        for (int i = 0; i < numChildren; ++i)
        {
            if (mChild[i])
            {
                target.Register(mChild[i]);
            }
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Node::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Spatial::Save(target);

    const int numChildren = (int)mChild.size();
    target.Write(numChildren);
    for (int i = 0; i < numChildren; ++i)
    {
        if (mChild[i])
        {
            target.WritePointer(mChild[i]);
        }
        else
        {
            target.WritePointer((Spatial*)0);
        }
    }


    WM5_END_DEBUG_STREAM_SAVE(Node, target);
}
//----------------------------------------------------------------------------
int Node::GetStreamingSize () const
{
    int size = Spatial::GetStreamingSize();
    int numChildren = (int)mChild.size();
    size += sizeof(numChildren);
    size += numChildren*WM5_POINTERSIZE(mChild[0]);
    return size;
}
//----------------------------------------------------------------------------
