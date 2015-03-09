// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5PlanarReflectionEffect.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, GlobalEffect, PlanarReflectionEffect);
WM5_IMPLEMENT_STREAM(PlanarReflectionEffect);
WM5_IMPLEMENT_FACTORY(PlanarReflectionEffect);

//----------------------------------------------------------------------------
PlanarReflectionEffect::PlanarReflectionEffect (int numPlanes)
    :
    mNumPlanes(numPlanes)
{
    mPlanes = new1<TriMeshPtr>(mNumPlanes);
    mReflectances = new1<float>(mNumPlanes);
    mAlphaState = new0 AlphaState();
    mDepthState = new0 DepthState();
    mStencilState = new0 StencilState();
}
//----------------------------------------------------------------------------
PlanarReflectionEffect::~PlanarReflectionEffect ()
{
    delete1(mPlanes);
    delete1(mReflectances);
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Draw (Renderer* renderer,
    const VisibleSet& visibleSet)
{
    // Save the current global state overrides for restoration later.
    const DepthState* saveDState = renderer->GetOverrideDepthState();
    const StencilState* saveSState = renderer->GetOverrideStencilState();

    // Override the global state to support this effect.
    renderer->SetOverrideDepthState(mDepthState);
    renderer->SetOverrideStencilState(mStencilState);

    // The depth range will be modified during drawing, so save the current
    // depth range for restoration later.
    float minDepth, maxDepth;
    renderer->GetDepthRange(minDepth, maxDepth);

    // Get the camera to store post-world transformations.
    Camera* camera = renderer->GetCamera();

    const int numVisible = visibleSet.GetNumVisible();
    int i, j;
    for (i = 0; i < mNumPlanes; ++i)
    {
        // Render the mirror into the stencil plane.  All visible mirror
        // pixels will have the stencil value of the mirror.  Make sure that
        // no pixels are written to the depth buffer or color buffer, but use
        // depth buffer testing so that the stencil will not be written where
        // the plane is behind something already in the depth buffer.
        mStencilState->Enabled = true;
        mStencilState->Compare = StencilState::CM_ALWAYS;
        mStencilState->Reference = (unsigned int)(i + 1);
        mStencilState->OnFail = StencilState::OT_KEEP;     // irrelevant
        mStencilState->OnZFail = StencilState::OT_KEEP;    // invisible to 0
        mStencilState->OnZPass = StencilState::OT_REPLACE; // visible to i+1

        // Enable depth reads but disable depth writes.
        mDepthState->Enabled = true;
        mDepthState->Writable = false;
        mDepthState->Compare = DepthState::CM_LEQUAL;

        // Disable color writes.
        renderer->SetColorMask(false, false, false, false);

        renderer->Draw(mPlanes[i]);

        // Enable color writes.
        renderer->SetColorMask(true, true, true, true);

        // Render the mirror plane again by only processing pixels where the
        // stencil buffer contains the reference value.  This time there are
        // no changes to the stencil buffer and the depth buffer value is
        // reset to the far clipping plane.  This is done by setting the range
        // of depth values in the viewport volume to be [1,1].  Since the
        // mirror plane cannot also be semi-transparent, we do not care what
        // is behind the mirror plane in the depth buffer.  We need to move
        // the depth buffer values back where the mirror plane will be
        // rendered so that when the reflected object is rendered, it can be
        // depth buffered correctly.  Note that the rendering of the reflected
        // object will cause depth value to be written, which will appear to
        // be behind the mirror plane.  Enable writes to the color buffer.
        // Later when we want to render the reflecting plane and have it blend
        // with the background, which should contain the reflected caster, we
        // want to use the same blending function so that the pixels where the
        // reflected object was not rendered will contain the reflecting plane
        // colors.  In that case, the blending result will have the reflecting
        // plane appear to be opaque when in reality it was blended with
        // blending coefficients adding to one.
        mStencilState->Enabled = true;
        mStencilState->Compare = StencilState::CM_EQUAL;
        mStencilState->Reference = (unsigned int)(i + 1);
        mStencilState->OnFail = StencilState::OT_KEEP;
        mStencilState->OnZFail = StencilState::OT_KEEP;
        mStencilState->OnZPass = StencilState::OT_KEEP;

        // Set the depth buffer to "infinity" at those pixels for which the
        // stencil buffer is the reference value i+1.
        renderer->SetDepthRange(1.0f, 1.0f);
        mDepthState->Enabled = true;
        mDepthState->Writable = true;
        mDepthState->Compare = DepthState::CM_ALWAYS;

        renderer->Draw(mPlanes[i]);

        // Restore the depth range and depth testing function.
        mDepthState->Compare = DepthState::CM_LEQUAL;
        renderer->SetDepthRange(minDepth, maxDepth);

        // Compute the equation for the mirror plane in model coordinates
        // and get the reflection matrix in world coordinates.
        HMatrix reflection;
        HPlane modelPlane;
        GetReflectionMatrixAndModelPlane(i, reflection, modelPlane);

        // TODO:  Add clip plane support to the renderer.
        // Enable a clip plane so that only objects above the mirror plane
        // are reflected.  This occurs before SetTransformation because it
        // needs the current geometric pipeline matrices to compute the clip
        // plane in the correct coordinate system.
        //pkRenderer->EnableUserClipPlane(0,kPlane);

        // This temporarily modifies the world matrix to apply the reflection
        // after the model-to-world transformation.
        camera->SetPreViewMatrix(reflection);

        // Reverse the cull direction.  Allow for models that are not
        // necessarily set up with front or back face culling.
        renderer->SetReverseCullOrder(true);

        // Render the reflected object.  Only render where the stencil buffer
        // contains the reference value.
        for (j = 0; j < numVisible; ++j)
        {
            renderer->Draw((const Visual*)visibleSet.GetVisible(j));
        }                       

        renderer->SetReverseCullOrder(false);

        camera->SetPreViewMatrix(HMatrix::IDENTITY);
        // TODO:  Add clip plane support to the renderer.
        //pkRenderer->DisableUserClipPlane(0);

        // We are about to render the reflecting plane again.  Reset to the
        // global state for the reflecting plane.  We want to blend the
        // reflecting plane with what is already in the color buffer,
        // particularly either the image of the reflected caster or the
        // reflecting plane.  All we want for the reflecting plane at this
        // stage is to force the alpha channel to always be the reflectance
        // value for the reflecting plane.  Render the reflecting plane
        // wherever the stencil buffer is set to the reference value.  This
        // time clear the stencil buffer reference value where it is set.
        // Perform the normal depth buffer testing and writes.  Allow the
        // color buffer to be written to, but this time blend the reflecting
        // plane with the values in the color buffer based on the reflectance
        // value.  Note that where the stencil buffer is set, the color buffer
        // has either color values from the reflecting plane or the reflected
        // object.  Blending will use src=1-alpha (reflecting plane) and
        // dest=alpha background (reflecting plane or reflected object).
        const AlphaState* saveAState = renderer->GetOverrideAlphaState();
        renderer->SetOverrideAlphaState(mAlphaState);
        mAlphaState->BlendEnabled = true;
        mAlphaState->SrcBlend = AlphaState::SBM_ONE_MINUS_CONSTANT_ALPHA;
        mAlphaState->DstBlend = AlphaState::DBM_CONSTANT_ALPHA;
        mAlphaState->ConstantColor = Float4(0.0f, 0.0f, 0.0f,
            mReflectances[i]);

        mStencilState->Compare = StencilState::CM_EQUAL;
        mStencilState->Reference = (unsigned int)(i + 1);
        mStencilState->OnFail = StencilState::OT_KEEP;
        mStencilState->OnZFail = StencilState::OT_KEEP;
        mStencilState->OnZPass = StencilState::OT_INVERT;

        renderer->Draw(mPlanes[i]);

        renderer->SetOverrideAlphaState(saveAState);
    }

    // Restore the global state that existed before this function call.
    renderer->SetOverrideStencilState(saveSState);
    renderer->SetOverrideDepthState(saveDState);

    // Render the objects as usual, this time drawing only the potentially
    // visible objects.
    for (j = 0; j < numVisible; ++j)
    {
        renderer->Draw((const Visual*)visibleSet.GetVisible(j));
    }
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::GetReflectionMatrixAndModelPlane (int i,
    HMatrix& reflection, HPlane& modelPlane)
{
    // Compute the equation for the mirror plane in world coordinates.
    APoint vertex[3];
    mPlanes[i]->GetWorldTriangle(0, vertex);
    HPlane worldPlane(vertex[0], vertex[1], vertex[2]);

    // Compute the reflection matrix.
    reflection.MakeReflection(vertex[0], worldPlane.GetNormal());

    mPlanes[i]->GetModelTriangle(0, vertex);
    modelPlane = HPlane(vertex[0], vertex[1], vertex[2]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* PlanarReflectionEffect::GetObjectByName (const std::string& name)
{
    Object* found = GlobalEffect::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    for (int i = 0; i < mNumPlanes; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mPlanes[i], name, found);
    }

    return 0;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    GlobalEffect::GetAllObjectsByName(name, objects);

    for (int i = 0; i < mNumPlanes; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mPlanes[i], name, objects);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
PlanarReflectionEffect::PlanarReflectionEffect (LoadConstructor value)
    :
    GlobalEffect(value),
    mNumPlanes(0),
    mPlanes(0),
    mReflectances(0)
{
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    GlobalEffect::Load(source);

    source.ReadRR(mNumPlanes, mReflectances);
    source.ReadPointerVR(mNumPlanes, mPlanes);

    WM5_END_DEBUG_STREAM_LOAD(PlanarReflectionEffect, source);
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Link (InStream& source)
{
    GlobalEffect::Link(source);

    source.ResolveLink(mNumPlanes, mPlanes);
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::PostLink ()
{
    GlobalEffect::PostLink();

    mAlphaState = new0 AlphaState();
    mDepthState = new0 DepthState();
    mStencilState = new0 StencilState();
}
//----------------------------------------------------------------------------
bool PlanarReflectionEffect::Register (OutStream& target) const
{
    if (GlobalEffect::Register(target))
    {
        target.Register(mNumPlanes, mPlanes);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    GlobalEffect::Save(target);

    target.WriteW(mNumPlanes, mReflectances);
    target.WritePointerN(mNumPlanes, mPlanes);

    WM5_END_DEBUG_STREAM_SAVE(PlanarReflectionEffect, target);
}
//----------------------------------------------------------------------------
int PlanarReflectionEffect::GetStreamingSize () const
{
    int size = GlobalEffect::GetStreamingSize();
    size += sizeof(mNumPlanes);
    size += mNumPlanes*sizeof(mReflectances[0]);
    size += mNumPlanes*WM5_POINTERSIZE(mPlanes[0]);
    return size;
}
//----------------------------------------------------------------------------
