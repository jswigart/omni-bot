// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5InitTerm.h"
#include "Wm5Assert.h"
using namespace Wm5;

int InitTerm::msNumInitializers = 0;
InitTerm::Initializer InitTerm::msInitializers[MAX_ELEMENTS];
int InitTerm::msNumTerminators = 0;
InitTerm::Terminator InitTerm::msTerminators[MAX_ELEMENTS];

//----------------------------------------------------------------------------
void InitTerm::AddInitializer (Initializer function)
{
    if (msNumInitializers < MAX_ELEMENTS)
    {
        msInitializers[msNumInitializers++] = function;
    }
    else
    {
        assertion(false, "Increase MAX_ELEMENTS and recompile LibCore\n");
    }
}
//----------------------------------------------------------------------------
void InitTerm::ExecuteInitializers ()
{
    for (int i = 0; i < msNumInitializers; ++i)
    {
        msInitializers[i]();
    }
}
//----------------------------------------------------------------------------
void InitTerm::AddTerminator (Terminator function)
{
    if (msNumTerminators < MAX_ELEMENTS)
    {
        msTerminators[msNumTerminators++] = function;
    }
    else
    {
        assertion(false, "Increase MAX_ELEMENTS and recompile LibCore\n");
    }
}
//----------------------------------------------------------------------------
void InitTerm::ExecuteTerminators ()
{
    for (int i = 0; i < msNumTerminators; ++i)
    {
        msTerminators[i]();
    }
}
//----------------------------------------------------------------------------
