// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Vector3.h"

namespace Wm5
{
template<> const Vector3<float> Vector3<float>::ZERO(0.0f, 0.0f, 0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_X(1.0f, 0.0f, 0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_Y(0.0f, 1.0f, 0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_Z(0.0f, 0.0f, 1.0f);
template<> const Vector3<float> Vector3<float>::ONE(1.0f, 1.0f, 1.0f);

template<> const Vector3<double> Vector3<double>::ZERO(0.0, 0.0, 0.0);
template<> const Vector3<double> Vector3<double>::UNIT_X(1.0, 0.0, 0.0);
template<> const Vector3<double> Vector3<double>::UNIT_Y(0.0, 1.0, 0.0);
template<> const Vector3<double> Vector3<double>::UNIT_Z(0.0, 0.0, 1.0);
template<> const Vector3<double> Vector3<double>::ONE(1.0, 1.0, 1.0);
}
