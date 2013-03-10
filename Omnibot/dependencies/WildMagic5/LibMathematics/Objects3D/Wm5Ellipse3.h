// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5ELLIPSE3_H
#define WM5ELLIPSE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Ellipse3
{
public:
    // Plane containing ellipse is Dot(N,X-C) = 0 where X is any point in the
    // plane.  Vectors U, V, and N form an orthonormal right-handed set
    // (matrix [U V N] is orthonormal and has determinant 1).  Ellipse within
    // the plane is parameterized by X = C + a*cos(t)*U + b*sin(t)*V where
    // t is an angle in [0,2*pi) and where a >= b > 0.  The symbols in this
    // discussion are related to the class members as follows.  The member
    // 'Center' is C, 'Normal' is N, 'Major' is U, 'Minor' is V, 'MajorLength'
    // is a, and 'MinorLength' is b.

    // Construction and destruction.
    Ellipse3 ();  // uninitialized
    ~Ellipse3();

    Ellipse3 (const Vector3<Real>& center, const Vector3<Real>& normal,
        const Vector3<Real>& major, const Vector3<Real>& minor, 
        Real majorLength, Real minorLength);

    Vector3<Real> Center, Normal, Major, Minor;
    Real MajorLength, MinorLength;
};

#include "Wm5Ellipse3.inl"

typedef Ellipse3<float> Ellipse3f;
typedef Ellipse3<double> Ellipse3d;

}

#endif
