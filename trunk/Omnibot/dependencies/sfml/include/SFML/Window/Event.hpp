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

#ifndef SFML_EVENT_HPP
#define SFML_EVENT_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Config.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
/// Definition of key codes for keyboard events
////////////////////////////////////////////////////////////
namespace Key
{
    enum Code
    {
        A = 'a',
        B = 'b',
        C = 'c',
        D = 'd',
        E = 'e',
        F = 'f',
        G = 'g',
        H = 'h',
        I = 'i',
        J = 'j',
        K = 'k',
        L = 'l',
        M = 'm',
        N = 'n',
        O = 'o',
        P = 'p',
        Q = 'q',
        R = 'r',
        S = 's',
        T = 't',
        U = 'u',
        V = 'v',
        W = 'w',
        X = 'x',
        Y = 'y',
        Z = 'z',
        Num0 = '0',
        Num1 = '1',
        Num2 = '2',
        Num3 = '3',
        Num4 = '4',
        Num5 = '5',
        Num6 = '6',
        Num7 = '7',
        Num8 = '8',
        Num9 = '9', 
        Escape = 256,
        LControl,
        LShift,
        LAlt,
        LSystem,      ///< OS specific key (left side) : windows (Win and Linux), apple (MacOS), ...
        RControl,
        RShift,
        RAlt,
        RSystem,      ///< OS specific key (right side) : windows (Win and Linux), apple (MacOS), ...
        Menu,
        LBracket,     ///< [
        RBracket,     ///< ]
        SemiColon,    ///< ;
        Comma,        ///< ,
        Period,       ///< .
        Quote,        ///< '
        Slash,        ///< /
        BackSlash,
        Tilde,        ///< ~
        Equal,        ///< =
        Dash,         ///< -
        Space,
        Return,
        Back,
        Tab,
        PageUp,
        PageDown,
        End,
        Home,
        Insert,
        Delete,
        Add,          ///< +
        Subtract,     ///< -
        Multiply,     ///< *
        Divide,       ///< /
        Left,         ///< Left arrow
        Right,        ///< Right arrow
        Up,           ///< Up arrow
        Down,         ///< Down arrow
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        Pause,

		// custom
		PrintScreen,
		NumLock,
		CapsLock,
		ScrollLock,

        Count // For internal use
    };
}


////////////////////////////////////////////////////////////
/// Definition of button codes for mouse events
////////////////////////////////////////////////////////////
namespace Mouse
{
    enum Button
    {
        Left,
        Right,
        Middle,
        XButton1,
        XButton2,

        Count // For internal use
    };
}


////////////////////////////////////////////////////////////
/// Definition of joystick axis for joystick events
////////////////////////////////////////////////////////////
namespace Joy
{
    enum Axis
    {
        AxisX,
        AxisY,
        AxisZ,
        AxisR,
        AxisU,
        AxisV,
        AxisPOV,

        Count // For internal use
    };
}


////////////////////////////////////////////////////////////
/// Event defines a system event and its parameters
////////////////////////////////////////////////////////////
class Event
{
public :

    ////////////////////////////////////////////////////////////
    /// Keyboard event parameters
    ////////////////////////////////////////////////////////////
    struct KeyEvent
    {
        Key::Code Code;
        bool      Alt;
        bool      Control;
        bool      Shift;
    };

    ////////////////////////////////////////////////////////////
    /// Text event parameters
    ////////////////////////////////////////////////////////////
    struct TextEvent
    {
        Uint32 Unicode;
    };

    ////////////////////////////////////////////////////////////
    /// Mouse move event parameters
    ////////////////////////////////////////////////////////////
    struct MouseMoveEvent
    {
        int X;
        int Y;
    };

    ////////////////////////////////////////////////////////////
    /// Mouse buttons events parameters
    ////////////////////////////////////////////////////////////
    struct MouseButtonEvent
    {
        Mouse::Button Button;
        int           X;
        int           Y;
    };

    ////////////////////////////////////////////////////////////
    /// Mouse wheel events parameters
    ////////////////////////////////////////////////////////////
    struct MouseWheelEvent
    {
        int Delta;
    };

    ////////////////////////////////////////////////////////////
    /// Joystick axis move event parameters
    ////////////////////////////////////////////////////////////
    struct JoyMoveEvent
    {
        unsigned int JoystickId;
        Joy::Axis    Axis;
        float        Position;
    };

    ////////////////////////////////////////////////////////////
    /// Joystick buttons events parameters
    ////////////////////////////////////////////////////////////
    struct JoyButtonEvent
    {
        unsigned int JoystickId;
        unsigned int Button;
    };

    ////////////////////////////////////////////////////////////
    /// Size events parameters
    ////////////////////////////////////////////////////////////
    struct SizeEvent
    {
        unsigned int Width;
        unsigned int Height;
    };

    ////////////////////////////////////////////////////////////
    /// Enumeration of the different types of events
    ////////////////////////////////////////////////////////////
    enum EventType
    {
        Closed,
        Resized,
        LostFocus,
        GainedFocus,
        TextEntered,
        KeyPressed,
        KeyReleased,
        MouseWheelMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseEntered,
        MouseLeft,
        JoyButtonPressed,
        JoyButtonReleased,
        JoyMoved
    };

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    EventType Type; ///< Type of the event

    union
    {
        KeyEvent         Key;
        TextEvent        Text;
        MouseMoveEvent   MouseMove;
        MouseButtonEvent MouseButton;
        MouseWheelEvent  MouseWheel;
        JoyMoveEvent     JoyMove;
        JoyButtonEvent   JoyButton;
        SizeEvent        Size;
    };
};

} // namespace sf


#endif // SFML_EVENT_HPP
