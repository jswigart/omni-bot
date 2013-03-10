// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.1 (2010/10/01)

#ifndef WM5BLENDTRANSFORMCONTROLLER_H
#define WM5BLENDTRANSFORMCONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5TransformController.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM BlendTransformController : public TransformController
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(BlendTransformController);

public:
    // Construction and destruction.
    //
    // Set 'rsMatrices' to 'true' when theinput controllers manage
    // transformations of the form Y = R*S*X + T, where R is a rotation, S is
    // a diagonal scale matrix of positive scales, and T is a translation;
    // that is, each transform has mIsRSMatrix equal to 'true'.  In this case,
    // the rotation and scale blending is either geometric or arithmetic, as
    // specified in the other constructor inputs.  Translation blending is
    // always arithmetic.  Let {R0,S0,T0} and {R1,S1,T1} be the transformation
    // channels, and let weight w be in [0,1].  Let {R,S,T} be the blended
    // result.  Let q0, q1, and q be quaternions corresponding to R0, R1, and
    // R with Dot(q0,q1) >= 0 and A = angle(q0,q1) = acos(Dot(q0,q1)).
    //
    // Translation:  T = (1-w)*T0 + w*T1
    //
    // Arithmetic rotation:  q = Normalize((1-w)*q0 + w*q1)
    // Geometric rotation:
    //   q = Slerp(w,q0,q1)
    //     = (sin((1-w)*A)*q0 + sin(w*A)*q1)/sin(A)
    //
    // Arithmetic scale:  s = (1-w)*s0 + w*s1 for each channel s0, s1, s
    // Geometric scale:  s = sign(s0)*sign(s1)*pow(|s0|,1-w)*pow(|s1|,w)
    //    If either of s0 or s1 is zero, then s is zero.
    //
    // Set 'rsMatrices' to 'false' when mIsRMatrix is 'false' for either
    // transformation.  In this case, a weighted average of the full
    // transforms is computed.  This is not recommended, because the visual
    // results are difficult to predict.
    BlendTransformController (TransformController* controller0,
        TransformController* controller1, bool rsMatrices,
        bool geometricRotation = false, bool geometricScale = false);

    virtual ~BlendTransformController ();

    // Member access.  The weight w is a number for which 0 <= w <= 1.
    inline TransformController* GetController0 () const;
    inline TransformController* GetController1 () const;
    inline bool GetRSMatrices () const;
    inline void SetWeight (float weight);
    inline float GetWeight () const;

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

protected:
    // Set the object for 'this' and for the managed controllers.
    virtual void SetObject (ControlledObject* object);

    TransformControllerPtr mController0, mController1;
    float mWeight;
    bool mRSMatrices, mGeometricRotation, mGeometricScale;
};

WM5_REGISTER_STREAM(BlendTransformController);
typedef Pointer0<BlendTransformController> BlendTransformControllerPtr;
#include "Wm5BlendTransformController.inl"

}

#endif
