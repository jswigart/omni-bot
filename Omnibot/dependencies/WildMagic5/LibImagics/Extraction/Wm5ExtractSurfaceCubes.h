// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5EXTRACTSURFACECUBES_H
#define WM5EXTRACTSURFACECUBES_H

#include "Wm5ImagicsLIB.h"
#include "Wm5Vector3.h"
#include "Wm5TriangleKey.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM ExtractSurfaceCubes
{
public:
    // The input is a 3D image with lexicographically ordered voxels in
    // (x,y,z) stored in a linear array.  Voxel (x,y,z) is stored in the
    // array at location x+xbound*(y+ybound*z).  The caller is responsible
    // for deleting data if it was dynamically allocated.
    ExtractSurfaceCubes (int xBound, int yBound, int zBound, int* data);

    // The level value *must* not be exactly an integer.  This simplifies the
    // level surface construction immensely.
    void ExtractContour (float level, std::vector<Vector3f>& vertices,
        std::vector<TriangleKey>& triangles);

    // The extraction has duplicate vertices on edges shared by voxels.  This
    // function will eliminate the duplication.
    void MakeUnique (std::vector<Vector3f>& vertices,
        std::vector<TriangleKey>& triangles);

    // The extraction does not use any topological information about the level
    // surface.  The triangles can be a mixture of clockwise-ordered and
    // counterclockwise-ordered.  This function is an attempt to give the
    // triangles a consistent ordering by selecting a normal in approximately
    // the same direction as the average gradient at the vertices (when
    // sameDir is true), or in the opposite direction (when sameDir is
    // false).  This might not always produce a consistent order, but is
    // fast.  A consistent order can be computed if you build a table of
    // vertex, edge, and face adjacencies, but the resulting data structure
    // is very expensive to process to reorient triangles.
    void OrientTriangles (std::vector<Vector3f>& vertices,
        std::vector<TriangleKey>& triangles, bool sameDir);

    // In case you want vertex normals for dynamic lighting of the mesh.
    void ComputeNormals (const std::vector<Vector3f>& vertices,
        const std::vector<TriangleKey>& triangles,
        std::vector<Vector3f>& normals);

protected:
    enum
    {
        EI_XMIN_YMIN =  0,
        EI_XMIN_YMAX =  1,
        EI_XMAX_YMIN =  2,
        EI_XMAX_YMAX =  3,
        EI_XMIN_ZMIN =  4,
        EI_XMIN_ZMAX =  5,
        EI_XMAX_ZMIN =  6,
        EI_XMAX_ZMAX =  7,
        EI_YMIN_ZMIN =  8,
        EI_YMIN_ZMAX =  9,
        EI_YMAX_ZMIN = 10,
        EI_YMAX_ZMAX = 11,
        FI_XMIN      = 12,
        FI_XMAX      = 13,
        FI_YMIN      = 14,
        FI_YMAX      = 15,
        FI_ZMIN      = 16,
        FI_ZMAX      = 17,

        EB_XMIN_YMIN = 1 << EI_XMIN_YMIN,
        EB_XMIN_YMAX = 1 << EI_XMIN_YMAX,
        EB_XMAX_YMIN = 1 << EI_XMAX_YMIN,
        EB_XMAX_YMAX = 1 << EI_XMAX_YMAX,
        EB_XMIN_ZMIN = 1 << EI_XMIN_ZMIN,
        EB_XMIN_ZMAX = 1 << EI_XMIN_ZMAX,
        EB_XMAX_ZMIN = 1 << EI_XMAX_ZMIN,
        EB_XMAX_ZMAX = 1 << EI_XMAX_ZMAX,
        EB_YMIN_ZMIN = 1 << EI_YMIN_ZMIN,
        EB_YMIN_ZMAX = 1 << EI_YMIN_ZMAX,
        EB_YMAX_ZMIN = 1 << EI_YMAX_ZMIN,
        EB_YMAX_ZMAX = 1 << EI_YMAX_ZMAX,
        FB_XMIN      = 1 << FI_XMIN,
        FB_XMAX      = 1 << FI_XMAX,
        FB_YMIN      = 1 << FI_YMIN,
        FB_YMAX      = 1 << FI_YMAX,
        FB_ZMIN      = 1 << FI_ZMIN,
        FB_ZMAX      = 1 << FI_ZMAX
    };

    // vertex-edge-triangle table to support mesh topology
    class VETable
    {
    public:
        VETable ();

        bool IsValidVertex (int i) const;
        float GetX (int i) const;
        float GetY (int i) const;
        float GetZ (int i) const;
        void Insert (int i, const Vector3f& P);
        void Insert (int i0, int i1);
        void RemoveTriangles (std::vector<Vector3f>& vertices,
            std::vector<TriangleKey>& triangles);

    protected:
        void RemoveVertex (int i);
        bool Remove (TriangleKey& key);

        class Vertex
        {
        public:
            Vertex ();

            Vector3f P;
            int NumAdjacents;
            int Adj[4];
            bool Valid;
        };

        Vertex mVertex[18];
    };

    int GetVertices (float level, int x, int y, int z, VETable& table);

    void GetXMinEdges (int x, int y, int z, int type, VETable& table);
    void GetXMaxEdges (int x, int y, int z, int type, VETable& table);
    void GetYMinEdges (int x, int y, int z, int type, VETable& table);
    void GetYMaxEdges (int x, int y, int z, int type, VETable& table);
    void GetZMinEdges (int x, int y, int z, int type, VETable& table);
    void GetZMaxEdges (int x, int y, int z, int type, VETable& table);

    Vector3f GetGradient (Vector3f P);

    int mXBound, mYBound, mZBound, mXYBound;
    int* mData;
};

}

#endif
