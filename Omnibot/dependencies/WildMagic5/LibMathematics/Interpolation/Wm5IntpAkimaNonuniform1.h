// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5INTPAKIMANONUNIFORM1_H
#define WM5INTPAKIMANONUNIFORM1_H

#include "Wm5MathematicsLIB.h"
#include "Wm5IntpAkima1.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpAkimaNonuniform1 : public IntpAkima1<Real>
{
public:
    // Construction and destruction.  IntpAkimaNonuniform1 does not
    // accept responsibility for deleting the input arrays.  The application
    // must do so.  The interpolator is for arbitrarily spaced x-values.
    IntpAkimaNonuniform1 (int quantity, Real* X, Real* F);
    virtual ~IntpAkimaNonuniform1 ();

    const Real* GetX () const;
    virtual Real GetXMin () const;
    virtual Real GetXMax () const;

protected:
    using IntpAkima1<Real>::mQuantity;
    using IntpAkima1<Real>::mPoly;
    using IntpAkima1<Real>::ComputeDerivative;

    virtual bool Lookup (Real x, int& index, Real& dx) const;

    Real* mX;
};

typedef IntpAkimaNonuniform1<float> IntpAkimaNonuniform1f;
typedef IntpAkimaNonuniform1<double> IntpAkimaNonuniform1d;

}

#endif
