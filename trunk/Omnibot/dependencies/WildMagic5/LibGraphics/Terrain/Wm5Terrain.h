// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TERRAIN_H
#define WM5TERRAIN_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Node.h"
#include "Wm5Camera.h"
#include "Wm5TerrainPage.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Terrain : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Terrain);

public:
    // Construction and destruction.
    Terrain (const std::string& heightName, VertexFormat* vformat,
        Camera* camera, int mode = FileIO::FM_DEFAULT_READ);

    virtual ~Terrain ();

    // Member access.
    inline int GetRowQuantity () const;
    inline int GetColQuantity () const;
    inline int GetSize () const;
    inline float GetMinElevation () const;
    inline float GetMaxElevation () const;
    inline float GetSpacing () const;

    // Page management.
    TerrainPage* GetPage (int row, int col);
    TerrainPage* GetCurrentPage (float x, float y) const;
    float GetHeight (float x, float y) const;

    // Estimate a normal vector at (x,y) by using the neighbors (x+dx,y+dy),
    // where (dx,dy) in {(s,0),(-s,0),(0,s),(0,-s)}.  The value s is the
    // spacing (returned by GetSpacing()).
    AVector GetNormal (float x, float y) const;

    // Allow a page to be replaced.  The code unstitches the old page, loads
    // the new page and stitches it, then returns the old page in case the
    // application wants to cache it for quick reuse.
    TerrainPagePtr ReplacePage (int row, int col,
        const std::string& heightName, const std::string& heightSuffix);

    TerrainPagePtr ReplacePage (int row, int col, TerrainPage* newPage);

    // Update of active set of terrain pages.
    void OnCameraMotion ();

protected:
    void LoadHeader (const std::string& heightName);

    void LoadPage (int row, int col, const std::string& heightName,
        const std::string& heightSuffix);

    // Read mode for the height header file and the terrain page data.
    int mMode;

    // Shared by the pages.
    VertexFormatPtr mVFormat;

    // Page information.
    int mNumRows, mNumCols;
    int mSize;
    float mMinElevation, mMaxElevation, mSpacing;
    TerrainPagePtr** mPages;

    // Current page containing the camera.
    int mCameraRow, mCameraCol;
    CameraPtr mCamera;
};

WM5_REGISTER_STREAM(Terrain);
typedef Pointer0<Terrain> TerrainPtr;
#include "Wm5Terrain.inl"

}

#endif
