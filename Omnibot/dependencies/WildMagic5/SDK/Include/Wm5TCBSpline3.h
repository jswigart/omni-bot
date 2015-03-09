// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5TCBSPLINE3_H
#define WM5TCBSPLINE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5MultipleCurve3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM TCBSpline3 : public MultipleCurve3<Real>
{
public:
    // Construction and destruction.  TCBSpline3 accepts responsibility for
    // deleting the input arrays.
    TCBSpline3 (int numSegments, Real* times, Vector3<Real>* points,
        Real* tension, Real* continuity, Real* bias);

    virtual ~TCBSpline3 ();

    const Vector3<Real>* GetPoints () const;
    const Real* GetTensions () const;
    const Real* GetContinuities () const;
    const Real* GetBiases () const;

    virtual Vector3<Real> GetPosition (Real t) const;
    virtual Vector3<Real> GetFirstDerivative (Real t) const;
    virtual Vector3<Real> GetSecondDerivative (Real t) const;
    virtual Vector3<Real> GetThirdDerivative (Real t) const;

protected:
    using MultipleCurve3<Real>::mNumSegments;
    using MultipleCurve3<Real>::mTimes;
    using MultipleCurve3<Real>::GetKeyInfo;
    using MultipleCurve3<Real>::GetSpeedWithData;

    void ComputePoly (int i0, int i1, int i2, int i3);

    virtual Real GetSpeedKey (int key, Real t) const;
    virtual Real GetLengthKey (int key, Real t0, Real t1) const;

    Vector3<Real>* mPoints;
    Real* mTension;
    Real* mContinuity;
    Real* mBias;
    Vector3<Real>* mA;
    Vector3<Real>* mB;
    Vector3<Real>* mC;
    Vector3<Real>* mD;

    class WM5_MATHEMATICS_ITEM SplineKey
    {
    public:
        SplineKey (const TCBSpline3* spline, int key);

        const TCBSpline3* Spline;
        int Key;
    };
};

typedef TCBSpline3<float> TCBSpline3f;
typedef TCBSpline3<double> TCBSpline3d;

}

#endif
