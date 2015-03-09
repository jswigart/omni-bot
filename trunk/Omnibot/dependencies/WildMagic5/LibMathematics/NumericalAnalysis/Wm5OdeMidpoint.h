// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5ODEMIDPOINT_H
#define WM5ODEMIDPOINT_H

#include "Wm5MathematicsLIB.h"
#include "Wm5OdeSolver.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM OdeMidpoint : public OdeSolver<Real>
{
public:
    // Construction and destruction.
    OdeMidpoint (int dim, Real step,
        typename OdeSolver<Real>::Function function, void* userData = 0);
    virtual ~OdeMidpoint ();

    virtual void Update (Real tIn, Real* xIn, Real& tOut, Real* xOut);
    virtual void SetStepSize (Real step);

protected:
    using OdeSolver<Real>::mDim;
    using OdeSolver<Real>::mStep;
    using OdeSolver<Real>::mFunction;
    using OdeSolver<Real>::mUserData;
    using OdeSolver<Real>::mFValue;

    Real mHalfStep;
    Real* mXTemp;
};

typedef OdeMidpoint<float> OdeMidpointf;
typedef OdeMidpoint<double> OdeMidpointd;

}

#endif
