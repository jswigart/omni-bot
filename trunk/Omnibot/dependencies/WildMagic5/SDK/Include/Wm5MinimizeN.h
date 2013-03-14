// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/09)

#ifndef WM5MINIMIZEN_H
#define WM5MINIMIZEN_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Minimize1.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM MinimizeN
{
public:
    // The type of function to be minimized:  result = f(t,userData), where t
    // is a d-tuple (d is 'dimensions' in the constructor).  The userData is
    // the pointer provided by the constructor or SetUserData(*).  This allows
    // you to pass a class static function that wraps a call to a nonstatic
    // member function, in which case userData is a pointer to the class
    // object.  Naturally, the userData may be as complicated as you need
    // it to ensure that the 'function' has all the information needed to
    // compute it.
    typedef Real (*Function)(const Real*,void*);

    // Construction and destruction.  The Cartesian-product domain provided to
    // GetMinimum(*) has minimum values stored in t0[0..d-1] and maximum
    // values stored in t1[0..d-1], where d is 'dimensions'.  The domain is
    // searched along lines through the current estimate of the minimum
    // location.  Each such line is searched for a minimum using a
    // Minimize1<Real> object.  This is called "Powell's Direction Set
    // Method".  The parameters 'maxLevel' and 'maxBracket' are used by
    // Minimize1<Real>, so read the documentation for that class (in its
    // header file) to understand what these mean.  The input 'maxIterations'
    // is the number of iterations for the direction-set method.
    MinimizeN (int dimensions, Function function, int maxLevel,
        int maxBracket, int maxIterations, void* userData = 0);

    ~MinimizeN ();

    // Find the minimum on the Cartesian-product domain whose minimum values
    // are stored in t0[0..d-1] and whose maximum values are stored in
    // t1[0..d-1], where d is 'dimensions'.  An initial guess is specified in
    // tInitial[0..d-1].  The location of the minimum is tMin[0..d-1] and
    // the value of the minimum is 'fMin'.
    void GetMinimum (const Real* t0, const Real* t1, const Real* tInitial,
        Real* tMin, Real& fMin);

private:
    // The current estimate of the minimum location is mTCurr[0..d-1].  The
    // direction of the current line to search is mDCurr[0..d-1].  This line
    // must be clipped against the Cartesian-product domain, a process
    // implemented in this function.  If the line is mTCurr+s*mDCurr, the
    // clip result is the s-interval [ell0,ell1].
    void ComputeDomain (const Real* t0, const Real* t1, Real& ell0,
        Real& ell1);

    // The Minimize1<Real> object requires a C-style function to perform its
    // 1-dimensionsal search (along the line mTCurr+s*mDCurr).
    static Real LineFunction (Real t, void* userData);

    int mDimensions;
    Function mFunction;
    int mMaxIterations;
    void* mUserData;
    Minimize1<Real> mMinimizer;
    Real* mDirectionStorage;
    Real** mDirection;
    Real* mDConj;
    Real* mDCurr;
    Real* mTSave;
    Real* mTCurr;
    Real mFCurr;
    Real* mLineArg;
};

typedef MinimizeN<float> MinimizeNf;
typedef MinimizeN<double> MinimizeNd;

}

#endif
