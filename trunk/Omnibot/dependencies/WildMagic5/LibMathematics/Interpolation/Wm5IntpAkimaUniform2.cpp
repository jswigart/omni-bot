// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpAkimaUniform2.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpAkimaUniform2<Real>::IntpAkimaUniform2 (int xBound, int yBound, Real xMin,
    Real xSpacing, Real yMin, Real ySpacing, Real** F)
{
    // At least a 3x3 block of data points are needed to construct the
    // estimates of the boundary derivatives.
    assertion(xBound >= 3 && yBound >= 3 && F, "Invalid input\n");
    assertion(xSpacing > (Real)0 && ySpacing > (Real)0, "Invalid input\n");

    mXBound = xBound;
    mYBound = yBound;
    mXMin = xMin;
    mXSpacing = xSpacing;
    mYMin = yMin;
    mYSpacing = ySpacing;
    mF = F;

    int xBoundM1 = xBound - 1, yBoundM1 = yBound - 1;
    mQuantity = xBound*yBound;
    mXMax = xMin + xSpacing*xBoundM1;
    mYMax = yMin + ySpacing*yBoundM1;

    // Compute slopes.
    Real invDX = ((Real)1)/xSpacing;
    Real invDY = ((Real)1)/ySpacing;
    Real invDXDY = invDX*invDY;
    Real** xSlope = new2<Real>(xBound + 3, yBound);  //xslope[y][x]
    Real** ySlope = new2<Real>(yBound + 3, xBound);  //yslope[x][y]

    int ix, iy;
    for (iy = 0; iy < yBound; ++iy)
    {
        for (ix = 0; ix < xBoundM1; ++ix)
        {
            xSlope[iy][ix+2] = (F[iy][ix+1] - F[iy][ix])*invDX;
        }

        xSlope[iy][1] = ((Real)2)*xSlope[iy][2] - xSlope[iy][3];
        xSlope[iy][0] = ((Real)2)*xSlope[iy][1] - xSlope[iy][2];
        xSlope[iy][xBound+1] = ((Real)2)*xSlope[iy][xBound] -
            xSlope[iy][xBound-1];
        xSlope[iy][xBound+2] = ((Real)2)*xSlope[iy][xBound+1] -
            xSlope[iy][xBound];
    }

    for (ix = 0; ix < xBound; ++ix)
    {
        for (iy = 0; iy < yBoundM1; ++iy)
        {
            ySlope[ix][iy+2] = (F[iy+1][ix] - F[iy][ix])*invDY;
        }

        ySlope[ix][1] = ((Real)2)*ySlope[ix][2] - ySlope[ix][3];
        ySlope[ix][0] = ((Real)2)*ySlope[ix][1] - ySlope[ix][2];
        ySlope[ix][yBound+1] = ((Real)2)*ySlope[ix][yBound] -
            ySlope[ix][yBound-1];
        ySlope[ix][yBound+2] = ((Real)2)*ySlope[ix][yBound+1] -
            ySlope[ix][yBound];
    }

    // Construct first-order derivatives.
    Real** FX = new2<Real>(xBound, yBound);
    Real** FY = new2<Real>(xBound, yBound);

    for (iy = 0; iy < yBound; ++iy)
    {
        for (ix = 0; ix < xBound; ++ix)
        {
            FX[iy][ix] = ComputeDerivative(xSlope[iy] + ix);
        }
    }

    for (ix = 0; ix < xBound; ++ix)
    {
        for (iy = 0; iy < yBound; ++iy)
        {
            FY[iy][ix] = ComputeDerivative(ySlope[ix] + iy);
        }
    }

    // Construct second-order derivatives.
    Real** FXY = new2<Real>(xBound, yBound);

    unsigned int ix0 = xBoundM1, ix1 = ix0-1,  ix2 = ix1-1;
    unsigned int iy0 = yBoundM1, iy1 = iy0-1,  iy2 = iy1-1;

    // corners
    FXY[0][0] = ((Real)0.25)*invDXDY*(
         ((Real) 9)*F[0][0]
        -((Real)12)*F[0][1]
        +((Real) 3)*F[0][2]
        -((Real)12)*F[1][0]
        +((Real)16)*F[1][1]
        -((Real) 4)*F[1][2]
        +((Real) 3)*F[2][0]
        -((Real) 4)*F[2][1]
        +           F[2][2]);

    FXY[0][xBoundM1] = ((Real)0.25)*invDXDY*(
         ((Real) 9)*F[0][ix0]
        -((Real)12)*F[0][ix1]
        +((Real) 3)*F[0][ix2]
        -((Real)12)*F[1][ix0]
        +((Real)16)*F[1][ix1]
        -((Real) 4)*F[1][ix2]
        +((Real) 3)*F[2][ix0]
        -((Real) 4)*F[2][ix1]
        +           F[2][ix2]);

    FXY[yBoundM1][0] = ((Real)0.25)*invDXDY*(
         ((Real) 9)*F[iy0][0]
        -((Real)12)*F[iy0][1]
        +((Real) 3)*F[iy0][2]
        -((Real)12)*F[iy1][0]
        +((Real)16)*F[iy1][1]
        -((Real) 4)*F[iy1][2]
        +((Real) 3)*F[iy2][0]
        -((Real) 4)*F[iy2][1]
        +           F[iy2][2]);

    FXY[yBoundM1][xBoundM1] = ((Real)0.25)*invDXDY*(
         ((Real) 9)*F[iy0][ix0]
        -((Real)12)*F[iy0][ix1]
        +((Real) 3)*F[iy0][ix2]
        -((Real)12)*F[iy1][ix0]
        +((Real)16)*F[iy1][ix1]
        -((Real) 4)*F[iy1][ix2]
        +((Real) 3)*F[iy2][ix0]
        -((Real) 4)*F[iy2][ix1]
        +           F[iy2][ix2]);

    // x-edges
    for (ix = 1; ix < xBoundM1; ++ix)
    {
        FXY[0][ix] = ((Real)0.25)*invDXDY*(
            ((Real)3)*(F[0][ix-1] - F[0][ix+1])
           -((Real)4)*(F[1][ix-1] - F[1][ix+1])
           +          (F[2][ix-1] - F[2][ix+1]));

        FXY[yBoundM1][ix] = ((Real)0.25)*invDXDY*(
            ((Real)3)*(F[iy0][ix-1] - F[iy0][ix+1])
           -((Real)4)*(F[iy1][ix-1] - F[iy1][ix+1])
           +          (F[iy2][ix-1] - F[iy2][ix+1]));
    }

    // y-edges
    for (iy = 1; iy < yBoundM1; ++iy)
    {
        FXY[iy][0] = ((Real)0.25)*invDXDY*(
            ((Real)3)*(F[iy-1][0] - F[iy+1][0])
           -((Real)4)*(F[iy-1][1] - F[iy+1][1])
           +          (F[iy-1][2] - F[iy+1][2]));

        FXY[iy][xBoundM1] = ((Real)0.25)*invDXDY*(
            ((Real)3)*(F[iy-1][ix0] - F[iy+1][ix0])
           -((Real)4)*(F[iy-1][ix1] - F[iy+1][ix1])
           +          (F[iy-1][ix2] - F[iy+1][ix2]));
    }

    // interior
    for (iy = 1; iy < yBoundM1; ++iy)
    {
        for (ix = 1; ix < xBoundM1; ++ix)
        {
            FXY[iy][ix] = ((Real)0.25)*invDXDY*(F[iy-1][ix-1] -
                F[iy-1][ix+1] - F[iy+1][ix-1] + F[iy+1][ix+1]);
        }
    }

    // Construct polynomials.
    mPoly = new2<Polynomial>(xBoundM1, yBoundM1);
    for (iy = 0; iy < yBoundM1; ++iy)
    {
        for (ix = 0; ix < xBoundM1; ++ix)
        {
            // Note the 'transposing' of the 2x2 blocks (to match notation
            // used in the polynomial definition).
            Real G[2][2] =
            {
                {F[iy][ix], F[iy+1][ix]},
                {F[iy][ix+1], F[iy+1][ix+1]}
            };

            Real GX[2][2] =
            {
                {FX[iy][ix], FX[iy+1][ix]},
                {FX[iy][ix+1], FX[iy+1][ix+1]}
            };

            Real GY[2][2] =
            {
                {FY[iy][ix], FY[iy+1][ix]},
                {FY[iy][ix+1], FY[iy+1][ix+1]}
            };

            Real GXY[2][2] =
            {
                {FXY[iy][ix], FXY[iy+1][ix]},
                {FXY[iy][ix+1], FXY[iy+1][ix+1]}
            };

            Construct(mPoly[iy][ix], G, GX, GY, GXY);
        }
    }

    delete2(xSlope);
    delete2(ySlope);
    delete2(FX);
    delete2(FY);
    delete2(FXY);
}
//----------------------------------------------------------------------------
template <typename Real>
IntpAkimaUniform2<Real>::~IntpAkimaUniform2 ()
{
    delete2(mPoly);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpAkimaUniform2<Real>::GetXBound () const
{
    return mXBound;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpAkimaUniform2<Real>::GetYBound () const
{
    return mYBound;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpAkimaUniform2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
Real** IntpAkimaUniform2<Real>::GetF () const
{
    return mF;
}
//----------------------------------------------------------------------------
template <typename Real>
typename IntpAkimaUniform2<Real>::Polynomial**
IntpAkimaUniform2<Real>::GetPolynomials () const
{
    return mPoly;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename IntpAkimaUniform2<Real>::Polynomial&
IntpAkimaUniform2<Real>::GetPolynomial (int ix, int iy) const
{
    return mPoly[iy][ix];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::GetXMin () const
{
    return mXMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::GetXMax () const
{
    return mXMax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::GetXSpacing () const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::GetYMin () const
{
    return mYMin;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::GetYMax () const
{
    return mYMax;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::GetYSpacing () const
{
    return mYSpacing;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::ComputeDerivative (Real* slope) const
{
    if (slope[1] != slope[2])
    {
        if (slope[0] != slope[1])
        {
            if (slope[2] != slope[3])
            {
                Real ad0 = Math<Real>::FAbs(slope[3] - slope[2]);
                Real ad1 = Math<Real>::FAbs(slope[0] - slope[1]);
                return (ad0*slope[1] +ad1*slope[2])/(ad0+ad1);
            } 
            else
            {
                return slope[2];
            }
        }
        else
        {
            if (slope[2] != slope[3])
            {
                return slope[1];
            }
            else
            {
                return ((Real)0.5)*(slope[1] +slope[2]);
            } 
        }
    }
    else
    {
        return slope[1];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpAkimaUniform2<Real>::Construct (Polynomial& poly, Real F[2][2],
    Real FX[2][2], Real FY[2][2], Real FXY[2][2])
{
    Real dx = mXSpacing;
    Real dy = mYSpacing;
    Real invDX = ((Real)1)/dx, fInvDX2 = invDX*invDX;
    Real invDY = ((Real)1)/dy, fInvDY2 = invDY*invDY;
    Real b0, b1, b2, b3;

    poly.A(0,0) = F[0][0];
    poly.A(1,0) = FX[0][0];
    poly.A(0,1) = FY[0][0];
    poly.A(1,1) = FXY[0][0];

    b0 = (F[1][0] - poly(0, 0, dx, (Real)0))*fInvDX2;
    b1 = (FX[1][0] - poly(1, 0, dx, (Real)0))*invDX;
    poly.A(2,0) = ((Real)3)*b0 - b1;
    poly.A(3,0) = (-((Real)2)*b0 + b1)*invDX;

    b0 = (F[0][1] - poly(0, 0, (Real)0, dy))*fInvDY2;
    b1 = (FY[0][1] - poly(0, 1, (Real)0, dy))*invDY;
    poly.A(0,2) = ((Real)3)*b0 - b1;
    poly.A(0,3) = (-((Real)2)*b0 + b1)*invDY;

    b0 = (FY[1][0] - poly(0, 1, dx, (Real)0))*fInvDX2;
    b1 = (FXY[1][0] - poly(1, 1, dx, (Real)0))*invDX;
    poly.A(2,1) = ((Real)3)*b0 - b1;
    poly.A(3,1) = (-((Real)2)*b0 + b1)*invDX;

    b0 = (FX[0][1] - poly(1, 0, (Real)0, dy))*fInvDY2;
    b1 = (FXY[0][1] - poly(1, 1, (Real)0, dy))*invDY;
    poly.A(1,2) = ((Real)3)*b0 - b1;
    poly.A(1,3) = (-((Real)2)*b0 + b1)*invDY;

    b0 = (F[1][1] - poly(0, 0, dx, dy))*fInvDX2*fInvDY2;
    b1 = (FX[1][1] - poly(1, 0, dx, dy))*invDX*fInvDY2;
    b2 = (FY[1][1] - poly(0, 1, dx, dy))*fInvDX2*invDY;
    b3 = (FXY[1][1] - poly(1, 1, dx, dy))*invDX*invDY;
    poly.A(2,2) = ((Real)9)*b0 - ((Real)3)*b1 - ((Real)3)*b2 + b3;
    poly.A(3,2) = (-((Real)6)*b0 + ((Real)3)*b1 + ((Real)2)*b2 - b3)*invDX;
    poly.A(2,3) = (-((Real)6)*b0 + ((Real)2)*b1 + ((Real)3)*b2 - b3)*invDY;
    poly.A(3,3) = (((Real)4)*b0 - ((Real)2)*b1 - ((Real)2)*b2 + b3)*
        invDX*invDY;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpAkimaUniform2<Real>::XLookup (Real x, int& xIndex, Real& dx) const
{
    if (x >= mXMin)
    {
        if (x <= mXMax)
        {
            for (xIndex = 0; xIndex + 1 < mXBound; ++xIndex)
            {
                if (x < mXMin + mXSpacing*(xIndex + 1))
                {
                    dx = x - (mXMin + mXSpacing*xIndex);
                    return true;
                }
            }

            --xIndex;
            dx = x - (mXMin + mXSpacing*xIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpAkimaUniform2<Real>::YLookup (Real y, int& yIndex, Real& dy) const
{
    if (y >= mYMin)
    {
        if (y <= mYMax)
        {
            for (yIndex = 0; yIndex + 1 < mYBound; ++yIndex)
            {
                if (y < mYMin + mYSpacing*(yIndex + 1))
                {
                    dy = y - (mYMin + mYSpacing*yIndex);
                    return true;
                }
            }

            yIndex--;
            dy = y - (mYMin + mYSpacing*yIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::operator() (Real x, Real y) const
{
    int ix, iy;
    Real dx, dy;

    if (XLookup(x, ix, dx) && YLookup(y, iy, dy))
    {
        return mPoly[iy][ix](dx, dy);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::operator() (int xOrder, int yOrder, Real x,
    Real y) const
{
    int ix, iy;
    Real dx, dy;

    if (XLookup(x, ix, dx) && YLookup(y, iy, dy))
    {
        return mPoly[iy][ix](xOrder, yOrder, dx, dy);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IntpAkimaUniform2::Polynomial
//----------------------------------------------------------------------------
template <typename Real>
IntpAkimaUniform2<Real>::Polynomial::Polynomial ()
{
    memset(&mCoeff[0][0], 0, 16*sizeof(Real));
}
//----------------------------------------------------------------------------
template <typename Real>
Real& IntpAkimaUniform2<Real>::Polynomial::A (int ix, int iy)
{
    return mCoeff[ix][iy];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::Polynomial::operator() (Real x, Real y) const
{
    Real B[4];
    for (int i = 0; i < 4; ++i)
    {
        B[i] = mCoeff[i][0] + y*(mCoeff[i][1] + y*(mCoeff[i][2] +
            y*mCoeff[i][3]));
    }

    return B[0] + x*(B[1] + x*(B[2] + x*B[3]));
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpAkimaUniform2<Real>::Polynomial::operator() (int xOrder, int yOrder,
    Real x, Real y) const
{
    Real xPow[4];
    switch (xOrder)
    {
    case 0:
        xPow[0] = (Real)1;
        xPow[1] = x;
        xPow[2] = x*x;
        xPow[3] = x*x*x;
        break;
    case 1:
        xPow[0] = (Real)0;
        xPow[1] = (Real)1;
        xPow[2] = ((Real)2)*x;
        xPow[3] = ((Real)3)*x*x;
        break;
    case 2:
        xPow[0] = (Real)0;
        xPow[1] = (Real)0;
        xPow[2] = (Real)2;
        xPow[3] = ((Real)6)*x;
        break;
    case 3:
        xPow[0] = (Real)0;
        xPow[1] = (Real)0;
        xPow[2] = (Real)0;
        xPow[3] = (Real)6;
        break;
    default:
        return (Real)0;
    }

    Real yPow[4];
    switch (yOrder)
    {
    case 0:
        yPow[0] = (Real)1;
        yPow[1] = y;
        yPow[2] = y*y;
        yPow[3] = y*y*y;
        break;
    case 1:
        yPow[0] = (Real)0;
        yPow[1] = (Real)1;
        yPow[2] = ((Real)2)*y;
        yPow[3] = ((Real)3)*y*y;
        break;
    case 2:
        yPow[0] = (Real)0;
        yPow[1] = (Real)0;
        yPow[2] = (Real)2;
        yPow[3] = ((Real)6)*y;
        break;
    case 3:
        yPow[0] = (Real)0;
        yPow[1] = (Real)0;
        yPow[2] = (Real)0;
        yPow[3] = (Real)6;
        break;
    default:
        return (Real)0;
    }

    Real p = (Real)0;
    for (int iy = 0; iy <= 3; ++iy)
    {
        for (int ix = 0; ix <= 3; ++ix)
        {
            p += mCoeff[ix][iy]*xPow[ix]*yPow[iy];
        }
    }

    return p;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpAkimaUniform2<float>;

template WM5_MATHEMATICS_ITEM
class IntpAkimaUniform2<double>;
//----------------------------------------------------------------------------
}
