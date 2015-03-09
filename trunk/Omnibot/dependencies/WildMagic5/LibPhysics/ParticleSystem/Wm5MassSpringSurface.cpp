// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5MassSpringSurface.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
MassSpringSurface<Real,TVector>::MassSpringSurface (int numRows, int numCols,
    Real step)
    :
    ParticleSystem<Real,TVector>(numRows*numCols, step)
{
    mNumRows = numRows;
    mNumCols = numCols;
    mNumRowsM1 = mNumRows - 1;
    mNumColsM1 = mNumCols - 1;

    mPositionGrid = new1<TVector*>(mNumRows);
    mVelocityGrid = new1<TVector*>(mNumRows);
    for (int row = 0; row < mNumRows; ++row)
    {
        mPositionGrid[row] = &mPositions[mNumCols*row];
        mVelocityGrid[row] = &mVelocities[mNumCols*row];
    }

    mConstantsR = new2<Real>(mNumCols, mNumRowsM1);
    mLengthsR = new2<Real>(mNumCols, mNumRowsM1);
    mConstantsC = new2<Real>(mNumColsM1, mNumRows);
    mLengthsC = new2<Real>(mNumColsM1, mNumRows);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
MassSpringSurface<Real,TVector>::~MassSpringSurface ()
{
    delete1(mPositionGrid);
    delete1(mVelocityGrid);
    delete2(mConstantsR);
    delete2(mLengthsR);
    delete2(mConstantsC);
    delete2(mLengthsC);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringSurface<Real,TVector>::GetNumRows () const
{
    return mNumRows;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringSurface<Real,TVector>::GetNumCols () const
{
    return mNumCols;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void MassSpringSurface<Real,TVector>::SetMass (int row, int col, Real mass)
{
    ParticleSystem<Real,TVector>::SetMass(GetIndex(row, col), mass);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real MassSpringSurface<Real,TVector>::GetMass (int row, int col) const
{
    return ParticleSystem<Real,TVector>::GetMass(GetIndex(row, col));
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector** MassSpringSurface<Real,TVector>::Positions2D () const
{
    return mPositionGrid;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector& MassSpringSurface<Real,TVector>::Position (int row, int col)
{
    return ParticleSystem<Real,TVector>::Position(GetIndex(row, col));
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector** MassSpringSurface<Real,TVector>::Velocities2D () const
{
    return mVelocityGrid;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector& MassSpringSurface<Real,TVector>::Velocity (int row, int col)
{
    return ParticleSystem<Real,TVector>::Velocity(GetIndex(row, col));
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringSurface<Real,TVector>::ConstantR (int row, int col)
{
    return mConstantsR[row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringSurface<Real,TVector>::LengthR (int row, int col)
{
    return mLengthsR[row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringSurface<Real,TVector>::ConstantC (int row, int col)
{
    return mConstantsC[row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringSurface<Real,TVector>::LengthC (int row, int col)
{
    return mLengthsC[row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector MassSpringSurface<Real,TVector>::Acceleration (int i, Real time,
    const TVector* positions, const TVector* velocities)
{
    // Compute spring forces on position X[i].  The positions are not
    // necessarily mPositions since the RK4 solver in ParticleSystem
    // evaluates the acceleration function at intermediate positions.  The
    // edge and corner points of the surface of masses must be handled
    // separately since each has fewer than four springs attached to it.

    TVector acceleration = ExternalAcceleration(i,time,positions,
        velocities);

    TVector diff, force;
    Real ratio;

    int row, col, prev, next;
    GetCoordinates(i, row, col);

    if (row > 0)
    {
        prev = i - mNumCols;  // index to previous row-neighbor
        diff = positions[prev] - positions[i];
        ratio = LengthR(row - 1, col)/diff.Length();
        force = ConstantR(row - 1, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (row < mNumRowsM1)
    {
        next = i + mNumCols;  // index to next row-neighbor
        diff = positions[next] - positions[i];
        ratio = LengthR(row, col)/diff.Length();
        force = ConstantR(row, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (col > 0)
    {
        prev = i - 1;  // index to previous col-neighbor
        diff = positions[prev] - positions[i];
        ratio = LengthC(row, col - 1)/diff.Length();
        force = ConstantC(row, col - 1)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (col < mNumColsM1)
    {
        next = i + 1;  // index to next col-neighbor
        diff = positions[next] - positions[i];
        ratio = LengthC(row, col)/diff.Length();
        force = ConstantC(row, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    return acceleration;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector MassSpringSurface<Real,TVector>::ExternalAcceleration (int, Real,
    const TVector*, const TVector*)
{
    return TVector::ZERO;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringSurface<Real,TVector>::GetIndex (int row, int col) const
{
    return col + mNumCols*row;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void MassSpringSurface<Real,TVector>::GetCoordinates (int i, int& row,
    int& col) const
{
    col = i % mNumCols;
    row = i / mNumCols;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class MassSpringSurface<float,Vector2f>;

template WM5_PHYSICS_ITEM
class MassSpringSurface<float,Vector3f>;

template WM5_PHYSICS_ITEM
class MassSpringSurface<double,Vector2d>;

template WM5_PHYSICS_ITEM
class MassSpringSurface<double,Vector3d>;
//----------------------------------------------------------------------------
}
