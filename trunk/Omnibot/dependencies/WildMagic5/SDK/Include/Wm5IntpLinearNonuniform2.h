// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPLINEARNONUNIFORM2_H
#define WM5INTPLINEARNONUNIFORM2_H

// Linear interpolation of a network of triangles whose vertices are of the
// form (x,y,f(x,y)).

#include "Wm5MathematicsLIB.h"
#include "Wm5Delaunay2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpLinearNonuniform2
{
public:
    // Construction and destruction.  If you want IntpLinearNonuniform2 to
    // delete the input array during destruction, set owner to 'true'.
    // Otherwise, you own the array and must delete it yourself.
    IntpLinearNonuniform2 (const Delaunay2<Real>& DT, Real* F, bool owner);
    ~IntpLinearNonuniform2 ();

    // Linear interpolation.  The return value is 'true' if and only if the
    // input point P is in the convex hull of the input vertices, in which
    // case the interpolation is valid.
    bool Evaluate (const Vector2<Real>& P, Real& F);

private:
    const Delaunay2<Real>* mDT;
    Real* mF;
    bool mOwner;
};

typedef IntpLinearNonuniform2<float> IntpLinearNonuniform2f;
typedef IntpLinearNonuniform2<double> IntpLinearNonuniform2d;

}

#endif
