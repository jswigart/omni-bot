// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PARTICLES_H
#define WM5PARTICLES_H

#include "Wm5GraphicsLIB.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class Camera;
class Culler;

class WM5_GRAPHICS_ITEM Particles : public TriMesh
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Particles);

public:
    // Construction and destruction.  The VertexFormat object must have
    // 3-tuple positions.  It must also have 2-tuple texture coordinates in
    // channel zero; these are set to the standard ones (unit square per
    // quadrilateral).  The number of elements of vbuffer must be a multiple
    // of 4.  The number of elements of particles is 1/4 of the number of
    // elements of vbuffer.  The index buffer is automatically generated.
    // The 'positionSizes' contain position in the first three components
    // and size in the fourth component.
    Particles (VertexFormat* vformat, VertexBuffer* vbuffer, int indexSize,
        Float4* positionSizes, float sizeAdjust);

    virtual ~Particles ();

    // Member access.
    inline int GetNumParticles () const;
    inline const Float4* GetPositionSizes () const;
    inline Float4* GetPositionSizes ();
    void SetSizeAdjust (float sizeAdjust);
    inline float GetSizeAdjust () const;

    // Allow the application to specify fewer than the maximum number of
    // vertices to draw.
    void SetNumActive (int numActive);
    inline int GetNumActive () const;

    // The particles are billboards that always face the camera.
    void GenerateParticles (const Camera* camera);

protected:
    // Support for hierarchical culling.
    virtual void GetVisibleSet (Culler& culler, bool noCull);

    int mNumParticles;
    Float4* mPositionSizes;
    float mSizeAdjust;
    int mNumActive;
};

WM5_REGISTER_STREAM(Particles);
typedef Pointer0<Particles> ParticlesPtr;
#include "Wm5Particles.inl"

}

#endif
