// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5OutStream.h"
#include "Wm5FileIO.h"
#include "Wm5Object.h"
using namespace Wm5;

//----------------------------------------------------------------------------
OutStream::OutStream ()
{
}
//----------------------------------------------------------------------------
OutStream::~OutStream ()
{
}
//----------------------------------------------------------------------------
bool OutStream::Insert (Object* object)
{
    if (object)
    {
        // An object can be inserted only once.
        std::vector<Object*>::iterator iter = mTopLevel.begin();
        std::vector<Object*>::iterator end = mTopLevel.end();
        for (/**/; iter != end; ++iter)
        {
            if (object == *iter)
            {
                return false;
            }
        }

        if (object)
        {
            mTopLevel.push_back(object);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool OutStream::IsTopLevel (const Object* object) const
{
    std::vector<Object*>::const_iterator iter = mTopLevel.begin();
    std::vector<Object*>::const_iterator end = mTopLevel.end();
    for (/**/; iter != end; ++iter)
    {
        if (object == *iter)
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void OutStream::Save (int& bufferSize, char*& buffer, int mode)
{
    // Insert a pairing between the null pointer and zero index.
    const Object* object = 0;
    unsigned int uniqueID = 0;
    mRegistered.insert(std::make_pair(object, uniqueID));
    mOrdered.push_back(object);

    // Create the set of unique objects in the object graph.  Get the number
    // of bytes required for streaming so that we can create a buffer of the
    // exact size for writing.
    std::vector<Object*>::iterator iterT = mTopLevel.begin();
    std::vector<Object*>::iterator endT = mTopLevel.end();
    for (/**/; iterT != endT; ++iterT)
    {
        object = *iterT;
        object->Register(*this);
    }

    std::vector<const Object*>::iterator iterO = mOrdered.begin();
    std::vector<const Object*>::iterator endO = mOrdered.end();
    bufferSize = 0;
    for (++iterO; iterO != endO; ++iterO)
    {
        object = *iterO;
        bufferSize += object->GetStreamingSize();
    }

    // Adjust the buffer size to account for the "Top Level" strings.
    std::string topLevel("Top Level");
    int numTopLevelBytes = GetStreamingSize(topLevel);
    bufferSize += numTopLevelBytes*(int)mTopLevel.size();

    // Create the buffer into which the objects are written.
    buffer = new1<char>(bufferSize);
    mTarget.Open(bufferSize, buffer, mode);

    // Save the objects to the target buffer.
    iterO = mOrdered.begin();
    endO = mOrdered.end();
    for (++iterO, uniqueID = 1; iterO != endO; ++iterO, ++uniqueID)
    {
        object = *iterO;
        if (IsTopLevel(object))
        {
            WriteString(topLevel);
        }
        object->Save(*this);
    }

    // Empty the containers in preparation for another save.
    mRegistered.clear();
    mOrdered.clear();
    mTarget.Close();
}
//----------------------------------------------------------------------------
bool OutStream::Save (const std::string& name, std::string version, int mode)
{
    // Save the file version to disk.
    if (!FileIO::Save(name, true, (int)version.length(), version.c_str()))
    {
        // The file could not be opened.
        return false;
    }

    // Write the scene graph into the buffer.
    int bufferSize;
    char* buffer;
    Save(bufferSize, buffer, mode);

    // Save the scene graph from memory to disk.
    if (!FileIO::Append(name, true, bufferSize, buffer))
    {
        // The file could not be appended.
        delete1(buffer);
        return false;
    }

    delete1(buffer);
    return true;
}
//----------------------------------------------------------------------------
bool OutStream::WriteBool (const bool datum)
{
    unsigned int value = (datum ? 0xFFFFFFFFu : 0u);
    return mTarget.Write(sizeof(unsigned int), &value);
}
//----------------------------------------------------------------------------
bool OutStream::WriteBoolW (int numElements, const bool* data)
{
    if (!mTarget.Write(sizeof(int), &numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteBool(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool OutStream::WriteBoolN (int numElements, const bool* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteBool(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool OutStream::WriteString (const std::string& datum)
{
    int length = (int)datum.length();
    if (!mTarget.Write(sizeof(int), &length))
    {
        return false;
    }

    if (length > 0)
    {
        if (!mTarget.Write(sizeof(char), length, datum.c_str()))
        {
            return false;
        }

        // Strings are written in multiples of four bytes.
        char zero[4] = { 0, 0, 0, 0 };
        int padding = (length % 4);
        if (padding > 0)
        {
            padding = 4 - padding;
            if (!mTarget.Write(sizeof(char), padding, zero))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool OutStream::WriteStringW (int numElements, const std::string* data)
{
    if (!mTarget.Write(sizeof(int), &numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteString(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool OutStream::WriteStringN (int numElements, const std::string* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteString(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
int OutStream::GetBytesWritten () const
{
     return mTarget.GetNumBytesProcessed();
}
//----------------------------------------------------------------------------
bool OutStream::RegisterRoot (const Object* object)
{
    if (mRegistered.find(object) == mRegistered.end())
    {
        // This is the first time the object has been encountered in the
        // graph of objects.  We need to write a unique identifier to disk
        // so that the loader-linker system works properly.  The easiest thing
        // to write is the object address, so we could save the address in a
        // set ('registered') for use in Object::Save.  There are two problems
        // with this:
        //
        // 1. The addresses change between application runs.  Two runs that
        //    save the same scene graph will lead to different object
        //    orderings because the order of address in 'registered' has
        //    changed.  A solution is to store the addresses in an array
        //    ('ordered') and save the objects in the order stored in the
        //    array.
        //
        // 2. Even with the guaranteed object order, two runs that save the
        //    same scene graph will lead to files that do not match
        //    byte-by-byte.  The addresses themselves change.  A solution is
        //    to assign a 'virtual address' to each object as it is
        //    registered.  The simplest is to use the index of the object
        //    within 'ordered'.  To avoid confusion between the zero address
        //    and the zero index, we use indices that are the array index
        //    plus one.

        unsigned int uniqueID = (unsigned int)mOrdered.size();
        mRegistered.insert(std::make_pair(object, uniqueID));
        mOrdered.push_back(object);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void OutStream::WriteUniqueID (const Object* object)
{
    unsigned int uniqueID = mRegistered.find(object)->second;
    mTarget.Write(sizeof(unsigned int), &uniqueID);
}
//----------------------------------------------------------------------------
