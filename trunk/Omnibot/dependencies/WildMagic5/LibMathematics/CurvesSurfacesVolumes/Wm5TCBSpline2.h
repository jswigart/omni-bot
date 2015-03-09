// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5TCBSPLINE2_H
#define WM5TCBSPLINE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5MultipleCurve2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM TCBSpline2 : public MultipleCurve2<Real>
{
public:
    // Construction and destruction.  TCBSpline2 accepts responsibility for
    // deleting the input arrays.
    TCBSpline2 (int numSegments, Real* times, Vector2<Real>* points,
        Real* tension, Real* continuity, Real* bias);

    virtual ~TCBSpline2 ();

    const Vector2<Real>* GetPoints () const;
    const Real* GetTensions () const;
    const Real* GetContinuities () const;
    const Real* GetBiases () const;

    virtual Vector2<Real> GetPosition (Real t) const;
    virtual Vector2<Real> GetFirstDerivative (Real t) const;
    virtual Vector2<Real> GetSecondDerivative (Real t) const;
    virtual Vector2<Real> GetThirdDerivative (Real t) const;

protected:
    using MultipleCurve2<Real>::mNumSegments;
    using MultipleCurve2<Real>::mTimes;
    using MultipleCurve2<Real>::GetKeyInfo;
    using MultipleCurve2<Real>::GetSpeedWithData;

    void ComputePoly (int i0, int i1, int i2, int i3);

    virtual Real GetSpeedKey (int key, Real t) const;
    virtual Real GetLengthKey (int key, Real t0, Real t1) const;

    Vector2<Real>* mPoints;
    Real* mTension;
    Real* mContinuity;
    Real* mBias;
    Vector2<Real>* mA;
    Vector2<Real>* mB;
    Vector2<Real>* mC;
    Vector2<Real>* mD;

    class WM5_MATHEMATICS_ITEM SplineKey
    {
    public:
        SplineKey (const TCBSpline2* spline, int key);

        const TCBSpline2* Spline;
        int Key;
    };
};

typedef TCBSpline2<float> TCBSpline2f;
typedef TCBSpline2<double> TCBSpline2d;

}

#endif
