// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRTRIANGLE3TRIANGLE3_H
#define WM5INTRTRIANGLE3TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Triangle3.h"
#include "Wm5Line3.h"
#include "Wm5Plane3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrTriangle3Triangle3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Triangle3 (const Triangle3<Real>& triangle0,
        const Triangle3<Real>& triangle1);

    // Object access.
    const Triangle3<Real>& GetTriangle0 () const;
    const Triangle3<Real>& GetTriangle1 () const;

    bool mReportCoplanarIntersections;  // default 'true'

    // Static queries.
    virtual bool Test ();
    virtual bool Find ();

    // Dynamic queries.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::IT_PLANE;
    using Intersector<Real,Vector3<Real> >::IT_OTHER;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;
    using Intersector<Real,Vector3<Real> >::mContactTime;

    static void ProjectOntoAxis (const Triangle3<Real>& triangle,
        const Vector3<Real>& axis, Real& fmin, Real& fmax);

    static void TrianglePlaneRelations (const Triangle3<Real>& triangle,
        const Plane3<Real>& plane, Real distance[3], int sign[3],
        int& positive, int& negative, int& zero);

    static void GetInterval (const Triangle3<Real>& triangle,
        const Line3<Real>& line, const Real distance[3],
        const int sign[3], Real param[2]);

    bool ContainsPoint (const Triangle3<Real>& triangle,
        const Plane3<Real>& plane, const Vector3<Real>& point);

    bool IntersectsSegment (const Plane3<Real>& plane,
        const Triangle3<Real>& triangle, const Vector3<Real>& end0,
        const Vector3<Real>& end1);

    bool GetCoplanarIntersection (const Plane3<Real>& plane,
        const Triangle3<Real>& tri0, const Triangle3<Real>& tri1);

    static bool TestOverlap (Real tmax, Real speed, Real umin,
        Real umax, Real vmin, Real vmax, Real& tfirst, Real& tlast);

    bool TestOverlap (const Vector3<Real>& axis, Real tmax,
        const Vector3<Real>& velocity, Real& tfirst, Real& tlast);

    enum ProjectionMap
    {
        M2, M11,                // lines
        M3, M21, M12, M111,     // triangles
        M44, M2_2, M1_1         // boxes
    };

    enum ContactSide
    {
        CS_LEFT,
        CS_RIGHT,
        CS_NONE
    };

    class Configuration
    {
    public:
        ProjectionMap mMap;  // how vertices map to the projection interval
        int mIndex[8];       // the sorted indices of the vertices
        Real mMin, mMax;      // the interval is [min,max]
    };

    static void ProjectOntoAxis (const Triangle3<Real>& triangle,
        const Vector3<Real>& axis, Configuration& cfg);

    bool FindOverlap (Real tmax, Real speed, const Configuration& UC,
        const Configuration& VC, ContactSide& side, Configuration& TUC,
        Configuration& TVC, Real& tfirst, Real& tlast);

    bool FindOverlap (const Vector3<Real>& axis, Real tmax,
        const Vector3<Real>& velocity, ContactSide& side,
        Configuration& tcfg0, Configuration& tcfg1, Real& tfirst,
        Real& tlast);

    void FindContactSet (const Triangle3<Real>& tri0,
        const Triangle3<Real>& tri1, ContactSide& side,
        Configuration& cfg0, Configuration& cfg1);

    void GetEdgeEdgeIntersection (const Vector3<Real>& U0,
        const Vector3<Real>& U1, const Vector3<Real>& V0,
        const Vector3<Real>& V1);

    void GetEdgeFaceIntersection (const Vector3<Real>& U0,
        const Vector3<Real>& U1, const Triangle3<Real>& tri);

    // The objects to intersect.
    const Triangle3<Real>* mTriangle0;
    const Triangle3<Real>* mTriangle1;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[6];
};

typedef IntrTriangle3Triangle3<float> IntrTriangle3Triangle3f;
typedef IntrTriangle3Triangle3<double> IntrTriangle3Triangle3d;

}

#endif
