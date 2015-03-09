// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRPOLYFIT4_H
#define WM5APPRPOLYFIT4_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

// The samples are (x[i],y[i],z[i],w[i]) for 0 <= i < S.  Think of w as a
// function of x, y, and z, say w = f(x,y,z).  The function fits the samples
// with a polynomial of degree d0 in x, degree d1 in y, and degree d2 in z,
// say
//   w = sum_{i=0}^{d0} sum_{j=0}^{d1} sum_{k=0}^{d2} c[i][j][k]*x^i*y^j*z^k
// The method is a least-squares fitting algorithm.  The return array stores
// the c[i][j][k] values according to
//   returned[i+(d0+1)*(j+(d1+1)*k)] = c[i][j][k]
// for a total of (d0+1)*(d1+1)*(d2+1) coefficients.  The caller is
// responsible for deleting the input arrays if they were dynamically
// allocated.  The caller is also responsible for deleting the returned array.
//
// WARNING.  The fitting algorithm for polynomial terms
//   (1,x,x^2,...,x^d0), (1,y,y^2,...,y^d1), (1,z,z^2,...,z^d2)
// is known to be nonrobust for large degrees and for large magnitude data.
// One alternative is to use orthogonal polynomials
//   (f[0](x),...,f[d0](x)), (g[0](y),...,g[d1](y)), (h[0](z),...,h[d2](z))
// and apply the least-squares algorithm to these.  Another alternative is to
// transform
//   (x',y',z',w') = ((x-xcen)/rng, (y-ycen)/rng, (z-zcen)/rng, w/rng)
// where xmin = min(x[i]), xmax = max(x[i]), xcen = (xmin+xmax)/2,
// ymin = min(y[i]), ymax = max(y[i]), ycen = (ymin+ymax)/2, zmin = min(z[i]),
// zmax = max(z[i]), zcen = (zmin+zmax)/2, and
// rng = max(xmax-xmin,ymax-ymin,zmax-zmin).  Fit the (x',y',z',w') points,
//   w' = sum_{i=0}^{d0} sum_{j=0}^{d1} sum_{k=0}^{d2} c'[i][j][k] *
//          (x')^i*(y')^j*(z')^k
// The original polynomial is evaluated as
//   w = rng * sum_{i=0}^{d0} sum_{j=0}^{d1} sum_{k=0}^{d2} c'[i][j][k] *
//         ((x-xcen)/rng)^i * ((y-ycen)/rng)^j * ((z-zcen)/rng)^k

template <typename Real> WM5_MATHEMATICS_ITEM
Real* PolyFit4 (int numSamples, const Real* xSamples, const Real* ySamples,
    const Real* zSamples, const Real* wSamples, int xDegree, int yDegree,
    int zDegree);

}

#endif
