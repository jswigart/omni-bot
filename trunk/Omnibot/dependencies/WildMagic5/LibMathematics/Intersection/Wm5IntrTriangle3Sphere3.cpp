// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/06/24)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrTriangle3Sphere3.h"
#include "Wm5DistPoint3Triangle3.h"
#include "Wm5IntrSegment3Sphere3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrTriangle3Sphere3<Real>::IntrTriangle3Sphere3 (
    const Triangle3<Real>& triangle, const Sphere3<Real>& sphere)
    :
    mTriangle(&triangle),
    mSphere(&sphere)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrTriangle3Sphere3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrTriangle3Sphere3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Sphere3<Real>::Test ()
{
    DistPoint3Triangle3<Real> calc(mSphere->Center, *mTriangle);
    Real sqrDistance = calc.GetSquared();
    Real rSqr = mSphere->Radius*mSphere->Radius;
    return sqrDistance <= rSqr;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Sphere3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    // Get the triangle vertices.
    const Vector3<Real>* V = mTriangle->V;

    // Get the triangle edges.
    Vector3<Real> edges[3] =
    {
        V[1] - V[0],
        V[2] - V[1], 
        V[0] - V[2]
    };

    // Get the triangle normal.
    Vector3<Real> normal = edges[1].Cross(edges[0]);

    // Sphere center projection on triangle normal.
    Real NdC = normal.Dot(mSphere->Center);

    // Radius projected length in normal direction.  This defers the square
    // root to normalize normal until absolutely needed.
    Real rSqr = mSphere->Radius*mSphere->Radius;
    Real normRadiusSqr = normal.SquaredLength()*rSqr;

    // Triangle projection on triangle normal.
    Real NdT = normal.Dot(V[0]);

    // Distance from sphere to triangle along the normal.
    Real dist = NdC - NdT;
    
    // Normals for the plane formed by edge i and the triangle normal.
    Vector3<Real> ExN[3] =
    {
        edges[0].Cross(normal),
        edges[1].Cross(normal),
        edges[2].Cross(normal)
    };

    Segment3<Real> seg;

    if (dist*dist <= normRadiusSqr)
    {
        // Sphere currently intersects the plane of the triangle.

        // See which edges the sphere center is inside/outside.
        bool inside[3];
        for (int i = 0; i < 3; ++i)
        {
            inside[i] = (ExN[i].Dot(mSphere->Center) >= ExN[i].Dot(V[i]));
        }

        if (inside[0])
        {
            if (inside[1])
            {
                if (inside[2])
                {
                    // Triangle inside sphere.
                    return false;
                }
                else // !inside[2]
                {
                    // Potential intersection with edge <V2,V0>.
                    seg = Segment3<Real>(V[2], V[0]);
                    IntrSegment3Sphere3<Real> calc(seg, *mSphere);
                    if (calc.Find(tmax, velocity0, velocity1))
                    {
                        mContactTime = calc.GetContactTime();
                        mPoint = calc.GetPoint(0);
                        return true;
                    }
                    return false;
                }
            }
            else // !inside[1]
            {
                if (inside[2])
                {
                    // Potential intersection with edge <V1,V2>.
                    seg = Segment3<Real>(V[1], V[2]);
                    IntrSegment3Sphere3<Real> calc(seg, *mSphere);
                    if (calc.Find(tmax, velocity0, velocity1))
                    {
                        mContactTime = calc.GetContactTime();
                        mPoint = calc.GetPoint(0);
                        return true;
                    }
                    return false;
                }
                else // !inside[2]
                {
                    // Potential intersection with edges <V1,V2>, <V2,V0>.
                    return FindTriangleSphereCoplanarIntersection(2, V,
                        ExN[2], edges[2], tmax, velocity0, velocity1);
                }            
            }
        } 
        else // !inside[0]
        {
            if (inside[1])
            {
                if (inside[2])
                {
                    // Potential intersection with edge <V0,V1>.
                    seg = Segment3<Real>(V[0], V[1]);
                    IntrSegment3Sphere3<Real> calc(seg, *mSphere);
                    if (calc.Find(tmax, velocity0, velocity1))
                    {
                        mContactTime = calc.GetContactTime();
                        mPoint = calc.GetPoint(0);
                        return true;
                    }
                    return false;
                }
                else // !inside[2]
                {
                    // Potential intersection with edges <V2,V0>, <V0,V1>.
                    return FindTriangleSphereCoplanarIntersection(0, V,
                        ExN[0], edges[0], tmax, velocity0, velocity1);
                }
            }
            else // !inside[1]
            {
                if (inside[2])
                {
                    // Potential intersection with edges <V0,V1>, <V1,V2>.
                    return FindTriangleSphereCoplanarIntersection(1, V,
                        ExN[1], edges[1], tmax, velocity0, velocity1);
                }
                else // !inside[2]
                {
                    // We should not get here.
                    assertion(false, "Unexpected condition\n");
                    return false;
                }            
            }
        }
    }
    else
    {
        // Sphere does not currently intersect the plane of the triangle.

        // Sphere moving, triangle stationary.
        Vector3<Real> relVelocity = velocity1 - velocity0;

        // Find point of intersection of the sphere and the triangle
        // plane.  Where this point occurs on the plane relative to the
        // triangle determines the potential kind of intersection.

        normal.Normalize();

        // Point on sphere we care about intersecting the triangle plane.
        Vector3<Real> spherePoint;

        // On which side of the triangle is the sphere?
        if (NdC > NdT)
        {
            // Positive side.
            if (relVelocity.Dot(normal) >= (Real)0)
            {
                // Moving away, easy out.
                return false;
            }

            spherePoint = mSphere->Center - mSphere->Radius*normal;
        }
        else
        {
            // Negative side.
            if (relVelocity.Dot(normal) <= (Real)0)
            {
                // Moving away, easy out.
                return false;
            }

            spherePoint = mSphere->Center + mSphere->Radius*normal;
        }

        // Find intersection of velocity ray and triangle plane.

        // Project ray and plane onto the plane normal.
        Real plane = normal.Dot(V[0]);
        Real point = normal.Dot(spherePoint);
        Real vel = normal.Dot(relVelocity);
        Real time = (plane - point)/vel;

        // Where this intersects.
        Vector3<Real> intrPoint = spherePoint + time*relVelocity;

        // See which edges this intersection point is inside/outside.
        bool inside[3];
        for (int i = 0; i < 3; ++i)
        {
            inside[i] = (ExN[i].Dot(intrPoint) >=  ExN[i].Dot(V[i]));
        }

        if (inside[0])
        {
            if (inside[1])
            {
                if (inside[2])
                {
                    // Intersects face at time time.
                    if (time > tmax)
                    {
                        // Intersection after tMax.
                        return false;
                    }
                    else
                    {
                        // intrPoint is the point in space, assuming that 
                        // TriVel is 0.  Re-adjust the point to where it 
                        // should be, were it not.
                        mContactTime = time;
                        mPoint = intrPoint + time*velocity0;
                        return true;
                    }
                }
                else // !inside[2]
                {
                    // Potential intersection with edge <V2,V0>.
                    seg = Segment3<Real>(V[2], V[0]);
                    IntrSegment3Sphere3<Real> calc(seg, *mSphere);
                    if (calc.Find(tmax, velocity0, velocity1))
                    {
                        mContactTime = calc.GetContactTime();
                        mPoint = calc.GetPoint(0);
                        return true;
                    }
                    return false;
                }
            }
            else // !inside[1]
            {
                if (inside[2])
                {
                    // Potential intersection with edge <V1,V2>.
                    seg = Segment3<Real>(V[1], V[2]);
                    IntrSegment3Sphere3<Real> calc(seg, *mSphere);
                    if (calc.Find(tmax, velocity0, velocity1))
                    {
                        mContactTime = calc.GetContactTime();
                        mPoint = calc.GetPoint(0);
                        return true;
                    }
                    return false;
                }
                else // !inside[2]
                {
                    // Potential intersection with vertex V2.
                    return FindSphereVertexIntersection(V[2], tmax,
                        velocity1, velocity0);
                }            
            }
        } 
        else // !inside[0]
        {
            if (inside[1])
            {
                if (inside[2])
                {
                    // Potential intersection with edge <V0,V1>.
                    seg = Segment3<Real>(V[0], V[1]);
                    IntrSegment3Sphere3<Real> calc(seg, *mSphere);
                    if (calc.Find(tmax, velocity0, velocity1))
                    {
                        mContactTime = calc.GetContactTime();
                        mPoint = calc.GetPoint(0);
                        return true;
                    }
                    return false;
                }
                else // !inside[2]
                {
                    // Potential intersection with vertex V0.
                    return FindSphereVertexIntersection(V[0], tmax,
                        velocity1, velocity0);
                }
            }
            else // !inside[1]
            {
                if (inside[2])
                {
                    // Potential intersection with vertex V1.
                    return FindSphereVertexIntersection(V[1], tmax,
                        velocity1, velocity0);
                }
                else // !inside[2]
                {
                    // We should not get here.
                    assertion(false, "Unexpected condition\n");
                    return false;
                }            
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrTriangle3Sphere3<Real>::GetPoint () const
{
    return mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Sphere3<Real>::FindTriangleSphereCoplanarIntersection (
    int vertex, const Vector3<Real> V[3], const Vector3<Real>& sideNorm,
    const Vector3<Real>& side, Real tmax, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    // vertex is the "hinge" vertex that the two potential edges that can
    // be intersected by the sphere connect to, and it indexes into V.
    //
    // sideNorm is the normal of the plane formed by (vertex,vertex+1)
    // and the tri norm, passed so as not to recalculate

    // Check for intersections at time 0.
    Vector3<Real> dist = V[vertex] - mSphere->Center;
    if (dist.SquaredLength() < mSphere->Radius*mSphere->Radius)
    {
        // Already intersecting that vertex.
        mContactTime = (Real)0;
        return false;
    }

    // Tri stationary, sphere moving.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    // Check for easy out.
    if (relVelocity.Dot(dist) <= (Real)0)
    {
        // Moving away.
        return false;
    }

    // Find intersection of velocity ray and side normal.

    // Project ray and plane onto the plane normal.
    Real plane = sideNorm.Dot(V[vertex]);
    Real center = sideNorm.Dot(mSphere->Center);
    Real vel = sideNorm.Dot(relVelocity);
    Real factor = (plane - center)/vel;
    Vector3<Real> point = mSphere->Center + factor*relVelocity;

    // Find which side of the hinge vertex this lies by projecting both the
    // vertex and this new point onto the triangle edge (the same edge whose
    // "normal" was used to find this point).
    Real fvertex = side.Dot(V[vertex]);
    Real fpoint = side.Dot(point);
    Vector3<Real> end0 = V[vertex], end1;
    if (fpoint >= fvertex)
    {
        // Intersection with edge (vertex,vertex+1).
        end1 = V[(vertex+1) % 3];
    }
    else
    {
        // Intersection with edge (vertex-1,vertex).
        if (vertex != 0)
        {
            end1 = V[vertex-1];
        }
        else
        {
            end1 = V[2];
        }
    }
    Segment3<Real> seg(end0,end1);

    // This could be either an sphere-edge or a sphere-vertex intersection
    // (this test isn't enough to differentiate), so use the full-on
    // line-sphere test.
    IntrSegment3Sphere3<Real> calc(seg, *mSphere);
    if (calc.Find(tmax, velocity0, velocity1))
    {
        mContactTime = calc.GetContactTime();
        mPoint = calc.GetPoint(0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Sphere3<Real>::FindSphereVertexIntersection (
    const Vector3<Real>& vertex, Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    // Finds the time and place (and possible occurrence it may miss) of an
    // intersection between a sphere of fRadius at rkOrigin moving in rkDir
    // towards a vertex at vertex.

    Vector3<Real> relVelocity = velocity1 - velocity0;
    Vector3<Real> D = mSphere->Center - vertex;
    Vector3<Real> cross = D.Cross(relVelocity);
    Real rSqr = mSphere->Radius*mSphere->Radius;
    Real vsqr = relVelocity.SquaredLength();

    if (cross.SquaredLength() > rSqr*vsqr)
    {
        // The ray overshoots the sphere.
        return false;
    }

    // Find the time of intersection.
    Real dot = D.Dot(relVelocity);
    Real diff = D.SquaredLength() - rSqr;
    Real inv = Math<Real>::InvSqrt(Math<Real>::FAbs(dot*dot - vsqr*diff));

    mContactTime = diff*inv/((Real)1 - dot*inv);
    if (mContactTime > tmax)
    {
        // The intersection occurs after max time.
        return false;
    }

    // The intersection is a triangle vertex.
    mPoint = vertex + mContactTime*velocity0;
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrTriangle3Sphere3<float>;

template WM5_MATHEMATICS_ITEM
class IntrTriangle3Sphere3<double>;
//----------------------------------------------------------------------------
}
