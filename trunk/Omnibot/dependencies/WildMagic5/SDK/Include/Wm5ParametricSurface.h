// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PARAMETRICSURFACE_H
#define WM5PARAMETRICSURFACE_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Surface.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM ParametricSurface : public Surface<Real>
{
public:
    // Abstract base class.
    virtual ~ParametricSurface ();

    // The parametric domain is either rectangular or triangular.  Valid (u,v)
    // values for a rectangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax
    // Valid (u,v) values for a triangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax,
    //   (vmax-vmin)*(u-umin)+(umax-umin)*(v-vmax) <= 0
    Real GetUMin () const;
    Real GetUMax () const;
    Real GetVMin () const;
    Real GetVMax () const;
    bool IsRectangular () const;

    // position and derivatives up to second order
    virtual Vector3<Real> P (Real u, Real v) const = 0;
    virtual Vector3<Real> PU (Real u, Real v) const = 0;
    virtual Vector3<Real> PV (Real u, Real v) const = 0;
    virtual Vector3<Real> PUU (Real u, Real v) const = 0;
    virtual Vector3<Real> PUV (Real u, Real v) const = 0;
    virtual Vector3<Real> PVV (Real u, Real v) const = 0;

    // Compute a coordinate frame.  The set {T0,T1,N} is a right-handed
    // orthonormal set.
    void GetFrame (Real u, Real v, Vector3<Real>& position,
        Vector3<Real>& tangent0, Vector3<Real>& tangent1,
        Vector3<Real>& normal) const;

    // Differential geometric quantities.  The returned scalars are the
    // principal curvatures and the returned vectors are the corresponding
    // principal directions.
    void ComputePrincipalCurvatureInfo (Real u, Real v, Real& curv0,
        Real& curv1, Vector3<Real>& dir0, Vector3<Real>& dir1);

protected:
    ParametricSurface (Real umin, Real umax, Real vmin, Real vmax,
        bool rectangular);

    Real mUMin, mUMax, mVMin, mVMax;
    bool mRectangular;
};

typedef ParametricSurface<float> ParametricSurfacef;
typedef ParametricSurface<double> ParametricSurfaced;

}

#endif
