// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5NAMES_H
#define WM5NAMES_H

#include "Wm5CoreLIB.h"

//----------------------------------------------------------------------------
#define WM5_DECLARE_NAMES \
public: \
    virtual Object* GetObjectByName (const std::string& name); \
    virtual void GetAllObjectsByName (const std::string& name, \
        std::vector<Object*>& objects)
//----------------------------------------------------------------------------
#define WM5_IMPLEMENT_DEFAULT_NAMES(baseclassname, classname) \
Object* classname::GetObjectByName (const std::string& name) \
{ \
    return baseclassname::GetObjectByName(name); \
} \
\
void classname::GetAllObjectsByName (const std::string& name, \
    std::vector<Object*>& objects) \
{ \
    baseclassname::GetAllObjectsByName(name, objects); \
}
//----------------------------------------------------------------------------
#define WM5_GET_OBJECT_BY_NAME(source, name, found) \
if (source) \
{ \
    found = source->GetObjectByName(name); \
    if (found) \
    { \
        return found; \
    } \
}
//----------------------------------------------------------------------------
#define WM5_GET_ALL_OBJECTS_BY_NAME(source, name, objects) \
if (source) \
{ \
    source->GetAllObjectsByName(name, objects); \
}
//----------------------------------------------------------------------------

#endif
