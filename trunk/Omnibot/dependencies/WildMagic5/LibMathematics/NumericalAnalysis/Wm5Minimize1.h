// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/09)

#ifndef WM5MINIMIZE1_H
#define WM5MINIMIZE1_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Minimize1
{
public:
    // The type of function to be minimized:  result = f(t,userData).  The
    // userData is the pointer provided by the constructor or SetUserData(*).
    // This allows you to pass a class static function that wraps a call to a
    // nonstatic member function, in which case userData is a pointer to the
    // class object.  Naturally, the userData may be as complicated as you
    // need it to ensure that the 'function' has all the information needed to
    // compute it.
    typedef Real (*Function)(Real,void*);

    // Construction and destruction.  The interval [t0,t1] provided to
    // GetMinimum(Real,Real,Real,Real&,Real&) is processed by examining
    // subintervals.  On each subinteral [a,b], the values f0 = f(a),
    // f1 = f((a+b)/2), and f2 = f(b) are examined.  If {f0,f1,f2} is
    // monotonic, then [a,b] is subdivided and processed.  The maximum depth
    // of the recursion is limited by 'maxLevel'.  If {f0,f1,f2} is not
    // monotonic, then two cases arise.  First, if f1 = min{f0,f1,f2}, then
    // {f0,f1,f2} is said to "bracket a minimum" and GetBracketedMinimum(*) is
    // called to locate the function minimum.  The process uses a form of
    // bisection called "parabolic interpolation" and the maximum number of
    // bisection steps is 'maxBracket'.  Second, if f1 = max{f0,f1,f2}, then
    // {f0,f1,f2} brackets a maximum.  The minimum search continues
    // recursively as before on [a,(a+b)/2] and [(a+b)/2,b].
    Minimize1 (Function function, int maxLevel, int maxBracket,
        void* userData = 0);

    ~Minimize1 ();

    // Member access.
    void SetUserData (void* userData);
    void* GetUserData () const;

    // Search for a minimum of 'function' on the interval [t0,t1] using an
    // initial guess of 'tInitial'.  The location of the minimum is 'tMin' and
    // the value of the minimum is 'fMin'.
    void GetMinimum (Real t0, Real t1, Real tInitial, Real& tMin, Real& fMin);

private:
    // This is called to start the search on [t0,tInitial] and [tInitial,t1].
    void GetMinimum (Real t0, Real f0, Real t1, Real f1, int level);

    // This is called recursively to search [a,(a+b)/2] and [(a+b)/2,b].
    void GetMinimum (Real t0, Real f0, Real tm, Real fm, Real t1, Real f1,
        int level);

    // This is called when {f0,f1,f2} brackets a minimum.
    void GetBracketedMinimum (Real t0, Real f0, Real tm, Real fm, Real t1,
        Real f1, int level);

    Function mFunction;
    int mMaxLevel;
    int mMaxBracket;
    void* mUserData;
    Real mTMin, mFMin;
};

typedef Minimize1<float> Minimize1f;
typedef Minimize1<double> Minimize1d;

}

#endif
