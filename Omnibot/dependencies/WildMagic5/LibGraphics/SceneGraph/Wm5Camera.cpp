// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/08/25)

#include "Wm5GraphicsPCH.h"
#include "Wm5Camera.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Camera);
WM5_IMPLEMENT_STREAM(Camera);
WM5_IMPLEMENT_FACTORY(Camera);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, Camera);

Camera::DepthType Camera::msDefaultDepthType = Camera::PM_DEPTH_ZERO_TO_ONE;

//----------------------------------------------------------------------------
Camera::Camera (bool isPerspective)
    :
    mIsPerspective(isPerspective),
    mDepthType(msDefaultDepthType)
#ifdef WM5_VALIDATE_CAMERA_FRAME_ONCE
    ,
    mValidateCameraFrame(true)
#endif
{
    SetFrame(APoint::ORIGIN, -AVector::UNIT_Z, AVector::UNIT_Y,
        AVector::UNIT_X);

    SetFrustum(90.0f, 1.0f, 1.0f, 10000.0f);

    SetPreViewMatrix(HMatrix::IDENTITY);
    SetPostProjectionMatrix(HMatrix::IDENTITY);
}
//----------------------------------------------------------------------------
Camera::~Camera ()
{
}
//----------------------------------------------------------------------------
void Camera::SetFrame (const APoint& position, const AVector& dVector,
    const AVector& uVector, const AVector& rVector)
{
    mPosition = position;
    SetAxes(dVector, uVector, rVector);
}
//----------------------------------------------------------------------------
void Camera::SetPosition (const APoint& position)
{
    mPosition = position;
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::SetAxes (const AVector& dVector, const AVector& uVector,
    const AVector& rVector)
{
    mDVector = dVector;
    mUVector = uVector;
    mRVector = rVector;

    const float epsilon = 0.01f;
    float det = mDVector.Dot(mUVector.Cross(mRVector));
    if (Mathf::FAbs(1.0f - det) > epsilon)
    {
#ifdef WM5_VALIDATE_CAMERA_FRAME_ONCE
        if (mValidateCameraFrame)
        {
            mValidateCameraFrame = false;

            float lenD = mDVector.Length();
            float lenU = mUVector.Length();
            float lenR = mRVector.Length();
            float dotDU = mDVector.Dot(mUVector);
            float dotDR = mDVector.Dot(mRVector);
            float dotUR = mUVector.Dot(mRVector);
            if (Mathf::FAbs(1.0f - lenD) > epsilon
            ||  Mathf::FAbs(1.0f - lenU) > epsilon
            ||  Mathf::FAbs(1.0f - lenR) > epsilon
            ||  Mathf::FAbs(dotDU) > epsilon
            ||  Mathf::FAbs(dotDR) > epsilon
            ||  Mathf::FAbs(dotUR) > epsilon)
            {
                assertion(false, "Camera frame is not orthonormal.\n");
            }
        }
#endif
        // The input vectors do not appear to form an orthonormal set.  Time
        // to renormalize.
        AVector::Orthonormalize(mDVector, mUVector, mRVector);
    }

    OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (float dMin, float dMax, float uMin, float uMax,
    float rMin, float rMax)
{
    mFrustum[VF_DMIN] = dMin;
    mFrustum[VF_DMAX] = dMax;
    mFrustum[VF_UMIN] = uMin;
    mFrustum[VF_UMAX] = uMax;
    mFrustum[VF_RMIN] = rMin;
    mFrustum[VF_RMAX] = rMax;

    OnFrustumChange();
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (const float* frustum)
{
    mFrustum[VF_DMIN] = frustum[VF_DMIN];
    mFrustum[VF_DMAX] = frustum[VF_DMAX];
    mFrustum[VF_UMIN] = frustum[VF_UMIN];
    mFrustum[VF_UMAX] = frustum[VF_UMAX];
    mFrustum[VF_RMIN] = frustum[VF_RMIN];
    mFrustum[VF_RMAX] = frustum[VF_RMAX];

    OnFrustumChange();
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (float upFovDegrees, float aspectRatio, float dMin,
    float dMax)
{
    float halfAngleRadians = 0.5f*upFovDegrees*Mathf::DEG_TO_RAD;
    mFrustum[VF_UMAX] = dMin*Mathf::Tan(halfAngleRadians);
    mFrustum[VF_RMAX] = aspectRatio*mFrustum[VF_UMAX];
    mFrustum[VF_UMIN] = -mFrustum[VF_UMAX];
    mFrustum[VF_RMIN] = -mFrustum[VF_RMAX];
    mFrustum[VF_DMIN] = dMin;
    mFrustum[VF_DMAX] = dMax;

    OnFrustumChange();
}
//----------------------------------------------------------------------------
void Camera::GetFrustum (float& dMin, float& dMax, float& uMin, float& uMax,
    float& rMin, float& rMax) const
{
    dMin = mFrustum[VF_DMIN];
    dMax = mFrustum[VF_DMAX];
    uMin = mFrustum[VF_UMIN];
    uMax = mFrustum[VF_UMAX];
    rMin = mFrustum[VF_RMIN];
    rMax = mFrustum[VF_RMAX];
}
//----------------------------------------------------------------------------
bool Camera::GetFrustum (float& upFovDegrees, float& aspectRatio,
    float& dMin, float& dMax) const
{
    if (mFrustum[VF_RMIN] == -mFrustum[VF_RMAX]
    &&  mFrustum[VF_UMIN] == -mFrustum[VF_UMAX])
    {
        float tmp = mFrustum[VF_UMAX]/mFrustum[VF_DMIN];
        upFovDegrees = 2.0f*Mathf::ATan(tmp)*Mathf::RAD_TO_DEG;
        aspectRatio = mFrustum[VF_RMAX]/mFrustum[VF_UMAX];
        dMin = mFrustum[VF_DMIN];
        dMax = mFrustum[VF_DMAX];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Camera::SetProjectionMatrix (const APoint& p00, const APoint& p10,
    const APoint& p11, const APoint& p01, float nearExtrude,
    float farExtrude)
{
    assertion(nearExtrude > 0.0f, "Invalid nearExtrude.\n");
    assertion(farExtrude > nearExtrude, "Invalid farExtrude.\n");

    // Compute the near face of the view volume.
    APoint q000 = APoint::ORIGIN + nearExtrude*(p00 - APoint::ORIGIN);
    APoint q100 = APoint::ORIGIN + nearExtrude*(p10 - APoint::ORIGIN);
    APoint q110 = APoint::ORIGIN + nearExtrude*(p11 - APoint::ORIGIN);
    APoint q010 = APoint::ORIGIN + nearExtrude*(p01 - APoint::ORIGIN);

    // Compute the far face of the view volume.
    APoint q001 = APoint::ORIGIN + farExtrude*(p00 - APoint::ORIGIN);
    APoint q101 = APoint::ORIGIN + farExtrude*(p10 - APoint::ORIGIN);
    APoint q111 = APoint::ORIGIN + farExtrude*(p11 - APoint::ORIGIN);
    APoint q011 = APoint::ORIGIN + farExtrude*(p01 - APoint::ORIGIN);

    // Compute the representation of q111.
    AVector u0 = q100 - q000;
    AVector u1 = q010 - q000;
    AVector u2 = q001 - q000;
    HMatrix M(u0, u1, u2, q000, true);
    HMatrix invM = M.Inverse();
    APoint a = invM*q111;

    // Compute the coeffients in the fractional linear transformation.
    //   y[i] = n[i]*x[i]/(d[0]*x[0] + d[1]*x[1] + d[2]*x[2] + d[3])
    float n0 = 2.0f*a[0];
    float n1 = 2.0f*a[1];
    float n2 = 2.0f*a[2];
    float d0 = +a[0] - a[1] - a[2] + 1.0f;
    float d1 = -a[0] + a[1] - a[2] + 1.0f;
    float d2 = -a[0] - a[1] + a[2] + 1.0f;
    float d3 = +a[0] + a[1] + a[2] - 1.0f;

    // Compute the perspective projection from the canonical cuboid to the
    // canonical cube [-1,1]^2 x [0,1].
    float n2divn0 = n2/n0;
    float n2divn1 = n2/n1;
    HMatrix project;
    project[0][0] = n2divn0*(2.0f*d3 + d0);
    project[0][1] = n2divn1*d1;
    project[0][2] = d2;
    project[0][3] = -n2;
    project[1][0] = n2divn0*d0;
    project[1][1] = n2divn1*(2.0f*d3 + d1);
    project[1][2] = d2;
    project[1][3] = -n2;

    if (mDepthType == PM_DEPTH_ZERO_TO_ONE)
    {
        project[2][0] = 0.0f;
        project[2][1] = 0.0f;
        project[2][2] = d3;
        project[2][3] = 0.0f;
    }
    else
    {
        project[2][0] = n2divn0*d0;
        project[2][1] = n2divn1*d1;
        project[2][2] = 2.0f*d3 + d2;
        project[2][3] = -n2;
    }

    project[3][0] = -n2divn0*d0;
    project[3][1] = -n2divn1*d1;
    project[3][2] = -d2;
    project[3][3] = n2;

    // The full projection requires mapping the extruded-quadrilateral view
    // volume to the canonical cuboid, which is then followed by the
    // perspective projection to the canonical cube.
    SetProjectionMatrix(project*invM);
}
//----------------------------------------------------------------------------
void Camera::SetPreViewMatrix (const HMatrix& preViewMatrix)
{
    mPreViewMatrix = preViewMatrix;
    mPreViewIsIdentity = (mPreViewMatrix == HMatrix::IDENTITY);
    UpdatePVMatrix();
}
//----------------------------------------------------------------------------
void Camera::SetPostProjectionMatrix (const HMatrix& postProjMatrix)
{
    mPostProjectionMatrix = postProjMatrix;
    mPostProjectionIsIdentity = (mPostProjectionMatrix == HMatrix::IDENTITY);
    UpdatePVMatrix();
}
//----------------------------------------------------------------------------
void Camera::ComputeBoundingAABB (int numVertices, const char* vertices,
    int stride, const HMatrix& worldMatrix, float& xmin, float& xmax,
    float& ymin, float& ymax)
{
    // Compute the current world-view-projection matrix.
    HMatrix vpMatrix = GetProjectionMatrix()*GetViewMatrix();
    if (!PostProjectionIsIdentity())
    {
        vpMatrix = GetPostProjectionMatrix()*vpMatrix;
    }
    HMatrix wvpMatrix = vpMatrix*worldMatrix;

    // Compute the bounding rectangle in normalized display coordinates.
    xmin = Mathf::MAX_REAL;
    xmax = -Mathf::MAX_REAL;
    ymin = Mathf::MAX_REAL;
    ymax = -Mathf::MAX_REAL;
    for (int i = 0; i < numVertices; ++i, vertices += stride)
    {
        const float* vertex = (const float*)vertices;
        HPoint pos(vertex[0], vertex[1], vertex[2], 1.0f);
        HPoint hpos = wvpMatrix*pos;
        float invW = 1.0f/hpos[3];
        float xNDC = hpos[0]*invW;
        float yNDC = hpos[1]*invW;
        if (xNDC < xmin)
        {
            xmin = xNDC;
        }
        if (xNDC > xmax)
        {
            xmax = xNDC;
        }
        if (yNDC < ymin)
        {
            ymin = yNDC;
        }
        if (yNDC > ymax)
        {
            ymax = yNDC;
        }
    }
}
//----------------------------------------------------------------------------
void Camera::OnFrameChange ()
{
    mViewMatrix[0][0] = mRVector[0];
    mViewMatrix[0][1] = mRVector[1];
    mViewMatrix[0][2] = mRVector[2];
    mViewMatrix[0][3] = -mPosition.Dot(mRVector);
    mViewMatrix[1][0] = mUVector[0];
    mViewMatrix[1][1] = mUVector[1];
    mViewMatrix[1][2] = mUVector[2];
    mViewMatrix[1][3] = -mPosition.Dot(mUVector);
    mViewMatrix[2][0] = mDVector[0];
    mViewMatrix[2][1] = mDVector[1];
    mViewMatrix[2][2] = mDVector[2];
    mViewMatrix[2][3] = -mPosition.Dot(mDVector);
    mViewMatrix[3][0] = 0.0f;
    mViewMatrix[3][1] = 0.0f;
    mViewMatrix[3][2] = 0.0f;
    mViewMatrix[3][3] = 1.0f;

    UpdatePVMatrix();
}
//----------------------------------------------------------------------------
void Camera::OnFrustumChange ()
{
    float dMin = mFrustum[VF_DMIN];
    float dMax = mFrustum[VF_DMAX];
    float uMin = mFrustum[VF_UMIN];
    float uMax = mFrustum[VF_UMAX];
    float rMin = mFrustum[VF_RMIN];
    float rMax = mFrustum[VF_RMAX];

    float invDDiff = 1.0f/(dMax - dMin);
    float invUDiff = 1.0f/(uMax - uMin);
    float invRDiff = 1.0f/(rMax - rMin);
    float sumRMinRMaxInvRDiff = (rMin + rMax)*invRDiff;
    float sumUMinUMaxInvUDiff = (uMin + uMax)*invUDiff;
    float sumDMinDMaxInvDDiff = (dMin + dMax)*invDDiff;

    if (mIsPerspective)
    {
        float twoDMinInvRDiff = 2.0f*dMin*invRDiff;
        float twoDMinInvUDiff = 2.0f*dMin*invUDiff;
        float dMaxInvDDiff = dMax*invDDiff;
        float dMinDMaxInvDDiff = dMin*dMaxInvDDiff;
        float twoDMinDMaxInvDDiff = 2.0f*dMinDMaxInvDDiff;

        if (mDepthType == PM_DEPTH_ZERO_TO_ONE)
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[0,1].
            HMatrix& proj0 = mProjectionMatrix[PM_DEPTH_ZERO_TO_ONE];
            proj0[0][0] = twoDMinInvRDiff;
            proj0[0][1] = 0.0f;
            proj0[0][2] = -sumRMinRMaxInvRDiff;
            proj0[0][3] = 0.0f;
            proj0[1][0] = 0.0f;
            proj0[1][1] = twoDMinInvUDiff;
            proj0[1][2] = -sumUMinUMaxInvUDiff;
            proj0[1][3] = 0.0f;
            proj0[2][0] = 0.0f;
            proj0[2][1] = 0.0f;
            proj0[2][2] = dMaxInvDDiff;
            proj0[2][3] = -dMinDMaxInvDDiff;
            proj0[3][0] = 0.0f;
            proj0[3][1] = 0.0f;
            proj0[3][2] = 1.0f;
            proj0[3][3] = 0.0f;
        }
        else
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[-1,1].
            HMatrix& proj1 = mProjectionMatrix[PM_DEPTH_MINUS_ONE_TO_ONE];
            proj1[0][0] = twoDMinInvRDiff;
            proj1[0][1] = 0.0f;
            proj1[0][2] = -sumRMinRMaxInvRDiff;
            proj1[0][3] = 0.0f;
            proj1[1][0] = 0.0f;
            proj1[1][1] = twoDMinInvUDiff;
            proj1[1][2] = -sumUMinUMaxInvUDiff;
            proj1[1][3] = 0.0f;
            proj1[2][0] = 0.0f;
            proj1[2][1] = 0.0f;
            proj1[2][2] = sumDMinDMaxInvDDiff;
            proj1[2][3] = -twoDMinDMaxInvDDiff;
            proj1[3][0] = 0.0f;
            proj1[3][1] = 0.0f;
            proj1[3][2] = 1.0f;
            proj1[3][3] = 0.0f;
        }
    }
    else
    {
        float twoInvRDiff = 2.0f*invRDiff;
        float twoInvUDiff = 2.0f*invUDiff;
        float twoInvDDiff = 2.0f*invDDiff;
        float dMinInvDDiff = dMin*invDDiff;

        if (mDepthType == PM_DEPTH_ZERO_TO_ONE)
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[0,1].
            HMatrix& proj0 = mProjectionMatrix[PM_DEPTH_ZERO_TO_ONE];
            proj0[0][0] = twoInvRDiff;
            proj0[0][1] = 0.0f;
            proj0[0][2] = 0.0f;
            proj0[0][3] = -sumRMinRMaxInvRDiff;
            proj0[1][0] = 0.0f;
            proj0[1][1] = twoInvUDiff;
            proj0[1][2] = 0.0f;
            proj0[1][3] = -sumUMinUMaxInvUDiff;
            proj0[2][0] = 0.0f;
            proj0[2][1] = 0.0f;
            proj0[2][2] = invDDiff;
            proj0[2][3] = -dMinInvDDiff;
            proj0[3][0] = 0.0f;
            proj0[3][1] = 0.0f;
            proj0[3][2] = 0.0f;
            proj0[3][3] = 1.0f;
        }
        else
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[-1,1].
            HMatrix& proj1 = mProjectionMatrix[PM_DEPTH_MINUS_ONE_TO_ONE];
            proj1[0][0] = twoInvRDiff;
            proj1[0][1] = 0.0f;
            proj1[0][2] = 0.0f;
            proj1[0][3] = -sumRMinRMaxInvRDiff;
            proj1[1][0] = 0.0f;
            proj1[1][1] = twoInvUDiff;
            proj1[1][2] = 0.0f;
            proj1[1][3] = -sumUMinUMaxInvUDiff;
            proj1[2][0] = 0.0f;
            proj1[2][1] = 0.0f;
            proj1[2][2] = twoInvDDiff;
            proj1[2][3] = -sumDMinDMaxInvDDiff;
            proj1[3][0] = 0.0f;
            proj1[3][1] = 0.0f;
            proj1[3][2] = 0.0f;
            proj1[3][3] = 1.0f;
        }
    }

    UpdatePVMatrix();
}
//----------------------------------------------------------------------------
void Camera::UpdatePVMatrix ()
{
    HMatrix& pMatrix = mProjectionMatrix[mDepthType];
    HMatrix& pvMatrix = mProjectionViewMatrix[mDepthType];

    pvMatrix = pMatrix*mViewMatrix;
    if (!mPostProjectionIsIdentity)
    {
        pvMatrix = mPostProjectionMatrix*pvMatrix;
    }
    if (!mPreViewIsIdentity)
    {
        pvMatrix = pvMatrix*mPreViewMatrix;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Camera::Camera (LoadConstructor value)
    :
    Object(value),
    mPosition(APoint::ORIGIN),
    mDVector(AVector::ZERO),
    mUVector(AVector::ZERO),
    mRVector(AVector::ZERO),
    mViewMatrix(HMatrix::ZERO),
    mPreViewMatrix(HMatrix::ZERO),
    mPreViewIsIdentity(false),
    mPostProjectionMatrix(HMatrix::ZERO),
    mPostProjectionIsIdentity(false),
    mIsPerspective(false)
{
    for (int i = 0; i < VF_QUANTITY; ++i)
    {
        mFrustum[i] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Camera::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadAggregate(mPosition);
    source.ReadAggregate(mDVector);
    source.ReadAggregate(mUVector);
    source.ReadAggregate(mRVector);
    source.ReadVV(6, mFrustum);
    source.ReadAggregate(mPreViewMatrix);
    source.ReadBool(mPreViewIsIdentity);
    source.ReadAggregate(mPostProjectionMatrix);
    source.ReadBool(mPostProjectionIsIdentity);
    source.ReadBool(mIsPerspective);

    // Compute mViewMatrix and mProjectionMatrix[].
    OnFrameChange();
    OnFrustumChange();

    WM5_END_DEBUG_STREAM_LOAD(Camera, source);
}
//----------------------------------------------------------------------------
void Camera::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void Camera::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool Camera::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void Camera::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);
    
    Object::Save(target);

    target.WriteAggregate(mPosition);
    target.WriteAggregate(mDVector);
    target.WriteAggregate(mUVector);
    target.WriteAggregate(mRVector);
    target.WriteN(6, mFrustum);
    target.WriteAggregate(mPreViewMatrix);
    target.WriteBool(mPreViewIsIdentity);
    target.WriteAggregate(mPostProjectionMatrix);
    target.WriteBool(mPostProjectionIsIdentity);
    target.WriteBool(mIsPerspective);

    // mViewMatrix and mProjectionMatrix[] are derived quantities, so there
    // is no need to save them.

    WM5_END_DEBUG_STREAM_SAVE(Camera, target);
}
//----------------------------------------------------------------------------
int Camera::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mPosition);
    size += sizeof(mDVector);
    size += sizeof(mUVector);
    size += sizeof(mRVector);
    size += 6*sizeof(mFrustum[0]);
    size += sizeof(mPreViewMatrix);
    size += WM5_BOOLSIZE(mPreViewIsIdentity);
    size += sizeof(mPostProjectionMatrix);
    size += WM5_BOOLSIZE(mPostProjectionIsIdentity);
    size += WM5_BOOLSIZE(mIsPerspective);
    return size;
}
//----------------------------------------------------------------------------
