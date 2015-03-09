// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PARTICLECONTROLLER_H
#define WM5PARTICLECONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Controller.h"
#include "Wm5Particles.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ParticleController : public Controller
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ParticleController);

protected:
    // Abstract base class.  Construction and destruction.  The object to
    // which this is attached must be Particles.
    ParticleController ();
public:
    virtual ~ParticleController ();

    // The system motion, in local coordinates.  The velocity vectors should
    // be unit length.
    float SystemLinearSpeed;
    float SystemAngularSpeed;
    AVector SystemLinearAxis;
    AVector SystemAngularAxis;
    float SystemSizeChange;

    // Particle motion, in the model space of the system.  The velocity
    // vectors should be unit length.  In applications where the points
    // represent a rigid body, you might choose the origin of the system to
    // be the center of mass of the particles and the coordinate axes to
    // correspond to the principal directions of the inertia tensor.
    inline int GetNumParticles () const;
    inline float* GetParticleLinearSpeed () const;
    inline AVector* GetParticleLinearAxis () const;
    inline float* GetParticleSizeChange () const;

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

protected:
    // For deferred allocation of the particle motion arrays.
    void Reallocate (int numParticles);
    virtual void SetObject (ControlledObject* object);

    // This class computes the new positions and orientations from the motion
    // parameters.  Derived classes should update the motion parameters and
    // then either call the base class update methods or provide its own
    // update methods for position and orientation.
    virtual void UpdateSystemMotion (float ctrlTime);
    virtual void UpdatePointMotion (float ctrlTime);

    int mNumParticles;
    float* mParticleLinearSpeed;
    AVector* mParticleLinearAxis;
    float* mParticleSizeChange;
};

WM5_REGISTER_STREAM(ParticleController);
typedef Pointer0<ParticleController> ParticleControllerPtr;
#include "Wm5ParticleController.inl"

}

#endif
