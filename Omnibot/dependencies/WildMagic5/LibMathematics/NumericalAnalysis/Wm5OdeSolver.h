// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ODESOLVER_H
#define WM5ODESOLVER_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM OdeSolver
{
public:
    // The system is dx/dt = F(t,x).  The dimension of x is passed to the
    // constructor of OdeSolver.
    typedef void (*Function)(
        Real,         // t
        const Real*,  // x
        void*,        // user-specified data
        Real*);       // F(t,x)

    // Abstract base class.
protected:
    OdeSolver (int dim, Real step, Function function, void* userData = 0);
public:
    virtual ~OdeSolver ();

    virtual void Update (Real tIn, Real* xIn, Real& tOut, Real* xOut) = 0;
    virtual void SetStepSize (Real step) = 0;
    Real GetStepSize () const;
    void SetUserData (void* UserData);
    void* GetUserData () const;

protected:
    int mDim;
    Real mStep;
    Function mFunction;
    void* mUserData;
    Real* mFValue;
};

typedef OdeSolver<float> OdeSolverf;
typedef OdeSolver<double> OdeSolverd;

}

#endif
