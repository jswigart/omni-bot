// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRUTILITY3_H
#define WM5INTRUTILITY3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Box3.h"
#include "Wm5Segment3.h"
#include "Wm5Triangle3.h"

namespace Wm5
{

//----------------------------------------------------------------------------
template <typename Real>
class WM5_MATHEMATICS_ITEM IntrConfiguration
{
public:
    // ContactSide (order of the intervals of projection).
    enum
    {
        LEFT,
        RIGHT,
        NONE
    };

    // VertexProjectionMap (how the vertices are projected to the minimum
    // and maximum points of the interval).
    enum
    {
        m2, m11,             // segments
        m3, m21, m12, m111,  // triangles
        m44, m2_2, m1_1      // boxes
    };

    // The VertexProjectionMap value for the configuration.
    int mMap;

    // The order of the vertices.
    int mIndex[8];

    // Projection interval.
    Real mMin, mMax;
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
template <typename Real>
class WM5_MATHEMATICS_ITEM IntrAxis
{
public:
    // Test-query for intersection of projected intervals.  The velocity
    // input is the difference objectVelocity1 - objectVelocity0.  The
    // first and last times of contact are computed.
    static bool Test (const Vector3<Real>& axis,
        const Vector3<Real> segment[2], const Triangle3<Real>& triangle,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast);

    static bool Test (const Vector3<Real>& axis,
        const Vector3<Real> segment[2], const Box3<Real>& box,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast);

    static bool Test (const Vector3<Real>& axis,
        const Triangle3<Real>& triangle, const Box3<Real>& box,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast);

    static bool Test (const Vector3<Real>& axis,
        const Box3<Real>& box0, const Box3<Real>& box1,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast);

    // Find-query for intersection of projected intervals.  The velocity
    // input is the difference objectVelocity1 - objectVelocity0.  The
    // first and last times of contact are computed, as is information about
    // the contact configuration and the ordering of the projections (the
    // contact side).
    static bool Find (const Vector3<Real>& axis,
        const Vector3<Real> segment[2], const Triangle3<Real>& triangle,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast,
        int& side, IntrConfiguration<Real>& segCfgFinal,
        IntrConfiguration<Real>& triCfgFinal);

    static bool Find (const Vector3<Real>& axis,
        const Vector3<Real> segment[2], const Box3<Real>& box,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast,
        int& side, IntrConfiguration<Real>& segCfgFinal,
        IntrConfiguration<Real>& boxCfgFinal);

    static bool Find (const Vector3<Real>& axis,
        const Triangle3<Real>& triangle, const Box3<Real>& box,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast,
        int& side, IntrConfiguration<Real>& triCfgFinal,
        IntrConfiguration<Real>& boxCfgFinal);

    static bool Find (const Vector3<Real>& axis,
        const Box3<Real>& box0, const Box3<Real>& box1,
        const Vector3<Real>& velocity, Real tmax, Real& tfirst, Real& tlast,
        int& side, IntrConfiguration<Real>& box0CfgFinal,
        IntrConfiguration<Real>& box1CfgFinal);

    // Projections.
    static void GetProjection (const Vector3<Real>& axis,
        const Vector3<Real> segment[2], Real& imin, Real& imax);

    static void GetProjection (const Vector3<Real>& axis,
        const Triangle3<Real>& triangle, Real& imin, Real& imax);

    static void GetProjection (const Vector3<Real>& axis,
        const Box3<Real>& box, Real& imin, Real& imax);

    // Configurations.
    static void GetConfiguration (const Vector3<Real>& axis, 
        const Vector3<Real> segment[2], IntrConfiguration<Real>& cfg);
    
    static void GetConfiguration (const Vector3<Real>& axis, 
        const Triangle3<Real>& triangle, IntrConfiguration<Real>& cfg);

    static void GetConfiguration (const Vector3<Real>& axis, 
        const Box3<Real>& box, IntrConfiguration<Real>& cfg);

    // Low-level test-query for projections.
    static bool Test (const Vector3<Real>& axis,
        const Vector3<Real>& velocity, Real min0, Real max0, Real min1,
        Real max1, Real tmax, Real& tfirst, Real& tlast);

    // Low-level find-query for projections.
    static bool Find (const Vector3<Real>& axis,
        const Vector3<Real>& velocity,
        const IntrConfiguration<Real>& cfg0Start,
        const IntrConfiguration<Real>& cfg1Start, Real tmax, int& side,
        IntrConfiguration<Real>& cfg0Final,
        IntrConfiguration<Real>& cfg1Final, Real& tfirst, Real& tlast);
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
template <typename Real>
class WM5_MATHEMATICS_ITEM FindContactSet
{
public:
    FindContactSet (const Vector3<Real> segment[2],
        const Triangle3<Real>& triangle, int side,
        const IntrConfiguration<Real>& segCfg,
        const IntrConfiguration<Real>& triCfg,
        const Vector3<Real>& segVelocity, const Vector3<Real>& triVelocity,
        Real tfirst, int& quantity, Vector3<Real>* P);

    FindContactSet (const Vector3<Real> segment[2], const Box3<Real>& box,
        int side, const IntrConfiguration<Real>& segCfg,
        const IntrConfiguration<Real>& boxCfg,
        const Vector3<Real>& segVelocity, const Vector3<Real>& boxVelocity,
        Real tfirst, int& quantity, Vector3<Real>* P);

    FindContactSet (const Triangle3<Real>& triangle,
        const Box3<Real>& box, int side,
        const IntrConfiguration<Real>& triCfg,
        const IntrConfiguration<Real>& boxCfg,
        const Vector3<Real>& triVelocity, const Vector3<Real>& boxVelocity,
        Real tfirst, int& quantity, Vector3<Real>* P);

    FindContactSet (const Box3<Real>& box0, const Box3<Real>& box1,
        int side, const IntrConfiguration<Real>& box0Cfg,
        const IntrConfiguration<Real>& box1Cfg,
        const Vector3<Real>& box0Velocity,
        const Vector3<Real>& box1Velocity, Real tfirst, int& quantity,
        Vector3<Real>* P);

private:
    // These functions are called when it is known that the features are
    // intersecting.  Consequently, they are specialized versions of the
    // object-object intersection algorithms.

    static void ColinearSegments (const Vector3<Real> segment0[2],
        const Vector3<Real> segment1[2], int& quantity, Vector3<Real>* P);

    static void SegmentThroughPlane (const Vector3<Real> segment[2],
        const Vector3<Real>& planeOrigin, const Vector3<Real>& planeNormal,
        int& quantity, Vector3<Real>* P);

    static void SegmentSegment (const Vector3<Real> segment0[2], 
        const Vector3<Real> segment1[2], int& quantity, Vector3<Real>* P);

    static void ColinearSegmentTriangle (const Vector3<Real> segment[2],
        const Vector3<Real> triangle[3], int& quantity, Vector3<Real>* P);

    static void CoplanarSegmentRectangle (const Vector3<Real> segment[2],
        const Vector3<Real> rectangle[4], int& quantity, Vector3<Real>* P);

    static void CoplanarTriangleRectangle (const Vector3<Real> triangle[3],
        const Vector3<Real> rectangle[4], int& quantity, Vector3<Real>* P);

    static void CoplanarRectangleRectangle (
        const Vector3<Real> rectangle0[4],
        const Vector3<Real> rectangle1[4], int& quantity, Vector3<Real>* P);
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Miscellaneous support.
//----------------------------------------------------------------------------
// The input and output polygons are stored in P.  The size of P is
// assumed to be large enough to store the clipped convex polygon vertices.
// For now the maximum array size is 8 to support the current intersection
// algorithms.
template <typename Real> WM5_MATHEMATICS_ITEM
void ClipConvexPolygonAgainstPlane (const Vector3<Real>& normal,
    Real bonstant, int& quantity, Vector3<Real>* P);

// Translates an index into the box back into real coordinates.
template <typename Real> WM5_MATHEMATICS_ITEM
Vector3<Real> GetPointFromIndex (int index, const Box3<Real>& box);
//----------------------------------------------------------------------------
}

#endif
