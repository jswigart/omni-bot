// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprEllipsoidFit3.h"
#include "Wm5ContBox3.h"
#include "Wm5DistPoint3Ellipsoid3.h"
#include "Wm5MinimizeN.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
EllipsoidFit3<Real>::EllipsoidFit3 (int numPoints,
    const Vector3<Real>* points, Vector3<Real>& center,
    Matrix3<Real>& rotate, Real diag[3])
    :
    mNumPoints(numPoints),
    mPoints(points)
{
    // Energy function is E : R^9 -> R where
    // V = (V0,V1,V2,V3,V4,V5,V6,V7,V8)
    //   = (D[0],D[1],D[2],U.X(),U,y,U.Z(),A0,A1,A2). 
    // For really scattered data, you might need a search function

    mTemp = new1<Vector3<Real> >(numPoints);

    MinimizeN<Real> minimizer(9, Energy, 8, 8, 32, this);

    InitialGuess(numPoints, points, center, rotate, diag);

    Real angle[3];
    MatrixToAngles(rotate, angle);

    Real extent[3] =
    {
        diag[0]*Math<Real>::FAbs(rotate[0][0]) +
            diag[1]*Math<Real>::FAbs(rotate[0][1]) +
            diag[2]*Math<Real>::FAbs(rotate[0][2]),

        diag[0]*Math<Real>::FAbs(rotate[1][0]) +
            diag[1]*Math<Real>::FAbs(rotate[1][1]) +
            diag[2]*Math<Real>::FAbs(rotate[1][2]),

        diag[0]*Math<Real>::FAbs(rotate[2][0]) +
            diag[1]*Math<Real>::FAbs(rotate[2][1]) +
            diag[2]*Math<Real>::FAbs(rotate[2][2])
    };

    Real v0[9] =
    {
        ((Real)0.5)*diag[0],
        ((Real)0.5)*diag[1],
        ((Real)0.5)*diag[2],
        center.X() - extent[0],
        center.Y() - extent[1],
        center.Z() - extent[2],
        -Math<Real>::PI,
        (Real)0,
        (Real)0
    };

    Real v1[9] =
    {
        ((Real)2)*diag[0],
        ((Real)2)*diag[1],
        ((Real)2)*diag[2],
        center.X() + extent[0],
        center.Y() + extent[1],
        center.Z() + extent[2],
        Math<Real>::PI,
        Math<Real>::PI,
        Math<Real>::PI
    };

    Real vInitial[9] =
    {
        diag[0],
        diag[1],
        diag[2],
        center.X(),
        center.Y(),
        center.Z(),
        angle[0],
        angle[1],
        angle[2]
    };

    Real vMin[9];
    minimizer.GetMinimum(v0, v1, vInitial, vMin, mError);

    diag[0] = vMin[0];
    diag[1] = vMin[1];
    diag[2] = vMin[2];
    center.X() = vMin[3];
    center.Y() = vMin[4];
    center.Z() = vMin[5];

    AnglesToMatrix(&vMin[6], rotate);

    delete1(mTemp);
}
//----------------------------------------------------------------------------
template <typename Real>
EllipsoidFit3<Real>::operator Real ()
{
    return mError;
}
//----------------------------------------------------------------------------
template <typename Real>
void EllipsoidFit3<Real>::InitialGuess (int numPoints,
    const Vector3<Real>* points, Vector3<Real>& center,
    Matrix3<Real>& rotate, Real diag[3])
{
    Box3<Real> box = ContOrientedBox(numPoints, points);

    center = box.Center;
    rotate[0][0] = box.Axis[0].X();
    rotate[0][1] = box.Axis[0].Y();
    rotate[0][2] = box.Axis[0].Z();
    rotate[1][0] = box.Axis[1].X();
    rotate[1][1] = box.Axis[1].Y();
    rotate[1][2] = box.Axis[1].Z();
    rotate[2][0] = box.Axis[2].X();
    rotate[2][1] = box.Axis[2].Y();
    rotate[2][2] = box.Axis[2].Z();
    diag[0] = box.Extent[0];
    diag[1] = box.Extent[1];
    diag[2] = box.Extent[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Real EllipsoidFit3<Real>::Energy (const Real* input, void* userData)
{
    EllipsoidFit3& self = *(EllipsoidFit3*)userData;

    // Build rotation matrix.
    Matrix3<Real> rotate;
    AnglesToMatrix(&input[6], rotate);

    // Uniformly scale the extents to keep reasonable floating point values
    // in the distance calculations.
    Real maxValue = input[0];
    if (input[1] > maxValue)
    {
        maxValue = input[1];
    }
    if (input[2] > maxValue)
    {
        maxValue = input[2];
    }

    Real invMax = ((Real)1)/maxValue;
    Ellipsoid3<Real> ellipsoid(Vector3<Real>::ZERO, Vector3<Real>::UNIT_X,
        Vector3<Real>::UNIT_Y, Vector3<Real>::UNIT_Z, invMax*input[0],
        invMax*input[1], invMax*input[2]);

    // Transform the points to the coordinate system of center C and columns
    // of rotation R.
    Real energy = (Real)0;
    for (int i = 0; i < self.mNumPoints; ++i)
    {
        Vector3<Real> diff(
            self.mPoints[i].X() - input[3],
            self.mPoints[i].Y() - input[4],
            self.mPoints[i].Z() - input[5]);

        self.mTemp[i] = invMax*(diff*rotate);

        Real dist =
            DistPoint3Ellipsoid3<Real>(self.mTemp[i], ellipsoid).Get();
        energy += maxValue*dist;
    }

    return energy;
}
//----------------------------------------------------------------------------
template <typename Real>
void EllipsoidFit3<Real>::MatrixToAngles (const Matrix3<Real>& rotate,
    Real* angle)
{
    // rotation axis = (cos(a0)sin(a1),sin(a0)sin(a1),cos(a1))
    // a0 in [-pi,pi], a1 in [0,pi], a2 in [0,pi]

    Vector3<Real> axis;
    rotate.ExtractAxisAngle(axis, angle[2]);

    if (-(Real)1 < axis.Z())
    {
        if (axis.Z() < (Real)1)
        {
            angle[0] = Math<Real>::ATan2(axis.Y(), axis.X());
            angle[1] = Math<Real>::ACos(axis.Z());
        }
        else
        {
            angle[0] = (Real)0;
            angle[1] = (Real)0;
        }
    }
    else
    {
        angle[0] = (Real)0;
        angle[1] = Math<Real>::PI;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void EllipsoidFit3<Real>::AnglesToMatrix (const Real* angle,
    Matrix3<Real>& rotate)
{
    // rotation axis = (cos(a0)sin(a1),sin(a0)sin(a1),cos(a1))
    // a0 in [-pi,pi], a1 in [0,pi], a2 in [0,pi]

    Real cs0 = Math<Real>::Cos(angle[0]);
    Real sn0 = Math<Real>::Sin(angle[0]);
    Real cs1 = Math<Real>::Cos(angle[1]);
    Real sn1 = Math<Real>::Sin(angle[1]);
    Vector3<Real> axis(cs0*sn1, sn0*sn1, cs1);
    rotate.MakeRotation(axis, angle[2]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class EllipsoidFit3<float>;

template WM5_MATHEMATICS_ITEM
class EllipsoidFit3<double>;
//----------------------------------------------------------------------------
}
