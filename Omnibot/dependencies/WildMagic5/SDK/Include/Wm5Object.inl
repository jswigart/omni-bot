// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
// Static and dynamic typecasting.
//----------------------------------------------------------------------------
template <class T>
T* StaticCast (Object* object)
{
    return (T*)object;
}
//----------------------------------------------------------------------------
template <class T>
const T* StaticCast (const Object* object)
{
    return (const T*)object;
}
//----------------------------------------------------------------------------
template <class T>
T* DynamicCast (Object* object)
{
    return object && object->IsDerived(T::TYPE) ? (T*)object : 0;
}
//----------------------------------------------------------------------------
template <class T>
const T* DynamicCast (const Object* object)
{
    return object && object->IsDerived(T::TYPE) ? (const T*)object : 0;
}
//----------------------------------------------------------------------------
