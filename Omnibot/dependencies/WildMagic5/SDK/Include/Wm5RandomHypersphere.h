// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5RANDOMHYPERSPHERE_H
#define WM5RANDOMHYPERSPHERE_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

// Generate N random points on the hypersphere in D-space,
//
//     x_1^2 + ... + x_D^2 = 1
//
// The function selects a random angle in [0,2*pi) and partitions
// the equation into
//
//     x_1^2 + ... + x_{D/2}^2 = (cos(A))^2
//
// and
//
//     x_{D/2+1}^2 + ... + x_D^2 = (sin(A))^2
//
// The function initializes all components of P to 1.  The partitioned
// components are updated as x_i *= cos(A) for 0 <= i < D/2 and
// x_i *= sin(A) for D/2 <= i < D.  The function is recursively called
// on the partitioned components.

WM5_MATHEMATICS_ITEM void RandomPointOnHypersphere (int dimension,
    double* point);

// An attempt to determine the uniformity of N randomly generated
// points P[0] through P[N-1] on the hypersphere.  Select a positive
// angle.  For each point P[i] count the number H[i] of random points
// P[j] which lie in the cone with axis P[i] and specified angle.  For
// N suitably large, H[i] should be constant for all i.  To be really
// sure of the uniformity, you should look at other cones whose axes
// are not the sample points.  However, this requires generating random
// points to get the axes and, well, you can see the problem...

WM5_MATHEMATICS_ITEM void Histogram (int dimension, double angle,
    int numPoints, double** points, int* histogram);

//An example of how to use these functions.
//
//void TestRandomHyperspherePoints ()
//{
//    const int numPoints = 4096;
//    const int dimension = 3;
//    int i;
//
//    double** points = new2 <double>(numPoints, dimension);
//    for (i = 0; i < numPoints; ++i)
//    {
//        RandomPointOnHypersphere(dimension, points[i]);
//    }
//
//    int* histogram = new1<int>(numPoints);
//    double angle = 0.5;
//    Histogram(dimension, angle, numPoints, points, histogram);
//
//    ofstream ostr("histo.txt");
//    for (i = 0; i < numPoints; ++i)
//    {
//        ostr << i << ' ' << histogram[i] << endl;
//    }
//
//    delete1(histogram);
//    delete2(points);
//}
}

#endif
