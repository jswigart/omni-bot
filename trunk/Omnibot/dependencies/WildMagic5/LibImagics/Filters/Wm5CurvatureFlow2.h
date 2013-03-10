// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CURVATUREFLOW2_H
#define WM5CURVATUREFLOW2_H

#include "Wm5ImagicsLIB.h"
#include "Wm5PdeFilter2.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM CurvatureFlow2 : public PdeFilter2
{
public:
    CurvatureFlow2 (int xBound, int yBound, float xSpacing,
        float ySpacing, const float* data, const bool* mask,
        float borderValue, ScaleType scaleType);

    virtual ~CurvatureFlow2 ();

protected:
    virtual void OnUpdate (int x, int y);
};

}

#endif
