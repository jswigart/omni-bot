// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5SINGLECURVE2_H
#define WM5SINGLECURVE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Curve2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM SingleCurve2 : public Curve2<Real>
{
public:
    // Abstract base class.
    SingleCurve2 (Real tmin, Real tmax);

    // Length-from-time and time-from-length.
    virtual Real GetLength (Real t0, Real t1) const;
    virtual Real GetTime (Real length, int iterations = 32,
        Real tolerance = (Real)1e-06) const;

protected:
    using Curve2<Real>::mTMin;
    using Curve2<Real>::mTMax;
    using Curve2<Real>::GetSpeed;
    using Curve2<Real>::GetTotalLength;

    static Real GetSpeedWithData (Real t, void* data);
};

typedef SingleCurve2<float> SingleCurve2f;
typedef SingleCurve2<double> SingleCurve2d;

}

#endif
