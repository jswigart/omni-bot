// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpSphere2.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpSphere2<Real>::IntpSphere2 (int quantity, Real* theta, Real* phi,
    Real* F, bool owner, Query::Type queryType)
{
    // Copy the input data.  The larger arrays are used to support wrap-around
    // in the Delaunay triangulation for the interpolator.  The Vector2<Real>
    // object V corresponds to (V.X(),V.Y()) = (theta,phi).
    int threeQuantity = 3*quantity;
    Vector2<Real>* wrapAngles = new1<Vector2<Real> >(threeQuantity);
    Real* wrapF = new1<Real>(threeQuantity);
    for (int i = 0; i < quantity; ++i)
    {
        wrapAngles[i].X() = theta[i];
        wrapAngles[i].Y() = phi[i];
        wrapF[i] = F[i];
    }

    if (owner)
    {
        delete1(theta);
        delete1(phi);
        delete1(F);
    }

    // Use periodicity to get wrap-around in the Delaunay triangulation.
    int i0 = 0, i1 = quantity, i2 = 2*quantity;
    for (/**/; i0 < quantity; ++i0, ++i1, ++i2)
    {
        wrapAngles[i1].X() = wrapAngles[i0].X() + Math<Real>::TWO_PI;
        wrapAngles[i2].X() = wrapAngles[i0].X() - Math<Real>::TWO_PI;
        wrapAngles[i1].Y() = wrapAngles[i0].Y();
        wrapAngles[i2].Y() = wrapAngles[i0].Y();
        wrapF[i1] = wrapF[i0];
        wrapF[i2] = wrapF[i0];
    }

    mDT = new0 Delaunay2<Real>(threeQuantity, wrapAngles, (Real)0.001, true,
        queryType);
    mInterp = new0 IntpQdrNonuniform2<Real>(*mDT, wrapF, true);
}
//----------------------------------------------------------------------------
template <typename Real>
IntpSphere2<Real>::~IntpSphere2 ()
{
    delete0(mDT);
    delete0(mInterp);
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpSphere2<Real>::GetSphericalCoords (Real x, Real y, Real z,
    Real& theta, Real& phi)
{
    // Assumes (x,y,z) is unit length.  Returns -PI <= theta <= PI and
    // 0 <= phiAngle <= PI.

    if (z < (Real)1)
    {
        if (z > -(Real)1)
        {
            theta = Math<Real>::ATan2(y, x);
            phi = Math<Real>::ACos(z);
        }
        else
        {
            theta = -Math<Real>::PI;
            phi = Math<Real>::PI;
        }
    }
    else
    {
        theta = -Math<Real>::PI;
        phi = (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpSphere2<Real>::Evaluate (Real theta, Real phi, Real& F)
{
    Vector2<Real> angles(theta, phi);
    Real thetaDeriv, phiDeriv;
    return mInterp->Evaluate(angles, F, thetaDeriv, phiDeriv);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpSphere2<float>;

template WM5_MATHEMATICS_ITEM
class IntpSphere2<double>;
//----------------------------------------------------------------------------
}
