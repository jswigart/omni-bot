// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5ExtractSurfaceCubes.h"
#include "Wm5Assert.h"
using namespace Wm5;

typedef std::map<Vector3f,int> VMap;
typedef VMap::iterator VMapIterator;
typedef std::map<TriangleKey,int> TMap;
typedef TMap::iterator TMapIterator;

//----------------------------------------------------------------------------
ExtractSurfaceCubes::ExtractSurfaceCubes(int xBound, int yBound,
    int zBound, int* data)
    :
    mXBound(xBound),
    mYBound(yBound),
    mZBound(zBound),
    mXYBound(xBound*yBound),
    mData(data)
{
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::ExtractContour (float level,
    std::vector<Vector3f>& vertices, std::vector<TriangleKey>& triangles)
{
    vertices.clear();
    triangles.clear();

    for (int z = 0; z < mZBound-1; ++z)
    {
        for (int y = 0; y < mYBound-1; ++y)
        {
            for (int x = 0; x < mXBound-1; ++x)
            {
                // Get vertices on edges of box (if any).
                VETable table;
                int type = GetVertices(level, x, y, z, table);
                if (type != 0)
                {
                    // Get edges on faces of box.
                    GetXMinEdges(x, y, z, type, table);
                    GetXMaxEdges(x, y, z, type, table);
                    GetYMinEdges(x, y, z, type, table);
                    GetYMaxEdges(x, y, z, type, table);
                    GetZMinEdges(x, y, z, type, table);
                    GetZMaxEdges(x, y, z, type, table);

                    // Ear-clip the wireframe mesh.
                    table.RemoveTriangles(vertices, triangles);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::MakeUnique (std::vector<Vector3f>& vertices,
    std::vector<TriangleKey>& triangles)
{
    int numVertices = (int)vertices.size();
    int numTriangles = (int)triangles.size();
    if (numVertices == 0 || numTriangles == 0)
    {
        return;
    }

    // Use a hash table to generate unique storage.
    VMap vertexMap;
    VMapIterator vIter;
    for (int v = 0, nextVertex = 0; v < numVertices; ++v)
    {
        // Keep only unique vertices.
        std::pair<VMapIterator,bool> result = vertexMap.insert(
            std::make_pair(vertices[v], nextVertex));

        if (result.second == true)
        {
            ++nextVertex;
        }
    }

    // Use a hash table to generate unique storage.
    TMap triangleMap;
    TMapIterator tIter;
    for (int t = 0, nextTriangle = 0; t < numTriangles; ++t)
    {
        // R.place old vertex indices by new ones
        TriangleKey& tri = triangles[t];
        vIter = vertexMap.find(vertices[tri.V[0]]);
        assertion(vIter != vertexMap.end(), "Unexpected condition\n");
        tri.V[0] = vIter->second;
        vIter = vertexMap.find(vertices[tri.V[1]]);
        assertion(vIter != vertexMap.end(), "Unexpected condition\n");
        tri.V[1] = vIter->second;
        vIter = vertexMap.find(vertices[tri.V[2]]);
        assertion(vIter != vertexMap.end(), "Unexpected condition\n");
        tri.V[2] = vIter->second;

        // Keep only unique triangles.
        std::pair<TMapIterator,bool> result = triangleMap.insert(
            std::make_pair(tri, nextTriangle));

        if (result.second == true)
        {
            ++nextTriangle;
        }
    }

    // Pack the vertices.
    vertices.resize(vertexMap.size());
    for (vIter = vertexMap.begin(); vIter != vertexMap.end(); ++vIter)
    {
        vertices[vIter->second] = vIter->first;
    }

    // Pack the triangles.
    triangles.resize(triangleMap.size());
    for (tIter = triangleMap.begin(); tIter != triangleMap.end(); ++tIter)
    {
        triangles[tIter->second] = tIter->first;
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::OrientTriangles (std::vector<Vector3f>& vertices,
    std::vector<TriangleKey>& triangles, bool sameDir)
{
    const int numTriangles = (int)triangles.size();
    for (int i = 0; i < numTriangles; ++i)
    {
        TriangleKey& tri = triangles[i];

        // Get triangle vertices.
        Vector3f v0 = vertices[tri.V[0]];
        Vector3f v1 = vertices[tri.V[1]];
        Vector3f v2 = vertices[tri.V[2]];
        
        // Construct triangle normal based on current orientation.
        Vector3f edge1 = v1 - v0;
        Vector3f edge2 = v2 - v0;
        Vector3f normal = edge1.Cross(edge2);

        // Get the image gradient at the vertices.
        Vector3f grad0 = GetGradient(v0);
        Vector3f grad1 = GetGradient(v1);
        Vector3f grad2 = GetGradient(v2);

        // Compute the average gradient.
        Vector3f gradAvr = (grad0 + grad1 + grad2)/3.0f;
        
        // Compute the dot product of normal and average gradient.
        float dot = gradAvr.Dot(normal);

        // Choose triangle orientation based on gradient direction.
        int save;
        if (sameDir)
        {
            if (dot < 0.0f)
            {
                // Wrong orientation, reorder it.
                save = tri.V[1];
                tri.V[1] = tri.V[2];
                tri.V[2] = save;
            }
        }
        else
        {
            if (dot > 0.0f)
            {
                // Wrong orientation, reorder it.
                save = tri.V[1];
                tri.V[1] = tri.V[2];
                tri.V[2] = save;
            }
        }
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::ComputeNormals (
    const std::vector<Vector3f>& vertices,
    const std::vector<TriangleKey>& triangles, std::vector<Vector3f>& normals)
{
    // Maintain a running sum of triangle normals at each vertex.
    int numVertices = (int)vertices.size();
    int numTriangles = (int)triangles.size();
    normals.resize(numVertices);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        normals[i] = Vector3f::ZERO;
    }

    for (i = 0; i < numTriangles; ++i)
    {
        const TriangleKey& key = triangles[i];
        Vector3f v0 = vertices[key.V[0]];
        Vector3f v1 = vertices[key.V[1]];
        Vector3f v2 = vertices[key.V[2]];

        // Construct triangle normal.
        Vector3f edge1 = v1 - v0;
        Vector3f edge2 = v2 - v0;
        Vector3f normal = edge1.Cross(edge2);

        // Maintain the sum of normals at each vertex.
        normals[key.V[0]] += normal;
        normals[key.V[1]] += normal;
        normals[key.V[2]] += normal;
    }

    // The normal vector storage was used to accumulate the sum of triangle
    // normals.  Now these vectors must be rescaled to be unit length.
    for (i = 0; i < numVertices; ++i)
    {
        normals[i].Normalize();
    }
}
//----------------------------------------------------------------------------
int ExtractSurfaceCubes::GetVertices (float level, int x, int y, int z,
    VETable& table)
{
    int type = 0;

    // get image values at corners of voxel
    int i000 = x + mXBound*(y + mYBound*z);
    int i100 = i000 + 1;
    int i010 = i000 + mXBound;
    int i110 = i010 + 1;
    int i001 = i000 + mXYBound;
    int i101 = i001 + 1;
    int i011 = i001 + mXBound;
    int i111 = i011 + 1;
    float f000 = (float)mData[i000];
    float f100 = (float)mData[i100];
    float f010 = (float)mData[i010];
    float f110 = (float)mData[i110];
    float f001 = (float)mData[i001];
    float f101 = (float)mData[i101];
    float f011 = (float)mData[i011];
    float f111 = (float)mData[i111];

    float x0 = (float)x, y0 = (float)y, z0 = (float)z;
    float x1 = x0 + 1.0f, y1 = y0 + 1.0f, z1 = z0 + 1.0f;

    // xmin-ymin edge
    float diff0 = level - f000;
    float diff1 = level - f001;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMIN_YMIN;
        table.Insert(EI_XMIN_YMIN,
            Vector3f(x0, y0, z0 + diff0/(f001 - f000)));
    }

    // xmin-ymax edge
    diff0 = level - f010;
    diff1 = level - f011;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMIN_YMAX;
        table.Insert(EI_XMIN_YMAX,
            Vector3f(x0, y1, z0 + diff0/(f011 - f010)));
    }

    // xmax-ymin edge
    diff0 = level - f100;
    diff1 = level - f101;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMAX_YMIN;
        table.Insert(EI_XMAX_YMIN,
            Vector3f(x1, y0, z0 + diff0/(f101 - f100)));
    }

    // xmax-ymax edge
    diff0 = level - f110;
    diff1 = level - f111;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMAX_YMAX;
        table.Insert(EI_XMAX_YMAX,
            Vector3f(x1, y1, z0 + diff0/(f111 - f110)));
    }

    // xmin-zmin edge
    diff0 = level - f000;
    diff1 = level - f010;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMIN_ZMIN;
        table.Insert(EI_XMIN_ZMIN,
            Vector3f(x0, y0 + diff0/(f010 - f000), z0));
    }

    // xmin-zmax edge
    diff0 = level - f001;
    diff1 = level - f011;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMIN_ZMAX;
        table.Insert(EI_XMIN_ZMAX,
            Vector3f(x0, y0 + diff0/(f011 - f001), z1));
    }

    // xmax-zmin edge
    diff0 = level - f100;
    diff1 = level - f110;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMAX_ZMIN;
        table.Insert(EI_XMAX_ZMIN,
            Vector3f(x1, y0 + diff0/(f110 - f100), z0));
    }

    // xmax-zmax edge
    diff0 = level - f101;
    diff1 = level - f111;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_XMAX_ZMAX;
        table.Insert(EI_XMAX_ZMAX,
            Vector3f(x1, y0 + diff0/(f111 - f101), z1));
    }

    // ymin-zmin edge
    diff0 = level - f000;
    diff1 = level - f100;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_YMIN_ZMIN;
        table.Insert(EI_YMIN_ZMIN,
            Vector3f(x0 + diff0/(f100 - f000), y0, z0));
    }

    // ymin-zmax edge
    diff0 = level - f001;
    diff1 = level - f101;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_YMIN_ZMAX;
        table.Insert(EI_YMIN_ZMAX,
            Vector3f(x0 + diff0/(f101 - f001), y0, z1));
    }

    // ymax-zmin edge
    diff0 = level - f010;
    diff1 = level - f110;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_YMAX_ZMIN;
        table.Insert(EI_YMAX_ZMIN,
            Vector3f(x0 + diff0/(f110 - f010), y1, z0));
    }

    // ymax-zmax edge
    diff0 = level - f011;
    diff1 = level - f111;
    if (diff0*diff1 < 0.0f)
    {
        type |= EB_YMAX_ZMAX;
        table.Insert(EI_YMAX_ZMAX,
            Vector3f(x0 + diff0/(f111 - f011), y1, z1));
    }

    return type;
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::GetXMinEdges (int x, int y, int z, int type,
    VETable& table)
{
    int faceType = 0;
    if (type & EB_XMIN_YMIN)
    {
        faceType |= 0x01;
    }
    if (type & EB_XMIN_YMAX)
    {
        faceType |= 0x02;
    }
    if (type & EB_XMIN_ZMIN)
    {
        faceType |= 0x04;
    }
    if (type & EB_XMIN_ZMAX)
    {
        faceType |= 0x08;
    }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(EI_XMIN_YMIN, EI_XMIN_YMAX); break;
    case  5: table.Insert(EI_XMIN_YMIN, EI_XMIN_ZMIN); break;
    case  6: table.Insert(EI_XMIN_YMAX, EI_XMIN_ZMIN); break;
    case  9: table.Insert(EI_XMIN_YMIN, EI_XMIN_ZMAX); break;
    case 10: table.Insert(EI_XMIN_YMAX, EI_XMIN_ZMAX); break;
    case 12: table.Insert(EI_XMIN_ZMIN, EI_XMIN_ZMAX); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = x + mXBound*(y + mYBound*z);
        int f00 = mData[i];  // F(x,y,z)
        i += mXBound;
        int f10 = mData[i];  // F(x,y+1,z)
        i += mXYBound;
        int f11 = mData[i];  // F(x,y+1,z+1)
        i -= mXBound;
        int f01 = mData[i];  // F(x,y,z+1)
        int det = f00*f11 - f01*f10;

        if (det > 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(EI_XMIN_YMIN, EI_XMIN_ZMIN);
            table.Insert(EI_XMIN_YMAX, EI_XMIN_ZMAX);
        }
        else if (det < 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(EI_XMIN_YMIN, EI_XMIN_ZMAX);
            table.Insert(EI_XMIN_YMAX, EI_XMIN_ZMIN);
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_XMIN,Vector3f(table.GetX(EI_XMIN_ZMIN),
                table.GetY(EI_XMIN_ZMIN), table.GetZ(EI_XMIN_YMIN)));

            // Add edges sharing the branch point.
            table.Insert(EI_XMIN_YMIN, FI_XMIN);
            table.Insert(EI_XMIN_YMAX, FI_XMIN);
            table.Insert(EI_XMIN_ZMIN, FI_XMIN);
            table.Insert(EI_XMIN_ZMAX, FI_XMIN);
        }
        break;
    }
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::GetXMaxEdges (int x, int y, int z, int type,
    VETable& table)
{
    int faceType = 0;
    if (type & EB_XMAX_YMIN)
    {
        faceType |= 0x01;
    }
    if (type & EB_XMAX_YMAX)
    {
        faceType |= 0x02;
    }
    if (type & EB_XMAX_ZMIN)
    {
        faceType |= 0x04;
    }
    if (type & EB_XMAX_ZMAX)
    {
        faceType |= 0x08;
    }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(EI_XMAX_YMIN, EI_XMAX_YMAX); break;
    case  5: table.Insert(EI_XMAX_YMIN, EI_XMAX_ZMIN); break;
    case  6: table.Insert(EI_XMAX_YMAX, EI_XMAX_ZMIN); break;
    case  9: table.Insert(EI_XMAX_YMIN, EI_XMAX_ZMAX); break;
    case 10: table.Insert(EI_XMAX_YMAX, EI_XMAX_ZMAX); break;
    case 12: table.Insert(EI_XMAX_ZMIN, EI_XMAX_ZMAX); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = (x+1) + mXBound*(y + mYBound*z);
        int f00 = mData[i];  // F(x,y,z)
        i += mXBound;
        int f10 = mData[i];  // F(x,y+1,z)
        i += mXYBound;
        int f11 = mData[i];  // F(x,y+1,z+1)
        i -= mXBound;
        int f01 = mData[i];  // F(x,y,z+1)
        int det = f00*f11 - f01*f10;

        if (det > 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(EI_XMAX_YMIN, EI_XMAX_ZMIN);
            table.Insert(EI_XMAX_YMAX, EI_XMAX_ZMAX);
        }
        else if (det < 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(EI_XMAX_YMIN, EI_XMAX_ZMAX);
            table.Insert(EI_XMAX_YMAX, EI_XMAX_ZMIN);
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_XMAX,Vector3f(table.GetX(EI_XMAX_ZMIN),
                table.GetY(EI_XMAX_ZMIN), table.GetZ(EI_XMAX_YMIN)));

            // Add edges sharing the branch point.
            table.Insert(EI_XMAX_YMIN, FI_XMAX);
            table.Insert(EI_XMAX_YMAX, FI_XMAX);
            table.Insert(EI_XMAX_ZMIN, FI_XMAX);
            table.Insert(EI_XMAX_ZMAX, FI_XMAX);
        }
        break;
    }
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::GetYMinEdges (int x, int y, int z, int type,
    VETable& table)
{
    int faceType = 0;
    if (type & EB_XMIN_YMIN)
    {
        faceType |= 0x01;
    }
    if (type & EB_XMAX_YMIN)
    {
        faceType |= 0x02;
    }
    if (type & EB_YMIN_ZMIN)
    {
        faceType |= 0x04;
    }
    if (type & EB_YMIN_ZMAX)
    {
        faceType |= 0x08;
    }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(EI_XMIN_YMIN, EI_XMAX_YMIN); break;
    case  5: table.Insert(EI_XMIN_YMIN, EI_YMIN_ZMIN); break;
    case  6: table.Insert(EI_XMAX_YMIN, EI_YMIN_ZMIN); break;
    case  9: table.Insert(EI_XMIN_YMIN, EI_YMIN_ZMAX); break;
    case 10: table.Insert(EI_XMAX_YMIN, EI_YMIN_ZMAX); break;
    case 12: table.Insert(EI_YMIN_ZMIN, EI_YMIN_ZMAX); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = x + mXBound*(y + mYBound*z);
        int f00 = mData[i];  // F(x,y,z)
        i++;
        int f10 = mData[i];  // F(x+1,y,z)
        i += mXYBound;
        int f11 = mData[i];  // F(x+1,y,z+1)
        i--;
        int f01 = mData[i];  // F(x,y,z+1)
        int det = f00*f11 - f01*f10;

        if (det > 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(EI_XMIN_YMIN, EI_YMIN_ZMIN);
            table.Insert(EI_XMAX_YMIN, EI_YMIN_ZMAX);
        }
        else if (det < 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(EI_XMIN_YMIN, EI_YMIN_ZMAX);
            table.Insert(EI_XMAX_YMIN, EI_YMIN_ZMIN);
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_YMIN,Vector3f(table.GetX(EI_YMIN_ZMIN),
                table.GetY(EI_XMIN_YMIN), table.GetZ(EI_XMIN_YMIN)));

            // Add edges sharing the branch point.
            table.Insert(EI_XMIN_YMIN, FI_YMIN);
            table.Insert(EI_XMAX_YMIN, FI_YMIN);
            table.Insert(EI_YMIN_ZMIN, FI_YMIN);
            table.Insert(EI_YMIN_ZMAX, FI_YMIN);
        }
        break;
    }
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::GetYMaxEdges (int x, int y, int z, int type,
    VETable& table)
{
    int faceType = 0;
    if (type & EB_XMIN_YMAX)
    {
        faceType |= 0x01;
    }
    if (type & EB_XMAX_YMAX)
    {
        faceType |= 0x02;
    }
    if (type & EB_YMAX_ZMIN)
    {
        faceType |= 0x04;
    }
    if (type & EB_YMAX_ZMAX)
    {
        faceType |= 0x08;
    }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(EI_XMIN_YMAX, EI_XMAX_YMAX); break;
    case  5: table.Insert(EI_XMIN_YMAX, EI_YMAX_ZMIN); break;
    case  6: table.Insert(EI_XMAX_YMAX, EI_YMAX_ZMIN); break;
    case  9: table.Insert(EI_XMIN_YMAX, EI_YMAX_ZMAX); break;
    case 10: table.Insert(EI_XMAX_YMAX, EI_YMAX_ZMAX); break;
    case 12: table.Insert(EI_YMAX_ZMIN, EI_YMAX_ZMAX); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = x + mXBound*((y+1) + mYBound*z);
        int f00 = mData[i];  // F(x,y,z)
        i++;
        int f10 = mData[i];  // F(x+1,y,z)
        i += mXYBound;
        int f11 = mData[i];  // F(x+1,y,z+1)
        i--;
        int f01 = mData[i];  // F(x,y,z+1)
        int det = f00*f11 - f01*f10;

        if (det > 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(EI_XMIN_YMAX, EI_YMAX_ZMIN);
            table.Insert(EI_XMAX_YMAX, EI_YMAX_ZMAX);
        }
        else if (det < 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(EI_XMIN_YMAX, EI_YMAX_ZMAX);
            table.Insert(EI_XMAX_YMAX, EI_YMAX_ZMIN);
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_YMAX,Vector3f(table.GetX(EI_YMAX_ZMIN),
                table.GetY(EI_XMIN_YMAX), table.GetZ(EI_XMIN_YMAX)));

            // Add edges sharing the branch point.
            table.Insert(EI_XMIN_YMAX, FI_YMAX);
            table.Insert(EI_XMAX_YMAX, FI_YMAX);
            table.Insert(EI_YMAX_ZMIN, FI_YMAX);
            table.Insert(EI_YMAX_ZMAX, FI_YMAX);
        }
        break;
    }
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::GetZMinEdges (int x, int y, int z, int type,
    VETable& table)
{
    int faceType = 0;
    if (type & EB_XMIN_ZMIN)
    {
        faceType |= 0x01;
    }
    if (type & EB_XMAX_ZMIN)
    {
        faceType |= 0x02;
    }
    if (type & EB_YMIN_ZMIN)
    {
        faceType |= 0x04;
    }
    if (type & EB_YMAX_ZMIN)
    {
        faceType |= 0x08;
    }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(EI_XMIN_ZMIN, EI_XMAX_ZMIN); break;
    case  5: table.Insert(EI_XMIN_ZMIN, EI_YMIN_ZMIN); break;
    case  6: table.Insert(EI_XMAX_ZMIN, EI_YMIN_ZMIN); break;
    case  9: table.Insert(EI_XMIN_ZMIN, EI_YMAX_ZMIN); break;
    case 10: table.Insert(EI_XMAX_ZMIN, EI_YMAX_ZMIN); break;
    case 12: table.Insert(EI_YMIN_ZMIN, EI_YMAX_ZMIN); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = x + mXBound*(y + mYBound*z);
        int f00 = mData[i];  // F(x,y,z)
        i++;
        int f10 = mData[i];  // F(x+1,y,z)
        i += mXBound;
        int f11 = mData[i];  // F(x+1,y+1,z)
        i--;
        int f01 = mData[i];  // F(x,y+1,z)
        int det = f00*f11 - f01*f10;

        if (det > 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(EI_XMIN_ZMIN, EI_YMIN_ZMIN);
            table.Insert(EI_XMAX_ZMIN, EI_YMAX_ZMIN);
        }
        else if (det < 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(EI_XMIN_ZMIN, EI_YMAX_ZMIN);
            table.Insert(EI_XMAX_ZMIN, EI_YMIN_ZMIN);
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_ZMIN,Vector3f(table.GetX(EI_YMIN_ZMIN),
                table.GetY(EI_XMIN_ZMIN), table.GetZ(EI_XMIN_ZMIN)));

            // Add edges sharing the branch point.
            table.Insert(EI_XMIN_ZMIN, FI_ZMIN);
            table.Insert(EI_XMAX_ZMIN, FI_ZMIN);
            table.Insert(EI_YMIN_ZMIN, FI_ZMIN);
            table.Insert(EI_YMAX_ZMIN, FI_ZMIN);
        }
        break;
    }
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::GetZMaxEdges (int x, int y, int z, int type,
    VETable& table)
{
    int faceType = 0;
    if (type & EB_XMIN_ZMAX)
    {
        faceType |= 0x01;
    }
    if (type & EB_XMAX_ZMAX)
    {
        faceType |= 0x02;
    }
    if (type & EB_YMIN_ZMAX)
    {
        faceType |= 0x04;
    }
    if (type & EB_YMAX_ZMAX)
    {
        faceType |= 0x08;
    }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(EI_XMIN_ZMAX, EI_XMAX_ZMAX); break;
    case  5: table.Insert(EI_XMIN_ZMAX, EI_YMIN_ZMAX); break;
    case  6: table.Insert(EI_XMAX_ZMAX, EI_YMIN_ZMAX); break;
    case  9: table.Insert(EI_XMIN_ZMAX, EI_YMAX_ZMAX); break;
    case 10: table.Insert(EI_XMAX_ZMAX, EI_YMAX_ZMAX); break;
    case 12: table.Insert(EI_YMIN_ZMAX, EI_YMAX_ZMAX); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = x + mXBound*(y + mYBound*(z+1));
        int f00 = mData[i];  // F(x,y,z)
        i++;
        int f10 = mData[i];  // F(x+1,y,z)
        i += mXBound;
        int f11 = mData[i];  // F(x+1,y+1,z)
        i--;
        int f01 = mData[i];  // F(x,y+1,z)
        int det = f00*f11 - f01*f10;

        if (det > 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(EI_XMIN_ZMAX, EI_YMIN_ZMAX);
            table.Insert(EI_XMAX_ZMAX, EI_YMAX_ZMAX);
        }
        else if (det < 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(EI_XMIN_ZMAX, EI_YMAX_ZMAX);
            table.Insert(EI_XMAX_ZMAX, EI_YMIN_ZMAX);
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_ZMAX,Vector3f(table.GetX(EI_YMIN_ZMAX),
                table.GetY(EI_XMIN_ZMAX), table.GetZ(EI_XMIN_ZMAX)));

            // Add edges sharing the branch point.
            table.Insert(EI_XMIN_ZMAX, FI_ZMAX);
            table.Insert(EI_XMAX_ZMAX, FI_ZMAX);
            table.Insert(EI_YMIN_ZMAX, FI_ZMAX);
            table.Insert(EI_YMAX_ZMAX, FI_ZMAX);
        }
        break;
    }
    default:
        assertion(false, "Unexpected condition\n");
        break;
    }
}
//----------------------------------------------------------------------------
Vector3f ExtractSurfaceCubes::GetGradient (Vector3f P)
{
    int x = (int)P.X();
    if (x < 0 || x >= mXBound-1)
    {
        return Vector3f::ZERO;
    }

    int y = (int)P.Y();
    if (y < 0 || y >= mYBound-1)
    {
        return Vector3f::ZERO;
    }

    int z = (int)P.Z();
    if (z < 0 || z >= mZBound-1)
    {
        return Vector3f::ZERO;
    }

    // Get image values at corners of voxel.
    int i000 = x + mXBound*(y + mYBound*z);
    int i100 = i000 + 1;
    int i010 = i000 + mXBound;
    int i110 = i010 + 1;
    int i001 = i000 + mXYBound;
    int i101 = i001 + 1;
    int i011 = i001 + mXBound;
    int i111 = i011 + 1;
    float f000 = (float)mData[i000];
    float f100 = (float)mData[i100];
    float f010 = (float)mData[i010];
    float f110 = (float)mData[i110];
    float f001 = (float)mData[i001];
    float f101 = (float)mData[i101];
    float f011 = (float)mData[i011];
    float f111 = (float)mData[i111];

    P.X() -= x;
    P.Y() -= y;
    P.Z() -= z;
    float oneMX = 1.0f - P.X();
    float oneMY = 1.0f - P.Y();
    float oneMZ = 1.0f - P.Z();

    Vector3f grad;

    float tmp0 = oneMY*(f100 - f000) + P.Y()*(f110 - f010);
    float tmp1 = oneMY*(f101 - f001) + P.Y()*(f111 - f011);
    grad.X() = oneMZ*tmp0 + P.Z()*tmp1;
    
    tmp0 = oneMX*(f010 - f000) + P.X()*(f110 - f100);
    tmp1 = oneMX*(f011 - f001) + P.X()*(f111 - f101);
    grad.Y() = oneMZ*tmp0 + P.Z()*tmp1;
    
    tmp0 = oneMX*(f001 - f000) + P.X()*(f101 - f100);
    tmp1 = oneMX*(f011 - f010) + P.X()*(f111 - f110);
    grad.Z() = oneMY*tmp0 + P.Y()*tmp1;

    return grad;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ExtractSurfaceCubes::VETable
//----------------------------------------------------------------------------
ExtractSurfaceCubes::VETable::VETable ()
{
}
//----------------------------------------------------------------------------
bool ExtractSurfaceCubes::VETable::IsValidVertex (int i) const
{
    assertion(0 <= i && i < 18, "Invalid index\n");
    return mVertex[i].Valid;
}
//----------------------------------------------------------------------------
float ExtractSurfaceCubes::VETable::GetX (int i) const
{
    assertion(0 <= i && i < 18, "Invalid index\n");
    return mVertex[i].P.X();
}
//----------------------------------------------------------------------------
float ExtractSurfaceCubes::VETable::GetY (int i) const
{
    assertion(0 <= i && i < 18, "Invalid index\n");
    return mVertex[i].P.Y();
}
//----------------------------------------------------------------------------
float ExtractSurfaceCubes::VETable::GetZ (int i) const
{
    assertion(0 <= i && i < 18, "Invalid index\n");
    return mVertex[i].P.Z();
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::VETable::Insert (int i, const Vector3f& P)
{
    assertion(0 <= i && i < 18, "Invalid index\n");
    Vertex& vertex = mVertex[i];
    vertex.P = P;
    vertex.Valid = true;
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::VETable::Insert (int i0, int i1)
{
    assertion(0 <= i0 && i0 < 18 && 0 <= i1 && i1 < 18, "Invalid index\n");
    Vertex& vertex0 = mVertex[i0];
    Vertex& vertex1 = mVertex[i1];

    assertion(vertex0.NumAdjacents < 4 && vertex1.NumAdjacents < 4,
        "Invalid adjacent quantity\n");
    vertex0.Adj[vertex0.NumAdjacents++] = i1;
    vertex1.Adj[vertex1.NumAdjacents++] = i0;
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::VETable::RemoveTriangles (
    std::vector<Vector3f>& vertices, std::vector<TriangleKey>& triangles)
{
    // ear-clip the wireframe to get the triangles
    TriangleKey tri;
    while (Remove(tri))
    {
        int v0 = (int)vertices.size(), v1 = v0 + 1, v2 = v1 + 1;
        triangles.push_back(TriangleKey(v0, v1, v2));
        vertices.push_back(mVertex[tri.V[0]].P);
        vertices.push_back(mVertex[tri.V[1]].P);
        vertices.push_back(mVertex[tri.V[2]].P);
    }
}
//----------------------------------------------------------------------------
void ExtractSurfaceCubes::VETable::RemoveVertex (int i)
{
    assertion(0 <= i && i < 18, "Invalid index\n");
    Vertex& vertex0 = mVertex[i];

    assertion(vertex0.NumAdjacents == 2, "Invalid number of adjacents.\n");

    int a0 = vertex0.Adj[0], a1 = vertex0.Adj[1];
    Vertex& adjVertex0 = mVertex[a0];
    Vertex& adjVertex1 = mVertex[a1];

    int j;
    for (j = 0; j < adjVertex0.NumAdjacents; j++)
    {
        if (adjVertex0.Adj[j] == i)
        {
            adjVertex0.Adj[j] = a1;
            break;
        }
    }
    assertion(j != adjVertex0.NumAdjacents, "Unexpected condition\n");

    for (j = 0; j < adjVertex1.NumAdjacents; j++)
    {
        if (adjVertex1.Adj[j] == i)
        {
            adjVertex1.Adj[j] = a0;
            break;
        }
    }
    assertion(j != adjVertex1.NumAdjacents, "Unexpected condition\n");

    vertex0.Valid = false;

    if (adjVertex0.NumAdjacents == 2)
    {
        if (adjVertex0.Adj[0] == adjVertex0.Adj[1])
        {
            adjVertex0.Valid = false;
        }
    }

    if (adjVertex1.NumAdjacents == 2)
    {
        if (adjVertex1.Adj[0] == adjVertex1.Adj[1])
        {
            adjVertex1.Valid = false;
        }
    }
}
//----------------------------------------------------------------------------
bool ExtractSurfaceCubes::VETable::Remove (TriangleKey& tri)
{
    for (int i = 0; i < 18; ++i)
    {
        Vertex& vertex = mVertex[i];
        if (vertex.Valid && vertex.NumAdjacents == 2)
        {
            tri.V[0] = i;
            tri.V[1] = vertex.Adj[0];
            tri.V[2] = vertex.Adj[1];
            RemoveVertex(i);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ExtractSurfaceCubes::VETable::Vertex
//----------------------------------------------------------------------------
ExtractSurfaceCubes::VETable::Vertex::Vertex ()
    :
    NumAdjacents(0),
    Valid(false)
{
}
//----------------------------------------------------------------------------
