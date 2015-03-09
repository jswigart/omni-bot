// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5SurfacePatch.h"
#include "Wm5Math.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, SurfacePatch);
WM5_IMPLEMENT_STREAM(SurfacePatch);
WM5_IMPLEMENT_ABSTRACT_FACTORY(SurfacePatch);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, SurfacePatch);

//----------------------------------------------------------------------------
SurfacePatch::SurfacePatch (float umin, float umax, float vmin, float vmax,
    bool rectangular)
    :
    mUMin(umin),
    mUMax(umax),
    mVMin(vmin),
    mVMax(vmax),
    mRectangular(rectangular)
{
}
//----------------------------------------------------------------------------
SurfacePatch::~SurfacePatch ()
{
}
//----------------------------------------------------------------------------
float SurfacePatch::GetUMin () const
{
    return mUMin;
}
//----------------------------------------------------------------------------
float SurfacePatch::GetUMax () const
{
    return mUMax;
}
//----------------------------------------------------------------------------
float SurfacePatch::GetVMin () const
{
    return mVMin;
}
//----------------------------------------------------------------------------
float SurfacePatch::GetVMax () const
{
    return mVMax;
}
//----------------------------------------------------------------------------
bool SurfacePatch::IsRectangular () const
{
    return mRectangular;
}
//----------------------------------------------------------------------------
AVector SurfacePatch::Tangent0 (float u, float v) const
{
    AVector tangent0 = PU(u, v);
    tangent0.Normalize();
    return tangent0;
}
//----------------------------------------------------------------------------
AVector SurfacePatch::Tangent1 (float u, float v) const
{
    AVector tangent0 = PU(u, v);
    AVector tangent1 = PV(u, v);
    tangent0.Normalize();
    AVector normal = tangent0.UnitCross(tangent1);
    tangent1 = normal.Cross(tangent0);
    return tangent1;
}
//----------------------------------------------------------------------------
AVector SurfacePatch::Normal (float u, float v) const
{
    AVector tangent0 = PU(u, v);
    AVector tangent1 = PV(u, v);
    tangent0.Normalize();  // Include this to be consistent with GetFrame.
    AVector normal = tangent0.UnitCross(tangent1);
    return normal;
}
//----------------------------------------------------------------------------
void SurfacePatch::GetFrame (float u, float v, APoint& position,
    AVector& tangent0, AVector& tangent1, AVector& normal) const
{
    position = P(u, v);

    tangent0 = PU(u, v);
    tangent1 = PV(u, v);
    tangent0.Normalize();
    normal = tangent0.UnitCross(tangent1);

    // The normalized first derivatives are not necessarily orthogonal.
    // Recompute T1 so that {T0,T1,N} is an orthonormal set.
    tangent1 = normal.Cross(tangent0);
}
//----------------------------------------------------------------------------
void SurfacePatch::ComputePrincipalCurvatureInfo (float u, float v,
    float& curv0, float& curv1, AVector& dir0, AVector& dir1)
{
    // Tangents:  T0 = dP/du = (x_u,y_u,z_u), T1 = dP/dv = (x_v,y_v,z_v)
    // Normal:    N = Cross(T0,T1)/Length(Cross(T0,T1))
    // Metric Tensor:    G = +-                      -+
    //                       | Dot(T0,T0)  Dot(T0,T1) |
    //                       | Dot(T1,T0)  Dot(T1,T1) |
    //                       +-                      -+
    //
    // Curvature Tensor:  B = +-                          -+
    //                        | -Dot(N,T0_u)  -Dot(N,T0_v) |
    //                        | -Dot(N,T1_u)  -Dot(N,T1_v) |
    //                        +-                          -+
    //
    // Principal curvatures k are the generalized eigenvalues of
    //
    //     Bw = kGw
    //
    // If k is a curvature and w=(a,b) is the corresponding solution to
    // Bw = kGw, then the principal direction as a 3D vector is d = a*U+b*V.
    //
    // Let k1 and k2 be the principal curvatures.  The mean curvature
    // is (k1+k2)/2 and the Gaussian curvature is k1*k2.

    // Compute the derivatives.
    AVector derU = PU(u, v);
    AVector derV = PV(u, v);
    AVector derUU = PUU(u, v);
    AVector derUV = PUV(u, v);
    AVector derVV = PVV(u, v);

    // Compute the metric tensor.
    float metricTensor[2][2];
    metricTensor[0][0] = derU.Dot(derU);
    metricTensor[0][1] = derU.Dot(derV);
    metricTensor[1][0] = metricTensor[0][1];
    metricTensor[1][1] = derV.Dot(derV);

    // Compute the curvature tensor.
    AVector normal = derU.UnitCross(derV);
    float curvatureTensor[2][2];
    curvatureTensor[0][0] = -normal.Dot(derUU);
    curvatureTensor[0][1] = -normal.Dot(derUV);
    curvatureTensor[1][0] = curvatureTensor[0][1];
    curvatureTensor[1][1] = -normal.Dot(derVV);

    // Characteristic polynomial is 0 = det(B-kG) = c2*k^2+c1*k+c0.
    float c0 = curvatureTensor[0][0]*curvatureTensor[1][1] -
        curvatureTensor[0][1]*curvatureTensor[1][0];
    float c1 = 2.0f*curvatureTensor[0][1]* metricTensor[0][1] -
        curvatureTensor[0][0]*metricTensor[1][1] -
        curvatureTensor[1][1]*metricTensor[0][0];
    float c2 = metricTensor[0][0]*metricTensor[1][1] -
        metricTensor[0][1]*metricTensor[1][0];

    // Principal curvatures are roots of characteristic polynomial.
    float temp = Mathf::Sqrt(Mathf::FAbs(c1*c1 - 4.0f*c0*c2));
    curv0 = -0.5f*(c1+temp);
    curv1 = 0.5f*(-c1+temp);

    // Principal directions are solutions to (B-kG)w = 0,
    // w1 = (b12-k1*g12,-(b11-k1*g11)) OR (b22-k1*g22,-(b12-k1*g12))
    float a0 = curvatureTensor[0][1] - curv0*metricTensor[0][1];
    float a1 = curv0*metricTensor[0][0] - curvatureTensor[0][0];
    float length = Mathf::Sqrt(a0*a0 + a1*a1);
    if (length >= Mathf::ZERO_TOLERANCE)
    {
        dir0 = a0*derU + a1*derV;
    }
    else
    {
        a0 = curvatureTensor[1][1] - curv0*metricTensor[1][1];
        a1 = curv0*metricTensor[0][1] - curvatureTensor[0][1];
        length = Mathf::Sqrt(a0*a0 + a1*a1);
        if (length >= Mathf::ZERO_TOLERANCE)
        {
            dir0 = a0*derU + a1*derV;
        }
        else
        {
            // Umbilic (surface is locally sphere, any direction principal).
            dir0 = derU;
        }
    }
    dir0.Normalize();

    // Second tangent is cross product of first tangent and normal.
    dir1 = dir0.Cross(normal);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SurfacePatch::SurfacePatch (LoadConstructor value)
    :
    Object(value),
    mUMin(0.0f),
    mUMax(0.0f),
    mVMin(0.0f),
    mVMax(0.0f),
    mRectangular(false)
{
}
//----------------------------------------------------------------------------
void SurfacePatch::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.Read(mUMin);
    source.Read(mUMax);
    source.Read(mVMin);
    source.Read(mVMax);
    source.ReadBool(mRectangular);

    WM5_END_DEBUG_STREAM_LOAD(SurfacePatch, source);
}
//----------------------------------------------------------------------------
void SurfacePatch::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void SurfacePatch::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool SurfacePatch::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void SurfacePatch::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.Write(mUMin);
    target.Write(mUMax);
    target.Write(mVMin);
    target.Write(mVMax);
    target.WriteBool(mRectangular);

    WM5_END_DEBUG_STREAM_SAVE(SurfacePatch, target);
}
//----------------------------------------------------------------------------
int SurfacePatch::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mUMin);
    size += sizeof(mUMax);
    size += sizeof(mVMin);
    size += sizeof(mVMax);
    size += WM5_BOOLSIZE(mRectangular);
    return size;
}
//----------------------------------------------------------------------------
