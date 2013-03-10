// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PARTICLESYSTEM_H
#define WM5PARTICLESYSTEM_H

#include "Wm5PhysicsLIB.h"
#include "Wm5Vector2.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real, typename TVector>
class WM5_PHYSICS_ITEM ParticleSystem
{
public:
    // Construction and destruction.  If a particle is to be immovable, set
    // its mass to Math<Real>::MAX_REAL.
    ParticleSystem (int numParticles, Real step);
    virtual ~ParticleSystem ();

    int GetNumParticles () const;
    void SetMass (int i, Real mass);
    Real GetMass (int i) const;
    TVector* Positions () const;
    TVector& Position (int i);
    TVector* Velocities () const;
    TVector& Velocity (int i);
    void SetStep (Real step);
    Real GetStep () const;

    // Callback for acceleration (ODE solver uses x" = F/m) applied to
    // particle i.  The positions and velocities are not necessarily
    // mPositions and mVelocities since the ODE solver evaluates the
    // impulse function at intermediate positions.
    virtual TVector Acceleration (int i, Real time,
        const TVector* positions, const TVector* velocities) = 0;

    // Update the particle positions based on current time and particle state.
    // The Acceleration(...) function is called in this update for each
    // particle.  This function is virtual so that derived classes can
    // perform pre-update and/or post-update semantics.
    virtual void Update (Real time);

protected:
    int mNumParticles;
    Real* mMasses;
    Real* mInvMasses;
    TVector* mPositions;
    TVector* mVelocities;
    Real mStep, mHalfStep, mSixthStep;

    // Temporary storage for solver.
    typedef TVector* TVectorPtr;
    TVectorPtr mPTmp, mDPTmp1, mDPTmp2, mDPTmp3, mDPTmp4;
    TVectorPtr mVTmp, mDVTmp1, mDVTmp2, mDVTmp3, mDVTmp4;
};

typedef ParticleSystem<float,Vector2f> ParticleSystem2f;
typedef ParticleSystem<double,Vector2d> ParticleSystem2d;
typedef ParticleSystem<float,Vector3f> ParticleSystem3f;
typedef ParticleSystem<double,Vector3d> ParticleSystem3d;

}

#endif
