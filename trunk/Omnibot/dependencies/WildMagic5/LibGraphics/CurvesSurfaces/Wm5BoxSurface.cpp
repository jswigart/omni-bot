// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/06/28)

#include "Wm5GraphicsPCH.h"
#include "Wm5BoxSurface.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, BoxSurface);
WM5_IMPLEMENT_STREAM(BoxSurface);
WM5_IMPLEMENT_FACTORY(BoxSurface);
WM5_IMPLEMENT_DEFAULT_NAMES(Node, BoxSurface);

//----------------------------------------------------------------------------
BoxSurface::BoxSurface (BSplineVolumef* volume, int numUSamples,
    int numVSamples, int numWSamples, VertexFormat* vformat[6])
    :
    mVolume(volume),
    mNumUSamples(numUSamples),
    mNumVSamples(numVSamples),
    mNumWSamples(numWSamples),
    mDoSort(false)
{
    int permute[3];
    TriMesh* mesh;

    // u faces
    permute[0] = 1;
    permute[1] = 2;
    permute[2] = 0;

    // u = 0
    mesh = CreateFace(mNumWSamples, mNumVSamples, vformat[0], false, 0.0f,
        permute);
    mesh->SetName("u0");
    AttachChild(mesh);

    // u = 1
    mesh = CreateFace(mNumWSamples, mNumVSamples, vformat[1], true, 1.0f,
        permute);
    mesh->SetName("u1");
    AttachChild(mesh);

    // v faces
    permute[0] = 0;
    permute[1] = 2;
    permute[2] = 1;

    // v = 0
    mesh = CreateFace(mNumWSamples, mNumUSamples, vformat[2], true, 0.0f,
        permute);
    mesh->SetName("v0");
    AttachChild(mesh);

    // v = 1
    mesh = CreateFace(mNumWSamples, mNumUSamples, vformat[3], false, 1.0f,
        permute);
    mesh->SetName("v1");
    AttachChild(mesh);

    // w faces
    permute[0] = 0;
    permute[1] = 1;
    permute[2] = 2;

    // w = 0
    mesh = CreateFace(mNumVSamples, mNumUSamples, vformat[4], false, 0.0f,
        permute);
    mesh->SetName("w0");
    AttachChild(mesh);

    // w = 1
    mesh = CreateFace(mNumVSamples, mNumUSamples, vformat[5], true, 1.0f,
        permute);
    mesh->SetName("w1");
    AttachChild(mesh);
}
//----------------------------------------------------------------------------
BoxSurface::~BoxSurface ()
{
    delete0(mVolume);
}
//----------------------------------------------------------------------------
TriMesh* BoxSurface::CreateFace (int numRows, int numCols,
    VertexFormat* vformat, bool ccw, float faceValue, int permute[3])
{
    int numVertices = numRows*numCols;
    int vstride = vformat->GetStride();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    float param[3];
    param[permute[2]] = faceValue;
    float sign = (ccw ? 1.0f : -1.0f);
    float rowFactor = 1.0f/(float)(numRows - 1);
    float colFactor = 1.0f/(float)(numCols - 1);
    int row, col, i;
    for (row = 0, i = 0; row < numRows; ++row)
    {
        param[permute[1]] = row*rowFactor;
        for (col = 0; col < numCols; ++col, ++i)
        {
            param[permute[0]] = col*colFactor;
            vba.Position<Vector3f>(i) = mVolume->GetPosition(param);

            if (vba.HasNormal())
            {
                Vector3f cDer = mVolume->GetDerivative(permute[0], param);
                Vector3f rDer = mVolume->GetDerivative(permute[1], param);
                vba.Normal<Vector3f>(i) = sign*cDer.UnitCross(rDer);
            }

            const int numUnits = VertexFormat::AM_MAX_TCOORD_UNITS;
            for (int unit = 0; unit < numUnits; ++unit)
            {
                Float2 tcoord(param[permute[0]], param[permute[1]]);
                if (vba.HasTCoord(unit))
                {
                    assertion(vba.GetTCoordChannels(unit) == 2,
                        "Texture coordinate must be 2D\n");
                    vba.TCoord<Float2>(unit, i) = tcoord;
                }
            }
        }
    }

    int numTriangles = 2*(numRows - 1)*(numCols - 1);
    IndexBuffer* ibuffer = new0 IndexBuffer(3*numTriangles, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (row = 0, i = 0; row < numRows - 1; ++row)
    {
        int i0 = i;
        int i1 = i0 + 1;
        i += numCols;
        int i2 = i;
        int i3 = i2 + 1;
        for (col = 0; col < numCols - 1; ++col, indices += 6)
        {
            if (ccw)
            {
                indices[0] = i0++;
                indices[1] = i1;
                indices[2] = i2;
                indices[3] = i1++;
                indices[4] = i3++;
                indices[5] = i2++;
            }
            else
            {
                indices[0] = i0++;
                indices[1] = i2;
                indices[2] = i1;
                indices[3] = i1++;
                indices[4] = i2++;
                indices[5] = i3++;
            }
        }
    }

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    return mesh;
}
//----------------------------------------------------------------------------
void BoxSurface::UpdateFace (int numRows, int numCols, VertexFormat* vformat,
    VertexBuffer* vbuffer, bool ccw, float faceValue, int permute[3])
{
    VertexBufferAccessor vba(vformat, vbuffer);

    float param[3];
    param[permute[2]] = faceValue;
    float sign = (ccw ? 1.0f : -1.0f);
    float rowFactor = 1.0f/(float)(numRows - 1);
    float colFactor = 1.0f/(float)(numCols - 1);
    for (int row = 0, i = 0; row < numRows; ++row)
    {
        param[permute[1]] = row*rowFactor;
        for (int col = 0; col < numCols; ++col, ++i)
        {
            param[permute[0]] = col*colFactor;
            vba.Position<Vector3f>(i) = mVolume->GetPosition(param);

            if (vba.HasNormal())
            {
                Vector3f cDer = mVolume->GetDerivative(permute[0], param);
                Vector3f rDer = mVolume->GetDerivative(permute[1], param);
                vba.Normal<Vector3f>(i) = sign*cDer.UnitCross(rDer);
            }
        }
    }
}
//----------------------------------------------------------------------------
void BoxSurface::UpdateSurface ()
{
    int permute[3];
    TriMesh* mesh;
    VertexFormat* vformat;
    VertexBuffer* vbuffer;

    // u faces
    permute[0] = 1;
    permute[1] = 2;
    permute[2] = 0;

    // u = 0
    mesh = StaticCast<TriMesh>(GetChild(0));
    vformat = mesh->GetVertexFormat();
    vbuffer = mesh->GetVertexBuffer();
    UpdateFace(mNumWSamples, mNumVSamples, vformat, vbuffer, false, 0.0f,
        permute);
    mesh->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(vbuffer);

    // u = 1
    mesh = StaticCast<TriMesh>(GetChild(1));
    vformat = mesh->GetVertexFormat();
    vbuffer = mesh->GetVertexBuffer();
    UpdateFace(mNumWSamples, mNumVSamples, vformat, vbuffer, true, 1.0f,
        permute);
    mesh->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(vbuffer);

    // v faces
    permute[0] = 0;
    permute[1] = 2;
    permute[2] = 1;

    // v = 0
    mesh = StaticCast<TriMesh>(GetChild(2));
    vformat = mesh->GetVertexFormat();
    vbuffer = mesh->GetVertexBuffer();
    UpdateFace(mNumWSamples, mNumUSamples, vformat, vbuffer, true, 0.0f,
        permute);
    mesh->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(vbuffer);

    // v = 1
    mesh = StaticCast<TriMesh>(GetChild(3));
    vformat = mesh->GetVertexFormat();
    vbuffer = mesh->GetVertexBuffer();
    UpdateFace(mNumWSamples, mNumUSamples, vformat, vbuffer, false, 1.0f,
        permute);
    mesh->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(vbuffer);

    // w faces
    permute[0] = 0;
    permute[1] = 1;
    permute[2] = 2;

    // w = 0
    mesh = StaticCast<TriMesh>(GetChild(4));
    vformat = mesh->GetVertexFormat();
    vbuffer = mesh->GetVertexBuffer();
    UpdateFace(mNumVSamples, mNumUSamples, vformat, vbuffer, false, 0.0f,
        permute);
    mesh->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(vbuffer);

    // w = 1
    mesh = StaticCast<TriMesh>(GetChild(5));
    vformat = mesh->GetVertexFormat();
    vbuffer = mesh->GetVertexBuffer();
    UpdateFace(mNumVSamples, mNumUSamples, vformat, vbuffer, true, 1.0f,
        permute);
    mesh->UpdateModelSpace(Visual::GU_NORMALS);
    Renderer::UpdateAll(vbuffer);
}
//----------------------------------------------------------------------------
void BoxSurface::EnableSorting ()
{
    for (int face = 0; face < 6; face++)
    {
        TriMesh* mesh = StaticCast<TriMesh>(GetChild(face));
        VisualEffectInstance* effect = mesh->GetEffectInstance();
        const int numPasses = effect->GetNumPasses();
        for (int p = 0; p < numPasses; ++p)
        {
            const VisualPass* pass = effect->GetPass(p);
            CullState* cstate = pass->GetCullState();
            cstate->Enabled = false;
            DepthState* dstate = pass->GetDepthState();
            dstate->Enabled = false;
            dstate->Writable = true;
            dstate->Compare = DepthState::CM_LEQUAL;
        }
    }

    mDoSort = true;
}
//----------------------------------------------------------------------------
void BoxSurface::DisableSorting ()
{
    for (int face = 0; face < 6; face++)
    {
        TriMesh* mesh = StaticCast<TriMesh>(GetChild(face));
        VisualEffectInstance* effect = mesh->GetEffectInstance();
        const int numPasses = effect->GetNumPasses();
        for (int p = 0; p < numPasses; ++p)
        {
            const VisualPass* pass = effect->GetPass(p);
            CullState* cstate = pass->GetCullState();
            cstate->Enabled = true;
            DepthState* dstate = pass->GetDepthState();
            dstate->Enabled = true;
            dstate->Writable = true;
            dstate->Compare = DepthState::CM_LEQUAL;
        }
    }

    mDoSort = false;
}
//----------------------------------------------------------------------------
void BoxSurface::SortFaces (const AVector& worldViewDirection)
{
    if (!mDoSort)
    {
        // Sorting must be enabled in order to sort.
        return;
    }

    // Inverse transform the world view direction into the local space of
    // the box.
    AVector localDir = WorldTransform.Inverse()*worldViewDirection;

    // Store back-facing faces in the front of the array.  Store front-facing
    // faces in the rear of the array.
    SpatialPtr sorted[6];
    int backFace = 0, frontFace = 5;
    int i;
    for (i = 0; i < 6; ++i)
    {
        const std::string& name = GetChild(i)->GetName();
        if (name[0] == 'u')
        {
            if (name[1] == '0')
            {
                if (localDir[0] > 0.0f)
                {
                    sorted[frontFace--] = GetChild(i);
                }
                else
                {
                    sorted[backFace++] = GetChild(i);
                }
            }
            else  // name[1] == '1'
            {
                if (localDir[0] < 0.0f)
                {
                    sorted[frontFace--] = GetChild(i);
                }
                else
                {
                    sorted[backFace++] = GetChild(i);
                }
            }
        }
        else if (name[0] == 'v')
        {
            if (name[1] == '0')
            {
                if (localDir[1] > 0.0f)
                {
                    sorted[frontFace--] = GetChild(i);
                }
                else
                {
                    sorted[backFace++] = GetChild(i);
                }
            }
            else  // name[1] == '1'
            {
                if (localDir[1] < 0.0f)
                {
                    sorted[frontFace--] = GetChild(i);
                }
                else
                {
                    sorted[backFace++] = GetChild(i);
                }
            }
        }
        else // name[0] == 'w'
        {
            if (name[1] == '0')
            {
                if (localDir[2] > 0.0f)
                {
                    sorted[frontFace--] = GetChild(i);
                }
                else
                {
                    sorted[backFace++] = GetChild(i);
                }
            }
            else  // name[1] == '1'
            {
                if (localDir[2] < 0.0f)
                {
                    sorted[frontFace--] = GetChild(i);
                }
                else
                {
                    sorted[backFace++] = GetChild(i);
                }
            }
        }
        DetachChildAt(i);
    }
    assertion(backFace - frontFace == 1, "Unexpected condition\n");

    // Reassign the sorted children to the node parent.
    for (i = 0; i < 6; ++i)
    {
        SetChild(i, sorted[i]);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
BoxSurface::BoxSurface (LoadConstructor value)
    :
    Node(value),
    mVolume(0),
    mNumUSamples(0),
    mNumVSamples(0),
    mNumWSamples(0),
    mDoSort(false)
{
}
//----------------------------------------------------------------------------
void BoxSurface::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    int numUCtrlPoints, numVCtrlPoints, numWCtrlPoints;
    int uDegree, vDegree, wDegree;
    source.Read(numUCtrlPoints);
    source.Read(numVCtrlPoints);
    source.Read(numWCtrlPoints);
    source.Read(uDegree);
    source.Read(vDegree);
    source.Read(wDegree);
    mVolume = new0 BSplineVolumef(numUCtrlPoints, numVCtrlPoints,
        numWCtrlPoints, uDegree, vDegree, wDegree);
    for (int u = 0; u < numUCtrlPoints; ++u)
    {
        for (int v = 0; v < numVCtrlPoints; ++v)
        {
            for (int w = 0; w < numWCtrlPoints; ++w)
            {
                Vector3f ctrl;
                source.ReadAggregate(ctrl);
                mVolume->SetControlPoint(u, v, w, ctrl);
            }
        }
    }

    source.Read(mNumUSamples);
    source.Read(mNumVSamples);
    source.Read(mNumWSamples);
    source.ReadBool(mDoSort);

    WM5_END_DEBUG_STREAM_LOAD(BoxSurface, source);
}
//----------------------------------------------------------------------------
void BoxSurface::Link (InStream& source)
{
    Node::Link(source);
}
//----------------------------------------------------------------------------
void BoxSurface::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool BoxSurface::Register (OutStream& target) const
{
    return Node::Register(target);
}
//----------------------------------------------------------------------------
void BoxSurface::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    int numUCtrlPoints = mVolume->GetNumCtrlPoints(0);
    int numVCtrlPoints = mVolume->GetNumCtrlPoints(1);
    int numWCtrlPoints = mVolume->GetNumCtrlPoints(2);
    int uDegree = mVolume->GetDegree(0);
    int vDegree = mVolume->GetDegree(1);
    int wDegree = mVolume->GetDegree(2);
    target.Write(numUCtrlPoints);
    target.Write(numVCtrlPoints);
    target.Write(numWCtrlPoints);
    target.Write(uDegree);
    target.Write(vDegree);
    target.Write(wDegree);
    for (int u = 0; u < numUCtrlPoints; ++u)
    {
        for (int v = 0; v < numVCtrlPoints; ++v)
        {
            for (int w = 0; w < numWCtrlPoints; ++w)
            {
                Vector3f ctrl = mVolume->GetControlPoint(u, v, w);
                target.WriteAggregate(ctrl);
            }
        }
    }

    target.Write(mNumUSamples);
    target.Write(mNumVSamples);
    target.Write(mNumWSamples);
    target.WriteBool(mDoSort);

    WM5_END_DEBUG_STREAM_SAVE(BoxSurface, target);
}
//----------------------------------------------------------------------------
int BoxSurface::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();

    int numUCtrlPoints = mVolume->GetNumCtrlPoints(0);
    int numVCtrlPoints = mVolume->GetNumCtrlPoints(1);
    int numWCtrlPoints = mVolume->GetNumCtrlPoints(2);
    size += sizeof(numUCtrlPoints);
    size += sizeof(numVCtrlPoints);
    size += sizeof(numWCtrlPoints);
    size += sizeof(int);  // uDegree
    size += sizeof(int);  // vDegree
    size += sizeof(int);  // wDegree
    size += numUCtrlPoints*numVCtrlPoints*numWCtrlPoints*sizeof(Vector3f);

    size += sizeof(mNumUSamples);
    size += sizeof(mNumVSamples);
    size += sizeof(mNumWSamples);
    size += WM5_BOOLSIZE(mDoSort);

    return size;
}
//----------------------------------------------------------------------------
