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
#include <SFML/Window/Context.hpp>
#include <SFML/Window/WindowImpl.hpp>


namespace
{
    // Make sure the dummy context is created at global startup
    //sf::Context& Dummy = sf::Context::GetGlobal();
}


namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor, create the context
////////////////////////////////////////////////////////////
Context::Context() : myDummyWindow(0)
{
    myDummyWindow = priv::WindowImpl::New();
}


////////////////////////////////////////////////////////////
/// Destructor, destroy the context
////////////////////////////////////////////////////////////
Context::~Context()
{
    delete myDummyWindow;
}


////////////////////////////////////////////////////////////
/// Activate or deactivate the context
////////////////////////////////////////////////////////////
void Context::SetActive(bool Active)
{
    myDummyWindow->SetActive(Active);
}


////////////////////////////////////////////////////////////
/// Check if there's a context bound to the current thread
////////////////////////////////////////////////////////////
bool Context::IsContextActive()
{
    return priv::WindowImpl::IsContextActive();
}


////////////////////////////////////////////////////////////
/// Get the global context
////////////////////////////////////////////////////////////
Context& Context::GetGlobal()
{
    static Context* GlobalContext = new Context; // Never deleted, on purpose

    return *GlobalContext;
}

} // namespace sf
