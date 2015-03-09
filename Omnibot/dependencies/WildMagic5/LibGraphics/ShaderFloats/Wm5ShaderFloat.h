// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SHADERFLOAT_H
#define WM5SHADERFLOAT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"

namespace Wm5
{

class Camera;
class Visual;

class WM5_GRAPHICS_ITEM ShaderFloat : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ShaderFloat);

public:
    // Construction and destruction.  The number of registers must be
    // positive.  Each register represents four floating-point values.
    ShaderFloat ();
    ShaderFloat (int numRegisters);
    virtual ~ShaderFloat ();

    // For deferred construction.
    void SetNumRegisters (int numRegisters);

    // Member access.
    inline int GetNumRegisters () const;
    inline const float* GetData () const;
    inline float* GetData ();

    // Access to data by registers.  SetRegister copies four floats from
    // 'data' into register i.  SetRegisters copies mNumElements floats
    // from 'data' into all the registers.  Similar float-sizes are copied
    // by the Get* functions.
    void SetRegister (int i, const float* data);
    void SetRegisters (const float* data);
    void GetRegister (int i, float* data);
    void GetRegisters (float* data);

    // The same as SetRegisters.  This is useful when the source data is
    // an object that defines an implict conversion to 'const float*'.
    ShaderFloat& operator= (const float* data);

    // Access to data by index.
    inline float operator[] (int i) const;
    inline float& operator[] (int i);

    // This function allows for dynamic updates of the shader constants
    // in the VisualEffectInput::Enable function that the renderer calls.
    // Thus, the transfer of the data to the shader programs occurs as
    // late as possible in the rendering pass.  This is useful for global
    // effects that modify camera settings during their drawing calls.
    inline void EnableUpdater ();
    inline void DisableUpdater ();
    inline bool AllowUpdater () const;
    virtual void Update (const Visual* visual, const Camera* camera);

protected:
    int mNumElements;
    float* mData;
    bool mAllowUpdater;
};

WM5_REGISTER_STREAM(ShaderFloat);
typedef Pointer0<ShaderFloat> ShaderFloatPtr;
#include "Wm5ShaderFloat.inl"

}

#endif
