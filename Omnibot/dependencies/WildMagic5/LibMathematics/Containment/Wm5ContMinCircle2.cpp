// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/16)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContMinCircle2.h"
#include "Wm5Memory.h"

// All internal minimal circle calculations store the squared radius in the
// radius member of Circle2.  Only at the end is a sqrt computed.

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
MinCircle2<Real>::MinCircle2 (int numPoints, const Vector2<Real>* points,
    Circle2<Real>& minimal, Real epsilon)
    :
    mEpsilon(epsilon)
{
    mUpdate[0] = 0;
    mUpdate[1] = &MinCircle2<Real>::UpdateSupport1;
    mUpdate[2] = &MinCircle2<Real>::UpdateSupport2;
    mUpdate[3] = &MinCircle2<Real>::UpdateSupport3;

    Support support;
    Real distDiff;

    if (numPoints >= 1)
    {
        // Create identity permutation (0,1,...,numPoints-1).
        Vector2<Real>** permuted = new1<Vector2<Real>*>(numPoints);
        int i;
        for (i = 0; i < numPoints; ++i)
        {
            permuted[i] = (Vector2<Real>*)&points[i];
        }
        
        // Generate random permutation.
        for (i = numPoints - 1; i > 0; --i)
        {
            int j = rand() % (i+1);
            if (j != i)
            {
                Vector2<Real>* save = permuted[i];
                permuted[i] = permuted[j];
                permuted[j] = save;
            }
        }
        
        minimal = ExactCircle1(*permuted[0]);
        support.Quantity = 1;
        support.Index[0] = 0;
        i = 1;
        while (i < numPoints)
        {
            if (!support.Contains(i, permuted, mEpsilon))
            {
                if (!Contains(*permuted[i], minimal, distDiff))
                {
                    UpdateFunction update = mUpdate[support.Quantity];
                    Circle2<Real> circle = (this->*update)(i, permuted,
                        support);
                    if (circle.Radius > minimal.Radius)
                    {
                        minimal = circle;
                        i = 0;
                        continue;
                    }
                }
            }
            ++i;
        }
        
        delete1(permuted);
    }
    else
    {
        assertion(false, "Input must contain points\n");
    }

    minimal.Radius = Math<Real>::Sqrt(minimal.Radius);
}
//----------------------------------------------------------------------------
template <typename Real>
bool MinCircle2<Real>::Contains (const Vector2<Real>& point,
    const Circle2<Real>& circle, Real& distDiff)
{
    Vector2<Real> diff = point - circle.Center;
    Real test = diff.SquaredLength();

    // NOTE:  In this algorithm, Circle2 is storing the *squared radius*,
    // so the next line of code is not in error.
    distDiff = test - circle.Radius;

    return distDiff <= (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real> MinCircle2<Real>::ExactCircle1 (const Vector2<Real>& P)
{
    Circle2<Real> minimal;
    minimal.Center = P;
    minimal.Radius = (Real)0;
    return minimal;
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real> MinCircle2<Real>::ExactCircle2 (const Vector2<Real>& P0,
    const Vector2<Real>& P1)
{
    Vector2<Real> diff = P1 - P0;
    Circle2<Real> minimal;
    minimal.Center = ((Real)0.5)*(P0 + P1);
    minimal.Radius = ((Real)0.25)*diff.SquaredLength();
    return minimal;
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real> MinCircle2<Real>::ExactCircle3 (const Vector2<Real>& P0,
    const Vector2<Real>& P1, const Vector2<Real>& P2)
{
    Vector2<Real> E10 = P1 - P0;
    Vector2<Real> E20 = P2 - P0;

    Real A[2][2] =
    {
        { E10.X(), E10.Y() },
        { E20.X(), E20.Y() }
    };

    Real B[2] =
    {
        ((Real)0.5)*E10.SquaredLength(),
        ((Real)0.5)*E20.SquaredLength()
    };

    Circle2<Real> minimal;
    Real det = A[0][0]*A[1][1] - A[0][1]*A[1][0];

    if (Math<Real>::FAbs(det) > mEpsilon)
    {
        Real invDet = ((Real)1)/det;
        Vector2<Real> Q;
        Q.X() = (A[1][1]*B[0] - A[0][1]*B[1])*invDet;
        Q.Y() = (A[0][0]*B[1] - A[1][0]*B[0])*invDet;
        minimal.Center = P0 + Q;
        minimal.Radius = Q.SquaredLength();
    }
    else
    {
        minimal.Center = Vector2<Real>::ZERO;
        minimal.Radius = Math<Real>::MAX_REAL;
    }

    return minimal;
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real> MinCircle2<Real>::UpdateSupport1 (int i,
    Vector2<Real>** permuted, Support& support)
{
    const Vector2<Real>& P0 = *permuted[support.Index[0]];
    const Vector2<Real>& P1 = *permuted[i];

    Circle2<Real> minimal = ExactCircle2(P0, P1);
    support.Quantity = 2;
    support.Index[1] = i;

    return minimal;
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real> MinCircle2<Real>::UpdateSupport2 (int i,
    Vector2<Real>** permuted, Support& support)
{
    const Vector2<Real>* point[2] =
    {
        permuted[support.Index[0]],  // P0
        permuted[support.Index[1]]   // P1
    };
    const Vector2<Real>& P2 = *permuted[i];

    // Permutations of type 2, used for calling ExactCircle2(...).
    const int numType2 = 2;
    const int type2[numType2][2] =
    {
        {0, /*2*/ 1},
        {1, /*2*/ 0}
    };

    // Permutations of type 3, used for calling ExactCircle3(...).
    const int numType3 = 1;  // {0, 1, 2}

    Circle2<Real> circle[numType2 + numType3];
    int indexCircle = 0;
    Real minRSqr = Math<Real>::MAX_REAL;
    int indexMinRSqr = -1;
    Real distDiff, minDistDiff = Math<Real>::MAX_REAL;
    int indexMinDistDiff = -1;

    // Permutations of type 2.
    int j;
    for (j = 0; j < numType2; ++j, ++indexCircle)
    {
        circle[indexCircle] = ExactCircle2(*point[type2[j][0]], P2);
        if (circle[indexCircle].Radius < minRSqr)
        {
            if (Contains(*point[type2[j][1]], circle[indexCircle], distDiff))
            {
                minRSqr = circle[indexCircle].Radius;
                indexMinRSqr = indexCircle;
            }
            else if (distDiff < minDistDiff)
            {
                minDistDiff = distDiff;
                indexMinDistDiff = indexCircle;
            }
        }
    }

    // Permutations of type 3.
    circle[indexCircle] = ExactCircle3(*point[0], *point[1], P2);
    if (circle[indexCircle].Radius < minRSqr)
    {
        minRSqr = circle[indexCircle].Radius;
        indexMinRSqr = indexCircle;
    }

    // Theoreticaly, indexMinRSqr >= 0, but floating-point round-off errors
    // can lead to indexMinRSqr == -1.  When this happens, the minimal sphere
    // is chosen to be the one that has the minimum absolute errors between
    // the sphere and points (barely) outside the sphere.
    if (indexMinRSqr == -1)
    {
        indexMinRSqr = indexMinDistDiff;
    }

    Circle2<Real> minimal = circle[indexMinRSqr];
    switch (indexMinRSqr)
    {
    case 0:
        support.Index[1] = i;
        break;
    case 1:
        support.Index[0] = i;
        break;
    case 2:
        support.Quantity = 3;
        support.Index[2] = i;
        break;
    }

    return minimal;
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real> MinCircle2<Real>::UpdateSupport3 (int i,
    Vector2<Real>** permuted, Support& support)
{
    const Vector2<Real>* point[3] =
    {
        permuted[support.Index[0]],  // P0
        permuted[support.Index[1]],  // P1
        permuted[support.Index[2]]   // P2
    };
    const Vector2<Real>& P3 = *permuted[i];

    // Permutations of type 2, used for calling ExactCircle2(...).
    const int numType2 = 3;
    const int type2[numType2][3] =
    {
        {0, /*3*/ 1, 2},
        {1, /*3*/ 0, 2},
        {2, /*3*/ 0, 1}
    };

    // Permutations of type 2, used for calling ExactCircle3(...).
    const int numType3 = 3;
    const int type3[numType3][3] =
    {
        {0, 1, /*3*/ 2},
        {0, 2, /*3*/ 1},
        {1, 2, /*3*/ 0}
    };

    Circle2<Real> circle[numType2 + numType3];
    int indexCircle = 0;
    Real minRSqr = Math<Real>::MAX_REAL;
    int indexMinRSqr = -1;
    Real distDiff, minDistDiff = Math<Real>::MAX_REAL;
    int indexMinDistDiff = -1;

    // Permutations of type 2.
    int j;
    for (j = 0; j < numType2; ++j, ++indexCircle)
    {
        circle[indexCircle] = ExactCircle2(*point[type2[j][0]], P3);
        if (circle[indexCircle].Radius < minRSqr)
        {
            if (Contains(*point[type2[j][1]], circle[indexCircle], distDiff)
            &&  Contains(*point[type2[j][2]], circle[indexCircle], distDiff))
            {
                minRSqr = circle[indexCircle].Radius;
                indexMinRSqr = indexCircle;
            }
            else if (distDiff < minDistDiff)
            {
                minDistDiff = distDiff;
                indexMinDistDiff = indexCircle;
            }
        }
    }

    // Permutations of type 3.
    for (j = 0; j < numType3; ++j, ++indexCircle)
    {
        circle[indexCircle] = ExactCircle3(*point[type3[j][0]],
            *point[type3[j][1]], P3);
        if (circle[indexCircle].Radius < minRSqr)
        {
            if (Contains(*point[type3[j][2]], circle[indexCircle], distDiff))
            {
                minRSqr = circle[indexCircle].Radius;
                indexMinRSqr = indexCircle;
            }
            else if (distDiff < minDistDiff)
            {
                minDistDiff = distDiff;
                indexMinDistDiff = indexCircle;
            }
        }
    }

    // Theoreticaly, indexMinRSqr >= 0, but floating-point round-off errors
    // can lead to indexMinRSqr == -1.  When this happens, the minimal circle
    // is chosen to be the one that has the minimum absolute errors between
    // the circle and points (barely) outside the circle.
    if (indexMinRSqr == -1)
    {
        indexMinRSqr = indexMinDistDiff;
    }

    Circle2<Real> minimal = circle[indexMinRSqr];
    switch (indexMinRSqr)
    {
    case 0:
        support.Quantity = 2;
        support.Index[1] = i;
        break;
    case 1:
        support.Quantity = 2;
        support.Index[0] = i;
        break;
    case 2:
        support.Quantity = 2;
        support.Index[0] = support.Index[2];
        support.Index[1] = i;
        break;
    case 3:
        support.Index[2] = i;
        break;
    case 4:
        support.Index[1] = i;
        break;
    case 5:
        support.Index[0] = i;
        break;
    }

    return minimal;
}
//----------------------------------------------------------------------------
template <typename Real>
bool MinCircle2<Real>::Support::Contains (int index, Vector2<Real>** points,
    Real epsilon)
{
    for (int i = 0; i < Quantity; ++i)
    {
        Vector2<Real> diff = *points[index] - *points[Index[i]];
        if (diff.SquaredLength() < epsilon)
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class MinCircle2<float>;

template WM5_MATHEMATICS_ITEM
class MinCircle2<double>;
//----------------------------------------------------------------------------
}
