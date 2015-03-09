// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <int N>
Integer<N>::Integer (int i)
{
    if (i >= 0)
    {
        memset(mBuffer, 0, INT_BYTES);
    }
    else
    {
        memset(mBuffer, 0xFF, INT_BYTES);
    }
    memcpy(mBuffer, &i, sizeof(int));

#ifdef WM5_BIG_ENDIAN
    short save = mBuffer[0];
    mBuffer[0] = mBuffer[1];
    mBuffer[1] = save;
#endif
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>::Integer (const Integer& value)
{
    memcpy(mBuffer, value.mBuffer, INT_BYTES);
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>::~Integer ()
{
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>& Integer<N>::operator= (const Integer& value)
{
    memcpy(mBuffer, value.mBuffer, INT_BYTES);
    return *this;
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::GetSign () const
{
    return (mBuffer[INT_LAST] & 0x8000) ? -1 : +1;
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::operator== (const Integer& value) const
{
    return Compare(*this, value) == 0;
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::operator!= (const Integer& value) const
{
    return Compare(*this, value) != 0;
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::operator< (const Integer& value) const
{
    int s0 = GetSign();
    int s1 = value.GetSign();
    if (s0 > 0)
    {
        if (s1 > 0)
        {
            return Compare(*this, value) < 0;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (s1 > 0)
        {
            return true;
        }
        else
        {
            return Compare(*this, value) < 0;
        }
    }
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::operator<= (const Integer& value) const
{
    int s0 = GetSign();
    int s1 = value.GetSign();
    if (s0 > 0)
    {
        if (s1 > 0)
        {
            return Compare(*this, value) <= 0;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (s1 > 0)
        {
            return true;
        }
        else
        {
            return Compare(*this, value) <= 0;
        }
    }
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::operator> (const Integer& value) const
{
    int s0 = GetSign();
    int s1 = value.GetSign();
    if (s0 > 0)
    {
        if (s1 > 0)
        {
            return Compare(*this, value) > 0;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (s1 > 0)
        {
            return false;
        }
        else
        {
            return Compare(*this, value) > 0;
        }
    }
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::operator>= (const Integer& value) const
{
    int s0 = GetSign();
    int s1 = value.GetSign();
    if (s0 > 0)
    {
        if (s1 > 0)
        {
            return Compare(*this, value) >= 0;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (s1 > 0)
        {
            return false;
        }
        else
        {
            return Compare(*this, value) >= 0;
        }
    }
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::Compare (const Integer<N>& value0, const Integer<N>& value1)
{
    for (int i = INT_LAST; i >= 0; --i)
    {
        unsigned int uiValue0 = (unsigned int)value0.mBuffer[i];
        unsigned int uiValue1 = (unsigned int)value1.mBuffer[i];
        if (uiValue0 < uiValue1)
        {
            return -1;
        }
        else if (uiValue0 > uiValue1)
        {
            return +1;
        }
    }
    return 0;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator- () const
{
    Integer result = *this;

    // Negate the bits.
    int i;
    for (i = 0; i < INT_SIZE; ++i)
    {
        result.mBuffer[i] = ~result.mBuffer[i];
    }

    // Add 1 (place in carry bit and add zero to 'result').
    unsigned int carry = 1;
    for (i = 0; i < INT_SIZE; ++i)
    {
        unsigned int b1 = result.ToUnsignedInt(i);
        unsigned int sum = b1 + carry;
        result.FromUnsignedInt(i, sum);
        carry = (sum & 0x00010000) ? 1 : 0;
    }

    // Test for overflow.
    if (result.GetSign() == GetSign())
    {
        assertion(result == 0, "Integer overflow\n");
    }

    return result;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator+ (const Integer& value) const
{
    Integer result;

    unsigned int carry = 0;
    for (int i = 0; i < INT_SIZE; ++i)
    {
        unsigned int b0 = ToUnsignedInt(i);
        unsigned int b1 = value.ToUnsignedInt(i);
        unsigned int sum = b0 + b1 + carry;
        result.FromUnsignedInt(i, sum);
        carry = (sum & 0x00010000) ? 1 : 0;
    }

    // Test for overflow.
    if (GetSign() == value.GetSign())
    {
        assertion(result.GetSign() == GetSign(), "Integer overflow\n");
    }

    return result;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator- (const Integer& value) const
{
    return *this + (-value);
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator* (const Integer& value) const
{
    int s0 = GetSign();
    int s1 = value.GetSign();
    int sProduct = s0*s1;
    Integer op0 = (s0 > 0 ? *this : -*this);
    Integer op1 = (s1 > 0 ? value : -value);

    // Product of single-digit number with multiple-digit number.
    unsigned short product[2*INT_SIZE];
    unsigned short* pCurrent = product;

    // Product of the two multiple-digit operands.
    unsigned short result[2*INT_SIZE];
    unsigned short* rCurrent = result;
    memset(result,0,2*INT_BYTES);

    for (int i0 = 0, size = 2*INT_SIZE; i0 < INT_SIZE; ++i0, --size)
    {
        unsigned int b0 = op0.ToUnsignedInt(i0);
        if (b0 > 0)
        {
            unsigned short* pBuffer = pCurrent;
            unsigned int carry = 0;
            int i1;
            for (i1 = 0; i1 < INT_SIZE; ++i1)
            {
                unsigned int b1 = op1.ToUnsignedInt(i1);
                unsigned int prod = b0*b1 + carry;
                *pBuffer++ = (unsigned short)(prod & 0x0000FFFF);
                carry = (prod & 0xFFFF0000) >> 16;
            }
            *pBuffer = (unsigned short)carry;

            unsigned short* rBuffer = rCurrent;
            pBuffer = pCurrent;
            carry = 0;
            unsigned int sum, term0, term1;
            for (i1 = 0; i1 <= INT_SIZE; ++i1)
            {
                term0 = (unsigned int)(*pBuffer++);
                term1 = (unsigned int)(*rBuffer);
                sum = term0 + term1 + carry;
                *rBuffer++ = (unsigned short)(sum & 0x0000FFFF);
                carry = (sum & 0x00010000) ? 1 : 0;
            }

            for (/**/; carry > 0 && i1 < size; i1++)
            {
                term0 = (unsigned int)(*rBuffer);
                sum = term0 + carry;
                *rBuffer++ = (unsigned short)(sum & 0x0000FFFF);
                carry = (sum & 0x00010000) ? 1 : 0;
            }
        }

        pCurrent++;
        rCurrent++;
    }

    // Test for overflow.  You can test earlier inside the previous loop, but
    // testing here allows you to get an idea of how much overflow there is.
    // This information might be useful for an application to decide how large
    // to choose the integer size.
    for (int i = 2*INT_SIZE-1; i >= INT_SIZE; --i)
    {
        assertion(result[i] == 0, "Integer overflow\n");
    }
    assertion((result[INT_LAST] & 0x8000) == 0, "Integer overflow\n");

    Integer intResult;
    memcpy(intResult.mBuffer, result, INT_BYTES);
    if (sProduct < 0)
    {
        intResult = -intResult;
    }

    return intResult;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> operator* (int i, const Integer<N>& value)
{
    return value*i;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator/ (const Integer& value) const
{
    // TO DO.  On division by zero, return INVALID or signed INFINITY?
    Integer quotient, remainder;
    return GetDivMod(*this, value, quotient, remainder) ? quotient : 0;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator% (const Integer& value) const
{
    // TO DO.  On division by zero, return INVALID or signed INFINITY?
    Integer quotient, remainder;
    return GetDivMod(*this, value, quotient, remainder) ? remainder : 0;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>& Integer<N>::operator+= (const Integer& value)
{
    *this = *this + value;
    return *this;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>& Integer<N>::operator-= (const Integer& value)
{
    *this = *this - value;
    return *this;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>& Integer<N>::operator*= (const Integer& value)
{
    *this = *this * value;
    return *this;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>& Integer<N>::operator/= (const Integer& value)
{
    *this = *this / value;
    return *this;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator<< (int shift) const
{
    if (shift < 0)
    {
        return 0;
    }
    if (shift == 0)
    {
        return *this;
    }

    // Number of 16-bit blocks to shift.
    Integer result;
    int blocks = shift/16;
    if (blocks > INT_LAST)
    {
        return 0;
    }

    int i;
    if (blocks > 0)
    {
        int j = INT_LAST - blocks;
        for (i = INT_LAST; j >= 0; --i, --j)
        {
            result.mBuffer[i] = mBuffer[j];
        }

        for (/**/; i >= 0; --i)
        {
            result.mBuffer[i] = 0;
        }
    }

    // Number of left-over bits to shift.
    int bits = shift % 16;
    if (bits > 0)
    {
        unsigned int lo, hi, value;
        int iM1;
        for (i = INT_LAST, iM1 = i - 1; iM1 >= 0; --i, --iM1)
        {
            lo = ToUnsignedInt(iM1);
            hi = ToUnsignedInt(i);
            value = (lo | (hi << 16));
            value <<= bits;
            result.FromUnsignedInt(i, ((0xFFFF0000 & value) >> 16));
        }

        value = ToUnsignedInt(0);
        value <<= bits;
        result.FromUnsignedInt(0, (0x0000FFFF & value));
    }

    return result;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N> Integer<N>::operator>> (int shift) const
{
    if (shift < 0)
    {
        return 0;
    }
    if (shift == 0)
    {
        return *this;
    }

    // Number of 16-bit blocks to shift.
    Integer result;
    int blocks = shift/16;
    if (blocks > INT_LAST)
    {
        return 0;
    }

    int i;
    if (blocks > 0)
    {
        int j = blocks;
        for (i = 0; j <= INT_LAST; ++i, ++j)
        {
            result.mBuffer[i] = mBuffer[j];
        }

        if (GetSign() > 0)
        {
            for (/**/; i <= INT_LAST; ++i)
            {
                result.mBuffer[i] = 0;
            }
        }
        else
        {
            for (/**/; i <= INT_LAST; ++i)
            {
                result.mBuffer[i] = (short)(0x0000FFFFu);
            }
        }
    }

    // Number of left-over bits to shift.
    int bits = shift % 16;
    if (bits > 0)
    {
        unsigned int value;
        int p1;
        for (i = 0, p1 = 1; p1 <= INT_LAST; ++i, ++p1)
        {
            value = ToUnsignedInt(i, p1);
            value >>= bits;
            result.FromUnsignedInt(i, value);
        }

        value = ToUnsignedInt(INT_LAST);
        if (GetSign() < 0)
        {
            value |= 0xFFFF0000;  // sign extension
        }
        value >>= bits;
        result.FromUnsignedInt(INT_LAST, value);
    }

    return result;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>& Integer<N>::operator<<= (int shift)
{
    if (shift <= 0)
    {
        return *this;
    }

    // Number of 16-bit blocks to shift.
    int blocks = shift/16;
    if (blocks > INT_LAST)
    {
        return *this;
    }

    int i;
    if (blocks > 0)
    {
        int j = INT_LAST - blocks;
        for (i = INT_LAST; j >= 0; --i, --j)
        {
            mBuffer[i] = mBuffer[j];
        }

        for (/**/; i >= 0; --i)
        {
            mBuffer[i] = 0;
        }
    }

    // Number of left-over bits to shift.
    int bits = shift % 16;
    if (bits > 0)
    {
        unsigned int value;
        int m1;
        for (i = INT_LAST, m1 = i-1; m1 >= 0; i--, m1--)
        {
            value = ToUnsignedInt(m1, i);
            value <<= bits;
            FromUnsignedInt(i, ((0xFFFF0000 & value) >> 16));
        }

        value = ToUnsignedInt(0);
        value <<= bits;
        FromUnsignedInt(0, (0x0000FFFF & value));
    }

    return *this;
}
//----------------------------------------------------------------------------
template <int N>
Integer<N>& Integer<N>::operator>>= (int shift)
{
    if (shift <= 0)
    {
        return *this;
    }

    // Number of 16-bit blocks to shift.
    int blocks = shift/16;
    if (blocks > INT_LAST)
    {
        return *this;
    }

    int i;
    if (blocks > 0)
    {
        int j = blocks;
        for (i = 0, j = blocks; j <= INT_LAST; ++i, ++j)
        {
            mBuffer[i] = mBuffer[j];
        }

        if (GetSign() > 0)
        {
            for (/**/; i <= INT_LAST; ++i)
            {
                mBuffer[i] = 0;
            }
        }
        else
        {
            for (/**/; i <= INT_LAST; ++i)
            {
                mBuffer[i] = -1;
            }
        }
    }

    // Number of left-over bits to shift.
    int bits = shift % 16;
    if (bits > 0)
    {
        unsigned int value;
        int p1;
        for (i = 0, p1 = 1; p1 <= INT_LAST; ++i, ++p1)
        {
            value = ToUnsignedInt(i, p1);
            value >>= bits;
            FromUnsignedInt(i, value);
        }

        value = ToUnsignedInt(INT_LAST);
        if (GetSign() < 0)
        {
            value |= 0xFFFF0000;  // sign extension
        }
        value >>= bits;
        FromUnsignedInt(INT_LAST, value);
    }

    return *this;
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::GetDivMod (const Integer& numer, const Integer& denom,
    Integer& quotient, Integer& remainder)
{
    if (denom == 0)
    {
        assertion(false, "Division by zero\n");
        quotient = 0;
        remainder = 0;
        return false;
    }

    if (numer == 0)
    {
        quotient = 0;
        remainder = 0;
        return true;
    }

    // Work with the absolute values of the numerator and denominator.
    int s0 = numer.GetSign();
    int s1 = denom.GetSign();
    Integer absNumer = s0*numer;
    Integer absDenom = s1*denom;

    int compare = Compare(absNumer, absDenom);
    if (compare < 0)
    {
        // numerator < denominator:  numerator = 0*denominator + numerator
        quotient = 0;
        remainder = numer;
        return true;
    }

    if (compare == 0)
    {
        // numerator == denominator:  numerator = 1*denominator + 0
        quotient = 1;
        remainder = 0;
        return true;
    }

    // numerator > denominator, do the division to find quotient and remainder
    if (absDenom > 0x0000FFFF)
    {
        DivMultiple(absNumer, absDenom, quotient, remainder);
    }
    else
    {
        DivSingle(absNumer, absDenom.mBuffer[0], quotient, remainder);
    }

    // Apply the original signs of numerator and denominator.
    quotient *= s0*s1;
    remainder *= s0;

#ifdef _DEBUG
    Integer test = numer - denom*quotient - remainder;
    assertion(test == 0, "Invalid result\n");
#endif
    return true;
}
//----------------------------------------------------------------------------
template <int N>
void Integer<N>::DivSingle (const Integer& numer, short denom,
    Integer& quotient, Integer& remainder)
{
    // The denominator is a single "digit".
    unsigned int uiDenom = 0x0000FFFF & (unsigned int)denom;

    // Get the numerator.
    int nStart = numer.GetLeadingBlock();
    const short* numBuffer = &numer.mBuffer[nStart];
    unsigned int digit1 = 0;

    // Get the quotient.
    short* quoBuffer = &quotient.mBuffer[nStart];
    quotient = 0;
    int lastNonZero = -1;
    for (int i = nStart; i >= 0; --i, --numBuffer, --quoBuffer)
    {
        unsigned int digitB = digit1;
        digit1 = 0x0000FFFF & (unsigned int)(*numBuffer);
        unsigned int uiNumer = (digitB << 16) | digit1;
        unsigned int uiQuotient = uiNumer/uiDenom;
        digit1 = uiNumer - uiQuotient*uiDenom;
        *quoBuffer = (short)(uiQuotient & 0x0000FFFF);
        if (lastNonZero == -1 && uiQuotient > 0)
        {
            lastNonZero = i;
        }
    }
    assertion(lastNonZero >= 0, "Unexpected result\n");

    // Get the remainder.
    remainder = 0;
    if (digit1 & 0xFFFF0000)
    {
        memcpy(remainder.mBuffer, &digit1, 2*sizeof(short));
#ifdef WM5_BIG_ENDIAN
        short save = remainder.mBuffer[0];
        remainder.mBuffer[0] = remainder.mBuffer[1];
        remainder.mBuffer[1] = save;
#endif
    }
    else
    {
        unsigned short tmp = (unsigned short)digit1;
        memcpy(remainder.mBuffer, &tmp, sizeof(short));
    }
}
//----------------------------------------------------------------------------
template <int N>
void Integer<N>::DivMultiple (const Integer& numer,
    const Integer& denom, Integer& quotient, Integer& remainder)
{
    quotient = 0;
    remainder = 0;

    // Normalization to allow good estimate of quotient.  TO DO:  It is
    // possible that the numerator is large enough that normalization causes
    // overflow when computing the product adjust*numer; an assertion will
    // fire in this case.  Ideally, the overflow would be allowed and the
    // digit in the overflow position becomes the first digit of the numerator
    // in the division algorithm.  This will require a mixture of Integer<N>
    // and Integer<N+1>, though.
    int dInit = denom.GetLeadingBlock();
    int leadingDigit = denom.ToInt(dInit);
    int adjust = 0x10000/(leadingDigit + 1);
    Integer adjNum = adjust*numer;
    Integer adjDen = adjust*denom;
    assertion(adjDen.GetLeadingBlock() == dInit, "Unexpected result\n");

    // Get first two "digits" of denominator.
    unsigned int d1 = adjDen.ToUnsignedInt(dInit);
    unsigned int d2 = adjDen.ToUnsignedInt(dInit - 1);

    // Determine the maximum necessary division steps.
    int nInit = adjNum.GetLeadingBlock();
    assertion(nInit >= dInit, "Unexpected result\n");
    int qInit;
    unsigned int rHat;
    if (nInit != dInit)
    {
        qInit = nInit - dInit - 1;
        rHat = 1;
    }
    else
    {
        qInit = 0;
        rHat = 0;
    }

    for (/**/; qInit >= 0; --qInit)
    {
        // Get first three indices of remainder.
        unsigned int n0, n1, n2;
        if (rHat > 0)
        {
            n0 = adjNum.ToUnsignedInt(nInit--);
            n1 = adjNum.ToUnsignedInt(nInit--);
            n2 = adjNum.ToUnsignedInt(nInit);
        }
        else
        {
            n0 = 0;
            n1 = adjNum.ToUnsignedInt(nInit--);
            n2 = adjNum.ToUnsignedInt(nInit);
        }

        // Estimate the quotient.
        unsigned int tmp = (n0 << 16) | n1;
        unsigned int qHat = (n0 != d1 ? tmp/d1 : 0x0000FFFF);
        unsigned int prod = qHat*d1;
        assertion(tmp >= prod, "Unexpected result\n");
        rHat = tmp - prod;
        if (d2*qHat > 0x10000*rHat + n2)
        {
            qHat--;
            rHat += d1;
            if (d2*qHat > 0x10000*rHat + n2)
            {
                // If this block is entered, we have exactly the quotient for
                // the division.  The adjustment block of code later cannot
                // happen.
                qHat--;
                rHat += d1;
            }
        }

        // Compute the quotient for this step of the division.
        Integer localQuo;
        localQuo.FromUnsignedInt(qInit, qHat);

        // Compute the remainder.
        Integer product = localQuo*adjDen;
        adjNum -= product;
        if (adjNum < 0)
        {
            qHat--;
            adjNum += adjDen;
            assertion(adjNum >= 0, "Unexpected result\n");
        }

        // Set quotient digit.
        quotient.FromUnsignedInt(qInit, qHat);

        if (adjNum >= adjDen)
        {
            // Prepare to do another division step.
            nInit = adjNum.GetLeadingBlock();
        }
        else
        {
            // Remainder is smaller than divisor, finished dividing.
            break;
        }
    }

    // Unnormalize the remainder.
    if (adjNum > 0)
    {
        short divisor = (short)(adjust & 0x0000FFFF);
        Integer shouldBeZero;
        DivSingle(adjNum, divisor, remainder, shouldBeZero);
    }
    else
    {
        remainder = 0;
    }
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::GetLeadingBlock () const
{
    for (int i = INT_LAST; i >= 0; --i)
    {
        if (mBuffer[i] != 0)
        {
            return i;
        }
    }
    return -1;
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::GetTrailingBlock () const
{
    for (int i = 0; i <= INT_LAST; ++i)
    {
        if (mBuffer[i] != 0)
        {
            return i;
        }
    }
    return -1;
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::GetLeadingBit (int i) const
{
    assertion(0 <= i && i <= INT_LAST, "Input out of range\n");
    if (i < 0 || i > INT_LAST)
    {
        return -1;
    }

    // This is a binary search for the high-order bit of mBuffer[i].  The
    // return value is the index into the bits (0 <= index < 16).
    int value = (int)mBuffer[i];
    if ((value & 0xFF00) != 0)
    {
        if ((value & 0xF000) != 0)
        {
            if ((value & 0xC000) != 0)
            {
                if ((value & 0x8000) != 0)
                {
                    return 15;
                }
                else // (value & 0x4000) != 0
                {
                    return 14;
                }
            }
            else  // (value & 0x3000) != 0
            {
                if ((value & 0x2000) != 0)
                {
                    return 13;
                }
                else  // (value & 0x1000) != 0
                {
                    return 12;
                }
            }
        }
        else  // (value & 0x0F00) != 0
        {
            if ((value & 0x0C00) != 0)
            {
                if ((value & 0x0800) != 0)
                {
                    return 11;
                }
                else  // (value & 0x0400) != 0
                {
                    return 10;
                }
            }
            else  // (value & 0x0300) != 0
            {
                if ((value & 0x0200) != 0)
                {
                    return 9;
                }
                else  // (value & 0x0100) != 0
                {
                    return 8;
                }
            }
        }
    }
    else  // (value & 0x00FF)
    {
        if ((value & 0x00F0) != 0)
        {
            if ((value & 0x00C0) != 0)
            {
                if ((value & 0x0080) != 0)
                {
                    return 7;
                }
                else  // (value & 0x0040) != 0
                {
                    return 6;
                }
            }
            else  // (value & 0x0030) != 0
            {
                if ((value & 0x0020) != 0)
                {
                    return 5;
                }
                else  // (value & 0x0010) != 0
                {
                    return 4;
                }
            }
        }
        else  // (value & 0x000F) != 0
        {
            if ((value & 0x000C) != 0)
            {
                if ((value & 0x0008) != 0)
                {
                    return 3;
                }
                else  // (value & 0x0004) != 0
                {
                    return 2;
                }
            }
            else  // (value & 0x0003) != 0
            {
                if ((value & 0x0002) != 0)
                {
                    return 1;
                }
                else  // (value & 0x0001) != 0
                {
                    return 0;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::GetTrailingBit (int i) const
{
    assertion(0 <= i && i <= INT_LAST, "Input out of range\n");
    if (i < 0 || i > INT_LAST)
    {
        return -1;
    }

    // This is a binary search for the low-order bit of mBuffer[i].  The
    // return value is the index into the bits (0 <= index < 16).
    int value = (int)mBuffer[i];
    if ((value & 0x00FF) != 0)
    {
        if ((value & 0x000F) != 0)
        {
            if ((value & 0x0003) != 0)
            {
                if ((value & 0x0001) != 0)
                {
                    return 0;
                }
                else // (value & 0x0002) != 0
                {
                    return 1;
                }
            }
            else  // (value & 0x000C) != 0
            {
                if ((value & 0x0004) != 0)
                {
                    return 2;
                }
                else  // (value & 0x0080) != 0
                {
                    return 3;
                }
            }
        }
        else  // (value & 0x00F0) != 0
        {
            if ((value & 0x0030) != 0)
            {
                if ((value & 0x0010) != 0)
                {
                    return 4;
                }
                else  // (value & 0x0020) != 0
                {
                    return 5;
                }
            }
            else  // (value & 0x00C0) != 0
            {
                if ((value & 0x0040) != 0)
                {
                    return 6;
                }
                else  // (value & 0x0080) != 0
                {
                    return 7;
                }
            }
        }
    }
    else  // (value & 0xFF00)
    {
        if ((value & 0x0F00) != 0)
        {
            if ((value & 0x0300) != 0)
            {
                if ((value & 0x0100) != 0)
                {
                    return 8;
                }
                else  // (value & 0x0200) != 0
                {
                    return 9;
                }
            }
            else  // (value & 0x0C00) != 0
            {
                if ((value & 0x0400) != 0)
                {
                    return 10;
                }
                else  // (value & 0x0800) != 0
                {
                    return 11;
                }
            }
        }
        else  // (value & 0xF000) != 0
        {
            if ((value & 0x3000) != 0)
            {
                if ((value & 0x1000) != 0)
                {
                    return 12;
                }
                else  // (value & 0x2000) != 0
                {
                    return 13;
                }
            }
            else  // (value & 0xC000) != 0
            {
                if ((value & 0x4000) != 0)
                {
                    return 14;
                }
                else  // (value & 0x8000) != 0
                {
                    return 15;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::GetLeadingBit () const
{
    int block = GetLeadingBlock();
    if (block >= 0)
    {
        int bit = GetLeadingBit(block);
        if (bit >= 0)
        {
            return bit + 16*block;
        }
    }

    return -1;
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::GetTrailingBit () const
{
    int block = GetTrailingBlock();
    if (block >= 0)
    {
        int bit = GetTrailingBit(block);
        if (bit >= 0)
        {
            return bit + 16*block;
        }
    }

    return -1;
}
//----------------------------------------------------------------------------
template <int N>
void Integer<N>::SetBit (int i, bool on)
{
    // assert(0 <= i && i <= INT_LAST);
    int block = i/16;
    int bit = i % 16;
    if (on)
    {
        mBuffer[block] |= (1 << bit);
    }
    else
    {
        mBuffer[block] &= ~(1 << bit);
    }
}
//----------------------------------------------------------------------------
template <int N>
bool Integer<N>::GetBit (int i) const
{
    // assert(0 <= i && i <= INT_LAST);
    int block = i/16;
    int bit = i % 16;
    return (mBuffer[block] & (1 << bit)) != 0;
}
//----------------------------------------------------------------------------
template <int N>
unsigned int Integer<N>::ToUnsignedInt (int i) const
{
    // assert(0 <= i && i <= INT_LAST);
    return 0x0000FFFF & (unsigned int)mBuffer[i];
}
//----------------------------------------------------------------------------
template <int N>
void Integer<N>::FromUnsignedInt (int i, unsigned int value)
{
    // assert(0 <= i && i <= INT_LAST);
    mBuffer[i] = (short)(value & 0x0000FFFF);
}
//----------------------------------------------------------------------------
template <int N>
unsigned int Integer<N>::ToUnsignedInt (int lo, int hi) const
{
    unsigned int uiLo = ToUnsignedInt(lo);
    unsigned int uiHi = ToUnsignedInt(hi);
    return (uiLo | (uiHi << 16));
}
//----------------------------------------------------------------------------
template <int N>
int Integer<N>::ToInt (int i) const
{
    // assert(0 <= i && i <= INT_LAST);
    return (int)(0x0000FFFF & (unsigned int)mBuffer[i]);
}
//----------------------------------------------------------------------------
