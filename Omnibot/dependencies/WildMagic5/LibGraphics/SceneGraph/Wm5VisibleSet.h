// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VISIBLESET_H
#define WM5VISIBLESET_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Assert.h"

namespace Wm5
{

class Spatial;

class WM5_GRAPHICS_ITEM VisibleSet
{
public:
    // Construction and destruction.
    VisibleSet ();
    ~VisibleSet ();

    // Access to the elements of the visible set.
    inline int GetNumVisible () const;
    inline const std::vector<Spatial*>& GetAllVisible () const;
    inline Spatial* GetVisible (int i) const;

    // Insert a visible object into the set.
    void Insert (Spatial* visible);

    // Set the number of elements to zero.
    inline void Clear ();

private:
    int mNumVisible;
    std::vector<Spatial*> mVisible;
};

#include "Wm5VisibleSet.inl"

}

#endif
