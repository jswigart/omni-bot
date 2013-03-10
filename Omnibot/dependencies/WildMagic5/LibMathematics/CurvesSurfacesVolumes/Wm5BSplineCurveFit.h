// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINECURVEFIT_H
#define WM5BSPLINECURVEFIT_H

#include "Wm5MathematicsLIB.h"
#include "Wm5BSplineFitBasis.h"
#include "Wm5BandedMatrix.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BSplineCurveFit
{
public:
    // Construction and destruction.  The preconditions for calling the
    // constructor are
    //   1 <= degree && degree < numControls <= numSamples
    // The samples point are contiguous blocks of dimension real value
    // stored in sampleData.
    BSplineCurveFit (int dimension, int numSamples,
        const Real* sampleData, int degree, int numControls);
    ~BSplineCurveFit ();

    // Access to input sample information.
    int GetDimension () const;
    int GetSampleQuantity () const;
    const Real* GetSampleData () const;

    // Access to output control point and curve information.
    int GetDegree () const;
    int GetControlQuantity () const;
    const Real* GetControlData () const;
    const BSplineFitBasis<Real>& GetBasis () const;

    // Evaluation of the B-spline curve.  It is defined for 0 <= t <= 1.  If
    // a t-value is outside [0,1], an open spline clamps it to [0,1].  The
    // caller must ensure that position[] has (at least) 'dimension'
    // elements.
    void GetPosition (Real t, Real* position) const;

private:
    // Input sample information.
    int mDimension;
    int mNumSamples;
    const Real* mSampleData;

    // The fitted B-spline curve, open and with uniform knots.
    int mDegree;
    int mNumControls;
    Real* mControlData;
    BSplineFitBasis<Real> mBasis;
};

typedef BSplineCurveFit<float> BSplineCurveFitf;
typedef BSplineCurveFit<double> BSplineCurveFitd;

}

#endif
