// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPLINEARNONUNIFORM3_H
#define WM5INTPLINEARNONUNIFORM3_H

// Linear interpolation of a network of triangles whose vertices are of the
// form (x,y,z,f(x,y,z)).

#include "Wm5MathematicsLIB.h"
#include "Wm5Delaunay3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpLinearNonuniform3
{
public:
    // Construction and destruction.  If you want IntpLinearNonuniform3 to
    // delete the input array during destruction, set owner to 'true'.
    // Otherwise, you own the array and must delete it yourself.
    IntpLinearNonuniform3 (const Delaunay3<Real>& DT, Real* F, bool owner);
    ~IntpLinearNonuniform3 ();

    // Linear interpolation.  The return value is 'true' if and only if the
    // input point is in the convex hull of the input vertices, in which case
    // the interpolation is valid.
    bool Evaluate (const Vector3<Real>& P, Real& F);

private:
    const Delaunay3<Real>* mDT;
    Real* mF;
    bool mOwner;
};

typedef IntpLinearNonuniform3<float> IntpLinearNonuniform3f;
typedef IntpLinearNonuniform3<double> IntpLinearNonuniform3d;

}

#endif
