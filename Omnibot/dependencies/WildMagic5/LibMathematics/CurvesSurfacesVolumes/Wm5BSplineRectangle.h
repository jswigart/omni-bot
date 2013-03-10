// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINERECTANGLE_H
#define WM5BSPLINERECTANGLE_H

#include "Wm5MathematicsLIB.h"
#include "Wm5ParametricSurface.h"
#include "Wm5BSplineBasis.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BSplineRectangle : public ParametricSurface<Real>
{
public:
    // Construction and destruction.   The caller is responsible for deleting
    // the input arrays if they were dynamically allocated.  Internal copies
    // of the arrays are made, so to dynamically change control points or
    // knots you must use the 'SetControlPoint', 'GetControlPoint', and
    // 'Knot' member functions.

    // Spline types for curves are
    //   open uniform (OU)
    //   periodic uniform (PU)
    //   open nonuniform (ON)
    // For tensor product surfaces, you have to choose a type for each of two
    // dimensions, leading to nine possible spline types for surfaces.  The
    // constructors below represent these choices.

    // (OU,OU), (OU,PU), (PU,OU), or (PU,PU)
    BSplineRectangle (int numUCtrlPoints, int numVCtrlPoints,
        Vector3<Real>** ctrlPoint, int uDegree, int vDegree, bool uLoop,
        bool vLoop, bool uOpen, bool vOpen);

    // (OU,ON) or (PU,ON)
    BSplineRectangle (int numUCtrlPoints, int numVCtrlPoints,
        Vector3<Real>** ctrlPoint, int uDegree, int vDegree, bool uLoop,
        bool vLoop, bool uOpen, Real* vKnot);

    // (ON,OU) or (ON,PU)
    BSplineRectangle (int numUCtrlPoints, int numVCtrlPoints,
        Vector3<Real>** ctrlPoint, int uDegree, int vDegree, bool uLoop,
        bool vLoop, Real* uKnot, bool vOpen);

    // (ON,ON)
    BSplineRectangle (int numUCtrlPoints, int numVCtrlPoints,
        Vector3<Real>** ctrlPoint, int uDegree, int vDegree, bool uLoop,
        bool vLoop, Real* uKnot, Real* vKnot);

    virtual ~BSplineRectangle ();

    int GetNumCtrlPoints (int dim) const;
    int GetDegree (int dim) const;
    bool IsOpen (int dim) const;
    bool IsUniform (int dim) const;
    bool IsLoop (int dim) const;

    // Control points may be changed at any time.  If either input index is
    // invalid, GetControlPoint returns a vector whose components are all
    // MAX_REAL.
    void SetControlPoint (int uIndex, int vIndex, const Vector3<Real>& ctrl);
    Vector3<Real> GetControlPoint (int uIndex, int vIndex) const;

    // The knot values can be changed only if the surface is nonuniform in the
    // selected dimension and only if the input index is valid.  If these
    // conditions are not satisfied, GetKnot returns MAX_REAL.
    void SetKnot (int dim, int i, Real knot);
    Real GetKnot (int dim, int i) const;

    // The spline is defined for 0 <= u <= 1 and 0 <= v <= 1.  The input
    // values should be in this domain.  Any inputs smaller than 0 are clamped
    // to 0.  Any inputs larger than 1 are clamped to 1.
    virtual Vector3<Real> P (Real u, Real v) const;
    virtual Vector3<Real> PU (Real u, Real v) const;
    virtual Vector3<Real> PV (Real u, Real v) const;
    virtual Vector3<Real> PUU (Real u, Real v) const;
    virtual Vector3<Real> PUV (Real u, Real v) const;
    virtual Vector3<Real> PVV (Real u, Real v) const;

    // If you need position and derivatives at the same time, it is more
    // efficient to call these functions.  Pass the addresses of those
    // quantities whose values you want.  You may pass 0 in any argument
    // whose value you do not want.
    void Get (Real u, Real v, Vector3<Real>* pos, Vector3<Real>* derU,
        Vector3<Real>* derV, Vector3<Real>* derUU, Vector3<Real>* derUV,
        Vector3<Real>* derVV) const;

protected:
    // Replicate the necessary number of control points when the Create
    // function has bLoop equal to true, in which case the spline surface
    // must be a closed surface in the corresponding dimension.
    void CreateControl (Vector3<Real>** ctrlPoint);

    int mNumUCtrlPoints, mNumVCtrlPoints;
    Vector3<Real>** mCtrlPoint;  // ctrl[unum][vnum]
    bool mLoop[2];
    BSplineBasis<Real> mBasis[2];
    int mUReplicate, mVReplicate;
};

typedef BSplineRectangle<float> BSplineRectanglef;
typedef BSplineRectangle<double> BSplineRectangled;

}

#endif
