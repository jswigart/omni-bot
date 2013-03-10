// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SHADER_H
#define WM5SHADER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5Float4.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Shader : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Shader);

public:
    // Enumerations for the various components of a shader.

    // Maximum number of profiles.  The derived classes VertexShader and
    // PixelShader each have this number of profiles.  If you add a new
    // profile, change this constant and modify the enums in the derived
    // classes.
    enum { MAX_PROFILES = 5 };

    // Types for the input and output variables of the shader program.
    enum WM5_GRAPHICS_ITEM VariableType
    {
        VT_NONE,

        VT_FLOAT,
        VT_FLOAT1,   VT_FLOAT2,   VT_FLOAT3,   VT_FLOAT4,
        VT_FLOAT1X1, VT_FLOAT1X2, VT_FLOAT1X3, VT_FLOAT1X4,
        VT_FLOAT2X1, VT_FLOAT2X2, VT_FLOAT2X3, VT_FLOAT2X4,
        VT_FLOAT3X1, VT_FLOAT3X2, VT_FLOAT3X3, VT_FLOAT3X4,
        VT_FLOAT4X1, VT_FLOAT4X2, VT_FLOAT4X3, VT_FLOAT4X4,

        VT_HALF,
        VT_HALF1,    VT_HALF2,    VT_HALF3,    VT_HALF4,
        VT_HALF1X1,  VT_HALF1X2,  VT_HALF1X3,  VT_HALF1X4,
        VT_HALF2X1,  VT_HALF2X2,  VT_HALF2X3,  VT_HALF2X4,
        VT_HALF3X1,  VT_HALF3X2,  VT_HALF3X3,  VT_HALF3X4,
        VT_HALF4X1,  VT_HALF4X2,  VT_HALF4X3,  VT_HALF4X4,

        VT_INT,
        VT_INT1,     VT_INT2,     VT_INT3,     VT_INT4,
        VT_INT1X1,   VT_INT1X2,   VT_INT1X3,   VT_INT1X4,
        VT_INT2X1,   VT_INT2X2,   VT_INT2X3,   VT_INT2X4,
        VT_INT3X1,   VT_INT3X2,   VT_INT3X3,   VT_INT3X4,
        VT_INT4X1,   VT_INT4X2,   VT_INT4X3,   VT_INT4X4,

        VT_FIXED,
        VT_FIXED1,   VT_FIXED2,   VT_FIXED3,   VT_FIXED4,
        VT_FIXED1X1, VT_FIXED1X2, VT_FIXED1X3, VT_FIXED1X4,
        VT_FIXED2X1, VT_FIXED2X2, VT_FIXED2X3, VT_FIXED2X4,
        VT_FIXED3X1, VT_FIXED3X2, VT_FIXED3X3, VT_FIXED3X4,
        VT_FIXED4X1, VT_FIXED4X2, VT_FIXED4X3, VT_FIXED4X4,

        VT_BOOL,
        VT_BOOL1,    VT_BOOL2,    VT_BOOL3,    VT_BOOL4,
        VT_BOOL1X1,  VT_BOOL1X2,  VT_BOOL1X3,  VT_BOOL1X4,
        VT_BOOL2X1,  VT_BOOL2X2,  VT_BOOL2X3,  VT_BOOL2X4,
        VT_BOOL3X1,  VT_BOOL3X2,  VT_BOOL3X3,  VT_BOOL3X4,
        VT_BOOL4X1,  VT_BOOL4X2,  VT_BOOL4X3,  VT_BOOL4X4,

        VT_QUANTITY
    };

    // Semantics for the input and output variables of the shader program.
    enum WM5_GRAPHICS_ITEM VariableSemantic
    {
        VS_NONE,
        VS_POSITION,        // ATTR0
        VS_BLENDWEIGHT,     // ATTR1
        VS_NORMAL,          // ATTR2
        VS_COLOR0,          // ATTR3 (and for render targets)
        VS_COLOR1,          // ATTR4 (and for render targets)
        VS_FOGCOORD,        // ATTR5
        VS_PSIZE,           // ATTR6
        VS_BLENDINDICES,    // ATTR7
        VS_TEXCOORD0,       // ATTR8
        VS_TEXCOORD1,       // ATTR9
        VS_TEXCOORD2,       // ATTR10
        VS_TEXCOORD3,       // ATTR11
        VS_TEXCOORD4,       // ATTR12
        VS_TEXCOORD5,       // ATTR13
        VS_TEXCOORD6,       // ATTR14
        VS_TEXCOORD7,       // ATTR15
        VS_FOG,             // same as VS_FOGCOORD (ATTR5)
        VS_TANGENT,         // same as VS_TEXCOORD6 (ATTR14)
        VS_BINORMAL,        // same as VS_TEXCOORD7 (ATTR15)
        VS_COLOR2,          // support for multiple render targets
        VS_COLOR3,          // support for multiple render targets
        VS_DEPTH0,          // support for multiple render targets
        VS_QUANTITY
    };

    // The sampler type for interpreting the texture assigned to the sampler.
    enum WM5_GRAPHICS_ITEM SamplerType
    {
        ST_NONE,
        ST_1D,
        ST_2D,
        ST_3D,
        ST_CUBE,
        ST_QUANTITY
    };

    // Filtering modes for the samplers.
    enum WM5_GRAPHICS_ITEM SamplerFilter
    {
        SF_NONE,
        SF_NEAREST,          // nearest neighbor (default)
        SF_LINEAR,           // linear filtering
        SF_NEAREST_NEAREST,  // nearest within image, nearest across images
        SF_NEAREST_LINEAR,   // nearest within image, linear across images
        SF_LINEAR_NEAREST,   // linear within image, nearest across images
        SF_LINEAR_LINEAR,    // linear within image, linear across images
        SF_QUANTITY
    };

    // Maximum value for anisotropic filtering.
    enum
    {
        MAX_ANISOTROPY = 16
    };

    // Texture coordinate modes for the samplers.
    enum WM5_GRAPHICS_ITEM SamplerCoordinate
    {
        SC_NONE,
        SC_CLAMP,
        SC_REPEAT,
        SC_MIRRORED_REPEAT,
        SC_CLAMP_BORDER,
        SC_CLAMP_EDGE,  // default
        SC_QUANTITY
    };

    // Default lodBias is 0.
    // Default anisotropy is 1.
    // Default borderColor is (0,0,0,0).

protected:
    // Abstract base class.  Construction and destruction.  The class is the
    // base for VertexShader and PixelShader.  The class data defines the
    // shader but does not contain instances of shader constants and shader
    // textures.  Each instance of Shader may therefore be a singleton,
    // identified by 'shaderName'.  The drawing of geometry involves a Shader
    // (the abstraction) and a ShaderParameters (the instance of constants and
    // textures).
    //
    // The constructor arrays must be dynamically allocated.  Shader assumes
    // responsibility for deleting them.  The construction of a Shader is not
    // complete until all programs (for the various profiles) are provided
    // via the SetProgram function.
    Shader ();
    Shader (const std::string& programName, int numInputs, int numOutputs,
        int numConstants, int numSamplers, bool profileOwner);
public:
    virtual ~Shader ();

    // Support for deferred construction.
    void SetInput (int i, const std::string& name, VariableType type,
        VariableSemantic semantic);

    void SetOutput (int i, const std::string& name, VariableType type,
        VariableSemantic semantic);

    void SetConstant (int i, const std::string& name, int numRegistersUsed);

    void SetSampler (int i, const std::string& name, SamplerType type);
    void SetFilter (int i, SamplerFilter filter);
    void SetCoordinate (int i, int dim, SamplerCoordinate coordinate);
    void SetLodBias (int i, float lodBias);
    void SetAnisotropy (int i, float anisotropy);
    void SetBorderColor (int i, const Float4& borderColor);

    // Use these when 'profileOwner' is 'true'.  The storage for the items
    // has been dynamically allocated and the class assumes responsibility
    // for deleting it during destruction.
    void SetBaseRegister (int profile, int i, int baseRegister);
    void SetTextureUnit (int profile, int i, int textureUnit);
    void SetProgram (int profile, const std::string& program);

    // Use these when 'profileOwner' is 'false'.  This allows you to store the
    // profile-specific information as global data arrays embedded in the
    // executable or libraries.  The class will not attempt to delete these
    // during destruction.
    void SetBaseRegisters (int* baseRegisters[MAX_PROFILES]);
    void SetTextureUnits (int* textureUnits[MAX_PROFILES]);
    void SetPrograms (std::string programs[MAX_PROFILES]);

    // Member access.

    // Profile-independent data.
    inline int GetNumInputs () const;
    const std::string& GetInputName (int i) const;
    VariableType GetInputType (int i) const;
    VariableSemantic GetInputSemantic (int i) const;

    inline int GetNumOutputs () const;
    const std::string& GetOutputName (int i) const;
    VariableType GetOutputType (int i) const;
    VariableSemantic GetOutputSemantic (int i) const;

    inline int GetNumConstants () const;
    const std::string& GetConstantName (int i) const;
    int GetNumRegistersUsed (int i) const;

    inline int GetNumSamplers () const;
    const std::string& GetSamplerName (int i) const;
    SamplerType GetSamplerType (int i) const;
    SamplerFilter GetFilter (int i) const;
    SamplerCoordinate GetCoordinate (int i, int dim) const;
    float GetLodBias (int i) const;
    float GetAnisotropy (int i) const;
    Float4 GetBorderColor (int i) const;

    // Profile-dependent data.
    int GetBaseRegister (int profile, int i) const;
    int GetTextureUnit (int profile, int i) const;
    const std::string* GetProgram (int profile) const;

protected:
    int mNumInputs;
    std::string* mInputName;
    VariableType* mInputType;
    VariableSemantic* mInputSemantic;

    int mNumOutputs;
    std::string* mOutputName;
    VariableType* mOutputType;
    VariableSemantic* mOutputSemantic;

    int mNumConstants;
    std::string* mConstantName;
    int* mNumRegistersUsed;

    int mNumSamplers;
    std::string* mSamplerName;
    SamplerType* mSamplerType;
    SamplerFilter* mFilter;
    SamplerCoordinate* mCoordinate[3];
    float* mLodBias;
    float* mAnisotropy;
    Float4* mBorderColor;

    bool mProfileOwner;
    int* mBaseRegister[MAX_PROFILES];
    int* mTextureUnit[MAX_PROFILES];
    std::string* mProgram[MAX_PROFILES];

    static std::string msNullString;
};

WM5_REGISTER_STREAM(Shader);
typedef Pointer0<Shader> ShaderPtr;
#include "Wm5Shader.inl"

}

#endif
