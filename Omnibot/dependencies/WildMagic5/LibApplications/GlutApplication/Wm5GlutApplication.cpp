// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.0 (2011/07/23)

#include "Wm5ApplicationPCH.h"
#include "Wm5WindowApplication.h"
#include "Wm5GlutRendererInput.h"
#include "Wm5GlutRendererData.h"
using namespace Wm5;

const int WindowApplication::KEY_ESCAPE = 0x1B;
const int WindowApplication::KEY_LEFT_ARROW = GLUT_KEY_LEFT;
const int WindowApplication::KEY_RIGHT_ARROW = GLUT_KEY_RIGHT;
const int WindowApplication::KEY_UP_ARROW = GLUT_KEY_UP;
const int WindowApplication::KEY_DOWN_ARROW = GLUT_KEY_DOWN;
const int WindowApplication::KEY_HOME = GLUT_KEY_HOME;
const int WindowApplication::KEY_END = GLUT_KEY_END;
const int WindowApplication::KEY_PAGE_UP = GLUT_KEY_PAGE_UP;
const int WindowApplication::KEY_PAGE_DOWN = GLUT_KEY_PAGE_DOWN;
const int WindowApplication::KEY_INSERT = GLUT_KEY_INSERT;
const int WindowApplication::KEY_DELETE = 0x2E;
const int WindowApplication::KEY_F1 = GLUT_KEY_F1;
const int WindowApplication::KEY_F2 = GLUT_KEY_F2;
const int WindowApplication::KEY_F3 = GLUT_KEY_F3;
const int WindowApplication::KEY_F4 = GLUT_KEY_F4;
const int WindowApplication::KEY_F5 = GLUT_KEY_F5;
const int WindowApplication::KEY_F6 = GLUT_KEY_F6;
const int WindowApplication::KEY_F7 = GLUT_KEY_F7;
const int WindowApplication::KEY_F8 = GLUT_KEY_F8;
const int WindowApplication::KEY_F9 = GLUT_KEY_F9;
const int WindowApplication::KEY_F10 = GLUT_KEY_F10;
const int WindowApplication::KEY_F11 = GLUT_KEY_F11;
const int WindowApplication::KEY_F12 = GLUT_KEY_F12;
const int WindowApplication::KEY_BACKSPACE = 0x08;
const int WindowApplication::KEY_TAB = 0x09;
const int WindowApplication::KEY_ENTER = 0x0D;
const int WindowApplication::KEY_RETURN = 0x0D;

const int WindowApplication::KEY_SHIFT = GLUT_ACTIVE_SHIFT;
const int WindowApplication::KEY_CONTROL = GLUT_ACTIVE_CTRL;
const int WindowApplication::KEY_ALT = 0;      // not currently handled
const int WindowApplication::KEY_COMMAND = 0;  // not currently handled

const int WindowApplication::MOUSE_LEFT_BUTTON = GLUT_LEFT_BUTTON;
const int WindowApplication::MOUSE_MIDDLE_BUTTON = GLUT_MIDDLE_BUTTON;
const int WindowApplication::MOUSE_RIGHT_BUTTON = GLUT_RIGHT_BUTTON;
const int WindowApplication::MOUSE_UP = GLUT_UP;
const int WindowApplication::MOUSE_DOWN = GLUT_DOWN;

const int WindowApplication::MODIFIER_CONTROL = 0x0008;
const int WindowApplication::MODIFIER_LBUTTON = 0x0001;
const int WindowApplication::MODIFIER_MBUTTON = 0x0010;
const int WindowApplication::MODIFIER_RBUTTON = 0x0002;
const int WindowApplication::MODIFIER_SHIFT = 0x0004;

// Reading the state of the modifiers from GLUT cannot be done within the
// motion callback, so we cache it here.
static unsigned int gsGLUTModifiers = 0;
static int gsButton = -1;

//----------------------------------------------------------------------------
void WindowApplication::SetMousePosition (int, int)
{
    assertion(false, "Not implemented.\n");
}
//----------------------------------------------------------------------------
void WindowApplication::GetMousePosition (int&, int&) const
{
    assertion(false, "Not implemented.\n");
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* text) const
{
    if (!text || strlen(text) == 0)
    {
        return 0;
    }

    return 8.0f*strlen(text);
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharacterWidth (const char) const
{
    return 8.0f;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    return 13.0f;
}
//----------------------------------------------------------------------------
static void ReshapeCallback (int width, int height)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnResize(width, height);
        theApp->OnDisplay();
    }
}
//----------------------------------------------------------------------------
static void DisplayCallback ()
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnDisplay();
    }
}
//----------------------------------------------------------------------------
static void IdleCallback ()
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnIdle();
    }
}
//----------------------------------------------------------------------------
static void KeyDownCallback (unsigned char key, int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        if (key == theApp->KEY_TERMINATE)
        {
            exit(0);
        }

        theApp->OnKeyDown(key, x, y);
    }
}
//----------------------------------------------------------------------------
static void KeyUpCallback (unsigned char key, int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnKeyUp(key, x, y);
    }
}
//----------------------------------------------------------------------------
static void SpecialKeyDownCallback (int key, int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnSpecialKeyDown(key, x, y);
    }
}
//----------------------------------------------------------------------------
static void SpecialKeyUpCallback (int key, int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnSpecialKeyUp(key, x, y);
    }
}
//----------------------------------------------------------------------------
static void MouseClickCallback (int button, int state, int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        int modifiers = glutGetModifiers();
        gsGLUTModifiers = *(unsigned int*)&modifiers;
        if (state == WindowApplication::MOUSE_DOWN)
        {
            gsButton = button;
        }
        else
        {
            gsButton = -1;
        }

        theApp->OnMouseClick(button, state, x, y, gsGLUTModifiers);
    }
}
//----------------------------------------------------------------------------
static void MotionCallback (int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnMotion(gsButton, x, y, gsGLUTModifiers);
    }
}
//----------------------------------------------------------------------------
static void PassiveMotionCallback (int x, int y)
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnPassiveMotion(x, y);
    }
}
//----------------------------------------------------------------------------
static void Terminate ()
{
    WindowApplication* theApp =
        (WindowApplication*)Application::TheApplication;

    if (theApp)
    {
        theApp->OnTerminate();
        glutDestroyWindow(theApp->GetWindowID());
        Renderer* renderer = (Renderer*)theApp->GetRenderer();
        delete0(renderer);
    }
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int, char**)
{
    WindowApplication* theApp = (WindowApplication*)TheApplication;
    theApp->KEY_TERMINATE = WindowApplication::KEY_ESCAPE;

    // OpenGL uses a projection matrix for depth in [-1,1].
    Camera::SetDefaultDepthType(Camera::PM_DEPTH_MINUS_ONE_TO_ONE);

    // Register the termination function so that we can destroy the window
    // after GLUT abnormally calls 'exit'.
    if (atexit(Terminate) != 0)
    {
        return -1;
    }

    // Give GLUT dummy arguments, because we are not allowing control to
    // GLUT via command-line parameters.
    int numArguments = 1;
    char* arguments[1];
    arguments[0] = new char[6];
    strcpy(arguments[0], "dummy");
    glutInit(&numArguments, arguments);
    delete[] arguments[0];

    // We will always use double buffering, 32-bit RGBA front buffer,
    // depth buffer, and stencil buffer.
    if (mNumMultisamples == 0)
    {
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    }
    else
    {
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL
            | GLUT_MULTISAMPLE);
        mNumMultisamples = glutGet(GLUT_WINDOW_NUM_SAMPLES);
    }

    // Allow work to be done before the window is created.
    if (!theApp->OnPrecreate())
    {
        return -2;
    }

    // Create window and renderer.  Multisampling is not supported.
    glutInitWindowSize(theApp->GetWidth(), theApp->GetHeight());
    RendererInput input;
    input.mWindowID = glutCreateWindow(theApp->GetWindowTitle());
    input.mDisableVerticalSync = true;
    mRenderer = new0 Renderer(input, theApp->GetWidth(), theApp->GetHeight(),
        mColorFormat, mDepthStencilFormat, mNumMultisamples);

    // Save the handle as an 'int' for portable handle storage.
    theApp->SetWindowID(input.mWindowID);

    // Set the callbacks for event handling.
    glutReshapeFunc(ReshapeCallback);
    glutDisplayFunc(DisplayCallback);
    glutIdleFunc(IdleCallback);
    glutKeyboardFunc(KeyDownCallback);
    glutKeyboardUpFunc(KeyUpCallback);
    glutSpecialFunc(SpecialKeyDownCallback);
    glutSpecialUpFunc(SpecialKeyUpCallback);
    glutMouseFunc(MouseClickCallback);
    glutMotionFunc(MotionCallback);
    glutPassiveMotionFunc(PassiveMotionCallback);

    if (theApp->OnInitialize())
    {
        // The default OnPreidle() clears the buffers.  Allow the application
        // to fill them before the window is shown and before the event loop
        // starts.
        theApp->OnPreidle();

        glutMainLoop();
    }

    // Because glutMainLoop never exits, the clean-up is handled via the
    // static Terminate in this file (registered with 'atexit').
    return 0;
}
//----------------------------------------------------------------------------
