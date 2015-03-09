// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTEGRATE1_H
#define WM5INTEGRATE1_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Integrate1
{
public:
    // The last parameter is for user-defined data.
    typedef Real (*Function)(Real,void*);

    static Real RombergIntegral (int order, Real a, Real b, Function function,
        void* userData = 0);

    static Real GaussianQuadrature (Real a, Real b, Function function,
        void* userData = 0);

    static Real TrapezoidRule (int numSamples, Real a, Real b,
        Function function, void* userData = 0);
};

typedef Integrate1<float> Integrate1f;
typedef Integrate1<double> Integrate1d;

}

#endif
