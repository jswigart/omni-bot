// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5RigidBody.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
RigidBody<Real>::RigidBody ()
{
    // The default body is immovable.
    mMass = Math<Real>::MAX_REAL;
    mInvMass = (Real)0;
    mInertia = Matrix3<Real>::IDENTITY;
    mInvInertia = Matrix3<Real>::ZERO;
    mPosition = Vector3<Real>::ZERO;
    mQuatOrient = Quaternion<Real>::IDENTITY;
    mLinearMomentum = Vector3<Real>::ZERO;
    mAngularMomentum = Vector3<Real>::ZERO;
    mRotOrient = Matrix3<Real>::IDENTITY;
    mLinearVelocity = Vector3<Real>::ZERO;
    mAngularVelocity = Vector3<Real>::ZERO;
}
//----------------------------------------------------------------------------
template <typename Real>
RigidBody<Real>::~RigidBody ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>& RigidBody<Real>::Position ()
{
    return mPosition;
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetMass (float mass)
{
    if ((Real)0 < mass && mass < Math<Real>::MAX_REAL)
    {
        mMass = mass;
        mInvMass = ((Real)1)/mass;
    }
    else
    {
        // Assume the body as immovable.
        mMass = Math<Real>::MAX_REAL;
        mInvMass = (Real)0;
        mInertia = Matrix3<Real>::IDENTITY;
        mInvInertia = Matrix3<Real>::ZERO;
        mQuatOrient = Quaternion<Real>::IDENTITY;
        mLinearMomentum = Vector3<Real>::ZERO;
        mAngularMomentum = Vector3<Real>::ZERO;
        mRotOrient = Matrix3<Real>::IDENTITY;
        mLinearVelocity = Vector3<Real>::ZERO;
        mAngularVelocity = Vector3<Real>::ZERO;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetBodyInertia (const Matrix3<Real>& inertia)
{
    mInertia = inertia;
    mInvInertia = mInertia.Inverse();
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetPosition (const Vector3<Real>& position)
{
    mPosition = position;
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetQOrientation (const Quaternion<Real>& quatOrient)
{
    mQuatOrient = quatOrient;
    mQuatOrient.ToRotationMatrix(mRotOrient);
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetLinearMomentum (const Vector3<Real>& linearMomentum)
{
    mLinearMomentum = linearMomentum;
    mLinearVelocity = mInvMass*mLinearMomentum;
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetAngularMomentum (
    const Vector3<Real>& angularMomentum)
{
    mAngularMomentum = angularMomentum;
    mAngularVelocity = mRotOrient * mInvInertia * mRotOrient.Transpose() *
        mAngularMomentum;
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetROrientation (const Matrix3<Real>& rotOrient)
{
    mRotOrient = rotOrient;
    mQuatOrient.FromRotationMatrix(mRotOrient);
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetLinearVelocity (const Vector3<Real>& linearVelocity)
{
    mLinearVelocity = linearVelocity;
    mLinearMomentum = mMass*mLinearVelocity;
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::SetAngularVelocity (
    const Vector3<Real>& angularVelocity)
{
    mAngularVelocity = angularVelocity;
    mAngularMomentum = mRotOrient * mInertia * mRotOrient.Transpose() *
        mAngularVelocity;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RigidBody<Real>::GetMass () const
{
    return mMass;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RigidBody<Real>::GetInverseMass () const
{
    return mInvMass;
}
//----------------------------------------------------------------------------
template <typename Real>
const Matrix3<Real>& RigidBody<Real>::GetBodyInertia () const
{
    return mInertia;
}
//----------------------------------------------------------------------------
template <typename Real>
const Matrix3<Real>& RigidBody<Real>::GetBodyInverseInertia () const
{
    return mInvInertia;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> RigidBody<Real>::GetWorldInertia () const
{
    return mRotOrient*mInertia*mRotOrient.Transpose();
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> RigidBody<Real>::GetWorldInverseInertia () const
{
    return mRotOrient*mInvInertia*mRotOrient.Transpose();
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& RigidBody<Real>::GetPosition () const
{
    return mPosition;
}
//----------------------------------------------------------------------------
template <typename Real>
const Quaternion<Real>& RigidBody<Real>::GetQOrientation () const
{
    return mQuatOrient;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& RigidBody<Real>::GetLinearMomentum () const
{
    return mLinearMomentum;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& RigidBody<Real>::GetAngularMomentum () const
{
    return mAngularMomentum;
}
//----------------------------------------------------------------------------
template <typename Real>
const Matrix3<Real>& RigidBody<Real>::GetROrientation () const
{
    return mRotOrient;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& RigidBody<Real>::GetLinearVelocity () const
{
    return mLinearVelocity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& RigidBody<Real>::GetAngularVelocity () const
{
    return mAngularVelocity;
}
//----------------------------------------------------------------------------
template <typename Real>
void RigidBody<Real>::Update (Real t, Real dt)
{
    Real halfDT = ((Real)0.5)*dt;
    Real sixthDT = dt/((Real)6);
    Real TpHalfDT = t + halfDT;
    Real TpDT = t + dt;

    Vector3<Real> newPosition, newLinearMomentum, newAngularMomentum,
        newLinearVelocity, newAngularVelocity;
    Quaternion<Real> newQuatOrient;
    Matrix3<Real> newRotOrient;

    // A1 = G(T,S0), B1 = S0 + (DT/2)*A1
    Vector3<Real> A1DXDT = mLinearVelocity;
    Quaternion<Real> W = Quaternion<Real>((Real)0, mAngularVelocity.X(),
        mAngularVelocity.Y(), mAngularVelocity.Z());
    Quaternion<Real> A1DQDT = ((Real)0.5)*W*mQuatOrient;

    Vector3<Real> A1DPDT = mForce(t, mMass, mPosition, mQuatOrient,
        mLinearMomentum, mAngularMomentum, mRotOrient, mLinearVelocity,
        mAngularVelocity);

    Vector3<Real> A1DLDT = mTorque(t, mMass, mPosition, mQuatOrient,
        mLinearMomentum, mAngularMomentum, mRotOrient, mLinearVelocity,
        mAngularVelocity);

    newPosition = mPosition + halfDT*A1DXDT;
    newQuatOrient = mQuatOrient + halfDT*A1DQDT;
    newLinearMomentum = mLinearMomentum + halfDT*A1DPDT;
    newAngularMomentum = mAngularMomentum + halfDT*A1DLDT;
    newQuatOrient.ToRotationMatrix(newRotOrient);
    newLinearVelocity = mInvMass*newLinearMomentum;
    newAngularVelocity = newRotOrient*mInvInertia*newRotOrient.Transpose()*
        newAngularMomentum;

    // A2 = G(T+DT/2,B1), B2 = S0 + (DT/2)*A2
    Vector3<Real> A2DXDT = newLinearVelocity;
    W = Quaternion<Real>((Real)0, newAngularVelocity.X(),
        newAngularVelocity.Y(), newAngularVelocity.Z());
    Quaternion<Real> A2DQDT = ((Real)0.5)*W*newQuatOrient;

    Vector3<Real> A2DPDT = mForce(TpHalfDT, mMass, newPosition,
        newQuatOrient, newLinearMomentum, newAngularMomentum, newRotOrient,
        newLinearVelocity, newAngularVelocity);

    Vector3<Real> A2DLDT = mTorque(TpHalfDT, mMass, newPosition,
        newQuatOrient, newLinearMomentum, newAngularMomentum, newRotOrient,
        newLinearVelocity, newAngularVelocity);

    newPosition = mPosition + halfDT*A2DXDT;
    newQuatOrient = mQuatOrient + halfDT*A2DQDT;
    newLinearMomentum = mLinearMomentum + halfDT*A2DPDT;
    newAngularMomentum = mAngularMomentum + halfDT*A2DLDT;
    newQuatOrient.ToRotationMatrix(newRotOrient);
    newLinearVelocity = mInvMass*newLinearMomentum;
    newAngularVelocity = newRotOrient*mInvInertia*newRotOrient.Transpose()*
        newAngularMomentum;

    // A3 = G(T+DT/2,B2), B3 = S0 + DT*A3
    Vector3<Real> A3DXDT = newLinearVelocity;
    W = Quaternion<Real>((Real)0, newAngularVelocity.X(),
        newAngularVelocity.Y(), newAngularVelocity.Z());
    Quaternion<Real> A3DQDT = ((Real)0.5)*W*newQuatOrient;

    Vector3<Real> A3DPDT = mForce(TpHalfDT, mMass, newPosition,
        newQuatOrient, newLinearMomentum, newAngularMomentum, newRotOrient,
        newLinearVelocity, newAngularVelocity);

    Vector3<Real> A3DLDT = mTorque(TpHalfDT, mMass, newPosition,
        newQuatOrient, newLinearMomentum, newAngularMomentum, newRotOrient,
        newLinearVelocity, newAngularVelocity);

    newPosition = mPosition + dt*A3DXDT;
    newQuatOrient = mQuatOrient + dt*A3DQDT;
    newLinearMomentum = mLinearMomentum + dt*A3DPDT;
    newAngularMomentum = mAngularMomentum + dt*A3DLDT;
    newQuatOrient.ToRotationMatrix(newRotOrient);
    newLinearVelocity = mInvMass*newLinearMomentum;
    newAngularVelocity = newRotOrient*mInvInertia*newRotOrient.Transpose()*
        newAngularMomentum;

    // A4 = G(T+DT,B3), S1 = S0 + (DT/6)*(A1+2*(A2+A3)+A4)
    Vector3<Real> A4DXDT = newLinearVelocity;
    W = Quaternion<Real>((Real)0, newAngularVelocity.X(),
        newAngularVelocity.Y(), newAngularVelocity.Z());
    Quaternion<Real> A4DQDT = ((Real)0.5)*W*newQuatOrient;

    Vector3<Real> A4DPDT = mForce(TpDT, mMass, newPosition,
        newQuatOrient, newLinearMomentum, newAngularMomentum, newRotOrient,
        newLinearVelocity, newAngularVelocity);

    Vector3<Real> A4DLDT = mTorque(TpDT, mMass, newPosition, newQuatOrient,
        newLinearMomentum, newAngularMomentum, newRotOrient, newLinearVelocity,
        newAngularVelocity);

    mPosition = mPosition + sixthDT*(A1DXDT +
        ((Real)2)*(A2DXDT + A3DXDT) + A4DXDT);

    mQuatOrient = mQuatOrient + sixthDT*(A1DQDT +
        ((Real)2)*(A2DQDT + A3DQDT) + A4DQDT);

    mLinearMomentum = mLinearMomentum + sixthDT*(A1DPDT +
        ((Real)2)*(A2DPDT + A3DPDT) + A4DPDT);

    mAngularMomentum = mAngularMomentum + sixthDT*(A1DLDT +
        ((Real)2)*(A2DLDT + A3DLDT) + A4DLDT);

    mQuatOrient.ToRotationMatrix(mRotOrient);
    mLinearVelocity = mInvMass*mLinearMomentum;
    mAngularVelocity = mRotOrient*mInvInertia*mRotOrient.Transpose()*
        mAngularMomentum;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class RigidBody<float>;

template WM5_PHYSICS_ITEM
class RigidBody<double>;
//----------------------------------------------------------------------------
}
