// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/07)

#ifndef WM5FLOAT2_H
#define WM5FLOAT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class Float2 : public Tuple<2,float>
{
public:
    // Construction and destruction.
    inline Float2 ();   // uninitialized
    inline ~Float2 ();  // hides ~Tuple<2,float>
    inline Float2 (float f0, float f1);
    inline Float2 (const Float2& tuple);

    // Assignment.
    inline Float2& operator= (const Float2& tuple);
};

#include "Wm5Float2.inl"

}

#endif
