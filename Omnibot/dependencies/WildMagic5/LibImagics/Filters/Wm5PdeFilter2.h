// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PDEFILTER2_H
#define WM5PDEFILTER2_H

#include "Wm5ImagicsLIB.h"
#include "Wm5PdeFilter.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM PdeFilter2 : public PdeFilter
{
public:
    // Abstract base class.
    PdeFilter2 (int xBound, int yBound, float xSpacing, float ySpacing,
        const float* data, const bool* mask, float borderValue,
        ScaleType scaleType);

    virtual ~PdeFilter2 ();

    // Member access.  The internal 2D images for "data" and "mask" are
    // copies of the inputs to the constructor but padded with a 1-pixel
    // thick border to support filtering on the image boundary.  These images
    // are of size (xbound+2)-by-(ybound+2).  The inputs (x,y) to GetData and
    // GetMask are constrained to 0 <= x < xbound and 0 <= y < ybound.  The
    // correct lookups into the padded arrays are handled internally.
    inline int GetXBound () const;
    inline int GetYBound () const;
    inline float GetXSpacing () const;
    inline float GetYSpacing () const;

    // Pixel access and derivative estimation.  The lookups into the padded
    // data are handled correctly.  The estimation involves only the 3-by-3
    // neighborhood of (x,y), where 0 <= x < xbound and 0 <= y < ybound.
    // [If larger neighborhoods are desired at a later date, the padding and
    // associated code must be adjusted accordingly.]
    inline float GetU (int x, int y) const;
    inline float GetUx (int x, int y) const;
    inline float GetUy (int x, int y) const;
    inline float GetUxx (int x, int y) const;
    inline float GetUxy (int x, int y) const;
    inline float GetUyy (int x, int y) const;
    inline bool GetMask (int x, int y) const;

protected:
    // Assign values to the 1-pixel image border.
    void AssignDirichletImageBorder ();
    void AssignNeumannImageBorder ();

    // Assign values to the 1-pixel mask border.
    void AssignDirichletMaskBorder ();
    void AssignNeumannMaskBorder ();

    // This function recomputes the boundary values when Neumann conditions
    // are used.  If a derived class overrides this, it must call the
    // base-class OnPreUpdate first.
    virtual void OnPreUpdate ();

    // Iterate over all the pixels and call OnUpdate(x,y) for each pixel that
    // is not masked out.
    virtual void OnUpdate ();

    // If a derived class overrides this, it must call the base-class
    // OnPostUpdate last.  The base-class function swaps the buffers for the
    // next pass.
    virtual void OnPostUpdate ();

    // The per-pixel processing depends on the PDE algorithm.  The (x,y) must
    // be in padded coordinates: 1 <= x <= xbound and 1 <= y <= ybound.
    virtual void OnUpdate (int x, int y) = 0;

    // Copy source data to temporary storage.
    void LookUp5 (int x, int y);
    void LookUp9 (int x, int y);

    // Image parameters.
    int mXBound, mYBound;
    float mXSpacing;       // dx
    float mYSpacing;       // dy
    float mInvDx;          // 1/dx
    float mInvDy;          // 1/dy
    float mHalfInvDx;      // 1/(2*dx)
    float mHalfInvDy;      // 1/(2*dy)
    float mInvDxDx;        // 1/(dx*dx)
    float mFourthInvDxDy;  // 1/(4*dx*dy)
    float mInvDyDy;        // 1/(dy*dy)

    // Temporary storage for 3x3 neighborhood.  In the notation m_fUxy, the
    // x and y indices are in {m,z,p}, referring to subtract 1 (m), no change
    // (z), or add 1 (p) to the appropriate index.
    float mUmm, mUzm, mUpm;
    float mUmz, mUzz, mUpz;
    float mUmp, mUzp, mUpp;

    // Successive iterations toggle between two buffers.
    float** mSrc;
    float** mDst;
    bool** mMask;
};

#include "Wm5PdeFilter2.inl"

}

#endif
