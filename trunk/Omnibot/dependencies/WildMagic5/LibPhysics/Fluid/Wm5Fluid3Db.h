// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLUID3DB_H
#define WM5FLUID3DB_H

#include "Wm5PhysicsLIB.h"
#include "Wm5Vector3.h"

// This class is an implementation based on the ideas in "Real-Time Fluid
// Dynamics for Games", by Jos Stam (GDC 2003 Proceedings), but the code
// is faster in that it processes the source, diffusion, and advection
// terms all at the same time.  The only Gauss-Seidel iteration is in the
// Poisson solver.

namespace Wm5
{

template <typename Real>
class WM5_PHYSICS_ITEM Fluid3Db
{
public:
    // Construction and destruction.
    Fluid3Db (Real x0, Real y0, Real z0, Real x1, Real y1, Real z1, Real dt,
        Real denViscosity, Real velViscosity, int imax, int jmax, int kmax,
        int numGaussSeidelIterations, bool densityDirichlet);

    virtual ~Fluid3Db ();

    // Member access.
    inline Real GetX0 () const;
    inline Real GetY0 () const;
    inline Real GetZ0 () const;
    inline Real GetX1 () const;
    inline Real GetY1 () const;
    inline Real GetZ1 () const;
    inline Real GetDt () const;
    inline Real GetDx () const;
    inline Real GetDy () const;
    inline Real GetDz () const;
    inline Real GetTime () const;
    inline int GetIMax () const;
    inline int GetJMax () const;
    inline int GetKMax () const;
    inline const Real* GetX () const;
    inline const Real* GetY () const;
    inline const Real* GetZ () const;
    inline Real*** GetDensity () const;
    inline Vector3<Real>*** GetVelocity () const;

    // Derived classes must provide initialization at time 0.
    virtual Real InitialDensity (Real x, Real y, Real z,
        int i, int j, int k) = 0;

    virtual Vector3<Real> InitialVelocity (Real x, Real y, Real z,
        int i, int j, int k) = 0;

    // Derived classes must provide source terms.  The density has a
    // source-sink function.  The velocity has a force (impulse) function.
    virtual Real SourceDensity (Real t, Real x, Real y, Real z,
        int i, int j, int k) = 0;

    virtual Vector3<Real> SourceVelocity (Real t, Real x, Real y, Real z,
        int i, int j, int k) = 0;

    void Initialize ();
    void DoSimulationStep ();

protected:
    // Update the boundary values after the interior values have been
    // assigned.
    void DirichletBoundaryZero (Real*** data);
    void NeumannBoundaryZero (Real*** data);
    void UpdateDensityBoundary ();
    void UpdateVelocityBoundary ();

    // Swap the buffer pointers.
    void SwapDensityBuffers ();
    void SwapVelocityBuffers ();

    // Constructor inputs.
    Real mX0, mY0, mZ0, mX1, mY1, mZ1;
    Real mDt;
    Real mDenViscosity, mVelViscosity;
    int mIMax, mJMax, mKMax, mNumGaussSeidelIterations;
    bool mDensityDirichlet;

    // Derived quantities;
    int mIMaxM1, mJMaxM1, mKMaxM1, mIMaxP1, mJMaxP1, mKMaxP1, mNumVoxels;
    Real mDx, mDy, mDz;
    Real mDxDx, mDyDy, mDzDz;
    Real mHalfDivDx, mHalfDivDy, mHalfDivDz;
    Real mDtDivDx, mDtDivDy, mDtDivDz, mDtDivDxDx, mDtDivDyDy, mDtDivDzDz;
    Real mEpsilon0, mEpsilonX, mEpsilonY, mEpsilonZ;
    Real mDenLambdaX, mDenLambdaY, mDenLambdaZ;
    Real mVelLambdaX, mVelLambdaY, mVelLambdaZ;
    Real mDenGamma0, mDenGammaX, mDenGammaY, mDenGammaZ;
    Real mVelGamma0, mVelGammaX, mVelGammaY, mVelGammaZ;

    // Current simulation time.
    Real mTime;

    // Lattice cell centers.
    Real* mX;
    Real* mY;
    Real* mZ;

    // State arrays.
    Real*** mDensity0;
    Real*** mDensity1;
    Vector3<Real>*** mVelocity0;
    Vector3<Real>*** mVelocity1;
    Real*** mDivergence;
    Real*** mPoisson;
};

#include "Wm5Fluid3Db.inl"
}

#endif
