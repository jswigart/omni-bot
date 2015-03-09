// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5LIGHT_H
#define WM5LIGHT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5Float4.h"
#include "Wm5APoint.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Light : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Light);

public:
    enum WM5_GRAPHICS_ITEM Type
    {
        LT_AMBIENT,
        LT_DIRECTIONAL,
        LT_POINT,
        LT_SPOT,
        LT_QUANTITY
    };

    // Construction and destruction.
    Light (Type eType = LT_AMBIENT);
    virtual ~Light ();

    // The light type, currently only one of the classic types.  The default
    // value is LT_AMBIENT.
    inline void SetType (Type type);
    inline Type GetType () const;

    // The colors of the light.
    Float4 Ambient;   // default: (0,0,0,1)
    Float4 Diffuse;   // default: (0,0,0,1)
    Float4 Specular;  // default: (0,0,0,1)

    // Attenuation is typically specified as a modulator
    //     m = 1/(C + L*d + Q*d*d)
    // where C is the constant coefficient, L is the linear coefficient,
    // Q is the quadratic coefficient, and d is the distance from the light
    // position to the vertex position.  To allow for a linear adjustment of
    // intensity, the choice is to use instead
    //     m = I/(C + L*d + Q*d*d)
    // where I is an intensity factor.
    float Constant;   // default: 1
    float Linear;     // default: 0
    float Quadratic;  // default: 0
    float Intensity;  // default: 1

    // Parameters for spot lights.  The cone angle must be in radians and
    // should satisfy 0 < Angle <= pi.
    float Angle;     // default: pi
    float CosAngle;  // default: -1
    float SinAngle;  // default:  0
    float Exponent;  // default:  1

    // A helper function that lets you set mAngle and have mCosAngle and
    // mSinAngle computed for you.
    void SetAngle (float angle);

    // Although the standard directional and spot lights need only a direction
    // vector, to allow for new types of derived-class lights that would use
    // a full coordinate frame, Light provides storage for such a frame.  The
    // light frame is always in world coordinates.
    //   default position  P = (0,0,0)
    //   default direction D = (0,0,-1)
    //   default up        U = (0,1,0)
    //   default right     R = (1,0,0)
    // The set {D,U,R} must be a right-handed orthonormal set.  That is, each
    // vector is unit length, the vectors are mutually perpendicular, and
    // R = Cross(D,U).
    APoint Position;
    AVector DVector;
    AVector UVector;
    AVector RVector;

    // A helper function that lets you set the direction vector and computes
    // the up and right vectors automatically.
    void SetDirection (const AVector& direction);

protected:
    Type mType;
};

WM5_REGISTER_STREAM(Light);
typedef Pointer0<Light> LightPtr;
#include "Wm5Light.inl"

}

#endif
