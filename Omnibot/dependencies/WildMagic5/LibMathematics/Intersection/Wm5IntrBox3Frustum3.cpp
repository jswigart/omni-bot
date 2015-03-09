// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrBox3Frustum3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrBox3Frustum3<Real>::IntrBox3Frustum3 (const Box3<Real>& box,
    const Frustum3<Real>& frustum)
    :
    mBox(&box),
    mFrustum(&frustum)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrBox3Frustum3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
const Frustum3<Real>& IntrBox3Frustum3<Real>::GetFrustum () const
{
    return *mFrustum;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox3Frustum3<Real>::Test ()
{
    // for convenience
    const Vector3<Real>* axes = mBox->Axis;
    const Real* extents = mBox->Extent;

    Vector3<Real> diff = mBox->Center - mFrustum->Origin;  // C-E

    Real A[3];      // Dot(R,A[i])
    Real B[3];      // Dot(U,A[i])
    Real C[3];      // Dot(D,A[i])
    Real D[3];      // (Dot(R,C-E),Dot(U,C-E),Dot(D,C-E))
    Real NA[3];     // dmin*Dot(R,A[i])
    Real NB[3];     // dmin*Dot(U,A[i])
    Real NC[3];     // dmin*Dot(D,A[i])
    Real ND[3];     // dmin*(Dot(R,C-E),Dot(U,C-E),?)
    Real RC[3];     // rmax*Dot(D,A[i])
    Real RD[3];     // rmax*(?,?,Dot(D,C-E))
    Real UC[3];     // umax*Dot(D,A[i])
    Real UD[3];     // umax*(?,?,Dot(D,C-E))
    Real NApRC[3];  // dmin*Dot(R,A[i]) + rmax*Dot(D,A[i])
    Real NAmRC[3];  // dmin*Dot(R,A[i]) - rmax*Dot(D,A[i])
    Real NBpUC[3];  // dmin*Dot(U,A[i]) + umax*Dot(D,A[i])
    Real NBmUC[3];  // dmin*Dot(U,A[i]) - umax*Dot(D,A[i])
    Real RBpUA[3];  // rmax*Dot(U,A[i]) + umax*Dot(R,A[i])
    Real RBmUA[3];  // rmax*Dot(U,A[i]) - umax*Dot(R,A[i])
    Real DdD, radius, p, fmin, fmax, MTwoUF, MTwoRF, tmp;
    int i, j;

    // M = D
    D[2] = diff.Dot(mFrustum->DVector);
    for (i = 0; i < 3; ++i)
    {
        C[i] = axes[i].Dot(mFrustum->DVector);
    }
    radius =
        extents[0]*Math<Real>::FAbs(C[0]) +
        extents[1]*Math<Real>::FAbs(C[1]) +
        extents[2]*Math<Real>::FAbs(C[2]);
    if (D[2] + radius < mFrustum->DMin
    ||  D[2] - radius > mFrustum->DMax)
    {
        return false;
    }

    // M = n*R - r*D
    for (i = 0; i < 3; ++i)
    {
        A[i] = axes[i].Dot(mFrustum->RVector);
        RC[i] = mFrustum->RBound*C[i];
        NA[i] = mFrustum->DMin*A[i];
        NAmRC[i] = NA[i] - RC[i];
    }
    D[0] = diff.Dot(mFrustum->RVector);
    radius =
        extents[0]*Math<Real>::FAbs(NAmRC[0]) +
        extents[1]*Math<Real>::FAbs(NAmRC[1]) +
        extents[2]*Math<Real>::FAbs(NAmRC[2]);
    ND[0] = mFrustum->DMin*D[0];
    RD[2] = mFrustum->RBound*D[2];
    DdD = ND[0] - RD[2];
    MTwoRF = mFrustum->GetMTwoRF();
    if (DdD + radius < MTwoRF || DdD > radius)
    {
        return false;
    }

    // M = -n*R - r*D
    for (i = 0; i < 3; ++i)
    {
        NApRC[i] = NA[i] + RC[i];
    }
    radius =
        extents[0]*Math<Real>::FAbs(NApRC[0]) +
        extents[1]*Math<Real>::FAbs(NApRC[1]) +
        extents[2]*Math<Real>::FAbs(NApRC[2]);
    DdD = -(ND[0] + RD[2]);
    if (DdD + radius < MTwoRF || DdD > radius)
    {
        return false;
    }

    // M = n*U - u*D
    for (i = 0; i < 3; ++i)
    {
        B[i] = axes[i].Dot(mFrustum->UVector);
        UC[i] = mFrustum->UBound*C[i];
        NB[i] = mFrustum->DMin*B[i];
        NBmUC[i] = NB[i] - UC[i];
    }
    D[1] = diff.Dot(mFrustum->UVector);
    radius =
        extents[0]*Math<Real>::FAbs(NBmUC[0]) +
        extents[1]*Math<Real>::FAbs(NBmUC[1]) +
        extents[2]*Math<Real>::FAbs(NBmUC[2]);
    ND[1] = mFrustum->DMin*D[1];
    UD[2] = mFrustum->UBound*D[2];
    DdD = ND[1] - UD[2];
    MTwoUF = mFrustum->GetMTwoUF();
    if (DdD + radius < MTwoUF || DdD > radius)
    {
        return false;
    }

    // M = -n*U - u*D
    for (i = 0; i < 3; ++i)
    {
        NBpUC[i] = NB[i] + UC[i];
    }
    radius =
        extents[0]*Math<Real>::FAbs(NBpUC[0]) +
        extents[1]*Math<Real>::FAbs(NBpUC[1]) +
        extents[2]*Math<Real>::FAbs(NBpUC[2]);
    DdD = -(ND[1] + UD[2]);
    if (DdD + radius < MTwoUF || DdD > radius)
    {
        return false;
    }

    // M = A[i]
    for (i = 0; i < 3; ++i)
    {
        p = mFrustum->RBound*Math<Real>::FAbs(A[i]) +
             mFrustum->UBound*Math<Real>::FAbs(B[i]);
        NC[i] = mFrustum->DMin*C[i];
        fmin = NC[i] - p;
        if (fmin < (Real)0)
        {
            fmin *= mFrustum->GetDRatio();
        }
        fmax = NC[i] + p;
        if (fmax > (Real)0)
        {
            fmax *= mFrustum->GetDRatio();
        }
        DdD = A[i]*D[0] + B[i]*D[1] + C[i]*D[2];
        if (DdD + extents[i] < fmin || DdD - extents[i] > fmax)
        {
            return false;
        }
    }

    // M = Cross(R,A[i])
    for (i = 0; i < 3; ++i)
    {
        p = mFrustum->UBound*Math<Real>::FAbs(C[i]);
        fmin = -NB[i] - p;
        if (fmin < (Real)0)
        {
            fmin *= mFrustum->GetDRatio();
        }
        fmax = -NB[i] + p;
        if (fmax > (Real)0)
        {
            fmax *= mFrustum->GetDRatio();
        }
        DdD = C[i]*D[1] - B[i]*D[2];
        radius =
            extents[0]*Math<Real>::FAbs(B[i]*C[0]-B[0]*C[i]) +
            extents[1]*Math<Real>::FAbs(B[i]*C[1]-B[1]*C[i]) +
            extents[2]*Math<Real>::FAbs(B[i]*C[2]-B[2]*C[i]);
        if (DdD + radius < fmin || DdD - radius > fmax)
        {
            return false;
        }
    }

    // M = Cross(U,A[i])
    for (i = 0; i < 3; ++i)
    {
        p = mFrustum->RBound*Math<Real>::FAbs(C[i]);
        fmin = NA[i] - p;
        if (fmin < (Real)0)
        {
            fmin *= mFrustum->GetDRatio();
        }
        fmax = NA[i] + p;
        if (fmax > (Real)0)
        {
            fmax *= mFrustum->GetDRatio();
        }
        DdD = -C[i]*D[0] + A[i]*D[2];
        radius =
            extents[0]*Math<Real>::FAbs(A[i]*C[0]-A[0]*C[i]) +
            extents[1]*Math<Real>::FAbs(A[i]*C[1]-A[1]*C[i]) +
            extents[2]*Math<Real>::FAbs(A[i]*C[2]-A[2]*C[i]);
        if (DdD + radius < fmin || DdD - radius > fmax)
        {
            return false;
        }
    }

    // M = Cross(n*D+r*R+u*U,A[i])
    for (i = 0; i < 3; ++i)
    {
        Real fRB = mFrustum->RBound*B[i];
        Real fUA = mFrustum->UBound*A[i];
        RBpUA[i] = fRB + fUA;
        RBmUA[i] = fRB - fUA;
    }
    for (i = 0; i < 3; ++i)
    {
        p = mFrustum->RBound*Math<Real>::FAbs(NBmUC[i]) +
             mFrustum->UBound*Math<Real>::FAbs(NAmRC[i]);
        tmp = -mFrustum->DMin*RBmUA[i];
        fmin = tmp - p;
        if (fmin < (Real)0)
        {
            fmin *= mFrustum->GetDRatio();
        }
        fmax = tmp + p;
        if (fmax > (Real)0)
        {
            fmax *= mFrustum->GetDRatio();
        }
        DdD = D[0]*NBmUC[i] - D[1]*NAmRC[i] - D[2]*RBmUA[i];
        radius = (Real)0;
        for (j = 0; j < 3; j++)
        {
            radius += extents[j]*Math<Real>::FAbs(A[j]*NBmUC[i] -
                B[j]*NAmRC[i] - C[j]*RBmUA[i]);
        }
        if (DdD + radius < fmin || DdD - radius > fmax)
        {
            return false;
        }
    }

    // M = Cross(n*D+r*R-u*U,A[i])
    for (i = 0; i < 3; ++i)
    {
        p = mFrustum->RBound*Math<Real>::FAbs(NBpUC[i]) +
             mFrustum->UBound*Math<Real>::FAbs(NAmRC[i]);
        tmp = -mFrustum->DMin*RBpUA[i];
        fmin = tmp - p;
        if (fmin < (Real)0)
        {
            fmin *= mFrustum->GetDRatio();
        }
        fmax = tmp + p;
        if (fmax > (Real)0)
        {
            fmax *= mFrustum->GetDRatio();
        }
        DdD = D[0]*NBpUC[i] - D[1]*NAmRC[i] - D[2]*RBpUA[i];
        radius = (Real)0;
        for (j = 0; j < 3; ++j)
        {
            radius += extents[j]*Math<Real>::FAbs(A[j]*NBpUC[i] -
                B[j]*NAmRC[i] - C[j]*RBpUA[i]);
        }
        if (DdD + radius < fmin || DdD - radius > fmax)
        {
            return false;
        }
    }

    // M = Cross(n*D-r*R+u*U,A[i])
    for (i = 0; i < 3; ++i)
    {
        p = mFrustum->RBound*Math<Real>::FAbs(NBmUC[i]) +
             mFrustum->UBound*Math<Real>::FAbs(NApRC[i]);
        tmp = mFrustum->DMin*RBpUA[i];
        fmin = tmp - p;
        if (fmin < (Real)0)
        {
            fmin *= mFrustum->GetDRatio();
        }
        fmax = tmp + p;
        if (fmax > (Real)0)
        {
            fmax *= mFrustum->GetDRatio();
        }
        DdD = D[0]*NBmUC[i] - D[1]*NApRC[i] + D[2]*RBpUA[i];
        radius = (Real)0;
        for (j = 0; j < 3; ++j)
        {
            radius += extents[j]*Math<Real>::FAbs(A[j]*NBmUC[i] -
                B[j]*NApRC[i] + C[j]*RBpUA[i]);
        }
        if (DdD + radius < fmin || DdD - radius > fmax)
        {
            return false;
        }
    }

    // M = Cross(n*D-r*R-u*U,A[i])
    for (i = 0; i < 3; ++i)
    {
        p = mFrustum->RBound*Math<Real>::FAbs(NBpUC[i]) +
             mFrustum->UBound*Math<Real>::FAbs(NApRC[i]);
        tmp = mFrustum->DMin*RBmUA[i];
        fmin = tmp - p;
        if (fmin < (Real)0)
        {
            fmin *= mFrustum->GetDRatio();
        }
        fmax = tmp + p;
        if (fmax > (Real)0)
        {
            fmax *= mFrustum->GetDRatio();
        }
        DdD = D[0]*NBpUC[i] - D[1]*NApRC[i] + D[2]*RBmUA[i];
        radius = (Real)0;
        for (j = 0; j < 3; ++j)
        {
            radius += extents[j]*Math<Real>::FAbs(A[j]*NBpUC[i] -
                B[j]*NApRC[i] + C[j]*RBmUA[i]);
        }
        if (DdD + radius < fmin || DdD - radius > fmax)
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrBox3Frustum3<float>;

template WM5_MATHEMATICS_ITEM
class IntrBox3Frustum3<double>;
//----------------------------------------------------------------------------
}
