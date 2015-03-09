// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5GlobalEffect.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, GlobalEffect);
WM5_IMPLEMENT_STREAM(GlobalEffect);
WM5_IMPLEMENT_ABSTRACT_FACTORY(GlobalEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, GlobalEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(Object, GlobalEffect);

//----------------------------------------------------------------------------
GlobalEffect::GlobalEffect ()
{
}
//----------------------------------------------------------------------------
GlobalEffect::~GlobalEffect ()
{
}
//----------------------------------------------------------------------------
