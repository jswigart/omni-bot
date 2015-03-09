// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.5 (2012/03/09)

//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>::GMatrix (int numRows, int numColumns)
{
    mEntry = 0;
    SetSize(numRows, numColumns);
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>::GMatrix (int numRows, int numColumns, const Real* entry)
{
    mEntry = 0;
    SetMatrix(numRows, numColumns, entry);
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>::GMatrix (int numRows, int numColumns, const Real** matrix)
{
    mEntry = 0;
    SetMatrix(numRows, numColumns, matrix);
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>::GMatrix (const GMatrix& mat)
{
    mNumRows = 0;
    mNumColumns = 0;
    mNumElements = 0;
    mEntry = 0;
    *this = mat;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>::~GMatrix ()
{
    delete2(mEntry);
}
//----------------------------------------------------------------------------
template <typename Real>
void GMatrix<Real>::SetSize (int numRows, int numColumns)
{
    delete2(mEntry);
    if (numRows > 0 && numColumns > 0)
    {
        mNumRows = numRows;
        mNumColumns = numColumns;
        mNumElements = mNumRows*mNumColumns;
        mEntry = new2<Real>(mNumColumns, mNumRows);
        memset(mEntry[0], 0, mNumElements*sizeof(Real));
    }
    else
    {
        mNumRows = 0;
        mNumColumns = 0;
        mNumElements = 0;
        mEntry = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
inline void GMatrix<Real>::GetSize (int& numRows, int& numColumns) const
{
    numRows = mNumRows;
    numColumns = mNumColumns;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int GMatrix<Real>::GetNumRows () const
{
    return mNumRows;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int GMatrix<Real>::GetNumColumns () const
{
    return mNumColumns;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int GMatrix<Real>::GetNumElements () const
{
    return mNumElements;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real* GMatrix<Real>::GetElements () const
{
    return mEntry[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real* GMatrix<Real>::GetElements ()
{
    return mEntry[0];
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real* GMatrix<Real>::operator[] (int row) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= row && row < mNumRows, "Invalid index in operator[]\n");
#endif
    return mEntry[row];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real* GMatrix<Real>::operator[] (int row)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= row && row < mNumRows, "Invalid index in operator[]\n");
#endif
    return mEntry[row];
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real& GMatrix<Real>::operator() (int row, int col) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= row && row < mNumRows && 0 <= col && col <= mNumColumns,
        "Invalid index in operator()\n");
#endif
    return mEntry[row][col];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& GMatrix<Real>::operator() (int row, int col)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= row && row < mNumRows
        && 0 <= col && col <= mNumColumns,
        "Invalid index in operator()\n");
#endif
    return mEntry[row][col];
}
//----------------------------------------------------------------------------
template <typename Real>
void GMatrix<Real>::SwapRows (int row0, int row1)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= row0 && row0 < mNumRows
        && 0 <= row1 && row1 < mNumRows,
        "Invalid index in SwapRows\n");
#endif
    if (row0 != row1)
    {
        for (int c = 0; c < mNumColumns; ++c)
        {
            Real save = mEntry[row0][c];
            mEntry[row0][c] = mEntry[row1][c];
            mEntry[row1][c] = save;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void GMatrix<Real>::SwapColumns (int col0, int col1)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= col0 && col0 < mNumColumns
        && 0 <= col1 && col1 < mNumColumns,
        "Invalid index in SwapColumns\n");
#endif
    if (col0 != col1)
    {
        for (int r = 0; r < mNumRows; ++r)
        {
            Real save = mEntry[r][col0];
            mEntry[r][col0] = mEntry[r][col1];
            mEntry[r][col1] = save;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void GMatrix<Real>::SetRow (int row, const GVector<Real>& vec)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= row && row < mNumRows && vec.GetSize() == mNumColumns,
        "Invalid index in SetRow\n");
#endif
    for (int c = 0; c < mNumColumns; ++c)
    {
        mEntry[row][c] = vec[c];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
GVector<Real> GMatrix<Real>::GetRow (int row) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= row && row < mNumRows, "Invalid index in GetRow\n");
#endif
    GVector<Real> vec(mNumColumns);
    for (int c = 0; c < mNumColumns; ++c)
    {
        vec[c] = mEntry[row][c];
    }
    return vec;
}
//----------------------------------------------------------------------------
template <typename Real>
void GMatrix<Real>::SetColumn (int col, const GVector<Real>& vec)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= col && col < mNumColumns && vec.GetSize() == mNumRows,
        "Invalid index in SetColumn\n");
#endif
    for (int r = 0; r < mNumRows; ++r)
    {
        mEntry[r][col] = vec[r];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
GVector<Real> GMatrix<Real>::GetColumn (int col) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(0 <= col && col < mNumColumns,
        "Invalid index in GetColumn\n");
#endif
    GVector<Real> vec(mNumRows);
    for (int r = 0; r < mNumRows; ++r)
    {
        vec[r] = mEntry[r][col];
    }
    return vec;
}
//----------------------------------------------------------------------------
template <typename Real>
void GMatrix<Real>::SetMatrix (int numRows, int numColumns, const Real* entry)
{
    delete2(mEntry);
    if (numRows > 0 && numColumns > 0)
    {
        mNumRows = numRows;
        mNumColumns = numColumns;
        mNumElements = mNumRows*mNumColumns;
        mEntry = new2<Real>(mNumColumns, mNumRows);
        memcpy(mEntry[0], entry, mNumElements*sizeof(Real));
    }
    else
    {
        mNumRows = 0;
        mNumColumns = 0;
        mNumElements = 0;
        mEntry = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void GMatrix<Real>::SetMatrix (int numRows, int numColumns,
    const Real** matrix)
{
    delete2(mEntry);
    if (numRows > 0 && numColumns > 0)
    {
        mNumRows = numRows;
        mNumColumns = numColumns;
        mNumElements = mNumRows*mNumColumns;
        mEntry = new2<Real>(mNumColumns, mNumRows);
        for (int r = 0; r < mNumRows; ++r)
        {
            for (int c = 0; c < mNumColumns; ++c)
            {
                mEntry[r][c] = matrix[r][c];
            }
        }
    }
    else
    {
        mNumRows = 0;
        mNumColumns = 0;
        mNumElements = 0;
        mEntry = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>& GMatrix<Real>::operator= (const GMatrix& mat)
{
    if (mat.mNumElements > 0)
    {
        if (mNumRows != mat.mNumRows || mNumColumns != mat.mNumColumns)
        {
            delete2(mEntry);
            mNumRows = mat.mNumRows;
            mNumColumns = mat.mNumColumns;
            mNumElements = mat.mNumElements;
            mEntry = new2<Real>(mNumColumns, mNumRows);
        }
        memcpy(mEntry[0], mat.mEntry[0], mNumElements*sizeof(Real));
    }
    else
    {
        delete2(mEntry);
        mNumRows = 0;
        mNumColumns = 0;
        mNumElements = 0;
        mEntry = 0;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
bool GMatrix<Real>::operator== (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    for (int i = 0; i < mNumElements; ++i)
    {
        if (mEntry[0][i] != mat.mEntry[0][i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool GMatrix<Real>::operator!= (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    for (int i = 0; i < mNumElements; ++i)
    {
        if (mEntry[0][i] != mat.mEntry[0][i])
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool GMatrix<Real>::operator<  (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    for (int i = 0; i < mNumElements; ++i)
    {
        if (mEntry[0][i] < mat.mEntry[0][i])
        {
            return true;
        }

        if (mEntry[0][i] > mat.mEntry[0][i])
        {
            return false;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool GMatrix<Real>::operator<= (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    for (int i = 0; i < mNumElements; ++i)
    {
        if (mEntry[0][i] < mat.mEntry[0][i])
        {
            return true;
        }

        if (mEntry[0][i] > mat.mEntry[0][i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool GMatrix<Real>::operator>  (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    for (int i = 0; i < mNumElements; ++i)
    {
        if (mEntry[0][i] > mat.mEntry[0][i])
        {
            return true;
        }

        if (mEntry[0][i] < mat.mEntry[0][i])
        {
            return false;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool GMatrix<Real>::operator>= (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    for (int i = 0; i < mNumElements; ++i)
    {
        if (mEntry[0][i] > mat.mEntry[0][i])
        {
            return true;
        }

        if (mEntry[0][i] < mat.mEntry[0][i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::operator+ (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    GMatrix<Real> result(mNumRows, mNumColumns);
    const Real* srcEntry0 = mEntry[0];
    const Real* srcEntry1 = mat.mEntry[0];
    Real* trgEntry = result.mEntry[0];
    for (int i = 0; i < mNumElements; ++i)
    {
        trgEntry[i] = srcEntry0[i] + srcEntry1[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::operator- (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    GMatrix<Real> result(mNumRows, mNumColumns);
    const Real* srcEntry0 = mEntry[0];
    const Real* srcEntry1 = mat.mEntry[0];
    Real* trgEntry = result.mEntry[0];
    for (int i = 0; i < mNumElements; ++i)
    {
        trgEntry[i] = srcEntry0[i] - srcEntry1[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::operator* (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    // 'this' is RxN, 'M' is NxC, 'product = this*M' is RxC
    assertion(mNumColumns == mat.mNumRows, "Mismatch of matrix sizes.\n");
#endif

    GMatrix<Real> result(mNumRows, mat.mNumColumns);
    for (int r = 0; r < result.mNumRows; ++r)
    {
        for (int c = 0; c < result.mNumColumns; ++c)
        {
            for (int m = 0; m < mNumColumns; ++m)
            {
                result.mEntry[r][c] += mEntry[r][m] * mat.mEntry[m][c];
            }
        }
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::operator* (Real scalar) const
{
    GMatrix<Real> result(mNumRows, mNumColumns);
    const Real* srcEntry = mEntry[0];
    Real* trgEntry = result.mEntry[0];
    for (int i = 0; i < mNumElements; ++i)
    {
        trgEntry[i] = scalar*srcEntry[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::operator/ (Real scalar) const
{
    GMatrix<Real> result(mNumRows, mNumColumns);
    Real* trgEntry = result.mEntry[0];
    int i;

    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        const Real* srcEntry = mEntry[0];
        for (i = 0; i < mNumElements; ++i)
        {
            trgEntry[i] = invScalar*srcEntry[i];
        }
    }
    else
    {
        for (i = 0; i < mNumElements; ++i)
        {
            trgEntry[i] = Math<Real>::MAX_REAL;
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::operator- () const
{
    GMatrix<Real> result(mNumRows, mNumColumns);
    const Real* srcEntry = mEntry[0];
    Real* trgEntry = result.mEntry[0];
    for (int i = 0; i < mNumElements; ++i)
    {
        trgEntry[i] = -srcEntry[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>& GMatrix<Real>::operator+= (const GMatrix& mat)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    const Real* srcEntry = mat.mEntry[0];
    Real* trgEntry = mEntry[0];
    for (int i = 0; i < mNumElements; ++i)
    {
        trgEntry[i] += srcEntry[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>& GMatrix<Real>::operator-= (const GMatrix& mat)
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(mNumRows == mat.mNumRows && mNumColumns == mat.mNumColumns,
        "Mismatch of matrix sizes.\n");
#endif
    const Real* srcEntry = mat.mEntry[0];
    Real* trgEntry = mEntry[0];
    for (int i = 0; i < mNumElements; ++i)
    {
        trgEntry[i] -= srcEntry[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>& GMatrix<Real>::operator*= (Real scalar)
{
    Real* trgEntry = mEntry[0];
    for (int i = 0; i < mNumElements; ++i)
    {
        trgEntry[i] *= scalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real>& GMatrix<Real>::operator/= (Real scalar)
{
    Real* trgEntry = mEntry[0];
    int i;

    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        for (i = 0; i < mNumElements; ++i)
        {
            trgEntry[i] *= invScalar;
        }
    }
    else
    {
        for (i = 0; i < mNumElements; ++i)
        {
            trgEntry[i] = Math<Real>::MAX_REAL;
        }
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
GVector<Real> GMatrix<Real>::operator* (const GVector<Real>& vec) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(vec.GetSize() == mNumColumns,
        "Mismatched sizes in operator*.\n");
#endif
    GVector<Real> result(mNumRows);
    for (int r = 0; r < mNumRows; ++r)
    {
        for (int c = 0; c < mNumColumns; ++c)
        {
            result[r] += mEntry[r][c]*vec[c];
        }
            
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real GMatrix<Real>::QForm (const GVector<Real>& u, const GVector<Real>& v)
    const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    assertion(u.GetSize() == mNumRows && v.GetSize() == mNumColumns,
        "Mismatched sizes in QForm.\n");
#endif
    return u.Dot((*this)*v);
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::Transpose () const
{
    GMatrix<Real> result(mNumColumns, mNumRows);
    for (int r = 0; r < mNumRows; ++r)
    {
        for (int c = 0; c < mNumColumns; ++c)
        {
            result.mEntry[c][r] = mEntry[r][c];
        }
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::TransposeTimes (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    // P = A^T*B, P[r][c] = sum_m A[m][r]*B[m][c]
    assertion(mNumRows == mat.mNumRows, "Mismatch in TransposeTimes\n");
#endif
    GMatrix<Real> result(mNumColumns, mat.mNumColumns);
    for (int r = 0; r < result.mNumRows; ++r)
    {
        for (int c = 0; c < result.mNumColumns; ++c)
        {
            for (int m = 0; m < mNumRows; ++m)
            {
                result.mEntry[r][c] += mEntry[m][r] * mat.mEntry[m][c];
            }
        }
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::TimesTranspose (const GMatrix& mat) const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    // P = A*B^T, P[r][c] = sum_m A[r][m]*B[c][m]
    assertion(mNumColumns == mat.mNumColumns, "Mismatch in TimesTranspose\n");
#endif
    GMatrix<Real> result(mNumRows, mat.mNumRows);
    for (int r = 0; r < result.mNumRows; ++r)
    {
        for (int c = 0; c < result.mNumColumns; ++c)
        {
            for (int m = 0; m < mNumColumns; ++m)
            {
                result.mEntry[r][c] +=  mEntry[r][m] * mat.mEntry[c][m];
            }
        }
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
GMatrix<Real> GMatrix<Real>::TransposeTimesTranspose (const GMatrix& mat)
    const
{
#ifdef WM5_ASSERT_GMATRIX_OUT_OF_RANGE
    // P = A*B^T, P[r][c] = sum_m A[m][r]*B[c][m]
    assertion(mNumColumns == mat.mNumColumns,
        "Mismatch in TransposeTimesTranspose\n");
#endif
    GMatrix<Real> result(mNumColumns, mat.mNumRows);
    for (int r = 0; r < result.mNumRows; ++r)
    {
        for (int c = 0; c < result.mNumColumns; ++c)
        {
            for (int m = 0; m < mNumColumns; ++m)
            {
                result.mEntry[r][c] +=  mEntry[m][r] * mat.mEntry[c][m];
            }
        }
    }
    return result;
}
//----------------------------------------------------------------------------
