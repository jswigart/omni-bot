// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PDEFILTER_H
#define WM5PDEFILTER_H

#include "Wm5ImagicsLIB.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM PdeFilter
{
public:
    enum WM5_IMAGICS_ITEM ScaleType
    {
        // The data is processed as is.
        ST_NONE,

        // The data range is d in [min,max].  The scaled values are d'.

        // d' = (d-min)/(max-min) in [0,1]
        ST_UNIT,

        // d' = -1 + 2*(d-min)/(max-min) in [-1,1]
        ST_SYMMETRIC,

        // max > -min:  d' = d/max in [min/max,1]
        // max < -min:  d' = -d/min in [-1,-max/min]
        ST_PRESERVE_ZERO
    };

    // The abstract base class for all PDE-based filters.
    virtual ~PdeFilter ();

    // Member access.
    inline int GetQuantity () const;
    inline float GetBorderValue () const;
    inline ScaleType GetScaleType () const;

    // Access to the time step for the PDE solver.
    inline void SetTimeStep (float timeStep);
    inline float GetTimeStep () const;

    // This function calls OnPreUpdate, OnUpdate, and OnPostUpdate, in that
    // order.
    void Update ();

protected:
    PdeFilter (int quantity, const float* data, float borderValue,
        ScaleType scaleType);

    // The derived classes for 2D and 3D implement this to recompute the
    // boundary values when Neumann conditions are used.  If derived classes
    // built on top of the 2D or 3D classes implement this also, they must
    // call the base-class OnPreUpdate first.
    virtual void OnPreUpdate () = 0;

    // The derived classes for 2D and 3D implement this to iterate over the
    // image elements, updating an element only if it is not masked out.
    virtual void OnUpdate () = 0;

    // The derived classes for 2D and 3D implement this to swap the buffers
    // for the next pass.  If derived classes built on top of the 2D or 3D
    // classes implement this also, they must call the base-class OnPostUpdate
    // last. 
    virtual void OnPostUpdate () = 0;

    // The number of image elements.
    int mQuantity;

    // When set to Mathf::MAX_REAL, Neumann conditions are in use (zero-valued
    // derivatives on the image border).  Dirichlet conditions are used,
    // otherwise (image is constant on the border).
    float mBorderValue;

    // This member stores how the image data was transformed during the
    // constructor call.
    ScaleType mScaleType;
    float mMin, mOffset, mScale;

    // The time step for the PDE solver.  The stability of an algorithm
    // depends on the magnitude of the time step, but the magnitude itself
    // depends on the algorithm.
    float mTimeStep;
};

#include "Wm5PdeFilter.inl"

}

#endif
