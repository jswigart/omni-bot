// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/05/06)

#ifndef WM5WINDOWAPPLICATION_H
#define WM5WINDOWAPPLICATION_H

#include "Wm5Application.h"

namespace Wm5
{

class WindowApplication : public Application
{
protected:
    // Abstract base class.
    WindowApplication (const char* windowTitle, int xPosition,
        int yPosition, int width, int height, const Float4& clearColor);
public:
    virtual ~WindowApplication ();

    // Entry point to be implemented by the application.  The return value
    // is an exit code, if desired.
    virtual int Main (int numArguments, char** arguments);

    // Member access.
    inline const char* GetWindowTitle () const;
    inline int GetXPosition () const;
    inline int GetYPosition () const;
    inline int GetWidth () const;
    inline int GetHeight () const;
    inline float GetAspectRatio () const;
    inline void SetWindowID (int windowID);
    inline int GetWindowID () const;
    inline const Renderer* GetRenderer ();

    // Event callbacks.  The inline functions are stubs for the derived
    // classes.  The non-inline functions are implemented in the class
    // WindowApplication, but are still overridden by derived classes.
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnMove (int x, int y);
    virtual void OnResize (int width, int height);
    inline virtual bool OnPrecreate ();
    inline virtual void OnPreidle ();
    inline virtual void OnDisplay ();
    inline virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    inline virtual bool OnKeyUp (unsigned char key, int x, int y);
    inline virtual bool OnSpecialKeyDown (int key, int x, int y);
    inline virtual bool OnSpecialKeyUp (int key, int x, int y);
    inline virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    inline virtual bool OnMotion (int button, int x, int y,
        unsigned int modifiers);
    inline virtual bool OnPassiveMotion (int x, int y);
    inline virtual bool OnMouseWheel (int delta, int x, int y,
        unsigned int modifiers);

    // Mouse position.
    void SetMousePosition (int x, int y);
    void GetMousePosition (int& x, int& y) const;

    // Font information.  These are platform-specific, so classes that
    // implement the WindowApplication interfaces must implement these
    // functions.  They are not defined by WindowApplication.
    int GetStringWidth (const char* text) const;
    int GetCharacterWidth (const char character) const;
    int GetFontHeight () const;

    // Key identifiers.  These are platform-specific, so classes that
    // implement the WindowApplication interfaces must define these variables.
    // They are not defined by WindowApplication.
    int KEY_TERMINATE;  // default KEY_ESCAPE, redefine as desired
    static const int KEY_ESCAPE;
    static const int KEY_LEFT_ARROW;
    static const int KEY_RIGHT_ARROW;
    static const int KEY_UP_ARROW;
    static const int KEY_DOWN_ARROW;
    static const int KEY_HOME;
    static const int KEY_END;
    static const int KEY_PAGE_UP;
    static const int KEY_PAGE_DOWN;
    static const int KEY_INSERT;
    static const int KEY_DELETE;
    static const int KEY_F1;
    static const int KEY_F2;
    static const int KEY_F3;
    static const int KEY_F4;
    static const int KEY_F5;
    static const int KEY_F6;
    static const int KEY_F7;
    static const int KEY_F8;
    static const int KEY_F9;
    static const int KEY_F10;
    static const int KEY_F11;
    static const int KEY_F12;
    static const int KEY_BACKSPACE;
    static const int KEY_TAB;
    static const int KEY_ENTER;
    static const int KEY_RETURN;

    // Keyboard modifiers.
    static const int KEY_SHIFT;
    static const int KEY_CONTROL;
    static const int KEY_ALT;
    static const int KEY_COMMAND;

    // Mouse buttons.
    static const int MOUSE_LEFT_BUTTON;
    static const int MOUSE_MIDDLE_BUTTON;
    static const int MOUSE_RIGHT_BUTTON;

    // Mouse state.
    static const int MOUSE_UP;
    static const int MOUSE_DOWN;

    // Mouse modifiers.
    static const int MODIFIER_CONTROL;
    static const int MODIFIER_LBUTTON;
    static const int MODIFIER_MBUTTON;
    static const int MODIFIER_RBUTTON;
    static const int MODIFIER_SHIFT;

protected:
    // The hookup to the 'main' entry point into the executable.
    static int Run (int numArguments, char** arguments);

    // Window parameters (from the constructor).
    std::string mWindowTitle;
    int mXPosition, mYPosition, mWidth, mHeight;
    Float4 mClearColor;
    bool mAllowResize;

    // The window ID is platform-specific but hidden by an 'int' opaque
    // handle.
    int mWindowID;

    Texture::Format mColorFormat;
    Texture::Format mDepthStencilFormat;
    int mNumMultisamples;
    Renderer* mRenderer;

    // Performance measurements.
    void ResetTime ();
    void MeasureTime ();
    void UpdateFrameCount ();
    void DrawFrameRate (int x, int y, const Float4& color);

    double mLastTime, mAccumulatedTime, mFrameRate;
    int mFrameCount, mAccumulatedFrameCount, mTimer, mMaxTimer;
};

#include "Wm5WindowApplication.inl"

//----------------------------------------------------------------------------
#define WM5_WINDOW_APPLICATION(classname) \
WM5_IMPLEMENT_INITIALIZE(classname); \
WM5_IMPLEMENT_TERMINATE(classname); \
\
void classname::Initialize () \
{ \
    Application::Run = &WindowApplication::Run; \
    TheApplication = new0 classname(); \
} \
\
void classname::Terminate () \
{ \
    delete0(TheApplication); \
}
//----------------------------------------------------------------------------

}

#endif
