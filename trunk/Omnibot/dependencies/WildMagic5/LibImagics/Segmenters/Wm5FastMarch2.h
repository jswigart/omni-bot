// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FASTMARCH2_H
#define WM5FASTMARCH2_H

#include "Wm5ImagicsLIB.h"
#include "Wm5FastMarch.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM FastMarch2 : public FastMarch
{
public:
    // Construction and destruction.
    FastMarch2 (int xBound, int yBound, float xSpacing, float ySpacing,
        const float* speeds, const std::vector<int>& seeds);

    FastMarch2 (int xBound, int yBound, float xSpacing, float ySpacing,
        const float speed, const std::vector<int>& seeds);

    virtual ~FastMarch2 ();

    // Member access.
    inline int GetXBound () const;
    inline int GetYBound () const;
    inline float GetXSpacing () const;
    inline float GetYSpacing () const;
    inline int Index (int x, int y) const;

    // Pixel classification.
    virtual void GetBoundary (std::vector<int>& boundary) const;
    virtual bool IsBoundary (int i) const;

    // Run one step of the fast marching algorithm.
    virtual void Iterate ();

protected:
    // Called by the constructors.
    void Initialize (int xBound, int yBound, float xSpacing,
        float ySpacing);

    // Called by Iterate().
    void ComputeTime (int i);

    int mXBound, mYBound, mXBm1, mYBm1;
    float mXSpacing, mYSpacing, mInvXSpacing, mInvYSpacing;
};

#include "Wm5FastMarch2.inl"

}

#endif
