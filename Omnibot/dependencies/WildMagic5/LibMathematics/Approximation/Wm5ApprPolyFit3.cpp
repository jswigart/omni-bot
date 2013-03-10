// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprPolyFit3.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Real* PolyFit3 (int numSamples, const Real* xSamples, const Real* ySamples,
    const Real* wSamples, int xDegree, int yDegree)
{
    int xBound = xDegree + 1;
    int yBound = yDegree + 1;
    int quantity = xBound*yBound;
    Real* coeff = new1<Real>(quantity);

    int i0, j0, i1, j1, s;

    // Powers of x, y.
    Real** xPower = new2<Real>(2*xDegree+1, numSamples);
    Real** yPower = new2<Real>(2*yDegree+1, numSamples);
    for (s = 0; s < numSamples; ++s)
    {
        xPower[s][0] = (Real)1;
        for (i0 = 1; i0 <= 2*xDegree; ++i0)
        {
            xPower[s][i0] = xSamples[s]*xPower[s][i0-1];
        }

        yPower[s][0] = (Real)1;
        for (j0 = 1; j0 <= 2*yDegree; ++j0)
        {
            yPower[s][j0] = ySamples[s]*yPower[s][j0-1];
        }
    }

    // Vandermonde matrix and right-hand side of linear system.
    GMatrix<Real> A(quantity, quantity);
    Real* B = new1<Real>(quantity);

    for (j0 = 0; j0 <= yDegree; ++j0)
    {
        for (i0 = 0; i0 <= xDegree; ++i0)
        {
            int index0 = i0 + xBound*j0;
            Real sum = (Real)0;
            for (s = 0; s < numSamples; ++s)
            {
                sum += wSamples[s]*xPower[s][i0]*yPower[s][j0];
            }

            B[index0] = sum;

            for (j1 = 0; j1 <= yDegree; ++j1)
            {
                for (i1 = 0; i1 <= xDegree; ++i1)
                {
                    int index1 = i1 + xBound*j1;
                    sum = (Real)0;
                    for (s = 0; s < numSamples; ++s)
                    {
                        sum += xPower[s][i0 + i1]*yPower[s][j0 + j1];
                    }

                    A(index0,index1) = sum;
                }
            }
        }
    }

    // Solve for the polynomial coefficients.
    bool hasSolution = LinearSystem<Real>().Solve(A, B, coeff);
    assertion(hasSolution, "Failed to solve linear system\n");
    WM5_UNUSED(hasSolution);

    delete1(B);
    delete2(xPower);
    delete2(yPower);

    return coeff;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
float* PolyFit3<float> (int, const float*, const float*, const float*,
    int, int);

template WM5_MATHEMATICS_ITEM
double* PolyFit3<double> (int, const double*, const double*, const double*,
    int, int);
//----------------------------------------------------------------------------
}
