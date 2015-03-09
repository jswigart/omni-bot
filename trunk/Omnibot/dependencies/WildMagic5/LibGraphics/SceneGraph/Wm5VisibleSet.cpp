// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VisibleSet.h"
using namespace Wm5;

//----------------------------------------------------------------------------
VisibleSet::VisibleSet ()
    :
    mNumVisible(0)
{
}
//----------------------------------------------------------------------------
VisibleSet::~VisibleSet ()
{
}
//----------------------------------------------------------------------------
void VisibleSet::Insert (Spatial* visible)
{
    int size = (int)mVisible.size();
    if (mNumVisible < size)
    {
        mVisible[mNumVisible] = visible;
    }
    else
    {
        mVisible.push_back(visible);
    }
    ++mNumVisible;
}
//----------------------------------------------------------------------------
