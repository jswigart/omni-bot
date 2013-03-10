// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/06/27)

#include "Wm5ApplicationPCH.h"
#include "Wm5WindowApplication2.h"
using namespace Wm5;

//----------------------------------------------------------------------------
WindowApplication2::WindowApplication2 (const char* windowTitle,
    int xPosition, int yPosition, int width, int height,
    const Float4& clearColor)
    :
    // Rows are required to be multiple of 4 bytes, so width is adjusted.
    WindowApplication(windowTitle, xPosition, yPosition, width-(width % 4),
        height, clearColor),
    mScreenWidth(0),
    mScreenHeight(0),
    mScreen(0),
    mClampToWindow(true),
    mDoFlip(false)
{
}
//----------------------------------------------------------------------------
WindowApplication2::~WindowApplication2 ()
{
}
//----------------------------------------------------------------------------
bool WindowApplication2::OnInitialize ()
{
    if (!WindowApplication::OnInitialize())
    {
        return false;
    }

    // The RGBA screen pixels.
    mScreenWidth = GetWidth();
    mScreenHeight = GetHeight();
    mScreen = new1<ColorRGB>(mScreenWidth*mScreenHeight);
    ClearScreen();
    return true;
}
//----------------------------------------------------------------------------
void WindowApplication2::OnTerminate ()
{
    delete1(mScreen);
    WindowApplication::OnTerminate();
}
//----------------------------------------------------------------------------
void WindowApplication2::OnDisplay ()
{
    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw((const unsigned char*)mScreen, mDoFlip);
        ScreenOverlay();
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::OnResize (int width, int height)
{
    width = width - (width % 4);
    WindowApplication::OnResize(width, height);
    if (width*height <= 0)
    {
        return;
    }

    if (width != mScreenWidth || height != mScreenHeight)
    {
        delete1(mScreen);
        mScreenWidth = width;
        mScreenHeight = height;
        mScreen = new1<ColorRGB>(mScreenWidth*mScreenHeight);
        ClearScreen();
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::ScreenOverlay ()
{
    // Stub for derived classes.
}
//----------------------------------------------------------------------------
void WindowApplication2::ClearScreen ()
{
    unsigned char r = (unsigned char)(255.0f*mClearColor[0]);
    unsigned char g = (unsigned char)(255.0f*mClearColor[1]);
    unsigned char b = (unsigned char)(255.0f*mClearColor[2]);
    unsigned char a = (unsigned char)(255.0f*mClearColor[3]);
    ColorRGB color(r, g, b, a);
    ColorRGB* current = mScreen;
    const int imax = mWidth*mHeight;
    for (int i = 0; i < imax; ++i, ++current)
    {
        *current = color;
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::DoFlip (bool doFlip)
{
    mDoFlip = doFlip;
}
//----------------------------------------------------------------------------
bool& WindowApplication2::ClampToWindow ()
{
    return mClampToWindow;
}
//----------------------------------------------------------------------------
void WindowApplication2::SetPixel (int x, int y, ColorRGB color)
{
    if (mClampToWindow)
    {
        if (0 <= x && x < mWidth && 0 <= y && y < mHeight)
        {
            mScreen[x + mWidth*y] = color;
        }
    }
    else
    {
        mScreen[x + mWidth*y] = color;
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::SetThickPixel (int x, int y, int thick,
    ColorRGB color)
{
    for (int dy = -thick; dy <= thick; ++dy)
    {
        for (int dx = -thick; dx <= thick; ++dx)
        {
            SetPixel(x + dx, y + dy, color);
        }
    }
}
//----------------------------------------------------------------------------
WindowApplication2::ColorRGB WindowApplication2::GetPixel (int x, int y)
{
    if (mClampToWindow)
    {
        if (0 <= x && x < mWidth && 0 <= y && y < mHeight)
        {
            return mScreen[x + mWidth*y];
        }
        else
        {
            return ColorRGB(0, 0, 0, 0);
        }
    }
    else
    {
        return mScreen[x + mWidth*y];
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::DrawLine (int x0, int y0, int x1, int y1,
    ColorRGB color)
{
    int x = x0, y = y0;

    // The direction of the line segment.
    int dx = x1-x0, dy = y1-y0;

    // Increment or decrement depending on the direction of the line.
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    // Decision parameters for pixel selection.
    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }
    int ax = 2*dx, ay = 2*dy;
    int decx, decy;

    // Determine the largest direction component and single-step using the
    // related variable.
    int maxValue = dx, var = 0;
    if (dy > maxValue)
    {
        var = 1;
    }

    // Traverse the line segment using Bresenham's algorithm.
    switch (var)
    {
    case 0:  // Single-step in the x-direction.
        decy = ay - dx;
        for (/**/; /**/; x += sx, decy += ay)
        {
            // Process the pixel.
            SetPixel(x, y, color);

            // Take the Bresenham step.
            if (x == x1)
            {
                break;
            }
            if (decy >= 0)
            {
                decy -= ax;
                y += sy;
            }
        }
        break;
    case 1:  // Single-step in the y-direction.
        decx = ax - dy;
        for (/**/; /**/; y += sy, decx += ax)
        {
            // Process the pixel.
            SetPixel(x, y, color);

            // Take the Bresenham step.
            if (y == y1)
            {
                break;
            }
            if (decx >= 0)
            {
                decx -= ay;
                x += sx;
            }
        }
        break;
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::DrawRectangle (int xMin, int yMin, int xMax,
    int yMax, ColorRGB color, bool solid)
{
    if (xMin >= mWidth || xMax < 0 || yMin >= mHeight || yMax < 0)
    {
        // rectangle not visible
        return;
    }

    int x, y;

    if (solid)
    {
        for (y = yMin; y <= yMax; ++y)
        {
            for (x = xMin; x <= xMax; ++x)
            {
                SetPixel(x, y, color);
            }
        }
    }
    else
    {
        for (x = xMin; x <= xMax; ++x)
        {
            SetPixel(x, yMin, color);
            SetPixel(x, yMax, color);
        }
        for (y = yMin + 1; y <= yMax - 1; ++y)
        {
            SetPixel(xMin, y, color);
            SetPixel(xMax, y, color);
        }
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::DrawCircle (int xCenter, int yCenter, int radius,
    ColorRGB color, bool solid)
{
    int x, y, dec;

    if (solid)
    {
        int xValue, yMin, yMax, i;
        for (x = 0, y = radius, dec = 3 - 2*radius; x <= y; ++x)
        {
            xValue = xCenter + x;
            yMin = yCenter - y;
            yMax = yCenter + y;
            for (i = yMin; i <= yMax; ++i)
            {
                SetPixel(xValue, i, color);
            }

            xValue = xCenter - x;
            for (i = yMin; i <= yMax; ++i)
            {
                SetPixel(xValue, i, color);
            }

            xValue = xCenter + y;
            yMin = yCenter - x;
            yMax = yCenter + x;
            for (i = yMin; i <= yMax; ++i)
            {
                SetPixel(xValue, i, color);
            }

            xValue = xCenter - y;
            for (i = yMin; i <= yMax; ++i)
            {
                SetPixel(xValue, i, color);
            }

            if (dec >= 0)
            {
                dec += -4*(y--) + 4;
            }
            dec += 4*x + 6;
        }
    }
    else
    {
        for (x = 0, y = radius, dec = 3-2*radius; x <= y; ++x)
        {
            SetPixel(xCenter + x, yCenter + y, color);
            SetPixel(xCenter + x, yCenter - y, color);
            SetPixel(xCenter - x, yCenter + y, color);
            SetPixel(xCenter - x, yCenter - y, color);
            SetPixel(xCenter + y, yCenter + x, color);
            SetPixel(xCenter + y, yCenter - x, color);
            SetPixel(xCenter - y, yCenter + x, color);
            SetPixel(xCenter - y, yCenter - x, color);

            if (dec >= 0)
            {
                dec += -4*(y--) + 4;
            }
            dec += 4*x + 6;
        }
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::Fill (int x, int y, ColorRGB foreColor,
    ColorRGB backColor)
{
    // Allocate the maximum amount of space needed.  If you prefer less, you
    // need to modify this data structure to allow for dynamic reallocation
    // when it is needed.  An empty stack has top == -1.
    int xMax = mWidth, yMax = mHeight;
    int stackSize = xMax*yMax;
    int* xStack = new1<int>(stackSize);
    int* yStack = new1<int>(stackSize);

    // Push the seed point onto the stack if it has the background color.  All
    // points pushed onto stack have background color backColor.
    int top = 0;
    xStack[top] = x;
    yStack[top] = y;

    while (top >= 0)  // The stack is not empty.
    {
        // Read the top of the stack.  Do not pop it since we need to return
        // to this top value later to restart the fill in a different
        // direction.
        x = xStack[top];
        y = yStack[top];

        // Fill the pixel.
        SetPixel(x, y, foreColor);

        int xp1 = x+1;
        if (xp1 < xMax && GetPixel(xp1, y) == backColor)
        {
            // Push the pixel with the background color.
            top++;
            xStack[top] = xp1;
            yStack[top] = y;
            continue;
        }

        int xm1 = x-1;
        if (0 <= xm1 && GetPixel(xm1, y) == backColor)
        {
            // Push the pixel with the background color.
            top++;
            xStack[top] = xm1;
            yStack[top] = y;
            continue;
        }

        int yp1 = y+1;
        if (yp1 < yMax && GetPixel(x, yp1) == backColor)
        {
            // Push the pixel with the background color.
            top++;
            xStack[top] = x;
            yStack[top] = yp1;
            continue;
        }

        int ym1 = y-1;
        if (0 <= ym1 && GetPixel(x, ym1) == backColor)
        {
            // Push the pixel with the background color.
            top++;
            xStack[top] = x;
            yStack[top] = ym1;
            continue;
        }

        // We are done in all directions, so pop and return to search other
        // directions.
        top--;
    }

    delete1(xStack);
    delete1(yStack);
}
//----------------------------------------------------------------------------
