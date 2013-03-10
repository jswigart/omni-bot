// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Vector4.h"

namespace Wm5
{
template<> const Vector4<float> Vector4<float>::ZERO(0.0f,0.0f,0.0f,0.0f);
template<> const Vector4<float> Vector4<float>::UNIT_X(1.0f,0.0f,0.0f,0.0f);
template<> const Vector4<float> Vector4<float>::UNIT_Y(0.0f,1.0f,0.0f,0.0f);
template<> const Vector4<float> Vector4<float>::UNIT_Z(0.0f,0.0f,1.0f,0.0f);
template<> const Vector4<float> Vector4<float>::UNIT_W(0.0f,0.0f,0.0f,1.0f);
template<> const Vector4<float> Vector4<float>::ONE(1.0f,1.0f,1.0f,1.0f);

template<> const Vector4<double> Vector4<double>::ZERO(0.0,0.0,0.0,0.0);
template<> const Vector4<double> Vector4<double>::UNIT_X(1.0,0.0,0.0,0.0);
template<> const Vector4<double> Vector4<double>::UNIT_Y(0.0,1.0,0.0,0.0);
template<> const Vector4<double> Vector4<double>::UNIT_Z(0.0,0.0,1.0,0.0);
template<> const Vector4<double> Vector4<double>::UNIT_W(0.0,0.0,0.0,1.0);
template<> const Vector4<double> Vector4<double>::ONE(1.0,1.0,1.0,1.0);
}
