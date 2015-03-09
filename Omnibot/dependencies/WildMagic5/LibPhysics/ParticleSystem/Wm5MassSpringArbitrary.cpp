// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5MassSpringArbitrary.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <class Real, class TVector>
MassSpringArbitrary<Real,TVector>::MassSpringArbitrary (int numParticles,
    int numSprings, Real step)
    :
    ParticleSystem<Real,TVector>(numParticles, step)
{
    mNumSprings = numSprings;
    mSprings = new1<Spring>(mNumSprings);
    memset(mSprings, 0, mNumSprings*sizeof(Spring));

    mAdjacent = new1<std::set<int> >(mNumParticles);
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
MassSpringArbitrary<Real,TVector>::~MassSpringArbitrary ()
{
    delete1(mSprings);
    delete1(mAdjacent);
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
int MassSpringArbitrary<Real,TVector>::GetNumSprings () const
{
    return mNumSprings;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void MassSpringArbitrary<Real,TVector>::SetSpring (int spring, int particle0,
    int particle1, Real constant, Real length)
{
    mSprings[spring].Particle0 = particle0;
    mSprings[spring].Particle1 = particle1;
    mSprings[spring].Constant = constant;
    mSprings[spring].Length = length;

    mAdjacent[particle0].insert(spring);
    mAdjacent[particle1].insert(spring);
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void MassSpringArbitrary<Real,TVector>::GetSpring (int spring, int& particle0,
    int& particle1, Real& constant, Real& length) const
{
    particle0 = mSprings[spring].Particle0;
    particle1 = mSprings[spring].Particle1;
    constant = mSprings[spring].Constant;
    length = mSprings[spring].Length;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real& MassSpringArbitrary<Real,TVector>::Constant (int spring)
{
    return mSprings[spring].Constant;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real& MassSpringArbitrary<Real,TVector>::Length (int spring)
{
    return mSprings[spring].Length;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector MassSpringArbitrary<Real,TVector>::Acceleration (int i, Real time,
    const TVector* positions, const TVector* velocities)
{
    // Compute spring forces on position X[i].  The positions are not
    // necessarily m_akPosition since the RK4 solver in ParticleSystem
    // evaluates the acceleration function at intermediate positions.

    TVector acceleration = ExternalAcceleration(i, time, positions,
        velocities);

    TVector diff, force;
    Real ratio;

    std::set<int>::iterator iter = mAdjacent[i].begin();
    std::set<int>::iterator end = mAdjacent[i].end();
    for (/**/; iter != end; ++iter)
    {
        // Process a spring connected to particle i.
        const Spring& spring = mSprings[*iter];
        if (i != spring.Particle0)
        {
            diff = positions[spring.Particle0] - positions[i];
        }
        else
        {
            diff = positions[spring.Particle1] - positions[i];
        }

        ratio = spring.Length/diff.Length();
        force = spring.Constant*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    return acceleration;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector MassSpringArbitrary<Real,TVector>::ExternalAcceleration (int, Real,
    const TVector*, const TVector*)
{
    return TVector::ZERO;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class MassSpringArbitrary<float,Vector2f>;

template WM5_PHYSICS_ITEM
class MassSpringArbitrary<float,Vector3f>;

template WM5_PHYSICS_ITEM
class MassSpringArbitrary<double,Vector2d>;

template WM5_PHYSICS_ITEM
class MassSpringArbitrary<double,Vector3d>;
//----------------------------------------------------------------------------
}
