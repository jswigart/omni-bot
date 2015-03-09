// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprEllipseFit2.h"
#include "Wm5ContBox2.h"
#include "Wm5DistPoint2Ellipse2.h"
#include "Wm5MinimizeN.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
EllipseFit2<Real>::EllipseFit2 (int numPoints, const Vector2<Real>* points,
    Vector2<Real>& center, Matrix2<Real>& rotate, Real diag[2],
    Real& error)
    :
    mNumPoints(numPoints),
    mPoints(points)
{
    // Energy function is E : R^5 -> R where
    // V = (V0, V1, V2, V3, V4)
    //   = (D[0], D[1], U.X(), U.Y(), atan2(R[1][0],R[1][1])).

    mTemp = new1<Vector2<Real> >(numPoints);

    MinimizeN<Real> minimizer(5, Energy, 8, 8, 32, this);

    InitialGuess(numPoints, mPoints, center, rotate, diag);
    Real angle = Math<Real>::ACos(rotate[0][0]);
    Real e0 = diag[0]*Math<Real>::FAbs(rotate[0][0]) + 
        diag[1]*Math<Real>::FAbs(rotate[0][1]);
    Real e1 = diag[0]*Math<Real>::FAbs(rotate[1][0]) +
        diag[1]*Math<Real>::FAbs(rotate[1][1]);

    Real v0[5] =
    {
        ((Real)0.5)*diag[0],
        ((Real)0.5)*diag[1],
        center.X() - e0,
        center.Y() - e1,
        (Real)0
    };

    Real v1[5] =
    {
        ((Real)2)*diag[0],
        ((Real)2)*diag[1],
        center.X() + e0,
        center.Y() + e1,
        Math<Real>::PI
    };

    Real vInitial[5] =
    {
        diag[0],
        diag[1],
        center.X(),
        center.Y(),
        angle
    };

    Real vMin[5];
    minimizer.GetMinimum(v0, v1, vInitial, vMin, error);

    diag[0] = vMin[0];
    diag[1] = vMin[1];
    center.X() = vMin[2];
    center.Y() = vMin[3];
    rotate.MakeRotation(vMin[4]);

    delete1(mTemp);
}
//----------------------------------------------------------------------------
template <typename Real>
void EllipseFit2<Real>::InitialGuess (int numPoints,
    const Vector2<Real>* points, Vector2<Real>& center,
    Matrix2<Real>& rotate, Real diag[2])
{
    Box2<Real> box = ContOrientedBox(numPoints, points);

    center = box.Center;
    rotate[0][0] = box.Axis[0].X();
    rotate[0][1] = box.Axis[0].Y();
    rotate[1][0] = box.Axis[1].X();
    rotate[1][1] = box.Axis[1].Y();
    diag[0] = box.Extent[0];
    diag[1] = box.Extent[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Real EllipseFit2<Real>::Energy (const Real* input, void* userData)
{
    EllipseFit2& self = *(EllipseFit2*)userData;

    // Build rotation matrix.
    Matrix2<Real> rotate(input[4]);

    Ellipse2<Real> ellipse(Vector2<Real>::ZERO, Vector2<Real>::UNIT_X,
        Vector2<Real>::UNIT_Y, input[0], input[1]);

    // Transform the points to the coordinate system of center C and columns
    // of rotation R.
    Real energy = (Real)0;
    for (int i = 0; i < self.mNumPoints; ++i)
    {
        Vector2<Real> diff(
            self.mPoints[i].X() - input[2],
            self.mPoints[i].Y() - input[3]);

        self.mTemp[i] = diff*rotate;
        Real dist = DistPoint2Ellipse2<Real>(self.mTemp[i], ellipse).Get();
        energy += dist;
    }

    return energy;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class EllipseFit2<float>;

template WM5_MATHEMATICS_ITEM
class EllipseFit2<double>;
//----------------------------------------------------------------------------
}
