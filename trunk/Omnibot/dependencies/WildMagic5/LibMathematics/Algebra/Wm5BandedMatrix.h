// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5BANDEDMATRIX_H
#define WM5BANDEDMATRIX_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"

namespace Wm5
{

template <typename Real>
class BandedMatrix
{
public:
    // Construction and destruction.
    BandedMatrix (int size, int numLBands, int numUBands);
    BandedMatrix (const BandedMatrix& mat);
    ~BandedMatrix ();

    // Assignment.
    BandedMatrix& operator= (const BandedMatrix& mat);

    // Member access.
    int GetSize () const;
    int GetNumLBands () const;
    int GetNumUBands () const;

    Real* GetDBand ();
    const Real* GetDBand () const;

    int GetLBandMax (int i) const;  // LBand(i):  0 <= index < LBandMax
    Real* GetLBand (int i);
    const Real* GetLBand (int i) const;

    int GetUBandMax (int i) const;  // UBand(i):  0 <= index < UBandMax
    Real* GetUBand (int i);
    const Real* GetUBand (int i) const;

    Real& operator() (int row, int column);
    Real operator() (int row, int column) const;

    // Convenience functions for initializing matrices.
    void SetZero ();
    void SetIdentity ();

    // Factor the square banded matrix A into A = L*L^T, where L is a
    // lower-triangular matrix (L^T is an upper-triangular matrix).
    // This is an LU decomposition that allows for stable inversion
    // of A to solve A*X = B.  The return value is 'true' iff the
    // factorizing is successful (L is invertible).  If successful, A
    // contains the Cholesky factorization (L in the lower-triangular part
    // of A and/ L^T in the upper-triangular part of A).
    bool CholeskyFactor ();

    // Solve the linear system A*X = B, where A is an NxN banded matrix and
    // B is an Nx1 vector.  The unknown X is also Nx1.  The input to this
    // function is B.  The output X is computed and stored in B.  The return
    // value is 'true' iff the system has a solution.  The matrix A and the
    // vector B are both modified by this function.  If successful, A contains
    // the Cholesky factorization (L in the lower-triangular part of A and
    // L^T in the upper-triangular part of A).
    bool SolveSystem (Real* bVector);

    // Solve the linear system A*X = B, where A is an NxN banded matrix and
    // B is an NxM matrix.  The unknown X is also NxM.  The input to this
    // function is B.  The output X is computed and stored in B.  The return
    // value is 'true' iff the system has a solution.  The matrix A and the
    // vector B are both modified by this function.  If successful, A contains
    // the Cholesky factorization (L in the lower-triangular part of A and
    // L^T in the upper-triangular part of A).
    bool SolveSystem (Real** bMatrix, int numBColumns);

private:
    // The linear system is L*U*X = B, where A = L*U and U = L^T,  Reduce this
    // to U*X = L^{-1}*B.  The return value is 'true' iff the operation is
    // successful.
    bool SolveLower (Real* dataVector) const;

    // The linear system is U*X = L^{-1}*B.  Reduce this to
    // X = U^{-1}*L^{-1}*B.  The return value is 'true' iff the operation is
    // successful.
    bool SolveUpper (Real* dataVector) const;

    // The linear system is L*U*X = B, where A = L*U and U = L^T,  Reduce this
    // to U*X = L^{-1}*B.  The return value is 'true' iff the operation is
    // successful.
    bool SolveLower (Real** dataMatrix, int numColumns) const;

    // The linear system is U*X = L^{-1}*B.  Reduce this to
    // X = U^{-1}*L^{-1}*B.  The return value is 'true' iff the operation is
    // successful.
    bool SolveUpper (Real** dataMatrix, int numColumns) const;

    void Allocate ();
    void Deallocate ();

    int mSize, mNumLBands, mNumUBands;
    Real* mDBand;
    Real** mLBand;
    Real** mUBand;
};

#include "Wm5BandedMatrix.inl"

typedef BandedMatrix<float> BandedMatrixf;
typedef BandedMatrix<double> BandedMatrixd;

}

#endif
