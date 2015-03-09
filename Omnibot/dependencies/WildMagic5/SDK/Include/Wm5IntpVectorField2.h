// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPVECTORFIELD2_H
#define WM5INTPVECTORFIELD2_H

// Given points (x0[i],y0[i]) which are mapped to (x1[i],y1[i]) for
// 0 <= i < N, interpolate positions (xIn,yIn) to (xOut,yOut).

#include "Wm5MathematicsLIB.h"
#include "Wm5IntpQdrNonuniform2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpVectorField2
{
public:
    // Construction and destruction.  If you want IntpVectorField2 to delete
    // the input arrays during destruction, set owner to 'true'.  Otherwise,
    // you own the arrays and must delete them yourself.
    //
    // The computation type is for the Delaunay triangulation and should be
    // one of Query::{QT_INT64,QT_INTEGER,QT_RATIONAL,QT_REAL}.
    IntpVectorField2 (int quantity, Vector2<Real>* domain,
        Vector2<Real>* range, bool owner, Query::Type queryType);

    ~IntpVectorField2 ();

    // Return 'true' if and only if (xIn,yIn) is in the convex hull of the
    // input points.  In this case, (xOut,yOut) is valid.  If the return
    // value is 'false', (xOut,yOut) is invalid and should not be used.
    bool Evaluate (const Vector2<Real>& input, Vector2<Real>& output); 

protected:
    Delaunay2<Real>* mDT;
    IntpQdrNonuniform2<Real>* mXInterp;
    IntpQdrNonuniform2<Real>* mYInterp;
};

typedef IntpVectorField2<float> IntpVectorField2f;
typedef IntpVectorField2<double> IntpVectorField2d;

}

#endif
