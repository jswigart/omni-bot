// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SHADERPARAMETERS_H
#define WM5SHADERPARAMETERS_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Shader.h"
#include "Wm5ShaderFloat.h"
#include "Wm5Texture.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ShaderParameters : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ShaderParameters);

public:
    // Construction and destruction.
    ShaderParameters (Shader* shader);
    virtual ~ShaderParameters ();

    // Member access.
    inline int GetNumConstants () const;
    inline int GetNumTextures () const;
    inline ShaderFloatPtr* GetConstants () const;
    inline TexturePtr* GetTextures () const;

    // These functions set the constants/textures.  If successful, the return
    // value is nonnegative and is the index into the appropriate array.  This
    // index may passed to the Set* functions that have the paremeter
    // 'handle'.  The mechanism allows you to set directly by index and avoid
    // the name comparisons that occur with the Set* functions that have the
    // parameter 'const std::string& name'.
    int SetConstant (const std::string& name, ShaderFloat* sfloat);
    int SetTexture (const std::string& name, Texture* texture);

    // The 'handle' is the return value of the Set* functions above.
    void SetConstant (int handle, ShaderFloat* sfloat);
    void SetTexture (int handle, Texture* texture);

    // Member access.
    ShaderFloat* GetConstant (const std::string& name) const;
    Texture* GetTexture (const std::string& name) const;

    // The 'handle' is the return value of the Set* functions above.
    ShaderFloat* GetConstant (int handle) const;
    Texture* GetTexture (int handle) const;

    // Update shader constants during a drawing call.
    void UpdateConstants (const Visual* visual, const Camera* camera);

protected:
    ShaderPtr mShader;
    int mNumConstants;
    ShaderFloatPtr* mConstants;
    int mNumTextures;
    TexturePtr* mTextures;
};

WM5_REGISTER_STREAM(ShaderParameters);
typedef Pointer0<ShaderParameters> ShaderParametersPtr;
#include "Wm5ShaderParameters.inl"

}

#endif
