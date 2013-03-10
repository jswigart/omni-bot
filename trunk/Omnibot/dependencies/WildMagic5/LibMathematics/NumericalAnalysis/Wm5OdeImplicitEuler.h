// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5ODEIMPLICITEULER_H
#define WM5ODEIMPLICITEULER_H

#include "Wm5MathematicsLIB.h"
#include "Wm5OdeSolver.h"
#include "Wm5GMatrix.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM OdeImplicitEuler : public OdeSolver<Real>
{
public:
    // The function F(t,x) has input t, a scalar, and input x, an n-vector.
    // The first derivative matrix with respect to x is DF(t,x), an
    // n-by-n matrix.  Entry DF[r][c] is the derivative of F[r] with
    // respect to x[c].
    typedef void (*DerivativeFunction)(
        Real,             // t
        const Real*,      // x
        void*,            // user-specified data
        GMatrix<Real>&);  // DF(t,x)

    // Construction and destruction.
    OdeImplicitEuler (int dim, Real step,
        typename OdeSolver<Real>::Function function,
        DerivativeFunction derFunction, void* userData = 0);
    virtual ~OdeImplicitEuler ();

    virtual void Update (Real tIn, Real* xIn, Real& tOut, Real* xOut);
    virtual void SetStepSize (Real step);

protected:
    using OdeSolver<Real>::mDim;
    using OdeSolver<Real>::mStep;
    using OdeSolver<Real>::mFunction;
    using OdeSolver<Real>::mUserData;

    DerivativeFunction mDerFunction;
    GMatrix<Real> mDF;
    GVector<Real> mF;
    GMatrix<Real> mIdentity;
};

typedef OdeImplicitEuler<float> OdeImplicitEulerf;
typedef OdeImplicitEuler<double> OdeImplicitEulerd;

}

#endif
