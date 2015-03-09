// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPAKIMAUNIFORM3_H
#define WM5INTPAKIMAUNIFORM3_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpAkimaUniform3
{
public:
    // Construction and destruction.  Interp3DAkimaUniform does not accept
    // responsibility for deleting the input array.  The application must do
    // so.  The interpolator is for uniformly spaced (x,y,z)-values.  The
    // function values are assumed to be organized as f(x,y,z) = F[z][y][x].
    IntpAkimaUniform3 (int xBound, int yBound, int zBound, Real xMin,
        Real xSpacing, Real yMin, Real ySpacing, Real zMin, Real zSpacing,
        Real*** F);

    ~IntpAkimaUniform3 ();

    class WM5_MATHEMATICS_ITEM Polynomial
    {
    public:
        Polynomial ();

        // P(x,y,z) = sum_{i=0}^3 sum_{j=0}^3 sum_{k=0}^3 a_{ijk} x^i y^j z^k
        Real& A (int ix, int iy, int iz);

        Real operator() (Real x, Real y, Real z) const;
        Real operator() (int xOrder, int yOrder, int zOrder, Real x,
            Real y, Real z) const;

    protected:
        Real mCoeff[4][4][4];
    };

    int GetXBound () const;
    int GetYBound () const;
    int GetZBound () const;
    int GetQuantity () const;
    Real*** GetF () const;
    Polynomial*** GetPolynomials () const;
    const Polynomial& GetPolynomial (int ix, int iy, int iz) const;

    Real GetXMin () const;
    Real GetXMax () const;
    Real GetXSpacing () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetYSpacing () const;
    Real GetZMin () const;
    Real GetZMax () const;
    Real GetZSpacing () const;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax, ymin <= y <= ymax,
    // and zmin <= z <= zmax.  If (x,y,z) is outside the extremes, the
    // function returns MAXREAL.  The first operator is for function
    // evaluation.  The second operator is for function or derivative
    // evaluations.  The uiXOrder argument is the order of the x-derivative,
    // the uiYOrder argument is the order of the y-derivative, and the
    // uiZOrder argument is the order of the z-derivative.  All orders are
    // zero to get the function value itself.
    Real operator() (Real x, Real y, Real z) const;
    Real operator() (int xOrder, int yOrder, int zOrder, Real x,
        Real y, Real z) const;

private:
    Real ComputeDerivative (Real* slope) const;
    void Construct (Polynomial& poly, Real F[2][2][2], Real FX[2][2][2],
        Real FY[2][2][2],  Real FZ[2][2][2], Real FXY[2][2][2],
        Real FXZ[2][2][2], Real FYZ[2][2][2], Real FXYZ[2][2][2]);

    bool XLookup (Real x, int& xIndex, Real& dx) const;
    bool YLookup (Real y, int& yIndex, Real& dy) const;
    bool ZLookup (Real z, int& zIndex, Real& dz) const;

    int mXBound, mYBound, mZBound, mQuantity;
    Real*** mF;
    Polynomial*** mPoly;
    Real mXMin, mXMax, mXSpacing;
    Real mYMin, mYMax, mYSpacing;
    Real mZMin, mZMax, mZSpacing;

private:
    // These methods exist to split up the constructor into chunks of code
    // that compile quickly.

    Real*** GetFX ();
    Real*** GetFY ();
    Real*** GetFZ ();
    Real*** GetFXY ();
    Real*** GetFXZ ();
    Real*** GetFYZ ();
    Real*** GetFXYZ ();

    void GetPolynomials (Real*** FX, Real*** FY, Real*** FZ,
        Real*** FXY, Real*** FXZ, Real*** FYZ,
        Real*** FXYZ);
};

typedef IntpAkimaUniform3<float> IntpAkimaUniform3f;
typedef IntpAkimaUniform3<double> IntpAkimaUniform3d;

}

#endif
