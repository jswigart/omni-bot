// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5MassSpringCurve.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
MassSpringCurve<Real,TVector>::MassSpringCurve (int numParticles, Real step)
    :
    ParticleSystem<Real,TVector>(numParticles, step)
{
    mNumSprings = mNumParticles - 1;
    mConstants = new1<Real>(mNumSprings);
    mLengths = new1<Real>(mNumSprings);
    memset(mConstants, 0, mNumSprings*sizeof(Real));
    memset(mLengths, 0, mNumSprings*sizeof(Real));
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
MassSpringCurve<Real,TVector>::~MassSpringCurve ()
{
    delete1(mConstants);
    delete1(mLengths);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringCurve<Real,TVector>::GetNumSprings () const
{
    return mNumSprings;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringCurve<Real,TVector>::Constant (int i)
{
    return mConstants[i];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringCurve<Real,TVector>::Length (int i)
{
    return mLengths[i];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector MassSpringCurve<Real,TVector>::Acceleration (int i, Real time,
    const TVector* positions, const TVector* velocities)
{
    // Compute spring forces on position X[i].  The positions are not
    // necessarily m_akPosition since the RK4 solver in ParticleSystem
    // evaluates the acceleration function at intermediate positions.  The end
    // points of the curve of masses must be handled separately since each
    // has only one spring attached to it.

    TVector acceleration = ExternalAcceleration(i, time, positions,
        velocities);

    TVector diff, force;
    Real ratio;

    if (i > 0)
    {
        int iM1 = i-1;
        diff = positions[iM1] - positions[i];
        ratio = mLengths[iM1]/diff.Length();
        force = mConstants[iM1]*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    int iP1 = i+1;
    if (iP1 < mNumParticles)
    {
        diff = positions[iP1] - positions[i];
        ratio = mLengths[i]/diff.Length();
        force = mConstants[i]*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    return acceleration;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector MassSpringCurve<Real,TVector>::ExternalAcceleration (int, Real,
    const TVector*, const TVector*)
{
    return TVector::ZERO;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class MassSpringCurve<float,Vector2f>;

template WM5_PHYSICS_ITEM
class MassSpringCurve<float,Vector3f>;

template WM5_PHYSICS_ITEM
class MassSpringCurve<double,Vector2d>;

template WM5_PHYSICS_ITEM
class MassSpringCurve<double,Vector3d>;
//----------------------------------------------------------------------------
}
