// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
BandedMatrix<Real>::BandedMatrix (int size, int lBands, int uBands)
{
    assertion(size > 0 && lBands >= 0 && uBands >= 0 && lBands < size
        && uBands < size, "Invalid argument to BandedMatrix constructor\n");

    mSize = size;
    mNumLBands = lBands;
    mNumUBands = uBands;
    Allocate();
}
//----------------------------------------------------------------------------
template <typename Real>
BandedMatrix<Real>::BandedMatrix (const BandedMatrix& mat)
{
    mDBand = 0;
    mLBand = 0;
    mUBand = 0;
    *this = mat;
}
//----------------------------------------------------------------------------
template <typename Real>
BandedMatrix<Real>::~BandedMatrix ()
{
    Deallocate();
}
//----------------------------------------------------------------------------
template <typename Real>
BandedMatrix<Real>& BandedMatrix<Real>::operator= (const BandedMatrix& mat)
{
    Deallocate();
    mSize = mat.mSize;
    mNumLBands = mat.mNumLBands;
    mNumUBands = mat.mNumUBands;
    Allocate();

    size_t numBytes = mSize*sizeof(Real);
    memcpy(mDBand, mat.mDBand, numBytes);

    int i;
    for (i = 0; i < mNumLBands; ++i)
    {
        numBytes = (mSize - 1 - i)*sizeof(Real);
        memcpy(mLBand[i], mat.mLBand[i], numBytes);
    }

    for (i = 0; i < mNumUBands; ++i)
    {
        numBytes = (mSize - 1 - i)*sizeof(Real);
        memcpy(mUBand[i], mat.mUBand[i], numBytes);
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
int BandedMatrix<Real>::GetSize () const
{
    return mSize;
}
//----------------------------------------------------------------------------
template <typename Real>
int BandedMatrix<Real>::GetNumLBands () const
{
    return mNumLBands;
}
//----------------------------------------------------------------------------
template <typename Real>
int BandedMatrix<Real>::GetNumUBands () const
{
    return mNumUBands;
}
//----------------------------------------------------------------------------
template <typename Real>
Real* BandedMatrix<Real>::GetDBand ()
{
    return mDBand;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* BandedMatrix<Real>::GetDBand () const
{
    return mDBand;
}
//----------------------------------------------------------------------------
template <typename Real>
int BandedMatrix<Real>::GetLBandMax (int i) const
{
    assertion(0 <= i && i < mNumLBands, "Invalid index in GetLBandMax\n");
    return mSize - 1 - i;
}
//----------------------------------------------------------------------------
template <typename Real>
Real* BandedMatrix<Real>::GetLBand (int i)
{
    if (mLBand)
    {
        assertion(0 <= i && i < mNumLBands, "Invalid index in GetLBand\n");
        return mLBand[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* BandedMatrix<Real>::GetLBand (int i) const
{
    if (mLBand)
    {
        assertion(0 <= i && i < mNumLBands, "Invalid index in GetLBand\n");
        return mLBand[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int BandedMatrix<Real>::GetUBandMax (int i) const
{
    assertion(0 <= i && i < mNumUBands, "Invalid index in GetUBandMax\n");
    return mSize - 1 - i;
}
//----------------------------------------------------------------------------
template <typename Real>
Real* BandedMatrix<Real>::GetUBand (int i)
{
    if (mUBand)
    {
        assertion(0 <= i && i < mNumUBands, "Invalid index in GetUBand\n");
        return mUBand[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* BandedMatrix<Real>::GetUBand (int i) const
{
    if (mUBand)
    {
        assertion(0 <= i && i < mNumUBands, "Invalid index in GetUBand\n");
        return mUBand[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
Real& BandedMatrix<Real>::operator() (int row, int column)
{
    assertion(0 <= row && row < mSize && 0 <= column && column < mSize,
        "Invalid row or column in BandedMatrix::operator\n");

    int band = column - row;
    if (band > 0)
    {
        if (--band < mNumUBands && row < mSize - 1 - band)
        {
            return mUBand[band][row];
        }
    }
    else if (band < 0)
    {
        band = -band;
        if (--band < mNumLBands && column < mSize - 1 - band)
        {
            return mLBand[band][column];
        }
    }
    else
    {
        return mDBand[row];
    }

    static Real sDummy = (Real)0;
    return sDummy;
}
//----------------------------------------------------------------------------
template <typename Real>
Real BandedMatrix<Real>::operator() (int row, int column) const
{
    assertion(0 <= row && row < mSize && 0 <= column && column < mSize,
        "Invalid row or column in operator()\n");

    int band = column - row;
    if (band > 0)
    {
        if (--band < mNumUBands && row < mSize - 1 - band)
        {
            return mUBand[band][row];
        }
    }
    else if (band < 0)
    {
        band = -band;
        if (--band < mNumLBands && column < mSize - 1 - band)
        {
            return mLBand[band][column];
        }
    }
    else
    {
        return mDBand[row];
    }

    return (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
void BandedMatrix<Real>::SetZero ()
{
    assertion(mSize > 0, "Matrix size must be positive in SetZero\n");

    size_t numBytes = mSize*sizeof(Real);
    memset(mDBand, 0, numBytes);

    int i;
    for (i = 0; i < mNumLBands; ++i)
    {
        numBytes = (mSize - 1 - i)*sizeof(Real);
        memset(mLBand[i], 0, numBytes);
    }

    for (i = 0; i < mNumUBands; ++i)
    {
        numBytes = (mSize - 1 - i)*sizeof(Real);
        memset(mUBand[i], 0, numBytes);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void BandedMatrix<Real>::SetIdentity ()
{
    assertion(mSize > 0, "Matrix size must be positive in SetIdentity\n");

    int i;
    for (i = 0; i < mSize; ++i)
    {
        mDBand[i] = (Real)1;
    }

    size_t numBytes;
    for (i = 0; i < mNumLBands; ++i)
    {
        numBytes = (mSize - 1 - i)*sizeof(Real);
        memset(mLBand[i], 0, numBytes);
    }

    for (i = 0; i < mNumUBands; ++i)
    {
        numBytes = (mSize - 1 - i)*sizeof(Real);
        memset(mUBand[i], 0, numBytes);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool BandedMatrix<Real>::CholeskyFactor ()
{
    assertion(mNumLBands == mNumUBands,
        "Bands must be equal in CholeskyFactor\n");

    if (mNumLBands != mNumUBands)
    {
        return false;
    }

    int sizeM1 = mSize - 1;

    int k, kMax;
    for (int i = 0; i < mSize; ++i)
    {
        int jMin = i - mNumLBands;
        if (jMin < 0)
        {
            jMin = 0;
        }

        int j;
        for (j = jMin; j < i; ++j)
        {
            kMax = j + mNumLBands;
            if (kMax > sizeM1)
            {
                kMax = sizeM1;
            }

            for (k = i; k <= kMax; ++k)
            {
                (*this)(k,i) -= (*this)(i,j)*(*this)(k,j);
            }
        }

        kMax = j + mNumLBands;
        if (kMax > sizeM1)
        {
            kMax = sizeM1;
        }

        for (k = 0; k < i; ++k)
        {
            (*this)(k,i) = (*this)(i,k);
        }

        Real diagonal = (*this)(i,i);
        if (diagonal <= (Real)0)
        {
            return false;
        }
        Real invSqrt = Math<Real>::InvSqrt(diagonal);
        for (k = i; k <= kMax; ++k)
        {
            (*this)(k,i) *= invSqrt;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool BandedMatrix<Real>::SolveSystem (Real* bVector)
{
    return CholeskyFactor()
        && SolveLower(bVector)
        && SolveUpper(bVector);
}
//----------------------------------------------------------------------------
template <typename Real>
bool BandedMatrix<Real>::SolveSystem (Real** bMatrix, int numBColumns)
{
    return CholeskyFactor()
        && SolveLower(bMatrix, numBColumns)
        && SolveUpper(bMatrix, numBColumns);
}
//----------------------------------------------------------------------------
template <typename Real>
bool BandedMatrix<Real>::SolveLower (Real* dataVector) const
{
    for (int row = 0; row < mSize; ++row)
    {
        Real lowerRR = (*this)(row,row);
        if (Math<Real>::FAbs(lowerRR) < Math<Real>::ZERO_TOLERANCE)
        {
            return false;
        }

        for (int column = 0; column < row; ++column)
        {
            Real lowerRC = (*this)(row,column);
            dataVector[row] -= lowerRC*dataVector[column];
        }

        dataVector[row] /= lowerRR;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool BandedMatrix<Real>::SolveUpper (Real* dataVector) const
{
    for (int row = mSize - 1; row >= 0; --row)
    {
        Real upperRR = (*this)(row,row);
        if (Math<Real>::FAbs(upperRR) < Math<Real>::ZERO_TOLERANCE)
        {
            return false;
        }

        for (int column = row + 1; column < mSize; ++column)
        {
            Real upperRC = (*this)(row,column);
            dataVector[row] -= upperRC*dataVector[column];
        }

        dataVector[row] /= upperRR;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool BandedMatrix<Real>::SolveLower (Real** dataMatrix, int numColumns)
    const
{
    for (int row = 0; row < mSize; ++row)
    {
        Real lowerRR = (*this)(row,row);
        if (Math<Real>::FAbs(lowerRR) < Math<Real>::ZERO_TOLERANCE)
        {
            return false;
        }

        int bCol;
        for (int column = 0; column < row; ++column)
        {
            Real lowerRC = (*this)(row,column);
            for (bCol = 0; bCol < numColumns; ++bCol)
            {
                dataMatrix[row][bCol] -= lowerRC*dataMatrix[column][bCol];
            }
        }

        Real inverse = ((Real)1)/lowerRR;
        for (bCol = 0; bCol < numColumns; ++bCol)
        {
            dataMatrix[row][bCol] *= inverse;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool BandedMatrix<Real>::SolveUpper (Real** dataMatrix, int numColumns)
    const
{
    for (int row = mSize - 1; row >= 0; --row)
    {
        Real upperRR = (*this)(row,row);
        if (Math<Real>::FAbs(upperRR) < Math<Real>::ZERO_TOLERANCE)
        {
            return false;
        }

        int bCol;
        for (int column = row + 1; column < mSize; ++column)
        {
            Real upperRC = (*this)(row,column);
            for (bCol = 0; bCol < numColumns; ++bCol)
            {
                dataMatrix[row][bCol] -= upperRC*dataMatrix[column][bCol];
            }
        }

        Real inverse = ((Real)1)/upperRR;
        for (bCol = 0; bCol < numColumns; ++bCol)
        {
            dataMatrix[row][bCol] *= inverse;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void BandedMatrix<Real>::Allocate ()
{
    // assert:  mSize, mNumLBands, mNumUBands already set
    // assert:  mDBand, mLBand, mUBand all null

    mDBand = new1<Real>(mSize);
    size_t numBytes = mSize*sizeof(Real);
    memset(mDBand, 0, numBytes);

    if (mNumLBands > 0)
    {
        mLBand = new1<Real*>(mNumLBands);
    }
    else
    {
        mLBand = 0;
    }

    if (mNumUBands > 0)
    {
        mUBand = new1<Real*>(mNumUBands);
    }
    else
    {
        mUBand = 0;
    }

    int i;
    for (i = 0; i < mNumLBands; ++i)
    {
        int lowerSize = mSize - 1 - i;
        mLBand[i] = new1<Real>(lowerSize);
        memset(mLBand[i], 0, lowerSize*sizeof(Real));
    }

    for (i = 0; i < mNumUBands; ++i)
    {
        int upperSize = mSize - 1 - i;
        mUBand[i] = new1<Real>(upperSize);
        memset(mUBand[i], 0, upperSize*sizeof(Real));
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void BandedMatrix<Real>::Deallocate ()
{
    delete1(mDBand);

    int i;

    if (mLBand)
    {
        for (i = 0; i < mNumLBands; ++i)
        {
            delete1(mLBand[i]);
        }

        delete1(mLBand);
        mLBand = 0;
    }

    if (mUBand)
    {
        for (i = 0; i < mNumUBands; ++i)
        {
            delete1(mUBand[i]);
        }

        delete1(mUBand);
        mUBand = 0;
    }
}
//----------------------------------------------------------------------------
