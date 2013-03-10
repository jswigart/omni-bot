// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContEllipse2MinCR.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ContEllipse2MinCR<Real>::ContEllipse2MinCR (int numPoints,
    const Vector2<Real>* points, const Vector2<Real>& C,
    const Matrix2<Real>& R, Real D[2])
{
    // Compute the constraint coefficients, of the form (A[0],A[1]) for
    // each i.
    std::vector<Vector2<Real> > A(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
        Vector2<Real> diff = points[i] - C;  // P[i] - C
        Vector2<Real> prod = diff*R;  // R^T*(P[i] - C) = (u,v)
        A[i].X() = prod.X()*prod.X();  // u^2
        A[i].Y() = prod.Y()*prod.Y();  // v^2
    }

    // Sort to eliminate redundant constraints.
    typename std::vector<Vector2<Real> >::iterator end;

    // Lexicographical sort, (x0,y0) > (x1,y1) if x0 > x1 or if x0 = x1 and
    // y0 > y1.  Remove all but first entry in blocks with x0 = x1 since the
    // corresponding constraint lines for the first entry "hides" all the
    // others from the origin.
    std::sort(A.begin(), A.end(), XGreater);
    end = std::unique(A.begin(), A.end(), XEqual);
    A.erase(end, A.end());

    // Lexicographical sort, (x0,y0) > (x1,y1) if y0 > y1 or if y0 = y1 and
    // x0 > x1.  Remove all but first entry in blocks with y0 = y1 since the
    // corresponding constraint lines for the first entry "hides" all the
    // others from the origin.
    std::sort(A.begin(), A.end(), YGreater);
    end = std::unique(A.begin(), A.end(), YEqual);
    A.erase(end, A.end());

    MaxProduct(A, D);
}
//----------------------------------------------------------------------------
template <typename Real>
bool ContEllipse2MinCR<Real>::XGreater (const Vector2<Real>& P0,
    const Vector2<Real>& P1)
{
    if (P0.X() > P1.X())
    {
        return true;
    }

    if (P0.X() < P1.X())
    {
        return false;
    }

    return P0.Y() > P1.Y();
}
//----------------------------------------------------------------------------
template <typename Real>
bool ContEllipse2MinCR<Real>::XEqual (const Vector2<Real>& P0,
    const Vector2<Real>& P1)
{
    return P0.X() == P1.X();
}
//----------------------------------------------------------------------------
template <typename Real>
bool ContEllipse2MinCR<Real>::YGreater (const Vector2<Real>& P0,
    const Vector2<Real>& P1)
{
    if (P0.Y() > P1.Y())
    {
        return true;
    }

    if (P0.Y() < P1.Y())
    {
        return false;
    }

    return P0.X() > P1.X();
}
//----------------------------------------------------------------------------
template <typename Real>
bool ContEllipse2MinCR<Real>::YEqual (const Vector2<Real>& P0,
    const Vector2<Real>& P1)
{
    return P0.Y() == P1.Y();
}
//----------------------------------------------------------------------------
template <typename Real>
void ContEllipse2MinCR<Real>::MaxProduct (std::vector<Vector2<Real> >& A,
    Real D[2])
{
    // Keep track of which constraint lines have already been used in the
    // search.
    int numConstraints = (int)A.size();
    bool* used = new1<bool>(numConstraints);
    memset(used, 0, numConstraints*sizeof(bool));

    // Find the constraint line whose y-intercept (0,ymin) is closest to the
    // origin.  This line contributes to the convex hull of the constraints
    // and the search for the maximum starts here.  Also find the constraint
    // line whose x-intercept (xmin,0) is closest to the origin.  This line
    // contributes to the convex hull of the constraints and the search for
    // the maximum terminates before or at this line.
    int i, iYMin = -1;
    int iXMin = -1;
    Real axMax = (Real)0, ayMax = (Real)0;  // A[i] >= (0,0) by design
    for (i = 0; i < numConstraints; ++i)
    {
        // The minimum x-intercept is 1/A[iXMin].X() for A[iXMin].X() the
        // maximum of the A[i].X().
        if (A[i].X() > axMax)
        {
            axMax = A[i].X();
            iXMin = i;
        }

        // The minimum y-intercept is 1/A[iYMin].Y() for A[iYMin].Y() the
        // maximum of the A[i].Y().
        if (A[i].Y() > ayMax)
        {
            ayMax = A[i].Y();
            iYMin = i;
        }
    }
    assertion(iXMin != -1 && iYMin != -1, "Unexpected condition\n");
    WM5_UNUSED(iXMin);

    used[iYMin] = true;

    // The convex hull is searched in a clockwise manner starting with the
    // constraint line constructed above.  The next vertex of the hull occurs
    // as the closest point to the first vertex on the current constraint
    // line.  The following loop finds each consecutive vertex.
    Real x0 = (Real)0, xMax = ((Real)1)/axMax;
    int j;
    for (j = 0; j < numConstraints; ++j)
    {
        // Find the line whose intersection with the current line is closest
        // to the last hull vertex.  The last vertex is at (x0,y0) on the
        // current line.
        Real x1 = xMax;
        int line = -1;
        for (i = 0; i < numConstraints; ++i)
        {
            if (!used[i])
            {
                // This line not yet visited, process it.  Given current
                // constraint line a0*x+b0*y =1 and candidate line
                // a1*x+b1*y = 1, find the point of intersection.  The
                // determinant of the system is d = a0*b1-a1*b0.  We only
                // care about lines that have more negative slope than the
                // previous one, that is, -a1/b1 < -a0/b0, in which case we
                // process only lines for which d < 0.
                Real det = A[iYMin].DotPerp(A[i]);
                if (det < (Real)0)  // TO DO.  Need epsilon test here?
                {
                    // Compute the x-value for the point of intersection,
                    // (x1,y1).  There may be floating point error issues in
                    // the comparision 'D[0] <= fX1'.  Consider modifying to
                    // 'D[0] <= fX1+epsilon'.
                    D[0] = (A[i].Y() - A[iYMin].Y())/det;
                    if (x0 < D[0] && D[0] <= x1)
                    {
                        line = i;
                        x1 = D[0];
                    }
                }
            }
        }

        // Next vertex is at (x1,y1) whose x-value was computed above.  First
        // check for the maximum of x*y on the current line for x in [x0,x1].
        // On this interval the function is f(x) = x*(1-a0*x)/b0.  The
        // derivative is f'(x) = (1-2*a0*x)/b0 and f'(r) = 0 when
        // r = 1/(2*a0).  The three candidates for the maximum are f(x0),
        // f(r), and f(x1).  Comparisons are made between r and the end points
        // x0 and x1.  Since a0 = 0 is possible (constraint line is horizontal
        // and f is increasing on line), the division in r is not performed
        // and the comparisons are made between 1/2 = a0*r and a0*x0 or a0*x1.

        // Compare r < x0.
        if ((Real)0.5 < A[iYMin].X()*x0)
        {
            // The maximum is f(x0) since the quadratic f decreases for
            // x > r.
            D[0] = x0;
            D[1] = ((Real)1 - A[iYMin].X()*D[0])/A[iYMin].Y();  // = f(x0)
            break;
        }

        // Compare r < x1.
        if ((Real)0.5 < A[iYMin].X()*x1)
        {
            // The maximum is f(r).  The search ends here because the
            // current line is tangent to the level curve of f(x)=f(r)
            // and x*y can therefore only decrease as we traverse further
            // around the hull in the clockwise direction.
            D[0] = ((Real)0.5)/A[iYMin].X();
            D[1] = ((Real)0.5)/A[iYMin].Y();  // = f(r)
            break;
        }

        // The maximum is f(x1).  The function x*y is potentially larger
        // on the next line, so continue the search.
        assertion(line != -1, "Unexpected condition\n");
        x0 = x1;
        x1 = xMax;
        used[line] = true;
        iYMin = line;
    }

    assertion(j < numConstraints, "Unexpected condition\n");

    delete1(used);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ContEllipse2MinCR<float>;

template WM5_MATHEMATICS_ITEM
class ContEllipse2MinCR<double>;
//----------------------------------------------------------------------------
}
