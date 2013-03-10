// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5RandomHypersphere.h"
#include "Wm5Math.h"
using namespace Wm5;

//----------------------------------------------------------------------------
static void RecurseRandomPoint (int dimension, double* point)
{
    // Select random point on circle.
    double angle = Mathd::IntervalRandom(0.0,Mathd::TWO_PI);
    double cs = Mathd::Cos(angle);
    double sn = Mathd::Sin(angle);

    if (dimension > 3)
    {
        // Split components into two sets and adjust values.
        int i, half = dimension/2;
        for (i = 0; i < half; ++i)
        {
            point[i] *= cs;
        }
        for (i = half; i < dimension; ++i)
        {
            point[i] *= sn;
        }

        // Recurse on each half of the components.
        RecurseRandomPoint(half, point);
        RecurseRandomPoint(dimension - half, point + half);
    }
    else if (dimension == 3)
    {
        double value = Mathd::SymmetricRandom();
        double complement = Mathd::Sqrt(Mathd::FAbs(1.0 - value*value));
        point[0] *= value;
        point[1] *= complement*cs;
        point[2] *= complement*sn;
    }
    else if (dimension == 2)
    {
        point[0] *= cs;
        point[1] *= sn;
    }
}
//----------------------------------------------------------------------------
void Wm5::RandomPointOnHypersphere (int dimension, double* point)
{
    for (int i = 0; i < dimension; ++i)
    {
        point[i] = 1.0;
    }

    RecurseRandomPoint(dimension, point);
}
//----------------------------------------------------------------------------
void Wm5::Histogram (int dimension, double angle, int numPoints,
    double** points, int* histogram)
{
    // Count the number of points located in the cone of specified angle
    // about each of the samples.
    double cs = Mathd::Cos(angle);

    for (int i = 0; i < numPoints; ++i)
    {
        histogram[i] = 0;
        for (int j = 0; j < numPoints; ++j)
        {
            // Compute dot product between points P[i] and P[j].
            double dot = 0;
            for (int k = 0; k < dimension; ++k)
            {
                dot += points[i][k]*points[j][k];
            }
            if (dot >= cs)
            {
                histogram[i]++;
            }
        }
    }
}
//----------------------------------------------------------------------------
