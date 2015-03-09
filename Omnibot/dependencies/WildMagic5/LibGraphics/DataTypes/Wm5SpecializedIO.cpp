// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5SpecializedIO.h"

namespace Wm5
{
//----------------------------------------------------------------------------
// Bound
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (Bound& datum)
{
    APoint center;
    float radius;
    if (!ReadAggregate(center) || !Read(radius))
    {
        return false;
    }

    datum.SetCenter(center);
    datum.SetRadius(radius);
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, Bound* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadAggregate(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, Bound*& data)
{
    if (numElements > 0)
    {
        data = new1<Bound>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadAggregate(data[i]))
            {
                return false;
            }
        }
    }
    else
    {
        data = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, Bound*& data)
{
    if (!Read(numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        data = new1<Bound>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadAggregate(data[i]))
            {
                return false;
            }
        }
    }
    else
    {
        data = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const Bound& datum)
{
    APoint center = datum.GetCenter();
    float radius = datum.GetRadius();
    return WriteAggregate(center) && Write(radius);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const Bound* data)
{
    if (!Write(numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteAggregate(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const Bound* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteAggregate(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Float1
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (Float1& datum)
{
    return ReadVV(1, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, Float1* data)
{
    return ReadVV(numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, Float1*& data)
{
    if (numElements > 0)
    {
        data = new1<Float1>(numElements);
        return ReadVV(numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, Float1*& data)
{
    if (!Read(numElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    if (numElements > 0)
    {
        data = new1<Float1>(numElements);
        return ReadVV(numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const Float1& datum)
{
    return WriteN(1, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const Float1* data)
{
    return WriteW(numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const Float1* data)
{
    return WriteN(numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Float2
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (Float2& datum)
{
    return ReadVV(2, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, Float2* data)
{
    return ReadVV(2*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, Float2*& data)
{
    if (numElements > 0)
    {
        data = new1<Float2>(numElements);
        return ReadVV(2*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, Float2*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/2;
    if (numElements > 0)
    {
        data = new1<Float2>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const Float2& datum)
{
    return WriteN(2, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const Float2* data)
{
    return WriteW(2*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const Float2* data)
{
    return WriteN(2*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Float3
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (Float3& datum)
{
    return ReadVV(3, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, Float3* data)
{
    return ReadVV(3*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, Float3*& data)
{
    if (numElements > 0)
    {
        data = new1<Float3>(numElements);
        return ReadVV(3*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, Float3*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/3;
    if (numElements > 0)
    {
        data = new1<Float3>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const Float3& datum)
{
    return WriteN(3, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const Float3* data)
{
    return WriteW(3*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const Float3* data)
{
    return WriteN(3*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Float4
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (Float4& datum)
{
    return ReadVV(4, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, Float4* data)
{
    return ReadVV(4*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, Float4*& data)
{
    if (numElements > 0)
    {
        data = new1<Float4>(numElements);
        return ReadVV(4*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, Float4*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/4;
    if (numElements > 0)
    {
        data = new1<Float4>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const Float4& datum)
{
    return WriteN(4, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const Float4* data)
{
    return WriteW(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const Float4* data)
{
    return WriteN(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// HMatrix
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (HMatrix& datum)
{
    return ReadVV(16, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, HMatrix* data)
{
    return ReadVV(16*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, HMatrix*& data)
{
    if (numElements > 0)
    {
        data = new1<HMatrix>(numElements);
        return ReadVV(16*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, HMatrix*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/16;
    if (numElements > 0)
    {
        data = new1<HMatrix>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const HMatrix& datum)
{
    return WriteN(16, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const HMatrix* data)
{
    return WriteW(16*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const HMatrix* data)
{
    return WriteN(16*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// HPlane
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (HPlane& datum)
{
    return ReadVV(4, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, HPlane* data)
{
    return ReadVV(4*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, HPlane*& data)
{
    if (numElements > 0)
    {
        data = new1<HPlane>(numElements);
        return ReadVV(4*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, HPlane*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/4;
    if (numElements > 0)
    {
        data = new1<HPlane>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const HPlane& datum)
{
    return WriteN(4, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const HPlane* data)
{
    return WriteW(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const HPlane* data)
{
    return WriteN(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// APoint
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (APoint& datum)
{
    return ReadVV(4, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, APoint* data)
{
    return ReadVV(4*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, APoint*& data)
{
    if (numElements > 0)
    {
        data = new1<APoint>(numElements);
        return ReadVV(4*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, APoint*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/4;
    if (numElements > 0)
    {
        data = new1<APoint>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const APoint& datum)
{
    return WriteN(4, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const APoint* data)
{
    return WriteW(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const APoint* data)
{
    return WriteN(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// AVector
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (AVector& datum)
{
    return ReadVV(4, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, AVector* data)
{
    return ReadVV(4*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, AVector*& data)
{
    if (numElements > 0)
    {
        data = new1<AVector>(numElements);
        return ReadVV(4*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, AVector*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/4;
    if (numElements > 0)
    {
        data = new1<AVector>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const AVector& datum)
{
    return WriteN(4, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const AVector* data)
{
    return WriteW(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const AVector* data)
{
    return WriteN(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// HQuaternion
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (HQuaternion& datum)
{
    return ReadVV(4, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, HQuaternion* data)
{
    return ReadVV(4*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, HQuaternion*& data)
{
    if (numElements > 0)
    {
        data = new1<HQuaternion>(numElements);
        return ReadVV(4*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, HQuaternion*& data)
{
    int rawNumElements;
    if (!Read(rawNumElements))
    {
        numElements = 0;
        data = 0;
        return false;
    }

    numElements = rawNumElements/4;
    if (numElements > 0)
    {
        data = new1<HQuaternion>(numElements);
        return ReadVV(rawNumElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const HQuaternion& datum)
{
    return WriteN(4, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const HQuaternion* data)
{
    return WriteW(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const HQuaternion* data)
{
    return WriteN(4*numElements, (const float*)data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Transform
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (Transform& datum)
{
    return ReadAggregate(datum.mHMatrix)
        && ReadAggregate(datum.mInvHMatrix)
        && ReadAggregate(datum.mMatrix)
        && ReadAggregate(datum.mTranslate)
        && ReadAggregate(datum.mScale)
        && ReadBool(datum.mIsIdentity)
        && ReadBool(datum.mIsRSMatrix)
        && ReadBool(datum.mIsUniformScale)
        && ReadBool(datum.mInverseNeedsUpdate);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, Transform* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadAggregate(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, Transform*& data)
{
    if (numElements > 0)
    {
        data = new1<Transform>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadAggregate(data[i]))
            {
                return false;
            }
        }
    }
    else
    {
        data = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, Transform*& data)
{
    if (!Read(numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        data = new1<Transform>(numElements);
        for (int i = 0; i < numElements; ++i)
        {
            if (!ReadAggregate(data[i]))
            {
                return false;
            }
        }
    }
    else
    {
        data = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const Transform& datum)
{
    return WriteAggregate(datum.mHMatrix)
        && WriteAggregate(datum.mInvHMatrix)
        && WriteAggregate(datum.mMatrix)
        && WriteAggregate(datum.mTranslate)
        && WriteAggregate(datum.mScale)
        && WriteBool(datum.mIsIdentity)
        && WriteBool(datum.mIsRSMatrix)
        && WriteBool(datum.mIsUniformScale)
        && WriteBool(datum.mInverseNeedsUpdate);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const Transform* data)
{
    if (!Write(numElements))
    {
        return false;
    }

    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteAggregate(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const Transform* data)
{
    if (numElements > 0)
    {
        for (int i = 0; i < numElements; ++i)
        {
            if (!WriteAggregate(data[i]))
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Vector3f
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregate (Vector3f& datum)
{
    return ReadVV(3, (float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVV (int numElements, Vector3f* data)
{
    return ReadVV(3*numElements, (float*)data);
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateVR (int numElements, Vector3f*& data)
{
    if (numElements > 0)
    {
        data = new1<Vector3f>(numElements);
        return ReadVV(3*numElements, (float*)data);
    }
    data = 0;
    return true;
}
//----------------------------------------------------------------------------
template <>
bool InStream::ReadAggregateRR (int& numElements, Vector3f*& data)
{
    int rawNumElements;
    if (ReadRR(rawNumElements, (float*&)data))
    {
        numElements = rawNumElements/3;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregate (const Vector3f& datum)
{
    return WriteN(3, (const float*)datum);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateW (int numElements, const Vector3f* data)
{
    return WriteW(3*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
template <>
bool OutStream::WriteAggregateN (int numElements, const Vector3f* data)
{
    return WriteN(3*numElements, (const float*)data);
}
//----------------------------------------------------------------------------
}
