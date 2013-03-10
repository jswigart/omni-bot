// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5MassSpringVolume.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
MassSpringVolume<Real,TVector>::MassSpringVolume (int numSlices, int numRows,
    int numCols, Real step)
    :
    ParticleSystem<Real,TVector>(numSlices*numRows*numCols, step)
{
    mNumSlices = numSlices;
    mNumRows = numRows;
    mNumCols = numCols;
    mSliceQuantity = mNumRows*mNumCols;
    mNumSlicesM1 = mNumSlices - 1;
    mNumRowsM1 = mNumRows - 1;
    mNumColsM1 = mNumCols - 1;

    mPositionGrid = new1<TVector**>(mNumSlices);
    mVelocityGrid = new1<TVector**>(mNumSlices);
    for (int slice = 0; slice < mNumSlices; ++slice)
    {
        mPositionGrid[slice] = new1<TVector*>(mNumRows);
        mVelocityGrid[slice] = new1<TVector*>(mNumRows);
        for (int row = 0; row < mNumRows; ++row)
        {
            int i = mNumCols*(row + mNumRows*slice);
            mPositionGrid[slice][row] = &mPositions[i];
            mVelocityGrid[slice][row] = &mVelocities[i];
        }
    }

    mConstantsS = new3<Real>(mNumCols, mNumRows, mNumSlicesM1);
    mLengthsS = new3<Real>(mNumCols, mNumRows, mNumSlicesM1);
    mConstantsR = new3<Real>(mNumCols, mNumRowsM1, mNumSlices);
    mLengthsR = new3<Real>(mNumCols, mNumRowsM1, mNumSlices);
    mConstantsC = new3<Real>(mNumColsM1, mNumRows, mNumSlices);
    mLengthsC = new3<Real>(mNumColsM1, mNumRows, mNumSlices);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
MassSpringVolume<Real,TVector>::~MassSpringVolume ()
{
    for (int slice = 0; slice < mNumSlices; ++slice)
    {
        delete1(mPositionGrid[slice]);
        delete1(mVelocityGrid[slice]);
    }
    delete1(mPositionGrid);
    delete1(mVelocityGrid);

    delete3(mConstantsS);
    delete3(mLengthsS);
    delete3(mConstantsR);
    delete3(mLengthsR);
    delete3(mConstantsC);
    delete3(mLengthsC);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringVolume<Real,TVector>::GetNumSlices () const
{
    return mNumSlices;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringVolume<Real,TVector>::GetNumRows () const
{
    return mNumRows;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringVolume<Real,TVector>::GetNumCols () const
{
    return mNumCols;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void MassSpringVolume<Real,TVector>::SetMass (int slice, int row, int col,
    Real mass)
{
    int i = GetIndex(slice, row, col);
    ParticleSystem<Real,TVector>::SetMass(i, mass);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real MassSpringVolume<Real,TVector>::GetMass (int slice, int row, int col)
    const
{
    int i = GetIndex(slice, row, col);
    return ParticleSystem<Real,TVector>::GetMass(i);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector*** MassSpringVolume<Real,TVector>::Positions3D () const
{
    return mPositionGrid;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector& MassSpringVolume<Real,TVector>::Position (int slice, int row,
    int col)
{
    int i = GetIndex(slice, row, col);
    return ParticleSystem<Real,TVector>::Position(i);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector*** MassSpringVolume<Real,TVector>::Velocities3D () const
{
    return mVelocityGrid;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector& MassSpringVolume<Real,TVector>::Velocity (int slice, int row,
    int col)
{
    int i = GetIndex(slice, row, col);
    return ParticleSystem<Real,TVector>::Velocity(i);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringVolume<Real,TVector>::ConstantS (int slice, int row, int col)
{
    return mConstantsS[slice][row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringVolume<Real,TVector>::LengthS (int slice, int row, int col)
{
    return mLengthsS[slice][row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringVolume<Real,TVector>::ConstantR (int slice, int row, int col)
{
    return mConstantsR[slice][row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringVolume<Real,TVector>::LengthR (int slice, int row, int col)
{
    return mLengthsR[slice][row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringVolume<Real,TVector>::ConstantC (int slice, int row, int col)
{
    return mConstantsC[slice][row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real& MassSpringVolume<Real,TVector>::LengthC (int slice, int row, int col)
{
    return mLengthsC[slice][row][col];
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector MassSpringVolume<Real,TVector>::Acceleration (int i, Real time,
    const TVector* positions, const TVector* velocities)
{
    // Compute spring forces on position X[i].  The positions are not
    // necessarily mPositions since the RK4 solver in ParticleSystem
    // evaluates the acceleration function at intermediate positions.  The
    // face, edge, and corner points of the volume of masses must be handled
    // separately since each has fewer than eight spring attached to it.

    TVector acceleration = ExternalAcceleration(i, time, positions,
        velocities);

    TVector diff, force;
    Real ratio;

    int slice, row, col, prev, next;
    GetCoordinates(i, slice, row, col);

    if (slice > 0)
    {
        prev = i - mSliceQuantity;  // index to previous slice-neighbor
        diff = positions[prev] - positions[i];
        ratio = LengthS(slice - 1, row, col)/diff.Length();
        force = ConstantS(slice - 1, row, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (slice < mNumSlicesM1)
    {
        next = i + mSliceQuantity;  // index to next slice-neighbor
        diff = positions[next] - positions[i];
        ratio = LengthS(slice, row, col)/diff.Length();
        force = ConstantS(slice, row, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (row > 0)
    {
        prev = i - mNumCols;  // index to previous row-neighbor
        diff = positions[prev] - positions[i];
        ratio = LengthR(slice, row - 1, col)/diff.Length();
        force = ConstantR(slice, row - 1, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (row < mNumRowsM1)
    {
        next = i + mNumCols;  // index to next row-neighbor
        diff = positions[next] - positions[i];
        ratio = LengthR(slice, row, col)/diff.Length();
        force = ConstantR(slice, row, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (col > 0)
    {
        prev = i - 1;  // index to previous col-neighbor
        diff = positions[prev] - positions[i];
        ratio = LengthC(slice, row, col - 1)/diff.Length();
        force = ConstantC(slice, row, col - 1)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    if (col < mNumColsM1)
    {
        next = i + 1;  // index to next col-neighbor
        diff = positions[next] - positions[i];
        ratio = LengthC(slice, row, col)/diff.Length();
        force = ConstantC(slice, row, col)*((Real)1 - ratio)*diff;
        acceleration += mInvMasses[i]*force;
    }

    return acceleration;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
TVector MassSpringVolume<Real,TVector>::ExternalAcceleration (int, Real,
    const TVector*, const TVector*)
{
    return TVector::ZERO;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int MassSpringVolume<Real,TVector>::GetIndex (int slice, int row, int col)
    const
{
    return col + mNumCols*(row + mNumRows*slice);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void MassSpringVolume<Real,TVector>::GetCoordinates (int i, int& slice,
    int& row, int& col) const
{
    col = i % mNumCols;
    i = (i - col)/mNumCols;
    row = i % mNumRows;
    slice = i / mNumRows;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class MassSpringVolume<float,Vector3f>;

template WM5_PHYSICS_ITEM
class MassSpringVolume<double,Vector3d>;
//----------------------------------------------------------------------------
}
