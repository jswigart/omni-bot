// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5ParticleSystem.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
ParticleSystem<Real,TVector>::ParticleSystem (int numParticles, Real step)
{
    mNumParticles = numParticles;
    SetStep(step);

    mMasses = new1<Real>(mNumParticles);
    mInvMasses = new1<Real>(mNumParticles);
    mPositions = new1<TVector>(mNumParticles);
    mVelocities = new1<TVector>(mNumParticles);

    memset(mMasses, 0, mNumParticles*sizeof(Real));
    memset(mInvMasses, 0, mNumParticles*sizeof(Real));
    memset(mPositions, 0, mNumParticles*sizeof(TVector));
    memset(mVelocities ,0 ,mNumParticles*sizeof(TVector));

    mPTmp = new1<TVector>(mNumParticles);
    mDPTmp1 = new1<TVector>(mNumParticles);
    mDPTmp2 = new1<TVector>(mNumParticles);
    mDPTmp3 = new1<TVector>(mNumParticles);
    mDPTmp4 = new1<TVector>(mNumParticles);
    mVTmp = new1<TVector>(mNumParticles);
    mDVTmp1 = new1<TVector>(mNumParticles);
    mDVTmp2 = new1<TVector>(mNumParticles);
    mDVTmp3 = new1<TVector>(mNumParticles);
    mDVTmp4 = new1<TVector>(mNumParticles);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
ParticleSystem<Real,TVector>::~ParticleSystem ()
{
    delete1(mMasses);
    delete1(mInvMasses);
    delete1(mPositions);
    delete1(mVelocities);
    delete1(mPTmp);
    delete1(mDPTmp1);
    delete1(mDPTmp2);
    delete1(mDPTmp3);
    delete1(mDPTmp4);
    delete1(mVTmp);
    delete1(mDVTmp1);
    delete1(mDVTmp2);
    delete1(mDVTmp3);
    delete1(mDVTmp4);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int ParticleSystem<Real,TVector>::GetNumParticles () const
{
    return mNumParticles;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void ParticleSystem<Real,TVector>::SetMass (int i, Real mass)
{
    if ((Real)0 < mass && mass < Math<Real>::MAX_REAL)
    {
        mMasses[i] = mass;
        mInvMasses[i] = ((Real)1)/mass;
    }
    else
    {
        mMasses[i] = Math<Real>::MAX_REAL;
        mInvMasses[i] = (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real ParticleSystem<Real,TVector>::GetMass (int i) const
{
    return mMasses[i];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector* ParticleSystem<Real,TVector>::Positions () const
{
    return mPositions;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector& ParticleSystem<Real,TVector>::Position (int i)
{
    return mPositions[i];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector* ParticleSystem<Real,TVector>::Velocities () const
{
    return mVelocities;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector& ParticleSystem<Real,TVector>::Velocity (int i)
{
    return mVelocities[i];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void ParticleSystem<Real,TVector>::SetStep (Real step)
{
    mStep = step;
    mHalfStep = ((Real)0.5)*mStep;
    mSixthStep = mStep/(Real)6;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real ParticleSystem<Real,TVector>::GetStep () const
{
    return mStep;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void ParticleSystem<Real,TVector>::Update (Real time)
{
    // Runge-Kutta fourth-order solver.
    Real halfTime = time + mHalfStep;
    Real fullTime = time + mStep;

    // Compute the first step.
    int i;
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mDPTmp1[i] = mVelocities[i];
            mDVTmp1[i] = Acceleration(i, time, mPositions, mVelocities);
        }
    }
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mPTmp[i] = mPositions[i] + mHalfStep*mDPTmp1[i];
            mVTmp[i] = mVelocities[i] + mHalfStep*mDVTmp1[i];
        }
        else
        {
            mPTmp[i] = mPositions[i];
            mVTmp[i] = TVector::ZERO;
        }
    }

    // Compute the second step.
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mDPTmp2[i] = mVTmp[i];
            mDVTmp2[i] = Acceleration(i, halfTime, mPTmp, mVTmp);
        }
    }
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mPTmp[i] = mPositions[i] + mHalfStep*mDPTmp2[i];
            mVTmp[i] = mVelocities[i] + mHalfStep*mDVTmp2[i];
        }
        else
        {
            mPTmp[i] = mPositions[i];
            mVTmp[i] = TVector::ZERO;
        }
    }

    // Compute the third step.
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mDPTmp3[i] = mVTmp[i];
            mDVTmp3[i] = Acceleration(i, halfTime, mPTmp, mVTmp);
        }
    }
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mPTmp[i] = mPositions[i] + mStep*mDPTmp3[i];
            mVTmp[i] = mVelocities[i] + mStep*mDVTmp3[i];
        }
        else
        {
            mPTmp[i] = mPositions[i];
            mVTmp[i] = TVector::ZERO;
        }
    }

    // Compute the fourth step.
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mDPTmp4[i] = mVTmp[i];
            mDVTmp4[i] = Acceleration(i, fullTime, mPTmp, mVTmp);
        }
    }
    for (i = 0; i < mNumParticles; ++i)
    {
        if (mInvMasses[i] > (Real)0)
        {
            mPositions[i] += mSixthStep*(mDPTmp1[i] +
                ((Real)2)*(mDPTmp2[i] + mDPTmp3[i]) + mDPTmp4[i]);
            mVelocities[i] += mSixthStep*(mDVTmp1[i] +
                ((Real)2)*(mDVTmp2[i] + mDVTmp3[i]) + mDVTmp4[i]);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class ParticleSystem<float,Vector2f>;

template WM5_PHYSICS_ITEM
class ParticleSystem<float,Vector3f>;

template WM5_PHYSICS_ITEM
class ParticleSystem<double,Vector2d>;

template WM5_PHYSICS_ITEM
class ParticleSystem<double,Vector3d>;
//----------------------------------------------------------------------------
}
