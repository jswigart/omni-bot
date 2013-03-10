// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2012/07/01)

#include "Wm5ApplicationPCH.h"
#include "Wm5WindowApplication.h"
#include <windows.h>
using namespace Wm5;

#ifdef WM5_USE_DX9
#include "Wm5Dx9RendererInput.h"
#include "Wm5Dx9RendererData.h"
#endif

#ifdef WM5_USE_OPENGL
#include "Wm5WglRendererInput.h"
#include "Wm5WglRendererData.h"
#endif

const int WindowApplication::KEY_ESCAPE = VK_ESCAPE;
const int WindowApplication::KEY_LEFT_ARROW = VK_LEFT;
const int WindowApplication::KEY_RIGHT_ARROW = VK_RIGHT;
const int WindowApplication::KEY_UP_ARROW = VK_UP;
const int WindowApplication::KEY_DOWN_ARROW = VK_DOWN;
const int WindowApplication::KEY_HOME = VK_HOME;
const int WindowApplication::KEY_END = VK_END;
const int WindowApplication::KEY_PAGE_UP = VK_PRIOR;
const int WindowApplication::KEY_PAGE_DOWN = VK_NEXT;
const int WindowApplication::KEY_INSERT = VK_INSERT;
const int WindowApplication::KEY_DELETE = VK_DELETE;
const int WindowApplication::KEY_F1 = VK_F1;
const int WindowApplication::KEY_F2 = VK_F2;
const int WindowApplication::KEY_F3 = VK_F3;
const int WindowApplication::KEY_F4 = VK_F4;
const int WindowApplication::KEY_F5 = VK_F5;
const int WindowApplication::KEY_F6 = VK_F6;
const int WindowApplication::KEY_F7 = VK_F7;
const int WindowApplication::KEY_F8 = VK_F8;
const int WindowApplication::KEY_F9 = VK_F9;
const int WindowApplication::KEY_F10 = VK_F10;
const int WindowApplication::KEY_F11 = VK_F11;
const int WindowApplication::KEY_F12 = VK_F12;
const int WindowApplication::KEY_BACKSPACE = VK_BACK;
const int WindowApplication::KEY_TAB = VK_TAB;
const int WindowApplication::KEY_ENTER = VK_RETURN;
const int WindowApplication::KEY_RETURN = VK_RETURN;

const int WindowApplication::KEY_SHIFT = VK_SHIFT;
const int WindowApplication::KEY_CONTROL = VK_CONTROL;
const int WindowApplication::KEY_ALT = 0;      // not currently handled
const int WindowApplication::KEY_COMMAND = 0;  // not currently handled

const int WindowApplication::MOUSE_LEFT_BUTTON = 0;
const int WindowApplication::MOUSE_MIDDLE_BUTTON = 1;
const int WindowApplication::MOUSE_RIGHT_BUTTON = 2;
const int WindowApplication::MOUSE_UP = 0;
const int WindowApplication::MOUSE_DOWN = 1;

const int WindowApplication::MODIFIER_CONTROL = MK_CONTROL;
const int WindowApplication::MODIFIER_LBUTTON = MK_LBUTTON;
const int WindowApplication::MODIFIER_MBUTTON = MK_MBUTTON;
const int WindowApplication::MODIFIER_RBUTTON = MK_RBUTTON;
const int WindowApplication::MODIFIER_SHIFT = MK_SHIFT;

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

static bool gsIgnoreWindowDestroy = false;

//----------------------------------------------------------------------------
void WindowApplication::SetMousePosition (int x, int y)
{
    HWND handle = (HWND)IntToPtr(mWindowID);
    POINT point;
    point.x = (LONG)x;
    point.y = (LONG)y;
    ClientToScreen(handle, &point);
    SetCursorPos(point.x, point.y);
}
//----------------------------------------------------------------------------
void WindowApplication::GetMousePosition (int& x, int& y) const
{
    HWND handle = (HWND)IntToPtr(mWindowID);
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(handle, &point);
    x = (int)point.x;
    y = (int)point.y;
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* text) const
{
    if (!text || strlen(text) == 0)
    {
        return 0;
    }

    HWND handle = (HWND)IntToPtr(mWindowID);
    HDC context = GetDC(handle);
    SIZE size;
    GetTextExtentPoint32(context, text, (int)strlen(text),&size);
    ReleaseDC(handle, context);
    return (int)size.cx;
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharacterWidth (const char character) const
{
    HWND handle = (HWND)IntToPtr(mWindowID);
    HDC context = GetDC(handle);
    SIZE size;
    GetTextExtentPoint32(context, &character, 1, &size);
    ReleaseDC(handle, context);
    return (int)size.cx;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    HWND handle = (HWND)IntToPtr(mWindowID);
    HDC context = GetDC(handle);
    TEXTMETRIC metric;
    GetTextMetrics(context, &metric);
    ReleaseDC(handle, context);
    return (int)metric.tmHeight;
}
//----------------------------------------------------------------------------
LRESULT CALLBACK MsWindowEventHandler (HWND handle, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (!theApp || !theApp->GetWindowID())
    {
        return DefWindowProc(handle, message, wParam, lParam);
    }

    switch (message) 
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(handle, &ps);
            theApp->OnDisplay();
            EndPaint(handle, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
        {
            // This tells Windows not to erase the background (and that the
            // application is doing so).
            return 1;
        }
        case WM_MOVE:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            theApp->OnMove(xPos, yPos);
            return 0;
        }
        case WM_SIZE:
        {
            int width = (int)(LOWORD(lParam));
            int height = (int)(HIWORD(lParam));
            theApp->OnResize(width, height);
            return 0;
        }
        case WM_CHAR:
        {
            unsigned char key = (unsigned char)(char)wParam;

            // Quit the application if the KEY_TERMINATE key is pressed.
            if (key == theApp->KEY_TERMINATE)
            {
                PostQuitMessage(0);
                return 0;
            }

            // Get the cursor position in client coordinates.
            POINT point;
            GetCursorPos(&point);
            ScreenToClient(handle, &point);
            int xPos = (int)point.x;
            int yPos = (int)point.y;

            theApp->OnKeyDown(key, xPos, yPos);
            return 0;
        }
        case WM_KEYDOWN:
        {
            int virtKey = (int)wParam;

            // Get cursor position client coordinates.
            POINT point;
            GetCursorPos(&point);
            ScreenToClient(handle, &point);
            int xPos = (int)point.x;
            int yPos = (int)point.y;

            if ((VK_F1 <= virtKey && virtKey <= VK_F12)
            ||  (VK_PRIOR <= virtKey && virtKey <= VK_DOWN)
            ||  (virtKey == VK_INSERT) || (virtKey == VK_DELETE)
            ||  (virtKey == VK_SHIFT) || (virtKey == VK_CONTROL))
            {
                theApp->OnSpecialKeyDown(virtKey, xPos, yPos);
            }
            return 0;
        }
        case WM_KEYUP:
        {
            int virtKey = (int)wParam;

            // get the cursor position in client coordinates
            POINT point;
            GetCursorPos(&point);
            ScreenToClient(handle, &point);
            int xPos = (int)point.x;
            int yPos = (int)point.y;

            if ((VK_F1 <= virtKey && virtKey <= VK_F12)
            ||  (VK_PRIOR <= virtKey && virtKey <= VK_DOWN)
            ||  (virtKey == VK_INSERT) || (virtKey == VK_DELETE)
            ||  (virtKey == VK_SHIFT) || (virtKey == VK_CONTROL))
            {
                theApp->OnSpecialKeyUp(virtKey, xPos, yPos);
            }
            else
            {
                theApp->OnKeyUp((unsigned char)virtKey, xPos, yPos);
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            theApp->OnMouseClick(WindowApplication::MOUSE_LEFT_BUTTON,
                WindowApplication::MOUSE_DOWN, xPos, yPos,
                (unsigned int)wParam);
            return 0;
        }
        case WM_LBUTTONUP:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            theApp->OnMouseClick(WindowApplication::MOUSE_LEFT_BUTTON,
                WindowApplication::MOUSE_UP, xPos, yPos,
                (unsigned int)wParam);
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            theApp->OnMouseClick(WindowApplication::MOUSE_MIDDLE_BUTTON,
                WindowApplication::MOUSE_DOWN, xPos, yPos,
                (unsigned int)wParam);
            return 0;
        }
        case WM_MBUTTONUP:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            theApp->OnMouseClick(WindowApplication::MOUSE_MIDDLE_BUTTON,
                WindowApplication::MOUSE_UP, xPos, yPos,
                (unsigned int)wParam);
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            theApp->OnMouseClick(WindowApplication::MOUSE_RIGHT_BUTTON,
                WindowApplication::MOUSE_DOWN, xPos, yPos,
                (unsigned int)wParam);
            return 0;
        }
        case WM_RBUTTONUP:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            theApp->OnMouseClick(WindowApplication::MOUSE_RIGHT_BUTTON,
                WindowApplication::MOUSE_UP, xPos, yPos,
                (unsigned int)wParam);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));

            int button = -1;
            if (wParam & MK_LBUTTON)
            {
                button = WindowApplication::MOUSE_LEFT_BUTTON;
            }
            else if (wParam & MK_MBUTTON)
            {
                button = WindowApplication::MOUSE_MIDDLE_BUTTON;
            }
            else if (wParam & MK_RBUTTON)
            {
                button = WindowApplication::MOUSE_RIGHT_BUTTON;
            }

            if (button >= 0)
            {
                theApp->OnMotion(button, xPos, yPos, (unsigned int)wParam);
            }
            else
            {
                theApp->OnPassiveMotion(xPos, yPos);
            }

            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            short sWParam = (short)(HIWORD(wParam));
            int delta = ((int)sWParam)/WHEEL_DELTA;
            int xPos = (int)(LOWORD(lParam));
            int yPos = (int)(HIWORD(lParam));
            unsigned int modifiers = (unsigned int)(LOWORD(wParam));
            theApp->OnMouseWheel(delta, xPos, yPos, modifiers);
            return 0;
        }
        case WM_DESTROY:
        {
            // The DestroyWindow call when recreating the window for
            // multisampling causes the application to terminate.  It is
            // not clear why the same problem did not occur in WM4.
            if (!gsIgnoreWindowDestroy)
            {
                PostQuitMessage(0);
            }
            gsIgnoreWindowDestroy = false;
            return 0;
        }
    }

    return DefWindowProc(handle, message, wParam, lParam);
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int, char**)
{
    WindowApplication* theApp = (WindowApplication*)TheApplication;
    theApp->KEY_TERMINATE = WindowApplication::KEY_ESCAPE;

#ifdef WM5_USE_DX9
    // DirectX uses a projection matrix for depth in [0,1].
    Camera::SetDefaultDepthType(Camera::PM_DEPTH_ZERO_TO_ONE);
#endif

#ifdef WM5_USE_OPENGL
    // OpenGL uses a projection matrix for depth in [-1,1].
    Camera::SetDefaultDepthType(Camera::PM_DEPTH_MINUS_ONE_TO_ONE);
#endif

    // Allow work to be done before the window is created.
    if (!theApp->OnPrecreate())
    {
        return -1;
    }

    // === Create the window for rendering. ===

    // Register the window class.
    static char sWindowClass[] = "Wild Magic 5 Application";
    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = MsWindowEventHandler;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = 0;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = sWindowClass;
    wc.lpszMenuName  = 0;
    RegisterClass(&wc);

    DWORD dwStyle;
    if (mAllowResize)
    {
        dwStyle = WS_OVERLAPPEDWINDOW;
    }
    else
    {
        // This removes WS_THICKFRAME and WS_MAXIMIZEBOX, both of which allow
        // resizing of windows.
        dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    }

    // Require the window to have the specified client area.
    RECT rect = { 0, 0, theApp->GetWidth()-1, theApp->GetHeight()-1 };
    AdjustWindowRect(&rect, dwStyle, FALSE);

    // Create the application window.
    HWND handle = CreateWindow(sWindowClass, theApp->GetWindowTitle(),
        dwStyle, theApp->GetXPosition(), theApp->GetYPosition(),
        rect.right - rect.left + 1, rect.bottom - rect.top + 1, 0, 0, 0, 0);

    // Save the handle as an 'int' for portable handle storage.
    theApp->SetWindowID(PtrToInt(handle));

    // ===

#ifdef WM5_USE_DX9
    // Create the device for rendering.
    RendererInput input;
    input.mWindowHandle = handle;
    input.mDriver = Direct3DCreate9(D3D_SDK_VERSION);
    assertion(input.mDriver != 0, "Failed to create Direct3D9\n");
    mRenderer = new0 Renderer(input, theApp->GetWidth(), theApp->GetHeight(),
        mColorFormat, mDepthStencilFormat, mNumMultisamples);
#endif

#ifdef WM5_USE_OPENGL
    // The pixelFormat variable is used to support creation of a window that
    // supports multisampling.  This process requires creating a normal window,
    // and then querying whether the renderer supports multisampling.  The
    // renderer creates a device context for the window which we then need to
    // create a second window that supports multisampling.  The device context
    // rendererDC is set by the renderer during the process.
    RendererInput input;
    input.mWindowHandle = handle;
    input.mPixelFormat = 0;
    input.mRendererDC = 0;
    input.mDisableVerticalSync = true;
    mRenderer = new0 Renderer(input, theApp->GetWidth(), theApp->GetHeight(),
        mColorFormat, mDepthStencilFormat, mNumMultisamples);

    // To determine whether multisampling is supported, it is necessary to
    // create a window, an OpenGL context, and query the driver for the
    // multisampling extensions.  If it is, a new window must be created
    // because the renderer creation involves SetPixelFormat(...) that can
    // be called only once for a window.
    int numMultisamples = mRenderer->GetNumMultisamples();
    if (numMultisamples > 0)
    {
        int attributes[256], pos = 0;
        attributes[pos++] = WGL_SUPPORT_OPENGL_ARB;
        attributes[pos++] = 1;
        attributes[pos++] = WGL_DRAW_TO_WINDOW_ARB;
        attributes[pos++] = 1;
        attributes[pos++] = WGL_ACCELERATION_ARB;
        attributes[pos++] = WGL_FULL_ACCELERATION_ARB;
        attributes[pos++] = WGL_PIXEL_TYPE_ARB;
        attributes[pos++] = WGL_TYPE_RGBA_ARB;
        attributes[pos++] = WGL_RED_BITS_ARB;
        attributes[pos++] = 8;
        attributes[pos++] = WGL_GREEN_BITS_ARB;
        attributes[pos++] = 8;
        attributes[pos++] = WGL_BLUE_BITS_ARB;
        attributes[pos++] = 8;
        attributes[pos++] = WGL_ALPHA_BITS_ARB;
        attributes[pos++] = 8;
        attributes[pos++] = WGL_DEPTH_BITS_ARB;
        attributes[pos++] = 24;
        attributes[pos++] = WGL_STENCIL_BITS_ARB;
        attributes[pos++] = 8;
        attributes[pos++] = WGL_DOUBLE_BUFFER_ARB;
        attributes[pos++] = 1;
        attributes[pos++] = WGL_SAMPLE_BUFFERS_ARB;
        attributes[pos++] = 1;
        attributes[pos++] = WGL_SAMPLES_ARB;
        attributes[pos++] = numMultisamples;
        attributes[pos++] = 0; // list is zero-terminated

        unsigned int numFormats = 0;
        BOOL successful = wglChoosePixelFormatARB(input.mRendererDC,
            attributes, 0, 1, &input.mPixelFormat, &numFormats);
        if (successful && numFormats > 0)
        {
            // The card supports multisampling with the requested number of
            // samples.  Recreate the window and renderer.
            delete0(mRenderer);

            gsIgnoreWindowDestroy = true;
            DestroyWindow(handle);

            handle = CreateWindow(sWindowClass, theApp->GetWindowTitle(),
                WS_OVERLAPPEDWINDOW, theApp->GetXPosition(),
                theApp->GetYPosition(), rect.right - rect.left + 1,
                rect.bottom - rect.top + 1, 0, 0, 0, 0);

            theApp->SetWindowID(PtrToInt(handle));

            input.mWindowHandle = handle;
            mRenderer = new0 Renderer(input, theApp->GetWidth(),
                theApp->GetHeight(), mColorFormat, mDepthStencilFormat,
                mNumMultisamples);
        }
    }
#endif

    if (theApp->OnInitialize())
    {
        // The default OnPreidle() clears the buffers.  Allow the application
        // to fill them before the window is shown and before the event loop
        // starts.
        theApp->OnPreidle();

        // Display the window.
        ShowWindow(handle, SW_SHOW);
        UpdateWindow(handle);

        // Start the message pump.
        bool applicationRunning = true;
        while (applicationRunning)
        {
            MSG msg;
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    applicationRunning = false;
                    continue;
                }

                HACCEL accel = 0;
                if (!TranslateAccelerator(handle, accel, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else
            {
                theApp->OnIdle();
            }
        }
    }
    theApp->OnTerminate();
    delete0(mRenderer);

#ifdef WM5_USE_DX9
    input.mDriver->Release();
#endif

    return 0;
}
//----------------------------------------------------------------------------
