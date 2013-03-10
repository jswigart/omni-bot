// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Light.h"
#include "Wm5Math.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Light);
WM5_IMPLEMENT_STREAM(Light);
WM5_IMPLEMENT_FACTORY(Light);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, Light);

//----------------------------------------------------------------------------
Light::Light (Type eType)
    :
    Ambient(Float4(0.0f, 0.0f, 0.0f, 1.0f)),
    Diffuse(Float4(0.0f, 0.0f, 0.0f, 1.0f)),
    Specular(Float4(0.0f, 0.0f, 0.0f, 1.0f)),
    Constant(1.0f),
    Linear(0.0f),
    Quadratic(0.0f),
    Intensity(1.0f),
    Angle(Mathf::PI),
    CosAngle(-1.0f),
    SinAngle(0.0f),
    Exponent(1.0f),
    Position(APoint::ORIGIN),
    DVector(-AVector::UNIT_Z),
    UVector(AVector::UNIT_Y),
    RVector(AVector::UNIT_X),
    mType(eType)
{
}
//----------------------------------------------------------------------------
Light::~Light ()
{
}
//----------------------------------------------------------------------------
void Light::SetAngle (float angle)
{
    assertion(0.0f < angle && angle <= Mathf::PI,
        "Angle out of range in SetAngle.\n");

    Angle = angle;
    CosAngle = Mathf::Cos(angle);
    SinAngle = Mathf::Sin(angle);
}
//----------------------------------------------------------------------------
void Light::SetDirection (const AVector& direction)
{
    DVector = direction;
    AVector::GenerateOrthonormalBasis(UVector, RVector, DVector);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Light::Light (LoadConstructor value)
    :
    Object(value),
    Ambient(Float4(0.0f, 0.0f, 0.0f, 1.0f)),
    Diffuse(Float4(0.0f, 0.0f, 0.0f, 1.0f)),
    Specular(Float4(0.0f, 0.0f, 0.0f, 1.0f)),
    Constant(0.0f),
    Linear(0.0f),
    Quadratic(0.0f),
    Intensity(0.0f),
    Angle(0.0f),
    CosAngle(0.0f),
    SinAngle(0.0f),
    Exponent(0.0f),
    Position(APoint::ORIGIN),
    DVector(-AVector::UNIT_Z),
    UVector(AVector::UNIT_Y),
    RVector(AVector::UNIT_X),
    mType(LT_QUANTITY)
{
}
//----------------------------------------------------------------------------
void Light::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadEnum(mType);
    source.ReadAggregate(Ambient);
    source.ReadAggregate(Diffuse);
    source.ReadAggregate(Specular);
    source.Read(Constant);
    source.Read(Linear);
    source.Read(Quadratic);
    source.Read(Intensity);
    source.Read(Angle);
    source.Read(CosAngle);
    source.Read(SinAngle);
    source.Read(Exponent);
    source.ReadAggregate(Position);
    source.ReadAggregate(DVector);
    source.ReadAggregate(UVector);
    source.ReadAggregate(RVector);

    WM5_END_DEBUG_STREAM_LOAD(Light, source);
}
//----------------------------------------------------------------------------
void Light::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void Light::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool Light::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void Light::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteEnum(mType);
    target.WriteAggregate(Ambient);
    target.WriteAggregate(Diffuse);
    target.WriteAggregate(Specular);
    target.Write(Constant);
    target.Write(Linear);
    target.Write(Quadratic);
    target.Write(Intensity);
    target.Write(Angle);
    target.Write(CosAngle);
    target.Write(SinAngle);
    target.Write(Exponent);
    target.WriteAggregate(Position);
    target.WriteAggregate(DVector);
    target.WriteAggregate(UVector);
    target.WriteAggregate(RVector);

    WM5_END_DEBUG_STREAM_SAVE(Light, target);
}
//----------------------------------------------------------------------------
int Light::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_ENUMSIZE(mType);
    size += sizeof(Ambient);
    size += sizeof(Diffuse);
    size += sizeof(Specular);
    size += sizeof(Constant);
    size += sizeof(Linear);
    size += sizeof(Quadratic);
    size += sizeof(Intensity);
    size += sizeof(Angle);
    size += sizeof(CosAngle);
    size += sizeof(SinAngle);
    size += sizeof(Exponent);
    size += sizeof(Position);
    size += sizeof(DVector);
    size += sizeof(UVector);
    size += sizeof(RVector);
    return size;
}
//----------------------------------------------------------------------------
