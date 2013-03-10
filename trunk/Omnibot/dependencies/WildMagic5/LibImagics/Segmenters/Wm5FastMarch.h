// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FASTMARCH_H
#define WM5FASTMARCH_H

#include "Wm5ImagicsLIB.h"
#include "Wm5Math.h"
#include "Wm5MinHeap.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM FastMarch
{
public:
    // Abstract base class.
    virtual ~FastMarch ();

    // Member access.
    inline void SetTime (int i, float time);
    inline float GetTime (int i) const;
    inline int GetQuantity () const;
    void GetTimeExtremes (float& minValue, float& maxValue) const;

    // Image element classification.
    inline bool IsValid (int i) const;
    inline bool IsTrial (int i) const;
    inline bool IsFar (int i) const;
    inline bool IsZeroSpeed (int i) const;
    inline bool IsInterior (int i) const;
    void GetInterior (std::vector<int>& interior) const;
    virtual void GetBoundary (std::vector<int>& boundary) const = 0;
    virtual bool IsBoundary (int i) const = 0;

    // Run one step of the fast marching algorithm.
    virtual void Iterate () = 0;

protected:
    // Constructors.
    FastMarch (int quantity, const float* speeds,
        const std::vector<int>& seeds);

    FastMarch (int quantity, const float speed,
        const std::vector<int>& seeds);

    // Called by the constructors.
    void Initialize (int quantity, const std::vector<int>& seeds);
    void InitializeSpeed (const float* speeds);
    void InitializeSpeed (const float speed);

    int mQuantity;
    float* mInvSpeeds;
    float* mTimes;
    MinHeap<int,float> mHeap;
    const MinHeapRecord<int,float>** mTrials;
};

#include "Wm5FastMarch.inl"

}

#endif
