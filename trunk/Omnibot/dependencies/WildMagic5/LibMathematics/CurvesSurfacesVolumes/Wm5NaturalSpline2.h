// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2012/06/24)

#ifndef WM5NATURALSPLINE2_H
#define WM5NATURALSPLINE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5MultipleCurve2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM NaturalSpline2 : public MultipleCurve2<Real>
{
public:
    enum BoundaryType
    {
        BT_FREE,
        BT_CLAMPED,
        BT_CLOSED
    };

    // Construction and destruction.
    //   1. If N is the number of points, the number of segments must be N-1.
    //   2. NaturalSpline2 accepts responsibility for deleting the input
    //      arrays, so these arrays must be dynamically allocated by the
    //      caller.
    //   3. When the boundary type is BT_CLAMPED, the endpoint derivatives are
    //      automatically chosen to be
    //        derivativeStart = points[1] - points[0]
    //        derivativeFinal = points[N] - points[N-1]
    //      To specify the derivatives for BT_CLAMPED, use the second
    //      constructor listed below.
    NaturalSpline2 (BoundaryType type, int numSegments, Real* times,
        Vector2<Real>* points);

    // Specify the derivative vectors for clamped splines.
    NaturalSpline2 (int numSegments, Real* times, Vector2<Real>* points,
        const Vector2<Real>& derivativeStart,
        const Vector2<Real>& derivativeFinal);

    virtual ~NaturalSpline2 ();

    const Vector2<Real>* GetPoints () const;

    virtual Vector2<Real> GetPosition (Real t) const;
    virtual Vector2<Real> GetFirstDerivative (Real t) const;
    virtual Vector2<Real> GetSecondDerivative (Real t) const;
    virtual Vector2<Real> GetThirdDerivative (Real t) const;

protected:
    using MultipleCurve2<Real>::mNumSegments;
    using MultipleCurve2<Real>::mTimes;
    using MultipleCurve2<Real>::GetKeyInfo;
    using MultipleCurve2<Real>::GetSpeedWithData;

    void CreateFreeSpline ();

    void CreateClampedSpline (const Vector2<Real>& derivativeStart,
        const Vector2<Real>& derivativeFinal);

    void CreateClosedSpline ();

    virtual Real GetSpeedKey (int key, Real t) const;
    virtual Real GetLengthKey (int key, Real t0, Real t1) const;

    Vector2<Real>* mA;
    Vector2<Real>* mB;
    Vector2<Real>* mC;
    Vector2<Real>* mD;

    class WM5_MATHEMATICS_ITEM SplineKey
    {
    public:
        SplineKey (const NaturalSpline2* spline, int key);

        const NaturalSpline2* Spline;
        int Key;
    };
};

typedef NaturalSpline2<float> NaturalSpline2f;
typedef NaturalSpline2<double> NaturalSpline2d;

}

#endif
