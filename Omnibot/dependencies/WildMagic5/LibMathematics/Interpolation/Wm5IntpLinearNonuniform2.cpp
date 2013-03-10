// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpLinearNonuniform2.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpLinearNonuniform2<Real>::IntpLinearNonuniform2 (
    const Delaunay2<Real>& DT, Real* F, bool owner)
    :
    mDT(&DT),
    mF(F),
    mOwner(owner)
{
}
//----------------------------------------------------------------------------
template <typename Real>
IntpLinearNonuniform2<Real>::~IntpLinearNonuniform2 ()
{
    if (mOwner)
    {
        delete1(mF);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpLinearNonuniform2<Real>::Evaluate (const Vector2<Real>& P, Real& F)
{
    int i = mDT->GetContainingTriangle(P);
    if (i == -1)
    {
        return false;
    }

    // Get the barycentric coordinates of P with respect to the triangle,
    // P = b0*V0 + b1*V1 + b2*V2, where b0+b1+b2 = 1.
    Real bary[3];
    bool valid = mDT->GetBarycentricSet(i, P, bary);
    assertion(valid, "Unexpected condition\n");
    if (!valid)
    {
        return false;
    }

    // Get the vertex indices for look up into the function-value array.
    int index[3];
    valid = mDT->GetIndexSet(i, index);
    assertion(valid, "Unexpected condition\n");
    if (!valid)
    {
        return false;
    }

    // Result is a barycentric combination of function values.
    F = bary[0]*mF[index[0]] + bary[1]*mF[index[1]] + bary[2]*mF[index[2]];
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpLinearNonuniform2<float>;

template WM5_MATHEMATICS_ITEM
class IntpLinearNonuniform2<double>;
//----------------------------------------------------------------------------
}
