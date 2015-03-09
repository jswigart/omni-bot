// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLOAT2ARRAY_H
#define WM5FLOAT2ARRAY_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5Float2.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Float2Array : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Float2Array);

public:
    // Construction and destruction.
    Float2Array (int numElements = 0, Float2* elements = 0);
    virtual ~Float2Array ();

    // Member access.
    inline int GetNumElements () const;
    inline Float2* GetData () const;
    inline operator const Float2* () const;
    inline operator Float2* ();
    inline const Float2& operator[] (int i) const;
    inline Float2& operator[] (int i);

protected:
    int mNumElements;
    Float2* mElements;
};

WM5_REGISTER_STREAM(Float2Array);
typedef Pointer0<Float2Array> Float2ArrayPtr;
#include "Wm5Float2Array.inl"

}

#endif
