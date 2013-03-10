// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ELLIPSOIDGEODESIC_H
#define WM5ELLIPSOIDGEODESIC_H

#include "Wm5MathematicsLIB.h"
#include "Wm5RiemannianGeodesic.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM EllipsoidGeodesic : public RiemannianGeodesic<Real>
{
public:
    // The ellipsoid is (x/a)^2 + (y/b)^2 + (z/c)^2 = 1, where xExtent is
    // 'a', yExtent is 'b', and zExtent is 'c'.  The surface is represented
    // parametrically by angles u and v, say P(u,v) = (x(u,v),y(u,v),z(u,v)),
    //   P(u,v) =(a*cos(u)*sin(v), b*sin(u)*sin(v), c*cos(v))
    // with 0 <= u < 2*pi and 0 <= v <= pi.  The first-order derivatives are
    //   dP/du = (-a*sin(u)*sin(v), b*cos(u)*sin(v), 0)
    //   dP/dv = (a*cos(u)*cos(v), b*sin(u)*cos(v), -c*sin(v))
    // The metric tensor elements are
    //   g_{00} = Dot(dP/du,dP/du)
    //   g_{01} = Dot(dP/du,dP/dv)
    //   g_{10} = g_{01}
    //   g_{11} = Dot(dP/dv,dP/dv)

    EllipsoidGeodesic (Real xExtent, Real yExtent, Real zExtent);
    virtual ~EllipsoidGeodesic ();

    Vector3<Real> ComputePosition (const GVector<Real>& point);
    virtual void ComputeMetric (const GVector<Real>& point);
    virtual void ComputeChristoffel1 (const GVector<Real>& point);

    // To compute the geodesic path connecting two parameter points (u0,v0)
    // and (u1,v1):
    //
    // float a, b, c;  // the extents of the ellipsoid
    // EllipsoidGeodesic<float> EG(a,b,c);
    // GVectorf Param0(2), Param1(2);
    // Param0[0] = u0;
    // Param0[1] = v0;
    // Param1[0] = u1;
    // Param1[1] = v1;
    //
    // int quantity;
    // GVectorf* path;
    // EG.ComputeGeodesic(Param0, Param1, quantity, path);

private:
    using RiemannianGeodesic<Real>::mMetric;
    using RiemannianGeodesic<Real>::mChristoffel1;

    Real mXExtent, mYExtent, mZExtent;
};

typedef EllipsoidGeodesic<float> EllipsoidGeodesicf;
typedef EllipsoidGeodesic<double> EllipsoidGeodesicd;

}

#endif
