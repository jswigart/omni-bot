// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/01)

#ifndef WM5VISUALEFFECT_H
#define WM5VISUALEFFECT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5FileIO.h"
#include "Wm5VisualTechnique.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VisualEffect : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VisualEffect);

public:
    // Construction and destruction.
    VisualEffect ();
    virtual ~VisualEffect ();

    // Support for deferred construction.  The function appends the new
    // technique to the end of the array.
    void InsertTechnique (VisualTechnique* technique);

    // Member access.
    inline int GetNumTechniques () const;
    VisualTechnique* GetTechnique (int techniqueIndex) const;

    // Access to components of the effect.
    int GetNumPasses (int techniqueIndex) const;
    VisualPass* GetPass (int techniqueIndex, int passIndex) const;
    VertexShader* GetVertexShader (int techniqueIndex, int passIndex) const;
    PixelShader* GetPixelShader (int techniqueIndex, int passIndex) const;
    AlphaState* GetAlphaState (int techniqueIndex, int passIndex) const;
    CullState* GetCullState (int techniqueIndex, int passIndex) const;
    DepthState* GetDepthState (int techniqueIndex, int passIndex) const;
    OffsetState* GetOffsetState (int techniqueIndex, int passIndex) const;
    StencilState* GetStencilState (int techniqueIndex, int passIndex) const;
    WireState* GetWireState (int techniqueIndex, int passIndex) const;

protected:
    std::vector<VisualTechniquePtr> mTechniques;

// Support for the raw load/save of VisualEffect.
public:
    // Load/save a visual effect from/to a *.wmfx file outside the
    // streaming system.
    static VisualEffect* LoadWMFX (const std::string& name,
        int mode = FileIO::FM_DEFAULT_READ);

    void SaveWMFX (const std::string& name,
        int mode = FileIO::FM_DEFAULT_WRITE);

protected:
    // For derived classes to load the effect from a *.wmfx file.
    VisualEffect (const std::string& name,
        int mode = FileIO::FM_DEFAULT_READ);

private:
    static VisualTechnique* LoadVisualTechnique (FileIO& inFile);
    static VisualPass* LoadVisualPass (FileIO& inFile);
    static Shader* LoadShader (FileIO& inFile, bool isVertexShader);
    static AlphaState* LoadAlphaState (FileIO& inFile);
    static CullState* LoadCullState (FileIO& inFile);
    static DepthState* LoadDepthState (FileIO& inFile);
    static OffsetState* LoadOffsetState (FileIO& inFile);
    static StencilState* LoadStencilState (FileIO& inFile);
    static WireState* LoadWireState (FileIO& inFile);
    static std::string LoadStdString (FileIO& inFile);

    static void SaveVisualTechnique (FileIO& outFile,
        VisualTechnique* technique);
    static void SaveVisualPass (FileIO& outFile, VisualPass* pass);
    static void SaveShader (FileIO& outFile, Shader* shader);
    static void SaveAlphaState (FileIO& outFile, AlphaState* astate);
    static void SaveCullState (FileIO& outFile, CullState* cstate);
    static void SaveDepthState (FileIO& outFile, DepthState* dstate);
    static void SaveOffsetState (FileIO& outFile, OffsetState* ostate);
    static void SaveStencilState (FileIO& outFile, StencilState* sstate);
    static void SaveWireState (FileIO& outFile, WireState* wstate);
    static void SaveStdString (FileIO& outFile, std::string& name);

// End support for raw load/save.
};

WM5_REGISTER_STREAM(VisualEffect);
typedef Pointer0<VisualEffect> VisualEffectPtr;
#include "Wm5VisualEffect.inl"

}

#endif
