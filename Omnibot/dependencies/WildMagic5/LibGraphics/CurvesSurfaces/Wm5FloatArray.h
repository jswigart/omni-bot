// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLOATARRAY_H
#define WM5FLOATARRAY_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM FloatArray : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(FloatArray);

public:
    // Construction and destruction.
    FloatArray (int numElements = 0, float* elements = 0);
    virtual ~FloatArray ();

    // Member access.
    inline int GetNumElements () const;
    inline float* GetData () const;
    inline operator const float* () const;
    inline operator float* ();
    inline const float& operator[] (int i) const;
    inline float& operator[] (int i);

protected:
    int mNumElements;
    float* mElements;
};

WM5_REGISTER_STREAM(FloatArray);
typedef Pointer0<FloatArray> FloatArrayPtr;
#include "Wm5FloatArray.inl"

}

#endif
