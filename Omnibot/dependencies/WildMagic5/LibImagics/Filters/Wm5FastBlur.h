// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5FASTBLUR_H
#define WM5FASTBLUR_H

// The idea is to represent the blurred image as f(x,s) in terms of position
// x and scale s.  Gaussian blurring is accomplished by using the input image
// I(x,s0) as the initial image (of scale s0 > 0) for the partial differential
// equation
//   s*df/ds = s^2*Laplacian(f)
// where the Laplacian operator is
//   Laplacian = (d/dx)^2, dimension 1
//   Laplacian = (d/dx)^2+(d/dy)^2, dimension 2
//   Laplacian = (d/dx)^2+(d/dy)^2+(d/dz)^2, dimension 3
//
// The term s*df/ds is approximated by
//   s*df(x,s)/ds = (f(x,b*s)-f(x,s))/ln(b)
// for b > 1, but close to 1, where ln(b) is the natural logarithm of b.  If
// you take the limit of the right-hand side as b approaches 1, you get the
// left-hand side.
//
// The term s^2*((d/dx)^2)f is approximated by
//   s^2*((d/dx)^2)f = (f(x+h*s,s)-2*f(x,s)+f(x-h*s,s))/h^2
// for h > 0, but close to zero.
//
// Equating the approximations for the left-hand side and the right-hand side
// of the partial differential equation leads to the numerical method used in
// this code.
//
// For iterative application of these functions, the caller is responsible
// for constructing a geometric sequence of scales,
//   s0, s1 = s0*b, s2 = s1*b = s0*b^2, ...
// where the base b satisfies 1 < b < exp(0.5*d) where d is the dimension of
// the image.  The upper bound on b guarantees stability of the finite
// difference method used to approximate the partial differential equation.
// The method assumes a pixel size of h = 1.
//
//
// Sample usage in 2D:
//
// const int xBound = 256, yBound = 256;
// float** image = new2<float>(xBound, yBound);
// float** temp = new2<float>(xBound, yBound);
// <initialization image[][] goes here...>
// const int numPasses = <number of passes to blur>;
// const double scale = 1.0, logBase = 0.125, base = exp(logBase);
// for (int i = 1; i <= numPasses; ++i, scale *= base)
// {
//     FastBlur2(xBound, yBound, image, temp, scale, logBase);
//     <use the blurred image 'image' here if desired>;
// }
// delete2(temp);
// delete2(image);

#include "Wm5ImagicsLIB.h"
#include "Wm5Math.h"

namespace Wm5
{

// Explicit instantiation of the template code is done for PixelType of:
// short, int, float, or double.  The algorithm requires a temporary image
// of the same size as the original image.  The caller is responsible for
// providing this.

template <typename Numeric>
void FastBlur1 (int xBound, Numeric* image,
    Numeric* temp, double scale, double logBase);

template <typename Numeric>
void FastBlur2 (int xBound, int yBound,
    Numeric** image, Numeric** temp, double scale, double logBase);

template <typename Numeric>
void FastBlur3 (int xBound, int yBound, int zBound,
    Numeric*** image, Numeric*** temp, double scale, double logBase);

#include "Wm5FastBlur.inl"

}

#endif
