// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5PdeFilter2.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdeFilter2::PdeFilter2 (int xBound, int yBound, float xSpacing,
    float ySpacing, const float* data, const bool* mask, float borderValue,
    ScaleType scaleType)
    :
    PdeFilter(xBound*yBound, data, borderValue, scaleType),
    mXBound(xBound),
    mYBound(yBound),
    mXSpacing(xSpacing),
    mYSpacing(ySpacing),
    mInvDx(1.0f/xSpacing),
    mInvDy(1.0f/ySpacing),
    mHalfInvDx(0.5f*mInvDx),
    mHalfInvDy(0.5f*mInvDy),
    mInvDxDx(mInvDx*mInvDx),
    mFourthInvDxDy(mHalfInvDx*mHalfInvDy),
    mInvDyDy(mInvDy*mInvDy)
{
    // Create two buffers for filtering, one a source buffer and one a
    // destination buffer.  Their roles are reversed on each iteration of
    // the filter.
    int xBp2 = mXBound + 2, yBp2 = mYBound + 2;
    mSrc = new2<float>(xBp2, yBp2);
    mDst = new2<float>(xBp2, yBp2);
    if (mask)
    {
        mMask = new2<bool>(xBp2, yBp2);

        // This sets all mask values to 'false' initially.  The interior mask
        // values will be filled in later using the input mask.  The
        // boundary mask values will remain 'false'.
        memset(mMask[0], 0, xBp2*yBp2*sizeof(bool));
    }
    else
    {
        mMask = 0;
    }

    // Copy the inputs data and mask to the interior elements of mSrc
    // and mMask.
    int x, y, xp, yp, i;
    for (y = 0, yp = 1, i = 0; y < mYBound; ++y, ++yp)
    {
        for (x = 0, xp = 1; x < mXBound; ++x, ++xp, ++i)
        {
            mSrc[yp][xp] = mOffset + (data[i] - mMin)*mScale;
            if (mMask)
            {
                mMask[yp][xp] = mask[i];
            }
        }
    }

    // Assign values to the 1-pixel image border.
    if (mBorderValue != Mathf::MAX_REAL)
    {
        AssignDirichletImageBorder();
    }
    else
    {
        AssignNeumannImageBorder();
    }

    // To handle masks that do not cover the entire image, assign values to
    // those pixels that are 8-neighbors of the mask pixels.
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
PdeFilter2::~PdeFilter2 ()
{
    delete2(mSrc);
    delete2(mDst);
    delete2(mMask);
}
//----------------------------------------------------------------------------
void PdeFilter2::AssignDirichletImageBorder ()
{
    int xBp1 = mXBound + 1, yBp1 = mYBound + 1;
    int x, y;

    // vertex (0,0)
    mSrc[0][0] = mBorderValue;
    mDst[0][0] = mBorderValue;
    if (mMask)
    {
        mMask[0][0] = false;
    }

    // vertex (xmax,0)
    mSrc[0][xBp1] = mBorderValue;
    mDst[0][xBp1] = mBorderValue;
    if (mMask)
    {
        mMask[0][xBp1] = false;
    }

    // vertex (0,ymax)
    mSrc[yBp1][0] = mBorderValue;
    mDst[yBp1][0] = mBorderValue;
    if (mMask)
    {
        mMask[yBp1][0] = false;
    }

    // vertex (xmax,ymax)
    mSrc[yBp1][xBp1] = mBorderValue;
    mDst[yBp1][xBp1] = mBorderValue;
    if (mMask)
    {
        mMask[yBp1][xBp1] = false;
    }

    // edges (x,0) and (x,ymax)
    for (x = 1; x <= mXBound; ++x)
    {
        mSrc[0][x] = mBorderValue;
        mDst[0][x] = mBorderValue;
        if (mMask)
        {
            mMask[0][x] = false;
        }

        mSrc[yBp1][x] = mBorderValue;
        mDst[yBp1][x] = mBorderValue;
        if (mMask)
        {
            mMask[yBp1][x] = false;
        }
    }

    // edges (0,y) and (xmax,y)
    for (y = 1; y <= mYBound; ++y)
    {
        mSrc[y][0] = mBorderValue;
        mDst[y][0] = mBorderValue;
        if (mMask)
        {
            mMask[y][0] = false;
        }

        mSrc[y][xBp1] = mBorderValue;
        mDst[y][xBp1] = mBorderValue;
        if (mMask)
        {
            mMask[y][xBp1] = false;
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::AssignNeumannImageBorder ()
{
    int xBp1 = mXBound + 1, yBp1 = mYBound + 1;
    int x, y;
    float duplicate;

    // vertex (0,0)
    duplicate = mSrc[1][1];
    mSrc[0][0] = duplicate;
    mDst[0][0] = duplicate;
    if (mMask)
    {
        mMask[0][0] = false;
    }

    // vertex (xmax,0)
    duplicate = mSrc[1][mXBound];
    mSrc[0][xBp1] = duplicate;
    mDst[0][xBp1] = duplicate;
    if (mMask)
    {
        mMask[0][xBp1] = false;
    }

    // vertex (0,ymax)
    duplicate = mSrc[mYBound][1];
    mSrc[yBp1][0] = duplicate;
    mDst[yBp1][0] = duplicate;
    if (mMask)
    {
        mMask[yBp1][0] = false;
    }

    // vertex (xmax,ymax)
    duplicate = mSrc[mYBound][mXBound];
    mSrc[yBp1][xBp1] = duplicate;
    mDst[yBp1][xBp1] = duplicate;
    if (mMask)
    {
        mMask[yBp1][xBp1] = false;
    }

    // edges (x,0) and (x,ymax)
    for (x = 1; x <= mXBound; ++x)
    {
        duplicate = mSrc[1][x];
        mSrc[0][x] = duplicate;
        mDst[0][x] = duplicate;
        if (mMask)
        {
            mMask[0][x] = false;
        }

        duplicate = mSrc[mYBound][x];
        mSrc[yBp1][x] = duplicate;
        mDst[yBp1][x] = duplicate;
        if (mMask)
        {
            mMask[yBp1][x] = false;
        }
    }

    // edges (0,y) and (xmax,y)
    for (y = 1; y <= mYBound; ++y)
    {
        duplicate = mSrc[y][1];
        mSrc[y][0] = duplicate;
        mDst[y][0] = duplicate;
        if (mMask)
        {
            mMask[y][0] = false;
        }

        duplicate = mSrc[y][mXBound];
        mSrc[y][xBp1] = duplicate;
        mDst[y][xBp1] = duplicate;
        if (mMask)
        {
            mMask[y][xBp1] = false;
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::AssignDirichletMaskBorder ()
{
    if (mMask)
    {
        for (int y = 1; y <= mYBound; ++y)
        {
            for (int x = 1; x <= mXBound; ++x)
            {
                if (mMask[y][x])
                {
                    continue;
                }

                bool found = false;
                int i0, i1, j0, j1;
                for (i1 = 0, j1 = y-1; i1 < 3 && !found; ++i1, ++j1)
                {
                    for (i0 = 0, j0 = x-1; i0 < 3; ++i0, ++j0)
                    {
                        if (mMask[j1][j0])
                        {
                            mSrc[y][x] = mBorderValue;
                            mDst[y][x] = mBorderValue;
                            found = true;
                            break;
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
void PdeFilter2::AssignNeumannMaskBorder ()
{
    // Recompute the values just outside the masked region.  This guarantees
    // that derivative estimations use the current values around the boundary.
    for (int y = 1; y <= mYBound; ++y)
    {
        for (int x = 1; x <= mXBound; ++x)
        {
            if (mMask[y][x])
            {
                continue;
            }

            int count = 0;
            float average = 0.0f;
            int i0, i1, j0, j1;
            for (i1 = 0, j1 = y-1; i1 < 3; ++i1, ++j1)
            {
                for (i0 = 0, j0 = x-1; i0 < 3; ++i0, ++j0)
                {
                    if (mMask[j1][j0])
                    {
                        average += mSrc[j1][j0];
                        count++;
                    }
                }
            }

            if (count > 0)
            {
                average /= (float)count;
                mSrc[y][x] = average;
                mDst[y][x] = average;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::OnPreUpdate ()
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
void PdeFilter2::OnUpdate ()
{
    for (int y = 1; y <= mYBound; ++y)
    {
        for (int x = 1; x <= mXBound; ++x)
        {
            if (!mMask || mMask[y][x])
            {
                OnUpdate(x,y);
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::OnPostUpdate ()
{
    // Swap the buffers for the next pass.
    float** save = mSrc;
    mSrc = mDst;
    mDst = save;
}
//----------------------------------------------------------------------------
void PdeFilter2::LookUp5 (int x, int y)
{
    int xm = x - 1, xp = x + 1;
    int ym = y - 1, yp = y + 1;
    mUzm = mSrc[ym][x ];
    mUmz = mSrc[y ][xm];
    mUzz = mSrc[y ][x ];
    mUpz = mSrc[y ][xp];
    mUzp = mSrc[yp][x ];
}
//----------------------------------------------------------------------------
void PdeFilter2::LookUp9 (int x, int y)
{
    int xm = x - 1, xp = x + 1;
    int ym = y - 1, yp = y + 1;
    mUmm = mSrc[ym][xm];
    mUzm = mSrc[ym][x ];
    mUpm = mSrc[ym][xp];
    mUmz = mSrc[y ][xm];
    mUzz = mSrc[y ][x ];
    mUpz = mSrc[y ][xp];
    mUmp = mSrc[yp][xm];
    mUzp = mSrc[yp][x ];
    mUpp = mSrc[yp][xp];
}
//----------------------------------------------------------------------------
