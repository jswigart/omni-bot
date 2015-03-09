// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5LightNode.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, LightNode);
WM5_IMPLEMENT_STREAM(LightNode);
WM5_IMPLEMENT_FACTORY(LightNode);

//----------------------------------------------------------------------------
LightNode::LightNode (Light* light)
    :
    mLight(light)
{
    if (mLight)
    {
        LocalTransform.SetTranslate(mLight->Position);

        HMatrix rotate(mLight->DVector, mLight->UVector, mLight->RVector,
            APoint::ORIGIN, true);

        LocalTransform.SetRotate(rotate);
    }
}
//----------------------------------------------------------------------------
LightNode::~LightNode ()
{
}
//----------------------------------------------------------------------------
void LightNode::SetLight (Light* light)
{
    mLight = light;

    if (mLight)
    {
        LocalTransform.SetTranslate(mLight->Position);

        HMatrix rotate(mLight->DVector, mLight->UVector, mLight->RVector,
            APoint::ORIGIN, true);

        LocalTransform.SetRotate(rotate);

        Update();
    }
}
//----------------------------------------------------------------------------
void LightNode::UpdateWorldData (double applicationTime)
{
    Node::UpdateWorldData(applicationTime);

    if (mLight)
    {
        mLight->Position = WorldTransform.GetTranslate();
        WorldTransform.GetRotate().GetColumn(0, mLight->DVector);
        WorldTransform.GetRotate().GetColumn(1, mLight->UVector);
        WorldTransform.GetRotate().GetColumn(2, mLight->RVector);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* LightNode::GetObjectByName (const std::string& name)
{
    Object* found = Node::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mLight, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void LightNode::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Node::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mLight, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
LightNode::LightNode (LoadConstructor value)
    :
    Node(value)
{
}
//----------------------------------------------------------------------------
void LightNode::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    source.ReadPointer(mLight);

    WM5_END_DEBUG_STREAM_LOAD(LightNode, source);
}
//----------------------------------------------------------------------------
void LightNode::Link (InStream& source)
{
    Node::Link(source);

    source.ResolveLink(mLight);
}
//----------------------------------------------------------------------------
void LightNode::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool LightNode::Register (OutStream& target) const
{
    if (Node::Register(target))
    {
        target.Register(mLight);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void LightNode::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    target.WritePointer(mLight);

    WM5_END_DEBUG_STREAM_SAVE(LightNode, target);
}
//----------------------------------------------------------------------------
int LightNode::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();
    size += WM5_POINTERSIZE(mLight);
    return size;
}
//----------------------------------------------------------------------------
