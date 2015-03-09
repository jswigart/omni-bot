// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SMARTPOINTER_H
#define WM5SMARTPOINTER_H

#include "Wm5CoreLIB.h"
#include "Wm5Memory.h"

// The PointerD<T> supports sharing of objects of type T dynamically allocated
// using the Wild Magic memory manager 'newD' operator, where D is dimension
// (0, 1, 2, etc).  T should be an object type, not a pointer type.  For
// dimension 0, the T-object can also be allocated using the 'new' operator.
// For dimension 1, the T-object-array can also be allocated using the 'new[]'
// operator.  For dimensions D >= 2, you must use 'newD', whether those from
// the Wild Magic memory manager or those implemented to use 'new[]'; see the
// Memory class for such implementations.  The class does not support sharing
// of stack-based objects; that is, do not pass the address of T-objects on
// the stack.

namespace Wm5
{

//----------------------------------------------------------------------------
class PointerBase
{
protected:
    // The map key (void*) is the address of the shared object.  The map
    // value (int) is the number of references.
    typedef std::map<void*,int> ReferenceMap;
    typedef ReferenceMap::iterator RMIterator;
    typedef ReferenceMap::const_iterator RMCIterator;

    WM5_CORE_ITEM static ReferenceMap msMap;
    WM5_CORE_ITEM static Mutex msMutex;
};

//----------------------------------------------------------------------------
template <typename T>
class Pointer0 : public PointerBase
{
public:
    // Construction and destruction.
    Pointer0 (T* data = 0);
    Pointer0 (const Pointer0& pointer);
    ~Pointer0 ();

    // Implicit conversions.
    inline operator T* () const;
    inline T& operator* () const;
    inline T* operator-> () const;

    // Assignment.
    Pointer0& operator= (T* data);
    Pointer0& operator= (const Pointer0& pointer);

    // Comparisons.
    inline bool operator== (T* data) const;
    inline bool operator!= (T* data) const;
    inline bool operator== (const Pointer0& pointer) const;
    inline bool operator!= (const Pointer0& pointer) const;

protected:
    T* mData;
};

//----------------------------------------------------------------------------
template <typename T>
class Pointer1 : public PointerBase
{
public:
    // Construction and destruction.
    Pointer1 (T* data = 0);
    Pointer1 (const Pointer1& pointer);
    ~Pointer1 ();

    // Implicit conversions.
    inline operator T* () const;
    inline T& operator* () const;
    inline T* operator-> () const;

    // Assignment.
    Pointer1& operator= (T* data);
    Pointer1& operator= (const Pointer1& pointer);

    // Comparisons.
    inline bool operator== (T* data) const;
    inline bool operator!= (T* data) const;
    inline bool operator== (const Pointer1& pointer) const;
    inline bool operator!= (const Pointer1& pointer) const;

protected:
    T* mData;
};

//----------------------------------------------------------------------------
template <typename T>
class Pointer2 : public PointerBase
{
public:
    // Construction and destruction.
    Pointer2 (T** data = 0);
    Pointer2 (const Pointer2& pointer);
    ~Pointer2 ();

    // Implicit conversions.
    inline operator T** () const;
    inline T*& operator* () const;

    // Assignment.
    Pointer2& operator= (T** data);
    Pointer2& operator= (const Pointer2& pointer);

    // Comparisons.
    inline bool operator== (T** data) const;
    inline bool operator!= (T** data) const;
    inline bool operator== (const Pointer2& pointer) const;
    inline bool operator!= (const Pointer2& pointer) const;

protected:
    T** mData;
};

//----------------------------------------------------------------------------
template <typename T>
class Pointer3 : public PointerBase
{
public:
    // Construction and destruction.
    Pointer3 (T*** data = 0);
    Pointer3 (const Pointer3& pointer);
    ~Pointer3 ();

    // Implicit conversions.
    inline operator T*** () const;
    inline T**& operator* () const;

    // Assignment.
    Pointer3& operator= (T*** data);
    Pointer3& operator= (const Pointer3& pointer);

    // Comparisons.
    inline bool operator== (T*** data) const;
    inline bool operator!= (T*** data) const;
    inline bool operator== (const Pointer3& pointer) const;
    inline bool operator!= (const Pointer3& pointer) const;

protected:
    T*** mData;
};

//----------------------------------------------------------------------------
template <typename T>
class Pointer4 : public PointerBase
{
public:
    // Construction and destruction.
    Pointer4 (T**** data = 0);
    Pointer4 (const Pointer4& pointer);
    ~Pointer4 ();

    // Implicit conversions.
    inline operator T**** () const;
    inline T***& operator* () const;

    // Assignment.
    Pointer4& operator= (T**** data);
    Pointer4& operator= (const Pointer4& pointer);

    // Comparisons.
    inline bool operator== (T**** data) const;
    inline bool operator!= (T**** data) const;
    inline bool operator== (const Pointer4& pointer) const;
    inline bool operator!= (const Pointer4& pointer) const;

protected:
    T**** mData;
};

//----------------------------------------------------------------------------

#include "Wm5SmartPointer.inl"

}

#endif
