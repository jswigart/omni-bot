// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
// Pointer0
//----------------------------------------------------------------------------
template <typename T>
Pointer0<T>::Pointer0 (T* data)
{
    msMutex.Enter();

    mData = data;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            msMap[mData] = 1;
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer0<T>::Pointer0 (const Pointer0& pointer)
{
    msMutex.Enter();

    mData = pointer.mData;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            assertion(false, "pointer.mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer0<T>::~Pointer0 ()
{
    msMutex.Enter();

    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            if (--iter->second == 0)
            {
                msMap.erase(mData);
                delete0(mData);
            }
        }
        else
        {
            assertion(false, "mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
inline Pointer0<T>::operator T* () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline T& Pointer0<T>::operator* () const
{
    return *mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline T* Pointer0<T>::operator-> () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer0<T>& Pointer0<T>::operator= (T* data)
{
    msMutex.Enter();

    if (mData != data)
    {
        RMIterator iter;

        if (data)
        {
            iter = msMap.find(data);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                msMap[data] = 1;
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete0(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = data;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer0<T>& Pointer0<T>::operator= (const Pointer0& pointer)
{
    msMutex.Enter();

    if (mData != pointer.mData)
    {
        RMIterator iter;

        if (pointer.mData)
        {
            iter = msMap.find(pointer.mData);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                assertion(false, "pointer.mData must be in the map\n");
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete0(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = pointer.mData;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer0<T>::operator== (T* data) const
{
    return mData == data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer0<T>::operator!= (T* data) const
{
    return mData != data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer0<T>::operator== (const Pointer0& pointer) const
{
    return mData == pointer.mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer0<T>::operator!= (const Pointer0& pointer) const
{
    return mData != pointer.mData;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Pointer1
//----------------------------------------------------------------------------
template <typename T>
Pointer1<T>::Pointer1 (T* data)
{
    msMutex.Enter();

    mData = data;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            msMap[mData] = 1;
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer1<T>::Pointer1 (const Pointer1& pointer)
{
    msMutex.Enter();

    mData = pointer.mData;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            assertion(false, "pointer.mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer1<T>::~Pointer1 ()
{
    msMutex.Enter();

    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            if (--iter->second == 0)
            {
                msMap.erase(mData);
                delete1(mData);
            }
        }
        else
        {
            assertion(false, "mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
inline Pointer1<T>::operator T* () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline T& Pointer1<T>::operator* () const
{
    return *mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline T* Pointer1<T>::operator-> () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer1<T>& Pointer1<T>::operator= (T* data)
{
    msMutex.Enter();

    if (mData != data)
    {
        RMIterator iter;

        if (data)
        {
            iter = msMap.find(data);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                msMap[data] = 1;
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete1(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = data;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer1<T>& Pointer1<T>::operator= (const Pointer1& pointer)
{
    msMutex.Enter();

    if (mData != pointer.mData)
    {
        RMIterator iter;

        if (pointer.mData)
        {
            iter = msMap.find(pointer.mData);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                assertion(false, "pointer.mData must be in the map\n");
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete1(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = pointer.mData;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer1<T>::operator== (T* data) const
{
    return mData == data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer1<T>::operator!= (T* data) const
{
    return mData != data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer1<T>::operator== (const Pointer1& pointer) const
{
    return mData == pointer.mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer1<T>::operator!= (const Pointer1& pointer) const
{
    return mData != pointer.mData;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Pointer2
//----------------------------------------------------------------------------
template <typename T>
Pointer2<T>::Pointer2 (T** data)
{
    msMutex.Enter();

    mData = data;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            msMap[mData] = 1;
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer2<T>::Pointer2 (const Pointer2& pointer)
{
    msMutex.Enter();

    mData = pointer.mData;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            assertion(false, "pointer.mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer2<T>::~Pointer2 ()
{
    msMutex.Enter();

    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            if (--iter->second == 0)
            {
                msMap.erase(mData);
                delete2(mData);
            }
        }
        else
        {
            assertion(false, "mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
inline Pointer2<T>::operator T** () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline T*& Pointer2<T>::operator* () const
{
    return *mData;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer2<T>& Pointer2<T>::operator= (T** data)
{
    msMutex.Enter();

    if (mData != data)
    {
        RMIterator iter;

        if (data)
        {
            iter = msMap.find(data);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                msMap[data] = 1;
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete2(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = data;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer2<T>& Pointer2<T>::operator= (const Pointer2& pointer)
{
    msMutex.Enter();

    if (mData != pointer.mData)
    {
        RMIterator iter;

        if (pointer.mData)
        {
            iter = msMap.find(pointer.mData);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                assertion(false, "pointer.mData must be in the map\n");
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete2(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = pointer.mData;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer2<T>::operator== (T** data) const
{
    return mData == data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer2<T>::operator!= (T** data) const
{
    return mData != data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer2<T>::operator== (const Pointer2& pointer) const
{
    return mData == pointer.mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer2<T>::operator!= (const Pointer2& pointer) const
{
    return mData != pointer.mData;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Pointer3
//----------------------------------------------------------------------------
template <typename T>
Pointer3<T>::Pointer3 (T*** data)
{
    msMutex.Enter();

    mData = data;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            msMap[mData] = 1;
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer3<T>::Pointer3 (const Pointer3& pointer)
{
    msMutex.Enter();

    mData = pointer.mData;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            assertion(false, "pointer.mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer3<T>::~Pointer3 ()
{
    msMutex.Enter();

    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            if (--iter->second == 0)
            {
                msMap.erase(mData);
                delete3(mData);
            }
        }
        else
        {
            assertion(false, "mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
inline Pointer3<T>::operator T*** () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline T**& Pointer3<T>::operator* () const
{
    return *mData;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer3<T>& Pointer3<T>::operator= (T*** data)
{
    msMutex.Enter();

    if (mData != data)
    {
        RMIterator iter;

        if (data)
        {
            iter = msMap.find(data);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                msMap[data] = 1;
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete3(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = data;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer3<T>& Pointer3<T>::operator= (const Pointer3& pointer)
{
    msMutex.Enter();

    if (mData != pointer.mData)
    {
        RMIterator iter;

        if (pointer.mData)
        {
            iter = msMap.find(pointer.mData);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                assertion(false, "pointer.mData must be in the map\n");
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete3(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = pointer.mData;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer3<T>::operator== (T*** data) const
{
    return mData == data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer3<T>::operator!= (T*** data) const
{
    return mData != data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer3<T>::operator== (const Pointer3& pointer) const
{
    return mData == pointer.mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer3<T>::operator!= (const Pointer3& pointer) const
{
    return mData != pointer.mData;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Pointer4
//----------------------------------------------------------------------------
template <typename T>
Pointer4<T>::Pointer4 (T**** data)
{
    msMutex.Enter();

    mData = data;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            msMap[mData] = 1;
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer4<T>::Pointer4 (const Pointer4& pointer)
{
    msMutex.Enter();

    mData = pointer.mData;
    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            ++iter->second;
        }
        else
        {
            assertion(false, "pointer.mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
Pointer4<T>::~Pointer4 ()
{
    msMutex.Enter();

    if (mData)
    {
        RMIterator iter = msMap.find(mData);
        if (iter != msMap.end())
        {
            if (--iter->second == 0)
            {
                msMap.erase(mData);
                delete4(mData);
            }
        }
        else
        {
            assertion(false, "mData must be in the map\n");
        }
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
template <typename T>
inline Pointer4<T>::operator T**** () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline T***& Pointer4<T>::operator* () const
{
    return *mData;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer4<T>& Pointer4<T>::operator= (T**** data)
{
    msMutex.Enter();

    if (mData != data)
    {
        RMIterator iter;

        if (data)
        {
            iter = msMap.find(data);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                msMap[data] = 1;
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete4(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = data;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
Pointer4<T>& Pointer4<T>::operator= (const Pointer4& pointer)
{
    msMutex.Enter();

    if (mData != pointer.mData)
    {
        RMIterator iter;

        if (pointer.mData)
        {
            iter = msMap.find(pointer.mData);
            if (iter != msMap.end())
            {
                ++iter->second;
            }
            else
            {
                assertion(false, "pointer.mData must be in the map\n");
            }
        }

        if (mData)
        {
            iter = msMap.find(mData);
            if (iter != msMap.end())
            {
                if (--iter->second == 0)
                {
                    msMap.erase(mData);
                    delete4(mData);
                }
            }
            else
            {
                assertion(false, "mData must be in the map\n");
            }
        }

        mData = pointer.mData;
    }

    msMutex.Leave();
    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer4<T>::operator== (T**** data) const
{
    return mData == data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer4<T>::operator!= (T**** data) const
{
    return mData != data;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer4<T>::operator== (const Pointer4& pointer) const
{
    return mData == pointer.mData;
}
//----------------------------------------------------------------------------
template <typename T>
inline bool Pointer4<T>::operator!= (const Pointer4& pointer) const
{
    return mData != pointer.mData;
}
//----------------------------------------------------------------------------
