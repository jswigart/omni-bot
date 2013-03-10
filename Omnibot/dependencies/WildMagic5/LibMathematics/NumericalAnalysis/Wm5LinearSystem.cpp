// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
LinearSystem<Real>::LinearSystem ()
{
    ZeroTolerance = Math<Real>::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------
template <typename Real>
LinearSystem<Real>::~LinearSystem()
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::Solve2 (const Real A[2][2], const Real B[2],
    Real X[2])
{
    Real det = A[0][0]*A[1][1] - A[0][1]*A[1][0];
    if (Math<Real>::FAbs(det) < ZeroTolerance)
    {
        return false;
    }

    Real invDet = ((Real)1)/det;
    X[0] = (A[1][1]*B[0] - A[0][1]*B[1])*invDet;
    X[1] = (A[0][0]*B[1] - A[1][0]*B[0])*invDet;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::Solve3 (const Real A[3][3], const Real B[3],
    Real X[3])
{
    Real invA[3][3];
    invA[0][0] = A[1][1]*A[2][2]-A[1][2]*A[2][1];
    invA[0][1] = A[0][2]*A[2][1]-A[0][1]*A[2][2];
    invA[0][2] = A[0][1]*A[1][2]-A[0][2]*A[1][1];
    invA[1][0] = A[1][2]*A[2][0]-A[1][0]*A[2][2];
    invA[1][1] = A[0][0]*A[2][2]-A[0][2]*A[2][0];
    invA[1][2] = A[0][2]*A[1][0]-A[0][0]*A[1][2];
    invA[2][0] = A[1][0]*A[2][1]-A[1][1]*A[2][0];
    invA[2][1] = A[0][1]*A[2][0]-A[0][0]*A[2][1];
    invA[2][2] = A[0][0]*A[1][1]-A[0][1]*A[1][0];
    Real det = A[0][0]*invA[0][0] + A[0][1]*invA[1][0] + A[0][2]*invA[2][0];

    if (Math<Real>::FAbs(det) < ZeroTolerance)
    {
        return false;
    }

    Real invDet = ((Real)1)/det;
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            invA[row][col] *= invDet;
        }
    }

    X[0] = invA[0][0]*B[0] + invA[0][1]*B[1] + invA[0][2]*B[2];
    X[1] = invA[1][0]*B[0] + invA[1][1]*B[1] + invA[1][2]*B[2];
    X[2] = invA[2][0]*B[0] + invA[2][1]*B[1] + invA[2][2]*B[2];
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::Inverse (const GMatrix<Real>& A,
    GMatrix<Real>& invA)
{
    // Computations are performed in-place.
    assertion(A.GetNumRows() == A.GetNumColumns(), "Matrix must be square\n");

    int size = invA.GetNumRows();
    invA = A;

    int* colIndex = new1<int>(size);
    int* rowIndex = new1<int>(size);
    bool* pivoted = new1<bool>(size);
    memset(pivoted, 0, size*sizeof(bool));

    int i1, i2, row = 0, col = 0;
    Real save;

    // Elimination by full pivoting.
    for (int i0 = 0; i0 < size; ++i0)
    {
        // Search matrix (excluding pivoted rows) for maximum absolute entry.
        Real maxValue = (Real)0;
        for (i1 = 0; i1 < size; ++i1)
        {
            if (!pivoted[i1])
            {
                for (i2 = 0; i2 < size; ++i2)
                {
                    if (!pivoted[i2])
                    {
                        Real absValue = Math<Real>::FAbs(invA[i1][i2]);
                        if (absValue > maxValue)
                        {
                            maxValue = absValue;
                            row = i1;
                            col = i2;
                        }
                    }
                }
            }
        }

        if (maxValue == (Real)0)
        {
            // The matrix is not invertible.
            delete1(colIndex);
            delete1(rowIndex);
            delete1(pivoted);
            return false;
        }

        pivoted[col] = true;

        // Swap rows so that A[col][col] contains the pivot entry.
        if (row != col)
        {
            invA.SwapRows(row,col);
        }

        // Keep track of the permutations of the rows.
        rowIndex[i0] = row;
        colIndex[i0] = col;

        // Scale the row so that the pivot entry is 1.
        Real inv = ((Real)1)/invA[col][col];
        invA[col][col] = (Real)1;
        for (i2 = 0; i2 < size; i2++)
        {
            invA[col][i2] *= inv;
        }

        // Zero out the pivot column locations in the other rows.
        for (i1 = 0; i1 < size; ++i1)
        {
            if (i1 != col)
            {
                save = invA[i1][col];
                invA[i1][col] = (Real)0;
                for (i2 = 0; i2 < size; ++i2)
                {
                    invA[i1][i2] -= invA[col][i2]*save;
                }
            }
        }
    }

    // Reorder rows so that A[][] stores the inverse of the original matrix.
    for (i1 = size-1; i1 >= 0; --i1)
    {
        if (rowIndex[i1] != colIndex[i1])
        {
            for (i2 = 0; i2 < size; ++i2)
            {
                save = invA[i2][rowIndex[i1]];
                invA[i2][rowIndex[i1]] = invA[i2][colIndex[i1]];
                invA[i2][colIndex[i1]] = save;
            }
        }
    }

    delete1(colIndex);
    delete1(rowIndex);
    delete1(pivoted);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::Solve (const GMatrix<Real>& A, const Real* B,
    Real* X)
{
    // Computations are performed in-place.
    int size = A.GetNumColumns();
    GMatrix<Real> invA = A;
    memcpy(X, B, size*sizeof(Real));

    int* colIndex = new1<int>(size);
    int* rowIndex = new1<int>(size);
    bool* pivoted = new1<bool>(size);
    memset(pivoted, 0, size*sizeof(bool));

    int i1, i2, row = 0, col = 0;
    Real save;

    // Elimination by full pivoting.
    for (int i0 = 0; i0 < size; i0++)
    {
        // Search matrix (excluding pivoted rows) for maximum absolute entry.
        Real maxValue = (Real)0;
        for (i1 = 0; i1 < size; ++i1)
        {
            if (!pivoted[i1])
            {
                for (i2 = 0; i2 < size; ++i2)
                {
                    if (!pivoted[i2])
                    {
                        Real absValue = Math<Real>::FAbs(invA[i1][i2]);
                        if (absValue > maxValue)
                        {
                            maxValue = absValue;
                            row = i1;
                            col = i2;
                        }
                    }
                }
            }
        }

        if (maxValue == (Real)0)
        {
            // The matrix is not invertible.
            delete1(colIndex);
            delete1(rowIndex);
            delete1(pivoted);
            return false;
        }

        pivoted[col] = true;

        // Swap rows so that A[col][col] contains the pivot entry.
        if (row != col)
        {
            invA.SwapRows(row,col);

            save = X[row];
            X[row] = X[col];
            X[col] = save;
        }

        // Keep track of the permutations of the rows.
        rowIndex[i0] = row;
        colIndex[i0] = col;

        // Scale the row so that the pivot entry is 1.
        Real inv = ((Real)1.0)/invA[col][col];
        invA[col][col] = (Real)1;
        for (i2 = 0; i2 < size; ++i2)
        {
            invA[col][i2] *= inv;
        }
        X[col] *= inv;

        // Zero out the pivot column locations in the other rows.
        for (i1 = 0; i1 < size; ++i1)
        {
            if (i1 != col)
            {
                save = invA[i1][col];
                invA[i1][col] = (Real)0;
                for (i2 = 0; i2 < size; i2++)
                {
                    invA[i1][i2] -= invA[col][i2]*save;
                }
                X[i1] -= X[col]*save;
            }
        }
    }

    // Reorder rows so that A[][] stores the inverse of the original matrix.
    for (i1 = size-1; i1 >= 0; --i1)
    {
        if (rowIndex[i1] != colIndex[i1])
        {
            for (i2 = 0; i2 < size; ++i2)
            {
                save = invA[i2][rowIndex[i1]];
                invA[i2][rowIndex[i1]] = invA[i2][colIndex[i1]];
                invA[i2][colIndex[i1]] = save;
            }
        }
    }

    delete1(colIndex);
    delete1(rowIndex);
    delete1(pivoted);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::SolveTri (int size, Real* A, Real* B, Real* C,
    Real* R, Real* U)
{
    if (B[0] == (Real)0)
    {
        return false;
    }

    Real* D = new1<Real>(size - 1);
    Real E = B[0];
    Real invE = ((Real)1)/E;
    U[0] = R[0]*invE;

    int i0, i1;
    for (i0 = 0, i1 = 1; i1 < size; ++i0, ++i1)
    {
        D[i0] = C[i0]*invE;
        E = B[i1] - A[i0]*D[i0];
        if (E == (Real)0)
        {
            delete1(D);
            return false;
        }
        invE = ((Real)1)/E;
        U[i1] = (R[i1] - A[i0]*U[i0])*invE;
    }

    for (i0 = size-1, i1 = size-2; i1 >= 0; --i0, --i1)
    {
        U[i1] -= D[i1]*U[i0];
    }

    delete1(D);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::SolveConstTri (int size, Real A, Real B, Real C,
    Real* R, Real* U)
{
    if (B == (Real)0)
    {
        return false;
    }

    Real* D = new1<Real>(size - 1);
    Real E = B;
    Real invE = ((Real)1)/E;
    U[0] = R[0]*invE;

    int i0, i1;
    for (i0 = 0, i1 = 1; i1 < size; ++i0, ++i1)
    {
        D[i0] = C*invE;
        E = B - A*D[i0];
        if (E == (Real)0)
        {
            delete1(D);
            return false;
        }
        invE = ((Real)1)/E;
        U[i1] = (R[i1] - A*U[i0])*invE;
    }
    for (i0 = size-1, i1 = size-2; i1 >= 0; --i0, --i1)
    {
        U[i1] -= D[i1]*U[i0];
    }

    delete1(D);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Conjugate gradient methods.
//----------------------------------------------------------------------------
template <typename Real>
Real LinearSystem<Real>::Dot (int size, const Real* U, const Real* V)
{
    Real dot = (Real)0;
    for (int i = 0; i < size; ++i)
    {
        dot += U[i]*V[i];
    }
    return dot;
}
//----------------------------------------------------------------------------
template <typename Real>
void LinearSystem<Real>::Multiply (const GMatrix<Real>& A, const Real* X,
    Real* Prod)
{
    int size = A.GetNumRows();
    memset(Prod, 0, size*sizeof(Real));
    for (int row = 0; row < size; ++row)
    {
        for (int col = 0; col < size; ++col)
        {
            Prod[row] += A[row][col]*X[col];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void LinearSystem<Real>::Multiply (int size, const SparseMatrix& A,
    const Real* X, Real* Prod)
{
    memset(Prod, 0, size*sizeof(Real));
    typename SparseMatrix::const_iterator iter = A.begin();
    typename SparseMatrix::const_iterator end = A.end();
    for (/**/; iter != end; ++iter)
    {
        int i = iter->first.first;
        int j = iter->first.second;
        Real value = iter->second;
        Prod[i] += value*X[j];
        if (i != j)
        {
            Prod[j] += value*X[i];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void LinearSystem<Real>::UpdateX (int size, Real* X, Real alpha,
    const Real* P)
{
    for (int i = 0; i < size; ++i)
    {
        X[i] += alpha*P[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void LinearSystem<Real>::UpdateR (int size, Real* R, Real alpha,
    const Real* W)
{
    for (int i = 0; i < size; ++i)
    {
        R[i] -= alpha*W[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void LinearSystem<Real>::UpdateP (int size, Real* P, Real beta,
    const Real* R)
{
    for (int i = 0; i < size; ++i)
    {
        P[i] = R[i] + beta*P[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::SolveSymmetricCG (const GMatrix<Real>& A,
    const Real* B, Real* X)
{
    // Based on the algorithm in "Matrix Computations" by Golum and Van Loan.
    assertion(A.GetNumRows() == A.GetNumColumns(), "Matrix must be square\n");
    int size = A.GetNumRows();
    Real* R = new1<Real>(size);
    Real* P = new1<Real>(size);
    Real* W = new1<Real>(size);

    // The first iteration.
    size_t numBytes = size*sizeof(Real);
    memset(X, 0, numBytes);
    memcpy(R, B, numBytes);
    Real rho0 = Dot(size, R, R);
    memcpy(P, R, numBytes);
    Multiply(A, P, W);
    Real alpha = rho0/Dot(size, P, W);
    UpdateX(size, X, alpha, P);
    UpdateR(size, R, alpha, W);
    Real rho1 = Dot(size, R, R);

    // The remaining iterations.
    const int imax = 1024;
    int i;
    for (i = 1; i < imax; ++i)
    {
        Real root0 = Math<Real>::Sqrt(rho1);
        Real norm = Dot(size, B, B);
        Real root1 = Math<Real>::Sqrt(norm);
        if (root0 <= ZeroTolerance*root1)
        {
            break;
        }

        Real beta = rho1/rho0;
        UpdateP(size, P, beta, R);
        Multiply(A, P, W);
        alpha = rho1/Dot(size, P, W);
        UpdateX(size, X, alpha, P);
        UpdateR(size, R, alpha, W);
        rho0 = rho1;
        rho1 = Dot(size, R, R);
    }

    delete1(W);
    delete1(P);
    delete1(R);

    return i < imax;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::SolveSymmetricCG (int size,
    const SparseMatrix& A, const Real* B, Real* X)
{
    // Based on the algorithm in "Matrix Computations" by Golum and Van Loan.
    Real* R = new1<Real>(size);
    Real* P = new1<Real>(size);
    Real* W = new1<Real>(size);

    // The first iteration.
    size_t numBytes = size*sizeof(Real);
    memset(X, 0, numBytes);
    memcpy(R, B, numBytes);
    Real rho0 = Dot(size, R, R);
    memcpy(P, R, numBytes);
    Multiply(size, A, P, W);
    Real alpha = rho0/Dot(size, P, W);
    UpdateX(size, X, alpha, P);
    UpdateR(size, R, alpha, W);
    Real rho1 = Dot(size, R, R);

    // The remaining iterations.
    const int imax = 1024;
    int i;
    for (i = 1; i < imax; ++i)
    {
        Real root0 = Math<Real>::Sqrt(rho1);
        Real norm = Dot(size, B, B);
        Real root1 = Math<Real>::Sqrt(norm);
        if (root0 <= ZeroTolerance*root1)
        {
            break;
        }

        Real beta = rho1/rho0;
        UpdateP(size, P, beta, R);
        Multiply(size, A, P, W);
        alpha = rho1/Dot(size, P, W);
        UpdateX(size, X, alpha, P);
        UpdateR(size, R, alpha, W);
        rho0 = rho1;
        rho1 = Dot(size, R, R);
    }

    delete1(W);
    delete1(P);
    delete1(R);

    return i < imax;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Banded matrices.
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::ForwardEliminate (int reduceRow,
    BandedMatrix<Real>& A, Real* B)
{
    // The pivot must be nonzero in order to proceed.
    Real diag = A(reduceRow,reduceRow);
    if (diag == (Real)0)
    {
        return false;
    }

    Real invDiag = ((Real)1)/diag;
    A(reduceRow,reduceRow) = (Real)1;

    // Multiply the row to be consistent with diagonal term of 1.
    int colMin = reduceRow + 1;
    int colMax = colMin + A.GetNumUBands();
    if (colMax > A.GetSize())
    {
        colMax = A.GetSize();
    }

    int col;
    for (col = colMin; col < colMax; ++col)
    {
        A(reduceRow, col) *= invDiag;
    }

    B[reduceRow] *= invDiag;

    // Reduce the remaining rows.
    int rowMin = reduceRow + 1;
    int rowMax = rowMin + A.GetNumLBands();
    if (rowMax > A.GetSize())
    {
        rowMax = A.GetSize();
    }

    for (int row = rowMin; row < rowMax; ++row)
    {
        Real mult = A(row,reduceRow);
        A(row, reduceRow) = (Real)0;
        for (col = colMin; col < colMax; ++col)
        {
            A(row,col) -= mult*A(reduceRow,col);
        }
        B[row] -= mult*B[reduceRow];
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::SolveBanded (const BandedMatrix<Real>& A,
    const Real* B, Real* X)
{
    BandedMatrix<Real> tmp = A;
    int size = A.GetSize();
    size_t numBytes = size*sizeof(Real); 
    memcpy(X, B, numBytes);

    // Forward elimination.
    int row;
    for (row = 0; row < size; ++row)
    {
        if (!ForwardEliminate(row, tmp, X))
        {
            return false;
        }
    }

    // Backward substitution.
    for (row = size - 2; row >= 0; --row)
    {
        int colMin = row + 1;
        int colMax = colMin + tmp.GetNumUBands();
        if (colMax > size)
        {
            colMax = size;
        }
        for (int col = colMin; col < colMax; ++col)
        {
            X[row] -= tmp(row,col)*X[col];
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::ForwardEliminate (int reduceRow,
    BandedMatrix<Real>& A, GMatrix<Real>& B)
{
    // The pivot must be nonzero in order to proceed.
    Real diag = A(reduceRow,reduceRow);
    if (diag == (Real)0)
    {
        return false;
    }

    Real invDiag = ((Real)1)/diag;
    A(reduceRow,reduceRow) = (Real)1;

    // Multiply the row to be consistent with diagonal term of 1.
    int colMin = reduceRow + 1;
    int colMax = colMin + A.GetNumUBands();
    if (colMax > A.GetSize())
    {
        colMax = A.GetSize();
    }

    int col;
    for (col = colMin; col < colMax; ++col)
    {
        A(reduceRow,col) *= invDiag;
    }
    for (col = 0; col <= reduceRow; ++col)
    {
        B(reduceRow,col) *= invDiag;
    }

    // Reduce the remaining rows.
    int rowMin = reduceRow + 1;
    int rowMax = rowMin + A.GetNumLBands();
    if (rowMax > A.GetSize())
    {
        rowMax = A.GetSize();
    }

    for (int row = rowMin; row < rowMax; ++row)
    {
        Real mult = A(row,reduceRow);
        A(row,reduceRow) = (Real)0;
        for (col = colMin; col < colMax; ++col)
        {
            A(row,col) -= mult*A(reduceRow,col);
        }
        for (col = 0; col <= reduceRow; ++col)
        {
            B(row,col) -= mult*B(reduceRow,col);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void LinearSystem<Real>::BackwardEliminate (int reduceRow,
    BandedMatrix<Real>& A, GMatrix<Real>& B)
{
    int rowMax = reduceRow - 1;
    int rowMin = reduceRow - A.GetNumUBands();
    if (rowMin < 0)
    {
        rowMin = 0;
    }

    for (int row = rowMax; row >= rowMin; --row)
    {
        Real mult = A(row,reduceRow);
        A(row,reduceRow) = (Real)0;
        for (int col = 0; col < B.GetNumColumns(); ++col)
        {
            B(row,col) -= mult*B(reduceRow,col);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool LinearSystem<Real>::Invert (const BandedMatrix<Real>& A,
    GMatrix<Real>& invA)
{
    int size = A.GetSize();
    BandedMatrix<Real> tmp = A;
    int row;
    for (row = 0; row < size; ++row)
    {
        for (int col = 0; col < size; ++col)
        {
            if (row != col)
            {
                invA(row,col) = (Real)0;
            }
            else
            {
                invA(row,row) = (Real)1;
            }
        }
    }

    // Forward elimination.
    for (row = 0; row < size; ++row)
    {
        if (!ForwardEliminate(row, tmp, invA))
        {
            return false;
        }
    }

    // Backward elimination.
    for (row = size - 1; row >= 1; --row)
    {
        BackwardEliminate(row, tmp, invA);
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class LinearSystem<float>;

template WM5_MATHEMATICS_ITEM
class LinearSystem<double>;
//----------------------------------------------------------------------------
}
