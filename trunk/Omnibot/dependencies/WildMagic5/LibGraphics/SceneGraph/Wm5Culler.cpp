// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/05/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Culler.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Culler::Culler (const Camera* camera)
    :
    mCamera(camera),
    mPlaneQuantity(6)
{
    // The data members mFrustum, mPlane, and mPlaneState are
    // uninitialized.  They are initialized in the GetVisibleSet call.
}
//----------------------------------------------------------------------------
Culler::~Culler ()
{
}
//----------------------------------------------------------------------------
void Culler::SetFrustum (const float* frustum)
{
    if (!mCamera)
    {
        assertion(false, "SetFrustum requires the existence of a camera\n");
        return;
    }

    // Copy the frustum values.
    mFrustum[Camera::VF_DMIN] = frustum[Camera::VF_DMIN];
    mFrustum[Camera::VF_DMAX] = frustum[Camera::VF_DMAX];
    mFrustum[Camera::VF_UMIN] = frustum[Camera::VF_UMIN];
    mFrustum[Camera::VF_UMAX] = frustum[Camera::VF_UMAX];
    mFrustum[Camera::VF_RMIN] = frustum[Camera::VF_RMIN];
    mFrustum[Camera::VF_RMAX] = frustum[Camera::VF_RMAX];

    float dMin2 = mFrustum[Camera::VF_DMIN]*mFrustum[Camera::VF_DMIN];
    float uMin2 = mFrustum[Camera::VF_UMIN]*mFrustum[Camera::VF_UMIN];
    float uMax2 = mFrustum[Camera::VF_UMAX]*mFrustum[Camera::VF_UMAX];
    float rMin2 = mFrustum[Camera::VF_RMIN]*mFrustum[Camera::VF_RMIN];
    float rMax2 = mFrustum[Camera::VF_RMAX]*mFrustum[Camera::VF_RMAX];

    // Get the camera coordinate frame.
    APoint position = mCamera->GetPosition();
    AVector dVector = mCamera->GetDVector();
    AVector uVector = mCamera->GetUVector();
    AVector rVector = mCamera->GetRVector();
    float dirDotEye = position.Dot(dVector);

    // Update the near plane.
    mPlane[Camera::VF_DMIN].SetNormal(dVector);
    mPlane[Camera::VF_DMIN].SetConstant(
        dirDotEye + mFrustum[Camera::VF_DMIN]);

    // Update the far plane.
    mPlane[Camera::VF_DMAX].SetNormal(-dVector);
    mPlane[Camera::VF_DMAX].SetConstant(
        -(dirDotEye + mFrustum[Camera::VF_DMAX]));

    // Update the bottom plane
    float invLength = Mathf::InvSqrt(dMin2 + uMin2);
    float c0 = -mFrustum[Camera::VF_UMIN]*invLength;  // D component
    float c1 = +mFrustum[Camera::VF_DMIN]*invLength;  // U component
    AVector normal = c0*dVector + c1*uVector;
    float constant = position.Dot(normal);
    mPlane[Camera::VF_UMIN].SetNormal(normal);
    mPlane[Camera::VF_UMIN].SetConstant(constant);

    // Update the top plane.
    invLength = Mathf::InvSqrt(dMin2 + uMax2);
    c0 = +mFrustum[Camera::VF_UMAX]*invLength;  // D component
    c1 = -mFrustum[Camera::VF_DMIN]*invLength;  // U component
    normal = c0*dVector + c1*uVector;
    constant = position.Dot(normal);
    mPlane[Camera::VF_UMAX].SetNormal(normal);
    mPlane[Camera::VF_UMAX].SetConstant(constant);

    // Update the left plane.
    invLength = Mathf::InvSqrt(dMin2 + rMin2);
    c0 = -mFrustum[Camera::VF_RMIN]*invLength;  // D component
    c1 = +mFrustum[Camera::VF_DMIN]*invLength;  // R component
    normal = c0*dVector + c1*rVector;
    constant = position.Dot(normal);
    mPlane[Camera::VF_RMIN].SetNormal(normal);
    mPlane[Camera::VF_RMIN].SetConstant(constant);

    // Update the right plane.
    invLength = Mathf::InvSqrt(dMin2 + rMax2);
    c0 = +mFrustum[Camera::VF_RMAX]*invLength;  // D component
    c1 = -mFrustum[Camera::VF_DMIN]*invLength;  // R component
    normal = c0*dVector + c1*rVector;
    constant = position.Dot(normal);
    mPlane[Camera::VF_RMAX].SetNormal(normal);
    mPlane[Camera::VF_RMAX].SetConstant(constant);

    // All planes are active initially.
    mPlaneState = 0xFFFFFFFF;
}
//----------------------------------------------------------------------------
void Culler::Insert (Spatial* visible)
{
    mVisibleSet.Insert(visible);
}
//----------------------------------------------------------------------------
bool Culler::IsVisible (const Bound& bound)
{
    if (bound.GetRadius() == 0.0f)
    {
        // The node is a dummy node and cannot be visible.
        return false;
    }

    // Start with the last pushed plane, which is potentially the most
    // restrictive plane.
    int index = mPlaneQuantity - 1;
    unsigned int mask = (1 << index);

    for (int i = 0; i < mPlaneQuantity; ++i, --index, mask >>= 1)
    {
        if (mPlaneState & mask)
        {
            int side = bound.WhichSide(mPlane[index]);

            if (side < 0)
            {
                // The object is on the negative side of the plane, so
                // cull it.
                return false;
            }

            if (side > 0)
            {
                // The object is on the positive side of plane.  There is
                // no need to compare subobjects against this plane, so
                // mark it as inactive.
                mPlaneState &= ~mask;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Culler::IsVisible (int numVertices, const APoint* vertices,
    bool ignoreNearPlane)
{
    // The Boolean variable ignoreNearPlane should be set to 'true' when
    // the test polygon is a portal.  This avoids the situation when the
    // portal is in the view pyramid (eye+left/right/top/bottom), but is
    // between the eye and near plane.  In such a situation you do not want
    // the portal system to cull the portal.  This situation typically occurs
    // when the camera moves through the portal from current region to
    // adjacent region.

    // Start with the last pushed plane, which is potentially the most
    // restrictive plane.
    int index = mPlaneQuantity - 1;
    for (int i = 0; i < mPlaneQuantity; ++i, --index)
    {
        HPlane& plane = mPlane[index];
        if (ignoreNearPlane && index == Camera::VF_DMIN)
        {
            continue;
        }

        int j;
        for (j = 0; j < numVertices; ++j)
        {
            int side = plane.WhichSide(vertices[j]);
            if (side >= 0)
            {
                // The polygon is not totally outside this plane.
                break;
            }
        }

        if (j == numVertices)
        {
            // The polygon is totally outside this plane.
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
int Culler::WhichSide (const HPlane& plane) const
{
    // The plane is N*(X-C) = 0 where the * indicates dot product.  The signed
    // distance from the camera location E to the plane is N*(E-C).
    float NdEmC = plane.DistanceTo(mCamera->GetPosition());

    AVector normal = plane.GetNormal();
    float NdD = normal.Dot(mCamera->GetDVector());
    float NdU = normal.Dot(mCamera->GetUVector());
    float NdR = normal.Dot(mCamera->GetRVector());
    float FdN = mFrustum[Camera::VF_DMAX]/mFrustum[Camera::VF_DMIN];

    int positive = 0, negative = 0;
    float sgnDist;

    // Check near-plane vertices.
    float PDMin = mFrustum[Camera::VF_DMIN]*NdD;
    float NUMin = mFrustum[Camera::VF_UMIN]*NdU;
    float NUMax = mFrustum[Camera::VF_UMAX]*NdU;
    float NRMin = mFrustum[Camera::VF_RMIN]*NdR;
    float NRMax = mFrustum[Camera::VF_RMAX]*NdR;

    // V = E + dmin*D + umin*U + rmin*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umin*(N*U) + rmin*(N*R)
    sgnDist = NdEmC + PDMin + NUMin + NRMin;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    // V = E + dmin*D + umin*U + rmax*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umin*(N*U) + rmax*(N*R)
    sgnDist = NdEmC + PDMin + NUMin + NRMax;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    // V = E + dmin*D + umax*U + rmin*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umax*(N*U) + rmin*(N*R)
    sgnDist = NdEmC + PDMin + NUMax + NRMin;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    // V = E + dmin*D + umax*U + rmax*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umax*(N*U) + rmax*(N*R)
    sgnDist = NdEmC + PDMin + NUMax + NRMax;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    // check far-plane vertices (s = dmax/dmin)
    float PDMax = mFrustum[Camera::VF_DMAX]*NdD;
    float FUMin = FdN*NUMin;
    float FUMax = FdN*NUMax;
    float FRMin = FdN*NRMin;
    float FRMax = FdN*NRMax;

    // V = E + dmax*D + umin*U + rmin*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umin*(N*U) + s*rmin*(N*R)
    sgnDist = NdEmC + PDMax + FUMin + FRMin;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    // V = E + dmax*D + umin*U + rmax*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umin*(N*U) + s*rmax*(N*R)
    sgnDist = NdEmC + PDMax + FUMin + FRMax;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    // V = E + dmax*D + umax*U + rmin*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umax*(N*U) + s*rmin*(N*R)
    sgnDist = NdEmC + PDMax + FUMax + FRMin;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    // V = E + dmax*D + umax*U + rmax*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umax*(N*U) + s*rmax*(N*R)
    sgnDist = NdEmC + PDMax + FUMax + FRMax;
    if (sgnDist > 0.0f)
    {
        positive++;
    }
    else if (sgnDist < 0.0f)
    {
        negative++;
    }

    if (positive > 0)
    {
        if (negative > 0)
        {
            // Frustum straddles the plane.
            return 0;
        }

        // Frustum is fully on the positive side.
        return +1;
    }

    // Frustum is fully on the negative side.
    return -1;
}
//----------------------------------------------------------------------------
void Culler::ComputeVisibleSet (Spatial* scene)
{
    if (mCamera && scene)
    {
        SetFrustum(mCamera->GetFrustum());
        mVisibleSet.Clear();
        scene->OnGetVisibleSet(*this, false);
    }
    else
    {
        assertion(false, "A camera and a scene are required for culling\n");
    }
}
//----------------------------------------------------------------------------
