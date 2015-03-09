// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/07/01)

//----------------------------------------------------------------------------
template <typename T>
bool InStream::Read (T& datum)
{
    return mSource.Read(sizeof(T), &datum);
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadVV (int numElements, T* data)
{
    if (numElements > 0)
    {
        return mSource.Read(sizeof(T), numElements, data);
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadVR (int numElements, T*& data)
{
    if (numElements > 0)
    {
        data = new1<T>(numElements);
        return mSource.Read(sizeof(T), numElements, data);
    }

    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadRR (int& numElements, T*& data)
{
    if (!mSource.Read(sizeof(int), &numElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    if (numElements > 0)
    {
        data = new1<T>(numElements);
        return mSource.Read(sizeof(T), numElements, data);
    }

    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadEnum (T& datum)
{
    int value;
    if (!mSource.Read(sizeof(int), &value))
    {
        return false;
    }
    datum = (T)value;
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadEnumVV (int numElements, T* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadEnum(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadEnumVR (int numElements, T*& data)
{
    if (numElements > 0)
    {
        data = new1<T>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadEnum(data[i]))
            {
                return false;
            }
        }
    }
    else
    {
        data = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadEnumRR (int& numElements, T*& data)
{
    if (!mSource.Read(sizeof(int), &numElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    if (numElements > 0)
    {
        data = new1<T>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadEnum(data[i]))
            {
                return false;
            }
        }
    }
    else
    {
        data = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointer (T*& object)
{
    unsigned int uniqueID;
    mSource.Read(sizeof(unsigned int), &uniqueID);
    object = reinterpret_cast<T*>(uniqueID);
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointerVV (int numElements, T** objects)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadPointer(objects[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointerVR (int numElements, T**& objects)
{
    if (numElements > 0)
    {
        objects = new1<T*>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadPointer(objects[i]))
            {
                return false;
            }
        }
    }
    else
    {
        objects = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointerRR (int& numElements, T**& objects)
{
    if (!mSource.Read(sizeof(int), &numElements))
    {
        numElements = 0;
        objects = 0;
        return false;
    }

    if (numElements > 0)
    {
        objects = new1<T*>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadPointer(objects[i]))
            {
                return false;
            }
        }
    }
    else
    {
        objects = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointer (Pointer0<T>& object)
{
    unsigned int uniqueID;
    mSource.Read(sizeof(unsigned int), &uniqueID);
    unsigned int* data = reinterpret_cast<unsigned int*>(&object);
    *data = uniqueID;
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointerVV (int numElements, Pointer0<T>* objects)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadPointer(objects[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointerVR (int numElements, Pointer0<T>*& objects)
{
    if (numElements > 0)
    {
        objects = new1<Pointer0<T> >(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadPointer(objects[i]))
            {
                return false;
            }
        }
    }
    else
    {
        objects = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool InStream::ReadPointerRR (int& numElements, Pointer0<T>*& objects)
{
    if (!mSource.Read(sizeof(int), &numElements))
    {
        numElements = 0;
        objects = 0;
        return false;
    }

    if (numElements > 0)
    {
        objects = new1<Pointer0<T> >(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadPointer(objects[i]))
            {
                return false;
            }
        }
    }
    else
    {
        objects = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename T>
void InStream::ResolveLink (T*& object)
{
    if (object)
    {
        uint64_t uniqueID = reinterpret_cast<uint64_t>((void*)object);
        LinkMap::iterator iter = mLinked.find(uniqueID);
        if (iter != mLinked.end())
        {
            object = (T*)iter->second;
        }
        else
        {
            assertion(false, "Unexpected link failure.\n");
            object = 0;
        }
    }
}
//----------------------------------------------------------------------------
template <typename T>
void InStream::ResolveLink (int numElements, T** objects)
{
    for (int i = 0; i < numElements; ++i)
    {
        ResolveLink(objects[i]);
    }
}
//----------------------------------------------------------------------------
template <class T>
void InStream::ResolveLink (Pointer0<T>& object)
{
    if (object)
    {
        uint64_t uniqueID = reinterpret_cast<uint64_t>((void*)object);
        LinkMap::iterator iter = mLinked.find(uniqueID);
        *(T**)&object = 0;
        if (iter != mLinked.end())
        {
            object = (T*)iter->second;
        }
        else
        {
            assertion(false, "Unexpected link failure.\n");
            object = 0;
        }
    }
}
//----------------------------------------------------------------------------
template <typename T>
void InStream::ResolveLink (int numElements, Pointer0<T>* objects)
{
    for (int i = 0; i < numElements; ++i)
    {
        ResolveLink(objects[i]);
    }
}
//----------------------------------------------------------------------------
