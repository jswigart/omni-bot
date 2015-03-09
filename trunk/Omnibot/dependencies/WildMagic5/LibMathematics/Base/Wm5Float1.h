// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/07)

#ifndef WM5FLOAT1_H
#define WM5FLOAT1_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class Float1 : public Tuple<1,float>
{
public:
    // Construction and destruction.
    inline Float1 ();   // uninitialized
    inline ~Float1 ();  // hides ~Tuple<1,float>
    inline Float1 (float f0);
    inline Float1 (const Float1& tuple);

    // Assignment.
    inline Float1& operator= (const Float1& tuple);
};

#include "Wm5Float1.inl"

}

#endif
