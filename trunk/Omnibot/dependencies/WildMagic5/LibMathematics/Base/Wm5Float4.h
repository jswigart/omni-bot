// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/07)

#ifndef WM5FLOAT4_H
#define WM5FLOAT4_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class Float4 : public Tuple<4,float>
{
public:
    // Construction and destruction.
    inline Float4 ();   // uninitialized
    inline ~Float4 ();  // hides ~Tuple<4,float>
    inline Float4 (float f0, float f1, float f2, float f3);
    inline Float4 (const Float4& tuple);

    // Assignment.
    inline Float4& operator= (const Float4& tuple);
};

#include "Wm5Float4.inl"

}

#endif
