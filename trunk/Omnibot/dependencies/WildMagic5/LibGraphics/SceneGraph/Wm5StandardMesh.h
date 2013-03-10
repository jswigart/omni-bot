// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5STANDARDMESH_H
#define WM5STANDARDMESH_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VertexBufferAccessor.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM StandardMesh
{
public:
    // Standard meshes.  Each mesh is centered at (0,0,0) and has an up-axis
    // of (0,0,1).  The other axes forming the coordinate system are (1,0,0)
    // and (0,1,0).  An application may transform the meshes as necessary
    // by providing an input 'transform' to the constructor.  If a closed
    // mesh is to be visible to an observer outside the object, then set
    // 'inside' to 'false'; otherwise, set 'inside' to 'true' so that an
    // observer will see the mesh when inside the object.  Set 'isStatic'
    // to 'true' for a vertex buffer that has usage Buffer::BU_STATIC;
    // otherwise, 'isStatic' is 'false' and the vertex buffer has usage
    // Buffer::BU_DYNAMIC.

    // Construction and destruction.
    StandardMesh (VertexFormat* vformat, bool isStatic = true,
        bool inside = false, const Transform* transform = 0);

    ~StandardMesh ();

    // Access the transform for the mesh creator.
    void SetTransform (const Transform& transform);
    const Transform& GetTransform () const;

    // The comments for the various objects are for the case when the input
    // 'transform' is NULL (the identity transform).

    // The rectangle is in the plane z = 0 and is visible to an observer who
    // is on the side of the plane to which the normal (0,0,1) points.  It has
    // corners (-xExtent, -yExtent, 0), (+xExtent, -yExtent, 0),
    // (-xExtent, +yExtent, 0), and (+xExtent, +yExtent, 0).  The mesh has
    // xSamples vertices in the x-direction and ySamples vertices in the
    // y-direction for a total of xSamples*ySamples vertices.
    TriMesh* Rectangle (int xSamples, int ySamples, float xExtent,
        float yExtent);

    // The circular disk is in the plane z = 0 and is visible to an observer
    // who is on the side of the plane to which the normal (0,0,1) points.
    // It has center (0,0,0) and the specified 'radius'.  The mesh has its
    // first vertex at the center.  Samples are placed along rays whose
    // common origin is the center.  There are 'radialSamples' rays.  Along
    // each ray the mesh has 'shellSamples' vertices.
    TriMesh* Disk (int shellSamples, int radialSamples, float radius);

    // The box has center (0,0,0); unit-length axes (1,0,0), (0,1,0), and
    // (0,0,1); and extents (half-lengths) 'xExtent', 'yExtent', and
    // 'zExtent'.  The mesh has 8 vertices and 12 triangles.  For example,
    // the box corner in the first octant is (xExtent, yExtent, zExtent).
    TriMesh* Box (float xExtent, float yExtent, float zExtent);

    // The cylinder has center (0,0,0), the specified 'radius', and the
    // specified 'height'.  The cylinder axis is a line segment of the
    // form (0,0,0) + t*(0,0,1) for |t| <= height/2.  The cylinder wall
    // is implicitly defined by x^2+y^2 = radius^2.  If 'open' is 'true',
    // the cylinder end-disks are omitted; you have an open tube.  If
    // 'open' is 'false', the end-disks are included.  Each end-disk is
    // a regular polygon that is tessellated by including a vertex at
    // the center of the polygon and decomposing the polygon into triangles
    // that all share the center vertex and each triangle containing an
    // edge of the polygon.
    TriMesh* Cylinder (int axisSamples, int radialSamples, float radius,
        float height, bool open);

    // The sphere has center (0,0,0) and the specified 'radius'.  The north
    // pole is at (0,0,radius) and the south pole is at (0,0,-radius).  The
    // mesh has the topology of an open cylinder (which is also the topology
    // of a rectangle with wrap-around for one pair of parallel edges) and
    // is then stitched to the north and south poles.  The triangles are
    // unevenly distributed.  If you want a more even distribution, create
    // an icosahedron and subdivide it.
    TriMesh* Sphere (int zSamples, int radialSamples, float radius);

    // The torus has center (0,0,0).  If you observe the torus along the
    // line with direction (0,0,1), you will see an annulus.  The circle
    // that is the center of the annulus has radius 'outerRadius'.  The
    // distance from this circle to the boundaries of the annulus is the
    // 'inner radius'.
    TriMesh* Torus (int circleSamples, int radialSamples, float outerRadius,
        float innerRadius);

    // Platonic solids, all inscribed in a unit sphere centered at (0,0,0).
    TriMesh* Tetrahedron ();
    TriMesh* Hexahedron ();
    TriMesh* Octahedron ();
    TriMesh* Dodecahedron ();
    TriMesh* Icosahedron ();

private:
    void TransformData (VertexBufferAccessor& vba);
    void ReverseTriangleOrder (int numTriangles, int* indices);
    void CreatePlatonicNormals (VertexBufferAccessor& vba);
    void CreatePlatonicUVs (VertexBufferAccessor& vba);

    enum { MAX_UNITS = VertexFormat::AM_MAX_TCOORD_UNITS };
    VertexFormatPtr mVFormat;
    Transform mTransform;
    bool mIsStatic, mInside, mHasNormals;
    bool mHasTCoords[MAX_UNITS];
    Buffer::Usage mUsage;
};

}

#endif
