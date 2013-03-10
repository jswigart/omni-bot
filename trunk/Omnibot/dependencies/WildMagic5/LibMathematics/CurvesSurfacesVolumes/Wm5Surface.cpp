// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Surface.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Surface<Real>::Surface ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Surface<Real>::~Surface ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Surface<float>;

template WM5_MATHEMATICS_ITEM
class Surface<double>;
//----------------------------------------------------------------------------
}
