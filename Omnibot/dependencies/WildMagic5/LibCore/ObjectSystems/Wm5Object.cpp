// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5Object.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Object::Object ()
{
}
//----------------------------------------------------------------------------
Object::~Object ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Run-time type information.
//----------------------------------------------------------------------------
const Rtti Object::TYPE("Wm5.Object", 0);
//----------------------------------------------------------------------------
const Rtti& Object::GetRttiType () const
{
    return TYPE;
}
//----------------------------------------------------------------------------
bool Object::IsExactly (const Rtti& type) const
{
    return GetRttiType().IsExactly(type);
}
//----------------------------------------------------------------------------
bool Object::IsDerived (const Rtti& type) const
{
    return GetRttiType().IsDerived(type);
}
//----------------------------------------------------------------------------
bool Object::IsExactlyTypeOf (const Object* object) const
{
    return object && GetRttiType().IsExactly(object->GetRttiType());
}
//----------------------------------------------------------------------------
bool Object::IsDerivedTypeOf (const Object* object) const
{
    return object && GetRttiType().IsDerived(object->GetRttiType());
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Names.
//----------------------------------------------------------------------------
void Object::SetName (const std::string& name)
{
    mName = name;
}
//----------------------------------------------------------------------------
const std::string& Object::GetName () const
{
    return mName;
}
//----------------------------------------------------------------------------
Object* Object::GetObjectByName (const std::string& name)
{
    return (name == mName ? this : 0);
}
//----------------------------------------------------------------------------
void Object::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    if (name == mName)
    {
        objects.push_back(this);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming.
//----------------------------------------------------------------------------
Object::FactoryMap* Object::msFactories = 0;
bool Object::msStreamRegistered = false;
//----------------------------------------------------------------------------
Object* Object::Factory (InStream&)
{
    assertion(false, "Abstract classes have no factory.\n");
    return 0;
}
//----------------------------------------------------------------------------
bool Object::RegisterFactory ()
{
    if (!msStreamRegistered)
    {
        InitTerm::AddInitializer(Object::InitializeFactory);
        InitTerm::AddTerminator(Object::TerminateFactory);
        msStreamRegistered = true;
    }
    return msStreamRegistered;
}
//----------------------------------------------------------------------------
void Object::InitializeFactory ()
{
    if (!msFactories)
    {
        msFactories = new0 Object::FactoryMap();
    }
    msFactories->insert(std::make_pair(TYPE.GetName(), Factory));
}
//----------------------------------------------------------------------------
void Object::TerminateFactory ()
{
    delete0(msFactories);
    msFactories = 0;
}
//----------------------------------------------------------------------------
Object::FactoryFunction Object::Find (const std::string& name)
{
    assertion(msFactories != 0, "FactoryMap was not yet allocated.\n");
    FactoryMap::iterator iter = msFactories->find(name);
    if (iter != msFactories->end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
Object::Object (LoadConstructor)
{
}
//----------------------------------------------------------------------------
void Object::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    // The RTTI name was already read from the stream in order to look up the
    // correct Load function for the object.

    // Read the unique identifier of the object.  This provides information
    // for the linking phase.
    source.ReadUniqueID(this);

    // Read the name of the object.
    source.ReadString(mName);

    WM5_END_DEBUG_STREAM_LOAD(Object, source);
}
//----------------------------------------------------------------------------
void Object::Link (InStream&)
{
    // Object has no Object* members.
}
//----------------------------------------------------------------------------
void Object::PostLink ()
{
    // Object has no postlink semantics.
}
//----------------------------------------------------------------------------
bool Object::Register (OutStream& target) const
{
    return target.RegisterRoot(this);
}
//----------------------------------------------------------------------------
void Object::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    // Write the RTTI name for factory function lookup during Load.
    target.WriteString(GetRttiType().GetName());

    // Write the unique identifier for the object.  This is used during
    // loading and linking.
    target.WriteUniqueID(this);

    // Write the name of the object.
    target.WriteString(mName);

    WM5_END_DEBUG_STREAM_SAVE(Object, target);
}
//----------------------------------------------------------------------------
int Object::GetStreamingSize () const
{
    // The RTTI name.
    int size = WM5_STRINGSIZE(GetRttiType().GetName());

    // The unique identifier for the object.
    size += sizeof(unsigned int);

    // The name of the object.
    size += WM5_STRINGSIZE(mName);

    return size;
}
//----------------------------------------------------------------------------
#if 0
Object* Object::Copy (const std::string& uniqueNameAppend) const
{
    // Save the object to a memory buffer.
    Stream<Object> saveStream;
    saveStream.Insert((Object*)this);
    int bufferSize = 0;
    char* buffer = 0;
    bool saved = saveStream.Save(bufferSize, buffer, BufferIO::BM_WRITE);
    assertion(saved, "Copy function failed on save\n");
    if (!saved)
    {
        return 0;
    }

    // Load the object from the memory buffer.
    Stream<Object> loadStream;
    bool loaded = loadStream.Load(bufferSize, buffer, BufferIO::BM_READ);
    assertion(loaded, "Copy function failed on load\n");
    if (!loaded)
    {
        return 0;
    }
    delete1(buffer);

    if (uniqueNameAppend != "")
    {
        // The names of the input scene were copied as is.  Generate unique
        // names for the output scene.
        const std::vector<Object*>& ordered = loadStream.GetLoadedObjects();
        std::vector<Object*>::const_iterator iter = ordered.begin();
        std::vector<Object*>::const_iterator end = ordered.end();
        for (/**/; iter != end; ++iter)
        {
            Object* object = *iter;
            std::string name = object->GetName();
            if (name.length() > 0)
            {
                // The object has a name.  Append a string to make the name
                // unique.  TODO:  This code does not ensure that the
                // appended name is some other name in the copied scene.  To
                // do this would require building a set of names and verifying
                // that the appended names are not in this set.  For now we
                // think this is not worth the effort, but maybe later we can
                // add code to do this.
                name += uniqueNameAppend;
                object->SetName(name);
            }
        }
    }

    return loadStream.GetObjectAt(0);
}
#endif
//----------------------------------------------------------------------------
