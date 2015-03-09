// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContScribeCircle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
bool Circumscribe (const Vector2<Real>& v0, const Vector2<Real>& v1,
    const Vector2<Real>& v2, Circle2<Real>& circle)
{
    Vector2<Real> e10 = v1 - v0;
    Vector2<Real> e20 = v2 - v0;

    Real A[2][2] =
    {
        {e10[0], e10[1]},
        {e20[0], e20[1]}
    };

    Real B[2] =
    {
        ((Real)0.5)*e10.SquaredLength(),
        ((Real)0.5)*e20.SquaredLength()
    };

    Vector2<Real> solution;
    if (LinearSystem<Real>().Solve2(A, B, (Real*)&solution))
    {
        circle.Center = v0 + solution;
        circle.Radius = solution.Length();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Inscribe (const Vector2<Real>& v0, const Vector2<Real>& v1,
    const Vector2<Real>& v2, Circle2<Real>& circle)
{
    Vector2<Real> d10 = v1 - v0;
    Vector2<Real> d20 = v2 - v0;
    Vector2<Real> d21 = v2 - v1;
    Real len10 = d10.Length();
    Real len20 = d20.Length();
    Real len21 = d21.Length();
    Real perimeter = len10 + len20 + len21;
    if (perimeter > Math<Real>::ZERO_TOLERANCE)
    {
        Real inv = ((Real)1)/perimeter;
        len10 *= inv;
        len20 *= inv;
        len21 *= inv;
        circle.Center = len21*v0 + len20*v1 + len10*v2;
        circle.Radius = inv*Math<Real>::FAbs(d10.DotPerp(d20));
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
bool Circumscribe<float> (const Vector2<float>&, const Vector2<float>&,
    const Vector2<float>&, Circle2<float>&);

template WM5_MATHEMATICS_ITEM
bool Inscribe<float> (const Vector2<float>&, const Vector2<float>&,
    const Vector2<float>&, Circle2<float>&);

template WM5_MATHEMATICS_ITEM
bool Circumscribe<double> (const Vector2<double>&, const Vector2<double>&,
    const Vector2<double>&, Circle2<double>&);

template WM5_MATHEMATICS_ITEM
bool Inscribe<double> (const Vector2<double>&, const Vector2<double>&,
    const Vector2<double>&, Circle2<double>&);
//----------------------------------------------------------------------------
}
