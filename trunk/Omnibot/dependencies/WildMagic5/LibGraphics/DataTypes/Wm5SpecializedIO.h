// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5SPECIALIZEDIO_H
#define WM5SPECIALIZEDIO_H

#include "Wm5GraphicsLIB.h"
#include "Wm5InStream.h"
#include "Wm5OutStream.h"

#include "Wm5APoint.h"
#include "Wm5AVector.h"
#include "Wm5Bound.h"
#include "Wm5Float1.h"
#include "Wm5Float2.h"
#include "Wm5Float3.h"
#include "Wm5Float4.h"
#include "Wm5HMatrix.h"
#include "Wm5HPlane.h"
#include "Wm5HQuaternion.h"
#include "Wm5Transform.h"
#include "Wm5Vector3.h"

namespace Wm5
{
// Bound
template <> bool InStream::ReadAggregate (Bound&);
template <> bool InStream::ReadAggregateVV (int, Bound*);
template <> bool InStream::ReadAggregateVR (int, Bound*&);
template <> bool InStream::ReadAggregateRR (int&, Bound*&);
template <> bool OutStream::WriteAggregate (const Bound&);
template <> bool OutStream::WriteAggregateW (int, const Bound*);
template <> bool OutStream::WriteAggregateN (int, const Bound*);

// Float1
template <> bool InStream::ReadAggregate (Float1&);
template <> bool InStream::ReadAggregateVV (int, Float1*);
template <> bool InStream::ReadAggregateVR (int, Float1*&);
template <> bool InStream::ReadAggregateRR (int&, Float1*&);
template <> bool OutStream::WriteAggregate (const Float1&);
template <> bool OutStream::WriteAggregateW (int, const Float1*);
template <> bool OutStream::WriteAggregateN (int, const Float1*);

// Float2
template <> bool InStream::ReadAggregate (Float2&);
template <> bool InStream::ReadAggregateVV (int, Float2*);
template <> bool InStream::ReadAggregateVR (int, Float2*&);
template <> bool InStream::ReadAggregateRR (int&, Float2*&);
template <> bool OutStream::WriteAggregate (const Float2&);
template <> bool OutStream::WriteAggregateW (int, const Float2*);
template <> bool OutStream::WriteAggregateN (int, const Float2*);

// Float3
template <> bool InStream::ReadAggregate (Float3&);
template <> bool InStream::ReadAggregateVV (int, Float3*);
template <> bool InStream::ReadAggregateVR (int, Float3*&);
template <> bool InStream::ReadAggregateRR (int&, Float3*&);
template <> bool OutStream::WriteAggregate (const Float3&);
template <> bool OutStream::WriteAggregateW (int, const Float3*);
template <> bool OutStream::WriteAggregateN (int, const Float3*);

// Float4
template <> bool InStream::ReadAggregate (Float4&);
template <> bool InStream::ReadAggregateVV (int, Float4*);
template <> bool InStream::ReadAggregateVR (int, Float4*&);
template <> bool InStream::ReadAggregateRR (int&, Float4*&);
template <> bool OutStream::WriteAggregate (const Float4&);
template <> bool OutStream::WriteAggregateW (int, const Float4*);
template <> bool OutStream::WriteAggregateN (int, const Float4*);

// HMatrix
template <> bool InStream::ReadAggregate (HMatrix&);
template <> bool InStream::ReadAggregateVV (int, HMatrix*);
template <> bool InStream::ReadAggregateVR (int, HMatrix*&);
template <> bool InStream::ReadAggregateRR (int&, HMatrix*&);
template <> bool OutStream::WriteAggregate (const HMatrix&);
template <> bool OutStream::WriteAggregateW (int, const HMatrix*);
template <> bool OutStream::WriteAggregateN (int, const HMatrix*);

// HPlane
template <> bool InStream::ReadAggregate (HPlane&);
template <> bool InStream::ReadAggregateVV (int, HPlane*);
template <> bool InStream::ReadAggregateVR (int, HPlane*&);
template <> bool InStream::ReadAggregateRR (int&, HPlane*&);
template <> bool OutStream::WriteAggregate (const HPlane&);
template <> bool OutStream::WriteAggregateW (int, const HPlane*);
template <> bool OutStream::WriteAggregateN (int, const HPlane*);

// APoint
template <> bool InStream::ReadAggregate (APoint&);
template <> bool InStream::ReadAggregateVV (int, APoint*);
template <> bool InStream::ReadAggregateVR (int, APoint*&);
template <> bool InStream::ReadAggregateRR (int&, APoint*&);
template <> bool OutStream::WriteAggregate (const APoint&);
template <> bool OutStream::WriteAggregateW (int, const APoint*);
template <> bool OutStream::WriteAggregateN (int, const APoint*);

// AVector
template <> bool InStream::ReadAggregate (AVector&);
template <> bool InStream::ReadAggregateVV (int, AVector*);
template <> bool InStream::ReadAggregateVR (int, AVector*&);
template <> bool InStream::ReadAggregateRR (int&, AVector*&);
template <> bool OutStream::WriteAggregate (const AVector&);
template <> bool OutStream::WriteAggregateW (int, const AVector*);
template <> bool OutStream::WriteAggregateN (int, const AVector*);

// HQuaternion
template <> bool InStream::ReadAggregate (HQuaternion&);
template <> bool InStream::ReadAggregateVV (int, HQuaternion*);
template <> bool InStream::ReadAggregateVR (int, HQuaternion*&);
template <> bool InStream::ReadAggregateRR (int&, HQuaternion*&);
template <> bool OutStream::WriteAggregate (const HQuaternion&);
template <> bool OutStream::WriteAggregateW (int, const HQuaternion*);
template <> bool OutStream::WriteAggregateN (int, const HQuaternion*);

// Transform
template <> bool InStream::ReadAggregate (Transform&);
template <> bool InStream::ReadAggregateVV (int, Transform*);
template <> bool InStream::ReadAggregateVR (int, Transform*&);
template <> bool InStream::ReadAggregateRR (int&, Transform*&);
template <> bool OutStream::WriteAggregate (const Transform&);
template <> bool OutStream::WriteAggregateW (int, const Transform*);
template <> bool OutStream::WriteAggregateN (int, const Transform*);

// Vector3f
template <> bool InStream::ReadAggregate (Vector3f&);
template <> bool InStream::ReadAggregateVV (int, Vector3f*);
template <> bool InStream::ReadAggregateVR (int, Vector3f*&);
template <> bool InStream::ReadAggregateRR (int&, Vector3f*&);
template <> bool OutStream::WriteAggregate (const Vector3f&);
template <> bool OutStream::WriteAggregateW (int, const Vector3f*);
template <> bool OutStream::WriteAggregateN (int, const Vector3f*);
}

#endif
