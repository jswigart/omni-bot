// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Quaternion.h"

namespace Wm5
{
template<> const Quaternion<float>
Quaternion<float>::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

template<> const Quaternion<float>
Quaternion<float>::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f);


template<> const Quaternion<double>
Quaternion<double>::ZERO(0.0, 0.0, 0.0, 0.0);

template<> const Quaternion<double>
Quaternion<double>::IDENTITY(1.0, 0.0, 0.0, 0.0);
}
