// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VisualTechnique.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, VisualTechnique);
WM5_IMPLEMENT_STREAM(VisualTechnique);
WM5_IMPLEMENT_FACTORY(VisualTechnique);

//----------------------------------------------------------------------------
VisualTechnique::VisualTechnique ()
{
}
//----------------------------------------------------------------------------
VisualTechnique::~VisualTechnique ()
{
}
//----------------------------------------------------------------------------
void VisualTechnique::InsertPass (VisualPass* pass)
{
    if (pass)
    {
        mPasses.push_back(pass);
    }
    else
    {
        assertion(false, "Input to InsertPass must be nonnull.\n");
    }
}
//----------------------------------------------------------------------------
VisualPass* VisualTechnique::GetPass (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex];
    }

    assertion(false, "Invalid index in GetPass.\n");
    return 0;
}
//----------------------------------------------------------------------------
VertexShader* VisualTechnique::GetVertexShader (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetVertexShader();
    }

    assertion(false, "Invalid index in GetVertexShader.\n");
    return 0;
}
//----------------------------------------------------------------------------
PixelShader* VisualTechnique::GetPixelShader (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetPixelShader();
    }

    assertion(false, "Invalid index in GetPixelShader.\n");
    return 0;
}
//----------------------------------------------------------------------------
AlphaState* VisualTechnique::GetAlphaState (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetAlphaState();
    }

    assertion(false, "Invalid index in GetAlphaState.\n");
    return 0;
}
//----------------------------------------------------------------------------
CullState* VisualTechnique::GetCullState (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetCullState();
    }

    assertion(false, "Invalid index in GetCullState.\n");
    return 0;
}
//----------------------------------------------------------------------------
DepthState* VisualTechnique::GetDepthState (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetDepthState();
    }

    assertion(false, "Invalid index in GetDepthState.\n");
    return 0;
}
//----------------------------------------------------------------------------
OffsetState* VisualTechnique::GetOffsetState (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetOffsetState();
    }

    assertion(false, "Invalid index in GetOffsetState.\n");
    return 0;
}
//----------------------------------------------------------------------------
StencilState* VisualTechnique::GetStencilState (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetStencilState();
    }

    assertion(false, "Invalid index in GetStencilState.\n");
    return 0;
}
//----------------------------------------------------------------------------
WireState* VisualTechnique::GetWireState (int passIndex) const
{
    if (0 <= passIndex && passIndex < (int)mPasses.size())
    {
        return mPasses[passIndex]->GetWireState();
    }

    assertion(false, "Invalid index in GetWireState.\n");
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* VisualTechnique::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    std::vector<VisualPassPtr>::iterator iter = mPasses.begin();
    std::vector<VisualPassPtr>::iterator end = mPasses.end();
    for (/**/; iter != end; ++iter)
    {
        WM5_GET_OBJECT_BY_NAME((*iter), name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void VisualTechnique::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    std::vector<VisualPassPtr>::iterator iter = mPasses.begin();
    std::vector<VisualPassPtr>::iterator end = mPasses.end();
    for (/**/; iter != end; ++iter)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME((*iter), name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
VisualTechnique::VisualTechnique (LoadConstructor value)
    :
    Object(value)
{
}
//----------------------------------------------------------------------------
void VisualTechnique::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    int numPasses;
    source.Read(numPasses);
    mPasses.resize(numPasses);
    source.ReadPointerVV(numPasses, &mPasses[0]);

    WM5_END_DEBUG_STREAM_LOAD(VisualTechnique, source);
}
//----------------------------------------------------------------------------
void VisualTechnique::Link (InStream& source)
{
    Object::Link(source);

    std::vector<VisualPassPtr>::iterator iter = mPasses.begin();
    std::vector<VisualPassPtr>::iterator end = mPasses.end();
    for (/**/; iter != end; ++iter)
    {
        source.ResolveLink(*iter);
    }
}
//----------------------------------------------------------------------------
void VisualTechnique::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool VisualTechnique::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        std::vector<VisualPassPtr>::const_iterator iter = mPasses.begin();
        std::vector<VisualPassPtr>::const_iterator end = mPasses.end();
        for (/**/; iter != end; ++iter)
        {
            target.Register(*iter);
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void VisualTechnique::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WritePointerW((int)mPasses.size(), &mPasses[0]);

    WM5_END_DEBUG_STREAM_SAVE(VisualTechnique, target);
}
//----------------------------------------------------------------------------
int VisualTechnique::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    int numPasses = (int)mPasses.size();
    size += sizeof(numPasses);
    size += numPasses*WM5_POINTERSIZE(mPasses[0]);
    return size;
}
//----------------------------------------------------------------------------
