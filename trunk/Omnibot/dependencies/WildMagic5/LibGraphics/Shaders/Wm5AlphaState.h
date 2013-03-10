// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ALPHASTATE_H
#define WM5ALPHASTATE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5Float4.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM AlphaState : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(AlphaState);

public:
    // Construction and destruction.
    AlphaState ();
    virtual ~AlphaState ();

    enum WM5_GRAPHICS_ITEM SrcBlendMode
    {
        SBM_ZERO,
        SBM_ONE,
        SBM_DST_COLOR,
        SBM_ONE_MINUS_DST_COLOR,
        SBM_SRC_ALPHA,
        SBM_ONE_MINUS_SRC_ALPHA,
        SBM_DST_ALPHA,
        SBM_ONE_MINUS_DST_ALPHA,
        SBM_SRC_ALPHA_SATURATE,
        SBM_CONSTANT_COLOR,
        SBM_ONE_MINUS_CONSTANT_COLOR,
        SBM_CONSTANT_ALPHA,
        SBM_ONE_MINUS_CONSTANT_ALPHA,
        SBM_QUANTITY
    };

    enum WM5_GRAPHICS_ITEM DstBlendMode
    {
        DBM_ZERO,
        DBM_ONE,
        DBM_SRC_COLOR,
        DBM_ONE_MINUS_SRC_COLOR,
        DBM_SRC_ALPHA,
        DBM_ONE_MINUS_SRC_ALPHA,
        DBM_DST_ALPHA,
        DBM_ONE_MINUS_DST_ALPHA,
        DBM_CONSTANT_COLOR,
        DBM_ONE_MINUS_CONSTANT_COLOR,
        DBM_CONSTANT_ALPHA,
        DBM_ONE_MINUS_CONSTANT_ALPHA,
        DBM_QUANTITY
    };

    enum WM5_GRAPHICS_ITEM CompareMode
    {
        CM_NEVER,
        CM_LESS,
        CM_EQUAL,
        CM_LEQUAL,
        CM_GREATER,
        CM_NOTEQUAL,
        CM_GEQUAL,
        CM_ALWAYS,
        CM_QUANTITY
    };

    bool BlendEnabled;      // default: false
    SrcBlendMode SrcBlend;  // default: SBM_SRC_ALPHA
    DstBlendMode DstBlend;  // default: DBM_ONE_MINUS_SRC_ALPHA
    bool CompareEnabled;    // default: false
    CompareMode Compare;    // default: CM_ALWAYS
    float Reference;        // default: 0, always in [0,1]
    Float4 ConstantColor;   // default: (0,0,0,0)
};

WM5_REGISTER_STREAM(AlphaState);
typedef Pointer0<AlphaState> AlphaStatePtr;

}

#endif
