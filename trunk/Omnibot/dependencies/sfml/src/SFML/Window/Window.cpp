////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2008 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Context.hpp>
#include <SFML/Window/WindowImpl.hpp>
#include <SFML/System/Sleep.hpp>
#include <iostream>


namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor
///
////////////////////////////////////////////////////////////
Window::Window() :
myWindow        (NULL),
myLastFrameTime (0.f),
myIsExternal    (false),
myFramerateLimit(0),
mySetCursorPosX (-1),
mySetCursorPosY (-1)
{

}


////////////////////////////////////////////////////////////
/// Construct a new window
////////////////////////////////////////////////////////////
Window::Window(VideoMode Mode, const std::string& Title, unsigned long WindowStyle, const WindowSettings& Params) :
myWindow        (NULL),
myLastFrameTime (0.f),
myIsExternal    (false),
myFramerateLimit(0),
mySetCursorPosX (0xFFFF),
mySetCursorPosY (0xFFFF)
{
    Create(Mode, Title, WindowStyle, Params);
}


////////////////////////////////////////////////////////////
/// Construct the window from an existing control
////////////////////////////////////////////////////////////
Window::Window(WindowHandle Handle, const WindowSettings& Params, bool UseCurrentWindow) :
myWindow        (NULL),
myLastFrameTime (0.f),
myIsExternal    (true),
myFramerateLimit(0),
mySetCursorPosX (-1),
mySetCursorPosY (-1)
{
    Create(Handle, Params, UseCurrentWindow);
}


////////////////////////////////////////////////////////////
/// Destructor
////////////////////////////////////////////////////////////
Window::~Window()
{
    // Close the window
    Close();
}


////////////////////////////////////////////////////////////
/// Create the window
////////////////////////////////////////////////////////////
void Window::Create(VideoMode Mode, const std::string& Title, unsigned long WindowStyle, const WindowSettings& Params)
{
    // Check validity of video mode
    if ((WindowStyle & Style::Fullscreen) && !Mode.IsValid())
    {
        std::cerr << "The requested video mode is not available, switching to a valid mode" << std::endl;
        Mode = VideoMode::GetMode(0);
    }

    // Check validity of style
    if ((WindowStyle & Style::Close) || (WindowStyle & Style::Resize))
        WindowStyle |= Style::Titlebar;

    // Destroy the previous window implementation
    delete myWindow;

    // Activate the global context
    Context::GetGlobal().SetActive(true);

    mySettings = Params;
    Initialize(priv::WindowImpl::New(Mode, Title, WindowStyle, mySettings));
}


////////////////////////////////////////////////////////////
/// Create the window from an existing control
////////////////////////////////////////////////////////////
void Window::Create(WindowHandle Handle, const WindowSettings& Params, bool UseCurrentWindow)
{
    // Destroy the previous window implementation
    delete myWindow;

    // Activate the global context
    Context::GetGlobal().SetActive(true);

    mySettings = Params;
    Initialize(priv::WindowImpl::New(Handle, mySettings, UseCurrentWindow));
}


////////////////////////////////////////////////////////////
/// Close (destroy) the window.
/// The sf::Window instance remains valid and you can call
/// Create to recreate the window
////////////////////////////////////////////////////////////
void Window::Close()
{
    // Delete the window implementation
    delete myWindow;
    myWindow = NULL;
}


////////////////////////////////////////////////////////////
/// Tell whether or not the window is opened (ie. has been created).
/// Note that a hidden window (Show(false))
/// will still return true
////////////////////////////////////////////////////////////
bool Window::IsOpened() const
{
    return myWindow != NULL;
}


////////////////////////////////////////////////////////////
/// Get the width of the rendering region of the window
////////////////////////////////////////////////////////////
unsigned int Window::GetWidth() const
{
    return myWindow ? myWindow->GetWidth() : 0;
}


////////////////////////////////////////////////////////////
/// Get the height of the rendering region of the window
////////////////////////////////////////////////////////////
unsigned int Window::GetHeight() const
{
    return myWindow ? myWindow->GetHeight() : 0;
}


////////////////////////////////////////////////////////////
/// Get the creation settings of the window
////////////////////////////////////////////////////////////
const WindowSettings& Window::GetSettings() const
{
    return mySettings;
}


////////////////////////////////////////////////////////////
/// Get the event on top of events stack, if any
////////////////////////////////////////////////////////////
bool Window::GetEvent(Event& EventReceived)
{
    // Let the window implementation process incoming events if the events queue is empty
    if (myWindow && myEvents.empty())
        myWindow->DoEvents();

    // Pop first event of queue, if not empty
    if (!myEvents.empty())
    {
        EventReceived = myEvents.front();
        myEvents.pop();

        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////
/// Enable / disable vertical synchronization
////////////////////////////////////////////////////////////
void Window::UseVerticalSync(bool Enabled)
{
    if (SetActive())
        myWindow->UseVerticalSync(Enabled);
}


////////////////////////////////////////////////////////////
/// Show or hide the mouse cursor
////////////////////////////////////////////////////////////
void Window::ShowMouseCursor(bool Show)
{
    if (myWindow)
        myWindow->ShowMouseCursor(Show);
}


////////////////////////////////////////////////////////////
/// Change the position of the mouse cursor
////////////////////////////////////////////////////////////
void Window::SetCursorPosition(unsigned int Left, unsigned int Top)
{
    if (myWindow)
    {
        // Keep coordinates for later checking (to reject the generated MouseMoved event)
        mySetCursorPosX = Left;
        mySetCursorPosY = Top;

        myWindow->SetCursorPosition(Left, Top);
    }
}


////////////////////////////////////////////////////////////
/// Change the position of the window on screen
////////////////////////////////////////////////////////////
void Window::SetPosition(int Left, int Top)
{
    if (!myIsExternal)
    {
        if (myWindow)
            myWindow->SetPosition(Left, Top);
    }
    else
    {
        std::cerr << "Warning : trying to change the position of an external SFML window, which is not allowed" << std::endl;
    }
}


////////////////////////////////////////////////////////////
/// Change the size of the rendering region of the window
////////////////////////////////////////////////////////////
void Window::SetSize(unsigned int Width, unsigned int Height)
{
    if (myWindow)
        myWindow->SetSize(Width, Height);
}


////////////////////////////////////////////////////////////
/// Show or hide the window
////////////////////////////////////////////////////////////
void Window::Show(bool State)
{
    if (!myIsExternal)
    {
        if (myWindow)
            myWindow->Show(State);
    }
}


////////////////////////////////////////////////////////////
/// Enable or disable automatic key-repeat.
/// Automatic key-repeat is enabled by default
////////////////////////////////////////////////////////////
void Window::EnableKeyRepeat(bool Enabled)
{
    if (myWindow)
        myWindow->EnableKeyRepeat(Enabled);
}


////////////////////////////////////////////////////////////
/// Change the window's icon
////////////////////////////////////////////////////////////
void Window::SetIcon(unsigned int Width, unsigned int Height, const Uint8* Pixels)
{
    if (myWindow)
        myWindow->SetIcon(Width, Height, Pixels);
}


////////////////////////////////////////////////////////////
/// Activate of deactivate the window as the current target
/// for rendering
////////////////////////////////////////////////////////////
bool Window::SetActive(bool Active) const
{
    if (myWindow)
    {
        myWindow->SetActive(Active);
        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////
/// Display the window on screen
////////////////////////////////////////////////////////////
void Window::Display()
{
    // Limit the framerate if needed
    if (myFramerateLimit > 0)
    {
        float RemainingTime = 1.f / myFramerateLimit - myClock.GetElapsedTime();
        if (RemainingTime > 0)
            Sleep(RemainingTime);
    }

    // Measure the time elapsed since last frame
    myLastFrameTime = myClock.GetElapsedTime();
    myClock.Reset();

    // Display the backbuffer on screen
    if (SetActive())
        myWindow->Display();
}


////////////////////////////////////////////////////////////
/// Get the input manager of the window
////////////////////////////////////////////////////////////
const Input& Window::GetInput() const
{
    return myInput;
}


////////////////////////////////////////////////////////////
/// Set the framerate at a fixed frequency
////////////////////////////////////////////////////////////
void Window::SetFramerateLimit(unsigned int Limit)
{
    myFramerateLimit = Limit;
}


////////////////////////////////////////////////////////////
/// Get time elapsed since last frame
////////////////////////////////////////////////////////////
float Window::GetFrameTime() const
{
    return myLastFrameTime;
}


////////////////////////////////////////////////////////////
/// Change the joystick threshold, ie. the value below which
/// no move event will be generated
////////////////////////////////////////////////////////////
void Window::SetJoystickThreshold(float Threshold)
{
    if (myWindow)
        myWindow->SetJoystickThreshold(Threshold);
}


////////////////////////////////////////////////////////////
/// Called after the window has been created
////////////////////////////////////////////////////////////
void Window::OnCreate()
{
    // Nothing by default
}


////////////////////////////////////////////////////////////
/// Receive an event from window
////////////////////////////////////////////////////////////
void Window::OnEvent(const Event& EventReceived)
{
    // Discard MouseMove events generated by SetCursorPosition
    if ((EventReceived.Type        == Event::MouseMoved) &&
        (EventReceived.MouseMove.X == mySetCursorPosX)   &&
        (EventReceived.MouseMove.Y == mySetCursorPosY))
    {
        mySetCursorPosX = 0xFFFF;
        mySetCursorPosY = 0xFFFF;
        return;
    }

    myEvents.push(EventReceived);
}


////////////////////////////////////////////////////////////
/// Initialize internal window
////////////////////////////////////////////////////////////
void Window::Initialize(priv::WindowImpl* Window)
{
    // Assign new window and listen to its events
    myWindow = Window;
    myWindow->AddListener(this);

    // Attach input to the window
    myWindow->AddListener(&myInput);

    // Setup default behaviours (to get a consistent behaviour across different implementations)
    Show(true);
    UseVerticalSync(false);
    ShowMouseCursor(true);
    EnableKeyRepeat(true);

    // Reset frame time
    myClock.Reset();
    myLastFrameTime = 0.f;

    // Activate the window
    SetActive(true);

    // Notify the derived class
    OnCreate();
}

} // namespace sf
