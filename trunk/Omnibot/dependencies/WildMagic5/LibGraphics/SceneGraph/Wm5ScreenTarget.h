// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SCREENTARGET_H
#define WM5SCREENTARGET_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Camera.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ScreenTarget
{
public:
    // Create a screen-space camera for use with render targets.
    static Camera* CreateCamera ();

    // Create a screen-space rectangle for a render target of the specified
    // dimensions.  The vertex format must have 3-tuple positions and 2-tuple
    // texture coordinates in unit 0.  These attributes are filled in by the
    // function.  Any other attributes are not processed.  The rectangle
    // [xmin,xmax]x[ymin,ymax] must be contained in [0,1]x[0,1].
    static TriMesh* CreateRectangle (VertexFormat* vformat, int rtWidth,
        int rtHeight, float xmin, float xmax, float ymin, float ymax,
        float zValue);

    // Copy the screen-space rectangle positions to the input array.
    static bool CreatePositions (int rtWidth, int rtHeight, float xmin,
        float xmax, float ymin, float ymax, float zValue, Float3* positions);

    // Copy the screen-space rectangle texture coordinates to the input array.
    static void CreateTCoords (Float2* tcoords);

private:
    static bool ValidSizes (int rtWidth, int rtHeight);
    static bool ValidFormat (VertexFormat* vformat);
};

}

#endif
