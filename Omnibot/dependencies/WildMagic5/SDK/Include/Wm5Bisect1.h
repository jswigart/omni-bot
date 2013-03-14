// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BISECT1_H
#define WM5BISECT1_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Bisect1
{
public:
    typedef Real (*Function)(Real);

    Bisect1 (Function function, int maxLevel, Real tolerance);

    bool Bisect (Real x0, Real x1, Real& root);

private:
    // Input data and functions.
    Function mFunction;
    int mMaxLevel;
    Real mTolerance;
};

typedef Bisect1<float> Bisect1f;
typedef Bisect1<double> Bisect1d;

}

#endif
