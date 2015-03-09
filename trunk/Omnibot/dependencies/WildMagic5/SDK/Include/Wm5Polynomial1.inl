// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>::Polynomial1 (int degree)
{
    if (degree >= 0)
    {
        mDegree = degree;
        mCoeff = new1<Real>(mDegree + 1);
    }
    else
    {
        // default creation
        mDegree = -1;
        mCoeff = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>::Polynomial1 (const Polynomial1& poly)
{
    mDegree = poly.mDegree;
    mCoeff = new1<Real>(mDegree + 1);
    for (int i = 0; i <= mDegree; ++i)
    {
        mCoeff[i] = poly.mCoeff[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>::~Polynomial1 ()
{
    delete1(mCoeff);
}
//----------------------------------------------------------------------------
template <typename Real>
void Polynomial1<Real>::SetDegree (int degree)
{
    mDegree = degree;
    delete1(mCoeff);

    if (mDegree >= 0)
    {
        mCoeff = new1<Real>(mDegree + 1);
    }
    else
    {
        mCoeff = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Polynomial1<Real>::GetDegree () const
{
    return mDegree;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Polynomial1<Real>::operator const Real* () const
{
    return mCoeff;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Polynomial1<Real>::operator Real* ()
{
    return mCoeff;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real& Polynomial1<Real>::operator[] (int i) const
{
    assertion(0 <= i && i <= mDegree, "Invalid input to operator[]\n");
    return mCoeff[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real& Polynomial1<Real>::operator[] (int i)
{
    assertion(0 <= i && i <= mDegree, "Invalid input to operator[]\n");
    return mCoeff[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator= (const Polynomial1& poly)
{
    delete1(mCoeff);
    mDegree = poly.mDegree;

    if (mDegree >= 0)
    {
        mCoeff = new1<Real>(mDegree + 1);
        for (int i = 0; i <= mDegree; ++i)
        {
            mCoeff[i] = poly.mCoeff[i];
        }
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Polynomial1<Real>::operator() (Real t) const
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator()\n");

    Real result = mCoeff[mDegree];
    for (int i = mDegree - 1; i >= 0; --i)
    {
        result *= t;
        result += mCoeff[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator+ (const Polynomial1& poly) const
{
    assertion(mDegree >= 0 && poly.mDegree >= 0,
        "Degrees must be nonnegative in operator+\n");

    Polynomial1 result;
    int i;

    if (mDegree > poly.mDegree)
    {
        result.SetDegree(mDegree);
        for (i = 0; i <= poly.mDegree; ++i)
        {
            result.mCoeff[i] = mCoeff[i] + poly.mCoeff[i];
        }
        for (i = poly.mDegree + 1; i <= mDegree; ++i)
        {
            result.mCoeff[i] = mCoeff[i];
        }

    }
    else
    {
        result.SetDegree(poly.mDegree);
        for (i = 0; i <= mDegree; ++i)
        {
            result.mCoeff[i] = mCoeff[i] + poly.mCoeff[i];
        }
        for (i = mDegree + 1; i <= poly.mDegree; ++i)
        {
            result.mCoeff[i] = poly.mCoeff[i];
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator- (const Polynomial1& poly) const
{
    assertion(mDegree >= 0 && poly.mDegree >= 0,
        "Degrees must be nonnegative in operator-\n");

    Polynomial1 result;
    int i;

    if (mDegree > poly.mDegree)
    {
        result.SetDegree(mDegree);
        for (i = 0; i <= poly.mDegree; ++i)
        {
            result.mCoeff[i] = mCoeff[i] - poly.mCoeff[i];
        }
        for (i = poly.mDegree + 1; i <= mDegree; ++i)
        {
            result.mCoeff[i] = mCoeff[i];
        }

    }
    else
    {
        result.SetDegree(poly.mDegree);
        for (i = 0; i <= mDegree; ++i)
        {
            result.mCoeff[i] = mCoeff[i] - poly.mCoeff[i];
        }
        for (i = mDegree + 1; i <= poly.mDegree; ++i)
        {
            result.mCoeff[i] = -poly.mCoeff[i];
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator* (const Polynomial1& poly) const
{
    assertion(mDegree >= 0 && poly.mDegree >= 0,
        "Degrees must be nonnegative in operator*\n");

    Polynomial1 result(mDegree + poly.mDegree);

    memset(result.mCoeff, 0, (result.mDegree + 1)*sizeof(Real));
    for (int i0 = 0; i0 <= mDegree; ++i0)
    {
        for (int i1 = 0; i1 <= poly.mDegree; ++i1)
        {
            int i2 = i0 + i1;
            result.mCoeff[i2] += mCoeff[i0]*poly.mCoeff[i1];
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator+ (Real scalar) const
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator+\n");

    Polynomial1 result(*this);
    result.mCoeff[0] += scalar;
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator- (Real scalar) const
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator-\n");

    Polynomial1 result(*this);
    result.mCoeff[0] -= scalar;
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator* (Real scalar) const
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator*\n");

    Polynomial1 result(mDegree);
    for (int i = 0; i <= mDegree; ++i)
    {
        result.mCoeff[i] = scalar*mCoeff[i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator/ (Real scalar) const
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator/\n");

    Polynomial1 result(mDegree);
    int i;

    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        for (i = 0; i <= mDegree; ++i)
        {
            result.mCoeff[i] = invScalar*mCoeff[i];
        }
    }
    else
    {
        for (i = 0; i <= mDegree; ++i)
        {
            result.mCoeff[i] = Math<Real>::MAX_REAL;
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::operator- () const
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator-\n");

    Polynomial1 result(mDegree);
    for (int i = 0; i <= mDegree; ++i)
    {
        result.mCoeff[i] = -mCoeff[i];
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator += (const Polynomial1& poly)
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator+=\n");

    *this = *this + poly;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator -= (const Polynomial1& poly)
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator-=\n");

    *this = *this - poly;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator *= (const Polynomial1& poly)
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator*=\n");

    *this = (*this)*poly;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator += (Real scalar)
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator+=\n");

    mCoeff[0] += scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator -= (Real scalar)
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator-=\n");

    mCoeff[0] -= scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator *= (Real scalar)
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator*=\n");

    *this = (*this)*scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real>& Polynomial1<Real>::operator /= (Real scalar)
{
    assertion(mDegree >= 0, "Degree must be nonnegative in operator/=\n");

    *this = (*this)/scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::GetDerivative () const
{
    if (mDegree > 0)
    {
        Polynomial1 result(mDegree - 1);
        for (int i0 = 0, i1 = 1; i0 < mDegree; ++i0, ++i1)
        {
            result.mCoeff[i0] = i1*mCoeff[i1];
        }
        return result;
    }
    else if (mDegree == 0)
    {
        Polynomial1 result(0);
        result.mCoeff[0] = (Real)0;
        return result;
    }
    else
    {
        // invalid in, invalid out
        return Polynomial1<Real>();
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> Polynomial1<Real>::GetInversion () const
{
    Polynomial1 result(mDegree);
    for (int i = 0; i <= mDegree; ++i)
    {
        result.mCoeff[i] = mCoeff[mDegree - i];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void Polynomial1<Real>::Compress (Real epsilon)
{
    int i;
    for (i = mDegree; i >= 0; --i)
    {
        if (Math<Real>::FAbs(mCoeff[i]) <= epsilon)
        {
            --mDegree;
        }
        else
        {
            break;
        }
    }

    if (mDegree >= 0)
    {
        Real invLeading = ((Real)1)/mCoeff[mDegree];
        mCoeff[mDegree] = (Real)1;
        for (i = 0; i < mDegree; ++i)
        {
            mCoeff[i] *= invLeading;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Polynomial1<Real>::Divide (const Polynomial1& divisor,
    Polynomial1& quotient, Polynomial1& remainder, Real epsilon) const
{
    int quotientDegree = mDegree - divisor.mDegree;
    if (quotientDegree >= 0)
    {
        quotient.SetDegree(quotientDegree);

        // Temporary storage for the remainder.
        Polynomial1 tmp = *this;

        // Do the division (Euclidean algorithm).
        Real inv = ((Real)1)/divisor[divisor.mDegree];
        for (int i = quotientDegree; i >= 0; --i)
        {
            int j = divisor.mDegree + i;
            quotient[i] = inv*tmp[j];
            for (j--; j >= i; j--)
            {
                tmp[j] -= quotient[i]*divisor[j - i];
            }
        }

        // Calculate the correct degree for the remainder.
        int remainderDegree = divisor.mDegree - 1;
        while (remainderDegree > 0
            && Math<Real>::FAbs(tmp[remainderDegree]) < epsilon)
        {
            --remainderDegree;
        }

        if (remainderDegree == 0 && Math<Real>::FAbs(tmp[0]) < epsilon)
        {
            tmp[0] = (Real)0;
        }

        remainder.SetDegree(remainderDegree);
        size_t numBytes = (remainderDegree + 1)*sizeof(Real);
        memcpy(remainder.mCoeff, tmp.mCoeff, numBytes);
    }
    else
    {
        quotient.SetDegree(0);
        quotient[0] = (Real)0;
        remainder = *this;
    }
}
//----------------------------------------------------------------------------
