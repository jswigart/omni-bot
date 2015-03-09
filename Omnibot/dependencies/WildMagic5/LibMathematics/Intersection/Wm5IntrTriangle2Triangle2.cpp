// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrTriangle2Triangle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrTriangle2Triangle2<Real>::IntrTriangle2Triangle2 (
    const Triangle2<Real>& triangle0, const Triangle2<Real>& triangle1)
    :
    mTriangle0(&triangle0),
    mTriangle1(&triangle1)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle2<Real>& IntrTriangle2Triangle2<Real>::GetTriangle0 () const
{
    return *mTriangle0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle2<Real>& IntrTriangle2Triangle2<Real>::GetTriangle1 () const
{
    return *mTriangle1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle2Triangle2<Real>::Test ()
{
    int i0, i1;
    Vector2<Real> dir;

    // Test edges of triangle0 for separation.
    for (i0 = 0, i1 = 2; i0 < 3; i1 = i0++)
    {
        // Test axis V0[i1] + t*perp(V0[i0]-V0[i1]), perp(x,y) = (y,-x).
        dir.X() = mTriangle0->V[i0].Y() - mTriangle0->V[i1].Y();
        dir.Y() = mTriangle0->V[i1].X() - mTriangle0->V[i0].X();
        if (WhichSide(mTriangle1->V, mTriangle0->V[i1], dir) > 0)
        {
            // Triangle1 is entirely on positive side of triangle0 edge.
            return false;
        }
    }

    // Test edges of triangle1 for separation.
    for (i0 = 0, i1 = 2; i0 < 3; i1 = i0++)
    {
        // Test axis V1[i1] + t*perp(V1[i0]-V1[i1]), perp(x,y) = (y,-x).
        dir.X() = mTriangle1->V[i0].Y() - mTriangle1->V[i1].Y();
        dir.Y() = mTriangle1->V[i1].X() - mTriangle1->V[i0].X();
        if (WhichSide(mTriangle0->V, mTriangle1->V[i1], dir) > 0)
        {
            // Triangle0 is entirely on positive side of triangle1 edge.
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle2Triangle2<Real>::Find ()
{
    // The potential intersection is initialized to triangle1.  The set of
    // vertices is refined based on clipping against each edge of triangle0.
    mQuantity = 3;
    for (int i = 0; i < 3; ++i)
    {
        mPoint[i] = mTriangle1->V[i];
    }

    for (int i1 = 2, i0 = 0; i0 < 3; i1 = i0++)
    {
        // Clip against edge <V0[i1],V0[i0]>.
        Vector2<Real> N(
            mTriangle0->V[i1].Y() - mTriangle0->V[i0].Y(),
            mTriangle0->V[i0].X() - mTriangle0->V[i1].X());
        Real c = N.Dot(mTriangle0->V[i1]);
        ClipConvexPolygonAgainstLine(N, c, mQuantity, mPoint);
        if (mQuantity == 0)
        {
            // Triangle completely clipped, no intersection occurs.
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle2Triangle2<Real>::Test (Real tmax,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    // Process as if V0-triangle is stationary and V1-triangle is moving.
    Vector2<Real> W = velocity1 - velocity0;
    int side = 0;  // 0 = NONE, -1 = LEFT, +1 = RIGHT
    Real tfirst = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    Configuration cfg0, cfg1, tcfg0, tcfg1;
    int i0, i1, i2;
    Vector2<Real> D;
    Real speed;

    // Process edges of V0-triangle.
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2++)
    {
        // Test axis V0[i1] + t*perp(V0[i2]-V0[i1]), perp(x,y) = (y,-x).
        D.X() = mTriangle0->V[i2].Y() - mTriangle0->V[i1].Y();
        D.Y() = mTriangle0->V[i1].X() - mTriangle0->V[i2].X();
        speed = D.Dot(W);

        ComputeTwo(cfg0, mTriangle0->V, D, i0, i1, i2);
        ComputeThree(cfg1, mTriangle1->V, D, mTriangle0->V[i1]);

        if (NoIntersect(cfg0, cfg1, tmax, speed, side, tcfg0, tcfg1,
            tfirst, tlast))
        {
            return false;
        }
    }

    // Process edges of V1-triangle.
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2++)
    {
        // Test axis V1[i1] + t*perp(V1[i2]-V1[i1]), perp(x,y) = (y,-x).
        D.X() = mTriangle1->V[i2].Y() - mTriangle1->V[i1].Y();
        D.Y() = mTriangle1->V[i1].X() - mTriangle1->V[i2].X();
        speed = D.Dot(W);

        ComputeTwo(cfg1, mTriangle1->V, D, i0, i1, i2);
        ComputeThree(cfg0, mTriangle0->V, D, mTriangle1->V[i1]);

        if (NoIntersect(cfg0, cfg1, tmax, speed, side, tcfg0, tcfg1,
            tfirst, tlast))
        {
            return false;
        }
    }

    mContactTime = tfirst;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle2Triangle2<Real>::Find (Real tmax,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    // Process as if V0-triangle is stationary and V1-triangle is moving.
    Vector2<Real> W = velocity1 - velocity0;
    int side = 0;  // 0 = NONE, -1 = LEFT, +1 = RIGHT
    Real tfirst = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    Configuration cfg0, cfg1, tcfg0, tcfg1;
    int i0, i1, i2;
    Vector2<Real> D;
    Real speed;

    // Process edges of V0-triangle.
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2++)
    {
        // Test axis V0[i1] + t*perp(V0[i2]-V0[i1]), perp(x,y) = (y,-x).
        D.X() = mTriangle0->V[i2].Y() - mTriangle0->V[i1].Y();
        D.Y() = mTriangle0->V[i1].X() - mTriangle0->V[i2].X();
        speed = D.Dot(W);

        ComputeTwo(cfg0, mTriangle0->V, D, i0, i1, i2);
        ComputeThree(cfg1, mTriangle1->V, D, mTriangle0->V[i1]);

        if (NoIntersect(cfg0, cfg1, tmax, speed, side, tcfg0, tcfg1,
            tfirst, tlast))
        {
            return false;
        }
    }

    // Process edges of V1-triangle.
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2++)
    {
        // Test axis V1[i1] + t*perp(V1[i2]-V1[i1]), perp(x,y) = (y,-x).
        D.X() = mTriangle1->V[i2].Y() - mTriangle1->V[i1].Y();
        D.Y() = mTriangle1->V[i1].X() - mTriangle1->V[i2].X();
        speed = D.Dot(W);

        ComputeTwo(cfg1, mTriangle1->V, D, i0, i1, i2);
        ComputeThree(cfg0, mTriangle0->V, D, mTriangle1->V[i1]);

        if (NoIntersect(cfg0, cfg1, tmax, speed, side, tcfg0, tcfg1,
            tfirst, tlast))
        {
            return false;
        }
    }

    // Move triangles to first contact.
    Vector2<Real> moveV0[3], moveV1[3];
    for (int i = 0; i < 3; ++i)
    {
        moveV0[i] = mTriangle0->V[i] + tfirst*velocity0;
        moveV1[i] = mTriangle1->V[i] + tfirst*velocity1;
    };

    GetIntersection(tcfg0, tcfg1, side, moveV0, moveV1, mQuantity, mPoint);

    mContactTime = tfirst;
    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrTriangle2Triangle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrTriangle2Triangle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrTriangle2Triangle2<Real>::WhichSide (const Vector2<Real> V[3],
    const Vector2<Real>& P, const Vector2<Real>& D)
{
    // Vertices are projected to the form P+t*D.  Return value is +1 if all
    // t > 0, -1 if all t < 0, 0 otherwise, in which case the line splits the
    // triangle.

    int positive = 0, negative = 0, zero = 0;
    for (int i = 0; i < 3; ++i)
    {
        Real t = D.Dot(V[i] - P);
        if (t > (Real)0)
        {
            ++positive;
        }
        else if (t < (Real)0)
        {
            ++negative;
        }
        else
        {
            ++zero;
        }

        if (positive > 0 && negative > 0)
        {
            return 0;
        }
    }
    return (zero == 0 ? (positive > 0 ? 1 : -1) : 0);
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle2Triangle2<Real>::ClipConvexPolygonAgainstLine (
    const Vector2<Real>& N, Real c, int& quantity, Vector2<Real> V[6])
{
    // The input vertices are assumed to be in counterclockwise order.  The
    // ordering is an invariant of this function.

    // Test on which side of line the vertices are.
    int positive = 0, negative = 0, pIndex = -1;
    Real test[6];
    int i;
    for (i = 0; i < quantity; ++i)
    {
        test[i] = N.Dot(V[i]) - c;
        if (test[i] > (Real)0)
        {
            positive++;
            if (pIndex < 0)
            {
                pIndex = i;
            }
        }
        else if (test[i] < (Real)0)
        {
            negative++;
        }
    }

    if (positive > 0)
    {
        if (negative > 0)
        {
            // Line transversely intersects polygon.
            Vector2<Real> CV[6];
            int cQuantity = 0, cur, prv;
            Real t;

            if (pIndex > 0)
            {
                // First clip vertex on line.
                cur = pIndex;
                prv = cur - 1;
                t = test[cur]/(test[cur] - test[prv]);
                CV[cQuantity++] = V[cur] + t*(V[prv] - V[cur]);

                // Vertices on positive side of line.
                while (cur < quantity && test[cur] > (Real)0)
                {
                    CV[cQuantity++] = V[cur++];
                }

                // Last clip vertex on line.
                if (cur < quantity)
                {
                    prv = cur - 1;
                }
                else
                {
                    cur = 0;
                    prv = quantity - 1;
                }
                t = test[cur]/(test[cur] - test[prv]);
                CV[cQuantity++] = V[cur] + t*(V[prv]-V[cur]);
            }
            else  // pIndex is 0
            {
                // Vertices on positive side of line.
                cur = 0;
                while (cur < quantity && test[cur] > (Real)0)
                {
                    CV[cQuantity++] = V[cur++];
                }

                // Last clip vertex on line.
                prv = cur - 1;
                t = test[cur]/(test[cur] - test[prv]);
                CV[cQuantity++] = V[cur] + t*(V[prv] - V[cur]);

                // Skip vertices on negative side.
                while (cur < quantity && test[cur] <= (Real)0)
                {
                    ++cur;
                }

                // First clip vertex on line.
                if (cur < quantity)
                {
                    prv = cur - 1;
                    t = test[cur]/(test[cur] - test[prv]);
                    CV[cQuantity++] = V[cur] + t*(V[prv] - V[cur]);

                    // Vertices on positive side of line.
                    while (cur < quantity && test[cur] > (Real)0)
                    {
                        CV[cQuantity++] = V[cur++];
                    }
                }
                else
                {
                    // cur = 0
                    prv = quantity - 1;
                    t = test[0]/(test[0] - test[prv]);
                    CV[cQuantity++] = V[0] + t*(V[prv] - V[0]);
                }
            }

            quantity = cQuantity;
            memcpy(V, CV, cQuantity*sizeof(Vector2<Real>));
        }
        // else polygon fully on positive side of line, nothing to do.
    }
    else
    {
        // Polygon does not intersect positive side of line, clip all.
        quantity = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle2Triangle2<Real>::ComputeTwo (Configuration& cfg,
    const Vector2<Real> V[3], const Vector2<Real>& D, int i0, int i1,
    int i2)
{
    cfg.Map = M12;
    cfg.Index[0] = i0;
    cfg.Index[1] = i1;
    cfg.Index[2] = i2;
    cfg.Min = D.Dot(V[i0] - V[i1]);
    cfg.Max = (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle2Triangle2<Real>::ComputeThree (Configuration& cfg,
    const Vector2<Real> V[3], const Vector2<Real>& D,
    const Vector2<Real>& P)
{
    Real d0 = D.Dot(V[0] - P);
    Real d1 = D.Dot(V[1] - P);
    Real d2 = D.Dot(V[2] - P);

    // Make sure that m_aiIndex[...] is an even permutation of (0,1,2)
    // whenever the map value is M12 or M21.  This is needed to guarantee
    // the intersection of overlapping edges is properly computed.

    if (d0 <= d1)
    {
        if (d1 <= d2)  // d0 <= d1 <= d2
        {
            if (d0 != d1)
            {
                cfg.Map = (d1 != d2 ? M11 : M12);
            }
            else
            {
                cfg.Map = M21;
            }

            cfg.Index[0] = 0;
            cfg.Index[1] = 1;
            cfg.Index[2] = 2;
            cfg.Min = d0;
            cfg.Max = d2;
        }
        else if (d0 <= d2)  // d0 <= d2 < d1
        {
            if (d0 != d2)
            {
                cfg.Map = M11;
                cfg.Index[0] = 0;
                cfg.Index[1] = 2;
                cfg.Index[2] = 1;
            }
            else
            {
                cfg.Map = M21;
                cfg.Index[0] = 2;
                cfg.Index[1] = 0;
                cfg.Index[2] = 1;
            }

            cfg.Min = d0;
            cfg.Max = d1;
        }
        else  // d2 < d0 <= d1
        {
            cfg.Map = (d0 != d1 ? M12 : M11);
            cfg.Index[0] = 2;
            cfg.Index[1] = 0;
            cfg.Index[2] = 1;
            cfg.Min = d2;
            cfg.Max = d1;
        }
    }
    else
    {
        if (d2 <= d1)  // d2 <= d1 < d0
        {
            if (d2 != d1)
            {
                cfg.Map = M11;
                cfg.Index[0] = 2;
                cfg.Index[1] = 1;
                cfg.Index[2] = 0;
            }
            else
            {
                cfg.Map = M21;
                cfg.Index[0] = 1;
                cfg.Index[1] = 2;
                cfg.Index[2] = 0;
            }

            cfg.Min = d2;
            cfg.Max = d0;
        }
        else if (d2 <= d0)  // d1 < d2 <= d0
        {
            cfg.Map = (d2 != d0 ? M11 : M12);
            cfg.Index[0] = 1;
            cfg.Index[1] = 2;
            cfg.Index[2] = 0;
            cfg.Min = d1;
            cfg.Max = d0;
        }
        else  // d1 < d0 < d2
        {
            cfg.Map = M11;
            cfg.Index[0] = 1;
            cfg.Index[1] = 0;
            cfg.Index[2] = 2;
            cfg.Min = d1;
            cfg.Max = d2;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle2Triangle2<Real>::NoIntersect (
    const Configuration& cfg0, const Configuration& cfg1, Real tmax,
    Real speed, int& side, Configuration& tcfg0, Configuration& tcfg1,
    Real& tfirst, Real& tlast)
{
    Real invSpeed, t;

    if (cfg1.Max < cfg0.Min)
    {
        // V1-interval initially on left of V0-interval.
        if (speed <= (Real)0)
        {
            // Intervals moving apart.
            return true;
        }

        // Update first time.
        invSpeed = ((Real)1)/speed;
        t = (cfg0.Min - cfg1.Max)*invSpeed;
        if (t > tfirst)
        {
            tfirst = t;
            side = -1;
            tcfg0 = cfg0;
            tcfg1 = cfg1;
        }

        // Test for exceedance of time interval.
        if (tfirst > tmax)
        {
            return true;
        }

        // Update last time.
        t = (cfg0.Max - cfg1.Min)*invSpeed;
        if (t < tlast)
        {
            tlast = t;
        }

        // Test for separation.
        if (tfirst > tlast)
        {
            return true;
        }
    }
    else if (cfg0.Max < cfg1.Min)
    {
        // V1-interval initially on right of V0-interval.
        if (speed >= (Real)0)
        {
            // Intervals moving apart.
            return true;
        }

        // Update first time.
        invSpeed = ((Real)1)/speed;
        t = (cfg0.Max - cfg1.Min)*invSpeed;
        if (t > tfirst)
        {
            tfirst = t;
            side = 1;
            tcfg0 = cfg0;
            tcfg1 = cfg1;
        }

        // Test for exceedance of time interval.
        if (tfirst > tmax)
        {
            return true;
        }

        // Update last time.
        t = (cfg0.Min - cfg1.Max)*invSpeed;
        if (t < tlast)
        {
            tlast = t;
        }

        // Test for separation.
        if (tfirst > tlast)
        {
            return true;
        }
    }
    else
    {
        // V0-interval and V1-interval initially overlap.
        if (speed > (Real)0)
        {
            // Update last time.
            invSpeed = ((Real)1)/speed;
            t = (cfg0.Max - cfg1.Min)*invSpeed;
            if (t < tlast)
            {
                tlast = t;
            }

            // Test for separation.
            if (tfirst > tlast)
            {
                return true;
            }
        }
        else if (speed < (Real)0)
        {
            // Update last time.
            invSpeed = ((Real)1)/speed;
            t = (cfg0.Min - cfg1.Max)*invSpeed;
            if (t < tlast)
            {
                tlast = t;
            }

            // Test for separation.
            if (tfirst > tlast)
            {
                return true;
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTriangle2Triangle2<Real>::GetIntersection (
    const Configuration& cfg0, const Configuration& cfg1, int side,
    const Vector2<Real> V0[3], const Vector2<Real> V1[3], int& quantity,
    Vector2<Real> vertex[6])
{
    Vector2<Real> edge, diff;
    const Vector2<Real>* origin;
    Real invEdE, emin, emax;
    int i;

    if (side == 1)  // V1-interval contacts V0-interval on right.
    {
        if (cfg0.Map == M21 || cfg0.Map == M11)
        {
            quantity = 1;
            vertex[0] = V0[cfg0.Index[2]];
        }
        else if (cfg1.Map == M12 || cfg1.Map == M11)
        {
            quantity = 1;
            vertex[0] = V1[cfg1.Index[0]];
        }
        else  // cfg0.Map == M12 && cfg1.Map == M21 (edge overlap).
        {
            origin = &V0[cfg0.Index[1]];
            edge = V0[cfg0.Index[2]] - *origin;
            invEdE = ((Real)1)/edge.Dot(edge);
            diff = V1[cfg1.Index[1]] - *origin;
            emin = edge.Dot(diff)*invEdE;
            diff = V1[cfg1.Index[0]] - *origin;
            emax = edge.Dot(diff)*invEdE;
            assertion(emin <= emax, "Unexpected condition\n");
            Intersector1<Real> intr((Real)0, (Real)1, emin, emax);
            quantity = intr.GetNumIntersections();
            assertion(quantity > 0, "Unexpected condition\n");
            for (i = 0; i < quantity; ++i)
            {
                vertex[i] = *origin + intr.GetIntersection(i)*edge;
            }
        }
    }
    else if (side == -1)  // V1-interval contacts V0-interval on left.
    {
        if (cfg1.Map == M21 || cfg1.Map == M11)
        {
            quantity = 1;
            vertex[0] = V1[cfg1.Index[2]];
        }
        else if (cfg0.Map == M12 || cfg0.Map == M11)
        {
            quantity = 1;
            vertex[0] = V0[cfg0.Index[0]];
        }
        else  // cfg1.Map == M12 && cfg0.Map == M21 (edge overlap).
        {
            origin = &V1[cfg1.Index[1]];
            edge = V1[cfg1.Index[2]] - *origin;
            invEdE = ((Real)1)/edge.Dot(edge);
            diff = V0[cfg0.Index[1]] - *origin;
            emin = edge.Dot(diff)*invEdE;
            diff = V0[cfg0.Index[0]] - *origin;
            emax = edge.Dot(diff)*invEdE;
            assertion(emin <= emax, "Unexpected condition\n");
            Intersector1<Real> intr((Real)0, (Real)1, emin, emax);
            quantity = intr.GetNumIntersections();
            assertion(quantity > 0, "Unexpected condition\n");
            for (i = 0; i < quantity; ++i)
            {
                vertex[i] = *origin + intr.GetIntersection(i)*edge;
            }
        }
    }
    else  // Triangles were initially intersecting.
    {
        Triangle2<Real> tri0(V0), tri1(V1);
        IntrTriangle2Triangle2 intr(tri0, tri1);
        intr.Find();
        quantity = intr.GetQuantity();
        for (i = 0; i < quantity; ++i)
        {
            vertex[i] = intr.GetPoint(i);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrTriangle2Triangle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrTriangle2Triangle2<double>;
//----------------------------------------------------------------------------
}
