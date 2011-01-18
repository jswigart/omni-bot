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

#ifndef SFML_VIDEOMODESUPPORTWIN32_HPP
#define SFML_VIDEOMODESUPPORTWIN32_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/VideoMode.hpp>
#include <vector>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// Win32 implementation of VideoModeSupport 
/// Give access to video mode related OS-specific functions
////////////////////////////////////////////////////////////
class VideoModeSupport
{
public :

    ////////////////////////////////////////////////////////////
    /// Get supported video modes
    ///
    /// \param Modes : Array to fill with available video modes
    ///
    ////////////////////////////////////////////////////////////
    static void GetSupportedVideoModes(std::vector<VideoMode>& Modes);

    ////////////////////////////////////////////////////////////
    /// Get current desktop video mode
    ///
    /// \return Current desktop video mode
    ///
    ////////////////////////////////////////////////////////////
    static VideoMode GetDesktopVideoMode();
};

} // namespace priv

} // namespace sf


#endif // SFML_VIDEOMODESUPPORTWIN32_HPP
