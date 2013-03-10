// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Matrix3.h"

namespace Wm5
{
template<> const Matrix3<float> Matrix3<float>::ZERO(true);
template<> const Matrix3<float> Matrix3<float>::IDENTITY(false);

template<> const Matrix3<double> Matrix3<double>::ZERO(true);
template<> const Matrix3<double> Matrix3<double>::IDENTITY(false);
}
