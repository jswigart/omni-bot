// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BEZIERCURVE2_H
#define WM5BEZIERCURVE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5SingleCurve2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BezierCurve2 : public SingleCurve2<Real>
{
public:
    // Construction and destruction.  BezierCurve2 accepts responsibility for
    // deleting the input array.
    BezierCurve2 (int degree, Vector2<Real>* ctrlPoint);
    virtual ~BezierCurve2 ();

    int GetDegree () const;
    const Vector2<Real>* GetControlPoints () const;

    virtual Vector2<Real> GetPosition (Real t) const;
    virtual Vector2<Real> GetFirstDerivative (Real t) const;
    virtual Vector2<Real> GetSecondDerivative (Real t) const;
    virtual Vector2<Real> GetThirdDerivative (Real t) const;

protected:
    int mDegree;
    int mNumCtrlPoints;
    Vector2<Real>* mCtrlPoint;
    Vector2<Real>* mDer1CtrlPoint;
    Vector2<Real>* mDer2CtrlPoint;
    Vector2<Real>* mDer3CtrlPoint;
    Real** mChoose;
};

typedef BezierCurve2<float> BezierCurve2f;
typedef BezierCurve2<double> BezierCurve2d;

}

#endif
