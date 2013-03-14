// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5FRUSTUM3_H
#define WM5FRUSTUM3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Frustum3
{
public:
    // Orthogonal frustum.  Let E be the origin, D be the direction vector,
    // U be the up vector, and R be the right vector.  Let u > 0 and r > 0 be
    // the extents in the U and R directions, respectively.  Let n and f be
    // the extents in the D direction with 0 < n < f.  The four corners of the
    // frustum in the near plane are E + n*D + s0*u*U + s1*r*R where |s0| =
    // |s1| = 1 (four choices).  The four corners of the frustum in the far
    // plane are E + f*D + (f/n)*(s0*u*U + s1*r*R) where |s0| = |s1| = 1 (four
    // choices).

    // Construction and destruction.
    Frustum3 ();  // uninitialized
    ~Frustum3 ();

    Frustum3 (const Vector3<Real>& origin, const Vector3<Real>& dvector,
        const Vector3<Real>& uvector, const Vector3<Real>& rvector,
        Real dmin, Real dmax, Real ubound, Real rbound);

    // The Update() function must be called whenever changes are made to
    // DMin, DMac, UBound, or RBound.  The values DRatio, MTwoUF, and MTwoRF
    // are dependent on the changes, so call the Get accessors only after the
    // Update() call.
    void Update ();
    inline Real GetDRatio () const;
    inline Real GetMTwoUF () const;
    inline Real GetMTwoRF () const;

    void ComputeVertices (Vector3<Real> vertex[8]) const;

    Vector3<Real> Origin, DVector, UVector, RVector;
    Real DMin, DMax, UBound, RBound;

protected:
    // Quantities derived from the constructor inputs.
    Real mDRatio, mMTwoUF, mMTwoRF;
};

#include "Wm5Frustum3.inl"

typedef Frustum3<float> Frustum3f;
typedef Frustum3<double> Frustum3d;

}

#endif
