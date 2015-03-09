// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/06/27)

#ifndef WM5RENDERER_H
#define WM5RENDERER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5AlphaState.h"
#include "Wm5Camera.h"
#include "Wm5CullState.h"
#include "Wm5DepthState.h"
#include "Wm5Float4.h"
#include "Wm5HMatrix.h"
#include "Wm5IndexBuffer.h"
#include "Wm5Mutex.h"
#include "Wm5OffsetState.h"
#include "Wm5PixelShader.h"
#include "Wm5RenderTarget.h"
#include "Wm5StencilState.h"
#include "Wm5Texture1D.h"
#include "Wm5Texture2D.h"
#include "Wm5Texture3D.h"
#include "Wm5TextureCube.h"
#include "Wm5VertexBuffer.h"
#include "Wm5VertexFormat.h"
#include "Wm5VertexShader.h"
#include "Wm5VisibleSet.h"
#include "Wm5Visual.h"
#include "Wm5WireState.h"

// The Renderer class is an abstract interface that is implemented by each
// platform of interest (Direct3D, OpenGL, software, embedded devices, etc.).
// The class description is factored into two sections.  The first section
// lists the platform-independent functions and data.  These are implemented
// in LibGraphics files Wm5Renderer.{h,inl,cpp}.  The second section lists
// the platform-dependent functions and data.  These are implemented by each
// platform of interest in the LibRenderers folder.

namespace Wm5
{

class RendererInput;
class RendererData;
class GlobalEffect;

// Opaque classes that resolve to the platform-dependent resource (pdr)
// classes.
class PdrIndexBuffer;
class PdrPixelShader;
class PdrRenderTarget;
class PdrTexture1D;
class PdrTexture2D;
class PdrTexture3D;
class PdrTextureCube;
class PdrVertexBuffer;
class PdrVertexFormat;
class PdrVertexShader;

class WM5_GRAPHICS_ITEM Renderer
{
//============================================================================
// Platform-independent portion of the interface.
public:
    // The platform-dependent constructor must call this function first before
    // doing any of its own work.
    void Initialize (int width, int height, Texture::Format colorFormat,
        Texture::Format depthStencilFormat, int numMultisamples);

    // The platform-dependent destructor must call this function after doing
    // any of its own work.
    void Terminate ();

    // Access to constructor inputs.
    inline int GetWidth () const;
    inline int GetHeight () const;
    inline Texture::Format GetColorFormat () const;
    inline Texture::Format GetDepthStencilFormat () const;
    inline int GetNumMultisamples () const;

    // === Resource management.  The resources are defined to be vertex
    // formats, vertex buffers, index buffers, textures (1D, 2D, 3D, cube),
    // render targets, vertex shaders, and pixel shaders.  The functions
    // perform the following operations.
    //
    // Bind:  Create a resource corresponding to the input object.  The
    //    renderer maintains a mapping between the object and the resource.
    //    In most cases, video memory is allocated and a copy is made from
    //    the corresponding system memory of the object.  If Enable or Lock
    //    are called before Bind, the renderer silently creates a resource
    //    rather than cause an exception.
    //
    // BindAll:  Create the resources corresponding to the input object for
    //    all renderers in the system.
    //
    // Unbind:  Destroy the resource corresponding to the input object.  The
    //    renderer removes the object-resource pair from the mapping.  The
    //    object maintains its system memory copy and properties, so it can
    //    always be bound again.
    //
    // UnbindAll:  Destroy the resources corresponding to the input object
    //    for all renderers that created a resource from the object.  This
    //    function is called in the destructors for the objects.
    //
    // Enable:  The resource is made active during the current draw call but
    //    before the DrawPrimitive call is made.
    //
    // Disable:  The resource is made inactive during the current draw call
    //    but after the DrawPrimitive call is made.
    //
    // Lock:  Get a pointer to the video memory of the resource (although the
    //    memory might also be in AGP memory or even system memory depending
    //    on the graphics API and resource usage).  Use this function to
    //    update video memory.  If you do so, be aware that failure to update
    //    the corresponding system memory of the object means the two memory
    //    copies are not synchronized.  You may also lock just for reading, in
    //    which case the video memory content is preserved.  Keep the resource
    //    locked for as little time as possible.
    //
    // Unlock:  Release the pointer to the video memory of the resource.
    //
    // Update:  Lock the video memory of the resource, copy the system memory
    //    contents to it, and then unlock the video memory.  This is the
    //    recommended way for updating resources (update the system memory and
    //    then call Update).  The update is for the renderer calling the
    //    function.
    //
    // UpdateAll:  The same as Update but applied to all renderers sharing the
    //    source object of the resource.
    //
    // ReadColor:  This function is implemented only for RenderTarget.  The
    //    render target must be disabled when ReadColor is called.  The
    //    function returns a copy of the render target color video memory as
    //    a Texture2D object.  The caller is responsible for deleting the
    //    returned object.

    // Vertex format management.  The vertex format object must have been
    // already allocated and its attributes and stride set by the application
    // code.
    void Bind (const VertexFormat* vformat);
    static void BindAll (const VertexFormat* vformat);
    void Unbind (const VertexFormat* vformat);
    static void UnbindAll (const VertexFormat* vformat);
    void Enable (const VertexFormat* vformat);
    void Disable (const VertexFormat* vformat);

    // Vertex buffer management.  The vertex buffer object must have been
    // already allocated by the application code.
    void Bind (const VertexBuffer* vbuffer);
    static void BindAll (const VertexBuffer* vbuffer);
    void Unbind (const VertexBuffer* vbuffer);
    static void UnbindAll (const VertexBuffer* vbuffer);
    void Enable (const VertexBuffer* vbuffer,
        unsigned int streamIndex = 0, unsigned int offset = 0);
    void Disable (const VertexBuffer* vbuffer,
        unsigned int streamIndex = 0);
    void* Lock (const VertexBuffer* vbuffer,
        Buffer::Locking mode);
    void Unlock (const VertexBuffer* vbuffer);
    void Update (const VertexBuffer* vbuffer);
    static void UpdateAll (const VertexBuffer* vbuffer);

    // Index buffer management.  The index buffer object must have been
    // already allocated by the application code.
    void Bind (const IndexBuffer* ibuffer);
    static void BindAll (const IndexBuffer* ibuffer);
    void Unbind (const IndexBuffer* ibuffer);
    static void UnbindAll (const IndexBuffer* ibuffer);
    void Enable (const IndexBuffer* ibuffer);
    void Disable (const IndexBuffer* ibuffer);
    void* Lock (const IndexBuffer* ibuffer, Buffer::Locking mode);
    void Unlock (const IndexBuffer* ibuffer);
    void Update (const IndexBuffer* ibuffer);
    static void UpdateAll (const IndexBuffer* ibuffer);

    // 1D texture management.  The texture object must have been already
    // allocated by the application code.
    void Bind (const Texture1D* texture);
    static void BindAll (const Texture1D* texture);
    void Unbind (const Texture1D* texture);
    static void UnbindAll (const Texture1D* texture);
    void Enable (const Texture1D* texture, int textureUnit);
    void Disable (const Texture1D* texture, int textureUnit);
    void* Lock (const Texture1D* texture, int level,
        Buffer::Locking mode);
    void Unlock (const Texture1D* texture, int level);
    void Update (const Texture1D* texture, int level);
    static void UpdateAll (const Texture1D* texture, int level);

    // 2D texture management.  The texture object must have been already
    // allocated by the application code.
    void Bind (const Texture2D* texture);
    static void BindAll (const Texture2D* texture);
    void Unbind (const Texture2D* texture);
    static void UnbindAll (const Texture2D* texture);
    void Enable (const Texture2D* texture, int textureUnit);
    void Disable (const Texture2D* texture, int textureUnit);
    void* Lock (const Texture2D* texture, int level,
        Buffer::Locking mode);
    void Unlock (const Texture2D* texture, int level);
    void Update (const Texture2D* texture, int level);
    static void UpdateAll (const Texture2D* texture, int level);

    // 3D texture management.  The texture object must have been already
    // allocated by the application code.
    void Bind (const Texture3D* texture);
    static void BindAll (const Texture3D* texture);
    void Unbind (const Texture3D* texture);
    static void UnbindAll (const Texture3D* texture);
    void Enable (const Texture3D* texture, int textureUnit);
    void Disable (const Texture3D* texture, int textureUnit);
    void* Lock (const Texture3D* texture, int level,
        Buffer::Locking mode);
    void Unlock (const Texture3D* texture, int level);
    void Update (const Texture3D* texture, int level);
    static void UpdateAll (const Texture3D* texture, int level);

    // Cube texture management.  The texture object must have been already
    // allocated by the application code.
    void Bind (const TextureCube* texture);
    static void BindAll (const TextureCube* texture);
    void Unbind (const TextureCube* texture);
    static void UnbindAll (const TextureCube* texture);
    void Enable (const TextureCube* texture, int textureUnit);
    void Disable (const TextureCube* texture, int textureUnit);
    void* Lock (const TextureCube* texture, int face, int level,
        Buffer::Locking mode);
    void Unlock (const TextureCube* texture, int face, int level);
    void Update (const TextureCube* texture, int face, int level);
    static void UpdateAll (const TextureCube* texture, int face, int level);

    // Render target management.  The render target object must have been
    // already allocated by the application code.  The index i in ReadColor
    // is the index of the target in a multiple render target object.  Set the
    // input texture pointer to null if you want ReadColor to create the
    // texture.  If you provide an already existing texture, it must be of the
    // correct format and size; otherwise, ReadColor creates an appropriate
    // one, destroys yours, and gives you the new one.
    void Bind (const RenderTarget* renderTarget);
    static void BindAll (const RenderTarget* renderTarget);
    void Unbind (const RenderTarget* renderTarget);
    static void UnbindAll (const RenderTarget* renderTarget);
    void Enable (const RenderTarget* renderTarget);
    void Disable (const RenderTarget* renderTarget);
    void ReadColor (int i, const RenderTarget* renderTarget,
        Texture2D*& texture);

    // Vertex shader management.  The vertex shader object must have been
    // already allocated by the application code.
    void Bind (const VertexShader* vshader);
    static void BindAll (const VertexShader* vshader);
    void Unbind (const VertexShader* vshader);
    static void UnbindAll (const VertexShader* vshader);
    void Enable (const VertexShader* vshader,
        const ShaderParameters* parameters);
    void Disable (const VertexShader* vshader,
        const ShaderParameters* parameters);

    // Pixel shader management.  The pixel shader object must have been
    // already allocated by the application code.
    void Bind (const PixelShader* pshader);
    static void BindAll (const PixelShader* pshader);
    void Unbind (const PixelShader* pshader);
    static void UnbindAll (const PixelShader* pshader);
    void Enable (const PixelShader* pshader,
        const ShaderParameters* parameters);
    void Disable (const PixelShader* pshader,
        const ShaderParameters* parameters);

    // TODO.  ShaderParameters should be another resource, mapped to
    // "constant buffers".  Add these to the renderer.  When ready, remove the
    // ShaderParameters inputs to Enable/Disable of shaders and set up a block
    // of Bind/Unbind/Enable/Disable functions.

    // === End resource management.

    // Access to current global state.
    inline const AlphaState* GetAlphaState () const;
    inline const CullState* GetCullState () const;
    inline const DepthState* GetDepthState () const;
    inline const OffsetState* GetOffsetState () const;
    inline const StencilState* GetStencilState () const;
    inline const WireState* GetWireState () const;

    // Access to the current camera state.
    inline void SetCamera (Camera* camera);
    inline Camera* GetCamera ();
    inline const Camera* GetCamera () const;
    inline const HMatrix& GetViewMatrix () const;
    inline const HMatrix& GetProjectionMatrix () const;
    inline const HMatrix& GetPostProjectionMatrix () const;

    // Compute a picking ray from the specified left-handed screen
    // coordinates (x,y) and using the current camera.  The output
    // 'origin' is the camera position and the 'direction' is a
    // unit-length vector.  Both are in world coordinates.  The return
    // value is 'true' iff (x,y) is in the current viewport.
    bool GetPickRay (int x, int y, APoint& origin, AVector& direction) const;

    // Access to the current clearing parameters for the color, depth, and
    // stencil buffers.  The color buffer is the back buffer.
    inline void SetClearColor (const Float4& clearColor);
    inline const Float4& GetClearColor () const;
    inline void SetClearDepth (float clearDepth);
    inline float GetClearDepth () const;
    inline void SetClearStencil (unsigned int clearStencil);
    inline unsigned int GetClearStencil () const;

    // Access to the current color channel masks.
    inline void GetColorMask (bool& allowRed, bool& allowGreen,
        bool& allowBlue, bool& allowAlpha) const;

    // Override the global state.  If overridden, this state is used instead
    // of the VisualPass state during a drawing call.  To undo the override,
    // pass a null pointer.
    inline void SetOverrideAlphaState (const AlphaState* alphaState);
    inline void SetOverrideCullState (const CullState* cullState);
    inline void SetOverrideDepthState (const DepthState* depthState);
    inline void SetOverrideOffsetState (const OffsetState* offsetState);
    inline void SetOverrideStencilState (const StencilState* stencilState);
    inline void SetOverrideWireState (const WireState* wireState);
    inline const AlphaState* GetOverrideAlphaState () const;
    inline const CullState* GetOverrideCullState () const;
    inline const DepthState* GetOverrideDepthState () const;
    inline const OffsetState* GetOverrideOffsetState () const;
    inline const StencilState* GetOverrideStencilState () const;
    inline const WireState* GetOverrideWireState () const;
    inline void SetReverseCullOrder (bool reverseCullOrder);
    inline bool GetReverseCullOrder () const;

    // The entry point to drawing the visible set of a scene graph.
    void Draw (const VisibleSet& visibleSet, GlobalEffect* globalEffect = 0);

    // The entry points for drawing a single object.
    void Draw (const Visual* visual);
    void Draw (const Visual* visual, const VisualEffectInstance* instance);

private:
    // Support for destructor. Destroy any remaining resources that the
    // application did not explicitly release.
    void DestroyAllVertexFormats ();
    void DestroyAllVertexBuffers ();
    void DestroyAllIndexBuffers ();
    void DestroyAllTexture1Ds ();
    void DestroyAllTexture2Ds ();
    void DestroyAllTexture3Ds ();
    void DestroyAllTextureCubes ();
    void DestroyAllRenderTargets ();
    void DestroyAllVertexShaders ();
    void DestroyAllPixelShaders ();

    // Construction parameters.
    int mWidth;
    int mHeight;
    Texture::Format mColorFormat;
    Texture::Format mDepthStencilFormat;
    int mNumMultisamples;

    // Global state.
    AlphaState* mDefaultAlphaState;
    CullState* mDefaultCullState;
    DepthState* mDefaultDepthState;
    OffsetState* mDefaultOffsetState;
    StencilState* mDefaultStencilState;
    WireState* mDefaultWireState;
    const AlphaState* mAlphaState;
    const CullState* mCullState;
    const DepthState* mDepthState;
    const OffsetState* mOffsetState;
    const StencilState* mStencilState;
    const WireState* mWireState;
    bool mReverseCullOrder;

    // Overrides of global state.
    const AlphaState* mOverrideAlphaState;
    const CullState* mOverrideCullState;
    const DepthState* mOverrideDepthState;
    const OffsetState* mOverrideOffsetState;
    const StencilState* mOverrideStencilState;
    const WireState* mOverrideWireState;

    // Geometric transformation pipeline.  The camera stores the view,
    // projection, and postprojection matrices.
    Camera* mCamera;

    // Framebuffer clearing.
    Float4 mClearColor;
    float mClearDepth;
    unsigned int mClearStencil;

    // Channel masking for the back buffer.
    bool mAllowRed, mAllowGreen, mAllowBlue, mAllowAlpha;

    // Active resources.
    typedef std::map<const VertexFormat*, PdrVertexFormat*> VertexFormatMap;
    typedef std::map<const VertexBuffer*, PdrVertexBuffer*> VertexBufferMap;
    typedef std::map<const IndexBuffer*, PdrIndexBuffer*> IndexBufferMap;
    typedef std::map<const Texture1D*, PdrTexture1D*> Texture1DMap;
    typedef std::map<const Texture2D*, PdrTexture2D*> Texture2DMap;
    typedef std::map<const Texture3D*, PdrTexture3D*> Texture3DMap;
    typedef std::map<const TextureCube*, PdrTextureCube*> TextureCubeMap;
    typedef std::map<const RenderTarget*, PdrRenderTarget*> RenderTargetMap;
    typedef std::map<const VertexShader*, PdrVertexShader*> VertexShaderMap;
    typedef std::map<const PixelShader*, PdrPixelShader*> PixelShaderMap;
    VertexFormatMap mVertexFormats;
    VertexBufferMap mVertexBuffers;
    IndexBufferMap mIndexBuffers;
    Texture1DMap mTexture1Ds;
    Texture2DMap mTexture2Ds;
    Texture3DMap mTexture3Ds;
    TextureCubeMap mTextureCubes;
    RenderTargetMap mRenderTargets;
    VertexShaderMap mVertexShaders;
    PixelShaderMap mPixelShaders;

    // Support for message passing.
    static Mutex msMutex;
    typedef std::set<Renderer*> RendererSet;
    static RendererSet msRenderers;

//============================================================================
// Platform-dependent portion of the interface.
public:
    // Construction and destruction.  Each platform passes opaquely through
    // 'input' whatever information is needed to create a renderer.
    Renderer (RendererInput& input, int width, int height,
        Texture::Format colorFormat, Texture::Format depthStencilFormat,
        int numMultisamples);

    ~Renderer ();

    // Visual state management.
    void SetAlphaState (const AlphaState* alphaState);
    void SetCullState (const CullState* cullState);
    void SetDepthState (const DepthState* depthState);
    void SetOffsetState (const OffsetState* offsetState);
    void SetStencilState (const StencilState* stencilState);
    void SetWireState (const WireState* wireState);

    // Viewport management.  The viewport is specified in right-handed screen
    // coordinates.  The origin is the lower-left corner of the screen, the
    // y-axis points upward, and the x-axis points rightward.
    void SetViewport (int xPosition, int yPosition, int width,
        int height);
    void GetViewport (int& xPosition, int& yPosition, int& width,
        int& height) const;
    void SetDepthRange (float zMin, float zMax);
    void GetDepthRange (float& zMin, float& zMax) const;
    void Resize (int width, int height);

    // Support for clearing the color, depth, and stencil buffers.
    void ClearColorBuffer ();
    void ClearDepthBuffer ();
    void ClearStencilBuffer ();
    void ClearBuffers ();
    void ClearColorBuffer (int x, int y, int w, int h);
    void ClearDepthBuffer (int x, int y, int w, int h);
    void ClearStencilBuffer (int x, int y, int w, int h);
    void ClearBuffers (int x, int y, int w, int h);
    void DisplayColorBuffer ();

    // Support for masking color channels during drawing.
    void SetColorMask (bool allowRed, bool allowGreen,
        bool allowBlue, bool allowAlpha);

    // Support for predraw and postdraw semantics.  All Renderer abstract
    // interface functions and drawing functions must occur within a block of
    // code bounded by PreDraw() and PostDraw().  The general format is
    //   if (renderer->PreDraw())
    //   {
    //       <abstract-interface renderer calls and draw calls>;
    //       renderer->PostDraw();
    //   }
    bool PreDraw ();
    void PostDraw ();

    // The entry point for drawing 2D buffers (for 2D applications).
    void Draw (const unsigned char* screenBuffer, bool reflectY = false);

    // The entry point for drawing 2D text.
    void Draw (int x, int y, const Float4& color,
        const std::string& message);

private:
    // The entry point for drawing 3D objects, called by the single-object
    // Draw function.
    void DrawPrimitive (const Visual* visual);

public_internal:
    // The platform-specific data.  It is in public scope to allow the
    // renderer resource classes to access it.
    RendererData* mData;

    PdrVertexFormat* GetResource (const VertexFormat* vformat);
    PdrVertexBuffer* GetResource (const VertexBuffer* vbuffer);
    PdrIndexBuffer* GetResource (const IndexBuffer* ibuffer);
    PdrTexture1D* GetResource (const Texture1D* texture);
    PdrTexture2D* GetResource (const Texture2D* texture);
    PdrTexture3D* GetResource (const Texture3D* texture);
    PdrTextureCube* GetResource (const TextureCube* texture);
    PdrRenderTarget* GetResource (const RenderTarget* renderTarget);
    PdrVertexShader* GetResource (const VertexShader* vshader);
    PdrPixelShader* GetResource (const PixelShader* pshader);

    // For render target access to allow creation of color/depth textures.
    inline bool InTexture2DMap (const Texture2D* texture);
    inline void InsertInTexture2DMap (const Texture2D* texture,
        PdrTexture2D* platformTexture);
//============================================================================
};

#include "Wm5Renderer.inl"

}

#endif
