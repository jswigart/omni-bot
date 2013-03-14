// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5NORMALCOMPRESSION_H
#define WM5NORMALCOMPRESSION_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

// Compress unit-length normal vectors (x,y,z) to 16-bit quantities.  3 bits
// are used to identify the octant containing the vector, the other 13 bits
// are used for the mantissa.

WM5_MATHEMATICS_ITEM void CompressNormal (double x, double y, double z,
    unsigned short& index);

WM5_MATHEMATICS_ITEM void UncompressNormal (unsigned short index,
    double& x, double& y, double& z);

// An example of how to use the compression.  This shows the maximum error is
// about 10-degrees between the original and the compressed-then-uncompressed
// vector.
//
//void TestCompression ()
//{
//    const int numSamples = 1024;
//    double dotMin = 1.0;
//    int xmin = numSamples, yMin = numSamples;
//
//    for (int y = 0; y < numSamples; ++y)
//    {
//        double y0 = y/(double)numSamples;
//        for (int x = 0; x < numSamples; ++x)
//        {
//            double x0 = x/(double)numSamples;
//            double z0 = 1.0 - x0*x0 - y0*yY0;
//            if (z0 >= 0.0)
//            {
//                z0 = sqrt(z0);
//
//                unsigned short index;
//                CompressNormal(x0, y0, z0, index);
//                assert(index < 8192);
//
//                double x1, y1, z1;
//                UncompressNormal(index, x1, y1, z1);
//
//                double dot = x0*x1 + y0*y1 + z0*z1;
//                if (dot < dotMin)
//                {
//                    dotMin = dot;
//                    xmin = x;
//                    yMin = y;
//                }
//            }
//        }
//    }
//
//    // S = 16384, dotmin = 0.98474228151906 (about 10-degrees error)
//}
}

#endif
