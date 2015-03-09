// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINEVOLUME_H
#define WM5BSPLINEVOLUME_H

#include "Wm5MathematicsLIB.h"
#include "Wm5BSplineBasis.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BSplineVolume
{
public:
    // Construction and destruction of an open uniform B-spline volume.  The
    // class will allocate space for the control points.  The caller is
    // responsible for setting the values with the member function
    // ControlPoint.

    BSplineVolume (int numUCtrlPoints, int numVCtrlPoints,
        int numWCtrlPoints, int uDegree, int vDegree, int wDegree);

    ~BSplineVolume ();

    int GetNumCtrlPoints (int dim) const;
    int GetDegree (int dim) const;

    // Control points may be changed at any time.  If any input index is
    // invalid, the returned point is a vector whose components are all
    // MAX_REAL.
    void SetControlPoint (int uIndex, int vIndex, int wIndex,
        const Vector3<Real>& ctrlPoint);
    Vector3<Real> GetControlPoint (int uIndex, int vIndex, int wIndex) const;

    // The spline is defined for 0 <= u <= 1, 0 <= v <= 1, and 0 <= w <= 1.
    // The input values should be in this domain.  Any inputs smaller than 0
    // are clamped to 0.  Any inputs larger than 1 are clamped to 1.
    Vector3<Real> GetPosition (Real u, Real v, Real w) const;
    Vector3<Real> GetDerivativeU (Real u, Real v, Real w) const;
    Vector3<Real> GetDerivativeV (Real u, Real v, Real w) const;
    Vector3<Real> GetDerivativeW (Real u, Real v, Real w) const;

    // for array indexing:  i = 0 for u, i = 1 for v, i = 2 for w
    Vector3<Real> GetPosition (Real pos[3]) const;
    Vector3<Real> GetDerivative (int i, Real pos[3]) const;

private:
    Vector3<Real>*** mCtrlPoint;  // ctrl[unum][vnum][wnum]
    BSplineBasis<Real> mBasis[3];
};

typedef BSplineVolume<float> BSplineVolumef;
typedef BSplineVolume<double> BSplineVolumed;

}

#endif
