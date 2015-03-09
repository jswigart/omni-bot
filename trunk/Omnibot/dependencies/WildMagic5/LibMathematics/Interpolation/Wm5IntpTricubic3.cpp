// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpTricubic3.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpTricubic3<Real>::IntpTricubic3 (int xBound, int yBound, int zBound,
    Real xMin, Real xSpacing, Real yMin, Real ySpacing, Real zMin,
    Real zSpacing, Real*** F, bool catmullRom)
{
    // At least a 4x4x4 block of data points are needed to construct the
    // tricubic interpolation.
    assertion(xBound >= 4 && yBound >= 4 && zBound >= 4 && F,
        "Invalid input\n");
    assertion(xSpacing > (Real)0 && ySpacing > (Real)0 && zSpacing > (Real)0,
        "Invalid input\n");

    mXBound = xBound;
    mYBound = yBound;
    mZBound = zBound;
    mQuantity = xBound*yBound*zBound;

    mXMin = xMin;
    mXSpacing = xSpacing;
    mInvXSpacing = ((Real)1)/xSpacing;
    mXMax = xMin + xSpacing*(xBound - 1);

    mYMin = yMin;
    mYSpacing = ySpacing;
    mInvYSpacing = ((Real)1)/ySpacing;
    mYMax = yMin + ySpacing*(yBound - 1);

    mZMin = zMin;
    mZSpacing = zSpacing;
    mInvZSpacing = ((Real)1)/zSpacing;
    mZMax = yMin + zSpacing*(zBound - 1);

    mF = F;

    mBlend = (catmullRom ? msCRBlend : msBSBlend);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpTricubic3<Real>::GetXBound () const
{
    return mXBound;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpTricubic3<Real>::GetYBound () const
{
    return mYBound;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpTricubic3<Real>::GetZBound () const
{
    return mZBound;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpTricubic3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
Real*** IntpTricubic3<Real>::GetF () const
{
    return mF;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetXMin () const
{
    return mXMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetXMax () const
{
    return mXMax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetXSpacing () const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetYMin () const
{
    return mYMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetYMax () const
{
    return mYMax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetYSpacing () const
{
    return mYSpacing;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetZMin () const
{
    return mZMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetZMax () const
{
    return mZMax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::GetZSpacing () const
{
    return mZSpacing;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::operator() (Real x, Real y, Real z) const
{
    // Compute x-index and clamp to image.
    Real xIndex = (x - mXMin)*mInvXSpacing;
    int ix = (int)xIndex;
    if (ix < 0 || ix > mXBound - 1)
    {
        return Math<Real>::MAX_REAL;
    }

    // Compute y-index and clamp to image.
    Real yIndex = (y - mYMin)*mInvYSpacing;
    int iy = (int)yIndex;
    if (iy < 0 || iy > mYBound - 1)
    {
        return Math<Real>::MAX_REAL;
    }

    // Compute z-index and clamp to image.
    Real zIndex = (z - mZMin)*mInvZSpacing;
    int iz = (int)zIndex;
    if (iz < 0 || iz > mZBound - 1)
    {
        return Math<Real>::MAX_REAL;
    }

    Real U[4];
    U[0] = (Real)1;
    U[1] = xIndex - ix;
    U[2] = U[1]*U[1];
    U[3] = U[1]*U[2];

    Real V[4];
    V[0] = (Real)1;
    V[1] = yIndex - iy;
    V[2] = V[1]*V[1];
    V[3] = V[1]*V[2];

    Real W[4];
    W[0] = (Real)1;
    W[1] = zIndex - iz;
    W[2] = W[1]*W[1];
    W[3] = W[1]*W[2];

    // Compute P = M*U, Q = M*V, R = M*W.
    Real P[4], Q[4], R[4];
    int row, col;
    for (row = 0; row < 4; ++row)
    {
        P[row] = (Real)0;
        Q[row] = (Real)0;
        R[row] = (Real)0;
        for (col = 0; col < 4; ++col)
        {
            P[row] += mBlend[row][col]*U[col];
            Q[row] += mBlend[row][col]*V[col];
            R[row] += mBlend[row][col]*W[col];
        }
    }

    // Compute the tensor product (M*U)(M*V)(M*W)*D where D is the 4x4x4
    // subimage containing (x,y,z).
    --ix;
    --iy;
    --iz;
    Real result = (Real)0;
    for (int slice = 0; slice < 4; ++slice)
    {
        int zClamp = iz + slice;
        if (zClamp < 0)
        {
            zClamp = 0;
        }
        else if (zClamp > mZBound - 1)
        {
            zClamp = mZBound - 1;
        }

        for (row = 0; row < 4; ++row)
        {
            int yClamp = iy + row;
            if (yClamp < 0)
            {
                yClamp = 0;
            }
            else if (yClamp > mYBound - 1)
            {
                yClamp = mYBound - 1;
            }

            for (col = 0; col < 4; ++col)
            {
                int xClamp = ix + col;
                if (xClamp < 0)
                {
                    xClamp = 0;
                }
                else if (xClamp > mXBound - 1)
                {
                    xClamp = mXBound - 1;
                }

                result += P[col]*Q[row]*R[slice]*mF[zClamp][yClamp][xClamp];
            }
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpTricubic3<Real>::operator() (int xOrder, int yOrder, int zOrder,
    Real x, Real y, Real z) const
{
    // Compute x-index and clamp to image.
    Real xIndex = (x - mXMin)*mInvXSpacing;
    int ix = (int)xIndex;
    if (ix < 0 || ix > mXBound - 1)
    {
        return Math<Real>::MAX_REAL;
    }

    // Compute y-index and clamp to image.
    Real yIndex = (y - mYMin)*mInvYSpacing;
    int iy = (int)yIndex;
    if (iy < 0 || iy > mYBound - 1)
    {
        return Math<Real>::MAX_REAL;
    }

    // Compute z-index and clamp to image.
    Real zIndex = (z - mZMin)*mInvZSpacing;
    int iz = (int)zIndex;
    if (iz < 0 || iz > mZBound - 1)
    {
        return Math<Real>::MAX_REAL;
    }

    Real U[4], dx, xMult;
    switch (xOrder)
    {
    case 0:
        dx = xIndex - ix;
        U[0] = (Real)1;
        U[1] = dx;
        U[2] = dx*U[1];
        U[3] = dx*U[2];
        xMult = (Real)1;
        break;
    case 1:
        dx = xIndex - ix;
        U[0] = (Real)0;
        U[1] = (Real)1;
        U[2] = ((Real)2)*dx;
        U[3] = ((Real)3)*dx*dx;
        xMult = mInvXSpacing;
        break;
    case 2:
        dx = xIndex - ix;
        U[0] = (Real)0;
        U[1] = (Real)0;
        U[2] = (Real)2;
        U[3] = ((Real)6)*dx;
        xMult = mInvXSpacing*mInvXSpacing;
        break;
    case 3:
        U[0] = (Real)0;
        U[1] = (Real)0;
        U[2] = (Real)0;
        U[3] = (Real)6;
        xMult = mInvXSpacing*mInvXSpacing*mInvXSpacing;
        break;
    default:
        return (Real)0;
    }

    Real V[4], dy, yMult;
    switch (yOrder)
    {
    case 0:
        dy = yIndex - iy;
        V[0] = (Real)1;
        V[1] = dy;
        V[2] = dy*V[1];
        V[3] = dy*V[2];
        yMult = (Real)1;
        break;
    case 1:
        dy = yIndex - iy;
        V[0] = (Real)0;
        V[1] = (Real)1;
        V[2] = ((Real)2)*dy;
        V[3] = ((Real)3)*dy*dy;
        yMult = mInvYSpacing;
        break;
    case 2:
        dy = yIndex - iy;
        V[0] = (Real)0;
        V[1] = (Real)0;
        V[2] = (Real)2;
        V[3] = ((Real)6)*dy;
        yMult = mInvYSpacing*mInvYSpacing;
        break;
    case 3:
        V[0] = (Real)0;
        V[1] = (Real)0;
        V[2] = (Real)0;
        V[3] = (Real)6;
        yMult = mInvYSpacing*mInvYSpacing*mInvYSpacing;
        break;
    default:
        return (Real)0;
    }

    Real W[4], dz, zMult;
    switch (zOrder)
    {
    case 0:
        dz = zIndex - iz;
        W[0] = (Real)1;
        W[1] = dz;
        W[2] = dz*W[1];
        W[3] = dz*W[2];
        zMult = (Real)1;
        break;
    case 1:
        dz = zIndex - iz;
        W[0] = (Real)0;
        W[1] = (Real)1;
        W[2] = ((Real)2)*dz;
        W[3] = ((Real)3)*dz*dz;
        zMult = mInvZSpacing;
        break;
    case 2:
        dz = zIndex - iz;
        W[0] = (Real)0;
        W[1] = (Real)0;
        W[2] = (Real)2;
        W[3] = ((Real)6)*dz;
        zMult = mInvZSpacing*mInvZSpacing;
        break;
    case 3:
        W[0] = (Real)0;
        W[1] = (Real)0;
        W[2] = (Real)0;
        W[3] = (Real)6;
        zMult = mInvZSpacing*mInvZSpacing*mInvZSpacing;
        break;
    default:
        return (Real)0;
    }

    // Compute P = M*U, Q = M*V, and R = M*W.
    Real P[4], Q[4], R[4];
    int row, col;
    for (row = 0; row < 4; ++row)
    {
        P[row] = (Real)0;
        Q[row] = (Real)0;
        R[row] = (Real)0;
        for (col = 0; col < 4; ++col)
        {
            P[row] += mBlend[row][col]*U[col];
            Q[row] += mBlend[row][col]*V[col];
            R[row] += mBlend[row][col]*W[col];
        }
    }

    // Compute the tensor product (M*U)(M*V)(M*W)*D where D is the 4x4x4
    // subimage containing (x,y,z).
    --ix;
    --iy;
    --iz;
    Real result = (Real)0;
    for (int slice = 0; slice < 4; ++slice)
    {
        int zClamp = iz + slice;
        if (zClamp < 0)
        {
            zClamp = 0;
        }
        else if (zClamp > mZBound - 1)
        {
            zClamp = mZBound - 1;
        }

        for (row = 0; row < 4; ++row)
        {
            int yClamp = iy + row;
            if (yClamp < 0)
            {
                yClamp = 0;
            }
            else if (yClamp > mYBound - 1)
            {
                yClamp = mYBound - 1;
            }

            for (col = 0; col < 4; ++col)
            {
                int xClamp = ix + col;
                if (xClamp < 0)
                {
                    xClamp = 0;
                }
                else if (xClamp > mXBound - 1)
                {
                    xClamp = mXBound - 1;
                }

                result += P[col]*Q[row]*R[slice]*mF[zClamp][yClamp][xClamp];
            }
        }
    }
    result *= xMult*yMult*zMult;

    return result;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template<>
const float IntpTricubic3<float>::msCRBlend[4][4] =
{
    { 0.0f, -0.5f,  1.0f, -0.5f },
    { 1.0f,  0.0f, -2.5f,  1.5f },
    { 0.0f,  0.5f,  2.0f, -1.5f },
    { 0.0f,  0.0f, -0.5f,  0.5f }
};

template<>
const float IntpTricubic3<float>::msBSBlend[4][4] =
{
    { 1.0f/6.0f, -3.0f/6.0f,  3.0f/6.0f, -1.0f/6.0f },
    { 4.0f/6.0f,  0.0f/6.0f, -6.0f/6.0f,  3.0f/6.0f },
    { 1.0f/6.0f,  3.0f/6.0f,  3.0f/6.0f, -3.0f/6.0f },
    { 0.0f/6.0f,  0.0f/6.0f,  0.0f/6.0f,  1.0f/6.0f }
};

template WM5_MATHEMATICS_ITEM
class IntpTricubic3<float>;

template<>
const double IntpTricubic3<double>::msCRBlend[4][4] =
{
    { 0.0, -0.5,  1.0, -0.5 },
    { 1.0,  0.0, -2.5,  1.5 },
    { 0.0,  0.5,  2.0, -1.5 },
    { 0.0,  0.0, -0.5,  0.5 }
};

template<>
const double IntpTricubic3<double>::msBSBlend[4][4] =
{
    { 1.0/6.0, -3.0/6.0,  3.0/6.0, -1.0/6.0 },
    { 4.0/6.0,  0.0/6.0, -6.0/6.0,  3.0/6.0 },
    { 1.0/6.0,  3.0/6.0,  3.0/6.0, -3.0/6.0 },
    { 0.0/6.0,  0.0/6.0,  0.0/6.0,  1.0/6.0 }
};

template WM5_MATHEMATICS_ITEM
class IntpTricubic3<double>;
//----------------------------------------------------------------------------
}
