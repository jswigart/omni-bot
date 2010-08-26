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
#include <SFML/Audio/SoundBufferRecorder.hpp>
#include <algorithm>
#include <iterator>


namespace sf
{
////////////////////////////////////////////////////////////
/// /see SoundBuffer::OnStart
////////////////////////////////////////////////////////////
bool SoundBufferRecorder::OnStart()
{
    mySamples.clear();

    return true;
}


////////////////////////////////////////////////////////////
/// /see SoundBuffer::OnProcessSamples
////////////////////////////////////////////////////////////
bool SoundBufferRecorder::OnProcessSamples(const Int16* Samples, std::size_t SamplesCount)
{
    std::copy(Samples, Samples + SamplesCount, std::back_inserter(mySamples));

    return true;
}


////////////////////////////////////////////////////////////
/// /see SoundBuffer::OnStop
////////////////////////////////////////////////////////////
void SoundBufferRecorder::OnStop()
{
    if (!mySamples.empty())
        myBuffer.LoadFromSamples(&mySamples[0], mySamples.size(), 1, GetSampleRate());
}


////////////////////////////////////////////////////////////
/// Get the sound buffer containing the captured audio data
////////////////////////////////////////////////////////////
const SoundBuffer& SoundBufferRecorder::GetBuffer() const
{
    return myBuffer;
}

} // namespace sf
