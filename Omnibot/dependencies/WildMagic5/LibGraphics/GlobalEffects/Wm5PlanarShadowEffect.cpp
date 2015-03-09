// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5PlanarShadowEffect.h"
#include "Wm5MaterialEffect.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, GlobalEffect, PlanarShadowEffect);
WM5_IMPLEMENT_STREAM(PlanarShadowEffect);
WM5_IMPLEMENT_FACTORY(PlanarShadowEffect);

//----------------------------------------------------------------------------
PlanarShadowEffect::PlanarShadowEffect (int numPlanes, Node* shadowCaster)
    :
    mNumPlanes(numPlanes),
    mShadowCaster(shadowCaster)
{
    mPlanes = new1<TriMeshPtr>(mNumPlanes);
    mProjectors = new1<LightPtr>(mNumPlanes);
    mShadowColors = new1<Float4>(mNumPlanes);

    mAlphaState = new0 AlphaState();
    mDepthState = new0 DepthState();
    mStencilState = new0 StencilState();

    mMaterial = new0 Material();
    mMaterialEffect = new0 MaterialEffect();
    mMaterialEffectInstance = mMaterialEffect->CreateInstance(mMaterial);

    // The material diffuse color changes per plane on every draw call.
    ShaderFloat* sfloat = mMaterialEffectInstance->GetVertexConstant(0,
        "MaterialDiffuse");
    sfloat->EnableUpdater();
}
//----------------------------------------------------------------------------
PlanarShadowEffect::~PlanarShadowEffect ()
{
    delete1(mPlanes);
    delete1(mProjectors);
    delete1(mShadowColors);
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Draw (Renderer* renderer,
    const VisibleSet& visibleSet)
{
    // Draw the potentially visible portions of the shadow caster.
    const int numVisible = visibleSet.GetNumVisible();
    int j;
    for (j = 0; j < numVisible; ++j)
    {
        renderer->Draw((const Visual*)visibleSet.GetVisible(j));
    }

    // Save the current global state overrides for restoration later.
    const DepthState* saveDState = renderer->GetOverrideDepthState();
    const StencilState* saveSState = renderer->GetOverrideStencilState();

    // Override the global state to support this effect.
    renderer->SetOverrideDepthState(mDepthState);
    renderer->SetOverrideStencilState(mStencilState);

    // Get the camera to store post-world transformations.
    Camera* camera = renderer->GetCamera();

    for (int i = 0; i < mNumPlanes; ++i)
    {
        // Enable depth buffering.  NOTE: The plane object should not have a
        // ZBufferState object that changes the current settings.
        mDepthState->Enabled = true;
        mDepthState->Writable = true;
        mDepthState->Compare = DepthState::CM_LEQUAL;

        // Enable the stencil buffer so that the shadow can be clipped by the
        // plane.  The stencil values are set whenever the corresponding
        // plane pixels are visible.
        mStencilState->Enabled = true;
        mStencilState->Compare = StencilState::CM_ALWAYS;
        mStencilState->Reference = (unsigned int)(i + 1);
        mStencilState->OnFail = StencilState::OT_KEEP;      // irrelevant
        mStencilState->OnZFail = StencilState::OT_KEEP;     // invisible to 0
        mStencilState->OnZPass = StencilState::OT_REPLACE;  // visible to i+1

        // Draw the plane.
        renderer->Draw(mPlanes[i]);

        // Blend the shadow color with the pixels drawn on the projection
        // plane.  The blending equation is
        //   (rf,gf,bf) = as*(rs,gs,bs) + (1-as)*(rd,gd,bd)
        // where (rf,gf,bf) is the final color to be written to the frame
        // buffer, (rs,gs,bs,as) is the shadow color, and (rd,gd,bd) is the
        // current color of the frame buffer.
        const AlphaState* saveAState = renderer->GetOverrideAlphaState();
        renderer->SetOverrideAlphaState(mAlphaState);
        mAlphaState->BlendEnabled = true;
        mAlphaState->SrcBlend = AlphaState::SBM_SRC_ALPHA;
        mAlphaState->DstBlend = AlphaState::DBM_ONE_MINUS_SRC_ALPHA;
        mMaterial->Diffuse = mShadowColors[i];

        // Disable the depth buffer reading so that no depth-buffer fighting
        // occurs.  The drawing of pixels is controlled solely by the stencil
        // value.
        mDepthState->Enabled = false;

        // Only draw where the plane has been drawn.
        mStencilState->Enabled = true;
        mStencilState->Compare = StencilState::CM_EQUAL;
        mStencilState->Reference = (unsigned int)(i + 1);
        mStencilState->OnFail = StencilState::OT_KEEP;   // invisible kept 0
        mStencilState->OnZFail = StencilState::OT_KEEP;  // irrelevant
        mStencilState->OnZPass = StencilState::OT_ZERO;  // visible set to 0

        // Get the projection matrix relative to the projector (light).
        HMatrix projection;
        if (!GetProjectionMatrix(i, projection))
        {
            continue;
        }
        camera->SetPreViewMatrix(projection);

        // Draw the caster again, but temporarily use a material effect so
        // that the shadow color is blended onto the plane.  TODO:  This
        // drawing pass should use a VisibleSet relative to the projector so
        // that objects that are out of view (i.e. culled relative to the
        // camera and not in the camera's VisibleSet) can cast shadows.
        for (j = 0; j < numVisible; ++j)
        {
            Visual* visual = (Visual*)visibleSet.GetVisible(j);
            VisualEffectInstancePtr save = visual->GetEffectInstance();
            visual->SetEffectInstance(mMaterialEffectInstance);
            renderer->Draw(visual);
            visual->SetEffectInstance(save);
        }

        camera->SetPreViewMatrix(HMatrix::IDENTITY);

        renderer->SetOverrideAlphaState(saveAState);
    }

    // Restore the global state that existed before this function call.
    renderer->SetOverrideStencilState(saveSState);
    renderer->SetOverrideDepthState(saveDState);
}
//----------------------------------------------------------------------------
bool PlanarShadowEffect::GetProjectionMatrix (int i, HMatrix& projection)
{
    // Compute the equation for the shadow plane in world coordinates.
    APoint vertex[3];
    mPlanes[i]->GetWorldTriangle(0, vertex);
    HPlane worldPlane(vertex[0], vertex[1], vertex[2]);

    // This is a conservative test to see whether a shadow should be cast.
    // This can cause incorrect results if the caster is large and intersects
    // the plane, but ordinarily we are not trying to cast shadows in such
    // situations.
    if (mShadowCaster->WorldBound.WhichSide(worldPlane) < 0)
    {
        // The shadow caster is on the far side of plane, so it cannot cast
        // a shadow.
        return false;
    }

    // Compute the projection matrix for the light source.
    Light* projector = mProjectors[i];
    AVector normal = worldPlane.GetNormal();
    if (projector->GetType() == Light::LT_DIRECTIONAL)
    {
        float NdD = normal.Dot(projector->DVector);
        if (NdD >= 0.0f)
        {
            // The projection must be onto the "positive side" of the plane.
            return false;
        }

        projection.MakeObliqueProjection(vertex[0], normal,
            projector->DVector);
    }
    else if (projector->GetType() == Light::LT_POINT
    ||  projector->GetType() == Light::LT_SPOT)
    {
        float NdE = projector->Position.Dot(normal);
        if (NdE <= 0.0f)
        {
            // The projection must be onto the "positive side" of the plane.
            return false;
        }

        projection.MakePerspectiveProjection(vertex[0], normal,
            projector->Position);
    }
    else
    {
        assertion(false, "Light type not supported.\n");
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* PlanarShadowEffect::GetObjectByName (const std::string& name)
{
    Object* found = GlobalEffect::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    for (int i = 0; i < mNumPlanes; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mPlanes[i], name, found);
        WM5_GET_OBJECT_BY_NAME(mProjectors[i], name, found);
    }

    // Avoid the cycle by not checking mShadowCaster.

    return 0;
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    GlobalEffect::GetAllObjectsByName(name, objects);

    for (int i = 0; i < mNumPlanes; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mPlanes[i], name, objects);
        WM5_GET_ALL_OBJECTS_BY_NAME(mProjectors[i], name, objects);
    }

    // Avoid the cycle by not checking mShadowCaster.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
PlanarShadowEffect::PlanarShadowEffect (LoadConstructor value)
    :
    GlobalEffect(value),
    mNumPlanes(0),
    mPlanes(0),
    mProjectors(0),
    mShadowColors(0)
{
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    GlobalEffect::Load(source);

    source.ReadAggregateRR(mNumPlanes, mShadowColors);
    source.ReadPointerVR(mNumPlanes, mPlanes);
    source.ReadPointerVR(mNumPlanes, mProjectors);
    source.ReadPointer(mShadowCaster);

    WM5_END_DEBUG_STREAM_LOAD(PlanarShadowEffect, source);
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Link (InStream& source)
{
    GlobalEffect::Link(source);

    source.ResolveLink(mNumPlanes, mPlanes);
    source.ResolveLink(mNumPlanes, mProjectors);
    source.ResolveLink(mShadowCaster);
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::PostLink ()
{
    GlobalEffect::PostLink();

    mAlphaState = new0 AlphaState();
    mDepthState = new0 DepthState();
    mStencilState = new0 StencilState();
    mMaterial = new0 Material();
    mMaterialEffect = new0 MaterialEffect();
    mMaterialEffectInstance = mMaterialEffect->CreateInstance(mMaterial);
}
//----------------------------------------------------------------------------
bool PlanarShadowEffect::Register (OutStream& target) const
{
    if (GlobalEffect::Register(target))
    {
        target.Register(mNumPlanes, mPlanes);
        target.Register(mNumPlanes, mProjectors);
        target.Register(mShadowCaster);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    GlobalEffect::Save(target);

    target.WriteAggregateW(mNumPlanes, mShadowColors);
    target.WritePointerN(mNumPlanes, mPlanes);
    target.WritePointerN(mNumPlanes, mProjectors);
    target.WritePointer(mShadowCaster);

    WM5_END_DEBUG_STREAM_SAVE(PlanarShadowEffect, target);
}
//----------------------------------------------------------------------------
int PlanarShadowEffect::GetStreamingSize () const
{
    int size = GlobalEffect::GetStreamingSize();
    size += sizeof(mNumPlanes);
    size += mNumPlanes*sizeof(mShadowCaster[0]);
    size += mNumPlanes*WM5_POINTERSIZE(mPlanes[0]);
    size += mNumPlanes*WM5_POINTERSIZE(mProjectors[0]);
    size += WM5_POINTERSIZE(mShadowCaster);
    return size;
}
//----------------------------------------------------------------------------
