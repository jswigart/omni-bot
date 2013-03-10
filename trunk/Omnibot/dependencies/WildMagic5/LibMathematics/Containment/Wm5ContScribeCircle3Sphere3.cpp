// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContScribeCircle3Sphere3.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
bool Circumscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, Circle3<Real>& circle)
{
    Vector3<Real> E02 = v0 - v2;
    Vector3<Real> E12 = v1 - v2;
    Real e02e02 = E02.Dot(E02);
    Real e02e12 = E02.Dot(E12);
    Real e12e12 = E12.Dot(E12);
    Real det = e02e02*e12e12 - e02e12*e02e12;
    if (Math<Real>::FAbs(det) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    Real halfInvDet = ((Real)0.5)/det;
    Real u0 = halfInvDet*e12e12*(e02e02 - e02e12);
    Real u1 = halfInvDet*e02e02*(e12e12 - e02e12);
    Vector3<Real> tmp = u0*E02 + u1*E12;

    circle.Center = v2 + tmp;
    circle.Radius = tmp.Length();

    circle.Normal = E02.UnitCross(E12);

    if (Math<Real>::FAbs(circle.Normal.X())
        >= Math<Real>::FAbs(circle.Normal.Y())
    &&  Math<Real>::FAbs(circle.Normal.X())
        >= Math<Real>::FAbs(circle.Normal.Z()))
    {
        circle.Direction0.X() = -circle.Normal.Y();
        circle.Direction0.Y() = circle.Normal.X();
        circle.Direction0.Z() = (Real)0;
    }
    else
    {
        circle.Direction0.X() = (Real)0;
        circle.Direction0.Y() = circle.Normal.Z();
        circle.Direction0.Z() = -circle.Normal.Y();
    }

    circle.Direction0.Normalize();
    circle.Direction1 = circle.Normal.Cross(circle.Direction0);

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Circumscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, const Vector3<Real>& v3,
    Sphere3<Real>& sphere)
{
    Vector3<Real> E10 = v1 - v0;
    Vector3<Real> E20 = v2 - v0;
    Vector3<Real> E30 = v3 - v0;

    Real A[3][3] =
    {
        {E10.X(), E10.Y(), E10.Z()},
        {E20.X(), E20.Y(), E20.Z()},
        {E30.X(), E30.Y(), E30.Z()}
    };

    Real B[3] =
    {
        ((Real)0.5)*E10.SquaredLength(),
        ((Real)0.5)*E20.SquaredLength(),
        ((Real)0.5)*E30.SquaredLength()
    };

    Vector3<Real> solution;
    if (LinearSystem<Real>().Solve3(A, B,(Real*)&solution))
    {
        sphere.Center = v0 + solution;
        sphere.Radius = solution.Length();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Inscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, Circle3<Real>& circle)
{
    // Edges.
    Vector3<Real> E0 = v1 - v0;
    Vector3<Real> E1 = v2 - v1;
    Vector3<Real> E2 = v0 - v2;

    // Plane normal.
    circle.Normal = E1.Cross(E0);

    // Edge normals within the plane.
    Vector3<Real> N0 = circle.Normal.UnitCross(E0);
    Vector3<Real> N1 = circle.Normal.UnitCross(E1);
    Vector3<Real> N2 = circle.Normal.UnitCross(E2);

    Real a0 = N1.Dot(E0);
    if (Math<Real>::FAbs(a0) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    Real a1 = N2.Dot(E1);
    if (Math<Real>::FAbs(a1) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    Real a2 = N0.Dot(E2);
    if (Math<Real>::FAbs(a2) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    Real invA0 = ((Real)1)/a0;
    Real invA1 = ((Real)1)/a1;
    Real invA2 = ((Real)1)/a2;

    circle.Radius = ((Real)1)/(invA0 + invA1 + invA2);
    circle.Center = circle.Radius*(invA0*v0 + invA1*v1 + invA2*v2);

    circle.Normal.Normalize();
    circle.Direction0 = N0;
    circle.Direction1 = circle.Normal.Cross(circle.Direction0);

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Inscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, const Vector3<Real>& v3,
    Sphere3<Real>& sphere)
{
    // Edges.
    Vector3<Real> E10 = v1 - v0;
    Vector3<Real> E20 = v2 - v0;
    Vector3<Real> E30 = v3 - v0;
    Vector3<Real> E21 = v2 - v1;
    Vector3<Real> E31 = v3 - v1;

    // Normals.
    Vector3<Real> N0 = E31.Cross(E21);
    Vector3<Real> N1 = E20.Cross(E30);
    Vector3<Real> N2 = E30.Cross(E10);
    Vector3<Real> N3 = E10.Cross(E20);

    // Normalize the normals.
    if (Math<Real>::FAbs(N0.Normalize()) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }
    if (Math<Real>::FAbs(N1.Normalize()) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }
    if (Math<Real>::FAbs(N2.Normalize()) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }
    if (Math<Real>::FAbs(N3.Normalize()) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    Real A[3][3] =
    {
        {N1.X() - N0.X(), N1.Y() - N0.Y(), N1.Z() - N0.Z()},
        {N2.X() - N0.X(), N2.Y() - N0.Y(), N2.Z() - N0.Z()},
        {N3.X() - N0.X(), N3.Y() - N0.Y(), N3.Z() - N0.Z()}
    };

    Real B[3] =
    {
        (Real)0,
        (Real)0,
        -N3.Dot(E30)
    };

    Vector3<Real> solution;
    if (LinearSystem<Real>().Solve3(A, B, (Real*)&solution))
    {
        sphere.Center = v3 + solution;
        sphere.Radius = Math<Real>::FAbs(N0.Dot(solution));
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
bool Circumscribe<float> (const Vector3<float>&, const Vector3<float>&,
    const Vector3<float>&, Circle3<float>&);

template WM5_MATHEMATICS_ITEM
bool Circumscribe<float> (const Vector3<float>&, const Vector3<float>&,
    const Vector3<float>&, const Vector3<float>&, Sphere3<float>&);

template WM5_MATHEMATICS_ITEM
bool Inscribe<float> (const Vector3<float>&, const Vector3<float>&,
    const Vector3<float>&, Circle3<float>&);

template WM5_MATHEMATICS_ITEM
bool Inscribe<float> (const Vector3<float>&, const Vector3<float>&,
    const Vector3<float>&, const Vector3<float>&, Sphere3<float>&);


template WM5_MATHEMATICS_ITEM
bool Circumscribe<double> (const Vector3<double>&, const Vector3<double>&,
    const Vector3<double>&, Circle3<double>&);

template WM5_MATHEMATICS_ITEM
bool Circumscribe<double> (const Vector3<double>&, const Vector3<double>&,
    const Vector3<double>&, const Vector3<double>&, Sphere3<double>&);

template WM5_MATHEMATICS_ITEM
bool Inscribe<double> (const Vector3<double>&, const Vector3<double>&,
    const Vector3<double>&, Circle3<double>&);

template WM5_MATHEMATICS_ITEM
bool Inscribe<double> (const Vector3<double>&, const Vector3<double>&,
    const Vector3<double>&, const Vector3<double>&, Sphere3<double>&);
//----------------------------------------------------------------------------
}
