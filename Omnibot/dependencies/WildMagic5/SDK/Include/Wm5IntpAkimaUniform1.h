// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#ifndef WM5INTPAKIMAUNIFORM1_H
#define WM5INTPAKIMAUNIFORM1_H

#include "Wm5MathematicsLIB.h"
#include "Wm5IntpAkima1.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpAkimaUniform1 : public IntpAkima1<Real>
{
public:
    // Construction and destruction.  IntpAkimaUniform1 accepts
    // responsibility for deleting the input array.  The interpolator is for
    // uniformly spaced x-values.
    IntpAkimaUniform1 (int quantity, Real xMin, Real xSpacing, Real* F);
    virtual ~IntpAkimaUniform1 ();

    virtual Real GetXMin () const;
    virtual Real GetXMax () const;
    Real GetXSpacing () const;

protected:
    using IntpAkima1<Real>::mQuantity;
    using IntpAkima1<Real>::mPoly;
    using IntpAkima1<Real>::ComputeDerivative;

    virtual bool Lookup (Real x, int& index, Real& dx) const;

    Real mXMin, mXMax, mXSpacing;
};

typedef IntpAkimaUniform1<float> IntpAkimaUniform1f;
typedef IntpAkimaUniform1<double> IntpAkimaUniform1d;

}

#endif
