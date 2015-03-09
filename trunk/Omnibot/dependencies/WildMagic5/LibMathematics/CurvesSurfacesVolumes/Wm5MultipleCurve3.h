// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MULTIPLECURVE3_H
#define WM5MULTIPLECURVE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Curve3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM MultipleCurve3 : public Curve3<Real>
{
public:
    // Construction and destruction for abstract base class.  MultipleCurve3
    // accepts responsibility for deleting the input array.
    MultipleCurve3 (int numSegments, Real* times);
    virtual ~MultipleCurve3 ();

    // Member access.
    int GetSegments () const;
    const Real* GetTimes () const;

    // Length-from-time and time-from-length.
    virtual Real GetLength (Real t0, Real t1) const;
    virtual Real GetTime (Real length, int iterations = 32,
        Real tolerance = (Real)1e-06) const;

protected:
    using Curve3<Real>::mTMin;
    using Curve3<Real>::mTMax;

    int mNumSegments;
    Real* mTimes;

    // These quantities are allocated by GetLength when they are needed the
    // first time.  The allocations occur in InitializeLength (called by
    // GetLength), so this member function must be 'const'. In order to
    // allocate the arrays in a 'const' function, they must be declared as
    // 'mutable'.
    mutable Real* mLengths;
    mutable Real* mAccumLengths;

    void GetKeyInfo (Real t, int& key, Real& dt) const;

    void InitializeLength () const;
    virtual Real GetSpeedKey (int key, Real t) const = 0;
    virtual Real GetLengthKey (int key, Real t0, Real t1) const = 0;

    static Real GetSpeedWithData (Real t, void* data);
};

typedef MultipleCurve3<float> MultipleCurve3f;
typedef MultipleCurve3<double> MultipleCurve3d;

}

#endif
