// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5DX9RENDERERINPUT_H
#define WM5DX9RENDERERINPUT_H

#include "Wm5Dx9RendererLIB.h"

namespace Wm5
{

class RendererInput
{
public:
    IDirect3D9* mDriver;
    HWND mWindowHandle;
};

}

#endif
