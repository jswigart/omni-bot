// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TERRAINPAGE_H
#define WM5TERRAINPAGE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM TerrainPage : public TriMesh
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(TerrainPage);

public:
    // Construction and destruction.  The following is required:
    // size = 2^p + 1, p <= 7 (size = 3, 5, 9, 17, 33, 65, 129)
    TerrainPage (VertexFormat* vformat, int size, unsigned short* heights,
        const Float2& origin, float minElevation, float maxElevation,
        float spacing);

    virtual ~TerrainPage ();

    // Height field access.
    inline int GetSize () const;
    inline const unsigned short* GetHeights () const;
    inline const Float2& GetOrigin () const;
    inline float GetMinElevation () const;
    inline float GetMaxElevation () const;
    inline float GetSpacing () const;

    // Height field measurements.  If the location is not in the page, the
    // return value is MAX_REAL.
    float GetHeight (float x, float y) const;

protected:
    // Support for tessellation.
    inline float GetX (int x) const;
    inline float GetY (int y) const;
    inline float GetHeight (int index) const;

    // Height field parameters.
    int mSize, mSizeM1;
    unsigned short* mHeights;
    Float2 mOrigin;
    float mMinElevation, mMaxElevation, mSpacing;
    float mInvSpacing, mMultiplier;
};

WM5_REGISTER_STREAM(TerrainPage);
typedef Pointer0<TerrainPage> TerrainPagePtr;
#include "Wm5TerrainPage.inl"

}

#endif
