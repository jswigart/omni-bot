// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/07)

#ifndef WM5FLOAT3_H
#define WM5FLOAT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class Float3 : public Tuple<3,float>
{
public:
    // Construction and destruction.
    inline Float3 ();   // uninitialized
    inline ~Float3 ();  // hides ~Tuple<3,float>
    inline Float3 (float f0, float f1, float f2);
    inline Float3 (const Float3& tuple);

    // Assignment.
    inline Float3& operator= (const Float3& tuple);
};

#include "Wm5Float3.inl"

}

#endif
