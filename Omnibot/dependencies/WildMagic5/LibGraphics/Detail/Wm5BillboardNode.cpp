// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5BillboardNode.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, BillboardNode);
WM5_IMPLEMENT_STREAM(BillboardNode);
WM5_IMPLEMENT_FACTORY(BillboardNode);

//----------------------------------------------------------------------------
BillboardNode::BillboardNode (Camera* camera)
    :
    mCamera(camera)
{
}
//----------------------------------------------------------------------------
BillboardNode::~BillboardNode ()
{
}
//----------------------------------------------------------------------------
void BillboardNode::UpdateWorldData (double applicationTime)
{
    // Compute the billboard's world transforms based on its parent's world
    // transform and its local transforms.  Notice that you should not call
    // Node::UpdateWorldData since that function updates its children.  The
    // children of a BillboardNode cannot be updated until the billboard is
    // aligned with the camera.
    Spatial::UpdateWorldData(applicationTime);

    if (mCamera)
    {
        // Inverse-transform the camera to the model space of the billboard.
        APoint modelPos = WorldTransform.Inverse()*mCamera->GetPosition();

        // To align the billboard, the projection of the camera to the
        // xz-plane of the billboard's model space determines the angle of
        // rotation about the billboard's model y-axis.  If the projected
        // camera is on the model axis (x = 0 and z = 0), ATan2 returns zero
        // (rather than NaN), so there is no need to trap this degenerate
        // case and handle it separately.
        float angle = Mathf::ATan2(modelPos[0], modelPos[2]);
        HMatrix orient(AVector::UNIT_Y, angle);
        WorldTransform.SetRotate(WorldTransform.GetRotate()*orient);
    }

    // Update the children now that the billboard orientation is known.
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

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* BillboardNode::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mCamera, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void BillboardNode::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Node::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mCamera, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
BillboardNode::BillboardNode (LoadConstructor value)
    :
    Node(value),
    mCamera(0)
{
}
//----------------------------------------------------------------------------
void BillboardNode::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    source.ReadPointer(mCamera);

    WM5_END_DEBUG_STREAM_LOAD(BillboardNode, source);
}
//----------------------------------------------------------------------------
void BillboardNode::Link (InStream& source)
{
    Node::Link(source);

    source.ResolveLink(mCamera);
}
//----------------------------------------------------------------------------
void BillboardNode::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool BillboardNode::Register (OutStream& target) const
{
    if (Node::Register(target))
    {
        target.Register(mCamera);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void BillboardNode::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    target.WritePointer(mCamera);

    WM5_END_DEBUG_STREAM_SAVE(BillboardNode, target);
}
//----------------------------------------------------------------------------
int BillboardNode::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();
    size += WM5_POINTERSIZE(mCamera);
    return size;
}
//----------------------------------------------------------------------------
