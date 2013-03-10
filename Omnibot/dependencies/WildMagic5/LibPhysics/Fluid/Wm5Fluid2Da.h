// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLUID2DA_H
#define WM5FLUID2DA_H

#include "Wm5PhysicsLIB.h"
#include "Wm5Vector2.h"

// This class is an implementation of the algorithm in "Real-Time Fluid
// Dynamics for Games", by Jos Stam (GDC 2003 Proceedings).  There are
// some modifications to what is described in the paper.

namespace Wm5
{

template <typename Real>
class WM5_PHYSICS_ITEM Fluid2Da
{
public:
    // Construction and destruction.
    Fluid2Da (Real x0, Real y0, Real x1, Real y1, Real dt, Real denViscosity,
        Real velViscosity, int imax, int jmax, int numGaussSeidelIterations,
        bool densityDirichlet);

    virtual ~Fluid2Da ();

    // Member access.
    inline Real GetX0 () const;
    inline Real GetY0 () const;
    inline Real GetX1 () const;
    inline Real GetY1 () const;
    inline Real GetDt () const;
    inline Real GetDx () const;
    inline Real GetDy () const;
    inline Real GetTime () const;
    inline int GetIMax () const;
    inline int GetJMax () const;
    inline const Real* GetX () const;
    inline const Real* GetY () const;
    inline Real** GetDensity () const;
    inline Vector2<Real>** GetVelocity () const;

    // Derived classes must provide initialization at time 0.
    virtual Real InitialDensity (Real x, Real y,
        int i, int j) = 0;

    virtual Vector2<Real> InitialVelocity (Real x, Real y,
        int i, int j) = 0;

    // Derived classes must provide source terms.  The density has a
    // source-sink function.  The velocity has a force (impulse) function.
    virtual Real SourceDensity (Real t, Real x, Real y,
        int i, int j) = 0;

    virtual Vector2<Real> SourceVelocity (Real t, Real x, Real y,
        int i, int j) = 0;

    void Initialize ();
    void DoSimulationStep ();

protected:
    // State updates.
    void UpdateDensitySource ();
    void UpdateDensityDiffusion ();
    void UpdateDensityAdvection ();
    void UpdateVelocitySource ();
    void UpdateVelocityDiffusion ();
    void UpdateVelocityAdvection ();
    void GetLerpInfo (int i, int j,
        int& i0, int& i1, Real& a0, Real& a1,
        int& j0, int& j1, Real& b0, Real& b1);

    // Use the Helmholtz decomposition to subtract out the gradient field.
    void AdjustVelocity ();

    // Update the boundary values after the interior values have been
    // assigned.
    void DirichletBoundaryZero (Real** data);
    void NeumannBoundaryZero (Real** data);
    void UpdateDensityBoundary ();
    void UpdateVelocityBoundary ();

    // Swap the buffer pointers.
    void SwapDensityBuffers ();
    void SwapVelocityBuffers ();

    // Constructor inputs.
    Real mX0, mY0, mX1, mY1;
    Real mDt;
    Real mDenViscosity, mVelViscosity;
    int mIMax, mJMax;
    int mNumGaussSeidelIterations;
    bool mDensityDirichlet;

    // Derived quantities;
    int mIMaxM1, mJMaxM1, mIMaxP1, mJMaxP1, mKMaxP1, mNumPixels;
    Real mDx, mDy;
    Real mDxDx, mDyDy;
    Real mHalfDivDx, mHalfDivDy;
    Real mDtDivDx, mDtDivDy, mDtDivDxDx, mDtDivDyDy;
    Real mEpsilon0, mEpsilonX, mEpsilonY;
    Real mDenLambdaX, mDenLambdaY;
    Real mVelLambdaX, mVelLambdaY;
    Real mDenGamma0, mDenGammaX, mDenGammaY;
    Real mVelGamma0, mVelGammaX, mVelGammaY;

    // Current simulation time.
    Real mTime;

    // Lattice cell centers.
    Real* mX;
    Real* mY;

    // State arrays.
    Real** mDensity0;
    Real** mDensity1;
    Vector2<Real>** mVelocity0;
    Vector2<Real>** mVelocity1;
    Real** mDivergence;
    Real** mPoisson;
};

#include "Wm5Fluid2Da.inl"

}

#endif
