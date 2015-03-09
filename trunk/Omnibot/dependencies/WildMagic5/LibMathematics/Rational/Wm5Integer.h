// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5INTEGER_H
#define WM5INTEGER_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Assert.h"

namespace Wm5
{

template <int N> class Rational;

// N is the number of 32-bit words you want per Integer.
template <int N>
class Integer
{
public:
    // Construction and destruction.
    Integer (int i = 0);
    Integer (const Integer& value);
    ~Integer ();

    // Assignment.
    Integer& operator= (const Integer& value);

    // Comparison.
    bool operator== (const Integer& value) const;
    bool operator!= (const Integer& value) const;
    bool operator<  (const Integer& value) const;
    bool operator<= (const Integer& value) const;
    bool operator>  (const Integer& value) const;
    bool operator>= (const Integer& value) const;

    // Arithmetic operations.
    Integer operator- () const;
    Integer operator+ (const Integer& value) const;
    Integer operator- (const Integer& value) const;
    Integer operator* (const Integer& value) const;
    Integer operator/ (const Integer& value) const;
    Integer operator% (const Integer& value) const;

    // Arithmetic updates.
    Integer& operator+= (const Integer& value);
    Integer& operator-= (const Integer& value);
    Integer& operator*= (const Integer& value);
    Integer& operator/= (const Integer& value);

    // Shift operations.
    Integer operator<< (int shift) const;
    Integer operator>> (int shift) const;

    // Shift updates.
    Integer& operator<<= (int shift);
    Integer& operator>>= (int shift);

private:
    // Support for comparisons.  The return value of Compare is
    //   -1 when value0 < value1,
    //    0 when value0 == value1,
    //   +1 when value0 > value1.
    static int Compare (const Integer& value0, const Integer& value1);
    int GetSign () const;

    // Support for division and modulo.
    static bool GetDivMod (const Integer& numer, const Integer& denom,
        Integer& quotient, Integer& remainder);

    static void DivSingle (const Integer& numer, short denom,
        Integer& quotient, Integer& remainder);

    static void DivMultiple (const Integer& numer, const Integer& denom,
        Integer& quotient, Integer& remainder);

    // Miscellaneous utilities.
    int GetLeadingBlock () const;
    int GetTrailingBlock () const;
    int GetLeadingBit (int i) const;  // of mBuffer[i]
    int GetTrailingBit (int i) const;  // of mBuffer[i]
    int GetLeadingBit () const;  // of entire number
    int GetTrailingBit () const;  // of entire number
    void SetBit (int i, bool on);
    bool GetBit (int i) const;
    unsigned int ToUnsignedInt (int i) const;
    void FromUnsignedInt (int i, unsigned int value);
    unsigned int ToUnsignedInt (int lo, int hi) const;
    int ToInt (int i) const;

    enum
    {
        INT_SIZE = 2*N,
        INT_BYTES = INT_SIZE*sizeof(short),
        INT_LAST = INT_SIZE - 1
    };

    short mBuffer[INT_SIZE];

    // Rational needs access to private members of Integer.
    friend class Rational<N>;
};

template <int N>
Integer<N> operator* (int i, const Integer<N>& value);

#include "Wm5Integer.inl"

}

#endif
