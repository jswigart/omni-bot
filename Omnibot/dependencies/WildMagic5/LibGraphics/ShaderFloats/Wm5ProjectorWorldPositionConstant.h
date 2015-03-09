// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PROJECTORWORLDPOSITIONCONSTANT_H
#define WM5PROJECTORWORLDPOSITIONCONSTANT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Projector.h"
#include "Wm5ShaderFloat.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ProjectorWorldPositionConstant : public ShaderFloat
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ProjectorWorldPositionConstant);

public:
    // Construction and destruction.
    ProjectorWorldPositionConstant (Projector* projector);
    virtual ~ProjectorWorldPositionConstant ();

    // Member access.
    Projector* GetProjector ();

    virtual void Update (const Visual* visual, const Camera* camera);

protected:
    ProjectorPtr mProjector;
};

WM5_REGISTER_STREAM(ProjectorWorldPositionConstant);
typedef Pointer0<ProjectorWorldPositionConstant>
    ProjectorWorldPositionConstantPtr;

}

#endif
