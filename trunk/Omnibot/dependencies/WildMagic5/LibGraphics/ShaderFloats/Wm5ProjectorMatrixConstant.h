// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PROJECTORMATRIXCONSTANT_H
#define WM5PROJECTORMATRIXCONSTANT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Projector.h"
#include "Wm5ShaderFloat.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ProjectorMatrixConstant : public ShaderFloat
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ProjectorMatrixConstant);

public:
    // Construction and destruction.  Set bsMatrix to 0 for the
    // bias-scale matrix that maps y' = (1-y)/2.  Set bsMatrix to 1 for the
    // bias-scale matrix that maps y' = (1-y)/2.
    ProjectorMatrixConstant (Projector* projector, bool biased,
        int bsMatrix);
    virtual ~ProjectorMatrixConstant ();

    // Member access.
    Projector* GetProjector ();

    virtual void Update (const Visual* visual, const Camera* camera);

protected:
    ProjectorPtr mProjector;
    bool mBiased;
    int mBSMatrix;
};

WM5_REGISTER_STREAM(ProjectorMatrixConstant);
typedef Pointer0<ProjectorMatrixConstant> ProjectorMatrixConstantPtr;

}

#endif
