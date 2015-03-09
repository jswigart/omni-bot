// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CRMCuller.h"
using namespace Wm5;

//----------------------------------------------------------------------------
CRMCuller::CRMCuller (const Camera* camera)
    :
    Culler(camera)
{
}
//----------------------------------------------------------------------------
CRMCuller::~CRMCuller ()
{
}
//----------------------------------------------------------------------------
void CRMCuller::Insert (Spatial* visible)
{
    if (mUnique.find(visible) == mUnique.end())
    {
        // The object was not in the set of unique objects, so insert it.
        Culler::Insert(visible);
    }
}
//----------------------------------------------------------------------------
