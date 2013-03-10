// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ENDIAN_H
#define WM5ENDIAN_H

#include "Wm5CoreLIB.h"

namespace Wm5
{

class WM5_CORE_ITEM Endian
{
public:
    // Test whether the byte order of the machine is big endian.
    static bool IsBig ();

    // Test whether the byte order of the machine is little endian.
    static bool IsLittle ();

    // Swap the byte order.  The functions with itemSize input are designed
    // to be used with template classes, where the item size depends on the
    // template parameter.
    static void Swap2 (void* datum);
    static void Swap2 (int numItems, void* data);
    static void Swap4 (void* datum);
    static void Swap4 (int numItems, void* data);
    static void Swap8 (void* datum);
    static void Swap8 (int numItems, void* data);
    static void Swap (size_t itemSize, void* datum);
    static void Swap (size_t itemSize, int numItems, void* data);

private:
    static bool msIsLittle;
};

}

#endif
