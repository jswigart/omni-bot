// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5Rtti.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Rtti::Rtti (const char* name, const Rtti* baseType)
{
    mName = name;
    mBaseType = baseType;
}
//----------------------------------------------------------------------------
Rtti::~Rtti ()
{
}
//----------------------------------------------------------------------------
bool Rtti::IsDerived (const Rtti& type) const
{
    const Rtti* search = this;
    while (search)
    {
        if (search == &type)
        {
            return true;
        }
        search = search->mBaseType;
    }
    return false;
}
//----------------------------------------------------------------------------
