// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5PolyhedralMassProperties.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
void ComputeMassProperties (const Vector3<Real>* vertices, int numTriangles,
    const int* indices, bool bodyCoords, Real& mass, Vector3<Real>& center,
    Matrix3<Real>& inertia)
{
    const Real oneDiv6 = (Real)(1.0/6.0);
    const Real oneDiv24 = (Real)(1.0/24.0);
    const Real oneDiv60 = (Real)(1.0/60.0);
    const Real oneDiv120 = (Real)(1.0/120.0);

    // order:  1, x, y, z, x^2, y^2, z^2, xy, yz, zx
    Real integral[10] = { (Real)0.0, (Real)0.0, (Real)0.0, (Real)0.0,
        (Real)0.0, (Real)0.0, (Real)0.0, (Real)0.0, (Real)0.0, (Real)0.0 };

    const int* index = indices;
    int i;
    for (i = 0; i < numTriangles; i++)
    {
        // Get vertices of triangle i.
        Vector3<Real> v0 = vertices[*index++];
        Vector3<Real> v1 = vertices[*index++];
        Vector3<Real> v2 = vertices[*index++];

        // Get cross product of edges and normal vector.
        Vector3<Real> V1mV0 = v1 - v0;
        Vector3<Real> V2mV0 = v2 - v0;
        Vector3<Real> N = V1mV0.Cross(V2mV0);

        // Compute integral terms.
        Real tmp0, tmp1, tmp2;
        Real f1x, f2x, f3x, g0x, g1x, g2x;
        tmp0 = v0.X() + v1.X();
        f1x = tmp0 + v2.X();
        tmp1 = v0.X()*v0.X();
        tmp2 = tmp1 + v1.X()*tmp0;
        f2x = tmp2 + v2.X()*f1x;
        f3x = v0.X()*tmp1 + v1.X()*tmp2 + v2.X()*f2x;
        g0x = f2x + v0.X()*(f1x + v0.X());
        g1x = f2x + v1.X()*(f1x + v1.X());
        g2x = f2x + v2.X()*(f1x + v2.X());

        Real f1y, f2y, f3y, g0y, g1y, g2y;
        tmp0 = v0.Y() + v1.Y();
        f1y = tmp0 + v2.Y();
        tmp1 = v0.Y()*v0.Y();
        tmp2 = tmp1 + v1.Y()*tmp0;
        f2y = tmp2 + v2.Y()*f1y;
        f3y = v0.Y()*tmp1 + v1.Y()*tmp2 + v2.Y()*f2y;
        g0y = f2y + v0.Y()*(f1y + v0.Y());
        g1y = f2y + v1.Y()*(f1y + v1.Y());
        g2y = f2y + v2.Y()*(f1y + v2.Y());

        Real f1z, f2z, f3z, g0z, g1z, g2z;
        tmp0 = v0.Z() + v1.Z();
        f1z = tmp0 + v2.Z();
        tmp1 = v0.Z()*v0.Z();
        tmp2 = tmp1 + v1.Z()*tmp0;
        f2z = tmp2 + v2.Z()*f1z;
        f3z = v0.Z()*tmp1 + v1.Z()*tmp2 + v2.Z()*f2z;
        g0z = f2z + v0.Z()*(f1z + v0.Z());
        g1z = f2z + v1.Z()*(f1z + v1.Z());
        g2z = f2z + v2.Z()*(f1z + v2.Z());

        // Update integrals.
        integral[0] += N.X()*f1x;
        integral[1] += N.X()*f2x;
        integral[2] += N.Y()*f2y;
        integral[3] += N.Z()*f2z;
        integral[4] += N.X()*f3x;
        integral[5] += N.Y()*f3y;
        integral[6] += N.Z()*f3z;
        integral[7] += N.X()*(v0.Y()*g0x + v1.Y()*g1x + v2.Y()*g2x);
        integral[8] += N.Y()*(v0.Z()*g0y + v1.Z()*g1y + v2.Z()*g2y);
        integral[9] += N.Z()*(v0.X()*g0z + v1.X()*g1z + v2.X()*g2z);
    }

    integral[0] *= oneDiv6;
    integral[1] *= oneDiv24;
    integral[2] *= oneDiv24;
    integral[3] *= oneDiv24;
    integral[4] *= oneDiv60;
    integral[5] *= oneDiv60;
    integral[6] *= oneDiv60;
    integral[7] *= oneDiv120;
    integral[8] *= oneDiv120;
    integral[9] *= oneDiv120;

    // mass
    mass = integral[0];

    // center of mass
    center = Vector3<Real>(integral[1], integral[2], integral[3])/mass;

    // inertia relative to world origin
    inertia[0][0] = integral[5] + integral[6];
    inertia[0][1] = -integral[7];
    inertia[0][2] = -integral[9];
    inertia[1][0] = inertia[0][1];
    inertia[1][1] = integral[4] + integral[6];
    inertia[1][2] = -integral[8];
    inertia[2][0] = inertia[0][2];
    inertia[2][1] = inertia[1][2];
    inertia[2][2] = integral[4] + integral[5];

    // inertia relative to center of mass
    if (bodyCoords)
    {
        inertia[0][0] -= mass*(center.Y()*center.Y() +
            center.Z()*center.Z());
        inertia[0][1] += mass*center.X()*center.Y();
        inertia[0][2] += mass*center.Z()*center.X();
        inertia[1][0] = inertia[0][1];
        inertia[1][1] -= mass*(center.Z()*center.Z() +
            center.X()*center.X());
        inertia[1][2] += mass*center.Y()*center.Z();
        inertia[2][0] = inertia[0][2];
        inertia[2][1] = inertia[1][2];
        inertia[2][2] -= mass*(center.X()*center.X() +
            center.Y()*center.Y());
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
void ComputeMassProperties<float> (const Vector3<float>*, int, const int*,
    bool, float&, Vector3<float>&, Matrix3<float>&);

template WM5_PHYSICS_ITEM
void ComputeMassProperties<double> (const Vector3<double>*, int, const int*,
    bool, double&, Vector3<double>&, Matrix3<double>&);
//----------------------------------------------------------------------------
}
