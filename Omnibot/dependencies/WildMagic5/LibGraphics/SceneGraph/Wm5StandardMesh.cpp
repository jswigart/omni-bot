// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5StandardMesh.h"
#include "Wm5Float2.h"
using namespace Wm5;

//----------------------------------------------------------------------------
StandardMesh::StandardMesh (VertexFormat* vformat, bool isStatic, bool inside,
    const Transform* transform)
    :
    mVFormat(vformat),
    mIsStatic(isStatic),
    mInside(inside),
    mHasNormals(false),
    mUsage(isStatic ? Buffer::BU_STATIC : Buffer::BU_DYNAMIC)
{
    int posIndex = mVFormat->GetIndex(VertexFormat::AU_POSITION);
    assertion(posIndex >= 0, "Vertex format must have positions\n");
    VertexFormat::AttributeType posType =
        mVFormat->GetAttributeType(posIndex);
    assertion(posType == VertexFormat::AT_FLOAT3,
        "Positions must be 3-tuples of floats\n");
    WM5_UNUSED(posType);

    int norIndex = mVFormat->GetIndex(VertexFormat::AU_NORMAL);
    if (norIndex >= 0)
    {
        VertexFormat::AttributeType norType =
            mVFormat->GetAttributeType(norIndex);

        if (norType == VertexFormat::AT_FLOAT3)
        {
            mHasNormals = true;
        }
    }

    for (int unit = 0; unit < MAX_UNITS; ++unit)
    {
        mHasTCoords[unit] = false;
        int tcdIndex = mVFormat->GetIndex(VertexFormat::AU_TEXCOORD, unit);
        if (tcdIndex >= 0)
        {
            VertexFormat::AttributeType tcdType =
                mVFormat->GetAttributeType(tcdIndex);

            if (tcdType == VertexFormat::AT_FLOAT2)
            {
                mHasTCoords[unit] = true;
            }
        }
    }

    if (transform)
    {
        mTransform = *transform;
    }
}
//----------------------------------------------------------------------------
StandardMesh::~StandardMesh ()
{
}
//----------------------------------------------------------------------------
void StandardMesh::SetTransform (const Transform& transform)
{
    mTransform = transform;
}
//----------------------------------------------------------------------------
const Transform& StandardMesh::GetTransform () const
{
    return mTransform;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Rectangle (int xSamples, int ySamples, float xExtent,
    float yExtent)
{
    int numVertices = xSamples*ySamples;
    int numTriangles = 2*(xSamples-1)*(ySamples-1);
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    float inv0 = 1.0f/(xSamples - 1.0f);
    float inv1 = 1.0f/(ySamples - 1.0f);
    float u, v, x, y;
    int i, i0, i1;
    for (i1 = 0, i = 0; i1 < ySamples; ++i1)
    {
        v = i1*inv1;
        y = (2.0f*v - 1.0f)*yExtent;
        for (i0 = 0; i0 < xSamples; ++i0, ++i)
        {
            u = i0*inv0;
            x = (2.0f*u - 1.0f)*xExtent;

            vba.Position<Float3>(i) = Float3(x, y, 0.0f);

            if (mHasNormals)
            {
                vba.Normal<Float3>(i) = Float3(0.0f, 0.0f, 1.0f);
            }

            Float2 tcoord(u, v);
            for (int unit = 0; unit < MAX_UNITS; ++unit)
            {
                if (mHasTCoords[unit])
                {
                    vba.TCoord<Float2>(unit, i) = tcoord;
                }
            }
        }
    }
    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    for (i1 = 0; i1 < ySamples - 1; ++i1)
    {
        for (i0 = 0; i0 < xSamples - 1; ++i0)
        {
            int v0 = i0 + xSamples * i1;
            int v1 = v0 + 1;
            int v2 = v1 + xSamples;
            int v3 = v0 + xSamples;
            *indices++ = v0;
            *indices++ = v1;
            *indices++ = v2;
            *indices++ = v0;
            *indices++ = v2;
            *indices++ = v3;
        }
    }

    return new0 TriMesh(mVFormat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Disk (int shellSamples, int radialSamples,
    float radius)
{
    int rsm1 = radialSamples - 1, ssm1 = shellSamples - 1;
    int numVertices = 1 + radialSamples*ssm1;
    int numTriangles = radialSamples*(2*ssm1-1);
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.

    // Center of disk.
    vba.Position<Float3>(0) = Float3(0.0f, 0.0f, 0.0f);
    if (mHasNormals)
    {
        vba.Normal<Float3>(0) = Float3(0.0f, 0.0f, 1.0f);
    }

    Float2 tcoord(0.5f, 0.5f);
    int unit;
    for (unit = 0; unit < MAX_UNITS; ++unit)
    {
        if (mHasTCoords[unit])
        {
            vba.TCoord<Float2>(unit, 0) = tcoord;
        }
    }

    float invSSm1 = 1.0f/(float)ssm1;
    float invRS = 1.0f/(float)radialSamples;
    for (int r = 0; r < radialSamples; ++r)
    {
        float angle = Mathf::TWO_PI*invRS*r;
        float cs = Mathf::Cos(angle);
        float sn = Mathf::Sin(angle);
        AVector radial(cs, sn, 0.0f);

        for (int s = 1; s < shellSamples; ++s)
        {
            float fraction = invSSm1*s;  // in (0,R]
            AVector fracRadial = fraction*radial;
            int i = s + ssm1*r;

            vba.Position<Float3>(i) = radius*fracRadial;
            if (mHasNormals)
            {
                vba.Normal<Float3>(i) = Float3(0.0f, 0.0f, 1.0f);
            }

            tcoord[0] = 0.5f + 0.5f*fracRadial[0];
            tcoord[1] = 0.5f + 0.5f*fracRadial[1];
            for (unit = 0; unit < MAX_UNITS; ++unit)
            {
                if (mHasTCoords[unit])
                {
                    vba.TCoord<Float2>(unit, i) = tcoord;
                }
            }
        }
    }
    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    for (int r0 = rsm1, r1 = 0, t = 0; r1 < radialSamples; r0 = r1++)
    {
        indices[0] = 0;
        indices[1] = 1 + ssm1*r0;
        indices[2] = 1 + ssm1*r1;
        indices += 3;
        ++t;
        for (int s = 1; s < ssm1; ++s, indices += 6)
        {
            int i00 = s + ssm1*r0;
            int i01 = s + ssm1*r1;
            int i10 = i00 + 1;
            int i11 = i01 + 1;
            indices[0] = i00;
            indices[1] = i10;
            indices[2] = i11;
            indices[3] = i00;
            indices[4] = i11;
            indices[5] = i01;
            t += 2;
        }
    }

    return new0 TriMesh(mVFormat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Box (float xExtent, float yExtent, float zExtent)
{
    int numVertices = 8;
    int numTriangles = 12;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    vba.Position<Float3>(0) = Float3(-xExtent, -yExtent, -zExtent);
    vba.Position<Float3>(1) = Float3(+xExtent, -yExtent, -zExtent);
    vba.Position<Float3>(2) = Float3(+xExtent, +yExtent, -zExtent);
    vba.Position<Float3>(3) = Float3(-xExtent, +yExtent, -zExtent);
    vba.Position<Float3>(4) = Float3(-xExtent, -yExtent, +zExtent);
    vba.Position<Float3>(5) = Float3(+xExtent, -yExtent, +zExtent);
    vba.Position<Float3>(6) = Float3(+xExtent, +yExtent, +zExtent);
    vba.Position<Float3>(7) = Float3(-xExtent, +yExtent, +zExtent);
    for (int unit = 0; unit < MAX_UNITS; ++unit)
    {
        if (mHasTCoords[unit])
        {
            vba.TCoord<Float2>(unit, 0) = Float2(0.25f, 0.75f);
            vba.TCoord<Float2>(unit, 1) = Float2(0.75f, 0.75f);
            vba.TCoord<Float2>(unit, 2) = Float2(0.75f, 0.25f);
            vba.TCoord<Float2>(unit, 3) = Float2(0.25f, 0.25f);
            vba.TCoord<Float2>(unit, 4) = Float2(0.0f, 1.0f);
            vba.TCoord<Float2>(unit, 5) = Float2(1.0f, 1.0f);
            vba.TCoord<Float2>(unit, 6) = Float2(1.0f, 0.0f);
            vba.TCoord<Float2>(unit, 7) = Float2(0.0f, 0.0f);
        }
    }
    TransformData(vba);

    // Generate indices (outside view).
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0;  indices[ 1] = 2;  indices[ 2] = 1;
    indices[ 3] = 0;  indices[ 4] = 3;  indices[ 5] = 2;
    indices[ 6] = 0;  indices[ 7] = 1;  indices[ 8] = 5;
    indices[ 9] = 0;  indices[10] = 5;  indices[11] = 4;
    indices[12] = 0;  indices[13] = 4;  indices[14] = 7;
    indices[15] = 0;  indices[16] = 7;  indices[17] = 3;
    indices[18] = 6;  indices[19] = 4;  indices[20] = 5;
    indices[21] = 6;  indices[22] = 7;  indices[23] = 4;
    indices[24] = 6;  indices[25] = 5;  indices[26] = 1;
    indices[27] = 6;  indices[28] = 1;  indices[29] = 2;
    indices[30] = 6;  indices[31] = 2;  indices[32] = 3;
    indices[33] = 6;  indices[34] = 3;  indices[35] = 7;

    if (mInside)
    {
        ReverseTriangleOrder(numTriangles, indices);
    }

    TriMesh* mesh = new0 TriMesh(mVFormat, vbuffer, ibuffer);
    if (mHasNormals)
    {
        mesh->UpdateModelSpace(Visual::GU_NORMALS);
    }
    return mesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Cylinder (int axisSamples, int radialSamples,
    float radius, float height, bool open)
{
    TriMesh* mesh;
    int unit;
    Float2 tcoord;

    if (open)
    {
        int numVertices = axisSamples*(radialSamples+1);
        int numTriangles = 2*(axisSamples-1)*radialSamples;
        int numIndices = 3*numTriangles;
        int stride = mVFormat->GetStride();

        // Create a vertex buffer.
        VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride,
            mUsage);

        VertexBufferAccessor vba(mVFormat, vbuffer);

        // Generate geometry.
        float invRS = 1.0f/(float)radialSamples;
        float invASm1 = 1.0f/(float)(axisSamples-1);
        float halfHeight = 0.5f*height;
        int r, a, aStart, i;

        // Generate points on the unit circle to be used in computing the
        // mesh points on a cylinder slice.
        float* cs = new1<float>(radialSamples + 1);
        float* sn = new1<float>(radialSamples + 1);
        for (r = 0; r < radialSamples; ++r)
        {
            float angle = Mathf::TWO_PI*invRS*r;
            cs[r] = Mathf::Cos(angle);
            sn[r] = Mathf::Sin(angle);
        }
        cs[radialSamples] = cs[0];
        sn[radialSamples] = sn[0];

        // Generate the cylinder itself.
        for (a = 0, i = 0; a < axisSamples; ++a)
        {
            float axisFraction = a*invASm1;  // in [0,1]
            float z = -halfHeight + height*axisFraction;

            // Compute center of slice.
            APoint sliceCenter(0.0f, 0.0f, z);

            // Compute slice vertices with duplication at endpoint.
            int save = i;
            for (r = 0; r < radialSamples; ++r)
            {
                float radialFraction = r*invRS;  // in [0,1)
                AVector normal(cs[r], sn[r], 0.0f);

                vba.Position<Float3>(i) = sliceCenter + radius*normal;

                if (mHasNormals)
                {
                    if (mInside)
                    {
                        vba.Normal<Float3>(i) = -normal;
                    }
                    else
                    {
                        vba.Normal<Float3>(i) = normal;
                    }
                }

                tcoord = Float2(radialFraction, axisFraction);
                for (unit = 0; unit < MAX_UNITS; ++unit)
                {
                    if (mHasTCoords[unit])
                    {
                        vba.TCoord<Float2>(unit, i) = tcoord;
                    }
                }

                ++i;
            }

            vba.Position<Float3>(i) = vba.Position<Float3>(save);

            if (mHasNormals)
            {
                vba.Normal<Float3>(i) = vba.Normal<Float3>(save);
            }

            tcoord = Float2(1.0f, axisFraction);
            for (unit = 0; unit < MAX_UNITS; ++unit)
            {
                if (mHasTCoords[unit])
                {
                    vba.TCoord<Float2>(0, i) = tcoord;
                }
            }

            ++i;
        }
        TransformData(vba);

        // Generate indices.
        IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
        int* indices = (int*)ibuffer->GetData();
        for (a = 0, aStart = 0; a < axisSamples-1; ++a)
        {
            int i0 = aStart;
            int i1 = i0 + 1;
            aStart += radialSamples + 1;
            int i2 = aStart;
            int i3 = i2 + 1;
            for (i = 0; i < radialSamples; ++i, indices += 6)
            {
                if (mInside)
                {
                    indices[0] = i0++;
                    indices[1] = i2;
                    indices[2] = i1;
                    indices[3] = i1++;
                    indices[4] = i2++;
                    indices[5] = i3++;
                }
                else // outside view
                {
                    indices[0] = i0++;
                    indices[1] = i1;
                    indices[2] = i2;
                    indices[3] = i1++;
                    indices[4] = i3++;
                    indices[5] = i2++;
                }
            }
        }

        delete1(cs);
        delete1(sn);

        mesh = new0 TriMesh(mVFormat, vbuffer, ibuffer);
    }
    else
    {
        mesh = Sphere(axisSamples, radialSamples, radius);
        VertexBuffer* vbuffer = mesh->GetVertexBuffer();
        int numVertices = vbuffer->GetNumElements();

        VertexBufferAccessor vba(mVFormat, vbuffer);

        // Flatten sphere at poles.
        float hDiv2 = 0.5f*height;
        vba.Position<Float3>(numVertices-2)[2] = -hDiv2;  // south pole
        vba.Position<Float3>(numVertices-1)[2] = +hDiv2;  // north pole

        // Remap z-values to [-h/2,h/2].
        float zFactor = 2.0f/(axisSamples-1);
        float tmp0 = radius*(-1.0f + zFactor);
        float tmp1 = 1.0f/(radius*(+1.0f - zFactor));
        for (int i = 0; i < numVertices-2; ++i)
        {
            Float3& pos = vba.Position<Float3>(i);
            pos[2] = hDiv2*(-1.0f + tmp1*(pos[2] - tmp0));
            float adjust = radius*Mathf::InvSqrt(pos[0]*pos[0] +
                pos[1]*pos[1]);
            pos[0] *= adjust;
            pos[1] *= adjust;
        }
        TransformData(vba);

        if (mHasNormals)
        {
            mesh->UpdateModelSpace(Visual::GU_NORMALS);
        }
    }

    // The duplication of vertices at the seam causes the automatically
    // generated bounding volume to be slightly off center.  Reset the bound
    // to use the true information.
    float maxDist = Mathf::Sqrt(radius*radius + height*height);
    mesh->GetModelBound().SetCenter(APoint::ORIGIN);
    mesh->GetModelBound().SetRadius(maxDist);
    return mesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Sphere (int zSamples, int radialSamples,
    float radius)
{
    int zsm1 = zSamples-1, zsm2 = zSamples-2, zsm3 = zSamples-3;
    int rsp1 = radialSamples+1;
    int numVertices = zsm2*rsp1 + 2;
    int numTriangles = 2*zsm2*radialSamples;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    float invRS = 1.0f/(float)radialSamples;
    float zFactor = 2.0f/(float)zsm1;
    int r, z, zStart, i, unit;
    Float2 tcoord;

    // Generate points on the unit circle to be used in computing the mesh
    // points on a cylinder slice.
    float* sn = new1<float>(rsp1);
    float* cs = new1<float>(rsp1);
    for (r = 0; r < radialSamples; ++r)
    {
        float angle = Mathf::TWO_PI*invRS*r;
        cs[r] = Mathf::Cos(angle);
        sn[r] = Mathf::Sin(angle);
    }
    sn[radialSamples] = sn[0];
    cs[radialSamples] = cs[0];

    // Generate the cylinder itself.
    for (z = 1, i = 0; z < zsm1; ++z)
    {
        float zFraction = -1.0f + zFactor*z;  // in (-1,1)
        float zValue = radius*zFraction;

        // Compute center of slice.
        APoint sliceCenter(0.0f, 0.0f, zValue);

        // Compute radius of slice.
        float sliceRadius = Mathf::Sqrt(Mathf::FAbs(
            radius*radius - zValue*zValue));

        // Compute slice vertices with duplication at endpoint.
        AVector normal;
        int save = i;
        for (r = 0; r < radialSamples; ++r)
        {
            float radialFraction = r*invRS;  // in [0,1)
            AVector radial(cs[r], sn[r], 0.0f);

            vba.Position<Float3>(i) = sliceCenter + sliceRadius*radial;

            if (mHasNormals)
            {
                normal = vba.Position<Float3>(i);
                normal.Normalize();
                if (mInside)
                {
                    vba.Normal<Float3>(i) = -normal;
                }
                else
                {
                    vba.Normal<Float3>(i) = normal;
                }
            }

            tcoord[0] = radialFraction;
            tcoord[1] = 0.5f*(zFraction + 1.0f);
            for (unit = 0; unit < MAX_UNITS; ++unit)
            {
                if (mHasTCoords[unit])
                {
                    vba.TCoord<Float2>(unit, i) = tcoord;
                }
            }

            ++i;
        }

        vba.Position<Float3>(i) = vba.Position<Float3>(save);
        if (mHasNormals)
        {
            vba.Normal<Float3>(i) = vba.Normal<Float3>(save);
        }

        tcoord[0] = 1.0f;
        tcoord[1] = 0.5f*(zFraction + 1.0f);
        for (unit = 0; unit < MAX_UNITS; ++unit)
        {
            if (mHasTCoords[unit])
            {
                vba.TCoord<Float2>(unit, i) = tcoord;
            }
        }

        ++i;
    }

    // south pole
    vba.Position<Float3>(i) = Float3(0.0f, 0.0f, -radius);
    if (mHasNormals)
    {
        if (mInside)
        {
            vba.Normal<Float3>(i) = Float3(0.0f, 0.0f, 1.0f);
        }
        else
        {
            vba.Normal<Float3>(i) = Float3(0.0f, 0.0f, -1.0f);
        }
    }

    tcoord = Float2(0.5f, 0.5f);
    for (unit = 0; unit < MAX_UNITS; ++unit)
    {
        if (mHasTCoords[unit])
        {
            vba.TCoord<Float2>(unit, i) = tcoord;
        }
    }

    ++i;

    // north pole
    vba.Position<Float3>(i) = Float3(0.0f, 0.0f, radius);
    if (mHasNormals)
    {
        if (mInside)
        {
            vba.Normal<Float3>(i) = Float3(0.0f, 0.0f, -1.0f);
        }
        else
        {
            vba.Normal<Float3>(i) = Float3(0.0f, 0.0f, 1.0f);
        }
    }

    tcoord = Float2(0.5f, 1.0f);
    for (unit = 0; unit < MAX_UNITS; ++unit)
    {
        if (mHasTCoords[unit])
        {
            vba.TCoord<Float2>(unit, i) = tcoord;
        }
    }

    ++i;

    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    for (z = 0, zStart = 0; z < zsm3; ++z)
    {
        int i0 = zStart;
        int i1 = i0 + 1;
        zStart += rsp1;
        int i2 = zStart;
        int i3 = i2 + 1;
        for (i = 0; i < radialSamples; ++i, indices += 6)
        {
            if (mInside)
            {
                indices[0] = i0++;
                indices[1] = i2;
                indices[2] = i1;
                indices[3] = i1++;
                indices[4] = i2++;
                indices[5] = i3++;
            }
            else  // inside view
            {
                indices[0] = i0++;
                indices[1] = i1;
                indices[2] = i2;
                indices[3] = i1++;
                indices[4] = i3++;
                indices[5] = i2++;
            }
        }
    }

    // south pole triangles
    int numVerticesM2 = numVertices - 2;
    for (i = 0; i < radialSamples; ++i, indices += 3)
    {
        if (mInside)
        {
            indices[0] = i;
            indices[1] = i + 1;
            indices[2] = numVerticesM2;
        }
        else
        {
            indices[0] = i;
            indices[1] = numVerticesM2;
            indices[2] = i + 1;
        }
    }

    // north pole triangles
    int numVerticesM1 = numVertices-1, offset = zsm3*rsp1;
    for (i = 0; i < radialSamples; ++i, indices += 3)
    {
        if (mInside)
        {
            indices[0] = i + offset;
            indices[1] = numVerticesM1;
            indices[2] = i + 1 + offset;
        }
        else
        {
            indices[0] = i + offset;
            indices[1] = i + 1 + offset;
            indices[2] = numVerticesM1;
        }
    }

    delete1(cs);
    delete1(sn);

    // The duplication of vertices at the seam cause the automatically
    // generated bounding volume to be slightly off center.  Reset the bound
    // to use the true information.
    TriMesh* mesh = new0 TriMesh(mVFormat, vbuffer, ibuffer);
    mesh->GetModelBound().SetCenter(APoint::ORIGIN);
    mesh->GetModelBound().SetRadius(radius);
    return mesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Torus (int circleSamples, int radialSamples,
    float outerRadius, float innerRadius)
{
    int numVertices = (circleSamples+1)*(radialSamples+1);
    int numTriangles = 2*circleSamples*radialSamples;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    float invCS = 1.0f/(float)circleSamples;
    float invRS = 1.0f/(float)radialSamples;
    int c, r, i, unit;
    Float2 tcoord;

    // Generate the cylinder itself.
    for (c = 0, i = 0; c < circleSamples; ++c)
    {
        // Compute center point on torus circle at specified angle.
        float circleFraction = c*invCS;  // in [0,1)
        float theta = Mathf::TWO_PI*circleFraction;
        float cosTheta = Mathf::Cos(theta);
        float sinTheta = Mathf::Sin(theta);
        AVector radial(cosTheta, sinTheta, 0.0f);
        AVector torusMiddle = outerRadius*radial;

        // Compute slice vertices with duplication at endpoint.
        int save = i;
        for (r = 0; r < radialSamples; ++r)
        {
            float radialFraction = r*invRS;  // in [0,1)
            float phi = Mathf::TWO_PI*radialFraction;
            float cosPhi = Mathf::Cos(phi);
            float sinPhi = Mathf::Sin(phi);
            AVector normal = cosPhi*radial + sinPhi*AVector::UNIT_Z;
            vba.Position<Float3>(i) = torusMiddle + innerRadius*normal;
            if (mHasNormals)
            {
                if (mInside)
                {
                    vba.Normal<Float3>(i) = -normal;
                }
                else
                {
                    vba.Normal<Float3>(i) = normal;
                }
            }

            tcoord = Float2(radialFraction, circleFraction);
            for (unit = 0; unit < MAX_UNITS; ++unit)
            {
                if (mHasTCoords[unit])
                {
                    vba.TCoord<Float2>(unit, i) = tcoord;
                }
            }

            ++i;
        }

        vba.Position<Float3>(i) = vba.Position<Float3>(save);
        if (mHasNormals)
        {
            vba.Normal<Float3>(i) = vba.Normal<Float3>(save);
        }

        tcoord = Float2(1.0f, circleFraction);
        for (unit = 0; unit < MAX_UNITS; ++unit)
        {
            if (mHasTCoords[unit])
            {
                vba.TCoord<Float2>(unit, i) = tcoord;
            }
        }

        ++i;
    }

    // Duplicate the cylinder ends to form a torus.
    for (r = 0; r <= radialSamples; ++r, ++i)
    {
        vba.Position<Float3>(i) = vba.Position<Float3>(r);
        if (mHasNormals)
        {
            vba.Normal<Float3>(i) = vba.Normal<Float3>(r);
        }

        for (unit = 0; unit < MAX_UNITS; ++unit)
        {
            if (mHasTCoords[unit])
            {
                vba.TCoord<Float2>(unit, i) =
                    Float2(vba.TCoord<Float2>(unit, r)[0], 1.0f);
            }
        }
    }

    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    int cStart = 0;
    for (c = 0; c < circleSamples; ++c)
    {
        int i0 = cStart;
        int i1 = i0 + 1;
        cStart += radialSamples + 1;
        int i2 = cStart;
        int i3 = i2 + 1;
        for (i = 0; i < radialSamples; ++i, indices += 6)
        {
            if (mInside)
            {
                indices[0] = i0++;
                indices[1] = i1;
                indices[2] = i2;
                indices[3] = i1++;
                indices[4] = i3++;
                indices[5] = i2++;
            }
            else  // inside view
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

    // The duplication of vertices at the seam cause the automatically
    // generated bounding volume to be slightly off center.  Reset the bound
    // to use the true information.
    TriMesh* mesh = new0 TriMesh(mVFormat, vbuffer, ibuffer);
    mesh->GetModelBound().SetCenter(APoint::ORIGIN);
    mesh->GetModelBound().SetRadius(outerRadius);
    return mesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Tetrahedron ()
{
    float fSqrt2Div3 = Mathf::Sqrt(2.0f)/3.0f;
    float fSqrt6Div3 = Mathf::Sqrt(6.0f)/3.0f;
    float fOneThird = 1.0f/3.0f;

    int numVertices = 4;
    int numTriangles = 4;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    vba.Position<Float3>(0) = Float3(0.0f, 0.0f, 1.0f);
    vba.Position<Float3>(1) = Float3(2.0f*fSqrt2Div3, 0.0f, -fOneThird);
    vba.Position<Float3>(2) = Float3(-fSqrt2Div3, fSqrt6Div3, -fOneThird);
    vba.Position<Float3>(3) = Float3(-fSqrt2Div3, -fSqrt6Div3, -fOneThird);
    CreatePlatonicNormals(vba);
    CreatePlatonicUVs(vba);
    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0;  indices[ 1] = 1;  indices[ 2] = 2;
    indices[ 3] = 0;  indices[ 4] = 2;  indices[ 5] = 3;
    indices[ 6] = 0;  indices[ 7] = 3;  indices[ 8] = 1;
    indices[ 9] = 1;  indices[10] = 3;  indices[11] = 2;

    if (mInside)
    {
        ReverseTriangleOrder(numTriangles,indices);
    }

    return new0 TriMesh(mVFormat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Hexahedron ()
{
    float fSqrtThird = Mathf::Sqrt(1.0f/3.0f);

    int numVertices = 8;
    int numTriangles = 12;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);

    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    vba.Position<Float3>(0) = Float3(-fSqrtThird, -fSqrtThird, -fSqrtThird);
    vba.Position<Float3>(1) = Float3( fSqrtThird, -fSqrtThird, -fSqrtThird);
    vba.Position<Float3>(2) = Float3( fSqrtThird,  fSqrtThird, -fSqrtThird);
    vba.Position<Float3>(3) = Float3(-fSqrtThird,  fSqrtThird, -fSqrtThird);
    vba.Position<Float3>(4) = Float3(-fSqrtThird, -fSqrtThird,  fSqrtThird);
    vba.Position<Float3>(5) = Float3( fSqrtThird, -fSqrtThird,  fSqrtThird);
    vba.Position<Float3>(6) = Float3( fSqrtThird,  fSqrtThird,  fSqrtThird);
    vba.Position<Float3>(7) = Float3(-fSqrtThird,  fSqrtThird,  fSqrtThird);
    CreatePlatonicNormals(vba);
    CreatePlatonicUVs(vba);
    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0;  indices[ 1] = 3;  indices[ 2] = 2;
    indices[ 3] = 0;  indices[ 4] = 2;  indices[ 5] = 1;
    indices[ 6] = 0;  indices[ 7] = 1;  indices[ 8] = 5;
    indices[ 9] = 0;  indices[10] = 5;  indices[11] = 4;
    indices[12] = 0;  indices[13] = 4;  indices[14] = 7;
    indices[15] = 0;  indices[16] = 7;  indices[17] = 3;
    indices[18] = 6;  indices[19] = 5;  indices[20] = 1;
    indices[21] = 6;  indices[22] = 1;  indices[23] = 2;
    indices[24] = 6;  indices[25] = 2;  indices[26] = 3;
    indices[27] = 6;  indices[28] = 3;  indices[29] = 7;
    indices[30] = 6;  indices[31] = 7;  indices[32] = 4;
    indices[33] = 6;  indices[34] = 4;  indices[35] = 5;

    if (mInside)
    {
        ReverseTriangleOrder(numTriangles,indices);
    }

    return new0 TriMesh(mVFormat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Octahedron ()
{
    int numVertices = 6;
    int numTriangles = 8;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    vba.Position<Float3>(0) = Float3( 1.0f, 0.0f, 0.0f);
    vba.Position<Float3>(1) = Float3(-1.0f, 0.0f, 0.0f);
    vba.Position<Float3>(2) = Float3( 0.0f, 1.0f, 0.0f);
    vba.Position<Float3>(3) = Float3( 0.0f,-1.0f, 0.0f);
    vba.Position<Float3>(4) = Float3( 0.0f, 0.0f, 1.0f);
    vba.Position<Float3>(5) = Float3( 0.0f, 0.0f,-1.0f);
    CreatePlatonicNormals(vba);
    CreatePlatonicUVs(vba);
    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 4;  indices[ 1] = 0;  indices[ 2] = 2;
    indices[ 3] = 4;  indices[ 4] = 2;  indices[ 5] = 1;
    indices[ 6] = 4;  indices[ 7] = 1;  indices[ 8] = 3;
    indices[ 9] = 4;  indices[10] = 3;  indices[11] = 0;
    indices[12] = 5;  indices[13] = 2;  indices[14] = 0;
    indices[15] = 5;  indices[16] = 1;  indices[17] = 2;
    indices[18] = 5;  indices[19] = 3;  indices[20] = 1;
    indices[21] = 5;  indices[22] = 0;  indices[23] = 3;

    if (mInside)
    {
        ReverseTriangleOrder(numTriangles,indices);
    }

    return new0 TriMesh(mVFormat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Dodecahedron ()
{
    float a = 1.0f/Mathf::Sqrt(3.0f);
    float b = Mathf::Sqrt((3.0f-Mathf::Sqrt(5.0f))/6.0f);
    float c = Mathf::Sqrt((3.0f+Mathf::Sqrt(5.0f))/6.0f);

    int numVertices = 20;
    int numTriangles = 36;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    vba.Position<Float3>( 0) = Float3( a, a, a);
    vba.Position<Float3>( 1) = Float3( a, a,-a);
    vba.Position<Float3>( 2) = Float3( a,-a, a);
    vba.Position<Float3>( 3) = Float3( a,-a,-a);
    vba.Position<Float3>( 4) = Float3(-a, a, a);
    vba.Position<Float3>( 5) = Float3(-a, a,-a);
    vba.Position<Float3>( 6) = Float3(-a,-a, a);
    vba.Position<Float3>( 7) = Float3(-a,-a,-a);
    vba.Position<Float3>( 8) = Float3(  b,  c, 0.0f);
    vba.Position<Float3>( 9) = Float3( -b,  c, 0.0f);
    vba.Position<Float3>(10) = Float3(  b, -c, 0.0f);
    vba.Position<Float3>(11) = Float3( -b, -c, 0.0f);
    vba.Position<Float3>(12) = Float3(  c, 0.0f,  b);
    vba.Position<Float3>(13) = Float3(  c, 0.0f, -b);
    vba.Position<Float3>(14) = Float3( -c, 0.0f,  b);
    vba.Position<Float3>(15) = Float3( -c, 0.0f, -b);
    vba.Position<Float3>(16) = Float3(0.0f,   b,  c);
    vba.Position<Float3>(17) = Float3(0.0f,  -b,  c);
    vba.Position<Float3>(18) = Float3(0.0f,   b, -c);
    vba.Position<Float3>(19) = Float3(0.0f,  -b, -c);
    CreatePlatonicNormals(vba);
    CreatePlatonicUVs(vba);
    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    indices[  0] =  0;  indices[  1] =  8;  indices[  2] =  9;
    indices[  3] =  0;  indices[  4] =  9;  indices[  5] =  4;
    indices[  6] =  0;  indices[  7] =  4;  indices[  8] = 16;
    indices[  9] =  0;  indices[ 10] = 12;  indices[ 11] = 13;
    indices[ 12] =  0;  indices[ 13] = 13;  indices[ 14] =  1;
    indices[ 15] =  0;  indices[ 16] =  1;  indices[ 17] =  8;
    indices[ 18] =  0;  indices[ 19] = 16;  indices[ 20] = 17;
    indices[ 21] =  0;  indices[ 22] = 17;  indices[ 23] =  2;
    indices[ 24] =  0;  indices[ 25] =  2;  indices[ 26] = 12;
    indices[ 27] =  8;  indices[ 28] =  1;  indices[ 29] = 18;
    indices[ 30] =  8;  indices[ 31] = 18;  indices[ 32] =  5;
    indices[ 33] =  8;  indices[ 34] =  5;  indices[ 35] =  9;
    indices[ 36] = 12;  indices[ 37] =  2;  indices[ 38] = 10;
    indices[ 39] = 12;  indices[ 40] = 10;  indices[ 41] =  3;
    indices[ 42] = 12;  indices[ 43] =  3;  indices[ 44] = 13;
    indices[ 45] = 16;  indices[ 46] =  4;  indices[ 47] = 14;
    indices[ 48] = 16;  indices[ 49] = 14;  indices[ 50] =  6;
    indices[ 51] = 16;  indices[ 52] =  6;  indices[ 53] = 17;
    indices[ 54] =  9;  indices[ 55] =  5;  indices[ 56] = 15;
    indices[ 57] =  9;  indices[ 58] = 15;  indices[ 59] = 14;
    indices[ 60] =  9;  indices[ 61] = 14;  indices[ 62] =  4;
    indices[ 63] =  6;  indices[ 64] = 11;  indices[ 65] = 10;
    indices[ 66] =  6;  indices[ 67] = 10;  indices[ 68] =  2;
    indices[ 69] =  6;  indices[ 70] =  2;  indices[ 71] = 17;
    indices[ 72] =  3;  indices[ 73] = 19;  indices[ 74] = 18;
    indices[ 75] =  3;  indices[ 76] = 18;  indices[ 77] =  1;
    indices[ 78] =  3;  indices[ 79] =  1;  indices[ 80] = 13;
    indices[ 81] =  7;  indices[ 82] = 15;  indices[ 83] =  5;
    indices[ 84] =  7;  indices[ 85] =  5;  indices[ 86] = 18;
    indices[ 87] =  7;  indices[ 88] = 18;  indices[ 89] = 19;
    indices[ 90] =  7;  indices[ 91] = 11;  indices[ 92] =  6;
    indices[ 93] =  7;  indices[ 94] =  6;  indices[ 95] = 14;
    indices[ 96] =  7;  indices[ 97] = 14;  indices[ 98] = 15;
    indices[ 99] =  7;  indices[100] = 19;  indices[101] =  3;
    indices[102] =  7;  indices[103] =  3;  indices[104] = 10;
    indices[105] =  7;  indices[106] = 10;  indices[107] = 11;

    if (mInside)
    {
        ReverseTriangleOrder(numTriangles,indices);
    }

    return new0 TriMesh(mVFormat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Icosahedron ()
{
    float goldenRatio = 0.5f*(1.0f + Mathf::Sqrt(5.0f));
    float invRoot = 1.0f/Mathf::Sqrt(1.0f + goldenRatio*goldenRatio);
    float u = goldenRatio*invRoot;
    float v = invRoot;

    int numVertices = 12;
    int numTriangles = 20;
    int numIndices = 3*numTriangles;
    int stride = mVFormat->GetStride();

    // Create a vertex buffer.
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride, mUsage);
    VertexBufferAccessor vba(mVFormat, vbuffer);

    // Generate geometry.
    vba.Position<Float3>( 0) = Float3(  u,  v,0.0f);
    vba.Position<Float3>( 1) = Float3( -u,  v,0.0f);
    vba.Position<Float3>( 2) = Float3(  u, -v,0.0f);
    vba.Position<Float3>( 3) = Float3( -u, -v,0.0f);
    vba.Position<Float3>( 4) = Float3(  v,0.0f,  u);
    vba.Position<Float3>( 5) = Float3(  v,0.0f, -u);
    vba.Position<Float3>( 6) = Float3( -v,0.0f,  u);
    vba.Position<Float3>( 7) = Float3( -v,0.0f, -u);
    vba.Position<Float3>( 8) = Float3(0.0f,  u,  v);
    vba.Position<Float3>( 9) = Float3(0.0f, -u,  v);
    vba.Position<Float3>(10) = Float3(0.0f,  u, -v);
    vba.Position<Float3>(11) = Float3(0.0f, -u, -v);
    CreatePlatonicNormals(vba);
    CreatePlatonicUVs(vba);
    TransformData(vba);

    // Generate indices.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, 4, mUsage);
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] =  0;  indices[ 1] =  8;  indices[ 2] =  4;
    indices[ 3] =  0;  indices[ 4] =  5;  indices[ 5] = 10;
    indices[ 6] =  2;  indices[ 7] =  4;  indices[ 8] =  9;
    indices[ 9] =  2;  indices[10] = 11;  indices[11] =  5;
    indices[12] =  1;  indices[13] =  6;  indices[14] =  8;
    indices[15] =  1;  indices[16] = 10;  indices[17] =  7;
    indices[18] =  3;  indices[19] =  9;  indices[20] =  6;
    indices[21] =  3;  indices[22] =  7;  indices[23] = 11;
    indices[24] =  0;  indices[25] = 10;  indices[26] =  8;
    indices[27] =  1;  indices[28] =  8;  indices[29] = 10;
    indices[30] =  2;  indices[31] =  9;  indices[32] = 11;
    indices[33] =  3;  indices[34] = 11;  indices[35] =  9;
    indices[36] =  4;  indices[37] =  2;  indices[38] =  0;
    indices[39] =  5;  indices[40] =  0;  indices[41] =  2;
    indices[42] =  6;  indices[43] =  1;  indices[44] =  3;
    indices[45] =  7;  indices[46] =  3;  indices[47] =  1;
    indices[48] =  8;  indices[49] =  6;  indices[50] =  4;
    indices[51] =  9;  indices[52] =  4;  indices[53] =  6;
    indices[54] = 10;  indices[55] =  5;  indices[56] =  7;
    indices[57] = 11;  indices[58] =  7;  indices[59] =  5;

    if (mInside)
    {
        ReverseTriangleOrder(numTriangles,indices);
    }

    return new0 TriMesh(mVFormat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
void StandardMesh::TransformData (VertexBufferAccessor& vba)
{
    if (mTransform.IsIdentity())
    {
        return;
    }

    const int numVertices = vba.GetNumVertices();
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        APoint position = vba.Position<Float3>(i);
        vba.Position<Float3>(i) = mTransform*position;
    }

    if (mHasNormals)
    {
        for (i = 0; i < numVertices; ++i)
        {
            vba.Normal<AVector>(i).Normalize();
        }
    }
}
//----------------------------------------------------------------------------
void StandardMesh::ReverseTriangleOrder (int numTriangles, int* indices)
{
    for (int i = 0; i < numTriangles; ++i)
    { 
        int j1 = 3*i + 1;
        int j2 = j1 + 1;
        int save = indices[j1];
        indices[j1] = indices[j2];
        indices[j2] = save;
    }
}
//----------------------------------------------------------------------------
void StandardMesh::CreatePlatonicNormals (VertexBufferAccessor& vba)
{
    if (mHasNormals)
    {
        const int numVertices = vba.GetNumVertices();
        for (int i = 0; i < numVertices; ++i)
        {
            vba.Normal<Float3>(i) = vba.Position<Float3>(i);
        }
    }
}
//----------------------------------------------------------------------------
void StandardMesh::CreatePlatonicUVs (VertexBufferAccessor& vba)
{
    for (int unit = 0; unit < MAX_UNITS; ++unit)
    {
        if (mHasTCoords[unit])
        {
            const int numVertices = vba.GetNumVertices();
            for (int i = 0; i < numVertices; ++i)
            {
                if (Mathf::FAbs(vba.Position<Float3>(i)[2]) < 1.0f)
                {
                    vba.TCoord<Float2>(unit, i)[0] = 0.5f*(1.0f +
                        Mathf::ATan2(vba.Position<Float3>(i)[1],
                        vba.Position<Float3>(i)[0])*Mathf::INV_PI);
                }
                else
                {
                    vba.TCoord<Float2>(unit, i)[0] = 0.5f;
                }
                vba.TCoord<Float2>(unit, i)[1] = Mathf::ACos(
                    vba.Position<Float3>(i)[2])*Mathf::INV_PI;
            }
        }
    }
}
//----------------------------------------------------------------------------
