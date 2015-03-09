// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FASTMARCH3_H
#define WM5FASTMARCH3_H

#include "Wm5ImagicsLIB.h"
#include "Wm5FastMarch.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM FastMarch3 : public FastMarch
{
public:
    // Construction and destruction.
    FastMarch3 (int xBound, int yBound, int zBound, float xSpacing,
        float ySpacing, float zSpacing, const float* speeds,
        const std::vector<int>& seeds);

    FastMarch3 (int xBound, int yBound, int zBound, float xSpacing,
        float ySpacing, float zSpacing, const float speed,
        const std::vector<int>& seeds);

    virtual ~FastMarch3 ();

    // Member access.
    inline int GetXBound () const;
    inline int GetYBound () const;
    inline int GetZBound () const;
    inline float GetXSpacing () const;
    inline float GetYSpacing () const;
    inline float GetZSpacing () const;
    inline int Index (int x, int y, int z) const;

    // Voxel classification.
    virtual void GetBoundary (std::vector<int>& rkBoundary) const;
    virtual bool IsBoundary (int i) const;

    // Run one step of the fast marching algorithm.
    virtual void Iterate ();

protected:
    // Called by the constructors.
    void Initialize (int xBound, int yBound, int zBound, float xSpacing,
        float ySpacing, float zSpacing);

    // Called by Iterate().
    void ComputeTime (int i);

    int mXBound, mYBound, mZBound, mXYBound;
    int mXBm1, mYBm1, mZBm1;
    float mXSpacing, mYSpacing, mZSpacing;
    float mInvXSpacing, mInvYSpacing, mInvZSpacing;

    static const float msOneThird;
};

#include "Wm5FastMarch3.inl"

}

#endif
