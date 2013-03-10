// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CRMCULLER_H
#define WM5CRMCULLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Culler.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM CRMCuller : public Culler
{
public:
    // Construction and destruction.
    CRMCuller (const Camera* camera = 0);
    virtual ~CRMCuller ();

    // Two portals leading into the room can be visible to the camera.  The
    // Culler would store visible objects twice.  CRMCuller maintains a set
    // of unique objects.
    virtual void Insert (Spatial* visible);

protected:
    std::set<Spatial*> mUnique;
};

}

#endif
