// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5WINDOWAPPLICATION3_H
#define WM5WINDOWAPPLICATION3_H

#include "Wm5WindowApplication.h"

namespace Wm5
{

class WindowApplication3 : public WindowApplication
{
public:
    // Construction and destruction.
    WindowApplication3 (const char* windowTitle, int xPosition, int yPosition,
        int width, int height, const Float4& clearColor);

    virtual ~WindowApplication3 ();

    // Event callbacks.
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnSpecialKeyDown (int key, int x, int y);
    virtual bool OnSpecialKeyUp (int key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    virtual bool OnMotion (int button, int x, int y,
        unsigned int modifiers);

protected:
    // Camera motion.
    void InitializeCameraMotion (float trnSpeed, float rotSpeed,
        float trnSpeedFactor = 2.0f, float rotSpeedFactor = 2.0f);
    virtual bool MoveCamera ();
    virtual void MoveForward ();
    virtual void MoveBackward ();
    virtual void MoveUp ();
    virtual void MoveDown ();
    virtual void MoveLeft ();
    virtual void MoveRight ();
    virtual void TurnLeft ();
    virtual void TurnRight ();
    virtual void LookUp ();
    virtual void LookDown ();

    CameraPtr mCamera;
    AVector mWorldAxis[3];
    float mTrnSpeed, mTrnSpeedFactor;
    float mRotSpeed, mRotSpeedFactor;
    bool mUArrowPressed;
    bool mDArrowPressed;
    bool mLArrowPressed;
    bool mRArrowPressed;
    bool mPgUpPressed;
    bool mPgDnPressed;
    bool mHomePressed;
    bool mEndPressed;
    bool mInsertPressed;
    bool mDeletePressed;
    bool mCameraMoveable;

    // Object motion.
    void InitializeObjectMotion (Spatial* motionObject);
    bool MoveObject ();
    void RotateTrackBall (float x0, float y0, float x1, float y1);

    SpatialPtr mMotionObject;
    int mDoRoll, mDoYaw, mDoPitch;
    float mXTrack0, mYTrack0, mXTrack1, mYTrack1;
    HMatrix mSaveRotate;
    bool mUseTrackBall, mTrackBallDown;
};

}

#endif
