// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ELEMENT_H
#define WM5ELEMENT_H

#include "Wm5ImagicsLIB.h"

// Wrappers for native types.
#define WM5_DECL_ELEMENT(T) \
class WM5_IMAGICS_ITEM E##T \
{ \
public: \
    E##T (T value = 0) { mValue = value; } \
    \
    E##T& operator= (E##T element) \
    { \
        mValue = element.mValue; \
        return *this; \
    } \
    \
    operator T () { return mValue; } \
    \
    static int GetRTTI () { return msRTTI; } \
    \
protected: \
    T mValue; \
    static const int msRTTI; \
}

#define WM5_IMPL_ELEMENT(T,rtti) \
const int E##T::msRTTI = rtti

namespace Wm5
{

const int gNumImageElements = 12;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short rgb5;
typedef unsigned int rgb8;

WM5_DECL_ELEMENT(char);
WM5_DECL_ELEMENT(uchar);
WM5_DECL_ELEMENT(short);
WM5_DECL_ELEMENT(ushort);
WM5_DECL_ELEMENT(int);
WM5_DECL_ELEMENT(uint);
WM5_DECL_ELEMENT(long);
WM5_DECL_ELEMENT(ulong);
WM5_DECL_ELEMENT(float);
WM5_DECL_ELEMENT(double);
WM5_DECL_ELEMENT(rgb5);
WM5_DECL_ELEMENT(rgb8);

inline unsigned short GetColor16 (unsigned char r, unsigned char g,
    unsigned char b);
inline unsigned char GetRed16 (unsigned short color);
inline unsigned char GetGreen16 (unsigned short color);
inline unsigned char GetBlue16 (unsigned short color);

inline unsigned int GetColor24 (unsigned char r, unsigned char g,
    unsigned char b);
inline unsigned char GetRed24 (unsigned int color);
inline unsigned char GetGreen24 (unsigned int color);
inline unsigned char GetBlue24 (unsigned int color);

#include "Wm5Element.inl"

}

#endif
