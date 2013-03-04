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
#include <SFML/Audio/SoundRecorder.hpp>
#include <SFML/Audio/AudioDevice.hpp>
#include <SFML/Audio/OpenAL.hpp>
#include <SFML/System/Sleep.hpp>
#include <iostream>


////////////////////////////////////////////////////////////
// Private data
////////////////////////////////////////////////////////////
namespace
{
    ALCdevice* CaptureDevice = NULL;
}

namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor
////////////////////////////////////////////////////////////
SoundRecorder::SoundRecorder() :
mySampleRate (0),
myIsCapturing(false)
{

}


////////////////////////////////////////////////////////////
/// Virtual destructor
////////////////////////////////////////////////////////////
SoundRecorder::~SoundRecorder()
{
    // Nothing to do
}


////////////////////////////////////////////////////////////
/// Start the capture.
/// Warning : only one capture can happen at the same time
////////////////////////////////////////////////////////////
void SoundRecorder::Start(unsigned int SampleRate)
{
    // Check if the device can do audio capture
    if (!CanCapture())
    {
        std::cerr << "Failed to start capture : your system cannot capture audio data (call SoundRecorder::CanCapture to check it)" << std::endl;
        return;
    }

    // Check that another capture is not already running
    if (CaptureDevice)
    {
        std::cerr << "Trying to start audio capture, but another capture is already running" << std::endl;
        return;
    }

    // Open the capture device for capturing 16 bits mono samples
    CaptureDevice = alcCaptureOpenDevice(NULL, SampleRate, AL_FORMAT_MONO16, SampleRate);
    if (!CaptureDevice)
    {
        std::cerr << "Failed to open the audio capture device" << std::endl;
        return;
    }

    // Clear the sample array
    mySamples.clear();

    // Store the sample rate
    mySampleRate = SampleRate;

    // Notify derived class
    if (OnStart())
    {
        // Start the capture
        alcCaptureStart(CaptureDevice);

        // Start the capture in a new thread, to avoid blocking the main thread
        myIsCapturing = true;
        Launch();
    }
}


////////////////////////////////////////////////////////////
/// Stop the capture
////////////////////////////////////////////////////////////
void SoundRecorder::Stop()
{
    // Stop the capturing thread
    myIsCapturing = false;
    Wait();
}


////////////////////////////////////////////////////////////
/// Get the sample rate
////////////////////////////////////////////////////////////
unsigned int SoundRecorder::GetSampleRate() const
{
    return mySampleRate;
}


////////////////////////////////////////////////////////////
/// Tell if the system supports sound capture.
/// If not, this class won't be usable
////////////////////////////////////////////////////////////
bool SoundRecorder::CanCapture()
{
    ALCdevice* Device = priv::AudioDevice::GetInstance().GetDevice();

    return alcIsExtensionPresent(Device, "ALC_EXT_CAPTURE") != AL_FALSE;
}


////////////////////////////////////////////////////////////
/// Start recording audio data
////////////////////////////////////////////////////////////
bool SoundRecorder::OnStart()
{
    // Nothing to do
    return true;
}


////////////////////////////////////////////////////////////
/// Stop recording audio data
////////////////////////////////////////////////////////////
void SoundRecorder::OnStop()
{
    // Nothing to do
}


////////////////////////////////////////////////////////////
/// /see Thread::Run
////////////////////////////////////////////////////////////
void SoundRecorder::Run()
{
    while (myIsCapturing)
    {
        // Process available samples
        ProcessCapturedSamples();

        // Don't bother the CPU while waiting for more captured data
        Sleep(0.1f);
    }

    // Capture is finished : clean up everything
    CleanUp();

    // Notify derived class
    OnStop();
}


////////////////////////////////////////////////////////////
/// Get available captured samples and process them
////////////////////////////////////////////////////////////
void SoundRecorder::ProcessCapturedSamples()
{
    // Get the number of samples available
    ALCint SamplesAvailable;
    alcGetIntegerv(CaptureDevice, ALC_CAPTURE_SAMPLES, 1, &SamplesAvailable);

    if (SamplesAvailable > 0)
    {
        // Get the recorded samples
        mySamples.resize(SamplesAvailable);
        alcCaptureSamples(CaptureDevice, &mySamples[0], SamplesAvailable);

        // Forward them to the derived class
        if (!OnProcessSamples(&mySamples[0], mySamples.size()))
        {
            // The user wants to stop the capture
            myIsCapturing = false;
        }
    }
}


////////////////////////////////////////////////////////////
/// Clean up recorder internal resources
////////////////////////////////////////////////////////////
void SoundRecorder::CleanUp()
{
    // Stop the capture
    alcCaptureStop(CaptureDevice);

    // Get the samples left in the buffer
    ProcessCapturedSamples();

    // Close the device
    alcCaptureCloseDevice(CaptureDevice);
    CaptureDevice = NULL;
}

} // namespace sf
