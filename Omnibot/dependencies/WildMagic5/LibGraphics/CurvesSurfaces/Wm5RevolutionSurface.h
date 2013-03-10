// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5REVOLUTIONSURFACE_H
#define WM5REVOLUTIONSURFACE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5TriMesh.h"
#include "Wm5Curve2.h"
#include "Wm5Float3.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM RevolutionSurface : public TriMesh
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(RevolutionSurface);

public:
    // The axis of revolution is the z-axis.  You can obtain arbitrary axes
    // after the fact by applying translations and rotations to the object.
    // The curve of revolution is (x(t),z(t)) with t in [tmin,tmax] and
    // z(t) > 0.  It is also assumed to be non-self-intersecting.  When the
    // curve is open, three cases to consider:  If z(tmin) and z(tmax) are
    // both positive, the surface topology is that of a cylinder (use
    // REV_CYLINDER_TOPOLOGY).  If exactly one of z(tmin) or z(tmax) is zero,
    // the surface topology is that of a disk (use REV_DISK_TOPOLOGY).  If
    // z(tmin) and z(tmax) are both zero, the surface topology is that of a
    // sphere (use REV_SPHERE_TOPOLOGY).  When the curve of revolution is
    // closed so that (x(tmin),z(tmin)) and (x(tmax),z(tmax)) are the same
    // point, the surface topology is that of a torus.  For now, rather than
    // having the surface object determine the type of curve, use the
    // Topology enumerated values to provide this information.

    enum WM5_GRAPHICS_ITEM TopologyType
    {
        REV_DISK_TOPOLOGY,
        REV_CYLINDER_TOPOLOGY,
        REV_SPHERE_TOPOLOGY,
        REV_TORUS_TOPOLOGY,
        MAX_TOPOLOGY_TYPES
    };

    // Construction and destruction.  The caller is responsible for deleting
    // the input curve.
    RevolutionSurface (Curve2f* curve, float xCenter, TopologyType topology,
        int numCurveSamples, int numRadialSamples, bool sampleByArcLength,
        bool outsideView, VertexFormat* vformat);

    virtual ~RevolutionSurface ();

    // Mmber access.
    inline int GetCurveSamples () const;
    inline int GetRadialSamples () const;
    inline void SetCurve (Curve2f* curve);
    inline const Curve2f* GetCurve () const;
    inline TopologyType GetTopology () const;
    inline void SetSampleByArcLength (bool sampleByArcLength);
    inline bool GetSampleByArcLength () const;

    // Modify vertices and normals when the curve itself changes over time.
    // You are responsible for maintaining the topology of the curve.  For
    // example, if your constructor input was REV_SPHERE_TOPOLOGY, you should
    // not change the curve to produce a non-spherical topology.
    void UpdateSurface ();

protected:
    void ComputeSampleData ();
    void UpdateDisk ();
    void UpdateCylinder ();
    void UpdateSphere ();
    void UpdateTorus ();

    Curve2f* mCurve;
    float mXCenter;
    TopologyType mTopology;
    int mNumCurveSamples, mNumRadialSamples;
    float* mSin;
    float* mCos;
    Float3* mSamples;
    bool mSampleByArcLength;
};

WM5_REGISTER_STREAM(RevolutionSurface);
typedef Pointer0<RevolutionSurface> RevolutionSurfacePtr;
#include "Wm5RevolutionSurface.inl"

}

#endif
