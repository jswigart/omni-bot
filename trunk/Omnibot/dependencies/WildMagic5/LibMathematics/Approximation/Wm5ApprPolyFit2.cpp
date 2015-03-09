// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprPolyFit2.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Real* PolyFit2 (int numSamples, const Real* xSamples, const Real* wSamples,
    int xDegree)
{
    int quantity = xDegree + 1;
    Real* coeff = new1<Real>(quantity);

    int i0, i1, s;

    // Powers of x.
    Real** xPower = new2<Real>(2*xDegree+1, numSamples);
    for (s = 0; s < numSamples; ++s)
    {
        xPower[s][0] = (Real)1;
        for (i0 = 1; i0 <= 2*xDegree; ++i0)
        {
            xPower[s][i0] = xSamples[s]*xPower[s][i0-1];
        }
    }

    // Vandermonde matrix and right-hand side of linear system.
    GMatrix<Real> A(quantity, quantity);
    Real* B = new1<Real>(quantity);

    for (i0 = 0; i0 <= xDegree; ++i0)
    {
        Real sum = (Real)0;
        for (s = 0; s < numSamples; ++s)
        {
            sum += wSamples[s]*xPower[s][i0];
        }

        B[i0] = sum;

        for (i1 = 0; i1 <= xDegree; ++i1)
        {
            sum = (Real)0;
            for (s = 0; s < numSamples; ++s)
            {
                sum += xPower[s][i0 + i1];
            }

            A(i0,i1) = sum;
        }
    }

    // Solve for the polynomial coefficients.
    bool hasSolution = LinearSystem<Real>().Solve(A, B, coeff);
    assertion(hasSolution, "Failed to solve linear system\n");
    WM5_UNUSED(hasSolution);

    delete1(B);
    delete2(xPower);

    return coeff;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
float* PolyFit2<float> (int, const float*, const float*, int);

template WM5_MATHEMATICS_ITEM
double* PolyFit2<double> (int, const double*, const double*, int);
//----------------------------------------------------------------------------
}
