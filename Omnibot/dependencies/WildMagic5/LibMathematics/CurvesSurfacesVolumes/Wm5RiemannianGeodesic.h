// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5RIEMANNIANGEODESIC_H
#define WM5RIEMANNIANGEODESIC_H

#include "Wm5MathematicsLIB.h"
#include "Wm5GMatrix.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM RiemannianGeodesic
{
public:
    // Construction and destruction.  The input dimension must be two or
    // larger.
    RiemannianGeodesic (int dimension);
    virtual ~RiemannianGeodesic ();

    // Tweakable parameters.
    // 1. The integral samples are the number of samples used in the Trapezoid
    //    Rule numerical integrator.
    // 2. The search samples are the number of samples taken along a ray for
    //    the steepest descent algorithm used to refine the vertices of the
    //    polyline approximation to the geodesic curve.
    // 3. The derivative step is the value of h used for centered difference
    //    approximations df/dx = (f(x+h)-f(x-h))/(2*h) in the steepest
    //    descent algorithm.
    // 4. The number of subdivisions indicates how many times the polyline
    //    segments should be subdivided.  The number of polyline vertices
    //    will be pow(2,subdivisions)+1.
    // 5. The number of refinements per subdivision.  Setting this to a
    //    positive value appears necessary when the geodesic curve has a
    //    large length.
    // 6. The search radius is the distance over which the steepest descent
    //    algorithm searches for a minimum on the line whose direction is the
    //    estimated gradient.  The default of 1 means the search interval is
    //    [-L,L], where L is the length of the gradient.  If the search
    //    radius is r, then the interval is [-r*L,r*L].
    int IntegralSamples;  // default = 16
    int SearchSamples;    // default = 32
    Real DerivativeStep;  // default = 0.0001
    int Subdivisions;     // default = 7
    int Refinements;      // default = 8
    Real SearchRadius;    // default = 1.0

    // The dimension of the manifold.
    int GetDimension () const;

    // Returns the length of the line segment connecting the points.
    Real ComputeSegmentLength (const GVector<Real>& point0,
        const GVector<Real>& point1);

    // Compute the total length of the polyline.  The lengths of the segments
    // are computed relative to the metric tensor.
    Real ComputeTotalLength (int quantity, const GVector<Real>* path);

    // Returns a polyline approximation to a geodesic curve connecting the
    // points.  The caller is responsible for deleting the output array (it
    // is dynamically allocated).
    void ComputeGeodesic (const GVector<Real>& end0,
        const GVector<Real>& end1, int& quantity,
        GVector<Real>*& path);

    // Start with the midpoint M of the line segment (E0,E1) and use a
    // steepest descent algorithm to move M so that
    //     Length(E0,M) + Length(M,E1) < Length(E0,E1)
    // This is essentially a relaxation scheme that inserts points into the
    // current polyline approximation to the geodesic curve.
    bool Subdivide (const GVector<Real>& end0, GVector<Real>& mid,
        const GVector<Real>& end1);

    // Apply the steepest descent algorithm to move the midpoint M of the
    // line segment (E0,E1) so that
    //     Length(E0,M) + Length(M,E1) < Length(E0,E1)
    // This is essentially a relaxation scheme that inserts points into the
    // current polyline approximation to the geodesic curve.
    bool Refine (const GVector<Real>& end0, GVector<Real>& mid,
        const GVector<Real>& end1);

    // A callback that is executed during each call of Refine.
    typedef void (*RefineCallbackFunction)();
    RefineCallbackFunction RefineCallback;

    // Information to be used during the callback.
    int GetSubdivisionStep () const;
    int GetRefinementStep () const;
    int GetCurrentQuantity () const;

    // Curvature computations to measure how close the approximating
    // polyline is to a geodesic.

    // Returns the total curvature of the line segment connecting the points.
    Real ComputeSegmentCurvature (const GVector<Real>& point0,
        const GVector<Real>& point1);

    // Compute the total curvature of the polyline.  The curvatures of the
    // segments are computed relative to the metric tensor.
    Real ComputeTotalCurvature (int quantity, const GVector<Real>* path);

protected:
    // Support for ComputeSegmentCurvature.
    Real ComputeIntegrand (const GVector<Real>& pos,
        const GVector<Real>& der);

    // Compute the metric tensor for the specified point.  Derived classes
    // are responsible for implementing this function.
    virtual void ComputeMetric (const GVector<Real>& point) = 0;

    // Compute the Christoffel symbols of the first kind for the current
    // point.  Derived classes are responsible for implementing this function.
    virtual void ComputeChristoffel1 (const GVector<Real>& point) = 0;

    // Compute the inverse of the current metric tensor.  The function
    // returns 'true' iff the inverse exists.
    bool ComputeMetricInverse ();

    // Compute the derivative of the metric tensor for the current state.
    // This is a triply indexed quantity, the values computed using the
    // Christoffel symbols of the first kind.
    void ComputeMetricDerivative ();

    // Compute the Christoffel symbols of the second kind for the current
    // state.  The values depend on the inverse of the metric tensor, so
    // they may be computed only when the inverse exists.  The function
    // returns 'true' whenever the inverse metric tensor exists.
    bool ComputeChristoffel2 ();

    int mDimension;
    GMatrix<Real> mMetric;
    GMatrix<Real> mMetricInverse;
    GMatrix<Real>* mChristoffel1;
    GMatrix<Real>* mChristoffel2;
    GMatrix<Real>* mMetricDerivative;
    bool mMetricInverseExists;

    // Progress parameters that are useful to mRefineCallback.
    int mSubdivide, mRefine, mCurrentQuantity;

    // Derived tweaking parameters.
    Real mIntegralStep;      // = 1/(mIntegralQuantity-1)
    Real mSearchStep;        // = 1/mSearchQuantity
    Real mDerivativeFactor;  // = 1/(2*mDerivativeStep)
};

typedef RiemannianGeodesic<float> RiemannianGeodesicf;
typedef RiemannianGeodesic<double> RiemannianGeodesicd;

}

#endif
