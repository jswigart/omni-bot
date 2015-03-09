// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ConformalMap.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ConformalMap<Real>::ConformalMap (int numPoints,
    const Vector3<Real>* points, int numTriangles, const int* indices,
    int punctureTriangle)
{
    // Construct a vertex-triangle-edge representation of mesh.
    BasicMesh mesh(numPoints, points, numTriangles, indices);
    int numEdges = mesh.GetNumEdges();
    const BasicMesh::Edge* edges = mesh.GetEdges();
    const BasicMesh::Triangle* triangles = mesh.GetTriangles();

    mPlanes = new1<Vector2<Real> >(numPoints);
    mSpheres = new1<Vector3<Real> >(numPoints);

    // Construct sparse matrix A nondiagonal entries.
    typename LinearSystem<Real>::SparseMatrix AMat;
    int i, e, t, v0, v1, v2;
    Real value = (Real)0;
    for (e = 0; e < numEdges; ++e)
    {
        const BasicMesh::Edge& edge = edges[e];
        v0 = edge.V[0];
        v1 = edge.V[1];

        Vector3<Real> E0, E1;

        const BasicMesh::Triangle& T0 = triangles[edge.T[0]];
        for (i = 0; i < 3; ++i)
        {
            v2 = T0.V[i];
            if (v2 != v0 && v2 != v1)
            {
                E0 = points[v0] - points[v2];
                E1 = points[v1] - points[v2];
                value = E0.Dot(E1)/E0.Cross(E1).Length();
            }
        }

        const BasicMesh::Triangle& T1 = triangles[edge.T[1]];
        for (i = 0; i < 3; ++i)
        {
            v2 = T1.V[i];
            if (v2 != v0 && v2 != v1)
            {
                E0 = points[v0] - points[v2];
                E1 = points[v1] - points[v2];
                value += E0.Dot(E1)/E0.Cross(E1).Length();
            }
        }

        value *= -(Real)0.5;
        AMat[std::make_pair(v0, v1)] = value;
    }

    // Aonstruct sparse matrix A diagonal entries.
    Real* tmp = new1<Real>(numPoints);
    memset(tmp, 0, numPoints*sizeof(Real));
    typename LinearSystem<Real>::SparseMatrix::iterator iter = AMat.begin();
    typename LinearSystem<Real>::SparseMatrix::iterator end = AMat.end();
    for (/**/; iter != end; ++iter)
    {
        v0 = iter->first.first;
        v1 = iter->first.second;
        value = iter->second;
        assertion(v0 != v1, "Unexpected condition\n");
        tmp[v0] -= value;
        tmp[v1] -= value;
    }
    for (int v = 0; v < numPoints; ++v)
    {
        AMat[std::make_pair(v, v)] = tmp[v];
    }

    assertion(numPoints + numEdges == (int)AMat.size(),
        "Mismatch in sizes\n");

    // Construct column vector B (happens to be sparse).
    const BasicMesh::Triangle& tri = triangles[punctureTriangle];
    v0 = tri.V[0];
    v1 = tri.V[1];
    v2 = tri.V[2];
    Vector3<Real> V0 = points[v0];
    Vector3<Real> V1 = points[v1];
    Vector3<Real> V2 = points[v2];
    Vector3<Real> E10 = V1 - V0;
    Vector3<Real> E20 = V2 - V0;
    Vector3<Real> E12 = V1 - V2;
    Vector3<Real> cross = E20.Cross(E10);
    Real len10 = E10.Length();
    Real invLen10 = ((Real)1)/len10;
    Real twoArea = cross.Length();
    Real invLenCross = ((Real)1)/twoArea;
    Real invProd = invLen10*invLenCross;
    Real re0 = -invLen10;
    Real im0 = invProd*E12.Dot(E10);
    Real re1 = invLen10;
    Real im1 = invProd*E20.Dot(E10);
    Real re2 = (Real)0;
    Real im2 = -len10*invLenCross;

    // Solve sparse system for real parts.
    memset(tmp, 0, numPoints*sizeof(Real));
    tmp[v0] = re0;
    tmp[v1] = re1;
    tmp[v2] = re2;
    Real* result = new1<Real>(numPoints);
    bool solved = LinearSystem<Real>().SolveSymmetricCG(numPoints, AMat, tmp,
        result);
    assertion(solved, "Failed to solve linear system\n");
    WM5_UNUSED(solved);
    for (i = 0; i < numPoints; ++i)
    {
        mPlanes[i].X() = result[i];
    }

    // Solve sparse system for imaginary parts.
    memset(tmp, 0, numPoints*sizeof(Real));
    tmp[v0] = -im0;
    tmp[v1] = -im1;
    tmp[v2] = -im2;
    solved = LinearSystem<Real>().SolveSymmetricCG(numPoints, AMat, tmp,
        result);
    assertion(solved, "Failed to solve linear system\n");
    for (i = 0; i < numPoints; ++i)
    {
        mPlanes[i].Y() = result[i];
    }
    delete1(tmp);
    delete1(result);

    // Scale to [-1,1]^2 for numerical conditioning in later steps.
    Real fmin = mPlanes[0].X(), fmax = fmin;
    for (i = 0; i < numPoints; i++)
    {
        if (mPlanes[i].X() < fmin)
        {
            fmin = mPlanes[i].X();
        }
        else if (mPlanes[i].X() > fmax)
        {
            fmax = mPlanes[i].X();
        }
        if (mPlanes[i].Y() < fmin)
        {
            fmin = mPlanes[i].Y();
        }
        else if (mPlanes[i].Y() > fmax)
        {
            fmax = mPlanes[i].Y();
        }
    }
    Real halfRange = ((Real)0.5)*(fmax - fmin);
    Real invHalfRange = ((Real)1)/halfRange;
    for (i = 0; i < numPoints; ++i)
    {
        mPlanes[i].X() = -(Real)1 + invHalfRange*(mPlanes[i].X() - fmin);
        mPlanes[i].Y() = -(Real)1 + invHalfRange*(mPlanes[i].Y() - fmin);
    }

    // Map plane points to sphere using inverse stereographic projection.
    // The main issue is selecting a translation in (x,y) and a radius of
    // the projection sphere.  Both factors strongly influence the final
    // result.

    // Use the average as the south pole.  The points tend to be clustered
    // approximately in the middle of the conformally mapped punctured
    // triangle, so the average is a good choice to place the pole.
    Vector2<Real> origin((Real)0 ,(Real)0 );
    for (i = 0; i < numPoints; ++i)
    {
        origin += mPlanes[i];
    }
    origin /= (Real)numPoints;
    for (i = 0; i < numPoints; ++i)
    {
        mPlanes[i] -= origin;
    }

    mPlaneMin = mPlanes[0];
    mPlaneMax = mPlanes[0];
    for (i = 1; i < numPoints; ++i)
    {
        if (mPlanes[i].X() < mPlaneMin.X())
        {
            mPlaneMin.X() = mPlanes[i].X();
        }
        else if (mPlanes[i].X() > mPlaneMax.X())
        {
            mPlaneMax.X() = mPlanes[i].X();
        }

        if (mPlanes[i].Y() < mPlaneMin.Y())
        {
            mPlaneMin.Y() = mPlanes[i].Y();
        }
        else if (mPlanes[i].Y() > mPlaneMax.Y())
        {
            mPlaneMax.Y() = mPlanes[i].Y();
        }
    }

    // Select the radius of the sphere so that the projected punctured
    // triangle has an area whose fraction of total spherical area is the
    // same fraction as the area of the punctured triangle to the total area
    // of the original triangle mesh.
    Real twoTotalArea = (Real)0;
    for (t = 0; t < numTriangles; ++t)
    {
        const BasicMesh::Triangle& T0 = triangles[t];
        const Vector3<Real>& V0 = points[T0.V[0]];
        const Vector3<Real>& V1 = points[T0.V[1]];
        const Vector3<Real>& V2 = points[T0.V[2]];
        Vector3<Real> E0 = V1 - V0, E1 = V2 - V0;
        twoTotalArea += E0.Cross(E1).Length();
    }
    mRadius = ComputeRadius(mPlanes[v0], mPlanes[v1], mPlanes[v2],
        twoArea/twoTotalArea);
    Real radiusSqr = mRadius*mRadius;

    // Inverse stereographic projection to obtain sphere coordinates.  The
    // sphere is centered at the origin and has radius 1.
    for (i = 0; i < numPoints; i++)
    {
        Real rSqr = mPlanes[i].SquaredLength();
        Real mult = ((Real)1)/(rSqr + radiusSqr);
        Real x = ((Real)2)*mult*radiusSqr*mPlanes[i].X();
        Real y = ((Real)2)*mult*radiusSqr*mPlanes[i].Y();
        Real z = mult*mRadius*(rSqr - radiusSqr);
        mSpheres[i] = Vector3<Real>(x,y,z)/mRadius;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
ConformalMap<Real>::~ConformalMap ()
{
    delete1(mPlanes);
    delete1(mSpheres);
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>* ConformalMap<Real>::GetPlaneCoordinates () const
{
    return mPlanes;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& ConformalMap<Real>::GetPlaneMin () const
{
    return mPlaneMin;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& ConformalMap<Real>::GetPlaneMax () const
{
    return mPlaneMax;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* ConformalMap<Real>::GetSphereCoordinates () const
{
    return mSpheres;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ConformalMap<Real>::GetSphereRadius () const
{
    return mRadius;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ConformalMap<Real>::ComputeRadius (const Vector2<Real>& V0,
    const Vector2<Real>& V1, const Vector2<Real>& V2, Real areaFraction)
    const
{
    Real r0Sqr = V0.SquaredLength();
    Real r1Sqr = V1.SquaredLength();
    Real r2Sqr = V2.SquaredLength();
    Real diffR10 = r1Sqr - r0Sqr;
    Real diffR20 = r2Sqr - r0Sqr;
    Real diffX10 = V1.X() - V0.X();
    Real diffY10 = V1.Y() - V0.Y();
    Real diffX20 = V2.X() - V0.X();
    Real diffY20 = V2.Y() - V0.Y();
    Real diffRX10 = V1.X()*r0Sqr - V0.X()*r1Sqr;
    Real diffRY10 = V1.Y()*r0Sqr - V0.Y()*r1Sqr;
    Real diffRX20 = V2.X()*r0Sqr - V0.X()*r2Sqr;
    Real diffRY20 = V2.Y()*r0Sqr - V0.Y()*r2Sqr;

    Real c0 = diffR20*diffRY10 - diffR10*diffRY20;
    Real c1 = diffR20*diffY10 - diffR10*diffY20;
    Real d0 = diffR10*diffRX20 - diffR20*diffRX10;
    Real d1 = diffR10*diffX20 - diffR20*diffX10;
    Real e0 = diffRX10*diffRY20 - diffRX20*diffRY10;
    Real e1 = diffRX10*diffY20 - diffRX20*diffY10;
    Real e2 = diffX10*diffY20 - diffX20*diffY10;

    Polynomial1<Real> poly0(6);
    poly0[0] = (Real)0;
    poly0[1] = (Real)0;
    poly0[2] = e0*e0;
    poly0[3] = c0*c0 + d0*d0 + ((Real)2)*e0*e1;
    poly0[4] = ((Real)2)*(c0*c1 + d0*d1 + e0*e1) + e1*e1;
    poly0[5] = c1*c1 + d1*d1 + ((Real)2)*e1*e2;
    poly0[6] = e2*e2;

    Polynomial1<Real> qpoly0(1), qpoly1(1), qpoly2(1);
    qpoly0[0] = r0Sqr;
    qpoly0[1] = (Real)1;
    qpoly1[0] = r1Sqr;
    qpoly1[1] = (Real)1;
    qpoly2[0] = r2Sqr;
    qpoly2[1] = (Real)1;

    Real tmp = areaFraction*Math<Real>::PI;
    Real amp = tmp*tmp;

    Polynomial1<Real> poly1 = amp*qpoly0;
    poly1 = poly1*qpoly0;
    poly1 = poly1*qpoly0;
    poly1 = poly1*qpoly0;
    poly1 = poly1*qpoly1;
    poly1 = poly1*qpoly1;
    poly1 = poly1*qpoly2;
    poly1 = poly1*qpoly2;

    Polynomial1<Real> final = poly1 - poly0;
    assertion(final.GetDegree() <= 8, "Unexpected condition\n");

    // Bound a root near zero and apply bisection to find t.
    Real tmin = (Real)0, fmin = final(tmin);
    Real tmax = (Real)1, fmax = final(tmax);
    assertion(fmin > (Real)0 && fmax < (Real)0, "Unexpected condition\n");

    // Determine the number of iterations to get 'digits' of accuracy.
    const int digits = 6;
    Real tmp0 = Math<Real>::Log(tmax - tmin);
    Real tmp1 = ((Real)digits)*Math<Real>::Log((Real)10);
    Real arg = (tmp0 + tmp1)/Math<Real>::Log((Real)2);
    int maxIter = (int)(arg + (Real)0.5);
    Real tmid = (Real)0, fmid;
    for (int i = 0; i < maxIter; ++i)
    {
        tmid = ((Real)0.5)*(tmin + tmax);
        fmid = final(tmid);
        Real product = fmid*fmin;
        if (product < (Real)0)
        {
            tmax = tmid;
            fmax = fmid;
        }
        else
        {
            tmin = tmid;
            fmin = fmid;
        }
    }

    Real radius = Math<Real>::Sqrt(tmid);
    return radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ConformalMap<float>;

template WM5_MATHEMATICS_ITEM
class ConformalMap<double>;
//----------------------------------------------------------------------------
}
