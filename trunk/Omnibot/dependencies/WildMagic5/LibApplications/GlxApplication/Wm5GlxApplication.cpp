// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

#include "Wm5ApplicationPCH.h"
#include "Wm5WindowApplication.h"
#include "Wm5GlxRendererInput.h"
#include "Wm5GlxRendererData.h"
using namespace Wm5;

const int WindowApplication::KEY_ESCAPE             = 0x1B;
const int WindowApplication::KEY_HOME               = 0x95;
const int WindowApplication::KEY_LEFT_ARROW         = 0x96;
const int WindowApplication::KEY_UP_ARROW           = 0x97;
const int WindowApplication::KEY_RIGHT_ARROW        = 0x98;
const int WindowApplication::KEY_DOWN_ARROW         = 0x99;
const int WindowApplication::KEY_PAGE_UP            = 0x9A;
const int WindowApplication::KEY_PAGE_DOWN          = 0x9B;
const int WindowApplication::KEY_END                = 0x9C;
const int WindowApplication::KEY_INSERT             = 0x9E;
const int WindowApplication::KEY_DELETE             = 0x9F;
const int WindowApplication::KEY_F1                 = 0xBE;
const int WindowApplication::KEY_F2                 = 0xBF;
const int WindowApplication::KEY_F3                 = 0xC0;
const int WindowApplication::KEY_F4                 = 0xC1;
const int WindowApplication::KEY_F5                 = 0xC2;
const int WindowApplication::KEY_F6                 = 0xC3;
const int WindowApplication::KEY_F7                 = 0xC4;
const int WindowApplication::KEY_F8                 = 0xC5;
const int WindowApplication::KEY_F9                 = 0xC6;
const int WindowApplication::KEY_F10                = 0xC7;
const int WindowApplication::KEY_F11                = 0xC8;
const int WindowApplication::KEY_F12                = 0xC9;
const int WindowApplication::KEY_BACKSPACE          = 0x08;
const int WindowApplication::KEY_TAB                = 0x09;
const int WindowApplication::KEY_ENTER              = 0x0D;
const int WindowApplication::KEY_RETURN             = 0x0D;

const int WindowApplication::KEY_SHIFT              = 0xE1;  // L-shift
const int WindowApplication::KEY_CONTROL            = 0xE3;  // L-ctrl
const int WindowApplication::KEY_ALT                = 0xE9;  // L-alt
const int WindowApplication::KEY_COMMAND            = 0xEB;  // L-command

const int WindowApplication::MOUSE_LEFT_BUTTON      = 0x0001;
const int WindowApplication::MOUSE_MIDDLE_BUTTON    = 0x0002;
const int WindowApplication::MOUSE_RIGHT_BUTTON     = 0x0003;
const int WindowApplication::MOUSE_DOWN             = 0x0004;
const int WindowApplication::MOUSE_UP               = 0x0005;

const int WindowApplication::MODIFIER_CONTROL       = 0x0004;
const int WindowApplication::MODIFIER_LBUTTON       = 0x0001;
const int WindowApplication::MODIFIER_MBUTTON       = 0x0002;
const int WindowApplication::MODIFIER_RBUTTON       = 0x0003;
const int WindowApplication::MODIFIER_SHIFT         = 0x0001;

//----------------------------------------------------------------------------
// Extra-data system.
//----------------------------------------------------------------------------
// Indices into the extra data storage for button up-down state and for key
// key modifier up-down state.
#define GLXAPP_BUTTONDOWN  0
#define GLXAPP_SHIFTDOWN   GLXAPP_BUTTONDOWN + 8*sizeof(bool)
#define GLXAPP_CONTROLDOWN GLXAPP_SHIFTDOWN + sizeof(bool)
#define GLXAPP_ALTDOWN     GLXAPP_CONTROLDOWN + sizeof(bool)
#define GLXAPP_COMMANDDOWN GLXAPP_ALTDOWN + sizeof(bool)

const int APP_EXTRA_DATA_QUANTITY = 128;
static char gsExtraData[APP_EXTRA_DATA_QUANTITY];
//----------------------------------------------------------------------------
static void SetExtraData (int index, int size, const void* data)
{
    if (0 <= index && index < APP_EXTRA_DATA_QUANTITY - size)
    {
        memcpy(gsExtraData, data, size);
    }
}
//----------------------------------------------------------------------------
static void GetExtraData (int index, int size, void* data)
{
    if (0 <= index && index < APP_EXTRA_DATA_QUANTITY - size)
    {
        memcpy(data, gsExtraData, size);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void WindowApplication::SetMousePosition (int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    GlxRendererData* data = (GlxRendererData*)theApp->mRenderer->mData;

    XWarpPointer(data->mDisplay, 0, data->mWindow, 0, 0, 0, 0, x, y);
    XFlush(data->mDisplay);
}
//----------------------------------------------------------------------------
void WindowApplication::GetMousePosition (int& x, int& y) const
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    GlxRendererData* data = (GlxRendererData*)theApp->mRenderer->mData;

    Window rootWindow, childWindow;
    int rootX, rootY;
    unsigned int modifier;
    XQueryPointer(data->mDisplay, data->mWindow, &rootWindow, &childWindow,
        &rootX, &rootY, &x, &y, &modifier);
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* text) const
{
    if (!text || strlen(text) == 0)
    {
        return 0;
    }

    return 8*(int)strlen(text);
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharacterWidth (const char) const
{
    return 8;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    return 13;
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int numArguments, char** arguments)
{
    // Initialize the extra data.  TODO:  Port the extra-data system of WM4
    // to WM5.
    memset(gsExtraData, 0, APP_EXTRA_DATA_QUANTITY*sizeof(char));

    WindowApplication* theApp = (WindowApplication*)TheApplication;
    theApp->KEY_TERMINATE = KEY_ESCAPE;

    // OpenGL uses a projection matrix for depth in [-1,1].
    Camera::SetDefaultDepthType(Camera::PM_DEPTH_MINUS_ONE_TO_ONE);

    // Allow work to be done before the window is created.
    if (!theApp->OnPrecreate())
    {
        return -1;
    }

    // Connect to the X server.
    const char* displayName = 0;
    Display* display = XOpenDisplay(displayName);
    if (!display)
    {
        return -2;
    }

    // Make sure the X server has OpenGL GLX extensions.
    int errorBase, eventBase;
    Bool success = glXQueryExtension(display, &errorBase, &eventBase);
    assertion(success == True, "GLX extensions not found.\n");
    if (!success)
    {
        return -3;
    }

    // Partial construction of a GLX renderer.  The window for the renderer
    // is not yet constructed.  When it is, the window identifier is supplied
    // to the renderer to complete its construction.
    RendererInput input;
    input.mDisplay = display;
    input.mVisual = 0;
    input.mContext = 0;
    mRenderer = new0 Renderer(input, theApp->GetWidth(), theApp->GetHeight(),
        mColorFormat, mDepthStencilFormat, mNumMultisamples);
    if (!input.mVisual || !input.mContext)
    {
        return -4;
    }

    // Create an X Window with the visual information created by the renderer
    // constructor.  The visual information might not be the default, so
    // create an X colormap to use.
    Window rootWindow = RootWindow(display, input.mVisual->screen);
    Colormap cMap = XCreateColormap(display, rootWindow,
        input.mVisual->visual, AllocNone);

    // Set the event mask to include exposure (paint), button presses (mouse),
    // and key presses (keyboard).
    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = cMap;
    windowAttributes.border_pixel = 0;
    windowAttributes.event_mask =
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask |
        Button1MotionMask |
        Button2MotionMask |
        Button3MotionMask |
        KeyPressMask |
        KeyReleaseMask |
        ExposureMask |
        StructureNotifyMask;

    int xPos = theApp->GetXPosition();
    int yPos = theApp->GetYPosition();
    unsigned int width = (unsigned int)theApp->GetWidth();
    unsigned int height = (unsigned int)theApp->GetHeight();
    unsigned int borderWidth = 0;
    unsigned long valueMask = CWBorderPixel | CWColormap | CWEventMask;
    Window window = XCreateWindow(display, rootWindow, xPos, yPos, width,
        height, borderWidth, input.mVisual->depth, InputOutput,
        input.mVisual->visual, valueMask, &windowAttributes);

    XSizeHints hints;
    hints.flags = PPosition | PSize;
    hints.x = xPos;
    hints.y = yPos;
    hints.width = width;
    hints.height = height;
    XSetNormalHints(display, window, &hints);

    const char* iconName = theApp->GetWindowTitle();
    Pixmap iconPixmap = None;
    XSetStandardProperties(display, window, theApp->GetWindowTitle(),
        iconName, iconPixmap, arguments, numArguments, &hints);

    // Intercept the close-window event when the user selects the
    // window close button.  The event is a "client message".
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDelete, 1);

    // Finish the construction of the renderer.
    GlxRendererData* data = (GlxRendererData*)mRenderer->mData;
    if (!data->FinishConstruction(window, mRenderer))
    {
        return -5;
    }

    if (theApp->OnInitialize())
    {
        // The default OnPreidle() clears the buffers. Allow the application
        // to fill them before the window is shown and before the event loop
        // starts.
        theApp->OnPreidle();

        // Display the window.
        XMapWindow(display, window);

        // Start the message pump.
        bool applicationRunning = true;
        while (applicationRunning)
        {
            if (!XPending(display))
            {
                theApp->OnIdle();
                continue;
            }

            XEvent evt;
            XNextEvent(display, &evt);
            int index;
            bool state;

            if (evt.type == ButtonPress || evt.type == ButtonRelease)
            {
                theApp->OnMouseClick(evt.xbutton.button, evt.xbutton.type,
                    evt.xbutton.x, evt.xbutton.y, evt.xbutton.state);

                if (evt.type == ButtonPress)
                {
                    index = GLXAPP_BUTTONDOWN + evt.xbutton.button;
                    state = true;
                    SetExtraData(index, sizeof(bool), &state);
                }
                else
                {
                    index = GLXAPP_BUTTONDOWN + evt.xbutton.button;
                    state = false;
                    SetExtraData(index, sizeof(bool), &state);
                }

                continue;
            }

            if (evt.type == MotionNotify)
            {
                int button = 0;

                index = GLXAPP_BUTTONDOWN + 1;
                GetExtraData(index, sizeof(bool), &state);
                if (state)
                {
                    button = MOUSE_LEFT_BUTTON;
                }
                else
                {
                    index = GLXAPP_BUTTONDOWN + 2;
                    GetExtraData(index, sizeof(bool), &state);
                    if (state)
                    {
                        button = MOUSE_MIDDLE_BUTTON;
                    }
                    else
                    {
                        index = GLXAPP_BUTTONDOWN + 3;
                        GetExtraData(index, sizeof(bool), &state);
                        if (state)
                        {
                            button = MOUSE_RIGHT_BUTTON;
                        }
                    }
                }

                if (button > 0)
                {
                    theApp->OnMotion(button, evt.xmotion.x, evt.xmotion.y,
                        evt.xmotion.state);
                }
                else
                {
                    theApp->OnPassiveMotion(evt.xmotion.x, evt.xmotion.y);
                }
                continue;
            }

            if (evt.type == KeyPress || evt.type == KeyRelease)
            {
                KeySym keySym = XKeycodeToKeysym(display,
                    evt.xkey.keycode, 0);

                int key = (keySym & 0x00FF);

                // Quit application if the KEY_TERMINATE key is pressed.
                if (key == theApp->KEY_TERMINATE)
                {
                    XDestroyWindow(display, window);
                    applicationRunning = false;
                    continue;
                }

                // Adjust for special keys that exist on the key pad and on
                // the number pad.
                if ((keySym & 0xFF00) != 0)
                {
                    if (0x50 <= key && key <= 0x57)
                    {
                        // keypad Home, {L,U,R,D}Arrow, Pg{Up,Dn}, End
                        key += 0x45;
                    }
                    else if (key == 0x63)
                    {
                        // keypad Insert
                        key = 0x9e;
                    }
                    else if (key == 0xFF)
                    {
                        // keypad Delete
                        key = 0x9f;
                    }
                    else if (key == 0xE1 || key == 0xE2)
                    {
                        // L-shift or R-shift
                        key = KEY_SHIFT;
                        state = (evt.type == KeyPress);
                        SetExtraData(GLXAPP_SHIFTDOWN, sizeof(bool), &state);
                    }
                    else if (key == 0xE3 || key == 0xE4)
                    {
                        // L-ctrl or R-ctrl
                        key = KEY_CONTROL;
                    }
                    else if (key == 0xE9 || key == 0xEA)
                    {
                        // L-alt or R-alt
                        key = KEY_ALT;
                    }
                    else if (key == 0xEB || key == 0xEC)
                    {
                        key = KEY_COMMAND;
                    }
                }

                if ((KEY_HOME <= key &&  key <= KEY_END)
                ||  (KEY_F1 <= key   &&  key <= KEY_F12)
                ||  (KEY_SHIFT <= key && key <= KEY_COMMAND))
                {
                    if (evt.type == KeyPress)
                    {
                        theApp->OnSpecialKeyDown(key, evt.xbutton.x,
                            evt.xbutton.y);
                    }
                    else
                    {
                        theApp->OnSpecialKeyUp(key, evt.xbutton.x,
                            evt.xbutton.y);
                    }
                }
                else
                {
                    // Get key-modifier state.  Adjust for shift state.
                    unsigned char ucKey = (unsigned char)key;
                    GetExtraData(GLXAPP_SHIFTDOWN, sizeof(bool), &state);
                    if (state && 'a' <= ucKey && ucKey <= 'z')
                    {
                        ucKey = (unsigned char)(key - 32);
                    }

                    if (evt.type == KeyPress)
                    {
                        theApp->OnKeyDown(ucKey, evt.xbutton.x,
                            evt.xbutton.y);
                    }
                    else
                    {
                        theApp->OnKeyUp(ucKey, evt.xbutton.x,
                            evt.xbutton.y);
                    }
                }
                continue;
            }

            if (evt.type == Expose)
            {
                theApp->OnDisplay();
                continue;
            }

            if (evt.type == ConfigureNotify)
            {
                theApp->OnMove(evt.xconfigure.x, evt.xconfigure.y);
                theApp->OnResize(evt.xconfigure.width,
                    evt.xconfigure.height);
                continue;
            }

            if (evt.type == ClientMessage
            &&  evt.xclient.data.l[0] == wmDelete)
            {
                XDestroyWindow(display, window);
                applicationRunning = false;
                continue;
            }
        }
    }

    theApp->OnTerminate();
    XCloseDisplay(display);
    return 0;
}
//----------------------------------------------------------------------------
