// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SURFACE_H
#define WM5SURFACE_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Surface
{
public:
    // Abstract base class.
    virtual ~Surface ();

protected:
    Surface ();
};

typedef Surface<float> Surfacef;
typedef Surface<double> Surfaced;

}

#endif
