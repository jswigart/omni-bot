// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5ConvexRegionManager.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, BspNode, ConvexRegionManager);
WM5_IMPLEMENT_STREAM(ConvexRegionManager);
WM5_IMPLEMENT_FACTORY(ConvexRegionManager);
WM5_IMPLEMENT_DEFAULT_NAMES(BspNode, ConvexRegionManager);
WM5_IMPLEMENT_DEFAULT_STREAM(BspNode, ConvexRegionManager);

//----------------------------------------------------------------------------
ConvexRegionManager::ConvexRegionManager ()
{
}
//----------------------------------------------------------------------------
ConvexRegionManager::ConvexRegionManager (const HPlane& modelPlane)
    :
    BspNode(modelPlane)
{
}
//----------------------------------------------------------------------------
ConvexRegionManager::~ConvexRegionManager ()
{
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::AttachOutside (Spatial* outside)
{
    return SetChild(1, outside);
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::DetachOutside ()
{
    return DetachChildAt(1);
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::GetOutside ()
{
    return GetChild(1);
}
//----------------------------------------------------------------------------
ConvexRegion* ConvexRegionManager::GetContainingRegion (const APoint& point)
{
    return DynamicCast<ConvexRegion>(GetContainingNode(point));
}
//----------------------------------------------------------------------------
void ConvexRegionManager::GetVisibleSet (Culler& culler, bool noCull)
{
    ConvexRegion* region = GetContainingRegion(
        culler.GetCamera()->GetPosition());

    if (region)
    {
        // Accumulate visible objects starting in the region containing the
        // camera.  Use the CRMCuller to maintain a list of unique objects.
        //pkRegion->GetVisibleSet(m_kCuller,bNoCull);
        mCuller.SetCamera(culler.GetCamera());
        mCuller.SetFrustum(culler.GetFrustum());
        mCuller.ComputeVisibleSet(region);

        // Copy the unique list to the scene culler.
        VisibleSet& visibleSet = mCuller.GetVisibleSet();
        const int numVisible = visibleSet.GetNumVisible();
        for (int i = 0; i < numVisible; ++i)
        {
            culler.Insert(visibleSet.GetVisible(i));
        }
    }
    else
    {
        // The camera is outside the set of regions.  Accumulate visible
        // objects for the outside scene (if it exists).
        if (GetOutside())
        {
            GetOutside()->GetVisibleSet(culler, noCull);
        }
    }
}
//----------------------------------------------------------------------------
