// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/30)

//----------------------------------------------------------------------------
inline const char* WindowApplication::GetWindowTitle () const
{
    return mWindowTitle.c_str();
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetXPosition () const
{
    return mXPosition;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetYPosition () const
{
    return mYPosition;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetWidth () const
{
    return mWidth;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetHeight () const
{
    return mHeight;
}
//----------------------------------------------------------------------------
inline float WindowApplication::GetAspectRatio () const
{
    return ((float)mWidth)/(float)mHeight;
}
//----------------------------------------------------------------------------
inline void WindowApplication::SetWindowID (int windowID)
{
    mWindowID = windowID;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetWindowID () const
{
    return mWindowID;
}
//----------------------------------------------------------------------------
inline const Renderer* WindowApplication::GetRenderer ()
{
    return mRenderer;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnPrecreate ()
{
    // stub for derived classes
    return true;
}
//----------------------------------------------------------------------------
inline void WindowApplication::OnPreidle ()
{
    // The default behavior is to clear the buffers before the window is
    // displayed for the first time.
    mRenderer->ClearBuffers();
}
//----------------------------------------------------------------------------
inline void WindowApplication::OnDisplay ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
inline void WindowApplication::OnIdle ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnKeyUp (unsigned char, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnSpecialKeyDown (int, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnSpecialKeyUp (int, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMouseClick (int, int, int, int,
    unsigned int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMotion (int, int, int, unsigned int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnPassiveMotion (int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMouseWheel (int, int, int, unsigned int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
