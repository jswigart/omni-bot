// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/15)

#ifndef WM5INTRELLIPSOID3ELLIPSOID3_H
#define WM5INTRELLIPSOID3ELLIPSOID3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ellipsoid3.h"
#include "Wm5Polynomial1.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrEllipsoid3Ellipsoid3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrEllipsoid3Ellipsoid3 (const Ellipsoid3<Real>& ellipsoid0,
        const Ellipsoid3<Real>& ellipsoid1);

    // Object access.
    const Ellipsoid3<Real>& GetEllipsoid0 () const;
    const Ellipsoid3<Real>& GetEllipsoid1 () const;

    // A form of test query.  The ellipsoids are separated, intersecting
    // (at points or curves), ellipsoid0 is strictly contained in ellipsoid1,
    // or ellipsoid1 is strictly contained in ellipsoid0.
    enum Classification
    {
        EC_ELLIPSOIDS_SEPARATED,
        EC_ELLIPSOIDS_INTERSECTING,
        EC_ELLIPSOID0_CONTAINS_ELLIPSOID1,
        EC_ELLIPSOID1_CONTAINS_ELLIPSOID0
    };

    Classification GetClassification () const;

private:
    // Support functions for GetClassification().
    static void BisectF (Real d0, Real d1, Real d2, Real d0c0, Real d1c1,
        Real d2c2, Real smin, Real fmin, Real smax, Real fmax, Real& s,
        Real& f);

    static void BisectDF (Real d0, Real d1, Real d2, Real d0c0, Real d1c1,
        Real d2c2, Real smin, Real dfmin, Real smax, Real dfmax, Real& s,
        Real& df);

    static void GetRoots (Real d0, Real d1, Real d2, Real c0, Real c1,
        Real c2, int& numRoots, Real* roots);

    static void BisectF (Real d0, Real d1, Real d0c0, Real d1c1, Real smin,
        Real fmin, Real smax, Real fmax, Real& s, Real& f);

    static void BisectDF (Real d0, Real d1, Real d0c0, Real d1c1, Real smin,
        Real dfmin, Real smax, Real dfmax, Real& s, Real& df);

    static void GetRoots (Real d0, Real d1, Real c0, Real c1, int& numRoots,
        Real* roots);

    static void GetRoots (Real d0, Real c0, int& numRoots, Real* roots);

    // The objects to intersect.
    const Ellipsoid3<Real>* mEllipsoid0;
    const Ellipsoid3<Real>* mEllipsoid1;
};

typedef IntrEllipsoid3Ellipsoid3<float> IntrEllipsoid3Ellipsoid3f;
typedef IntrEllipsoid3Ellipsoid3<double> IntrEllipsoid3Ellipsoid3d;

}

#endif
