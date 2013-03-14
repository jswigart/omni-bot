// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5LINEARSYSTEM_H
#define WM5LINEARSYSTEM_H

#include "Wm5MathematicsLIB.h"
#include "Wm5BandedMatrix.h"
#include "Wm5GMatrix.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM LinearSystem
{
public:
    // Construction and destruction.
    LinearSystem ();
    ~LinearSystem ();

    // 2x2 and 3x3 systems (avoids overhead of Gaussian elimination)
    bool Solve2 (const Real A[2][2], const Real B[2], Real X[2]);
    bool Solve3 (const Real A[3][3], const Real B[3], Real X[3]);

    // Input:
    //     A[iSize][iSize], entries are A[row][col]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     InvA[iSize][iSize], inverse matrix
    bool Inverse (const GMatrix<Real>& A, GMatrix<Real>& invA);

    // Input:
    //     A[iSize][iSize] coefficient matrix, entries are A[row][col]
    //     B[iSize] vector, entries are B[row]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     X[iSize] is solution X to AX = B
    bool Solve (const GMatrix<Real>& A, const Real* B, Real* X);

    // Input:
    //     Matrix is tridiagonal.
    //     Lower diagonal A[iSize-1]
    //     Main  diagonal B[iSize]
    //     Upper diagonal C[iSize-1]
    //     Right-hand side R[iSize]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     U[iSize] is solution
    bool SolveTri (int size, Real* A, Real* B, Real* C, Real* R, Real* U);

    // Input:
    //     Matrix is tridiagonal.
    //     Lower diagonal is constant, A
    //     Main  diagonal is constant, B
    //     Upper diagonal is constant, C
    //     Right-hand side Rr[iSize]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     U[iSize] is solution
    bool SolveConstTri (int size, Real A, Real B, Real C, Real* R,
        Real* U);

    // Solution using the conjugate gradient method.
    // Input:
    //    A[iSize][iSize] symmetrix matrix, entries are A[row][col]
    //    B[iSize] vector, entries are B[row]
    // Output:
    //    X[iSize] is the solution x to Ax = B
    bool SolveSymmetricCG (const GMatrix<Real>& A, const Real* B, Real* X);

    // Conjugate gradient method for sparse, symmetric matrices.
    // Input:
    //    The nonzero entries of the symmetrix matrix A are stored in a map
    //    whose keys are pairs (i,j) and whose values are real numbers.  The
    //    pair (i,j) is the location of the value in the array.  Only one of
    //    (i,j) and (j,i) should be stored since A is symmetric.  The code
    //    assumes this is how you set up A.  The column vector B is stored as
    //    an array of contiguous values.
    // Output:
    //    X[iSize] is the solution x to Ax = B
    typedef std::map<std::pair<int,int>,Real> SparseMatrix;
    bool SolveSymmetricCG (int size, const SparseMatrix& A, const Real* B,
        Real* X);

    // Solve banded matrix systems.
    // Input:
    //     A, a banded matrix
    //     B[iSize] vector, entries are B[row]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     X[iSize] is solution X to AX = B
    bool SolveBanded (const BandedMatrix<Real>& A, const Real* B, Real* X);

    // Invert a banded matrix.
    // Input:
    //     A, a banded matrix
    // Output:
    //     return value is TRUE if the inverse exists, FALSE otherwise
    //     InvA, the inverse of A
    bool Invert (const BandedMatrix<Real>& A, GMatrix<Real>& invA);

    // Tolerance for linear system solving.
    Real ZeroTolerance;  // default = Math<Real>::ZERO_TOLERANCE

private:
    // Support for the conjugate gradient method for standard arrays.
    Real Dot (int size, const Real* U, const Real* V);
    void Multiply (const GMatrix<Real>& A, const Real* X, Real* Prod);
    void UpdateX (int size, Real* X, Real alpha, const Real* P);
    void UpdateR (int size, Real* R, Real alpha, const Real* W);
    void UpdateP (int size, Real* P, Real beta, const Real* R);

    // Support for the conjugate gradient method for sparse arrays.
    void Multiply (int size, const SparseMatrix& A, const Real* X,
        Real* Prod);

    // Support for banded matrices.
    bool ForwardEliminate (int reduceRow, BandedMatrix<Real>& A, Real* B);
    bool ForwardEliminate (int reduceRow, BandedMatrix<Real>& A,
        GMatrix<Real>& B);
    void BackwardEliminate (int reduceRow, BandedMatrix<Real>& A,
        GMatrix<Real>& B);
};

typedef LinearSystem<float> LinearSystemf;
typedef LinearSystem<double> LinearSystemd;

}

#endif
