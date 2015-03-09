// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5BSplineSurfacePatch.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, SurfacePatch, BSplineSurfacePatch);
WM5_IMPLEMENT_STREAM(BSplineSurfacePatch);
WM5_IMPLEMENT_FACTORY(BSplineSurfacePatch);
WM5_IMPLEMENT_DEFAULT_NAMES(SurfacePatch, BSplineSurfacePatch);

//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int numUCtrlPoints,
    int numVCtrlPoints, Vector3f** ctrlPoints, int uDegree, int vDegree,
    bool uLoop, bool vLoop, bool uOpen, bool vOpen)
    :
    SurfacePatch(0.0f, 1.0f, 0.0f, 1.0f, true),
    mConstructor(1)
{
    mPatch = new0 BSplineRectanglef(numUCtrlPoints, numVCtrlPoints,
        ctrlPoints, uDegree, vDegree, uLoop, vLoop, uOpen, vOpen);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int numUCtrlPoints,
    int numVCtrlPoints, Vector3f** ctrlPoints, int uDegree, int vDegree,
    bool uLoop, bool vLoop, bool uOpen, float* vKnots)
    :
    SurfacePatch(0.0f, 1.0f, 0.0f, 1.0f, true),
    mConstructor(2)
{
    mPatch = new0 BSplineRectanglef(numUCtrlPoints, numVCtrlPoints,
        ctrlPoints, uDegree, vDegree, uLoop, vLoop, uOpen, vKnots);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int numUCtrlPoints,
    int numVCtrlPoints, Vector3f** ctrlPoints, int uDegree, int vDegree,
    bool uLoop, bool vLoop, float* uKnots, bool vOpen)
    :
    SurfacePatch(0.0f, 1.0f, 0.0f, 1.0f, true),
    mConstructor(3)
{
    mPatch = new0 BSplineRectanglef(numUCtrlPoints, numVCtrlPoints,
        ctrlPoints, uDegree, vDegree, uLoop, vLoop, uKnots, vOpen);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int numUCtrlPoints,
    int numVCtrlPoints, Vector3f** ctrlPoints, int uDegree, int vDegree,
    bool uLoop, bool vLoop, float* uKnots, float* vKnots)
    :
    SurfacePatch(0.0f, 1.0f, 0.0f, 1.0f, true),
    mConstructor(4)
{
    mPatch = new0 BSplineRectanglef(numUCtrlPoints, numVCtrlPoints,
        ctrlPoints, uDegree, vDegree, uLoop, vLoop, uKnots, vKnots);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::~BSplineSurfacePatch ()
{
    delete1(mPatch);
}
//----------------------------------------------------------------------------
APoint BSplineSurfacePatch::P (float u, float v) const
{
    Vector3f p = mPatch->P(u, v);
    return APoint(p[0], p[1], p[2]);
}
//----------------------------------------------------------------------------
AVector BSplineSurfacePatch::PU (float u, float v) const
{
    Vector3f pu = mPatch->PU(u, v);
    return AVector(pu[0], pu[1], pu[2]);
}
//----------------------------------------------------------------------------
AVector BSplineSurfacePatch::PV (float u, float v) const
{
    Vector3f pv = mPatch->PV(u, v);
    return AVector(pv[0], pv[1], pv[2]);
}
//----------------------------------------------------------------------------
AVector BSplineSurfacePatch::PUU (float u, float v) const
{
    Vector3f puu = mPatch->PUU(u, v);
    return AVector(puu[0], puu[1], puu[2]);
}
//----------------------------------------------------------------------------
AVector BSplineSurfacePatch::PUV (float u, float v) const
{
    Vector3f puv = mPatch->PUV(u, v);
    return AVector(puv[0], puv[1], puv[2]);
}
//----------------------------------------------------------------------------
AVector BSplineSurfacePatch::PVV (float u, float v) const
{
    Vector3f pvv = mPatch->PVV(u, v);
    return AVector(pvv[0], pvv[1], pvv[2]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (LoadConstructor value)
    :
    SurfacePatch(value),
    mConstructor(0),
    mPatch(0)
{
}
//----------------------------------------------------------------------------
void BSplineSurfacePatch::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    SurfacePatch::Load(source);

    source.Read(mConstructor);

    int numCtrlPoints0, numCtrlPoints1;
    source.Read(numCtrlPoints0);
    source.Read(numCtrlPoints1);
    Vector3f** ctrl = new2<Vector3f>(numCtrlPoints1, numCtrlPoints0);
    int i0, i1;
    for (i0 = 0; i0 < numCtrlPoints0; ++i0)
    {
        for (i1 = 0; i1 < numCtrlPoints1; ++i1)
        {
            source.ReadAggregate(ctrl[i0][i1]);
        }
    }

    int degree0, degree1;
    source.Read(degree0);
    source.Read(degree1);

    bool loop0, loop1;
    source.ReadBool(loop0);
    source.ReadBool(loop1);

    bool open0, open1;
    float* knot0;
    float* knot1;
    int numKnots0, numKnots1;

    switch (mConstructor)
    {
    case 1:
        source.ReadBool(open0);
        source.ReadBool(open1);

        mPatch = new0 BSplineRectanglef(numCtrlPoints0, numCtrlPoints1,
            ctrl, degree0, degree1, loop0, loop1, open0, open1);
        break;
    case 2:
        source.ReadBool(open0);
        numKnots1 = numCtrlPoints1 - degree1 - 1;
        source.ReadVR(numKnots1, knot1);

        mPatch = new0 BSplineRectanglef(numCtrlPoints0, numCtrlPoints1,
            ctrl, degree0, degree1, loop0, loop1, open0, knot1);

        delete1(knot1);
        break;
    case 3:
        numKnots0 = numCtrlPoints0 - degree0 - 1;
        source.ReadVR(numKnots0, knot0);
        source.ReadBool(open1);

        mPatch = new0 BSplineRectanglef(numCtrlPoints0, numCtrlPoints1,
            ctrl, degree0, degree1, loop0, loop1, knot0, open1);

        delete1(knot0);
        break;
    case 4:
        numKnots0 = numCtrlPoints0 - degree0 - 1;
        source.ReadVR(numKnots0, knot0);
        numKnots1 = numCtrlPoints1 - degree1 - 1;
        source.ReadVR(numKnots1, knot1);

        mPatch = new0 BSplineRectanglef(numCtrlPoints0, numCtrlPoints1,
            ctrl, degree0, degree1, loop0, loop1, knot0, knot1);

        delete1(knot0);
        delete1(knot1);
        break;
    default:
        assertion(false, "Unexpected condition\n");
    }

    WM5_END_DEBUG_STREAM_LOAD(BSplineSurfacePatch, source);
}
//----------------------------------------------------------------------------
void BSplineSurfacePatch::Link (InStream& source)
{
    SurfacePatch::Link(source);
}
//----------------------------------------------------------------------------
void BSplineSurfacePatch::PostLink ()
{
    SurfacePatch::PostLink();
}
//----------------------------------------------------------------------------
bool BSplineSurfacePatch::Register (OutStream& target) const
{
    return SurfacePatch::Register(target);
}
//----------------------------------------------------------------------------
void BSplineSurfacePatch::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    SurfacePatch::Save(target);

    target.Write(mConstructor);

    int numCtrlPoints0 = mPatch->GetNumCtrlPoints(0);
    int numCtrlPoints1 = mPatch->GetNumCtrlPoints(1);
    target.Write(numCtrlPoints0);
    target.Write(numCtrlPoints1);
    int i0, i1;
    for (i0 = 0; i0 < numCtrlPoints0; ++i0)
    {
        for (i1 = 0; i1 < numCtrlPoints1; ++i1)
        {
            target.WriteAggregate(mPatch->GetControlPoint(i0, i1));
        }
    }

    int degree0 = mPatch->GetDegree(0);
    int degree1 = mPatch->GetDegree(1);
    target.Write(degree0);
    target.Write(degree1);

    bool loop0 = mPatch->IsLoop(0);
    bool loop1 = mPatch->IsLoop(1);
    target.WriteBool(loop0);
    target.WriteBool(loop1);

    bool open0, open1;
    int numKnots0, numKnots1;

    switch (mConstructor)
    {
    case 1:
        open0 = mPatch->IsOpen(0);
        target.WriteBool(open0);
        open1 = mPatch->IsOpen(1);
        target.WriteBool(open1);
        break;
    case 2:
        open0 = mPatch->IsOpen(0);
        target.WriteBool(open0);
        numKnots1 = numCtrlPoints1 - degree1 - 1;
        for (i1 = 0; i1 < numKnots1; ++i1)
        {
            target.Write(mPatch->GetKnot(1, i1));
        }
        break;
    case 3:
        numKnots0 = numCtrlPoints0 - degree0 - 1;
        for (i0 = 0; i0 < numKnots0; ++i0)
        {
            target.Write(mPatch->GetKnot(0, i0));
        }

        open1 = mPatch->IsOpen(1);
        target.WriteBool(open1);
        break;
    case 4:
        numKnots0 = numCtrlPoints0 - degree0 - 1;
        for (i0 = 0; i0 < numKnots0; ++i0)
        {
            target.Write(mPatch->GetKnot(0, i0));
        }

        numKnots1 = numCtrlPoints1 - degree1 - 1;
        for (i1 = 0; i1 < numKnots1; ++i1)
        {
            target.Write(mPatch->GetKnot(1, i1));
        }
        break;
    default:
        assertion(false, "Unexpected condition\n");
    }

    WM5_END_DEBUG_STREAM_SAVE(SurfacePatch, target);
}
//----------------------------------------------------------------------------
int BSplineSurfacePatch::GetStreamingSize () const
{
    int size = SurfacePatch::GetStreamingSize();
    size += sizeof(mConstructor);

    int numCtrlPoints0 = mPatch->GetNumCtrlPoints(0);
    int numCtrlPoints1 = mPatch->GetNumCtrlPoints(1);
    int degree0 = mPatch->GetDegree(0);
    int degree1 = mPatch->GetDegree(1);

    size += sizeof(numCtrlPoints0);
    size += sizeof(numCtrlPoints1);
    size += sizeof(degree0);
    size += sizeof(degree1);
    size += 2*WM5_BOOLSIZE(bool);  // loop0, loop1

    switch (mConstructor)
    {
    case 1:
        size += 2*WM5_BOOLSIZE(bool);  // open0, open1
        break;
    case 2:
        size += WM5_BOOLSIZE(bool);  // open0
        size += (numCtrlPoints1 - degree1 - 1)*sizeof(float);  // knot1[]
        break;
    case 3:
        size += (numCtrlPoints0 - degree0 - 1)*sizeof(float);  // knot0[]
        size += WM5_BOOLSIZE(bool);  // open1
        break;
    case 4:
        size += (numCtrlPoints0 - degree0 - 1)*sizeof(float);  // knot0[]
        size += (numCtrlPoints1 - degree1 - 1)*sizeof(float);  // knot1[]
        break;
    default:
        assertion(false, "Unexpected condition\n");
    }

    return size;
}
//----------------------------------------------------------------------------
