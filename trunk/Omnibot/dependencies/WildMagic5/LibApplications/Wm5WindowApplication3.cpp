// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ApplicationPCH.h"
#include "Wm5WindowApplication3.h"
using namespace Wm5;

//----------------------------------------------------------------------------
WindowApplication3::WindowApplication3 (const char* windowTitle,
    int xPosition, int yPosition, int width, int height,
    const Float4& clearColor)
    :
    WindowApplication(windowTitle, xPosition, yPosition, width, height,
        clearColor),
    mTrnSpeed(0.0f),
    mRotSpeed(0.0f),
    mUArrowPressed(false),
    mDArrowPressed(false),
    mLArrowPressed(false),
    mRArrowPressed(false),
    mPgUpPressed(false),
    mPgDnPressed(false),
    mHomePressed(false),
    mEndPressed(false),
    mInsertPressed(false),
    mDeletePressed(false),
    mCameraMoveable(false),
    mDoRoll(0),
    mDoYaw(0),
    mDoPitch(0),
    mXTrack0(0.0f),
    mYTrack0(0.0f),
    mXTrack1(0.0f),
    mYTrack1(0.0f),
    mUseTrackBall(true),
    mTrackBallDown(false)
{
    mWorldAxis[0] = AVector::ZERO;
    mWorldAxis[1] = AVector::ZERO;
    mWorldAxis[2] = AVector::ZERO;
}
//----------------------------------------------------------------------------
WindowApplication3::~WindowApplication3 ()
{
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnInitialize ()
{
    if (!WindowApplication::OnInitialize())
    {
        return false;
    }

    mCamera = new0 Camera();
    mRenderer->SetCamera(mCamera);
    mMotionObject = 0;
    return true;
}
//----------------------------------------------------------------------------
void WindowApplication3::OnTerminate ()
{
    mRenderer->SetCamera(0);
    mCamera = 0;
    mMotionObject = 0;
    WindowApplication::OnTerminate();
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication::OnKeyDown(key, x, y))
    {
        return true;
    }

    // Standard keys for Wild Magic applications.
    switch (key)
    {
    case 't':  // Slower camera translation.
        if (mCameraMoveable)
        {
            mTrnSpeed /= mTrnSpeedFactor;
        }
        return true;
    case 'T':  // Faster camera translation.
        if (mCameraMoveable)
        {
            mTrnSpeed *= mTrnSpeedFactor;
        }
        return true;
    case 'r':  // Slower camera rotation.
        if (mCameraMoveable)
        {
            mRotSpeed /= mRotSpeedFactor;
        }
        return true;
    case 'R':  // Faster camera rotation.
        if (mCameraMoveable)
        {
            mRotSpeed *= mRotSpeedFactor;
        }
        return true;
    case '?':  // Reset the timer.
        ResetTime();
        return true;
    };

    return false;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnSpecialKeyDown (int key, int, int)
{
    if (mCameraMoveable)
    {
        if (key == KEY_LEFT_ARROW)
        {
            mLArrowPressed = true;
            return true;
        }
        if (key == KEY_RIGHT_ARROW)
        {
            mRArrowPressed = true;
            return true;
        }
        if (key == KEY_UP_ARROW)
        {
            mUArrowPressed = true;
            return true;
        }
        if (key == KEY_DOWN_ARROW)
        {
            mDArrowPressed = true;
            return true;
        }
        if (key == KEY_PAGE_UP)
        {
            mPgUpPressed = true;
            return true;
        }
        if (key == KEY_PAGE_DOWN)
        {
            mPgDnPressed = true;
            return true;
        }
        if (key == KEY_HOME)
        {
            mHomePressed = true;
            return true;
        }
        if (key == KEY_END)
        {
            mEndPressed = true;
            return true;
        }
        if (key == KEY_INSERT)
        {
            mInsertPressed = true;
            return true;
        }
        if (key == KEY_DELETE)
        {
            mDeletePressed = true;
            return true;
        }
    }

    if (mMotionObject)
    {
        if (key == KEY_F1)
        {
            mDoRoll = -1;
            return true;
        }
        if (key == KEY_F2)
        {
            mDoRoll = 1;
            return true;
        }
        if (key == KEY_F3)
        {
            mDoYaw = -1;
            return true;
        }
        if (key == KEY_F4)
        {
            mDoYaw = 1;
            return true;
        }
        if (key == KEY_F5)
        {
            mDoPitch = -1;
            return true;
        }
        if (key == KEY_F6)
        {
            mDoPitch = 1;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnSpecialKeyUp (int key, int, int)
{
    if (mCameraMoveable)
    {
        if (key == KEY_LEFT_ARROW)
        {
            mLArrowPressed = false;
            return true;
        }
        if (key == KEY_RIGHT_ARROW)
        {
            mRArrowPressed = false;
            return true;
        }
        if (key == KEY_UP_ARROW)
        {
            mUArrowPressed = false;
            return true;
        }
        if (key == KEY_DOWN_ARROW)
        {
            mDArrowPressed = false;
            return true;
        }
        if (key == KEY_PAGE_UP)
        {
            mPgUpPressed = false;
            return true;
        }
        if (key == KEY_PAGE_DOWN)
        {
            mPgDnPressed = false;
            return true;
        }
        if (key == KEY_HOME)
        {
            mHomePressed = false;
            return true;
        }
        if (key == KEY_END)
        {
            mEndPressed = false;
            return true;
        }
        if (key == KEY_INSERT)
        {
            mInsertPressed = false;
            return true;
        }
        if (key == KEY_DELETE)
        {
            mDeletePressed = false;
            return true;
        }
    }

    if (mMotionObject)
    {
        if (key == KEY_F1)
        {
            mDoRoll = 0;
            return true;
        }
        if (key == KEY_F2)
        {
            mDoRoll = 0;
            return true;
        }
        if (key == KEY_F3)
        {
            mDoYaw = 0;
            return true;
        }
        if (key == KEY_F4)
        {
            mDoYaw = 0;
            return true;
        }
        if (key == KEY_F5)
        {
            mDoPitch = 0;
            return true;
        }
        if (key == KEY_F6)
        {
            mDoPitch = 0;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnMouseClick (int button, int state, int x, int y,
    unsigned int)
{
    if (!mUseTrackBall
    ||  button != MOUSE_LEFT_BUTTON
    ||  !mMotionObject)
    {
        return false;
    }

    float mult = 1.0f/(mWidth >= mHeight ? mHeight : mWidth);

    if (state == MOUSE_DOWN)
    {
        // Get the starting point.
        mTrackBallDown = true;
        mSaveRotate = mMotionObject->LocalTransform.GetRotate();
        mXTrack0 = (2*x - mWidth)*mult;
        mYTrack0 = (2*(mHeight - 1 - y) - mHeight)*mult;
    }
    else
    {
        mTrackBallDown = false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnMotion (int button, int x, int y, unsigned int)
{
    if (!mUseTrackBall
    ||  button != MOUSE_LEFT_BUTTON
    ||  !mTrackBallDown
    ||  !mMotionObject)
    {
        return false;
    }

    // Get the ending point.
    float mult = 1.0f/(mWidth >= mHeight ? mHeight : mWidth);
    mXTrack1 = (2*x - mWidth)*mult;
    mYTrack1 = (2*(mHeight - 1 - y) - mHeight)*mult;

    // Update the object's local rotation.
    RotateTrackBall(mXTrack0, mYTrack0, mXTrack1, mYTrack1);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// camera motion
//----------------------------------------------------------------------------
void WindowApplication3::InitializeCameraMotion (float trnSpeed,
    float rotSpeed, float trnSpeedFactor, float rotSpeedFactor)
{
    mCameraMoveable = true;

    mTrnSpeed = trnSpeed;
    mRotSpeed = rotSpeed;
    mTrnSpeedFactor = trnSpeedFactor;
    mRotSpeedFactor = rotSpeedFactor;

    mWorldAxis[0] = mCamera->GetDVector();
    mWorldAxis[1] = mCamera->GetUVector();
    mWorldAxis[2] = mCamera->GetRVector();
}
//----------------------------------------------------------------------------
bool WindowApplication3::MoveCamera ()
{
    if (!mCameraMoveable)
    {
        return false;
    }

    bool bMoved = false;

    if (mUArrowPressed)
    {
        MoveForward();
        bMoved = true;
    }

    if (mDArrowPressed)
    {
        MoveBackward();
        bMoved = true;
    }

    if (mHomePressed)
    {
        MoveUp();
        bMoved = true;
    }

    if (mEndPressed)
    {
        MoveDown();
        bMoved = true;
    }

    if (mLArrowPressed)
    {
        TurnLeft();
        bMoved = true;
    }

    if (mRArrowPressed)
    {
        TurnRight();
        bMoved = true;
    }

    if (mPgUpPressed)
    {
        LookUp();
        bMoved = true;
    }

    if (mPgDnPressed)
    {
        LookDown();
        bMoved = true;
    }

    if (mInsertPressed)
    {
        MoveRight();
        bMoved = true;
    }

    if (mDeletePressed)
    {
        MoveLeft();
        bMoved = true;
    }

    return bMoved;
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveForward ()
{
    APoint pos = mCamera->GetPosition();
    pos += mTrnSpeed*mWorldAxis[0];
    mCamera->SetPosition(pos);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveBackward ()
{
    APoint pos = mCamera->GetPosition();
    pos -= mTrnSpeed*mWorldAxis[0];
    mCamera->SetPosition(pos);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveUp ()
{
    APoint pos = mCamera->GetPosition();
    pos += mTrnSpeed*mWorldAxis[1];
    mCamera->SetPosition(pos);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveDown ()
{
    APoint pos = mCamera->GetPosition();
    pos -= mTrnSpeed*mWorldAxis[1];
    mCamera->SetPosition(pos);
}
//----------------------------------------------------------------------------
void WindowApplication3::TurnLeft ()
{
    HMatrix incr(mWorldAxis[1], mRotSpeed);
    mWorldAxis[0] = incr*mWorldAxis[0];
    mWorldAxis[2] = incr*mWorldAxis[2];

    AVector dVector = incr*mCamera->GetDVector();
    AVector uVector = incr*mCamera->GetUVector();
    AVector rVector = incr*mCamera->GetRVector();
    mCamera->SetAxes(dVector, uVector, rVector);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveLeft ()
{
    APoint pos = mCamera->GetPosition();
    pos -= mTrnSpeed*mWorldAxis[2];
    mCamera->SetPosition(pos);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveRight ()
{
    APoint pos = mCamera->GetPosition();
    pos += mTrnSpeed*mWorldAxis[2];
    mCamera->SetPosition(pos);
}
//----------------------------------------------------------------------------
void WindowApplication3::TurnRight ()
{
    HMatrix incr(mWorldAxis[1], -mRotSpeed);
    mWorldAxis[0] = incr*mWorldAxis[0];
    mWorldAxis[2] = incr*mWorldAxis[2];

    AVector dVector = incr*mCamera->GetDVector();
    AVector uVector = incr*mCamera->GetUVector();
    AVector rVector = incr*mCamera->GetRVector();
    mCamera->SetAxes(dVector, uVector, rVector);
}
//----------------------------------------------------------------------------
void WindowApplication3::LookUp ()
{
    HMatrix incr(mWorldAxis[2], mRotSpeed);

    AVector dVector = incr*mCamera->GetDVector();
    AVector uVector = incr*mCamera->GetUVector();
    AVector rVector = incr*mCamera->GetRVector();
    mCamera->SetAxes(dVector, uVector, rVector);
}
//----------------------------------------------------------------------------
void WindowApplication3::LookDown ()
{
    HMatrix incr(mWorldAxis[2],-mRotSpeed);

    AVector dVector = incr*mCamera->GetDVector();
    AVector uVector = incr*mCamera->GetUVector();
    AVector rVector = incr*mCamera->GetRVector();
    mCamera->SetAxes(dVector, uVector, rVector);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Object motion.
//----------------------------------------------------------------------------
void WindowApplication3::InitializeObjectMotion (Spatial* motionObject)
{
    mMotionObject = motionObject;
}
//----------------------------------------------------------------------------
bool WindowApplication3::MoveObject ()
{
    // The coordinate system in which the rotations are applied is that of
    // the object's parent, if it has one.  The parent's world rotation
    // matrix is R, of which the columns are the coordinate axis directions.
    // Column 0 is "direction", column 1 is "up", and column 2 is "right".
    // If the object does not have a parent, the world coordinate axes are
    // used, in which case the rotation matrix is I, the identity.  Column 0
    // is (1,0,0) and is "direction", column 1 is (0,1,0) and is "up", and
    // column 2 is (0,0,1) and is "right".  This choice is consistent with
    // the use of rotations in the Camera and Light classes to store
    // coordinate axes.
    //
    // Roll is about the "direction" axis, yaw is about the "up" axis, and
    // pitch is about the "right" axis.

    if (!mCameraMoveable || !mMotionObject)
    {
        return false;
    }

    // Check if the object has been moved by the virtual trackball.
    if (mTrackBallDown)
    {
        return true;
    }

    // Check if the object has been moved by the function keys.
    Spatial* parent = mMotionObject->GetParent();
    AVector axis;
    float angle;
    HMatrix rot, incr;

    if (mDoRoll)
    {
        rot = mMotionObject->LocalTransform.GetRotate();

        angle = mDoRoll*mRotSpeed;
        if (parent)
        {
            parent->WorldTransform.GetRotate().GetColumn(0, axis);
        }
        else
        {
            axis = AVector::UNIT_X;
        }

        incr.MakeRotation(axis, angle);
        rot = incr*rot;
        rot.Orthonormalize();
        mMotionObject->LocalTransform.SetRotate(rot);
        return true;
    }

    if (mDoYaw)
    {
        rot = mMotionObject->LocalTransform.GetRotate();

        angle = mDoYaw*mRotSpeed;
        if (parent)
        {
            parent->WorldTransform.GetRotate().GetColumn(1, axis);
        }
        else
        {
            axis = AVector::UNIT_Y;
        }

        incr.MakeRotation(axis, angle);
        rot = incr*rot;
        rot.Orthonormalize();
        mMotionObject->LocalTransform.SetRotate(rot);
        return true;
    }

    if (mDoPitch)
    {
        rot = mMotionObject->LocalTransform.GetRotate();

        angle = mDoPitch*mRotSpeed;
        if (parent)
        {
            parent->WorldTransform.GetRotate().GetColumn(2, axis);
        }
        else
        {
            axis = AVector::UNIT_Z;
        }

        incr.MakeRotation(axis, angle);
        rot = incr*rot;
        rot.Orthonormalize();
        mMotionObject->LocalTransform.SetRotate(rot);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void WindowApplication3::RotateTrackBall (float x0, float y0, float x1,
    float y1)
{
    if ((x0 == x1 && y0 == y1) || !mCamera)
    {
        // Nothing to rotate.
        return;
    }

    // Get the first vector on the sphere.
    float length = Mathf::Sqrt(x0*x0 + y0*y0), invLength, z0, z1;
    if (length > 1.0f)
    {
        // Outside the unit disk, project onto it.
        invLength = 1.0f/length;
        x0 *= invLength;
        y0 *= invLength;
        z0 = 0.0f;
    }
    else
    {
        // Compute point (x0,y0,z0) on negative unit hemisphere.
        z0 = 1.0f - x0*x0 - y0*y0;
        z0 = (z0 <= 0.0f ? 0.0f : Mathf::Sqrt(z0));
    }
    z0 *= -1.0f;

    // Use camera world coordinates, order is (D,U,R), so point is (z,y,x).
    AVector vec0(z0, y0, x0);

    // Get the second vector on the sphere.
    length = Mathf::Sqrt(x1*x1 + y1*y1);
    if (length > 1.0f)
    {
        // Outside unit disk, project onto it.
        invLength = 1.0f/length;
        x1 *= invLength;
        y1 *= invLength;
        z1 = 0.0f;
    }
    else
    {
        // Compute point (x1,y1,z1) on negative unit hemisphere.
        z1 = 1.0f - x1*x1 - y1*y1;
        z1 = (z1 <= 0.0f ? 0.0f : Mathf::Sqrt(z1));
    }
    z1 *= -1.0f;

    // Use camera world coordinates, order is (D,U,R), so point is (z,y,x).
    AVector vec1(z1, y1, x1);

    // Create axis and angle for the rotation.
    AVector axis = vec0.Cross(vec1);
    float dot = vec0.Dot(vec1);
    float angle;
    if (axis.Normalize() > Mathf::ZERO_TOLERANCE)
    {
        angle = Mathf::ACos(dot); 
    }
    else  // Vectors are parallel.
    {
        if (dot < 0.0f)
        {
            // Rotated pi radians.
            invLength = Mathf::InvSqrt(x0*x0 + y0*y0);
            axis.X() = y0*invLength;
            axis.Y() = -x0*invLength;
            axis.Z() = 0.0f;
            angle = Mathf::PI;
        }
        else
        {
            // Rotation by zero radians.
            axis = AVector::UNIT_X;
            angle = 0.0f;
        }
    }

    // Compute the world rotation matrix implied by trackball motion.  The
    // axis vector was computed in camera coordinates.  It must be converted
    // to world coordinates.  Once again, I use the camera ordering (D,U,R).
    AVector worldAxis =
        axis.X()*mCamera->GetDVector() +
        axis.Y()*mCamera->GetUVector() +
        axis.Z()*mCamera->GetRVector();

    HMatrix trackRotate(worldAxis, angle);

    // Compute the new local rotation.  If the object is the root of the
    // scene, the new rotation is simply the *incremental rotation* of the
    // trackball applied *after* the object has been rotated by its old
    // local rotation.  If the object is not the root of the scene, you have
    // to convert the incremental rotation by a change of basis in the
    // parent's coordinate space. 
    const Spatial* parent = mMotionObject->GetParent();
    HMatrix localRot;
    if (parent)
    {
        const HMatrix& parWorRotate = parent->WorldTransform.GetRotate();
        localRot = parWorRotate.TransposeTimes(trackRotate) * parWorRotate *
            mSaveRotate;
    }
    else
    {
        localRot = trackRotate*mSaveRotate;
    }
    localRot.Orthonormalize();
    mMotionObject->LocalTransform.SetRotate(localRot);
}
//----------------------------------------------------------------------------
