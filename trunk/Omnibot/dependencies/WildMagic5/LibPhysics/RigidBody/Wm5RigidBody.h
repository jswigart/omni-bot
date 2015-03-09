// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5RIGIDBODY_H
#define WM5RIGIDBODY_H

#include "Wm5PhysicsLIB.h"
#include "Wm5Matrix3.h"
#include "Wm5Quaternion.h"

namespace Wm5
{

template <typename Real>
class WM5_PHYSICS_ITEM RigidBody
{
public:
    // Construction and destruction.  The rigid body state is uninitialized.
    // Use the set functions to initialize the state before starting the
    // simulation.
    RigidBody ();
    virtual ~RigidBody ();

    // Set/get position.
    Vector3<Real>& Position ();

    // Set rigid body state.
    void SetMass (float mass);
    void SetBodyInertia (const Matrix3<Real>& inertia);
    void SetPosition (const Vector3<Real>& position);
    void SetQOrientation (const Quaternion<Real>& quatOrient);
    void SetLinearMomentum (const Vector3<Real>& linearMomentum);
    void SetAngularMomentum (const Vector3<Real>& angularMomentum);
    void SetROrientation (const Matrix3<Real>& rotOrient);
    void SetLinearVelocity (const Vector3<Real>& linearVelocity);
    void SetAngularVelocity (const Vector3<Real>& angularVelocity);

    // Get rigid body state.
    Real GetMass () const;
    Real GetInverseMass () const;
    const Matrix3<Real>& GetBodyInertia () const;
    const Matrix3<Real>& GetBodyInverseInertia () const;
    Matrix3<Real> GetWorldInertia () const;
    Matrix3<Real> GetWorldInverseInertia () const;
    const Vector3<Real>& GetPosition () const;
    const Quaternion<Real>& GetQOrientation () const;
    const Vector3<Real>& GetLinearMomentum () const;
    const Vector3<Real>& GetAngularMomentum () const;
    const Matrix3<Real>& GetROrientation () const;
    const Vector3<Real>& GetLinearVelocity () const;
    const Vector3<Real>& GetAngularVelocity () const;

    // Force/torque function format.
    typedef Vector3<Real> (*Function)
    (
        Real,                      // time of application
        Real,                      // mass
        const Vector3<Real>&,     // position
        const Quaternion<Real>&,   // orientation
        const Vector3<Real>&,     // linear momentum
        const Vector3<Real>&,     // angular momentum
        const Matrix3<Real>&,  // orientation
        const Vector3<Real>&,     // linear velocity
        const Vector3<Real>&      // angular velocity
    );

    // Force and torque functions.
    Function mForce;
    Function mTorque;

    // Runge-Kutta fourth-order differential equation solver
    void Update (Real t, Real dt);

protected:
    // Constant quantities (matrices in body coordinates).
    Real mMass, mInvMass;
    Matrix3<Real> mInertia, mInvInertia;

    // State variables.
    Vector3<Real> mPosition;
    Quaternion<Real> mQuatOrient;
    Vector3<Real> mLinearMomentum;
    Vector3<Real> mAngularMomentum;

    // Derived state variables.
    Matrix3<Real> mRotOrient;
    Vector3<Real> mLinearVelocity;
    Vector3<Real> mAngularVelocity;
};

typedef RigidBody<float> RigidBodyf;
typedef RigidBody<double> RigidBodyd;

}

#endif
