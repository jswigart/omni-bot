// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PDEFILTER3_H
#define WM5PDEFILTER3_H

#include "Wm5ImagicsLIB.h"
#include "Wm5PdeFilter.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM PdeFilter3 : public PdeFilter
{
public:
    // Abstract base class.
    PdeFilter3 (int xBound, int yBound, int zBound, float xSpacing,
        float ySpacing, float zSpacing, const float* data,
        const bool* mask, float borderValue, ScaleType scaleType);

    virtual ~PdeFilter3 ();

    // Member access.  The internal 2D images for "data" and "mask" are
    // copies of the inputs to the constructor but padded with a 1-pixel
    // thick border to support filtering on the image boundary.  These images
    // are of size (xbound+2)-by-(ybound+2)-by-(zbound+2).  The inputs (x,y,z)
    // to GetData and GetMask are constrained to 0 <= x < xbound,
    // 0 <= y < ybound, and 0 <= z < zbound.  The correct lookups into the
    // padded arrays are handled internally.
    inline int GetXBound () const;
    inline int GetYBound () const;
    inline int GetZBound () const;
    inline float GetXSpacing () const;
    inline float GetYSpacing () const;
    inline float GetZSpacing () const;

    // Voxel access and derivative estimation.  The lookups into the padded
    // data are handled correctly.  The estimation involves only the
    // 3-by-3-by-3 neighborhood of (x,y,z), where 0 <= x < xbound,
    // 0 <= y < ybound, and 0 <= z < zbound.  [If larger neighborhoods are
    // desired at a later date, the padding and associated code must be
    // adjusted accordingly.]
    inline float GetU (int x, int y, int z) const;
    inline float GetUx (int x, int y, int z) const;
    inline float GetUy (int x, int y, int z) const;
    inline float GetUz (int x, int y, int z) const;
    inline float GetUxx (int x, int y, int z) const;
    inline float GetUxy (int x, int y, int z) const;
    inline float GetUxz (int x, int y, int z) const;
    inline float GetUyy (int x, int y, int z) const;
    inline float GetUyz (int x, int y, int z) const;
    inline float GetUzz (int x, int y, int z) const;
    inline bool GetMask (int x, int y, int z) const;

protected:
    // Assign values to the 1-voxel image border.
    void AssignDirichletImageBorder ();
    void AssignNeumannImageBorder ();

    // Assign values to the 1-voxel mask border.
    void AssignDirichletMaskBorder ();
    void AssignNeumannMaskBorder ();

    // This function recomputes the boundary values when Neumann conditions
    // are used.  If a derived class overrides this, it must call the
    // base-class OnPreUpdate first.
    virtual void OnPreUpdate ();

    // Iterate over all the pixels and call OnUpdate(x,y,z) for each voxel
    // that is not masked out.
    virtual void OnUpdate ();

    // If a derived class overrides this, it must call the base-class
    // OnPostUpdate last.  The base-class function swaps the buffers for the
    // next pass.
    virtual void OnPostUpdate ();

    // The per-pixel processing depends on the PDE algorithm.  The (x,y,z)
    // must be in padded coordinates: 1 <= x <= xbound, 1 <= y <= ybound, and
    // 1 <= z <= zbound.
    virtual void OnUpdate (int x, int y, int z) = 0;

    // Copy source data to temporary storage.
    void LookUp7 (int x, int y, int z);
    void LookUp27 (int x, int y, int z);

    // Image parameters.
    int mXBound, mYBound, mZBound;
    float mXSpacing;       // dx
    float mYSpacing;       // dy
    float mZSpacing;       // dz
    float mInvDx;          // 1/dx
    float mInvDy;          // 1/dy
    float mInvDz;          // 1/dz
    float mHalfInvDx;      // 1/(2*dx)
    float mHalfInvDy;      // 1/(2*dy)
    float mHalfInvDz;      // 1/(2*dz)
    float mInvDxDx;        // 1/(dx*dx)
    float mFourthInvDxDy;  // 1/(4*dx*dy)
    float mFourthInvDxDz;  // 1/(4*dx*dz)
    float mInvDyDy;        // 1/(dy*dy)
    float mFourthInvDyDz;  // 1/(4*dy*dz)
    float mInvDzDz;        // 1/(dz*dz)

    // Temporary storage for 3x3x3 neighborhood.
    float mUmmm, mUzmm, mUpmm;
    float mUmzm, mUzzm, mUpzm;
    float mUmpm, mUzpm, mUppm;
    float mUmmz, mUzmz, mUpmz;
    float mUmzz, mUzzz, mUpzz;
    float mUmpz, mUzpz, mUppz;
    float mUmmp, mUzmp, mUpmp;
    float mUmzp, mUzzp, mUpzp;
    float mUmpp, mUzpp, mUppp;

    // Successive iterations toggle between two buffers.
    float*** mSrc;
    float*** mDst;
    bool*** mMask;
};

#include "Wm5PdeFilter3.inl"

}

#endif
