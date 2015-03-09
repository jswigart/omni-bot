// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5BspNode.h"
#include "Wm5Camera.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, BspNode);
WM5_IMPLEMENT_STREAM(BspNode);
WM5_IMPLEMENT_FACTORY(BspNode);
WM5_IMPLEMENT_DEFAULT_NAMES(Node, BspNode);

//----------------------------------------------------------------------------
BspNode::BspNode ()
    :
    ModelPlane(0.0f, 0.0f, 0.0f, 0.0f),
    mWorldPlane(0.0f, 0.0f, 0.0f, 0.0f)
{
    mChild.push_back(0);  // left child
    mChild.push_back(0);  // middle child
    mChild.push_back(0);  // right child
}
//----------------------------------------------------------------------------
BspNode::BspNode (const HPlane& modelPlane)
    :
    ModelPlane(modelPlane),
    mWorldPlane(modelPlane)
{
    mChild.push_back(0);  // left child
    mChild.push_back(0);  // middle child
    mChild.push_back(0);  // right child
}
//----------------------------------------------------------------------------
BspNode::~BspNode ()
{
}
//----------------------------------------------------------------------------
Spatial* BspNode::GetContainingNode (const APoint& point)
{
    SpatialPtr posChild = GetPositiveChild();
    SpatialPtr negChild = GetNegativeChild();

    if (posChild || negChild)
    {
        BspNode* bspChild;

        if (mWorldPlane.WhichSide(point) < 0)
        {
            bspChild = DynamicCast<BspNode>(negChild);
            if (bspChild)
            {
                return bspChild->GetContainingNode(point);
            }
            else
            {
                return negChild;
            }
        }
        else
        {
            bspChild = DynamicCast<BspNode>(posChild);
            if (bspChild)
            {
                return bspChild->GetContainingNode(point);
            }
            else
            {
                return posChild;
            }
        }
    }

    return this;
}
//----------------------------------------------------------------------------
void BspNode::UpdateWorldData (double applicationTime)
{
    Node::UpdateWorldData(applicationTime);
    mWorldPlane = ModelPlane*WorldTransform.Inverse();
    mWorldPlane.Normalize();
}
//----------------------------------------------------------------------------
void BspNode::GetVisibleSet (Culler& culler, bool noCull)
{
    // Get visible Geometry in back-to-front order.  If a global effect is
    // active, the Geometry objects in the subtree will be drawn using it.
    SpatialPtr posChild = GetPositiveChild();
    SpatialPtr copChild = GetCoplanarChild();
    SpatialPtr negChild = GetNegativeChild();

    const Camera* camera = culler.GetCamera();
    int positionSide = mWorldPlane.WhichSide(camera->GetPosition());
    int frustumSide = culler.WhichSide(mWorldPlane);

    if (positionSide > 0)
    {
        // Camera origin on positive side of plane.

        if (frustumSide <= 0)
        {
            // The frustum is on the negative side of the plane or straddles
            // the plane.  In either case, the negative child is potentially
            // visible.
            if (negChild)
            {
                negChild->OnGetVisibleSet(culler, noCull);
            }
        }

        if (frustumSide == 0)
        {
            // The frustum straddles the plane.  The coplanar child is
            // potentially visible.
            if (copChild)
            {
                copChild->OnGetVisibleSet(culler, noCull);
            }
        }

        if (frustumSide >= 0)
        {
            // The frustum is on the positive side of the plane or straddles
            // the plane.  In either case, the positive child is potentially
            // visible.
            if (posChild)
            {
                posChild->OnGetVisibleSet(culler, noCull);
            }
        }
    }
    else if (positionSide < 0)
    {
        // Camera origin on negative side of plane.

        if (frustumSide >= 0)
        {
            // The frustum is on the positive side of the plane or straddles
            // the plane.  In either case, the positive child is potentially
            // visible.
            if (posChild)
            {
                posChild->OnGetVisibleSet(culler, noCull);
            }
        }

        if (frustumSide == 0)
        {
            // The frustum straddles the plane.  The coplanar child is
            // potentially visible.
            if (copChild)
            {
                copChild->OnGetVisibleSet(culler, noCull);
            }
        }

        if (frustumSide <= 0)
        {
            // The frustum is on the negative side of the plane or straddles
            // the plane.  In either case, the negative child is potentially
            // visible.
            if (negChild)
            {
                negChild->OnGetVisibleSet(culler, noCull);
            }
        }
    }
    else
    {
        // Camera origin on plane itself.  Both sides of the plane are
        // potentially visible as well as the plane itself.  Select the
        // first-to-be-drawn half space to be the one to which the camera
        // direction points.
        float NdD = mWorldPlane.GetNormal().Dot(camera->GetDVector());
        if (NdD >= 0.0f)
        {
            if (posChild)
            {
                posChild->OnGetVisibleSet(culler, noCull);
            }

            if (copChild)
            {
                copChild->OnGetVisibleSet(culler, noCull);
            }

            if (negChild)
            {
                negChild->OnGetVisibleSet(culler, noCull);
            }
        }
        else
        {
            if (negChild)
            {
                negChild->OnGetVisibleSet(culler, noCull);
            }

            if (copChild)
            {
                copChild->OnGetVisibleSet(culler, noCull);
            }

            if (posChild)
            {
                posChild->OnGetVisibleSet(culler, noCull);
            }
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
BspNode::BspNode (LoadConstructor value)
    :
    Node(value),
    ModelPlane(0.0f, 0.0f, 0.0f, 0.0f),
    mWorldPlane(0.0f, 0.0f, 0.0f, 0.0f)
{
}
//----------------------------------------------------------------------------
void BspNode::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    source.ReadAggregate(ModelPlane);
    source.ReadAggregate(mWorldPlane);

    WM5_END_DEBUG_STREAM_LOAD(BspNode, source);
}
//----------------------------------------------------------------------------
void BspNode::Link (InStream& source)
{
    Node::Link(source);
}
//----------------------------------------------------------------------------
void BspNode::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool BspNode::Register (OutStream& target) const
{
    return Node::Register(target);
}
//----------------------------------------------------------------------------
void BspNode::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    target.Write(ModelPlane);
    target.Write(mWorldPlane);

    WM5_END_DEBUG_STREAM_SAVE(BspNode, target);
}
//----------------------------------------------------------------------------
int BspNode::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();
    size += sizeof(ModelPlane);
    size += sizeof(mWorldPlane);
    return size;
}
//----------------------------------------------------------------------------
