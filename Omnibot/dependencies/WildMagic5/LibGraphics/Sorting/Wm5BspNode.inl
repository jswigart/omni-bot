// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline SpatialPtr BspNode::AttachPositiveChild (Spatial* child)
{
    return SetChild(0, child);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::AttachCoplanarChild (Spatial* child)
{
    return SetChild(1, child);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::AttachNegativeChild (Spatial* child)
{
    return SetChild(2, child);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::DetachPositiveChild ()
{
    return DetachChildAt(0);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::DetachCoplanarChild ()
{
    return DetachChildAt(1);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::DetachNegativeChild ()
{
    return DetachChildAt(2);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::GetPositiveChild ()
{
    return GetChild(0);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::GetCoplanarChild ()
{
    return GetChild(1);
}
//----------------------------------------------------------------------------
inline SpatialPtr BspNode::GetNegativeChild ()
{
    return GetChild(2);
}
//----------------------------------------------------------------------------
inline const HPlane& BspNode::GetWorldPlane () const
{
    return mWorldPlane;
}
//----------------------------------------------------------------------------
