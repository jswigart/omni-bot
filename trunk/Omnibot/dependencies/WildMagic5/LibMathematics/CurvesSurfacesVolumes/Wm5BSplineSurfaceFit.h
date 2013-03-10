// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINESURFACEFIT_H
#define WM5BSPLINESURFACEFIT_H

#include "Wm5MathematicsLIB.h"
#include "Wm5BSplineFitBasis.h"
#include "Wm5BandedMatrix.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BSplineSurfaceFit
{
public:
    // Construction and destruction.  The preconditions for calling the
    // constructor are
    //   1 <= degree0 && degree0 < numControls0 <= numSamples0
    //   1 <= degree1 && degree1 < numControls1 <= numSamples1
    BSplineSurfaceFit (int degree0, int numControls0, int numSamples0,
        int degree1, int numControls1, int numSamples1,
        Vector3<Real>** samples);

    ~BSplineSurfaceFit ();

    // Access to input sample information.
    int GetSampleQuantity (int i) const;
    Vector3<Real>** GetSamplePoints () const;

    // Access to output control point and surface information.
    int GetDegree (int i) const;
    int GetControlQuantity (int i) const;
    Vector3<Real>** GetControlPoints () const;
    const BSplineFitBasis<Real>& GetBasis (int i) const;

    // Evaluation of the B-spline surface.  It is defined for 0 <= u <= 1
    // and 0 <= v <= 1.  If a parameter value is outside [0,1], it is clamped
    // to [0,1].
    Vector3<Real> GetPosition (Real u, Real v) const;

private:
    // Input sample information.
    int mNumSamples[2];
    Vector3<Real>** mSamples;

    // The fitted B-spline surface, open and with uniform knots.
    int mDegree[2];
    int mNumControls[2];
    Vector3<Real>** mControls;
    BSplineFitBasis<Real>* mBasis[2];
};

typedef BSplineSurfaceFit<float> BSplineSurfaceFitf;
typedef BSplineSurfaceFit<double> BSplineSurfaceFitd;

}

#endif
