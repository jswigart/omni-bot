// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5PdeFilter3.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdeFilter3::PdeFilter3 (int xBound, int yBound, int zBound,
    float xSpacing, float ySpacing, float zSpacing, const float* data,
    const bool* mask, float borderValue, ScaleType scaleType)
    :
    PdeFilter(xBound*yBound*zBound, data, borderValue, scaleType),
    mXBound(xBound),
    mYBound(yBound),
    mZBound(zBound),
    mXSpacing(xSpacing),
    mYSpacing(ySpacing),
    mZSpacing(zSpacing),
    mInvDx(1.0f/xSpacing),
    mInvDy(1.0f/ySpacing),
    mInvDz(1.0f/zSpacing),
    mHalfInvDx(0.5f*mInvDx),
    mHalfInvDy(0.5f*mInvDy),
    mHalfInvDz(0.5f*mInvDz),
    mInvDxDx(mInvDx*mInvDx),
    mFourthInvDxDy(mHalfInvDx*mHalfInvDy),
    mFourthInvDxDz(mHalfInvDx*mHalfInvDz),
    mInvDyDy(mInvDy*mInvDy),
    mFourthInvDyDz(mHalfInvDy*mHalfInvDz),
    mInvDzDz(mInvDz*mInvDz)
{
    // Create two buffers for filtering, one a source buffer and one a
    // destination buffer.  Their roles are reversed on each iteration of
    // the filter.
    int xBp2 = mXBound + 2, yBp2 = mYBound + 2, zBp2 = mZBound + 2;
    mSrc = new3<float>(xBp2, yBp2, zBp2);
    mDst = new3<float>(xBp2, yBp2, zBp2);
    if (mask)
    {
        mMask = new3<bool>(xBp2, yBp2, zBp2);

        // This sets all mask values to 'false' initially.  The interior mask
        // values will be filled in later using the input mask.  The
        // boundary mask values will remain 'false'.
        memset(mMask[0][0], 0, xBp2*yBp2*zBp2*sizeof(bool));
    }
    else
    {
        mMask = 0;
    }

    // Copy the inputs data and mask to the interior elements of
    // mSrc and mMask.
    int x, y, z, xp, yp, zp, i;
    for (z = 0, zp = 1, i = 0; z < mZBound; ++z, ++zp)
    {
        for (y = 0, yp = 1; y < mYBound; ++y, ++yp)
        {
            for (x = 0, xp = 1; x < mXBound; ++x, ++xp, ++i)
            {
                mSrc[zp][yp][xp] = mOffset + (data[i] - mMin)*mScale;

                if (mMask)
                {
                    mMask[zp][yp][xp] = mask[i];
                }
            }
        }
    }

    // Assign values to the 1-voxel thick border.
    if (mBorderValue != Mathf::MAX_REAL)
    {
        AssignDirichletImageBorder();
    }
    else
    {
        AssignNeumannImageBorder();
    }

    // To handle masks that do not cover the entire image, assign values to
    // those voxels that are 26-neighbors of the mask voxels.
    if (mMask)
    {
        if (mBorderValue != Mathf::MAX_REAL)
        {
            AssignDirichletMaskBorder();
        }
        else
        {
            AssignNeumannMaskBorder();
        }
    }
}
//----------------------------------------------------------------------------
PdeFilter3::~PdeFilter3 ()
{
    delete3(mSrc);
    delete3(mDst);
    delete3(mMask);
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignDirichletImageBorder ()
{
    int xBp1 = mXBound + 1, yBp1 = mYBound + 1, zBp1 = mZBound + 1;
    int x, y, z;

    // vertex (0,0,0)
    mSrc[0][0][0] = mBorderValue;
    mDst[0][0][0] = mBorderValue;
    if (mMask)
    {
        mMask[0][0][0] = false;
    }

    // vertex (xmax,0,0)
    mSrc[0][0][xBp1] = mBorderValue;
    mDst[0][0][xBp1] = mBorderValue;
    if (mMask)
    {
        mMask[0][0][xBp1] = false;
    }

    // vertex (0,ymax,0)
    mSrc[0][yBp1][0] = mBorderValue;
    mDst[0][yBp1][0] = mBorderValue;
    if (mMask)
    {
        mMask[0][yBp1][0] = false;
    }

    // vertex (xmax,ymax,0)
    mSrc[0][yBp1][xBp1] = mBorderValue;
    mDst[0][yBp1][xBp1] = mBorderValue;
    if (mMask)
    {
        mMask[0][yBp1][xBp1] = false;
    }

    // vertex (0,0,zmax)
    mSrc[zBp1][0][0] = mBorderValue;
    mDst[zBp1][0][0] = mBorderValue;
    if (mMask)
    {
        mMask[zBp1][0][0] = false;
    }

    // vertex (xmax,0,zmax)
    mSrc[zBp1][0][xBp1] = mBorderValue;
    mDst[zBp1][0][xBp1] = mBorderValue;
    if (mMask)
    {
        mMask[zBp1][0][xBp1] = false;
    }

    // vertex (0,ymax,zmax)
    mSrc[zBp1][yBp1][0] = mBorderValue;
    mDst[zBp1][yBp1][0] = mBorderValue;
    if (mMask)
    {
        mMask[zBp1][yBp1][0] = false;
    }

    // vertex (xmax,ymax,zmax)
    mSrc[zBp1][yBp1][xBp1] = mBorderValue;
    mDst[zBp1][yBp1][xBp1] = mBorderValue;
    if (mMask)
    {
        mMask[zBp1][yBp1][xBp1] = false;
    }

    // edges (x,0,0) and (x,ymax,0)
    for (x = 1; x <= mXBound; ++x)
    {
        mSrc[0][0][x] = mBorderValue;
        mDst[0][0][x] = mBorderValue;
        if (mMask)
        {
            mMask[0][0][x] = false;
        }

        mSrc[0][yBp1][x] = mBorderValue;
        mDst[0][yBp1][x] = mBorderValue;
        if (mMask)
        {
            mMask[0][yBp1][x] = false;
        }
    }

    // edges (0,y,0) and (xmax,y,0)
    for (y = 1; y <= mYBound; ++y)
    {
        mSrc[0][y][0] = mBorderValue;
        mDst[0][y][0] = mBorderValue;
        if (mMask)
        {
            mMask[0][y][0] = false;
        }

        mSrc[0][y][xBp1] = mBorderValue;
        mDst[0][y][xBp1] = mBorderValue;
        if (mMask)
        {
            mMask[0][y][xBp1] = false;
        }
    }

    // edges (x,0,zmax) and (x,ymax,zmax)
    for (x = 1; x <= mXBound; ++x)
    {
        mSrc[zBp1][0][x] = mBorderValue;
        mDst[zBp1][0][x] = mBorderValue;
        if (mMask)
        {
            mMask[zBp1][0][x] = false;
        }

        mSrc[zBp1][yBp1][x] = mBorderValue;
        mDst[zBp1][yBp1][x] = mBorderValue;
        if (mMask)
        {
            mMask[zBp1][yBp1][x] = false;
        }
    }

    // edges (0,y,zmax) and (xmax,y,zmax)
    for (y = 1; y <= mYBound; ++y)
    {
        mSrc[zBp1][y][0] = mBorderValue;
        mDst[zBp1][y][0] = mBorderValue;
        if (mMask)
        {
            mMask[zBp1][y][0] = false;
        }

        mSrc[zBp1][y][xBp1] = mBorderValue;
        mDst[zBp1][y][xBp1] = mBorderValue;
        if (mMask)
        {
            mMask[zBp1][y][xBp1] = false;
        }
    }

    // edges (0,0,z) and (xmax,0,z)
    for (z = 1; z <= mZBound; ++z)
    {
        mSrc[z][0][0] = mBorderValue;
        mDst[z][0][0] = mBorderValue;
        if (mMask)
        {
            mMask[z][0][0] = false;
        }

        mSrc[z][0][xBp1] = mBorderValue;
        mDst[z][0][xBp1] = mBorderValue;
        if (mMask)
        {
            mMask[z][0][xBp1] = false;
        }
    }

    // edges (0,ymax,z) and (xmax,ymax,z)
    for (z = 1; z <= mZBound; ++z)
    {
        mSrc[z][yBp1][0] = mBorderValue;
        mDst[z][yBp1][0] = mBorderValue;
        if (mMask)
        {
            mMask[z][yBp1][0] = false;
        }

        mSrc[z][yBp1][xBp1] = mBorderValue;
        mDst[z][yBp1][xBp1] = mBorderValue;
        if (mMask)
        {
            mMask[z][yBp1][xBp1] = false;
        }
    }

    // faces (x,y,0) and (x,y,zmax)
    for (y = 1; y <= mYBound; ++y)
    {
        for (x = 1; x <= mXBound; ++x)
        {
            mSrc[0][y][x] = mBorderValue;
            mDst[0][y][x] = mBorderValue;
            if (mMask)
            {
                mMask[0][y][x] = false;
            }

            mSrc[zBp1][y][x] = mBorderValue;
            mDst[zBp1][y][x] = mBorderValue;
            if (mMask)
            {
                mMask[zBp1][y][x] = false;
            }
        }
    }

    // faces (x,0,z) and (x,ymax,z)
    for (z = 1; z <= mZBound; ++z)
    {
        for (x = 1; x <= mXBound; ++x)
        {
            mSrc[z][0][x] = mBorderValue;
            mDst[z][0][x] = mBorderValue;
            if (mMask)
            {
                mMask[z][0][x] = false;
            }

            mSrc[z][yBp1][x] = mBorderValue;
            mDst[z][yBp1][x] = mBorderValue;
            if (mMask)
            {
                mMask[z][yBp1][x] = false;
            }
        }
    }

    // faces (0,y,z) and (xmax,y,z)
    for (z = 1; z <= mZBound; ++z)
    {
        for (y = 1; y <= mYBound; ++y)
        {
            mSrc[z][y][0] = mBorderValue;
            mDst[z][y][0] = mBorderValue;
            if (mMask)
            {
                mMask[z][y][0] = false;
            }

            mSrc[z][y][xBp1] = mBorderValue;
            mDst[z][y][xBp1] = mBorderValue;
            if (mMask)
            {
                mMask[z][y][xBp1] = false;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignNeumannImageBorder ()
{
    int xBp1 = mXBound + 1, yBp1 = mYBound + 1, zBp1 = mZBound + 1;
    int x, y, z;
    float duplicate;

    // vertex (0,0,0)
    duplicate = mSrc[1][1][1];
    mSrc[0][0][0] = duplicate;
    mDst[0][0][0] = duplicate;
    if (mMask)
    {
        mMask[0][0][0] = false;
    }

    // vertex (xmax,0,0)
    duplicate = mSrc[1][1][mXBound];
    mSrc[0][0][xBp1] = duplicate;
    mDst[0][0][xBp1] = duplicate;
    if (mMask)
    {
        mMask[0][0][xBp1] = false;
    }

    // vertex (0,ymax,0)
    duplicate = mSrc[1][mYBound][1];
    mSrc[0][yBp1][0] = duplicate;
    mDst[0][yBp1][0] = duplicate;
    if (mMask)
    {
        mMask[0][yBp1][0] = false;
    }

    // vertex (xmax,ymax,0)
    duplicate = mSrc[1][mYBound][mXBound];
    mSrc[0][yBp1][xBp1] = duplicate;
    mDst[0][yBp1][xBp1] = duplicate;
    if (mMask)
    {
        mMask[0][yBp1][xBp1] = false;
    }

    // vertex (0,0,zmax)
    duplicate = mSrc[mZBound][1][1];
    mSrc[zBp1][0][0] = duplicate;
    mDst[zBp1][0][0] = duplicate;
    if (mMask)
    {
        mMask[zBp1][0][0] = false;
    }

    // vertex (xmax,0,zmax)
    duplicate = mSrc[mZBound][1][mXBound];
    mSrc[zBp1][0][xBp1] = duplicate;
    mDst[zBp1][0][xBp1] = duplicate;
    if (mMask)
    {
        mMask[zBp1][0][xBp1] = false;
    }

    // vertex (0,ymax,zmax)
    duplicate = mSrc[mZBound][mYBound][1];
    mSrc[zBp1][yBp1][0] = duplicate;
    mDst[zBp1][yBp1][0] = duplicate;
    if (mMask)
    {
        mMask[zBp1][yBp1][0] = false;
    }

    // vertex (xmax,ymax,zmax)
    duplicate = mSrc[mZBound][mYBound][mXBound];
    mSrc[zBp1][yBp1][xBp1] = duplicate;
    mDst[zBp1][yBp1][xBp1] = duplicate;
    if (mMask)
    {
        mMask[zBp1][yBp1][xBp1] = false;
    }

    // edges (x,0,0) and (x,ymax,0)
    for (x = 1; x <= mXBound; ++x)
    {
        duplicate = mSrc[1][1][x];
        mSrc[0][0][x] = duplicate;
        mDst[0][0][x] = duplicate;
        if (mMask)
        {
            mMask[0][0][x] = false;
        }

        duplicate = mSrc[1][mYBound][x];
        mSrc[0][yBp1][x] = duplicate;
        mDst[0][yBp1][x] = duplicate;
        if (mMask)
        {
            mMask[0][yBp1][x] = false;
        }
    }

    // edges (0,y,0) and (xmax,y,0)
    for (y = 1; y <= mYBound; ++y)
    {
        duplicate = mSrc[1][y][1];
        mSrc[0][y][0] = duplicate;
        mDst[0][y][0] = duplicate;
        if (mMask)
        {
            mMask[0][y][0] = false;
        }

        duplicate = mSrc[1][y][mXBound];
        mSrc[0][y][xBp1] = duplicate;
        mDst[0][y][xBp1] = duplicate;
        if (mMask)
        {
            mMask[0][y][xBp1] = false;
        }
    }

    // edges (x,0,zmax) and (x,ymax,zmax)
    for (x = 1; x <= mXBound; ++x)
    {
        duplicate = mSrc[mZBound][1][x];
        mSrc[zBp1][0][x] = duplicate;
        mDst[zBp1][0][x] = duplicate;
        if (mMask)
        {
            mMask[zBp1][0][x] = false;
        }

        duplicate = mSrc[mZBound][mYBound][x];
        mSrc[zBp1][yBp1][x] = duplicate;
        mDst[zBp1][yBp1][x] = duplicate;
        if (mMask)
        {
            mMask[zBp1][yBp1][x] = false;
        }
    }

    // edges (0,y,zmax) and (xmax,y,zmax)
    for (y = 1; y <= mYBound; ++y)
    {
        duplicate = mSrc[mZBound][y][1];
        mSrc[zBp1][y][0] = duplicate;
        mDst[zBp1][y][0] = duplicate;
        if (mMask)
        {
            mMask[zBp1][y][0] = false;
        }

        duplicate = mSrc[mZBound][y][mXBound];
        mSrc[zBp1][y][xBp1] = duplicate;
        mDst[zBp1][y][xBp1] = duplicate;
        if (mMask)
        {
            mMask[zBp1][y][xBp1] = false;
        }
    }

    // edges (0,0,z) and (xmax,0,z)
    for (z = 1; z <= mZBound; ++z)
    {
        duplicate = mSrc[z][1][1];
        mSrc[z][0][0] = duplicate;
        mDst[z][0][0] = duplicate;
        if (mMask)
        {
            mMask[z][0][0] = false;
        }

        duplicate = mSrc[z][1][mXBound];
        mSrc[z][0][xBp1] = duplicate;
        mDst[z][0][xBp1] = duplicate;
        if (mMask)
        {
            mMask[z][0][xBp1] = false;
        }
    }

    // edges (0,ymax,z) and (xmax,ymax,z)
    for (z = 1; z <= mZBound; ++z)
    {
        duplicate = mSrc[z][mYBound][1];
        mSrc[z][yBp1][0] = duplicate;
        mDst[z][yBp1][0] = duplicate;
        if (mMask)
        {
            mMask[z][yBp1][0] = false;
        }

        duplicate = mSrc[z][mYBound][mXBound];
        mSrc[z][yBp1][xBp1] = duplicate;
        mDst[z][yBp1][xBp1] = duplicate;
        if (mMask)
        {
            mMask[z][yBp1][xBp1] = false;
        }
    }

    // faces (x,y,0) and (x,y,zmax)
    for (y = 1; y <= mYBound; ++y)
    {
        for (x = 1; x <= mXBound; ++x)
        {
            duplicate = mSrc[1][y][x];
            mSrc[0][y][x] = duplicate;
            mDst[0][y][x] = duplicate;
            if (mMask)
            {
                mMask[0][y][x] = false;
            }

            duplicate = mSrc[mZBound][y][x];
            mSrc[zBp1][y][x] = duplicate;
            mDst[zBp1][y][x] = duplicate;
            if (mMask)
            {
                mMask[zBp1][y][x] = false;
            }
        }
    }

    // faces (x,0,z) and (x,ymax,z)
    for (z = 1; z <= mZBound; ++z)
    {
        for (x = 1; x <= mXBound; ++x)
        {
            duplicate = mSrc[z][1][x];
            mSrc[z][0][x] = duplicate;
            mDst[z][0][x] = duplicate;
            if (mMask)
            {
                mMask[z][0][x] = false;
            }

            duplicate = mSrc[z][mYBound][x];
            mSrc[z][yBp1][x] = duplicate;
            mDst[z][yBp1][x] = duplicate;
            if (mMask)
            {
                mMask[z][yBp1][x] = false;
            }
        }
    }

    // faces (0,y,z) and (xmax,y,z)
    for (z = 1; z <= mZBound; ++z)
    {
        for (y = 1; y <= mYBound; ++y)
        {
            duplicate = mSrc[z][y][1];
            mSrc[z][y][0] = duplicate;
            mDst[z][y][0] = duplicate;
            if (mMask)
            {
                mMask[z][y][0] = false;
            }

            duplicate = mSrc[z][y][mXBound];
            mSrc[z][y][xBp1] = duplicate;
            mDst[z][y][xBp1] = duplicate;
            if (mMask)
            {
                mMask[z][y][xBp1] = false;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignDirichletMaskBorder ()
{
    if (mMask)
    {
        for (int z = 1; z <= mZBound; ++z)
        {
            for (int y = 1; y <= mYBound; ++y)
            {
                for (int x = 1; x <= mXBound; ++x)
                {
                    if (mMask[z][y][x])
                    {
                        continue;
                    }

                    bool found = false;
                    int i0, i1, i2, j0, j1, j2;
                    for (i2 = 0, j2 = z-1; i2 < 3 && !found; ++i2, ++j2)
                    {
                        for (i1 = 0, j1 = y-1; i1 < 3 && !found; ++i1, ++j1)
                        {
                            for (i0 = 0, j0 = x-1; i0 < 3; ++i0, ++j0)
                            {
                                if (mMask[j2][j1][j0])
                                {
                                    mSrc[z][y][x] = mBorderValue;
                                    mDst[z][y][x] = mBorderValue;
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        assertion(false, "Mask must exist\n");
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignNeumannMaskBorder ()
{
    // Recompute the values just outside the masked region.  This guarantees
    // that derivative estimations use the current values around the boundary.
    for (int z = 1; z <= mZBound; ++z)
    {
        for (int y = 1; y <= mYBound; ++y)
        {
            for (int x = 1; x <= mXBound; ++x)
            {
                if (mMask[z][y][x])
                {
                    continue;
                }

                int count = 0;
                float average = 0.0f;
                int i0, i1, i2, j0, j1, j2;
                for (i2 = 0, j2 = z-1; i2 < 3; ++i2, ++j2)
                {
                    for (i1 = 0, j1 = y-1; i1 < 3; ++i1, ++j1)
                    {
                        for (i0 = 0, j0 = x-1; i0 < 3; ++i0, ++j0)
                        {
                            if (mMask[j2][j1][j0])
                            {
                                average += mSrc[j2][j1][j0];
                                count++;
                            }
                        }
                    }
                }

                if (count > 0)
                {
                    average /= (float)count;
                    mSrc[z][y][x] = average;
                    mDst[z][y][x] = average;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::OnPreUpdate ()
{
    if (mMask && mBorderValue == Mathf::MAX_REAL)
    {
        // Neumann boundary conditions are in use, so recompute the mask
        // border.
        AssignNeumannMaskBorder();
    }
    // else: No mask has been specified or Dirichlet boundary conditions are
    // in use.  Nothing to do.
}
//----------------------------------------------------------------------------
void PdeFilter3::OnUpdate ()
{
    for (int z = 1; z <= mZBound; ++z)
    {
        for (int y = 1; y <= mYBound; ++y)
        {
            for (int x = 1; x <= mXBound; ++x)
            {
                if (!mMask || mMask[z][y][x])
                {
                    OnUpdate(x,y,z);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::OnPostUpdate ()
{
    // Swap the buffers for the next pass.
    float*** save = mSrc;
    mSrc = mDst;
    mDst = save;
}
//----------------------------------------------------------------------------
void PdeFilter3::LookUp7 (int x, int y, int z)
{
    int xm = x - 1, xp = x + 1;
    int ym = y - 1, yp = y + 1;
    int zm = z - 1, zp = z + 1;
    mUzzm = mSrc[zm][y ][x ];
    mUzmz = mSrc[z ][ym][x ];
    mUmzz = mSrc[z ][y ][xm];
    mUzzz = mSrc[z ][y ][x ];
    mUpzz = mSrc[z ][y ][xp];
    mUzpz = mSrc[z ][yp][x ];
    mUzzp = mSrc[zp][y ][x ];
}
//----------------------------------------------------------------------------
void PdeFilter3::LookUp27 (int x, int y, int z)
{
    int xm = x - 1, xp = x + 1;
    int ym = y - 1, yp = y + 1;
    int zm = z - 1, zp = z + 1;
    mUmmm = mSrc[zm][ym][xm];
    mUzmm = mSrc[zm][ym][x ];
    mUpmm = mSrc[zm][ym][xp];
    mUmzm = mSrc[zm][y ][xm];
    mUzzm = mSrc[zm][y ][x ];
    mUpzm = mSrc[zm][y ][xp];
    mUmpm = mSrc[zm][yp][xm];
    mUzpm = mSrc[zm][yp][x ];
    mUppm = mSrc[zm][yp][xp];
    mUmmz = mSrc[z ][ym][xm];
    mUzmz = mSrc[z ][ym][x ];
    mUpmz = mSrc[z ][ym][xp];
    mUmzz = mSrc[z ][y ][xm];
    mUzzz = mSrc[z ][y ][x ];
    mUpzz = mSrc[z ][y ][xp];
    mUmpz = mSrc[z ][yp][xm];
    mUzpz = mSrc[z ][yp][x ];
    mUppz = mSrc[z ][yp][xp];
    mUmmp = mSrc[zp][ym][xm];
    mUzmp = mSrc[zp][ym][x ];
    mUpmp = mSrc[zp][ym][xp];
    mUmzp = mSrc[zp][y ][xm];
    mUzzp = mSrc[zp][y ][x ];
    mUpzp = mSrc[zp][y ][xp];
    mUmpp = mSrc[zp][yp][xm];
    mUzpp = mSrc[zp][yp][x ];
    mUppp = mSrc[zp][yp][xp];
}
//----------------------------------------------------------------------------
