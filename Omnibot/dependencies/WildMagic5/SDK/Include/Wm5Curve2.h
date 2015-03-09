// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CURVE2_H
#define WM5CURVE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Curve2
{
public:
    // Abstract base class.
    Curve2 (Real tmin, Real tmax);
    virtual ~Curve2 ();

    // Interval on which curve parameter is defined.  If you are interested
    // in only a subinterval of the actual domain of the curve, you may set
    // that subinterval with SetTimeInterval.  This function requires that
    // tmin < tmax.
    Real GetMinTime () const;
    Real GetMaxTime () const;
    void SetTimeInterval (Real tmin, Real tmax);

    // Position and derivatives.
    virtual Vector2<Real> GetPosition (Real t) const = 0;
    virtual Vector2<Real> GetFirstDerivative (Real t) const = 0;
    virtual Vector2<Real> GetSecondDerivative (Real t) const = 0;
    virtual Vector2<Real> GetThirdDerivative (Real t) const = 0;

    // Differential geometric quantities.
    Real GetSpeed (Real t) const;
    virtual Real GetLength (Real t0, Real t1) const = 0;
    Real GetTotalLength () const;
    Vector2<Real> GetTangent (Real t) const;
    Vector2<Real> GetNormal (Real t) const;
    void GetFrame (Real t, Vector2<Real>& position, Vector2<Real>& tangent,
        Vector2<Real>& normal) const;
    Real GetCurvature (Real t) const;

    // Inverse mapping of s = Length(t) given by t = Length^{-1}(s).
    virtual Real GetTime (Real length, int iterations = 32,
        Real tolerance = (Real)1e-06) const = 0;

    // Subdivision.
    void SubdivideByTime (int numPoints, Vector2<Real>*& points) const;
    void SubdivideByLength (int numPoints, Vector2<Real>*& points) const;

protected:
    // Curve parameter is t where tmin <= t <= tmax.
    Real mTMin, mTMax;
};

typedef Curve2<float> Curve2f;
typedef Curve2<double> Curve2d;

}

#endif
