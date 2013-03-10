// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/07/01)

#include "Wm5CorePCH.h"
#include "Wm5InStream.h"
#include "Wm5FileIO.h"
#include "Wm5Object.h"
using namespace Wm5;

//----------------------------------------------------------------------------
InStream::InStream ()
{
}
//----------------------------------------------------------------------------
InStream::~InStream ()
{
}
//----------------------------------------------------------------------------
int InStream::GetNumObjects () const
{
    return (int)mTopLevel.size();
}
//----------------------------------------------------------------------------
Object* InStream::GetObjectAt (int i) const
{
    if (0 <= i && i < (int)mTopLevel.size())
    {
        return mTopLevel[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
void InStream::Load (int bufferSize, char* buffer, int mode)
{
    // Create a reader for the incoming buffer.
    mSource.Open(bufferSize, buffer, mode);

    // Load the set of unique objects.
    std::string topLevel("Top Level");
    Object* object;
    while (mSource.GetNumBytesProcessed() < mSource.GetNumBytesTotal())
    {
        // Read the "Top Level" name or RTTI name.
        std::string name;
        ReadString(name);
        bool isTopLevel = (name == topLevel);
        if (isTopLevel)
        {
            // Read the RTTI name.
            ReadString(name);
        }

        // Get the factory function for the type of object about to be read.
        // If the assert in Object::Find(name) fails, make sure you have
        // called InitTerm::ExecuteInitializers() in your 'main' or 'WinMain'
        // before you make calls to the engine code.  The initialization
        // allocates the factories map and populates it with the factory
        // functions.
        Object::FactoryFunction factory = Object::Find(name);
        if (!factory)
        {
            // Cannot find the factory function.  Make sure you have added
            // WM5_REGISTER_STREAM(someclass) to the header file for each
            // 'someclass'.  This macro registers the factory function for
            // the class.
            assertion(false, "Cannot find factory for %s.\n", name.c_str());
            return;
        }

        // Load the object from the source buffer.
        object = (*factory)(*this);

        // Keep track of all top level objects for use by the application.
        if (isTopLevel)
        {
            mTopLevel.push_back(object);
        }
    }

    // Link the objects.  This process replaces the old addresses stored in
    // any Object* data members with the new addresses of objects created
    // during the current application run.
    std::vector<Object*>::iterator iter = mOrdered.begin();
    std::vector<Object*>::iterator end = mOrdered.end();
    for (/**/; iter != end; ++iter)
    {
        (*iter)->Link(*this);
    }

    // Allow the objects to perform post-link semantics.  In the paradigm of
    // a loader-linker, the default constructor is used for creating an object
    // into which data is loaded.  The linker connects up the objects.  The
    // post-link function can do some of the work that the nondefault
    // constructors do when creating objects during the application runtime.
    iter = mOrdered.begin();
    end = mOrdered.end();
    for (/**/; iter != end; ++iter)
    {
        (*iter)->PostLink();
    }

    // Empty the containers in preparation for another save.
    mLinked.clear();
    mOrdered.clear();
    mSource.Close();
}
//----------------------------------------------------------------------------
bool InStream::Load (const std::string& name, std::string version, int mode)
{
    // Load the stream from disk to memory.
    int bufferSize;
    char* buffer;
    if (!FileIO::Load(name, true, bufferSize, buffer))
    {
        return false;
    }

    // Get the file version.
    int length = (int)version.length();
    if (bufferSize < length)
    {
        // The file does not exist or is not large enough to store the
        // version string.
        delete1(buffer);
        return false;
    }

    // Compare to the requested file version.
    if (strncmp(version.c_str(), buffer, length) != 0)
    {
        // The version strings do not match.
        delete1(buffer);
        return false;
    }

    // Reconstruct the scene graph from the buffer.
    bufferSize -= length;
    Load(bufferSize, buffer + length, mode);

    delete1(buffer);
    return true;
}
//----------------------------------------------------------------------------
bool InStream::ReadBool (bool& datum)
{
    unsigned int value;
    if (!mSource.Read(sizeof(unsigned int), &value))
    {
        return false;
    }
    datum = (value != 0);
    return true;
}
//----------------------------------------------------------------------------
bool InStream::ReadBoolVV (int numElements, bool* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadBool(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool InStream::ReadBoolVR (int numElements, bool*& data)
{
    if (numElements > 0)
    {
        data = new1<bool>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadBool(data[i]))
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
bool InStream::ReadBoolRR (int& numElements, bool*& data)
{
    if (!mSource.Read(sizeof(int), &numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        data = new1<bool>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadBool(data[i]))
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
bool InStream::ReadString (std::string& datum)
{
    int length;
    mSource.Read(sizeof(int), &length);
    if (length <= 0)
    {
        datum.clear();
        return false;
    }

    // Strings are written in multiples of four bytes.
    int padding = (length % 4);
    if (padding > 0)
    {
        padding = 4 - padding;
    }

    const char* text = mSource.GetBuffer() + mSource.GetNumBytesProcessed();
    datum.assign(text, length);
    mSource.IncrementNumBytesProcessed(length + padding);
    return true;
}
//----------------------------------------------------------------------------
bool InStream::ReadStringVV (int numElements, std::string* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadString(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool InStream::ReadStringVR (int numElements, std::string*& data)
{
    if (numElements > 0)
    {
        data = new1<std::string>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadString(data[i]))
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
bool InStream::ReadStringRR (int& numElements, std::string*& data)
{
    if (!mSource.Read(sizeof(int), &numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        data = new1<std::string>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadString(data[i]))
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
int InStream::GetBytesRead () const
{
    return mSource.GetNumBytesProcessed();
}
//----------------------------------------------------------------------------
void InStream::ReadUniqueID (Object* object)
{
    unsigned int uniqueID;
    if (mSource.Read(sizeof(unsigned int), &uniqueID))
    {
        mLinked.insert(std::make_pair((uint64_t)uniqueID, object));
        mOrdered.push_back(object);
    }
}
//----------------------------------------------------------------------------
