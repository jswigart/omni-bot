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
#include <SFML/Window/Input.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor
////////////////////////////////////////////////////////////
Input::Input() :
myMouseX(0),
myMouseY(0)
{
    for (int i = 0; i < Key::Count; ++i)
        myKeys[i] = false;

    for (int i = 0; i < Mouse::Count; ++i)
        myMouseButtons[i] = false;

    for (int i = 0; i < 16; ++i)
    {
        myJoystickButtons[0][i] = false;
        myJoystickButtons[1][i] = false;
    }

    for (int i = 0; i < Joy::Count; ++i)
    {
        myJoystickAxis[0][i] = 0.f;
        myJoystickAxis[1][i] = 0.f;
    }
}


////////////////////////////////////////////////////////////
/// Get the state of a key
////////////////////////////////////////////////////////////
bool Input::IsKeyDown(Key::Code KeyCode) const
{
    return myKeys[KeyCode];
}


////////////////////////////////////////////////////////////
/// Get the state of a mouse button
////////////////////////////////////////////////////////////
bool Input::IsMouseButtonDown(Mouse::Button Button) const
{
    return myMouseButtons[Button];
}


////////////////////////////////////////////////////////////
/// Get the state of a joystick button
////////////////////////////////////////////////////////////
bool Input::IsJoystickButtonDown(unsigned int JoyId, unsigned int Button) const
{
    if ((JoyId < 2) && (Button < 16))
        return myJoystickButtons[JoyId][Button];
    else
        return false;
}


////////////////////////////////////////////////////////////
/// Get the mouse left position
////////////////////////////////////////////////////////////
int Input::GetMouseX() const
{
    return myMouseX;
}


////////////////////////////////////////////////////////////
/// Get the mouse top position
////////////////////////////////////////////////////////////
int Input::GetMouseY() const
{
    return myMouseY;
}


////////////////////////////////////////////////////////////
/// Get a joystick axis position
////////////////////////////////////////////////////////////
float Input::GetJoystickAxis(unsigned int JoyId, Joy::Axis Axis) const
{
    return myJoystickAxis[JoyId][Axis];
}


////////////////////////////////////////////////////////////
/// /see WindowListener::OnEvent
////////////////////////////////////////////////////////////
void Input::OnEvent(const Event& EventReceived)
{
    switch (EventReceived.Type)
    {
        // Key events
        case Event::KeyPressed :  myKeys[EventReceived.Key.Code] = true;  break;
        case Event::KeyReleased : myKeys[EventReceived.Key.Code] = false; break;

        // Mouse event
        case Event::MouseButtonPressed :  myMouseButtons[EventReceived.MouseButton.Button] = true;  break;
        case Event::MouseButtonReleased : myMouseButtons[EventReceived.MouseButton.Button] = false; break;

        // Mouse move event
        case Event::MouseMoved :
            myMouseX = EventReceived.MouseMove.X;
            myMouseY = EventReceived.MouseMove.Y;
            break;

        // Joystick button events
        case Event::JoyButtonPressed :  myJoystickButtons[EventReceived.JoyButton.JoystickId][EventReceived.JoyButton.Button] = true;  break;
        case Event::JoyButtonReleased : myJoystickButtons[EventReceived.JoyButton.JoystickId][EventReceived.JoyButton.Button] = false; break;

        // Joystick move event
        case Event::JoyMoved :
            myJoystickAxis[EventReceived.JoyMove.JoystickId][EventReceived.JoyMove.Axis] = EventReceived.JoyMove.Position;
            break;

        // Lost focus event : we must reset all persistent states
        case Event::LostFocus :
        {
            for (int i = 0; i < Key::Count; ++i)
                myKeys[i] = false;

            for (int i = 0; i < Mouse::Count; ++i)
                myMouseButtons[i] = false;

            for (int i = 0; i < 16; ++i)
            {
                myJoystickButtons[0][i] = false;
                myJoystickButtons[1][i] = false;
            }
            break;
        }

        default :
            break;
    }
}

} // namespace sf
