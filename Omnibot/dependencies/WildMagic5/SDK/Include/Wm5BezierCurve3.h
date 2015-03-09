// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BEZIERCURVE3_H
#define WM5BEZIERCURVE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5SingleCurve3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BezierCurve3 : public SingleCurve3<Real>
{
public:
    // Construction and destruction.  BezierCurve3 accepts responsibility for
    // deleting the input arrays.
    BezierCurve3 (int degree, Vector3<Real>* ctrlPoint);
    virtual ~BezierCurve3 ();

    int GetDegree () const;
    const Vector3<Real>* GetControlPoints () const;

    virtual Vector3<Real> GetPosition (Real t) const;
    virtual Vector3<Real> GetFirstDerivative (Real t) const;
    virtual Vector3<Real> GetSecondDerivative (Real t) const;
    virtual Vector3<Real> GetThirdDerivative (Real t) const;

protected:
    int mDegree;
    int mNumCtrlPoints;
    Vector3<Real>* mCtrlPoint;
    Vector3<Real>* mDer1CtrlPoint;
    Vector3<Real>* mDer2CtrlPoint;
    Vector3<Real>* mDer3CtrlPoint;
    Real** mChoose;
};

typedef BezierCurve3<float> BezierCurve3f;
typedef BezierCurve3<double> BezierCurve3d;

}

#endif
