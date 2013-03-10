// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POLYPOINT_H
#define WM5POLYPOINT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Visual.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Polypoint : public Visual
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Polypoint);

public:
    // Construction and destruction.
    Polypoint (VertexFormat* vformat, VertexBuffer* vbuffer);
    virtual ~Polypoint ();

    // Member access.  Allow the application to specify fewer than the maximum
    // number of point to draw.
    int GetMaxNumPoints () const;
    void SetNumPoints (int numPoints);
    inline int GetNumPoints () const;

protected:
    // The number of points currently active.
    int mNumPoints;
};

WM5_REGISTER_STREAM(Polypoint);
typedef Pointer0<Polypoint> PolypointPtr;
#include "Wm5Polypoint.inl"

}

#endif
