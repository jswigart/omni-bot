// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DLODNODE_H
#define WM5DLODNODE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5SwitchNode.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM DlodNode : public SwitchNode
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(DlodNode);

public:
    // Construction and destruction.
    DlodNode (int numLevelsOfDetail);
    virtual ~DlodNode ();

    // Access to the center for level of detail (LOD).
    inline APoint& ModelCenter ();
    inline const APoint& GetModelCenter () const;
    inline const APoint& GetWorldCenter () const;

    // Access to the distance intervals for children.
    inline int GetNumLevelsOfDetail () const;
    inline float GetModelMinDistance (int i) const;
    inline float GetModelMaxDistance (int i) const;
    inline float GetWorldMinDistance (int i) const;
    inline float GetWorldMaxDistance (int i) const;
    void SetModelDistance (int i, float minDist, float maxDist);

protected:
    // Switch the child based on distance from world LOD center to camera.
    void SelectLevelOfDetail (const Camera* camera);

    // Support for hierarchical culling.
    virtual void GetVisibleSet (Culler& culler, bool noCull);

    // The point whose distance to the camera determines the correct child to
    // activate.
    APoint mModelLodCenter;
    APoint mWorldLodCenter;

    // Squared distances for each LOD interval.
    int mNumLevelsOfDetail;  // same as number of children of node
    float* mModelMinDist;
    float* mModelMaxDist;
    float* mWorldMinDist;
    float* mWorldMaxDist;
};

WM5_REGISTER_STREAM(DlodNode);
typedef Pointer0<DlodNode> DlodNodePtr;
#include "Wm5DlodNode.inl"

}

#endif
