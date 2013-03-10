// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.0 (2011/07/09)

#ifndef WM5BRENTSMETHOD_H
#define WM5BRENTSMETHOD_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Assert.h"

namespace Wm5
{

template <typename Real>
class BrentsMethod
{
public:
    // A function F(x), but with an additional parameter to allow you to pass
    // user-defined data that is needed to evaluate the function.
    typedef Real (*Function)(Real, void*);

    // Construction and destruction.  This is an implementation of Brent's
    // Method for computing a root of a function on an interval [x0,x1] for
    // which f(x0)*f(x1) < 0.  The method uses inverse quadratic interpolation
    // to generate a root estimate but falls back to inverse linear
    // interpolation (secant method) if necessary.  Moreover, based on
    // previous iterates, the method will fall back to bisection when it
    // appears the interpolated estimate is not of sufficient quality.
    //
    //   function:
    //       The function whose root is desired.  The function pointer must be
    //       non-null.
    //   maxIterations:
    //       The maximum number of iterations used to locate a root.  This
    //       should be positive.
    //   negFTolerance, posFTolerance:
    //       The root estimate x is accepted when the function value f(x)
    //       satisfies negFTolerance <= f(x) <= posFTolerance.  The values
    //       must satisfy:  negFTolerance <= 0, posFTolerance >= 0.
    //   stepXTolerance:
    //       Brent's Method requires additional tests before an interpolated
    //       x-value is accepted as the next root estimate.  One of these tests
    //       compares the difference of consecutive iterates and requires it
    //       to be larger than a user-specified x-tolerance (to ensure progress
    //       is made).  This parameter is that tolerance.
    //   convXTolerance:
    //       The root search is allowed to terminate when the current
    //       subinterval [xsub0,xsub1] is sufficiently small, say,
    //       |xsub1 - xsub0| <= tolerance.  This parameter is that tolerance.
    //   userData:
    //       This parameter allows you to pass specialized data that is
    //       required to evaluation the function.  Typically, 'function' is a
    //       static function of a class (to obtain a C-style function pointer)
    //       and 'userData' is 'this' (or a pointer to a subset of class data
    //       for 'this').

    BrentsMethod (Function function, int maxIterations, Real negFTolerance,
        Real posFTolerance, Real stepXTolerance, Real convXTolerance,
        void* userData);

    ~BrentsMethod ();

    // Member access.
    void SetFunction (Function function);
    Function GetFunction () const;
    void SetMaxIterations (int maxIterations);
    int GetMaxIterations () const;
    void SetNegFTolerance (Real negFTolerance);
    Real GetNegFTolerance () const;
    void SetPosFTolerance (Real posFTolerance);
    Real GetPosFTolerance () const;
    void SetStepXTolerance (Real stepXTolerance);
    Real GetStepXTolerance () const;
    void SetConvXTolerance (Real convXTolerance);
    Real GetConvXTolerance () const;
    void SetUserData (void* userData);
    void* GetUserData () const;

    // Compute a root, if any, on the interval [x0,x1].  The function returns
    // 'true' when the root is found, in which case 'xRoot' is the root and
    // 'fRoot' is the function value at the root.  The function returns
    // 'false' when the interval is invalid (x1 <= x0) or when the interval
    // does not bound a root (f(x0)*f(x1) > 0), in which case 'xRoot' and
    // 'fRoot' are indeterminate.
    bool GetRoot (Real x0, Real x1, Real& xRoot, Real& fRoot);

private:
    Function mFunction;
    int mMaxIterations;
    Real mNegFTolerance;
    Real mPosFTolerance;
    Real mStepXTolerance;
    Real mConvXTolerance;
    void* mUserData;
};

typedef BrentsMethod<float> BrentsMethodf;
typedef BrentsMethod<double> BrentsMethodd;

}

#endif
