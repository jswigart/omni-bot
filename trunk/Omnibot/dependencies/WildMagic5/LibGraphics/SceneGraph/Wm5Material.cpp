// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Material.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Material);
WM5_IMPLEMENT_STREAM(Material);
WM5_IMPLEMENT_FACTORY(Material);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, Material);

//----------------------------------------------------------------------------
Material::Material ()
    :
    Emissive(0.0f, 0.0f, 0.0f, 1.0f),
    Ambient(0.0f, 0.0f, 0.0f, 1.0f),
    Diffuse(0.0f, 0.0f, 0.0f, 1.0f),
    Specular(0.0f, 0.0f, 0.0f, 0.0f)
{
}
//----------------------------------------------------------------------------
Material::~Material ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Material::Material (LoadConstructor value)
    :
    Object(value),
    Emissive(0.0f, 0.0f, 0.0f, 0.0f),
    Ambient(0.0f, 0.0f, 0.0f, 0.0f),
    Diffuse(0.0f, 0.0f, 0.0f, 0.0f),
    Specular(0.0f, 0.0f, 0.0f, 0.0f)
{
}
//----------------------------------------------------------------------------
void Material::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadAggregate(Emissive);
    source.ReadAggregate(Ambient);
    source.ReadAggregate(Diffuse);
    source.ReadAggregate(Specular);

    WM5_END_DEBUG_STREAM_LOAD(Material, source);
}
//----------------------------------------------------------------------------
void Material::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void Material::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool Material::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void Material::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteAggregate(Emissive);
    target.WriteAggregate(Ambient);
    target.WriteAggregate(Diffuse);
    target.WriteAggregate(Specular);

    WM5_END_DEBUG_STREAM_SAVE(Material, target);
}
//----------------------------------------------------------------------------
int Material::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(Emissive);
    size += sizeof(Ambient);
    size += sizeof(Diffuse);
    size += sizeof(Specular);
    return size;
}
//----------------------------------------------------------------------------
