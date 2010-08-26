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
#include <SFML/xxx/Joystick.hpp>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// Initialize the instance and bind it to a physical joystick
////////////////////////////////////////////////////////////
void Joystick::Initialize(unsigned int Index)
{
    // Reset the joystick state
    
    // Initialize the Index-th available joystick
}


////////////////////////////////////////////////////////////
/// Update the current joystick and return its new state
////////////////////////////////////////////////////////////
JoystickState Joystick::UpdateState()
{
    // Fill a JoystickState instance with the current joystick state
}


////////////////////////////////////////////////////////////
/// Get the number of axes supported by the joystick
////////////////////////////////////////////////////////////
unsigned int Joystick::GetAxesCount() const
{
    // Return number of supported axes
}


////////////////////////////////////////////////////////////
/// Get the number of buttons supported by the joystick
////////////////////////////////////////////////////////////
unsigned int Joystick::GetButtonsCount() const
{
    // Return number of supported buttons
}

} // namespace priv

} // namespace sf
