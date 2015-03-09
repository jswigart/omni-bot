// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRTRIANGLE2TRIANGLE2_H
#define WM5INTRTRIANGLE2TRIANGLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Intersector1.h"
#include "Wm5Triangle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrTriangle2Triangle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrTriangle2Triangle2 (const Triangle2<Real>& triangle0,
        const Triangle2<Real>& triangle1);

    // Object access.
    const Triangle2<Real>& GetTriangle0 () const;
    const Triangle2<Real>& GetTriangle1 () const;

    // Static queries.
    virtual bool Test ();
    virtual bool Find ();

    // Dynamic queries.
    virtual bool Test (Real tmax, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);
    virtual bool Find (Real tmax, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);

    // Information about the intersection set.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::mContactTime;

    static int WhichSide (const Vector2<Real> V[3], const Vector2<Real>& P,
        const Vector2<Real>& D);

    static void ClipConvexPolygonAgainstLine (const Vector2<Real>& N,
        Real c, int& quantity, Vector2<Real> V[6]);

    enum ProjectionMap
    {
        M21,  // 2 vertices map to min, 1 vertex maps to max
        M12,  // 1 vertex maps to min, 2 vertices map to max
        M11   // 1 vertex maps to min, 1 vertex maps to max
    };

    class Configuration
    {
    public:
        ProjectionMap Map;  // how vertices map to the projection interval
        int Index[3];       // the sorted indices of the vertices
        Real Min, Max;      // the interval is [min,max]
    };

    void ComputeTwo (Configuration& cfg, const Vector2<Real> V[3],
        const Vector2<Real>& D, int i0, int i1, int i2);

    void ComputeThree (Configuration& cfg, const Vector2<Real> V[3],
        const Vector2<Real>& D, const Vector2<Real>& P);

    static bool NoIntersect (const Configuration& cfg0,
        const Configuration& cfg1, Real tmax, Real speed, int& side,
        Configuration& tcfg0, Configuration& tcfg1, Real& tfirst,
        Real& tlast);

    static void GetIntersection (const Configuration& cfg0,
        const Configuration& cfg1, int side, const Vector2<Real> V0[3],
        const Vector2<Real> V1[3], int& quantity, Vector2<Real> vertex[6]);

    // The objects to intersect.
    const Triangle2<Real>* mTriangle0;
    const Triangle2<Real>* mTriangle1;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[6];
};

typedef IntrTriangle2Triangle2<float> IntrTriangle2Triangle2f;
typedef IntrTriangle2Triangle2<double> IntrTriangle2Triangle2d;

}

#endif
