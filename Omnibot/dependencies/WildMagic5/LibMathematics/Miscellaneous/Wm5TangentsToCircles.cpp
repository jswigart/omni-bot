// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5TangentsToCircles.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
static void GetDirections (const Vector2<Real>& W, Real a,
    Vector2<Real>& dir0, Vector2<Real>& dir1)
{
    Real a2 = a*a;
    Real wx2 = W.X()*W.X();
    Real wy2 = W.Y()*W.Y();
    Real c2 = wx2 + wy2;
    Real minusHalfInvC2 = ((Real)-0.5)/c2;
    Real c0, c1, discr, root, inv;

    if (Math<Real>::FAbs(W.X()) >= Math<Real>::FAbs(W.Y()))
    {
        c0 = a2 - wx2;
        c1 = -((Real)2)*a*W.Y();
        discr = c1*c1 - ((Real)4.0)*c0*c2;
        root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
        inv = ((Real)1)/W.X();
        dir0.Y() = (c1 + root)*minusHalfInvC2;
        dir0.X() = (a - W.Y()*dir0.Y())*inv;
        dir1.Y() = (c1 - root)*minusHalfInvC2;
        dir1.X() = (a - W.Y()*dir1.Y())*inv;
    }
    else
    {
        c0 = a2 - wy2;
        c1 = -((Real)2)*a*W.X();
        discr = c1*c1 - ((Real)4.0)*c0*c2;
        root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
        inv = ((Real)1)/W.Y();
        dir0.X() = (c1 + root)*minusHalfInvC2;
        dir0.Y() = (a - W.X()*dir0.X())*inv;
        dir1.X() = (c1 - root)*minusHalfInvC2;
        dir1.Y() = (a - W.X()*dir1.X())*inv;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool GetTangentsToCircles (const Circle2<Real>& circle0,
    const Circle2<Real>& circle1, Line2<Real> line[4])
{
    Vector2<Real> W = circle1.Center - circle0.Center;
    Real wLenSqr = W.SquaredLength();
    Real rSum = circle0.Radius + circle1.Radius;
    if (wLenSqr <= rSum*rSum)
    {
        // Circles are either intersecting or nested.
        return false;
    }

    Real r0Sqr = circle0.Radius*circle0.Radius;
    Real tmp, a;

    Real rDiff = circle1.Radius - circle0.Radius;
    if (Math<Real>::FAbs(rDiff) >= Math<Real>::ZERO_TOLERANCE)
    {
        // Solve (R1^2-R0^2)*s^2 + 2*R0^2*s - R0^2 = 0..
        Real r1Sqr = circle1.Radius*circle1.Radius;
        Real c0 = -r0Sqr;
        Real c1 = ((Real)2)*r0Sqr;
        Real c2 = circle1.Radius*circle1.Radius - r0Sqr;
        Real minusHalfInvC2 = ((Real)-0.5)/c2;
        Real discr = Math<Real>::FAbs(c1*c1 - ((Real)4)*c0*c2);
        Real root = Math<Real>::Sqrt(discr);
        Real s, oneMinusS;

        // Get the first root.
        s = (c1 + root)*minusHalfInvC2;
        line[0].Origin = circle0.Center + s*W;
        line[1].Origin = line[0].Origin;
        if (s >= (Real)0.5)
        {
            tmp = Math<Real>::FAbs(wLenSqr - r0Sqr/(s*s));
            a = Math<Real>::Sqrt(tmp);
        }
        else
        {
            oneMinusS = (Real)1 - s;
            tmp = Math<Real>::FAbs(wLenSqr - r1Sqr/(oneMinusS*oneMinusS));
            a = Math<Real>::Sqrt(tmp);
        }
        GetDirections(W, a, line[0].Direction, line[1].Direction);

        // Get the second root.
        s = (c1 - root)*minusHalfInvC2;
        line[2].Origin = circle0.Center + s*W;
        line[3].Origin = line[2].Origin;
        if (s >= (Real)0.5)
        {
            tmp = Math<Real>::FAbs(wLenSqr - r0Sqr/(s*s));
            a = Math<Real>::Sqrt(tmp);
        }
        else
        {
            oneMinusS = (Real)1 - s;
            tmp = Math<Real>::FAbs(wLenSqr - r1Sqr/(oneMinusS*oneMinusS));
            a = Math<Real>::Sqrt(tmp);
        }
        GetDirections(W, a, line[2].Direction, line[3].Direction);
    }
    else
    {
        // Circles effectively have same radius.

        // Midpoint of circle centers.
        Vector2<Real> mid = ((Real)0.5)*(circle0.Center+circle1.Center);

        // Tangent lines passing through midpoint.
        tmp = Math<Real>::FAbs(wLenSqr - ((Real)4)*r0Sqr);
        a = Math<Real>::Sqrt(tmp);
        GetDirections(W, a, line[0].Direction, line[1].Direction);
        line[0].Origin = mid;
        line[1].Origin = mid;

        // Normalize W.
        W /= Math<Real>::Sqrt(wLenSqr);

        // Tangent lines parallel to normalized W:
        //   1.  D = W
        //   2.  a. P = mid+R0*perp(W), perp(a,b) = (b,-a)
        //       b. P = mid-R0*perp(W)
        line[2].Origin.X() = mid.X() + circle0.Radius*W.Y();
        line[2].Origin.Y() = mid.Y() - circle0.Radius*W.X();
        line[2].Direction = W;
        line[3].Origin.X() = mid.X() - circle0.Radius*W.Y();
        line[3].Origin.Y() = mid.Y() + circle0.Radius*W.X();
        line[3].Direction = W;
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
bool GetTangentsToCircles<float> (const Circle2<float>&,
    const Circle2<float>&, Line2<float>[4]);

template WM5_MATHEMATICS_ITEM
bool GetTangentsToCircles<double> (const Circle2<double>&,
    const Circle2<double>&, Line2<double>[4]);
//----------------------------------------------------------------------------
}
