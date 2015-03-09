// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5RTTI_H
#define WM5RTTI_H

#include "Wm5CoreLIB.h"

namespace Wm5
{

class WM5_CORE_ITEM Rtti
{
public:
    // Construction and destruction.  The name must be unique among all
    // objects in the system.  In the Wm5 namespace, a class Foo should use
    // "Wm5.Foo".  If an application has another namespace, SomeName, then
    // the name should be "SomeName.Foo".
    Rtti (const char* name, const Rtti* baseType);
    ~Rtti ();

    inline const char* GetName () const;
    inline bool IsExactly (const Rtti& type) const;
    bool IsDerived (const Rtti& type) const;

private:
    const char* mName;
    const Rtti* mBaseType;
};

#include "Wm5Rtti.inl"

}

//----------------------------------------------------------------------------
#define WM5_DECLARE_RTTI \
public: \
    static const Rtti TYPE; \
    \
    virtual const Rtti& GetRttiType () const \
    { \
        return TYPE; \
    }
//----------------------------------------------------------------------------
#define WM5_IMPLEMENT_RTTI(nsname, baseclassname, classname) \
    const Rtti classname::TYPE(#nsname"."#classname, &baseclassname::TYPE)
//----------------------------------------------------------------------------

#endif
