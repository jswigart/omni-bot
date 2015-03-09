// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/05/03)

#ifndef WM5DX9RENDERERLIB_H
#define WM5DX9RENDERERLIB_H

#include "Wm5GraphicsLIB.h"

// Disable the 'min' and 'max' macros that are sucked in by the indirect
// inclusion of windows.h in the DirectX header files.  These conflict
// with std::numeric_limits<type>::max().
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#endif
