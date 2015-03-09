// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5GRADIENTANISOTROPIC2_H
#define WM5GRADIENTANISOTROPIC2_H

#include "Wm5ImagicsLIB.h"
#include "Wm5PdeFilter2.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM GradientAnisotropic2 : public PdeFilter2
{
public:
    GradientAnisotropic2 (int xBound, int yBound, float xSpacing,
        float ySpacing, const float* data, const bool* mask,
        float borderValue, ScaleType scaleType, float K);

    virtual ~GradientAnisotropic2 ();

protected:
    void ComputeParam ();
    virtual void OnPreUpdate ();
    virtual void OnUpdate (int x, int y);

    float mK;           // k

    // These are updated on each iteration, since they depend on the current
    // average of the squared length of the gradients at the pixels.
    float mParam;       // 1/(k^2*average(gradMagSqr))
    float mMHalfParam;  // -0.5*mParam;
};

}

#endif
