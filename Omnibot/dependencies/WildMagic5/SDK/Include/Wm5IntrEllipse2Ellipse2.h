// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/15)

#ifndef WM5INTRELLIPSE2ELLIPSE2_H
#define WM5INTRELLIPSE2ELLIPSE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ellipse2.h"
#include "Wm5Polynomial1.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrEllipse2Ellipse2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrEllipse2Ellipse2 (const Ellipse2<Real>& ellipse0,
        const Ellipse2<Real>& ellipse1);

    // Object access.
    const Ellipse2<Real>& GetEllipse0 () const;
    const Ellipse2<Real>& GetEllipse1 () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q
    // satisfies 0 <= Q <= 4.  When Q > 0, the interpretation depends on the
    // intersection type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The ellipses are the same.  One of the ellipse objects is
    //              returned by GetIntersectionEllipse.  Q is invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Ellipse2<Real>& GetIntersectionEllipse () const;
    bool IsTransverseIntersection (int i) const;

    // Digits of accuracy for root finding.  The default value is 10.
    int DIGITS_ACCURACY;

    // A form of test query.  The ellipses are separated, intersecting,
    // ellipse0 is strictly contained in ellipse1, or ellipse1 is strictly
    // contained in ellipse0.
    enum Classification
    {
        EC_ELLIPSES_SEPARATED,
        EC_ELLIPSES_INTERSECTING,
        EC_ELLIPSE0_CONTAINS_ELLIPSE1,
        EC_ELLIPSE1_CONTAINS_ELLIPSE0
    };

    Classification GetClassification () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // Support for sorting potential intersection points.
    class Measurement
    {
    public:
        Measurement ();
        bool operator< (const Measurement& measure) const;

        // <x, y, sqrt(Q0(x,y)^2 + Q1(x,y)^2)>
        Vector2<Real> Point;
        Real Q0, Q1, Norm, Angle0;
        bool Transverse;
    };

    static Polynomial1<Real> GetQuartic (const Ellipse2<Real>& ellipse0,
        const Ellipse2<Real>& ellipse1);

    bool RefinePoint (const Real coeff[6], Vector2<Real>& point,
        Real& q0, Real& q1, Real& angle0);

    // Support functions for GetClassification().
    static void BisectF (Real d0, Real d1, Real d0c0, Real d1c1, Real smin,
        Real fmin, Real smax, Real fmax, Real& s, Real& f);

    static void BisectDF (Real d0, Real d1, Real d0c0, Real d1c1, Real smin,
        Real dfmin, Real smax, Real dfmax, Real& s, Real& df);

    static void GetRoots (Real d0, Real d1, Real c0, Real c1, int& numRoots,
        Real* roots);

    static void GetRoots (Real d0, Real c0, int& numRoots, Real* roots);

    // The objects to intersect.
    const Ellipse2<Real>* mEllipse0;
    const Ellipse2<Real>* mEllipse1;

    // Points of intersection.
    int mQuantity;
    Vector2<Real> mPoint[4];
    bool mTransverse[4];
};

typedef IntrEllipse2Ellipse2<float> IntrEllipse2Ellipse2f;
typedef IntrEllipse2Ellipse2<double> IntrEllipse2Ellipse2d;

}

#endif
