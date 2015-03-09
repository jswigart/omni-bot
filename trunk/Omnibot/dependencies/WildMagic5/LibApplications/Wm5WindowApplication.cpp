// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/08/13)

#include "Wm5ApplicationPCH.h"
#include "Wm5WindowApplication.h"
using namespace Wm5;

//----------------------------------------------------------------------------
WindowApplication::WindowApplication (const char* windowTitle, int xPosition,
    int yPosition, int width, int height, const Float4& clearColor)
    :
    mWindowTitle(windowTitle),
    mXPosition(xPosition),
    mYPosition(yPosition),
    mWidth(width),
    mHeight(height),
    mClearColor(clearColor),
    mAllowResize(true),
    mWindowID(0),
    mRenderer(0),
    mLastTime(-1.0),
    mAccumulatedTime(0.0),
    mFrameRate(0.0),
    mFrameCount(0),
    mAccumulatedFrameCount(0),
    mTimer(30),
    mMaxTimer(30)
{
    ThePath = WM5Path + std::string(windowTitle) + "/";

    // NOTE.  The derived-class constructors may override these settings.
    mColorFormat = Texture::TF_A8R8G8B8;
    mDepthStencilFormat = Texture::TF_D24S8;
    mNumMultisamples = 0;
}
//----------------------------------------------------------------------------
WindowApplication::~WindowApplication ()
{
}
//----------------------------------------------------------------------------
int WindowApplication::Run (int numArguments, char** arguments)
{
    WindowApplication* theApp = (WindowApplication*)TheApplication;
    return theApp->Main(numArguments, arguments);
}
//----------------------------------------------------------------------------
bool WindowApplication::OnInitialize ()
{
    // The device is created in the platform-dependent code in the
    // WindowApplication::Main function before OnInitialize is called.  Thus,
    // at this point the device state may be modified.
    mRenderer->SetClearColor(mClearColor);
    return true;
}
//----------------------------------------------------------------------------
void WindowApplication::OnTerminate ()
{
}
//----------------------------------------------------------------------------
void WindowApplication::OnMove (int x, int y)
{
    mXPosition = x;
    mYPosition = y;
}
//----------------------------------------------------------------------------
void WindowApplication::OnResize (int width, int height)
{
    if (width > 0 && height > 0)
    {
        if (mRenderer)
        {
            mRenderer->Resize(width, height);
        }

        mWidth = width;
        mHeight = height;
    }
}
//----------------------------------------------------------------------------
bool WindowApplication::OnKeyDown (unsigned char key, int, int)
{
    if (key == '?')
    {
        ResetTime();
        return true;
    };

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Performance measurements.
//----------------------------------------------------------------------------
void WindowApplication::ResetTime ()
{
    mLastTime = -1.0;
}
//----------------------------------------------------------------------------
void WindowApplication::MeasureTime ()
{
    // start performance measurements
    if (mLastTime == -1.0)
    {
        mLastTime = GetTimeInSeconds();
        mAccumulatedTime = 0.0;
        mFrameRate = 0.0;
        mFrameCount = 0;
        mAccumulatedFrameCount = 0;
        mTimer = mMaxTimer;
    }

    // accumulate the time only when the miniature time allows it
    if (--mTimer == 0)
    {
        double dCurrentTime = GetTimeInSeconds();
        double dDelta = dCurrentTime - mLastTime;
        mLastTime = dCurrentTime;
        mAccumulatedTime += dDelta;
        mAccumulatedFrameCount += mFrameCount;
        mFrameCount = 0;
        mTimer = mMaxTimer;
    }
}
//----------------------------------------------------------------------------
void WindowApplication::UpdateFrameCount ()
{
    ++mFrameCount;
}
//----------------------------------------------------------------------------
void WindowApplication::DrawFrameRate (int x, int y, const Float4& color)
{
    if (mAccumulatedTime > 0.0)
    {
        mFrameRate = mAccumulatedFrameCount/mAccumulatedTime;
    }
    else
    {
        mFrameRate = 0.0;
    }

    char message[256];
    sprintf(message, "fps: %.1lf", mFrameRate);
    mRenderer->Draw(x, y, color, message);
}
//----------------------------------------------------------------------------
