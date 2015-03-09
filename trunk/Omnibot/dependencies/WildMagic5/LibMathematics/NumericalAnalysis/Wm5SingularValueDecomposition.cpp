// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5SingularValueDecomposition.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
SingularValueDecomposition<Real>::SingularValueDecomposition (
    const GMatrix<Real>& M, GMatrix<Real>& L, GMatrix<Real>& D,
    GMatrix<Real>& RTranspose)
{
    // TODO.  Implement other QR factorizations and SVD code from "Matrix
    // Computations", and then give the user the ability to specify which
    // methods are used here.

    int numRows = M.GetNumRows();
    int numCols = M.GetNumColumns();
    L.SetSize(numRows, numRows);
    D.SetSize(numRows, numCols);
    RTranspose.SetSize(numCols, numCols);

    GMatrix<Real> kMTM = M.TransposeTimes(M);
    EigenDecomposition<Real> es(kMTM);
    es.Solve(false);
    GMatrix<Real> V = es.GetEigenvectors();
    GMatrix<Real> MV = M*V;
    HouseholderQR(MV, L, D);
    RTranspose = V.Transpose();
}
//----------------------------------------------------------------------------
template <typename Real>
SingularValueDecomposition<Real>::~SingularValueDecomposition ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
GVector<Real> SingularValueDecomposition<Real>::HouseholderVector (
    const GVector<Real>& X)
{
    GVector<Real> V = X;
    Real length = V.Length();
    Real beta = X[0] + Math<Real>::Sign(X[0])*length;
    if (beta != (Real)0)
    {
        Real invBeta = ((Real)1)/beta;
        for (int i = 1; i < V.GetSize(); ++i)
        {
            V[i] *= invBeta;
        }
    }
    V[0] = (Real)1;

    return V;
}
//----------------------------------------------------------------------------
template <typename Real>
void SingularValueDecomposition<Real>::HouseholderPremultiply (
    const GVector<Real>& V, GMatrix<Real>& A)
{
    GVector<Real> W = (((Real)-2)/V.SquaredLength())*(V*A);
    int numRows = A.GetNumRows();
    int numCols = A.GetNumColumns();
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            A[row][col] += V[row]*W[col];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void SingularValueDecomposition<Real>::HouseholderPostmultiply (
    const GVector<Real>& V, GMatrix<Real>& A)
{
    GVector<Real> W = (((Real)-2)/V.SquaredLength())*(A*V);
    int numRows = A.GetNumRows();
    int numCols = A.GetNumColumns();
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            A[row][col] += W[row]*V[col];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void SingularValueDecomposition<Real>::HouseholderQR (
    const GMatrix<Real>& A, GMatrix<Real>& Q, GMatrix<Real>& R)
{
    // The matrix R gets a copy of A, and is then overwritten during the
    // algorithm with the correct entries to be upper triangular.
    R = A;
    int numRows = R.GetNumRows();
    int numCols = R.GetNumColumns();
    assertion(numRows >= numCols, "Too many columns (use transpose)\n");
    int row, col;
    GVector<Real> V(numRows);
    std::vector<GVector<Real> > VSave;
    for (col = 0; col < numCols; ++col)
    {
        // Create the Householder vector for the partial column of A.
        for (row = 0; row < col; ++row)
        {
            V[row] = (Real)0;
        }
        Real length = (Real)0;
        for (row = col; row < numRows; ++row)
        {
            V[row] = R[row][col];
            length += V[row]*V[row];
        }
        length = Math<Real>::Sqrt(length);
        Real beta = V[col] + Math<Real>::Sign(V[col])*length;
        if (beta != (Real)0)
        {
            Real invBeta = ((Real)1)/beta;
            for (int i = col + 1; i < numRows; ++i)
            {
                V[i] *= invBeta;
            }
        }
        V[col] = (Real)1;

        // Premultiply A by the V-reflection matrix.
        HouseholderPremultiply(V, R);

        // Save the Householder vectors.
        VSave.push_back(V);
    }

    // First, make Q the identity.  Second, extract the Householder vectors
    // and premultiply by the V-reflections to build Q.
    memset(Q.GetElements(), 0, Q.GetNumElements()*sizeof(Real));
    for (row = 0; row < numRows; ++row)
    {
        Q[row][row] = (Real)1;
    }

    for (col = numCols - 1; col >= 0; --col)
    {
        // Get the Householder vector.
        V = VSave[col];

        // Premultiply Q by the V-reflection matrix.
        HouseholderPremultiply(V, Q);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class SingularValueDecomposition<float>;

template WM5_MATHEMATICS_ITEM
class SingularValueDecomposition<double>;
//----------------------------------------------------------------------------
}
