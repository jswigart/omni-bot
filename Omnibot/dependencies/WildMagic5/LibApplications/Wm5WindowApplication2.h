// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/06/27)

#ifndef WM5WINDOWAPPLICATION2_H
#define WM5WINDOWAPPLICATION2_H

#include "Wm5WindowApplication.h"

namespace Wm5
{

class WindowApplication2 : public WindowApplication
{
public:
    // Construction and destruction.
    WindowApplication2 (const char* windowTitle, int xPosition, int yPosition,
        int width, int height, const Float4& clearColor);

    virtual ~WindowApplication2 ();

    // Event callbacks.
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnResize (int width, int height);
    virtual void OnDisplay ();

    // Allows you to do additional drawing after the screen polygon is drawn.
    // Screen overlays should use the Renderer calls and not access the
    // mScreen array directly.
    virtual void ScreenOverlay ();

    void ClearScreen ();

    // TODO:  Added an alpha channel to get 32-bits per pixel for performance
    // in drawing on the GPU.  A change in class name will affect many
    // applications, so that will be deferred until closer to shipping WM5.6.
    class ColorRGB
    {
    public:
        inline ColorRGB (unsigned char red = 0, unsigned char green = 0,
            unsigned char blue = 0, unsigned char alpha = 255)
        {
            r = red;
            g = green;
            b = blue;
            a = alpha;
        }

        inline bool operator== (ColorRGB color) const
        {
            return b == color.b
                && g == color.g
                && r == color.r
                && a == color.a;
        }

        inline bool operator!= (ColorRGB color) const
        {
            return b != color.b
                || g != color.g
                || r != color.r
                || a != color.a;
        }

        unsigned char b, g, r, a;
    };

    // For right-handed drawing.  You still draw to the left-handed screen,
    // but immediately before drawing, the screen is copied into another
    // buffer with the rows reversed.  You need only call DoFlip(true) once
    // for an application.  The default is 'false'.
    void DoFlip (bool doFlip);

    // The drawing routines listed below perform range checking on any (x,y)
    // {Set/Get}Pixel call when mClampToWindow is 'true'.  Each pixel is
    // processed only when in range.
    bool& ClampToWindow ();

    // Set the pixel at location (x,y) to the specified color.
    void SetPixel (int x, int y, ColorRGB color);

    // Set the pixels (x',y') for x-thick <= x' <= x+thick and
    // y-thick <= y' <= y+thick.
    void SetThickPixel (int x, int y, int thick, ColorRGB color);

    // Get the pixel color at location (x,y).
    ColorRGB GetPixel (int x, int y);

    // Use Bresenham's algorithm to draw the line from (x0,y0) to (x1,y1)
    // using the specified color for the drawn pixels.  The algorithm is
    // biased in that the pixels set by DrawLine(x0,y0,x1,y1) are not
    // necessarily the same as those set by DrawLine(x1,y1,x0,y0).
    // TODO: Implement the midpoint algorithm to avoid the bias.
    void DrawLine (int x0, int y0, int x1, int y1, ColorRGB color);

    // Draw an axis-aligned rectangle using the specified color.  The
    // 'solid' parameter indicates whether or not to fill the rectangle.
    void DrawRectangle (int xMin, int yMin, int xMax, int yMax,
        ColorRGB color, bool solid = false);

    // Use Bresenham's algorithm to draw the circle centered at
    // (xCenter,yCenter) with the specified 'radius' and using the
    // specified color.  The 'solid' parameter indicates whether or not
    // to fill the circle.
    void DrawCircle (int xCenter, int yCenter, int radius, ColorRGB color,
        bool solid = false);

    // Flood-fill a region whose pixels are of color 'backColor' by
    // changing their color to 'foreColor'.  The fill treats the screen
    // as 4-connected; that is, after (x,y) is visited, then (x-1,y),
    // (x+1,y), (x,y-1), and (x,y+1) are visited (as long as they are in
    // the screen boundary).  The function simulates recursion by using
    // stacks, which avoids the expense of true recursion and the potential
    // to overflow the calling stack.
    void Fill (int x, int y, ColorRGB foreColor, ColorRGB backColor);

protected:
    int mScreenWidth, mScreenHeight;
    ColorRGB* mScreen;
    bool mClampToWindow;
    bool mDoFlip;
};

}

#endif
