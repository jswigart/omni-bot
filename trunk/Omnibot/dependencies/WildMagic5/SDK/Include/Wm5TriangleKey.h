// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TRIANGLEKEY_H
#define WM5TRIANGLEKEY_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM TriangleKey
{
public:
    TriangleKey (int v0 = -1, int v1 = -1, int v2 = -1);

    bool operator< (const TriangleKey& key) const;
    operator size_t () const;

    int V[3];
};

#include "Wm5TriangleKey.inl"

}

#endif
